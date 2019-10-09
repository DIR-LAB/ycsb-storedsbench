//
//  storeds_db.cc
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
        char *dummy;
        storeds_.Read(key.c_str(), dummy);
        result.push_back(std::make_pair(key, dummy));
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
            char *val = new char[field_pair.second.length() + 1];
            strcpy(val, field_pair.second.c_str());
            //printf("%s %s\n", key.c_str(), val);
            storeds_.Insert(key.c_str(), val);
        }
        return DB::kOK;
    }

    int StoredsDB::Update(const string &table, const string &key,
                          vector <KVPair> &values) {
        //we just ignore table and key as they are not useful in our KV setting
        for (KVPair &field_pair : values) {
            char *val = new char[field_pair.second.length() + 1];
            strcpy(val, field_pair.second.c_str());
            storeds_.Insert(key.c_str(), val);
        }
        return DB::kOK;
    }

    int StoredsDB::Delete(const std::string &table, const std::string &key) {
        throw "Delete: function not implemented!";
    }

    void StoredsDB::Close() {}

    StoredsDB::~StoredsDB() {
        storeds_.Free();
    }

} // namespace ycsbc
