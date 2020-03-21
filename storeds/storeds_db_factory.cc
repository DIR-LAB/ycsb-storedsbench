//
// Created by aislam6 on 11/11/19.
//

#include<string.h>

#include "storeds_db_factory.h"
#include "btree_mem_opt/bt_dram.cc"
#include "btree_mem_opt/bt_pmem.cc"
#include "btree_mem_opt/bt_pmem_tx.cc"
#include "btree_mem_opt/bt_vmem.cc"
#include "btree_mem_opt/bt_dram_concurrent_mlock.cc"
#include "btree_mem_opt/bt_pmem_concurrent_mlock.cc"
#include "btree_mem_opt/bt_pmem_tx_concurrent_mlock.cc"
#include "btree_mem_opt/bt_vmem_concurrent_mlock.cc"
#include "btree/btree_dram.cc"
#include "btree/btree_pmem.cc"
#include "btree/btree_pmem_tx.cc"
#include "btree/btree_vmem.cc"
#include "btree/btree_vmem_concurrent_mlock.cc"
#include "btree/btree_dram_concurrent_lock.cc"
#include "btree/btree_dram_concurrent_mlock.cc"
#include "btree/btree_pmem_concurrent_lock.cc"
#include "btree/btree_pmem_concurrent_mlock.cc"
#include "btree/btree_pmem_tx_concurrent_lock.cc"
#include "btree/btree_pmem_tx_concurrent_mlock.cc"
#include "bplustree/bplustree_dram.cc"
#include "bplustree/bplustree_pmem.cc"
#include "bplustree/bplustree_pmem_tx.cc"
#include "bplustree/bplustree_vmem.cc"
#include "bplustree/bplustree_vmem_concurrent_mlock.cc"
#include "bplustree/bplustree_dram_concurrent_lock.cc"
#include "bplustree/bplustree_dram_concurrent_mlock.cc"
#include "bplustree/bplustree_pmem_concurrent_lock.cc"
#include "bplustree/bplustree_pmem_concurrent_mlock.cc"
#include "bplustree/bplustree_pmem_tx_concurrent_lock.cc"
#include "bplustree/bplustree_pmem_tx_concurrent_mlock.cc"
#include "bplustree_mem_opt/bp_dram.cc"
#include "bplustree_mem_opt/bp_pmem.cc"
#include "bplustree_mem_opt/bp_pmem_tx.cc"
#include "bplustree_mem_opt/bp_vmem.cc"
#include "bplustree_mem_opt/bp_dram_concurrent_mlock.cc"
#include "bplustree_mem_opt/bp_pmem_concurrent_mlock.cc"
#include "bplustree_mem_opt/bp_pmem_tx_concurrent_mlock.cc"
#include "bplustree_mem_opt/bp_vmem_concurrent_mlock.cc"
#include "hashmap/ht_dram.cc"
#include "hashmap/ht_pmem.cc"
#include "hashmap/ht_pmem_tx.cc"
#include "hashmap/ht_vmem.cc"
#include "hashmap/ht_vmem_concurrent_mlock.cc"
#include "hashmap/ht_dram_concurrent_lock.cc"
#include "hashmap/ht_dram_concurrent_mlock.cc"
#include "hashmap/ht_pmem_concurrent_lock.cc"
#include "hashmap/ht_pmem_concurrent_mlock.cc"
#include "hashmap/ht_pmem_tx_concurrent_lock.cc"
#include "hashmap/ht_pmem_tx_concurrent_mlock.cc"
#include "skiplist/skiplist_dram.cc"
#include "skiplist/skiplist_pmem.cc"
#include "skiplist/skiplist_pmem_tx.cc"
#include "skiplist/skiplist_vmem.cc"
#include "skiplist/skiplist_vmem_concurrent_mlock.cc"
#include "skiplist/skiplist_dram_concurrent_lock.cc"
#include "skiplist/skiplist_dram_concurrent_mlock.cc"
#include "skiplist/skiplist_pmem_concurrent_lock.cc"
#include "skiplist/skiplist_pmem_concurrent_mlock.cc"
#include "skiplist/skiplist_pmem_tx_concurrent_lock.cc"
#include "skiplist/skiplist_pmem_tx_concurrent_mlock.cc"
#include "linkedlist/linkedlist_dram.cc"
#include "linkedlist/linkedlist_dram_concurrent_mlock.cc"
#include "linkedlist/linkedlist_pmem.cc"
#include "linkedlist/linkedlist_pmem_concurrent_mlock.cc"
#include "linkedlist/linkedlist_pmem_tx.cc"
#include "linkedlist/linkedlist_pmem_tx_concurrent_mlock.cc"
#include "linkedlist/linkedlist_vmem.cc"
#include "linkedlist/linkedlist_vmem_concurrent_mlock.cc"
#include "array/array_dram.cc"
#include "array/array_pmem.cc"
#include "array/array_pmem_tx.cc"
#include "array/array_vmem.cc"
#include "array/array_vmem_concurrent_mlock.cc"
#include "array/array_vmem_concurrent_pmlock.cc"
#include "array/array_dram_concurrent_lock.cc"
#include "array/array_dram_concurrent_mlock.cc"
#include "array/array_pmem_concurrent_lock.cc"
#include "array/array_pmem_concurrent_mlock.cc"
#include "array/array_pmem_concurrent_vmlock.cc"
#include "array/array_pmem_tx_concurrent_lock.cc"
#include "array/array_pmem_tx_concurrent_mlock.cc"
#include "rbtree/rbtree_dram.cc"
#include "rbtree/rbtree_pmem.cc"
#include "rbtree/rbtree_pmem_tx.cc"
#include "rbtree/rbtree_vmem.cc"
#include "rbtree/rbtree_vmem_concurrent_mlock.cc"
#include "rbtree/rbtree_vmem_concurrent_pmlock.cc"
#include "rbtree/rbtree_dram_concurrent_lock.cc"
#include "rbtree/rbtree_dram_concurrent_mlock.cc"
#include "rbtree/rbtree_pmem_concurrent_lock.cc"
#include "rbtree/rbtree_pmem_concurrent_mlock.cc"
#include "rbtree/rbtree_pmem_concurrent_vmlock.cc"
#include "rbtree/rbtree_pmem_tx_concurrent_lock.cc"
#include "rbtree/rbtree_pmem_tx_concurrent_mlock.cc"
#include "hashlist/hl_dram.cc"
#include "hashlist/hl_vmem.cc"
#include "hashlist/hl_pmem.cc"
#include "hashlist/hl_pmem_tx.cc"
#include "hashlist/hl_dram_conc_mlock.cc"
#include "hashlist/hl_vmem_conc_mlock.cc"
#include "hashlist/hl_vmem_conc_pmlock.cc"
#include "hashlist/hl_pmem_conc_mlock.cc"
#include "hashlist/hl_pmem_conc_vmlock.cc"
#include "hashlist/hl_pmem_tx_conc_mlock.cc"
#include "threemisses/threemisses_pmem.cc"

using ycsbc::StoredsDbFactory;
using ycsbc::StoredsBase;

StoredsBase *StoredsDbFactory::GetDB(const char *type, const char *path) {
    if (strcmp(type, "threemiss") == 0) {
        return new ThreeMissesPmem(path);
    }
    /*if (strcmp(type, "array-dram") == 0) {
        return new ArrayDram(path);
    } else if (strcmp(type, "array-pmem") == 0) {
        return new ArrayPmem(path);
    } else if (strcmp(type, "array-pmem-tx") == 0) {
        return new ArrayPmemTx(path);
    } else if (strcmp(type, "array-vmem") == 0) {
        return new ArrayVmem(path);
    } else if (strcmp(type, "array-vmem-conc-mlock") == 0) {
        return new ArrayVmemConcurrentMLock(path);
    } else if (strcmp(type, "array-vmem-conc-pmlock") == 0) {
        return new ArrayVmemConcurrentPMLock(path);
    } else if (strcmp(type, "array-dram-conc-lock") == 0) {
        return new ArrayDramConcurrentLock(path);
    } else if (strcmp(type, "array-dram-conc-mlock") == 0) {
        return new ArrayDramConcurrentMLock(path);
    } else if (strcmp(type, "array-pmem-conc-lock") == 0) {
        return new ArrayPmemConcurrentLock(path);
    } else if (strcmp(type, "array-pmem-conc-mlock") == 0) {
        return new ArrayPmemConcurrentMLock(path);
    } else if (strcmp(type, "array-pmem-conc-vmlock") == 0) {
        return new ArrayPmemConcurrentVMLock(path);
    } else if (strcmp(type, "array-pmem-tx-conc-lock") == 0) {
        return new ArrayPmemTxConcurrentLock(path);
    } else if (strcmp(type, "array-pmem-tx-conc-mlock") == 0) {
        return new ArrayPmemTxConcurrentMLock(path);
    } else if (strcmp(type, "bt-dram") == 0) {
        return new BTDram(path);
    } else if (strcmp(type, "bt-pmem") == 0) {
        return new BTPmem(path);
    } else if (strcmp(type, "bt-pmem-tx") == 0) {
        return new BTPmemTx(path);
    } else if (strcmp(type, "bt-vmem") == 0) {
        return new BTVmem(path);
    } else if (strcmp(type, "bt-dram-conc-mlock") == 0) {
        return new BTDramConcurrentMLock(path);
    } else if (strcmp(type, "bt-pmem-conc-mlock") == 0) {
        return new BTPmemConcurrentMLock(path);
    } else if (strcmp(type, "bt-pmem-tx-conc-mlock") == 0) {
        return new BTPmemTxConcurrentMLock(path);
    } else if (strcmp(type, "bt-vmem-conc-mlock") == 0) {
        return new BTVmemConcurrentMLock(path);
    } else if (strcmp(type, "btree-dram") == 0) {
        return new BTreeDram(path);
    } else if (strcmp(type, "btree-pmem") == 0) {
        return new BTreePmem(path);
    } else if (strcmp(type, "btree-pmem-tx") == 0) {
        return new BTreePmemTx(path);
    } else if (strcmp(type, "btree-vmem") == 0) {
        return new BTreeVmem(path);
    } else if (strcmp(type, "btree-memkind") == 0) {
        return new BTreeMemkind(path);
    } else if (strcmp(type, "btree-vmem-conc-mlock") == 0) {
        return new BTreeVmemConcurrentMLock(path);
    } else if (strcmp(type, "btree-dram-conc-lock") == 0) {
        return new BTreeDramConcurrentLock(path);
    } else if (strcmp(type, "btree-dram-conc-mlock") == 0) {
        return new BTreeDramConcurrentMLock(path);
    } else if (strcmp(type, "btree-pmem-conc-lock") == 0) {
        return new BTreePmemConcurrentLock(path);
    } else if (strcmp(type, "btree-pmem-conc-mlock") == 0) {
        return new BTreePmemConcurrentMLock(path);
    } else if (strcmp(type, "btree-pmem-tx-conc-lock") == 0) {
        return new BTreePmemTxConcurrentLock(path);
    } else if (strcmp(type, "btree-pmem-tx-conc-mlock") == 0) {
        return new BTreePmemTxConcurrentMLock(path);
    } else if (strcmp(type, "bp-dram") == 0) {
        return new BPDram(path);
    } else if (strcmp(type, "bp-pmem") == 0) {
        return new BPPmem(path);
    } else if (strcmp(type, "bp-pmem-tx") == 0) {
        return new BPPmemTx(path);
    } else if (strcmp(type, "bp-vmem") == 0) {
        return new BPVmem(path);
    } else if (strcmp(type, "bp-dram-conc-mlock") == 0) {
        return new BPDramConcurrentMLock(path);
    } else if (strcmp(type, "bp-pmem-conc-mlock") == 0) {
        return new BPPmemConcurrentMLock(path);
    } else if (strcmp(type, "bp-pmem-tx-conc-mlock") == 0) {
        return new BPPmemTxConcurrentMLock(path);
    } else if (strcmp(type, "bp-vmem-conc-mlock") == 0) {
        return new BPVmemConcurrentMLock(path);
    } else if (strcmp(type, "bplustree-dram") == 0) {
        return new BPlusTreeDram(path);
    } else if (strcmp(type, "bplustree-pmem") == 0) {
        return new BPlusTreePmem(path);
    } else if (strcmp(type, "bplustree-pmem-tx") == 0) {
        return new BPlusTreePmemTx(path);
    } else if (strcmp(type, "bplustree-vmem") == 0) {
        return new BPlusTreeVmem(path);
    } else if (strcmp(type, "bplustree-vmem-conc-mlock") == 0) {
        return new BPlusTreeVmemConcurrentMLock(path);
    } else if (strcmp(type, "bplustree-dram-conc-lock") == 0) {
        return new BPlusTreeDramConcurrentLock(path);
    } else if (strcmp(type, "bplustree-dram-conc-mlock") == 0) {
        return new BPlusTreeDramConcurrentMLock(path);
    } else if (strcmp(type, "bplustree-pmem-conc-lock") == 0) {
        return new BPlusTreePmemConcurrentLock(path);
    } else if (strcmp(type, "bplustree-pmem-conc-mlock") == 0) {
        return new BPlusTreePmemConcurrentMLock(path);
    } else if (strcmp(type, "bplustree-pmem-tx-conc-lock") == 0) {
        return new BPlusTreePmemTxConcurrentLock(path);
    } else if (strcmp(type, "bplustree-pmem-tx-conc-mlock") == 0) {
        return new BPlusTreePmemTxConcurrentMLock(path);
    } else if (strcmp(type, "hashtable-dram") == 0) {
        return new HtDram(path);
    } else if (strcmp(type, "hashtable-pmem") == 0) {
        return new HtPmem(path);
    } else if (strcmp(type, "hashtable-pmem-tx") == 0) {
        return new HtPmemTx(path);
    } else if (strcmp(type, "hashtable-vmem") == 0) {
        return new HtVmem(path);
    } else if (strcmp(type, "hashtable-vmem-conc-mlock") == 0) {
        return new HtVmemConcurrentMLock(path);
    } else if (strcmp(type, "hashtable-dram-conc-lock") == 0) {
        return new HtDramConcurrentLock(path);
    } else if (strcmp(type, "hashtable-dram-conc-mlock") == 0) {
        return new HtDramConcurrentMLock(path);
    } else if (strcmp(type, "hashtable-pmem-conc-lock") == 0) {
        return new HtPmemConcurrentLock(path);
    } else if (strcmp(type, "hashtable-pmem-conc-mlock") == 0) {
        return new HtPmemConcurrentMLock(path);
    } else if (strcmp(type, "hashtable-pmem-tx-conc-lock") == 0) {
        return new HtPmemTxConcurrentLock(path);
    } else if (strcmp(type, "hashtable-pmem-tx-conc-mlock") == 0) {
        return new HtPmemTxConcurrentMLock(path);
    } else if (strcmp(type, "skiplist-dram") == 0) {
        return new SkiplistDram(path);
    } else if (strcmp(type, "skiplist-pmem") == 0) {
        return new SkiplistPmem(path);
    } else if (strcmp(type, "skiplist-pmem-tx") == 0) {
        return new SkiplistPmemTx(path);
    } else if (strcmp(type, "skiplist-vmem") == 0) {
        return new SkiplistVmem(path);
    } else if (strcmp(type, "skiplist-vmem-conc-mlock") == 0) {
        return new SkiplistVmemConcurrentMLock(path);
    } else if (strcmp(type, "skiplist-dram-conc-lock") == 0) {
        return new SkiplistDramConcurrentLock(path);
    } else if (strcmp(type, "skiplist-dram-conc-mlock") == 0) {
        return new SkiplistDramConcurrentMLock(path);
    } else if (strcmp(type, "skiplist-pmem-conc-lock") == 0) {
        return new SkiplistPmemConcurrentLock(path);
    } else if (strcmp(type, "skiplist-pmem-conc-mlock") == 0) {
        return new SkiplistPmemConcurrentMLock(path);
    } else if (strcmp(type, "skiplist-pmem-tx-conc-lock") == 0) {
        return new SkiplistPmemTxConcurrentLock(path);
    } else if (strcmp(type, "skiplist-pmem-tx-conc-mlock") == 0) {
        return new SkiplistPmemTxConcurrentMLock(path);
    } else if (strcmp(type, "linkedlist-dram") == 0) {
        return new LinkedlistDram(path);
    } else if (strcmp(type, "linkedlist-dram-conc-mlock") == 0) {
        return new LinkedlistDramConcurrentMLock(path);
    } else if (strcmp(type, "linkedlist-pmem") == 0) {
        return new LinkedlistPmem(path);
    } else if (strcmp(type, "linkedlist-pmem-conc-mlock") == 0) {
        return new LinkedlistPmemConcurrentMLock(path);
    } else if (strcmp(type, "linkedlist-pmem-tx") == 0) {
        return new LinkedlistPmemTx(path);
    } else if (strcmp(type, "linkedlist-pmem-tx-conc-mlock") == 0) {
        return new LinkedlistPmemTxConcurrentMLock(path);
    } else if (strcmp(type, "linkedlist-vmem") == 0) {
        return new LinkedlistVmem(path);
    } else if (strcmp(type, "linkedlist-vmem-conc-mlock") == 0) {
        return new LinkedlistVmemConcurrentMLock(path);
    } else if (strcmp(type, "rbtree-dram") == 0) {
        return new RbtreeDram(path);
    } else if (strcmp(type, "rbtree-pmem") == 0) {
        return new RbtreePmem(path);
    } else if (strcmp(type, "rbtree-pmem-tx") == 0) {
        return new RbtreePmemTx(path);
    } else if (strcmp(type, "rbtree-vmem") == 0) {
        return new RbtreeVmem(path);
    } else if (strcmp(type, "rbtree-vmem-conc-mlock") == 0) {
        return new RbtreeVmemConcurrentMLock(path);
    } else if (strcmp(type, "rbtree-vmem-conc-pmlock") == 0) {
        return new RbtreeVmemConcurrentPMLock(path);
    } else if (strcmp(type, "rbtree-dram-conc-lock") == 0) {
        return new RbtreeDramConcurrentLock(path);
    } else if (strcmp(type, "rbtree-dram-conc-mlock") == 0) {
        return new RbtreeDramConcurrentMLock(path);
    } else if (strcmp(type, "rbtree-pmem-conc-lock") == 0) {
        return new RbtreePmemConcurrentLock(path);
    } else if (strcmp(type, "rbtree-pmem-conc-mlock") == 0) {
        return new RbtreePmemConcurrentMLock(path);
    } else if (strcmp(type, "rbtree-pmem-conc-vmlock") == 0) {
        return new RbtreePmemConcurrentVMLock(path);
    } else if (strcmp(type, "rbtree-pmem-tx-conc-lock") == 0) {
        return new RbtreePmemTxConcurrentLock(path);
    } else if (strcmp(type, "rbtree-pmem-tx-conc-mlock") == 0) {
        return new RbtreePmemTxConcurrentMLock(path);
    } else if (strcmp(type, "hl-dram") == 0) {
        return new HlDram(path);
    } else if (strcmp(type, "hl-vmem") == 0) {
        return new HlVmem(path);
    } else if (strcmp(type, "hl-pmem") == 0) {
        return new HlPmem(path);
    } else if (strcmp(type, "hl-pmem-tx") == 0) {
        return new HlPmemTx(path);
    } else if (strcmp(type, "hl-dram-conc-mlock") == 0) {
        return new HlDramConcurrentMLock(path);
    } else if (strcmp(type, "hl-vmem-conc-mlock") == 0) {
        return new HlVmemConcurrentMLock(path);
    } else if (strcmp(type, "hl-vmem-conc-pmlock") == 0) {
        return new HlVmemConcurrentPMLock(path);
    } else if (strcmp(type, "hl-pmem-conc-mlock") == 0) {
        return new HlPmemConcurrentMLock(path);
    } else if (strcmp(type, "hl-pmem-conc-vmlock") == 0) {
        return new HlPmemConcurrentVMLock(path);
    } else if (strcmp(type, "hl-pmem-tx-conc-mlock") == 0) {
        return new HlPmemTxConcurrentMLock(path);
    }*/ else {
        return NULL;
    }
}