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
#include "linkedlist/linkedlist_dram.h"
#include "linkedlist/linkedlist_pmem.h"
#include "linkedlist/linkedlist_pmem_tx.h"
#include "hashmap/ht_dram.h"
#include "hashmap/ht_pmem.h"
#include "hashmap/ht_pmem_tx.h"
#include "skiplist/skiplist_dram.h"
#include "skiplist/skiplist_pmem.h"
#include "skiplist/skiplist_pmem_tx.h"
#include "queue/queue_dram.h"
#include "queue/queue_pmem.h"
#include "queue/queue_pmem_tx.h"

namespace ycsbc {

    class StoredsClient {
    public:
        StoredsClient(const char *host, const char *path);

        int Read(const char *key, void *result);
        int Update(const char *key, void *value);
        int Insert(const char *key, void *value);
        void Free();

        int (*func_call_init[15])(const char *) = {
                array_dram_init,
                array_pmem_init,
                array_pmem_tx_init,
                linkedlist_dram_init,
                linkedlist_pmem_init,
                linkedlist_pmem_tx_init,
                ht_dram_init,
                ht_pmem_init,
                ht_pmem_tx_init,
                skiplist_dram_init,
                skiplist_pmem_init,
                skiplist_pmem_tx_init,
                queue_dram_init,
                queue_pmem_init,
                queue_pmem_tx_init
        };

        int (*func_call_read[15])(const char *, void *) = {
                array_dram_read,
                array_pmem_read,
                array_pmem_tx_read,
                linkedlist_dram_read,
                linkedlist_pmem_read,
                linkedlist_pmem_tx_read,
                ht_dram_read,
                ht_pmem_read,
                ht_pmem_tx_read,
                skiplist_dram_read,
                skiplist_pmem_read,
                skiplist_pmem_tx_read,
                queue_dram_read,
                queue_pmem_read,
                queue_pmem_tx_read
        };

        int (*func_call_update[15])(const char *, void *) = {
                array_dram_update,
                array_pmem_update,
                array_pmem_tx_update,
                linkedlist_dram_update,
                linkedlist_pmem_update,
                linkedlist_pmem_tx_update,
                ht_dram_update,
                ht_pmem_update,
                ht_pmem_tx_update,
                skiplist_dram_update,
                skiplist_pmem_update,
                skiplist_pmem_tx_update,
                queue_dram_update,
                queue_pmem_update,
                queue_pmem_tx_update
        };

        int (*func_call_insert[15])(const char *, void *) = {
                array_dram_insert,
                array_pmem_insert,
                array_pmem_tx_insert,
                linkedlist_dram_insert,
                linkedlist_pmem_insert,
                linkedlist_pmem_tx_insert,
                ht_dram_insert,
                ht_pmem_insert,
                ht_pmem_tx_insert,
                skiplist_dram_insert,
                skiplist_pmem_insert,
                skiplist_pmem_tx_insert,
                queue_dram_insert,
                queue_pmem_insert,
                queue_pmem_tx_insert
        };

        void (*func_call_destroy[15])() = {
                array_dram_free,
                array_pmem_free,
                array_pmem_tx_free,
                linkedlist_dram_free,
                linkedlist_pmem_free,
                linkedlist_pmem_tx_free,
                ht_dram_free,
                ht_pmem_free,
                ht_pmem_tx_free,
                skiplist_dram_free,
                skiplist_pmem_free,
                skiplist_pmem_tx_free,
                queue_dram_free,
                queue_pmem_free,
                queue_pmem_tx_free
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
        } else if (strcmp(type, "linkedlist-dram") == 0) {
            type_index = 3;
        } else if (strcmp(type, "linkedlist-pmem") == 0) {
            type_index = 4;
        } else if (strcmp(type, "linkedlist-pmem-tx") == 0) {
            type_index = 5;
        } else if (strcmp(type, "hashtable-dram") == 0) {
            type_index = 6;
        } else if (strcmp(type, "hashtable-pmem") == 0) {
            type_index = 7;
        } else if (strcmp(type, "hashtable-pmem-tx") == 0) {
            type_index = 8;
        } else if (strcmp(type, "skiplist-dram") == 0) {
            type_index = 9;
        } else if (strcmp(type, "skiplist-pmem") == 0) {
            type_index = 10;
        } else if (strcmp(type, "skiplist-pmem-tx") == 0) {
            type_index = 11;
        } else if (strcmp(type, "queue-dram") == 0) {
            type_index = 12;
        } else if (strcmp(type, "queue-pmem") == 0) {
            type_index = 13;
        } else if (strcmp(type, "queue-pmem-tx") == 0) {
            type_index = 14;
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
