//
// Created by aislam6 on 10/25/19.
//

#ifndef YCSB_STOREDSBENCH_RBTREE_PMEM_H
#define YCSB_STOREDSBENCH_RBTREE_PMEM_H

int rbtree_pmem_init(const char *path);

int rbtree_pmem_read(const char *key, void *&result);

int rbtree_pmem_update(const char *key, void *value);

int rbtree_pmem_insert(const char *key, void *value);

void rbtree_pmem_free();

#endif //YCSB_STOREDSBENCH_RBTREE_PMEM_H
