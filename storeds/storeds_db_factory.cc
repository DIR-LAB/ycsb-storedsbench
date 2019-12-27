//
// Created by aislam6 on 11/11/19.
//

#include<string.h>

#include "storeds_db_factory.h"
#include "btree/btree_dram.cc"
#include "btree/btree_pmem.cc"
#include "btree/btree_pmem_tx.cc"
#include "btree/btree_dram_concurrent_lock.cc"
#include "btree/btree_pmem_concurrent_lock.cc"
#include "btree/btree_pmem_tx_concurrent_lock.cc"
#include "bplustree/bplustree_dram.cc"
#include "bplustree/bplustree_pmem.cc"
#include "bplustree/bplustree_pmem_tx.cc"
#include "bplustree/bplustree_vmem.cc"
#include "bplustree/bplustree_dram_concurrent_lock.cc"
#include "bplustree/bplustree_pmem_concurrent_lock.cc"
#include "bplustree/bplustree_pmem_tx_concurrent_lock.cc"
#include "hashmap/ht_dram.cc"
#include "hashmap/ht_pmem.cc"
#include "hashmap/ht_pmem_tx.cc"
#include "hashmap/ht_dram_concurrent_lock.cc"
#include "hashmap/ht_pmem_concurrent_lock.cc"
#include "hashmap/ht_pmem_tx_concurrent_lock.cc"
#include "skiplist/skiplist_dram.cc"
#include "skiplist/skiplist_pmem.cc"
#include "skiplist/skiplist_pmem_tx.cc"
#include "skiplist/skiplist_dram_concurrent_lock.cc"
#include "skiplist/skiplist_pmem_concurrent_lock.cc"
#include "skiplist/skiplist_pmem_tx_concurrent_lock.cc"
#include "linkedlist/linkedlist_dram.cc"
#include "linkedlist/linkedlist_pmem.cc"
#include "linkedlist/linkedlist_pmem_tx.cc"
#include "array/array_dram.cc"
#include "array/array_pmem.cc"
#include "array/array_pmem_tx.cc"
#include "array/array_vmem.cc"
#include "array/array_dram_concurrent_lock.cc"
#include "array/array_pmem_concurrent_lock.cc"
#include "array/array_pmem_tx_concurrent_lock.cc"
#include "rbtree/rbtree_dram.cc"
#include "rbtree/rbtree_pmem.cc"
#include "rbtree/rbtree_pmem_tx.cc"
#include "rbtree/rbtree_vmem.cc"
#include "rbtree/rbtree_dram_concurrent_lock.cc"
#include "rbtree/rbtree_pmem_concurrent_lock.cc"
#include "rbtree/rbtree_pmem_tx_concurrent_lock.cc"

using ycsbc::StoredsDbFactory;
using ycsbc::StoredsBase;

StoredsBase *StoredsDbFactory::GetDB(const char *type, const char *path) {
    if (strcmp(type, "array-dram") == 0) {
        return new ArrayDram(path);
    } else if (strcmp(type, "array-pmem") == 0) {
        return new ArrayPmem(path);
    } else if (strcmp(type, "array-pmem-tx") == 0) {
        return new ArrayPmemTx(path);
    } else if (strcmp(type, "array-vmem") == 0) {
        return new ArrayVmem(path);
    } else if (strcmp(type, "array-dram-conc-lock") == 0) {
        return new ArrayDramConcurrentLock(path);
    } else if (strcmp(type, "array-pmem-conc-lock") == 0) {
        return new ArrayPmemConcurrentLock(path);
    } else if (strcmp(type, "array-pmem-tx-conc-lock") == 0) {
        return new ArrayPmemTxConcurrentLock(path);
    } else if (strcmp(type, "btree-dram") == 0) {
        return new BTreeDram(path);
    } else if (strcmp(type, "btree-pmem") == 0) {
        return new BTreePmem(path);
    } else if (strcmp(type, "btree-pmem-tx") == 0) {
        return new BTreePmemTx(path);
    } else if (strcmp(type, "btree-dram-conc-lock") == 0) {
        return new BTreeDramConcurrentLock(path);
    } else if (strcmp(type, "btree-pmem-conc-lock") == 0) {
        return new BTreePmemConcurrentLock(path);
    } else if (strcmp(type, "btree-pmem-tx-conc-lock") == 0) {
        return new BTreePmemTxConcurrentLock(path);
    } else if (strcmp(type, "bplustree-dram") == 0) {
        return new BPlusTreeDram(path);
    } else if (strcmp(type, "bplustree-pmem") == 0) {
        return new BPlusTreePmem(path);
    } else if (strcmp(type, "bplustree-pmem-tx") == 0) {
        return new BPlusTreePmemTx(path);
    } else if (strcmp(type, "bplustree-vmem") == 0) {
        return new BPlusTreeVmem(path);
    } else if (strcmp(type, "bplustree-dram-conc-lock") == 0) {
        return new BPlusTreeDramConcurrentLock(path);
    } else if (strcmp(type, "bplustree-pmem-conc-lock") == 0) {
        return new BPlusTreePmemConcurrentLock(path);
    } else if (strcmp(type, "bplustree-pmem-tx-conc-lock") == 0) {
        return new BPlusTreePmemTxConcurrentLock(path);
    } else if (strcmp(type, "hashtable-dram") == 0) {
        return new HtDram(path);
    } else if (strcmp(type, "hashtable-pmem") == 0) {
        return new HtPmem(path);
    } else if (strcmp(type, "hashtable-pmem-tx") == 0) {
        return new HtPmemTx(path);
    } else if (strcmp(type, "hashtable-dram-conc-lock") == 0) {
        return new HtDramConcurrentLock(path);
    } else if (strcmp(type, "hashtable-pmem-conc-lock") == 0) {
        return new HtPmemConcurrentLock(path);
    } else if (strcmp(type, "hashtable-pmem-tx-conc-lock") == 0) {
        return new HtPmemTxConcurrentLock(path);
    } else if (strcmp(type, "skiplist-dram") == 0) {
        return new SkiplistDram(path);
    } else if (strcmp(type, "skiplist-pmem") == 0) {
        return new SkiplistPmem(path);
    } else if (strcmp(type, "skiplist-pmem-tx") == 0) {
        return new SkiplistPmemTx(path);
    } else if (strcmp(type, "skiplist-dram-conc-lock") == 0) {
        return new SkiplistDramConcurrentLock(path);
    } else if (strcmp(type, "skiplist-pmem-conc-lock") == 0) {
        return new SkiplistPmemConcurrentLock(path);
    } else if (strcmp(type, "skiplist-pmem-tx-conc-lock") == 0) {
        return new SkiplistPmemTxConcurrentLock(path);
    } else if (strcmp(type, "linkedlist-dram") == 0) {
        return new LinkedlistDram(path);
    } else if (strcmp(type, "linkedlist-pmem") == 0) {
        return new LinkedlistPmem(path);
    } else if (strcmp(type, "linkedlist-pmem-tx") == 0) {
        return new LinkedlistPmemTx(path);
    } else if (strcmp(type, "rbtree-dram") == 0) {
        return new RbtreeDram(path);
    } else if (strcmp(type, "rbtree-pmem") == 0) {
        return new RbtreePmem(path);
    } else if (strcmp(type, "rbtree-pmem-tx") == 0) {
        return new RbtreePmemTx(path);
    } else if (strcmp(type, "rbtree-vmem") == 0) {
        return new RbtreeVmem(path);
    } else if (strcmp(type, "rbtree-dram-conc-lock") == 0) {
        return new RbtreeDramConcurrentLock(path);
    } else if (strcmp(type, "rbtree-pmem-conc-lock") == 0) {
        return new RbtreePmemConcurrentLock(path);
    } else if (strcmp(type, "rbtree-pmem-tx-conc-lock") == 0) {
        return new RbtreePmemTxConcurrentLock(path);
    } else {
        return NULL;
    }
}