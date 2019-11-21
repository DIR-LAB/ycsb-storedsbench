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

namespace ycsbc {
    class RbtreePmem : public StoredsBase {
    public:
        RbtreePmem(const char *path) {
            RbtreePmem::init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

        ~RbtreePmem();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct rbtree_pmem_node *pmem_root_p = NULL;

        int check();

        void lookup(PMEMoid current_node, uint64_t key, void *&result);

        bool update_if_found(PMEMoid current_node, uint64_t key, void *value);

        PMEMoid create_new_node(uint64_t key, void *value);

        PMEMoid bst_insert(PMEMoid current_node_oid, PMEMoid new_node_oid);
        
        void rotate_left(PMEMoid &new_node_oid);
        
        void rotate_right(PMEMoid &new_node_oid);

        void fix_violation(PMEMoid &current_node);
    };

    int RbtreePmem::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: rbtree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    int RbtreePmem::init(const char *path) {
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

        root_oid = pmemobj_root(pop, sizeof(struct rbtree_pmem_node));
        pmem_root_p = (struct rbtree_pmem_node *) pmemobj_direct(root_oid);
        if(pmem_root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }
        root_oid.off=0;
        return 1;
    }

    void RbtreePmem::lookup(PMEMoid current_node, uint64_t key, void *&result) {
        //base case
        if(current_node.off == 0) {
            return;
        }

        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node);
        if(current_node_p->key == key) {
            //data found
            result = current_node_p->value;
            return;
        }

        if(current_node_p->key > key) {
            return lookup(current_node_p->left, key, result);
        }
        else {
            return lookup(current_node_p->right, key, result);
        }
    }

    int RbtreePmem::read(const char *key, void *&result) {
        check();

        uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_oid, uint64_key, result);
        return 1;
    }

    int RbtreePmem::update(const char *key, void *value) {
        check();
        insert(key, value);
        return 1;
    }

    /*
     * update_if_found -- (internal) Read and update Value for a given Key of RBTree
     */
    bool RbtreePmem::update_if_found(PMEMoid current_node, uint64_t key, void *value) {
        //base case
        if(current_node.off == 0) {
            return false;
        }
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node);

        
        if(current_node_p->key == key) {
            memcpy(current_node_p->value, (char *) value, strlen((char *) value) + 1);

            
            pmemobj_persist(pop, &current_node_p->value, strlen((char *) value) + 1);
            return true;
        }
        if(current_node_p->key > key) {
            return update_if_found(current_node_p->left, key, value);
        }
        else {
            return update_if_found(current_node_p->right, key, value);
        }
    }

    /*
     * create_new_node -- (internal) allocate memory for new node
     */
    PMEMoid RbtreePmem::create_new_node(uint64_t key, void *value) {
        //prepare new in-memory node
        struct rbtree_pmem_node *in_memory_node_ptr = (struct rbtree_pmem_node *) malloc(sizeof(struct rbtree_pmem_node));
        in_memory_node_ptr->left = in_memory_node_ptr->right = in_memory_node_ptr->parent = OID_NULL;
        in_memory_node_ptr->color = RED;
        in_memory_node_ptr->key = key;
        strcpy(in_memory_node_ptr->value, (char *) value);

        //copy in-memory node to pmem-node
        PMEMoid new_node_oid;
        pmemobj_zalloc(pop, &new_node_oid, sizeof(struct rbtree_pmem_node), RB_NODE_TYPE);
        struct rbtree_pmem_node *pmem_node_ptr = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);
        memcpy(pmem_node_ptr, in_memory_node_ptr, sizeof(struct rbtree_pmem_node));
        
        //freeing in-memory node
        free(in_memory_node_ptr);

        return new_node_oid;
    }

    /*
     * bst_insert -- (internal) insert into raw bst, will update the balance later scope
     */
    //todo: we are persisting too many things ... we should convert this to iterative process to avoid unnecessary persist
    PMEMoid RbtreePmem::bst_insert(PMEMoid current_node_oid, PMEMoid new_node_oid) {
        
        if (current_node_oid.off == 0) {
            return new_node_oid;
        }

        /* Otherwise, recur down the tree */
        struct rbtree_pmem_node *current_node_ptr = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
        struct rbtree_pmem_node *new_node_ptr = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);
        if (new_node_ptr->key < current_node_ptr->key) {
            current_node_ptr->left = bst_insert(current_node_ptr->left, new_node_oid);
            struct rbtree_pmem_node *left_ptr = (struct rbtree_pmem_node *) pmemobj_direct(current_node_ptr->left);
            left_ptr->parent = current_node_oid;

            pmemobj_persist(pop, &current_node_ptr->left, sizeof(struct rbtree_pmem_node));
            pmemobj_persist(pop, &left_ptr->parent, sizeof(struct rbtree_pmem_node));
        }
        else {
            current_node_ptr->right = bst_insert(current_node_ptr->right, new_node_oid);
            struct rbtree_pmem_node *right_ptr = (struct rbtree_pmem_node *) pmemobj_direct(current_node_ptr->right);
            right_ptr->parent = current_node_oid;

            pmemobj_persist(pop, &current_node_ptr->right, sizeof(struct rbtree_pmem_node));
            pmemobj_persist(pop, &right_ptr->parent, sizeof(struct rbtree_pmem_node));
        }

        /* return the (unchanged) node */
        return current_node_oid;
    }

    /*
     * fix_violation -- (internal) Rebalance RB-Tree. This operation can be done in relaxed or active manner.
     */
    void RbtreePmem::rotate_left(PMEMoid &current_oid){
        rbtree_pmem_node *current_ptr = (struct rbtree_pmem_node *)pmemobj_direct(current_oid);
        PMEMoid right_node_oid = current_ptr->right;
        rbtree_pmem_node *right_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(right_node_oid);
        PMEMoid left_of_right = right_node_ptr->left; 
        current_ptr->right = left_of_right;
        if(right_node_oid.off!=0){
            right_node_ptr->parent = current_oid;
        }

        
        right_node_ptr->parent = current_ptr->parent;
        
        PMEMoid current_parent_oid = current_ptr->parent;
        
        rbtree_pmem_node *current_parent_node = (struct rbtree_pmem_node *)pmemobj_direct(current_parent_oid);
        
        
        if(current_ptr->parent.off==0){
            root_oid = right_node_oid;
        }

        else {
            PMEMoid parent_left_oid = current_parent_node->left;
        
            PMEMoid parent_right_oid = current_parent_node->right;
        
            if(current_ptr==(struct rbtree_pmem_node *)pmemobj_direct(parent_left_oid)){
                current_parent_node->left = right_node_oid;
            }

         else{
             current_parent_node->right = right_node_oid;
         }
        }
         right_node_ptr->left = current_oid;
         current_ptr->parent = right_node_oid;

    }
    void RbtreePmem::rotate_right(PMEMoid &current_oid){
        rbtree_pmem_node *current_ptr = (struct rbtree_pmem_node *)pmemobj_direct(current_oid);
        PMEMoid left_node_oid = current_ptr->left;
        rbtree_pmem_node *left_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(left_node_oid);
        current_ptr->left = left_node_ptr->right;
        if(left_node_oid.off!=0){
            left_node_ptr->parent = current_oid;    
        }

        PMEMoid parent_node_oid = current_ptr->parent;
        rbtree_pmem_node *parent_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(parent_node_oid);
        
        left_node_ptr->parent = current_ptr->parent;
        
        if(current_ptr->parent.off==0){
            root_oid = left_node_oid;
        }
        
        else if((struct rbtree_pmem_node *)pmemobj_direct(current_oid) == (struct rbtree_pmem_node *)pmemobj_direct(parent_node_ptr->left)){
            parent_node_ptr->left = left_node_oid;
        }

        else{
            parent_node_ptr->right = left_node_oid;
        }

        left_node_ptr->right = current_oid;
        current_ptr->parent = left_node_oid;
    }

    void RbtreePmem::fix_violation(PMEMoid &current_oid) {
        //todo: implement logic here
        rbtree_pmem_node *current_ptr = (struct rbtree_pmem_node *)pmemobj_direct(current_oid);
        
        rbtree_pmem_node *parent_node_ptr;
        PMEMoid parent_node_oid = current_ptr->parent;
        parent_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(parent_node_oid);
        if(parent_node_oid.off==0){
            rbtree_pmem_node *root_ptr = (struct rbtree_pmem_node *)pmemobj_direct(root_oid);
            root_ptr->color = BLACK;    
            return;
        }
        
         while((current_ptr!=(struct rbtree_pmem_node *)pmemobj_direct(root_oid))&&
            (current_ptr->color!=BLACK)&&
            (parent_node_ptr->color==RED)){
         
             PMEMoid grandparent_node_oid = parent_node_ptr->parent;
             rbtree_pmem_node *grandparent_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(grandparent_node_oid);
             if(parent_node_ptr==(struct rbtree_pmem_node *)pmemobj_direct(grandparent_node_ptr->left)){
         
                 PMEMoid uncle_node_oid = grandparent_node_ptr->right;
                rbtree_pmem_node *uncle_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(uncle_node_oid);
                
                if(uncle_node_oid.off!=0 && uncle_node_ptr->color==RED){
                    grandparent_node_ptr->color = RED;
                    parent_node_ptr->color = BLACK;
                    uncle_node_ptr->color = BLACK;
                    current_ptr = grandparent_node_ptr;
                }
                else{
                    if(current_ptr==(struct rbtree_pmem_node *)pmemobj_direct(parent_node_ptr->right)){
                        rotate_left(parent_node_oid);
                        current_ptr = parent_node_ptr;
                        parent_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(current_ptr->parent);

                    }
                    rotate_right(grandparent_node_oid);
                    std::swap(parent_node_ptr->color,grandparent_node_ptr->color);
                    current_ptr = parent_node_ptr;
                }

                

            }
            /* Case: Parent of current_node is right child of Grand-parent of current_node */
            else{
                PMEMoid uncle_node_oid = grandparent_node_ptr->left;
                rbtree_pmem_node *uncle_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(uncle_node_oid);
                /*Case: If the uncle of the node is also red, then we only require recoloring */
                if(current_ptr==(struct rbtree_pmem_node *)pmemobj_direct(parent_node_ptr->left)){
                        grandparent_node_ptr->color = RED;
                        parent_node_ptr->color = BLACK;
                        uncle_node_ptr->color = BLACK;
                        current_ptr = grandparent_node_ptr;                        
                }

                else{
                    if(current_ptr==(struct rbtree_pmem_node *)pmemobj_direct(parent_node_ptr->left)){
                        rotate_right(parent_node_oid);
                        current_ptr = parent_node_ptr;
                        parent_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(current_ptr->parent);    
                        
                    }
                    
                    rotate_left(grandparent_node_oid);
                    std::swap(parent_node_ptr->color,grandparent_node_ptr->color);
                    current_ptr = parent_node_ptr;
                
                }

            }
        }
        
        rbtree_pmem_node *root_ptr = (struct rbtree_pmem_node *)pmemobj_direct(root_oid);
        root_ptr->color = BLACK;


    }

    int RbtreePmem::insert(const char *key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        
        uint64_t uint64_key = strtoull(key, NULL, 0);


        
        
        PMEMoid new_oid = create_new_node(uint64_key, value);
        
        
        
        rbtree_pmem_node *new_ptr = (struct rbtree_pmem_node *)pmemobj_direct(new_oid);
        // Do a normal BST insert
        
        PMEMoid cur_oid = root_oid;
        if(root_oid.off==0){
            
            root_oid= new_oid;
            fix_violation(new_oid);
            pmemobj_persist(pop,&root_oid,sizeof(struct rbtree_pmem_node));

            return 1;
        }
        
        if(update_if_found(root_oid, uint64_key, value)) return 1;

        
        while(cur_oid.off!=0){
            rbtree_pmem_node *cur_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(cur_oid);
            if(uint64_key < cur_node_ptr->key){
                if(cur_node_ptr->left.off==0){
                    cur_node_ptr->left = new_oid;
                    new_ptr->parent = cur_oid;
                    pmemobj_persist(pop,&cur_node_ptr->left,sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop,&new_oid,sizeof(struct rbtree_pmem_node));
                    break;
                }
                else{
                    cur_oid = cur_node_ptr->left;
                }
            }
            //todo could add equals condition here, and if equals do update.
            else{
                if(cur_node_ptr->right.off==0){
                    cur_node_ptr->right = new_oid;
                    new_ptr->parent = cur_oid;
                    pmemobj_persist(pop,&cur_node_ptr->right,sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop,&new_oid,sizeof(struct rbtree_pmem_node));
                    break;
                }
                else{
                    cur_oid= cur_node_ptr->right;
                }
            }
            


        }
        
        
        rbtree_pmem_node *cur_node_ptr = (struct rbtree_pmem_node *)pmemobj_direct(cur_oid);
        rbtree_pmem_node *new_ptr_this = (struct rbtree_pmem_node *)pmemobj_direct(new_oid);
        rbtree_pmem_node *parent_ptr = (struct rbtree_pmem_node *)pmemobj_direct(new_ptr_this->parent);
        
        //root_oid = bst_insert(root_oid, new_node);

        fix_violation(new_oid);
        return 1;
    }

    void RbtreePmem::destroy() {
        check();
        //todo: update logic
    }
}   //ycsbc