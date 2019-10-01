//
//  storeds_db.h
//  YCSB-C
//

#ifndef YCSB_C_STOREDS_DB_H_
#define YCSB_C_STOREDS_DB_H_

#include "core/db.h"

#include <iostream>
#include <string>
#include "core/properties.h"
#include "core/db.h"
#include "storeds/storeds_client.h"

using std::cout;
using std::endl;

namespace ycsbc {

    class StoredsDB : public DB {
    public:
        StoredsDB(const char *type, const char *path) : storeds_(type, path) {
        }

        int Read(const std::string &table, const std::string &key,
                 const std::vector <std::string> *fields,
                 std::vector <KVPair> &result);

        int Scan(const std::string &table, const std::string &key,
                 int len, const std::vector <std::string> *fields,
                 std::vector <std::vector<KVPair>> &result);

        int Update(const std::string &table, const std::string &key,
                   std::vector <KVPair> &values);

        int Insert(const std::string &table, const std::string &key,
                   std::vector <KVPair> &values);

        int Delete(const std::string &table, const std::string &key);

    private:
        StoredsClient storeds_;
    };

} // ycsbc

#endif

