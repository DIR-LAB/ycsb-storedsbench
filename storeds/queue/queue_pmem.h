//
// Created by aislam6 on 10/17/19.
//

#ifndef YCSB_STOREDSBENCH_QUEUE_PMEM_H
#define YCSB_STOREDSBENCH_QUEUE_PMEM_H

int queue_pmem_init(const char *path);

int queue_pmem_read(const char *key, void *result);

int queue_pmem_update(const char *key, void *value);

int queue_pmem_insert(const char *key, void *value);

void queue_pmem_free();

#endif //YCSB_STOREDSBENCH_QUEUE_PMEM_H
