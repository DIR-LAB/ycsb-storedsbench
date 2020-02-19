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
#include "linkedlist_common.h"

namespace ycsbc {
    class LinkedlistPmemConcurrentMLock : public StoredsBase {
    public:
        LinkedlistPmemConcurrentMLock(const char *path) {
            LinkedlistPmemConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~LinkedlistPmemConcurrentMLock();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct ll_pmem_concurrent_mlock_root *root_p = NULL;

        int check();

        PMEMoid create_node(const uint64_t key, void *value);
    };

    int LinkedlistPmemConcurrentMLock::check() {
        if (root_p->head.off == 0) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    int LinkedlistPmemConcurrentMLock::init(const char *path) {
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, LL_LAYOUT_NAME, PMEM_LL_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        } else {
            if ((pop = pmemobj_open(path, LL_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct ll_pmem_concurrent_mlock_root));
        root_p = (struct ll_pmem_concurrent_mlock_root *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }
        return 1;
    }

    int LinkedlistPmemConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    int LinkedlistPmemConcurrentMLock::read(const uint64_t key, void *&result) {
        check();

        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;
        struct ll_pmem_node *current_node = (struct ll_pmem_node *) pmemobj_direct(root_p->head);

        while (current_node != NULL) {
            if (current_node->key == key) {
                result = current_node->value;
                break;
            }
            current_node = (struct ll_pmem_node *) pmemobj_direct(current_node->next);
        }
        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    int LinkedlistPmemConcurrentMLock::update(const uint64_t key, void *value) {
        check();

        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;
        struct ll_pmem_node *current_node = (struct ll_pmem_node *) pmemobj_direct(root_p->head);

        while (current_node != NULL) {
            if (current_node->key == key) {
                pmemobj_memcpy_persist(pop, current_node->value, (const char *) value, strlen((char *) value) + 1);
                break;
            }
            current_node = (struct ll_pmem_node *) pmemobj_direct(current_node->next);
        }
        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    inline PMEMoid LinkedlistPmemConcurrentMLock::create_node(const uint64_t key, void *value) {
        PMEMoid new_node_oid;
        pmemobj_alloc(pop, &new_node_oid, sizeof(struct ll_pmem_node), LL_NODE_TYPE, NULL, NULL);
        struct ll_pmem_node *pmem_node_ptr = (struct ll_pmem_node *) pmemobj_direct(new_node_oid);
        pmem_node_ptr->key = key;
        memcpy(pmem_node_ptr->value, (char *) value, strlen((char *) value) + 1);

        return new_node_oid;
    }

    int LinkedlistPmemConcurrentMLock::insert(const uint64_t key, void *value) {
        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;
        PMEMoid new_node_oid = create_node(key, value);

        if(root_p->head.off == 0) {
            root_p->head = new_node_oid;
            root_p->tail = new_node_oid;
            pmemobj_persist(pop, root_p, sizeof(struct ll_pmem_concurrent_mlock_root));
        }
        else {
            ((struct ll_pmem_node *) pmemobj_direct(root_p->tail))->next = new_node_oid;
            root_p->tail = ((struct ll_pmem_node *) pmemobj_direct(root_p->tail))->next;
            pmemobj_persist(pop, &root_p->tail, sizeof(struct ll_pmem_node));
        }
        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    void LinkedlistPmemConcurrentMLock::destroy() {
        check();
        PMEMoid current_node;

        while (root_p->head.off != 0) {
            current_node = root_p->head;
            root_p->head = ((struct ll_pmem_node *) pmemobj_direct(root_p->head))->next;
            pmemobj_free(&current_node);
        }
        if(root_p->tail.off != 0) pmemobj_free(&(root_p->tail));
        root_p->head = OID_NULL;
        root_p->tail = OID_NULL;
        root_oid = OID_NULL;
        pmemobj_close(pop);
    }
}   //ycsbc