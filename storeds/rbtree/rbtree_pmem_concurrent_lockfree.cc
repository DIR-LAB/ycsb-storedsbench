//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>
#include "../../lib/ex_common.h"
#include "rbtree_common.h"
#include "libatomic_ops/src/atomic_ops.h"
#include<bits/stdc++.h>


using namespace std;

enum rbtree_color {
    RED,
    BLACK
};
enum rbtree_alloc_types {
        RB_NODE_TYPE,
        RB_MAX_TYPES
    };

struct rbtree_pmem_node {
    uint64_t key;
    char value[DEFAULT_VALUE_LEN];
    PMEMoid left;
    PMEMoid right;
    PMEMoid parent;
    bool color;
    int* flag;
    char padding[7];
};

struct rbtree_pmem_root {
    PMEMoid root_node_oid;
};
#define PMEM_RB_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define RB_LAYOUT_NAME "rbtree_layout"

/* default length for value */
#define DEFAULT_VALUE_LEN 128
class RbtreePmem_Concurrent {
    public:
        RbtreePmem_Concurrent(const char *path) {
            RbtreePmem_Concurrent::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~RbtreePmem_Concurrent();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct rbtree_pmem_root *root_p = NULL;

        int check();

        void lookup(PMEMoid current_node, uint64_t key, void *&result);

        PMEMoid create_new_node(uint64_t key, void *value);

        PMEMoid bst_upsert(PMEMoid current_node_oid, uint64_t key, void *value);

        void rotate_left(PMEMoid &new_node_oid);

        void rotate_right(PMEMoid &new_node_oid);

        void fix_violation(PMEMoid &current_node);

        void free_node(PMEMoid current_node_oid);

        bool setup_local_area_for_insert(rbtree_pmem_node* this_node);

        rbtree_pmem_node* move_local_upward(rbtree_pmem_node* x, vector<rbtree_pmem_node*>& working);
    };

    /**
     * RbtreePmem_Concurrent::check -- checks if the root object has been initialized
     */
    int RbtreePmem_Concurrent::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: rbtree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreePmem_Concurrent::init -- rbtree initializer
     */
    int RbtreePmem_Concurrent::init(const char *path) {
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, RB_LAYOUT_NAME, PMEM_RB_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        } else {
            if ((pop = pmemobj_open(path, RB_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct rbtree_pmem_root));
        root_p = (struct rbtree_pmem_root *) pmemobj_direct(root_oid);

        if (root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }

        return 1;
    }

    /**
     * RbtreePmem_Concurrent::lookup -- (internal) recursively search the value of a key
     */
    void RbtreePmem_Concurrent::lookup(PMEMoid current_node, uint64_t key, void *&result) {
        //base case
        if (current_node.off == 0) {
            return;
        }

        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node);
        if (current_node_p->key == key) {
            //data found
            result = current_node_p->value;
            return;
        }

        if (current_node_p->key > key) {
            return lookup(current_node_p->left, key, result);
        } else {
            return lookup(current_node_p->right, key, result);
        }
    }

    /**
     * RbtreePmem_Concurrent::read -- read 'value' of 'key' and place it into '&result'
     */
    int RbtreePmem_Concurrent::read(const uint64_t key, void *&result) {
        check();

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p->root_node_oid, key, result);
        return 1;
    }

    /**
     * RbtreePmem_Concurrent::update -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmem_Concurrent::update(const uint64_t key, void *value) {
        insert(key, value);
        return 1;
    }

    /**
     * RbtreePmem_Concurrent::create_new_node -- (internal) allocate memory for new node
     */
    PMEMoid RbtreePmem_Concurrent::create_new_node(uint64_t key, void *value) {
        //prepare new in-memory node
        struct rbtree_pmem_node *in_memory_node_p = (struct rbtree_pmem_node *) malloc(sizeof(struct rbtree_pmem_node));
        in_memory_node_p->left = in_memory_node_p->right = in_memory_node_p->parent = OID_NULL;
        in_memory_node_p->color = RED;
        in_memory_node_p->key = key;
        in_memory_node_p->flag = new int(0);
        strcpy(in_memory_node_p->value, (char *) value);

        //copy in-memory node to pmem-node
        PMEMoid new_node_oid;
        pmemobj_zalloc(pop, &new_node_oid, sizeof(struct rbtree_pmem_node), RB_NODE_TYPE);
        struct rbtree_pmem_node *pmem_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);
        memcpy(pmem_node_p, in_memory_node_p, sizeof(struct rbtree_pmem_node));

        //freeing in-memory node
        free(in_memory_node_p);

        return new_node_oid;
    }

    /**
     * RbtreePmem_Concurrent::bst_upsert -- (internal) update the value if key already exist and return a NULL OID as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node OID
     * will update the balance in later scope
     */
    PMEMoid RbtreePmem_Concurrent::bst_upsert(PMEMoid current_node_oid, uint64_t key, void *value) {
        while (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            if (key == current_node_p->key) {
                //key already exist, update the value and return
                pmemobj_memcpy_persist(pop, current_node_p->value, (char *) value, strlen((char *) value) + 1);

                //return status: updated!
                return OID_NULL;
            } else if (key < current_node_p->key) {
                if (current_node_p->left.off == 0) {
                    PMEMoid new_node_oid = create_new_node(key, value);
                    struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);

                    current_node_p->left = new_node_oid;
                    new_node_p->parent = current_node_oid;
                    pmemobj_persist(pop, &current_node_p->left, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &new_node_oid, sizeof(struct rbtree_pmem_node));

                    //return status: inserted!
                    return new_node_oid;
                } else {
                    current_node_oid = current_node_p->left;
                }
            } else {
                if (current_node_p->right.off == 0) {
                    PMEMoid new_node_oid = create_new_node(key, value);
                    struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);

                    current_node_p->right = new_node_oid;
                    new_node_p->parent = current_node_oid;
                    pmemobj_persist(pop, &current_node_p->right, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &new_node_oid, sizeof(struct rbtree_pmem_node));

                    //return status: inserted!
                    return new_node_oid;
                } else {
                    current_node_oid = current_node_p->right;
                }
            }
        }

        fprintf(stderr, "[%s]: FATAL: failed to insert or update!\n", __FUNCTION__);
        assert(0);
    }

    /**
     * RbtreePmem_Concurrent::rotate_left -- (internal) Rebalance RB-Tree.
     */
    void RbtreePmem_Concurrent::rotate_left(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        PMEMoid old_right_node_oid = current_node_p->right;
        struct rbtree_pmem_node *old_right_node_p = (struct rbtree_pmem_node *) pmemobj_direct(old_right_node_oid);
        current_node_p->right = old_right_node_p->left;
        //pmemobj_persist(pop, &current_node_p->right, sizeof(struct rbtree_pmem_node));

        if (current_node_p->right.off != 0) {
            struct rbtree_pmem_node *new_right_node_p = (struct rbtree_pmem_node *) pmemobj_direct(
                    current_node_p->right);
            new_right_node_p->parent = current_node_oid;
            pmemobj_persist(pop, &new_right_node_p->parent, sizeof(struct rbtree_pmem_node));
        }

        old_right_node_p->parent = current_node_p->parent;
        //pmemobj_persist(pop, &old_right_node_p->parent, sizeof(struct rbtree_pmem_node));

        if (current_node_p->parent.off == 0) {
            root_p->root_node_oid = old_right_node_oid;
            pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct rbtree_pmem_node));
        } else {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);

            if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                parent_node_p->left = old_right_node_oid;
                pmemobj_persist(pop, &parent_node_p->left, sizeof(struct rbtree_pmem_node));
            } else {
                parent_node_p->right = old_right_node_oid;
                pmemobj_persist(pop, &parent_node_p->right, sizeof(struct rbtree_pmem_node));
            }
        }
        old_right_node_p->left = current_node_oid;
        //pmemobj_persist(pop, &old_right_node_p->left, sizeof(struct rbtree_pmem_node));
        current_node_p->parent = old_right_node_oid;
        //pmemobj_persist(pop, &current_node_p->parent, sizeof(struct rbtree_pmem_node));

        pmemobj_persist(pop, &old_right_node_oid, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &current_node_oid, sizeof(struct rbtree_pmem_node));
    }

    /**
     * RbtreePmem_Concurrent::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreePmem_Concurrent::rotate_right(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        PMEMoid old_left_node_oid = current_node_p->left;
        struct rbtree_pmem_node *old_left_node_p = (struct rbtree_pmem_node *) pmemobj_direct(old_left_node_oid);
        current_node_p->left = old_left_node_p->right;
        //pmemobj_persist(pop, &current_node_p->left, sizeof(struct rbtree_pmem_node));

        if (current_node_p->left.off != 0) {
            struct rbtree_pmem_node *new_left_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->left);
            new_left_node_p->parent = current_node_oid;
            pmemobj_persist(pop, &new_left_node_p->parent, sizeof(struct rbtree_pmem_node));
        }
        old_left_node_p->parent = current_node_p->parent;
        //pmemobj_persist(pop, &old_left_node_p->parent, sizeof(struct rbtree_pmem_node));

        if (current_node_p->parent.off == 0) {
            root_p->root_node_oid = old_left_node_oid;
            pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct rbtree_pmem_node));
        }
        else {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);

            if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                parent_node_p->left = old_left_node_oid;
                pmemobj_persist(pop, &parent_node_p->left, sizeof(struct rbtree_pmem_node));
            } else {
                parent_node_p->right = old_left_node_oid;
                pmemobj_persist(pop, &parent_node_p->right, sizeof(struct rbtree_pmem_node));
            }
        }

        old_left_node_p->right = current_node_oid;
        //pmemobj_persist(pop, &old_left_node_p->right, sizeof(struct rbtree_pmem_node));
        current_node_p->parent = old_left_node_oid;
        //pmemobj_persist(pop, &current_node_p->parent, sizeof(struct rbtree_pmem_node));

        pmemobj_persist(pop, &old_left_node_oid, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &current_node_oid, sizeof(struct rbtree_pmem_node));
    }

    /**
     * RbtreePmem_Concurrent::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreePmem_Concurrent::fix_violation(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
        vector<rbtree_pmem_node*> local_area;

        while ((!OID_EQUALS(current_node_oid, root_p->root_node_oid))
               && (current_node_p->color != BLACK)
               && (((struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent))->color == RED)) {

            //printf("current node pointer: %p\n", current_node_p);

            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);


            PMEMoid grandparent_node_oid = parent_node_p->parent;
            struct rbtree_pmem_node *grandparent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(
                    grandparent_node_oid);

            local_area.push_back(current_node_p);
            local_area.push_back(parent_node_p);
            local_area.push_back(grandparent_node_p);

            /* case: parent is left child of grandparent node */
            if (OID_EQUALS(parent_node_oid, grandparent_node_p->left)) {
                PMEMoid uncle_node_oid = grandparent_node_p->right;
                struct rbtree_pmem_node *uncle_node_p = (struct rbtree_pmem_node *) pmemobj_direct(uncle_node_oid);
                local_area.push_back(uncle_node_p);

                /* case: uncle is also red, only recoloring is required */
                if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                    //update the colors
                    grandparent_node_p->color = RED;
                    parent_node_p->color = BLACK;
                    uncle_node_p->color = BLACK;

                    pmemobj_persist(pop, grandparent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, parent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, uncle_node_p, sizeof(struct rbtree_pmem_node));
                    move_local_upward(current_node_p,local_area);
                    current_node_oid = grandparent_node_oid;
                    current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                } else {
                    /* case: current_node is right child of its parent -> left-rotation required */
                    if (OID_EQUALS(current_node_oid, parent_node_p->right)) {
                        rotate_left(parent_node_oid);

                        current_node_oid = parent_node_oid;
                        current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                        parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent);
                    }
                    /* case: current_node is now become the left child of its parent -> right-rotation required */
                    rotate_right(grandparent_node_oid);
                    
                    //std::swap(parent_node_p->color, grandparent_node_p->color);
                    bool parent_node_color = parent_node_p->color;
                    parent_node_p->color = grandparent_node_p->color;
                    grandparent_node_p->color = parent_node_color;

                    pmemobj_persist(pop, &parent_node_oid, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &grandparent_node_oid, sizeof(struct rbtree_pmem_node));
                    
                    current_node_oid = parent_node_oid;
                    current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                }
            }
                /* case: parent is right child of grandparent node */
            else {
                PMEMoid uncle_node_oid = grandparent_node_p->left;
                struct rbtree_pmem_node *uncle_node_p = (struct rbtree_pmem_node *) pmemobj_direct(uncle_node_oid);
                local_area.push_back(uncle_node_p);
                /* case: uncle is also red, only recoloring is required */
                if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                    grandparent_node_p->color = RED;
                    parent_node_p->color = BLACK;
                    uncle_node_p->color = BLACK;

                    pmemobj_persist(pop, grandparent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, parent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, uncle_node_p, sizeof(struct rbtree_pmem_node));
                    move_local_upward(current_node_p,local_area);
                    current_node_oid = grandparent_node_oid;
                    current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                } else {
                    /* case: current_node is left child of its parent -> right-rotation required */
                    if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                        rotate_right(parent_node_oid);

                        current_node_oid = parent_node_oid;
                        current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                        parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent);
                    }

                    /* case: current_node is right child of its parent -> left-rotation required */
                    rotate_left(grandparent_node_oid);
                    
                    //std::swap(parent_node_p->color, grandparent_node_p->color);
                    bool parent_node_color = parent_node_p->color;
                    parent_node_p->color = grandparent_node_p->color;
                    grandparent_node_p->color = parent_node_color;

                    pmemobj_persist(pop, &parent_node_oid, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &grandparent_node_oid, sizeof(struct rbtree_pmem_node));

                    current_node_oid = parent_node_oid;
                    current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                }
            }
        }

        struct rbtree_pmem_node *root_node_p = (struct rbtree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        root_node_p->color = BLACK;
        pmemobj_persist(pop, root_node_p, sizeof(struct rbtree_pmem_node));
    }

    /**
     * RbtreePmem_Concurrent::insert -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmem_Concurrent::insert(const uint64_t key, void *value) {
        check();

        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        //root node is NULL, insert to root node
        if (root_p->root_node_oid.off == 0) {
            root_p->root_node_oid = create_new_node(key, value);
            struct rbtree_pmem_node* root_ptr =  (struct rbtree_pmem_node *)pmemobj_direct(root_p->root_node_oid);
            

            //fix violation will update the color and persist it
            fix_violation(root_p->root_node_oid);
            return 1;
        }

        // Do a normal BST insert
        //PMEMoid new_node_oid = bst_upsert(root_p->root_node_oid, key, value);
        PMEMoid new_node_oid = create_new_node(key, value);
        rbtree_pmem_node* new_node = (struct rbtree_pmem_node *)pmemobj_direct(new_node_oid);

        struct rbtree_pmem_node* temp1,*temp2;
        struct rbtree_pmem_node* root_ptr = (struct rbtree_pmem_node *)pmemobj_direct(root_p->root_node_oid);
        while(true){
            temp1 = root_ptr;
            temp2 = NULL;
            while(temp1!=NULL){
                //cout<<"entersecond loop"<<endl;
                //cout<<endl<<temp1->key;
                temp2 = temp1;
                if(key<temp1->key){
                    temp1 = (struct rbtree_pmem_node *)pmemobj_direct(temp1->left);

                    //cout<<"goleft"<<endl;
                }
                else{
                    temp1 = (struct rbtree_pmem_node *)pmemobj_direct(temp1->right);
                    //if(temp1==NULL)
                    //    break;

                    //cout<<"goright"<<endl;

                }
                //cout<<"temp3NULL"<<" ";
                ////cout<<(temp1==NULL);
            }
            //cout<<"escape loop"<<endl;
            if(!setup_local_area_for_insert(temp2)){
                //cout<<"enter setuplocal area"<<endl;
                temp2->flag = new int(0);
            }
            else{
                break;
            }

        }
        new_node->flag = new int(1);
         fix_violation(new_node_oid);
        return 1;
    }

    /**
     * RbtreePmem_Concurrent::free_node -- (internal) recursively free the tree node's memory
     */
    void RbtreePmem_Concurrent::free_node(PMEMoid current_node_oid) {
        if (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            free_node(current_node_p->left);
            free_node(current_node_p->right);

            pmemobj_free(&current_node_oid);
        }
    }

    /**
     * RbtreePmem_Concurrent::destroy -- destructor
     */
    void RbtreePmem_Concurrent::destroy() {
        check();
        free_node(root_p->root_node_oid);
        root_p->root_node_oid = OID_NULL;
        pmemobj_free(&root_oid);
        root_oid = OID_NULL;
        pmemobj_close(pop);
    }

    bool RbtreePmem_Concurrent::setup_local_area_for_insert(rbtree_pmem_node* this_ptr) {
        if(this_ptr==NULL){
            return true;
        }

        PMEMoid parent_oid = this_ptr->parent;
        if(parent_oid.off==0){
            return true;
        }
        rbtree_pmem_node* parent_node = (struct rbtree_pmem_node *)pmemobj_direct(parent_oid);
        PMEMoid uncle_oid;
        if(!__sync_bool_compare_and_swap(this_ptr->flag,0,1)){
            return false;
        }
        if(!__sync_bool_compare_and_swap(parent_node->flag, 0,1)){
            return false;
        }

        if(parent_node!= (struct rbtree_pmem_node *)pmemobj_direct(this_ptr->parent)){
            parent_node->flag = false;
            return false;
        }

        if(this_ptr == (struct rbtree_pmem_node *)pmemobj_direct(parent_node->left)){
            uncle_oid = parent_node->right;
        }

        else{
            uncle_oid = parent_node->left;
        }

        rbtree_pmem_node* uncle_ptr = (struct rbtree_pmem_node *)pmemobj_direct(uncle_oid);
        if(uncle_oid.off!=0 && !__sync_bool_compare_and_swap(uncle_ptr->flag,0,1)){
            rbtree_pmem_node* now_parent_node = (struct rbtree_pmem_node *)pmemobj_direct(this_ptr->parent);
            now_parent_node->flag=false;
            return false;
        }

        return true;


    }

    rbtree_pmem_node* move_local_upward(rbtree_pmem_node* x, vector<rbtree_pmem_node*>& working){
        rbtree_pmem_node* parent_ptr = (struct rbtree_pmem_node *)pmemobj_direct(x->parent);
        rbtree_pmem_node* grandparent_ptr = (struct rbtree_pmem_node *)pmemobj_direct(parent_ptr->parent);
        rbtree_pmem_node *uncle_ptr;
         if(parent_ptr == (struct rbtree_pmem_node *)pmemobj_direct(grandparent_ptr->left)){
             uncle_ptr = (struct rbtree_pmem_node *)pmemobj_direct(grandparent_ptr->right);
         }
         else{
             uncle_ptr = (struct rbtree_pmem_node *)pmemobj_direct(grandparent_ptr->left);
         }

         rbtree_pmem_node* updated_this, *updated_parent = NULL, *updated_uncle = NULL, *updated_grandparent = NULL;
         updated_this = grandparent_ptr;


         while(true && updated_this->parent.off!=NULL){
             updated_parent = (struct rbtree_pmem_node *)pmemobj_direct(updated_this->parent);
             if(!__sync_bool_compare_and_swap(updated_parent->flag,0,1)){
                 continue;
             }
             updated_grandparent = (struct rbtree_pmem_node *)pmemobj_direct(updated_parent->parent);
             if(updated_parent->parent.off==0){
                 break;
             }
             if(!__sync_bool_compare_and_swap(updated_grandparent->flag,0,1)){
                 updated_parent->flag = false;
                 continue;
             }

             if(updated_parent == (struct rbtree_pmem_node *)pmemobj_direct(updated_grandparent->left)){
                 updated_uncle = (struct rbtree_pmem_node *)pmemobj_direct(updated_grandparent->left);
             }
             else{
                 updated_uncle = (struct rbtree_pmem_node *)pmemobj_direct(updated_grandparent->right);
             }

             if(updated_uncle!=NULL && __sync_bool_compare_and_swap(updated_uncle->flag,0,1)){
                 updated_grandparent->flag = false;
                 updated_parent->flag = false;
                 continue;
             }
             break;
         }

         working.push_back(updated_this);
         working.push_back(updated_parent);
         working.push_back(updated_grandparent);
         working.push_back(updated_uncle);

         return updated_uncle;



    }
