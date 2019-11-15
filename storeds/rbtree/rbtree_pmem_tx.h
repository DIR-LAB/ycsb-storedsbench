//
// Created by aislam6 on 10/25/19.
//

#ifndef YCSB_STOREDSBENCH_RBTREE_PMEM_TX_H
#define YCSB_STOREDSBENCH_RBTREE_PMEM_TX_H

int rbtree_pmem_tx_init(const char *path);

int rbtree_pmem_tx_read(const char *key, void *&result);

int rbtree_pmem_tx_update(const char *key, void *value);

int rbtree_pmem_tx_insert(const char *key, void *value);

void rbtree_pmem_tx_free();

#endif //YCSB_STOREDSBENCH_RBTREE_PMEM_TX_H
