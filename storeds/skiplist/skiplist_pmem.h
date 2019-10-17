//
// Created by aislam6 on 10/16/19.
//

#ifndef YCSB_STOREDSBENCH_SKIPLIST_PMEM_H
#define YCSB_STOREDSBENCH_SKIPLIST_PMEM_H

int skiplist_pmem_init(const char *path);

int skiplist_pmem_read(const char *key, void *result);

int skiplist_pmem_update(const char *key, void *value);

int skiplist_pmem_insert(const char *key, void *value);

void skiplist_pmem_free();

#endif //YCSB_STOREDSBENCH_SKIPLIST_PMEM_H
