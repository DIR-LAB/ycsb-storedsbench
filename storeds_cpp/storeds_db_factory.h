//
// Created by aislam6 on 11/11/19.
//

#ifndef YCSB_STOREDSBENCH_STOREDS_DB_FACTORY_H
#define YCSB_STOREDSBENCH_STOREDS_DB_FACTORY_H

#include "storeds_db_client.h"

namespace ycsbc {

    class StoredsDbFactory {
    public:
        static StoredsDbClient *GetDB(const char *type, const char *path);
    };

} // ycsbc

#endif //YCSB_STOREDSBENCH_STOREDS_DB_FACTORY_H
