//
//  ycsbc.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/19/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include <cstring>
#include <string>
#include <iostream>
#include <vector>
#include <future>
#include "core/utils.h"
#include "core/timer.h"
#include "core/client.h"
#include "core/core_workload.h"
#include "db/db_factory.h"

using namespace std;

#define MAX_VAL 100

void TestUsageMessage(const char *command);

bool TestStrStartWith(const char *str, const char *pre);

string TestParseCommandLine(int argc, const char *argv[], utils::Properties &props);

int main(const int argc, const char *argv[]) {
    utils::Properties props;
    string file_name = TestParseCommandLine(argc, argv, props);

    ycsbc::DB *db = ycsbc::DBFactory::CreateDB(props);
    if (!db) {
        cout << "Unknown database name " << props["dbname"] << endl;
        exit(0);
    }

    ycsbc::CoreWorkload wl;
    wl.Init(props);

    // Loads data
    db->Init();
    std::string tableName = "ycsb_test";
    for(uint64_t key=1; key<=MAX_VAL; key+=1) {
        std::vector <ycsbc::DB::KVPair> values;
        ycsbc::DB::KVPair pair;
        pair.first.append("field");
        pair.second.append(std::to_string(key));
        values.push_back(pair);
        assert(db->Insert(tableName, key, values) == ycsbc::DB::kOK);
    }
    db->Close();

    // Perform read
    db->Init();
    for(uint64_t key=1; key<=MAX_VAL; key+=1) {
        std::vector <ycsbc::DB::Kuint64VstrPair> result;
        assert(db->Read(tableName, key, NULL, result) == ycsbc::DB::kOK);
        //cout << result[0].first << " " << result[0].second << endl;
        assert(std::to_string(result[0].first).compare(result[0].second) == 0);
    }
    db->Close();

    cout << "load passed!" << endl;

    // Update data
    db->Init();
    for(uint64_t key=1; key<=MAX_VAL; key+=1) {
        std::vector <ycsbc::DB::KVPair> values;
        ycsbc::DB::KVPair pair;
        pair.first.append("field");
        pair.second.append(std::to_string(MAX_VAL-key));
        values.push_back(pair);
        assert(db->Update(tableName, key, values) == ycsbc::DB::kOK);
    }
    db->Close();

    // Perform read
    db->Init();
    for(uint64_t key=1; key<=MAX_VAL; key+=1) {
        std::vector <ycsbc::DB::Kuint64VstrPair> result;
        assert(db->Read(tableName, key, NULL, result) == ycsbc::DB::kOK);
        //cout << result[0].first << " " << result[0].second << endl;
        assert(std::to_string(MAX_VAL-key).compare(result[0].second) == 0);
    }

    //perform scan
    /*std::vector <std::vector<ycsbc::DB::Kuint64VstrPair>> result;
    assert(db->Scan(tableName, 1, 200, NULL, result) == ycsbc::DB::kOK);
    for(int i=0; i<result.size(); i+=1) {
        std::vector<ycsbc::DB::Kuint64VstrPair> tmp = result[i];
        cout << tmp[0].first << " " << tmp[0].second << endl;
    }*/

    db->Close();

    cout << "update passed!" << endl;

    if (db) db->~DB();
}

string TestParseCommandLine(int argc, const char *argv[], utils::Properties &props) {
    int argindex = 1;
    string filename;
    while (argindex < argc && TestStrStartWith(argv[argindex], "-")) {
        if (strcmp(argv[argindex], "-threads") == 0) {
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("threadcount", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-db") == 0) {
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("dbname", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-type") == 0) {
            // -type is only with storedsDB, it denotes the inner data structures we will use
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("type", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-dbpath") == 0) {
            // -dbpath is only with storedsDB, it shows where the db file is
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("dbpath", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-host") == 0) {
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("host", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-port") == 0) {
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("port", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-slaves") == 0) {
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            props.SetProperty("slaves", argv[argindex]);
            argindex++;
        } else if (strcmp(argv[argindex], "-P") == 0) {
            argindex++;
            if (argindex >= argc) {
                TestUsageMessage(argv[0]);
                exit(0);
            }
            filename.assign(argv[argindex]);
            ifstream input(argv[argindex]);
            try {
                props.Load(input);
            } catch (const string &message) {
                cout << message << endl;
                exit(0);
            }
            input.close();
            argindex++;
        } else {
            cout << "Unknown option '" << argv[argindex] << "'" << endl;
            exit(0);
        }
    }

    if (argindex == 1 || argindex != argc) {
        TestUsageMessage(argv[0]);
        exit(0);
    }

    return filename;
}

void TestUsageMessage(const char *command) {
    cout << "Usage: " << command << " [options]" << endl;
    cout << "Options:" << endl;
    cout << "  -db dbname: specify the name of the DB to use (default: storeds)" << endl;
    cout << "  -threads n: execute using n threads (default: 1)" << endl;
    cout << "  -dbpath dbpath: specify the name of the PMEM memory pool file to be created" << endl;
    cout << "  -type dsname: specify the name of the data-structure to use" << endl;
    cout << "  -P propertyfile: load properties from the given file. Multiple files can" << endl;
    cout << "                   be specified, and will be processed in the order specified" << endl;
}

inline bool TestStrStartWith(const char *str, const char *pre) {
    return strncmp(str, pre, strlen(pre)) == 0;
}

