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
#include<bits/stdc++.h>

#include "../../lib/ex_common.h"
#include "rbtree_common.h"

using namespace std;

namespace ycsbc {
    class RbtreePmemConcurrentLockfree : public StoredsBase {
    public:
        RbtreePmemConcurrentLockfree(const char *path) {
            RbtreePmemConcurrentLockfree::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);
        
        void levelorder();

        void destroy();

        ~RbtreePmemConcurrentLockfree();

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

        void rotate_right_new(PMEMoid &new_node_oid);

        void rotate_left_new(PMEMoid &new_node_oid);

        void fix_violation(PMEMoid &current_node);

        void free_node(PMEMoid current_node_oid);

        bool setup_local_area_for_insert(PMEMoid this_oid);

        rbtree_pmem_node *move_local_upward(rbtree_pmem_node *x, vector<rbtree_pmem_node *> &working);
    };

    /**
     * RbtreePmemConcurrentLockfree::check -- checks if the root object has been initialized
     */
    int RbtreePmemConcurrentLockfree::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: rbtree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreePmemConcurrentLockfree::init -- rbtree initializer
     */
    int RbtreePmemConcurrentLockfree::init(const char *path) {
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

    int RbtreePmemConcurrentLockfree::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * RbtreePmemConcurrentLockfree::lookup -- (internal) recursively search the value of a key
     */
    void RbtreePmemConcurrentLockfree::lookup(PMEMoid current_node, uint64_t key, void *&result) {
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
     * RbtreePmemConcurrentLockfree::read -- read 'value' of 'key' and place it into '&result'
     */
    int RbtreePmemConcurrentLockfree::read(const uint64_t key, void *&result) {
        check();

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p->root_node_oid, key, result);
        return 1;
    }

    /**
     * RbtreePmemConcurrentLockfree::update -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmemConcurrentLockfree::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * RbtreePmemConcurrentLockfree::create_new_node -- (internal) allocate memory for new node
     */
    PMEMoid RbtreePmemConcurrentLockfree::create_new_node(uint64_t key, void *value) {
        //prepare new in-memory node
        struct rbtree_pmem_node *in_memory_node_p = (struct rbtree_pmem_node *) malloc(sizeof(struct rbtree_pmem_node));
        in_memory_node_p->left = in_memory_node_p->right = in_memory_node_p->parent = OID_NULL;
        in_memory_node_p->color = RED;
        in_memory_node_p->key = key;
        strcpy(in_memory_node_p->value, (char *) value);

        //copy in-memory node to pmem-node
        PMEMoid new_node_oid;
        pmemobj_alloc(pop, &new_node_oid, sizeof(struct rbtree_pmem_node), RB_NODE_TYPE, NULL, NULL);
        struct rbtree_pmem_node *pmem_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);
        memcpy(pmem_node_p, in_memory_node_p, sizeof(struct rbtree_pmem_node));

        //freeing in-memory node
        free(in_memory_node_p);

        return new_node_oid;
    }

    /**
     * RbtreePmemConcurrentLockfree::bst_upsert -- (internal) update the value if key already exist and return a NULL OID as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node OID
     * will update the balance in later scope
     */
    PMEMoid RbtreePmemConcurrentLockfree::bst_upsert(PMEMoid current_node_oid, uint64_t key, void *value) {
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
     * RbtreePmemConcurrentLockfree::rotate_left -- (internal) Rebalance RB-Tree.
     */
    void RbtreePmemConcurrentLockfree::rotate_left(PMEMoid &current_node_oid) {
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
     * RbtreePmemConcurrentLockfree::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreePmemConcurrentLockfree::rotate_right(PMEMoid &current_node_oid) {
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
     * RbtreePmemConcurrentLockfree::rotate_right_new for concurrent rotation
     */
    void RbtreePmemConcurrentLockfree::rotate_right_new(PMEMoid &current_node_oid)
    {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *)pmemobj_direct(current_node_oid);

        PMEMoid old_left_node_oid = current_node_p->left;
        struct rbtree_pmem_node *old_left_node_p = (struct rbtree_pmem_node *)pmemobj_direct(old_left_node_oid);
        current_node_p->left = old_left_node_p->right;
        //pmemobj_persist(pop, &current_node_p->left, sizeof(struct rbtree_pmem_node));

        if (current_node_p->left.off != 0)
        {
            struct rbtree_pmem_node *new_left_node_p = (struct rbtree_pmem_node *)pmemobj_direct(current_node_p->left);
            new_left_node_p->parent = current_node_oid;
            pmemobj_persist(pop, &new_left_node_p->parent, sizeof(struct rbtree_pmem_node));
        }
        old_left_node_p->parent = current_node_p->parent;
        //pmemobj_persist(pop, &old_left_node_p->parent, sizeof(struct rbtree_pmem_node));

        old_left_node_p->right = current_node_oid;
        //pmemobj_persist(pop, &old_left_node_p->right, sizeof(struct rbtree_pmem_node));
        current_node_p->parent = old_left_node_oid;
        //pmemobj_persist(pop, &current_node_p->parent, sizeof(struct rbtree_pmem_node));

        pmemobj_persist(pop, &old_left_node_oid, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &current_node_oid, sizeof(struct rbtree_pmem_node));
        if (current_node_p->parent.off == 0)
        {
            root_p->root_node_oid = old_left_node_oid;
            pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct rbtree_pmem_node));
        }
        else
        {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *)pmemobj_direct(parent_node_oid);

            if (OID_EQUALS(current_node_oid, parent_node_p->left))
            {
                parent_node_p->left = old_left_node_oid;
                pmemobj_persist(pop, &parent_node_p->left, sizeof(struct rbtree_pmem_node));
            }
            else
            {
                parent_node_p->right = old_left_node_oid;
                pmemobj_persist(pop, &parent_node_p->right, sizeof(struct rbtree_pmem_node));
            }
        }
    }

    void RbtreePmemConcurrentLockfree::rotate_left_new(PMEMoid &current_node_oid)
    {
        
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *)pmemobj_direct(current_node_oid);

        PMEMoid old_right_node_oid = current_node_p->right;
        PMEMoid copy_current_node_oid;
        PMEMoid copy_old_right_node_oid;
        pmemobj_alloc(pop, &copy_current_node_oid, sizeof(rbtree_pmem_node), 0, NULL, NULL);
        pmemobj_alloc(pop, &copy_old_right_node_oid, sizeof(rbtree_pmem_node), 0, NULL, NULL);
        struct rbtree_pmem_node *old_right_node_p = (struct rbtree_pmem_node *)pmemobj_direct(old_right_node_oid);

        struct rbtree_pmem_node *copy_old_right_node_p = (struct rbtree_pmem_node *)malloc(sizeof(struct rbtree_pmem_node)), *copy_current_node_p = (struct rbtree_pmem_node *)malloc(sizeof(struct rbtree_pmem_node)), *copy_new_right_node_p = (struct rbtree_pmem_node *)malloc(sizeof(struct rbtree_pmem_node));
        copy_old_right_node_p = (struct rbtree_pmem_node *)pmemobj_direct(copy_old_right_node_oid);
        copy_current_node_p = (struct rbtree_pmem_node *)pmemobj_direct(copy_current_node_oid);

        memcpy(copy_current_node_p, current_node_p, sizeof(struct rbtree_pmem_node));
        memcpy(copy_old_right_node_p, old_right_node_p, sizeof(struct rbtree_pmem_node));
        /*
            copy_current_node_p->key = current_node_p->key;
            strcpy(copy_current_node_p->value,current_node_p->value);
            //copy_current_node_p->left = current_node_p->left;
            //copy_current_node_p->right = current_node_p->right;
            copy_current_node_p->parent = current_node_p->parent;
            copy_current_node_p->color = current_node_p->color;
            copy_current_node_p->flag = current_node_p->flag;
            */

        //current_node_p->right = old_right_node_p->left; //CURRENT RIGHT OLD LEFT OP1
        copy_current_node_p->right = copy_old_right_node_p->left; //PERSIST THIS STATEMENT

        pmemobj_persist(pop, &copy_current_node_p->right, sizeof(struct rbtree_pmem_node));

        if (copy_current_node_p->right.off != 0)
        {
            copy_new_right_node_p = (struct rbtree_pmem_node *)pmemobj_direct(
                copy_current_node_p->right);
            pmemobj_persist(pop, &copy_new_right_node_p, sizeof(struct rbtree_pmem_node));
            //new_right_node_p->parent = current_node_oid;  // NEW RIGHT PARENT = GRANDPARENT OP2
            copy_new_right_node_p->parent = copy_current_node_oid; // NEW RIGHT PARENT = GRANDPARENT OP2
            pmemobj_persist(pop, &copy_new_right_node_p->parent, sizeof(struct rbtree_pmem_node));
        }

        copy_old_right_node_p->parent = copy_current_node_p->parent; //OLD PARENT = GRANDPARENT PARENT OP3 PERSIST ME
        pmemobj_persist(pop, &copy_old_right_node_p->parent, sizeof(struct rbtree_pmem_node));
        copy_old_right_node_p->left = copy_current_node_oid;
        pmemobj_persist(pop, &copy_old_right_node_p->left, sizeof(struct rbtree_pmem_node));
        copy_current_node_p->parent = copy_old_right_node_oid;
        bool curcolor = copy_current_node_p->color;
        copy_current_node_p->color = copy_old_right_node_p->color;
        copy_old_right_node_p->color = curcolor;
        pmemobj_persist(pop, &copy_current_node_p->parent, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &copy_old_right_node_p->color, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &copy_current_node_p->color, sizeof(struct rbtree_pmem_node));

        if (current_node_p->parent.off == 0)
        {

            root_p->root_node_oid = copy_old_right_node_oid; // COPY AND DO

            pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct rbtree_pmem_node));
            return;
        }
        else
        {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *)pmemobj_direct(parent_node_oid);

            if (OID_EQUALS(current_node_oid, parent_node_p->left))
            {
                parent_node_p->left = copy_old_right_node_oid;
                pmemobj_persist(pop, &parent_node_p->left, sizeof(struct rbtree_pmem_node));
            }
            else
            {
                parent_node_p->right = copy_old_right_node_oid;
                pmemobj_persist(pop, &parent_node_p->right, sizeof(struct rbtree_pmem_node));
            }
        }
    }


    /**
     * RbtreePmemConcurrentLockfree::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreePmemConcurrentLockfree::fix_violation(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        while ((!OID_EQUALS(current_node_oid, root_p->root_node_oid))
               && (current_node_p->color != BLACK)
               && (((struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent))->color == RED)) {

            //printf("current node pointer: %p\n", current_node_p);

            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);

            PMEMoid grandparent_node_oid = parent_node_p->parent;
            struct rbtree_pmem_node *grandparent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(
                    grandparent_node_oid);

            /* case: parent is left child of grandparent node */
            if (OID_EQUALS(parent_node_oid, grandparent_node_p->left)) {
                PMEMoid uncle_node_oid = grandparent_node_p->right;
                struct rbtree_pmem_node *uncle_node_p = (struct rbtree_pmem_node *) pmemobj_direct(uncle_node_oid);

                /* case: uncle is also red, only recoloring is required */
                if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                    //update the colors
                    grandparent_node_p->color = RED;
                    parent_node_p->color = BLACK;
                    uncle_node_p->color = BLACK;

                    pmemobj_persist(pop, grandparent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, parent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, uncle_node_p, sizeof(struct rbtree_pmem_node));

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

                /* case: uncle is also red, only recoloring is required */
                if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                    grandparent_node_p->color = RED;
                    parent_node_p->color = BLACK;
                    uncle_node_p->color = BLACK;

                    pmemobj_persist(pop, grandparent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, parent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, uncle_node_p, sizeof(struct rbtree_pmem_node));

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
     * RbtreePmemConcurrentLockfree::insert -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmemConcurrentLockfree::insert(const uint64_t key, void *value) {
        check();

        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        //root node is null, insert to root node
        if (root_p->root_node_oid.off == 0) {
            root_p->root_node_oid = create_new_node(key, value);

            //fix violation will update the color and persist it
            fix_violation(root_p->root_node_oid);
            return 1;
        }

        // Do a normal BST insert
        PMEMoid new_node_oid = bst_upsert(root_p->root_node_oid, key, value);
        if (!OID_IS_NULL(new_node_oid)) fix_violation(new_node_oid);
        cout<<"LEVELORDER"<<endl;
        levelorder();
        cout<<"LEVELORDER"<<endl;
        return 1;
    }

    /**
     * RbtreePmemConcurrentLockfree::free_node -- (internal) recursively free the tree node's memory
     */
    void RbtreePmemConcurrentLockfree::free_node(PMEMoid current_node_oid) {
        if (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            free_node(current_node_p->left);
            free_node(current_node_p->right);

            pmemobj_free(&current_node_oid);
        }
    }

    /**
     * RbtreePmemConcurrentLockfree::destroy -- destructor
     */
    void RbtreePmemConcurrentLockfree::destroy() {
        check();
        free_node(root_p->root_node_oid);
        root_p->root_node_oid = OID_NULL;
        pmemobj_free(&root_oid);
        root_oid = OID_NULL;
        pmemobj_close(pop);
    }
    void RbtreePmemConcurrentLockfree::levelorder()
    {
        queue<PMEMoid> q;
        q.push(root_p->root_node_oid);
        while (!q.empty())
        {
            int sz = q.size();
            for (int i = 0; i < sz; i++)
            {
                rbtree_pmem_node *temp = (struct rbtree_pmem_node *)pmemobj_direct(q.front());
                cout << temp->key << " " << temp->color << " ";
                q.pop();
                if (temp->left.off != 0)
                {
                    q.push(temp->left);
                }
                if (temp->right.off != 0)
                {
                    q.push(temp->right);
                }
            }
            cout << endl;
        }
    }
    rbtree_pmem_node *RbtreePmemConcurrentLockfree::move_local_upward(rbtree_pmem_node *x, vector<rbtree_pmem_node *> &working)
    {
        rbtree_pmem_node *parent_ptr = (struct rbtree_pmem_node *)pmemobj_direct(x->parent);
        rbtree_pmem_node *grandparent_ptr = (struct rbtree_pmem_node *)pmemobj_direct(parent_ptr->parent);
        rbtree_pmem_node *uncle_ptr;
        if (parent_ptr == (struct rbtree_pmem_node *)pmemobj_direct(grandparent_ptr->left))
        {
            uncle_ptr = (struct rbtree_pmem_node *)pmemobj_direct(grandparent_ptr->right);
        }
        else
        {
            uncle_ptr = (struct rbtree_pmem_node *)pmemobj_direct(grandparent_ptr->left);
        }

        rbtree_pmem_node *updated_this, *updated_parent = NULL, *updated_uncle = NULL, *updated_grandparent = NULL;
        updated_this = grandparent_ptr;

        while (true && updated_this->parent.off != 0)
        {
            updated_parent = (struct rbtree_pmem_node *)pmemobj_direct(updated_this->parent);
            if (!__sync_bool_compare_and_swap(updated_parent->flag, 0, 1))
            {
                continue;
            }
            updated_grandparent = (struct rbtree_pmem_node *)pmemobj_direct(updated_parent->parent);
            if (updated_parent->parent.off == 0)
            {
                break;
            }
            if (!__sync_bool_compare_and_swap(updated_grandparent->flag, 0, 1))
            {
                updated_parent->flag = new int(0);
                continue;
            }

            if (updated_parent == (struct rbtree_pmem_node *)pmemobj_direct(updated_grandparent->left))
            {
                updated_uncle = (struct rbtree_pmem_node *)pmemobj_direct(updated_grandparent->left);
            }
            else
            {
                updated_uncle = (struct rbtree_pmem_node *)pmemobj_direct(updated_grandparent->right);
            }

            if (updated_uncle != NULL && __sync_bool_compare_and_swap(updated_uncle->flag, 0, 1))
            {
                updated_grandparent->flag = new int(0);
                updated_parent->flag = new int(0);
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
    bool RbtreePmemConcurrentLockfree::setup_local_area_for_insert(PMEMoid this_oid)
    {
        if (this_oid.off == 0)
        {
            return true;
        }

        struct rbtree_pmem_node *this_ptr = (struct rbtree_pmem_node *)pmemobj_direct(this_oid);

        PMEMoid parent_oid = this_ptr->parent;
        if (parent_oid.off == 0)
        {
            return true;
        }
        rbtree_pmem_node *parent_node = (struct rbtree_pmem_node *)pmemobj_direct(parent_oid);
        PMEMoid uncle_oid;
        if (!__sync_bool_compare_and_swap(this_ptr->flag, 0, 1))
        {
            return false;
        }
        if (!__sync_bool_compare_and_swap(parent_node->flag, 0, 1))
        {
            return false;
        }

        if (parent_node != (struct rbtree_pmem_node *)pmemobj_direct(this_ptr->parent))
        {
            parent_node->flag = new int(0);
            return false;
        }

        if (this_ptr == (struct rbtree_pmem_node *)pmemobj_direct(parent_node->left))
        {
            uncle_oid = parent_node->right;
        }

        else
        {
            uncle_oid = parent_node->left;
        }

        rbtree_pmem_node *uncle_ptr = (struct rbtree_pmem_node *)pmemobj_direct(uncle_oid);
        if (uncle_oid.off != 0 && !__sync_bool_compare_and_swap(uncle_ptr->flag, 0, 1))
        {
            rbtree_pmem_node *now_parent_node = (struct rbtree_pmem_node *)pmemobj_direct(this_ptr->parent);
            now_parent_node->flag = new int(0);
            return false;
        }

        return true;
    }

    
}   //ycsbc