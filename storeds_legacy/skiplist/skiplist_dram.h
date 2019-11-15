//
// Created by aislam6 on 10/16/19.
//

#ifndef YCSB_STOREDSBENCH_SKIPLIST_DRAM_H
#define YCSB_STOREDSBENCH_SKIPLIST_DRAM_H

int skiplist_dram_init(const char *path);

int skiplist_dram_read(const char *key, void *&result);

int skiplist_dram_update(const char *key, void *value);

int skiplist_dram_insert(const char *key, void *value);

void skiplist_dram_free();

#endif //YCSB_STOREDSBENCH_SKIPLIST_DRAM_H
