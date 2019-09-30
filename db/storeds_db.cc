//
//  redis_db.cc
//  YCSB-C
//

#include "storeds_db.h"

#include <cstring>

using namespace std;

namespace ycsbc {
    // all storedsDBs are key-value databases. There is no table, no fields.
    int StoredsDB::Read(const string &table, const string &key,
                        const vector <string> *fields,
                        vector <KVPair> &result) {
        result.clear();
        const char *value = storeds_->Get(key.c_str());
        result.push_back(std::make_pair(key, value));
        return DB::kOK;
    }

    int StoredsDB::Scan(const string &table, const string &key,
                        int len, const vector <string> *fields,
                        vector <vector<KVPair>> &result) {
        throw "Scan: function not implemented!";
    }

    int StoredsDB::Insert(const std::string &table, const std::string &key,
                          std::vector <KVPair> &values) {
        //we just ignore table and key as they are not useful in our KV setting
        for (KVPair &field_pair : values) {
            const char * val = CopyString(field_pair.second.c_str());
            storeds_.Insert(key.first.c_str(), val);
        }
        return DB::kOK;
    }

    int StoredsDB::Update(const string &table, const string &key,
                          vector <KVPair> &values) {
        //we just ignore table and key as they are not useful in our KV setting
        for (KVPair &field_pair : values) {
            const char * val = CopyString(field_pair.second.c_str());
            storeds_.Insert(key.first.c_str(), val);
        }
        return DB::kOK;
    }

    int StoredsDB::Delete(const std::string &table, const std::string &key) {
        throw "Delete: function not implemented!";
    }

} // namespace ycsbc
