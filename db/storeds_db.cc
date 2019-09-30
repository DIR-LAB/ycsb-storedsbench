//
//  redis_db.cc
//  YCSB-C
//

#include "storeds_db.h"

#include <cstring>

using namespace std;

namespace ycsbc {

    int StoredsDB::Read(const string &table, const string &key,
                      const vector<string> *fields,
                      vector<KVPair> &result) {
        return storeds_.Read(key, result);
    }

    int StoredsDB::Update(const string &table, const string &key,
                        vector<KVPair> &values) {
        return storeds_.Update(key, result);
    }

    int StoredsDB::Scan(const std::string &table, const std::string &key,
             int len, const std::vector<std::string> *fields,
             std::vector<std::vector<KVPair>> &result){
        return storeds_.Scan(key, len, result);
    }

    int StoredsDB::Insert(const std::string &table, const std::string &key,
               std::vector<KVPair> &values){
        return storeds_.Insert(key, values);
    }

    int StoredsDB::Delete(const std::string &table, const std::string &key){
        return storeds_.Delete(key);
    }

} // namespace ycsbc
