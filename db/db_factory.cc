//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db_factory.h"

#include <string>
#include "basic_db.h"
#include "lock_stl_db.h"
#include "tbb_rand_db.h"
#include "tbb_scan_db.h"
#include "storeds_db.h"

using namespace std;
using ycsbc::DB;
using ycsbc::DBFactory;

DB *DBFactory::CreateDB(utils::Properties &props) {
    if (props["dbname"] == "basic") {
        return new BasicDB;
    } else if (props["dbname"] == "lock_stl") {
        return new LockStlDB;
    } /*else if (props["dbname"] == "redis") {
        int port = stoi(props["port"]);
        int slaves = stoi(props["slaves"]);
        return new RedisDB(props["host"].c_str(), port, slaves);
    }*/ else if (props["dbname"] == "tbb_rand") {
        return new TbbRandDB;
    } else if (props["dbname"] == "tbb_scan") {
        return new TbbScanDB;
    } else if (props["dbname"] == "storeds") {
        return new StoredsDB(props.GetProperty("type", "array-dram").c_str(),
                props.GetProperty("dbpath", "/pmem/array.pmem").c_str());
    } else {
        return NULL;
    }
}

