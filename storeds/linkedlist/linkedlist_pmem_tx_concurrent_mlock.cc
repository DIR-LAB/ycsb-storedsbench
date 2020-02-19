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
    class LinkedlistPmemTxConcurrentMLock : public StoredsBase {
    public:
        LinkedlistPmemTxConcurrentMLock(const char *path) {
            LinkedlistPmemTxConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~LinkedlistPmemTxConcurrentMLock();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct ll_pmem_concurrent_mlock_root *root_p = NULL;

        int check();

        PMEMoid create_node(const uint64_t key, void *value);
    };

    int LinkedlistPmemTxConcurrentMLock::check() {
        if (root_p->head.off == 0) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    int LinkedlistPmemTxConcurrentMLock::init(const char *path) {
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

    int LinkedlistPmemTxConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    int LinkedlistPmemTxConcurrentMLock::read(const uint64_t key, void *&result) {
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

    int LinkedlistPmemTxConcurrentMLock::update(const uint64_t key, void *value) {
        check();

        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;
        struct ll_pmem_node *current_node = (struct ll_pmem_node *) pmemobj_direct(root_p->head);

        while (current_node != NULL) {
            if (current_node->key == key) {
                TX_BEGIN(pop) {
                    pmemobj_tx_add_range_direct(current_node, sizeof(struct ll_pmem_node));
                    memcpy(current_node->value, (const char *) value, strlen((char *) value) + 1);
                } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
                } TX_END
                break;
            }
            current_node = (struct ll_pmem_node *) pmemobj_direct(current_node->next);
        }
        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    inline PMEMoid LinkedlistPmemTxConcurrentMLock::create_node(const uint64_t key, void *value) {
        PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct ll_pmem_node), LL_NODE_TYPE);
        struct ll_pmem_node *pmem_node_ptr = (struct ll_pmem_node *) pmemobj_direct(new_node_oid);
        pmem_node_ptr->key = key;
        memcpy(pmem_node_ptr->value, (char *) value, strlen((char *) value) + 1);

        return new_node_oid;
    }

    int LinkedlistPmemTxConcurrentMLock::insert(const uint64_t key, void *value) {
        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;
        TX_BEGIN(pop) {
            PMEMoid new_node_oid = create_node(key, value);
            if(root_p->head.off == 0) {
                pmemobj_tx_add_range(root_oid, 0, sizeof(struct ll_pmem_concurrent_mlock_root));
                root_p->head = new_node_oid;
                root_p->tail = new_node_oid;
            }
            else {
                pmemobj_tx_add_range(root_p->tail, 0, sizeof(struct ll_pmem_node));
                ((struct ll_pmem_node *) pmemobj_direct(root_p->tail))->next = new_node_oid;
                root_p->tail = ((struct ll_pmem_node *) pmemobj_direct(root_p->tail))->next;
            }
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    void LinkedlistPmemTxConcurrentMLock::destroy() {
        check();
        PMEMoid current_node;

        TX_BEGIN(pop) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct ll_pmem_concurrent_mlock_root));
            while (root_p->head.off != 0) {
                current_node = root_p->head;
                root_p->head = ((struct ll_pmem_node *) pmemobj_direct(root_p->head))->next;
                pmemobj_tx_free(current_node);
            }
            if(root_p->tail.off != 0) pmemobj_tx_free(root_p->tail);
            root_p->head = OID_NULL;
            root_p->tail = OID_NULL;
            root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        pmemobj_close(pop);
    }
}   //ycsbc