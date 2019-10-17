//
// Created by aislam6 on 10/17/19.
//

#ifndef YCSB_STOREDSBENCH_QUEUE_PMEM_TX_H
#define YCSB_STOREDSBENCH_QUEUE_PMEM_TX_H

int queue_pmem_tx_init(const char *path);

int queue_pmem_tx_read(const char *key, void *result);

int queue_pmem_tx_update(const char *key, void *value);

int queue_pmem_tx_insert(const char *key, void *value);

void queue_pmem_tx_free();

#endif //YCSB_STOREDSBENCH_QUEUE_PMEM_TX_H
