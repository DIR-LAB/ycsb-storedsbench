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
        struct rbtree_pmem_root *pmem_root_p = NULL;

        int check();

        void lookup(PMEMoid current_node, uint64_t key, void *&result);

        bool update_if_found(PMEMoid current_node, uint64_t key, void *value);
    };

    int RbtreePmem::check() {
        if (pmem_root_p->root.off == 0) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
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

        root_oid = pmemobj_root(pop, sizeof(struct rbtree_pmem_root));
        pmem_root_p = (struct rbtree_pmem_root *) pmemobj_direct(root_oid);
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
        lookup(pmem_root_p->root, uint64_key, result);
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

    int RbtreePmem::insert(const char *key, void *value) {
        uint64_t uint64_key = strtoull(key, NULL, 0);
        //todo: update logic
        return 1;
    }

    void RbtreePmem::destroy() {
        check();
        //todo: update logic
    }
}   //ycsbc