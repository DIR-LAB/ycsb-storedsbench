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

namespace ycsbc {

    class StoredsClient {
    public:
        StoredsClient(const char *host, const char *path);

        int Read(const char *key, void *result);
        int Update(const char *key, void *value);
        int Insert(const char *key, void *value);
        void Free();

        int (*func_call_init[3])(const char *) = {array_dram_init, array_pmem_init, array_pmem_tx_init};
        int (*func_call_read[3])(const char *, void *) = {array_dram_read, array_pmem_read, array_pmem_tx_read};
        int (*func_call_update[3])(const char *, void *) = {array_dram_update, array_pmem_update, array_pmem_tx_update};
        int (*func_call_insert[3])(const char *, void *) = {array_dram_insert, array_pmem_insert, array_pmem_tx_insert};
        void (*func_call_destroy[3])() = {array_dram_free, array_pmem_free, array_pmem_tx_free};

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
