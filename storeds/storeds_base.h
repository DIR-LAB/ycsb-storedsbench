//
// Created by aislam6 on 11/11/19.
//

#ifndef YCSB_STOREDSBENCH_STOREDS_BASE_H
#define YCSB_STOREDSBENCH_STOREDS_BASE_H

#include <stdint.h>
#include "../core/db.h"

namespace ycsbc {

    class StoredsBase {
    public:
        virtual int init(const char *path) = 0;

        virtual int read(const uint64_t key, void *&result) = 0;

        virtual int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) = 0;

        virtual int update(const uint64_t key, void *value) = 0;

        virtual int insert(const uint64_t key, void *value) = 0;

        virtual void destroy() {}
    };
} // ycsbc

#endif //YCSB_STOREDSBENCH_STOREDS_BASE_H