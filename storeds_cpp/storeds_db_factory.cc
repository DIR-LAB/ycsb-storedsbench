//
// Created by aislam6 on 11/11/19.
//

#include<string.h>

#include "storeds_db_factory.h"
#include "btree/btree_dram.h"
#include "btree/btree_pmem.cc"
#include "btree/btree_pmem_tx.cc"

using ycsbc::StoredsDbFactory;
using ycsbc::StoredsDbClient;

StoredsDbClient *StoredsDbFactory::GetDB(const char *type, const char *path) {
    if (strcmp(type, "btree-dram") == 0) {
        return new BTreeDram(path);
    } else if (strcmp(type, "btree-pmem") == 0) {
        return new BTreePmem(path);
    } else if (strcmp(type, "btree-pmem-tx") == 0) {
        return new BTreePmemTx(path);
    } else {
        return NULL;
    }
}