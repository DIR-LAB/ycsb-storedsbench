//
// Created by aislam6 on 11/11/19.
//

#include<string.h>

#include "storeds_db_factory.h"
#include "btree/btree_dram.cc"
#include "btree/btree_pmem.cc"
#include "btree/btree_pmem_tx.cc"
#include "hashmap/ht_dram.cc"
#include "hashmap/ht_pmem.cc"
#include "hashmap/ht_pmem_tx.cc"
#include "skiplist/skiplist_dram.cc"
#include "skiplist/skiplist_pmem.cc"
#include "skiplist/skiplist_pmem_tx.cc"
#include "linkedlist/linkedlist_dram.cc"
#include "linkedlist/linkedlist_pmem.cc"
#include "linkedlist/linkedlist_pmem_tx.cc"
#include "array/array_dram.cc"
#include "array/array_pmem.cc"
#include "array/array_pmem_tx.cc"

using ycsbc::StoredsDbFactory;
using ycsbc::StoredsBase;

StoredsBase *StoredsDbFactory::GetDB(const char *type, const char *path) {
    if (strcmp(type, "array-dram") == 0) {
        return new ArrayDram(path);
    } else if (strcmp(type, "array-pmem") == 0) {
        return new ArrayPmem(path);
    } else if (strcmp(type, "array-pmem-tx") == 0) {
        return new ArrayPmemTx(path);
    } else if (strcmp(type, "btree-dram") == 0) {
        return new BTreeDram(path);
    } else if (strcmp(type, "btree-pmem") == 0) {
        return new BTreePmem(path);
    } else if (strcmp(type, "btree-pmem-tx") == 0) {
        return new BTreePmemTx(path);
    } else if (strcmp(type, "hashtable-dram") == 0) {
        return new HtDram(path);
    } else if (strcmp(type, "hashtable-pmem") == 0) {
        return new HtPmem(path);
    } else if (strcmp(type, "hashtable-pmem-tx") == 0) {
        return new HtPmemTx(path);
    } else if (strcmp(type, "skiplist-dram") == 0) {
        return new SkiplistDram(path);
    } else if (strcmp(type, "skiplist-pmem") == 0) {
        return new SkiplistPmem(path);
    } else if (strcmp(type, "skiplist-pmem-tx") == 0) {
        return new SkiplistPmemTx(path);
    } else if (strcmp(type, "linkedlist-dram") == 0) {
        return new LinkedlistDram(path);
    } else if (strcmp(type, "linkedlist-pmem") == 0) {
        return new LinkedlistPmem(path);
    } else if (strcmp(type, "linkedlist-pmem-tx") == 0) {
        return new LinkedlistPmemTx(path);
    } else {
        return NULL;
    }
}