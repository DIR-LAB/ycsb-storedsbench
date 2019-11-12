//
// Created by aislam6 on 11/11/19.
//

#ifndef YCSB_STOREDSBENCH_STOREDS_DB_CLIENT_H
#define YCSB_STOREDSBENCH_STOREDS_DB_CLIENT_H

namespace ycsbc {

    class StoredsDbClient {
    public:
        virtual int init(const char *path) {}

        virtual int read(const char *key, void *&result) {}

        virtual int update(const char *key, void *value) {}

        virtual int insert(const char *key, void *value) {}

        virtual void destroy() {}
    };
} // ycsbc

#endif //YCSB_STOREDSBENCH_STOREDS_DB_CLIENT_H