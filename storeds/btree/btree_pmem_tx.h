//
// Created by aislam6 on 10/27/19.
//

#ifndef YCSB_STOREDSBENCH_BTREE_PMEM_TX_H
#define YCSB_STOREDSBENCH_BTREE_PMEM_TX_H

int btree_pmem_tx_init(const char *path);

int btree_pmem_tx_read(const char *key, void *result);

int btree_pmem_tx_update(const char *key, void *value);

int btree_pmem_tx_insert(const char *key, void *value);

void btree_pmem_tx_free();

#endif //YCSB_STOREDSBENCH_BTREE_PMEM_TX_H
