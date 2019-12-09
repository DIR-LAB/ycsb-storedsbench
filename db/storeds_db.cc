//
//  storeds_db.cc
//  YCSB-C
//

#include "storeds_db.h"

#include <cstring>
#include <iostream>

using namespace std;

namespace ycsbc {
    // all storedsDBs are key-value databases. There is no table, no fields.
    int StoredsDB::Read(const string &table, const uint64_t &key,
                        const vector <string> *fields,
                        vector <Kuint64VstrPair> &result) {
        result.clear();
        char *dummy;
        storeds_->read(key, (void *&) dummy);
        result.push_back(std::make_pair(key, dummy));
        return DB::kOK;
    }

    int StoredsDB::Scan(const string &table, const uint64_t &key,
                        int len, const vector <string> *fields,
                        vector <vector<Kuint64VstrPair>> &result) {
        storeds_->scan(key, len, result);
        return DB::kOK;
    }

    int StoredsDB::Insert(const std::string &table, const uint64_t &key,
                          std::vector <KVPair> &values) {
        //we just ignore table and key as they are not useful in our KV setting
        for (KVPair &field_pair : values) {
            char *val = new char[field_pair.second.length() + 1];
            strcpy(val, field_pair.second.c_str());
            //printf("[INSERT] %s %s\n", key.c_str(), val);
            storeds_->insert(key, val);
        }
        return DB::kOK;
    }

    int StoredsDB::Update(const string &table, const uint64_t &key,
                          vector <KVPair> &values) {
        //we just ignore table and key as they are not useful in our KV setting
        for (KVPair &field_pair : values) {
            char *val = new char[field_pair.second.length() + 1];
            strcpy(val, field_pair.second.c_str());
            //printf("[UPDATE] %s %s\n", key.c_str(), val);
            storeds_->update(key, val);
        }
        return DB::kOK;
    }

    int StoredsDB::Delete(const std::string &table, const uint64_t &key) {
        throw "Delete: function not implemented!";
    }

    void StoredsDB::Close() {}

    StoredsDB::~StoredsDB() {
        storeds_->destroy();
    }

} // namespace ycsbc
