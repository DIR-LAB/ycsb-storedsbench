//
// Created by aislam6 on 10/27/19.
//

#ifndef YCSB_STOREDSBENCH_BTREE_DRAM_H
#define YCSB_STOREDSBENCH_BTREE_DRAM_H

int btree_dram_init(const char *path);

int btree_dram_read(const char *key, void *&result);

int btree_dram_update(const char *key, void *value);

int btree_dram_insert(const char *key, void *value);

void btree_dram_free();

#endif //YCSB_STOREDSBENCH_BTREE_DRAM_H
