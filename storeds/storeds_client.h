//
// A C++ client that wraps all data structures
// in StoreDS
//

#ifndef YCSB_C_STOREDS_CLIENT_H_
#define YCSB_C_STOREDS_CLIENT_H_

#include <iostream>
#include <string>

#include "array/array_dram.h"
#include "array/array_pmem.h"

namespace ycsbc {

    class StoredsClient {
    public:
        StoredsClient(const char *host, const char *path);
        ~StoredsClient();

        int (*func_call_init [])(const char *) = {array_dram_init, array_pmem_init};
        int (*func_call_read [])(const char *, void *) = {array_dram_read, array_pmem_read};
        int (*func_call_scan [])(const char *, int, void *) = {array_dram_scan, array_pmem_scan};
        int (*func_call_update [])(const char *, const char *) = {array_dram_update, array_pmem_update};
        int (*func_call_insert [])(const char *, const char *) = {array_dram_insert, array_pmem_insert};
        int (*func_call_delete [])(const char *) = {array_dram_delete, array_pmem_delete};

        int type;
    };

//
// Implementation
//
    inline StoredsClient::StoredsClient(const char *type, const char *path){
        int type_index = 0;
        if (strcmp(type, "array-dram") == 0){
            type_index = 0;
        } else if (strcmp(type, "array-pmem") == 0) {
            type_index = 1;
        } else {
            return;
        }
        func_call_init[type_index](path);
        return;
    }

    inline StoredsClient::~StoredsClient() {
        func_call_finit[type_index]();
        return
    }

    inline int StoredsClient::Read(const char *key, void *result){
        return func_call_read(key, result);
    }

    inline int storedsClient::Update(const char *key, const char *value){
        return func_call_update(key, value);
    }

    inline int storedsClient::Scan(const char *key, int len, const char *result){
        return func_call_scan(key, len, result);
    }

    inline int storedsClient::Insert(const char *key, const char *value){
        return func_call_insert(key, value);
    }

    inline int storedsClient::Delete(const char *key){
        return func_call_delete(key);
    }

} // namespace ycsbc

#endif // YCSB_C_STOREDS_CLIENT_H_
