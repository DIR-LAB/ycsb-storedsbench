//
// Created by aislam6 on 10/25/19.
//

#ifndef YCSB_STOREDSBENCH_RBTREE_DRAM_H
#define YCSB_STOREDSBENCH_RBTREE_DRAM_H

int rbtree_dram_init(const char *path);

int rbtree_dram_read(const char *key, void *&result);

int rbtree_dram_update(const char *key, void *value);

int rbtree_dram_insert(const char *key, void *value);

void rbtree_dram_free();

#endif //YCSB_STOREDSBENCH_RBTREE_DRAM_H
