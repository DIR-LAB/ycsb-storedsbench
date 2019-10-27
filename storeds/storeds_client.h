//
// A C++ client that wraps all data structures
// in StoreDS
//

#ifndef YCSB_C_STOREDS_CLIENT_H_
#define YCSB_C_STOREDS_CLIENT_H_

#include <iostream>
#include <string.h>

#include "array/array_dram.h"
#include "array/array_pmem.h"
#include "array/array_pmem_tx.h"
#include "array/array_pmem_raw.h"
#include "linkedlist/linkedlist_dram.h"
#include "linkedlist/linkedlist_pmem.h"
#include "linkedlist/linkedlist_pmem_tx.h"
#include "hashmap/ht_dram.h"
#include "hashmap/ht_pmem.h"
#include "hashmap/ht_pmem_tx.h"
#include "skiplist/skiplist_dram.h"
#include "skiplist/skiplist_pmem.h"
#include "skiplist/skiplist_pmem_tx.h"
#include "rbtree/rbtree_dram.h"
#include "rbtree/rbtree_pmem.h"
#include "rbtree/rbtree_pmem_tx.h"
#include "btree/btree_dram.h"
#include "btree/btree_pmem.h"
#include "btree/btree_pmem_tx.h"

namespace ycsbc {

    class StoredsClient {
    public:
        StoredsClient(const char *host, const char *path);

        int Read(const char *key, void *result);
        int Update(const char *key, void *value);
        int Insert(const char *key, void *value);
        void Free();

        int (*func_call_init[19])(const char *) = {
                array_dram_init,
                array_pmem_init,
                array_pmem_tx_init,
                array_pmem_raw_init,
                linkedlist_dram_init,
                linkedlist_pmem_init,
                linkedlist_pmem_tx_init,
                ht_dram_init,
                ht_pmem_init,
                ht_pmem_tx_init,
                skiplist_dram_init,
                skiplist_pmem_init,
                skiplist_pmem_tx_init,
                rbtree_dram_init,
                rbtree_pmem_init,
                rbtree_pmem_tx_init,
                btree_dram_init,
                btree_pmem_init,
                btree_pmem_tx_init
        };

        int (*func_call_read[19])(const char *, void *) = {
                array_dram_read,
                array_pmem_read,
                array_pmem_tx_read,
                array_pmem_raw_read,
                linkedlist_dram_read,
                linkedlist_pmem_read,
                linkedlist_pmem_tx_read,
                ht_dram_read,
                ht_pmem_read,
                ht_pmem_tx_read,
                skiplist_dram_read,
                skiplist_pmem_read,
                skiplist_pmem_tx_read,
                rbtree_dram_read,
                rbtree_pmem_read,
                rbtree_pmem_tx_read,
                btree_dram_read,
                btree_pmem_read,
                btree_pmem_tx_read
        };

        int (*func_call_update[19])(const char *, void *) = {
                array_dram_update,
                array_pmem_update,
                array_pmem_tx_update,
                array_pmem_raw_update,
                linkedlist_dram_update,
                linkedlist_pmem_update,
                linkedlist_pmem_tx_update,
                ht_dram_update,
                ht_pmem_update,
                ht_pmem_tx_update,
                skiplist_dram_update,
                skiplist_pmem_update,
                skiplist_pmem_tx_update,
                rbtree_dram_update,
                rbtree_pmem_update,
                rbtree_pmem_tx_update,
                btree_dram_update,
                btree_pmem_update,
                btree_pmem_tx_update
        };

        int (*func_call_insert[19])(const char *, void *) = {
                array_dram_insert,
                array_pmem_insert,
                array_pmem_tx_insert,
                array_pmem_raw_insert,
                linkedlist_dram_insert,
                linkedlist_pmem_insert,
                linkedlist_pmem_tx_insert,
                ht_dram_insert,
                ht_pmem_insert,
                ht_pmem_tx_insert,
                skiplist_dram_insert,
                skiplist_pmem_insert,
                skiplist_pmem_tx_insert,
                rbtree_dram_insert,
                rbtree_pmem_insert,
                rbtree_pmem_tx_insert,
                btree_dram_insert,
                btree_pmem_insert,
                btree_pmem_tx_insert
        };

        void (*func_call_destroy[19])() = {
                array_dram_free,
                array_pmem_free,
                array_pmem_tx_free,
                array_pmem_raw_free,
                linkedlist_dram_free,
                linkedlist_pmem_free,
                linkedlist_pmem_tx_free,
                ht_dram_free,
                ht_pmem_free,
                ht_pmem_tx_free,
                skiplist_dram_free,
                skiplist_pmem_free,
                skiplist_pmem_tx_free,
                rbtree_dram_free,
                rbtree_pmem_free,
                rbtree_pmem_tx_free,
                btree_dram_free,
                btree_pmem_free,
                btree_pmem_tx_free
        };

    private:
        int type_index;
    };

//
// Implementation
//
    inline StoredsClient::StoredsClient(const char *type, const char *path) {
        if (strcmp(type, "array-dram") == 0) {
            type_index = 0;
        } else if (strcmp(type, "array-pmem") == 0) {
            type_index = 1;
        } else if (strcmp(type, "array-pmem-tx") == 0) {
            type_index = 2;
        } else if (strcmp(type, "array-pmem-raw") == 0) {
            type_index = 3;
        } else if (strcmp(type, "linkedlist-dram") == 0) {
            type_index = 4;
        } else if (strcmp(type, "linkedlist-pmem") == 0) {
            type_index = 5;
        } else if (strcmp(type, "linkedlist-pmem-tx") == 0) {
            type_index = 6;
        } else if (strcmp(type, "hashtable-dram") == 0) {
            type_index = 7;
        } else if (strcmp(type, "hashtable-pmem") == 0) {
            type_index = 8;
        } else if (strcmp(type, "hashtable-pmem-tx") == 0) {
            type_index = 9;
        } else if (strcmp(type, "skiplist-dram") == 0) {
            type_index = 10;
        } else if (strcmp(type, "skiplist-pmem") == 0) {
            type_index = 11;
        } else if (strcmp(type, "skiplist-pmem-tx") == 0) {
            type_index = 12;
        } else if (strcmp(type, "rbtree-dram") == 0) {
            type_index = 13;
        } else if (strcmp(type, "rbtree-pmem") == 0) {
            type_index = 14;
        } else if (strcmp(type, "rbtree-pmem-tx") == 0) {
            type_index = 15;
        } else if (strcmp(type, "btree-dram") == 0) {
            type_index = 16;
        } else if (strcmp(type, "btree-pmem") == 0) {
            type_index = 17;
        } else if (strcmp(type, "btree-pmem-tx") == 0) {
            type_index = 18;
        } else {
            return;
        }
        func_call_init[type_index](path);
    }

    inline int StoredsClient::Read(const char *key, void *result) {
        return func_call_read[type_index](key, result);
    }

    inline int StoredsClient::Update(const char *key, void *value) {
        return func_call_update[type_index](key, value);
    }

    inline int StoredsClient::Insert(const char *key, void *value) {
        return func_call_insert[type_index](key, value);
    }

    inline void StoredsClient::Free() {
        func_call_destroy[type_index]();
    }

} // namespace ycsbc

#endif // YCSB_C_STOREDS_CLIENT_H_
