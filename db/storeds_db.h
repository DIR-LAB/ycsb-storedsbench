//
//  storeds_db.h
//  YCSB-C
//

#ifndef YCSB_C_STOREDS_DB_H_
#define YCSB_C_STOREDS_DB_H_

#include "../core/db.h"

#include <iostream>
#include <string>
#include "../core/properties.h"
#include "../storeds/storeds_db_factory.h"
#include "../storeds/storeds_base.h"

using std::cout;
using std::endl;

namespace ycsbc {

    class StoredsDB : public DB {
    public:
        StoredsDB(const char *type, const char *path) {
            storeds_ = StoredsDbFactory::GetDB(type, path);
        }

        int Read(const std::string &table, const uint64_t &key,
                 const std::vector <std::string> *fields,
                 std::vector <Kuint64VstrPair> &result);

        int Scan(const std::string &table, const uint64_t &key,
                 int len, const std::vector <std::string> *fields,
                 std::vector <std::vector<Kuint64VstrPair>> &result);

        int Update(const std::string &table, const uint64_t &key,
                   std::vector <KVPair> &values);

        int Insert(const std::string &table, const uint64_t &key,
                   std::vector <KVPair> &values);

        int Delete(const std::string &table, const uint64_t &key);

        void Close();

        ~StoredsDB();

    private:
        StoredsBase *storeds_;
    };

} // ycsbc

#endif

