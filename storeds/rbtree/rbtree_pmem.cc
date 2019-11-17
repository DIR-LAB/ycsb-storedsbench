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
    void RbtreePmem::fix_violation(PMEMoid &current_node) {
        //todo: implement logic here
    }

    int RbtreePmem::insert(const char *key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);

        uint64_t uint64_key = strtoull(key, NULL, 0);

        if(update_if_found(root_oid, uint64_key, value)) return 1;
        PMEMoid new_node = create_new_node(uint64_key, value);

        // Do a normal BST insert
        root_oid = bst_insert(root_oid, new_node);
        fix_violation(new_node);
        return 1;
    }

    void RbtreePmem::destroy() {
        check();
        //todo: update logic
    }
}   //ycsbc