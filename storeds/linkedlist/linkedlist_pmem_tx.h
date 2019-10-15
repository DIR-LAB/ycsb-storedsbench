//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#ifndef YCSB_STOREDSBENCH_LINKEDLIST_PMEM_TX_H
#define YCSB_STOREDSBENCH_LINKEDLIST_PMEM_TX_H

int linkedlist_pmem_tx_init(const char *path);

int linkedlist_pmem_tx_read(const char *key, void *result);

int linkedlist_pmem_tx_update(const char *key, void *value);

int linkedlist_pmem_tx_insert(const char *key, void *value);

void linkedlist_pmem_tx_free();

#endif //YCSB_STOREDSBENCH_LINKEDLIST_PMEM_TX_H
