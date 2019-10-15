//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#ifndef YCSB_STOREDSBENCH_HT_PMEM_H
#define YCSB_STOREDSBENCH_HT_PMEM_H

int ht_pmem_init(const char *path);

int ht_pmem_read(const char *key, void *result);

int ht_pmem_update(const char *key, void *value);

int ht_pmem_insert(const char *key, void *value);

void ht_pmem_free();

#endif //YCSB_STOREDSBENCH_HT_PMEM_H
