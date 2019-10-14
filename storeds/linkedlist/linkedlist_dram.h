//
// Created by aislam6 on 10/14/19.
//

#ifndef YCSB_STOREDSBENCH_LINKEDLIST_DRAM_H
#define YCSB_STOREDSBENCH_LINKEDLIST_DRAM_H

int linkedlist_dram_init(const char *path);

int linkedlist_dram_read(const char *key, void *result);

int linkedlist_dram_update(const char *key, void *value);

int linkedlist_dram_insert(const char *key, void *value);

void linkedlist_dram_free();

#endif //YCSB_STOREDSBENCH_LINKEDLIST_DRAM_H
