//
//  basic_db.cc
//  YCSB-C
//
//  Created by Jinglei Ren on 12/17/14.
//  Copyright (c) 2014 Jinglei Ren <jinglei@ren.systems>.
//

#include "db_factory.h"

#include <string>
#include "storeds_db.h"

using namespace std;
using ycsbc::DB;
using ycsbc::DBFactory;

DB *DBFactory::CreateDB(utils::Properties &props) {
    if (props["dbname"] == "storeds") {
        return new StoredsDB(props.GetProperty("type", "array-dram").c_str(),
                props.GetProperty("dbpath", "/pmem/array.pmem").c_str());
    } else {
        return NULL;
    }
}

