//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#ifndef YCSB_STOREDSBENCH_HT_PMEM_TX_H
#define YCSB_STOREDSBENCH_HT_PMEM_TX_H

int ht_pmem_tx_init(const char *path);

int ht_pmem_tx_read(const char *key, void *result);

int ht_pmem_tx_update(const char *key, void *value);

int ht_pmem_tx_insert(const char *key, void *value);

void ht_pmem_tx_free();

#endif //YCSB_STOREDSBENCH_HT_PMEM_TX_H
