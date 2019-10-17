//
// Created by aislam6 on 10/16/19.
//

#ifndef YCSB_STOREDSBENCH_SKIPLIST_PMEM_TX_H
#define YCSB_STOREDSBENCH_SKIPLIST_PMEM_TX_H

int skiplist_pmem_tx_init(const char *path);

int skiplist_pmem_tx_read(const char *key, void *result);

int skiplist_pmem_tx_update(const char *key, void *value);

int skiplist_pmem_tx_insert(const char *key, void *value);

void skiplist_pmem_tx_free();

#endif //YCSB_STOREDSBENCH_SKIPLIST_PMEM_TX_H
