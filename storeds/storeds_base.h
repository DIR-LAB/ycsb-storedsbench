//
// Created by aislam6 on 11/11/19.
//

#ifndef YCSB_STOREDSBENCH_STOREDS_BASE_H
#define YCSB_STOREDSBENCH_STOREDS_BASE_H

namespace ycsbc {

    class StoredsBase {
    public:
        virtual int init(const char *path) = 0;

        virtual int read(const char *key, void *&result) = 0;

        virtual int update(const char *key, void *value) = 0;

        virtual int insert(const char *key, void *value) = 0;

        virtual void destroy() {}
    };
} // ycsbc

#endif //YCSB_STOREDSBENCH_STOREDS_BASE_H