//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#ifndef YCSB_STOREDSBENCH_HT_DRAM_H
#define YCSB_STOREDSBENCH_HT_DRAM_H

int ht_dram_init(const char *path);

int ht_dram_read(const char *key, void *&result);

int ht_dram_update(const char *key, void *value);

int ht_dram_insert(const char *key, void *value);

void ht_dram_free();

#endif //YCSB_STOREDSBENCH_HT_DRAM_H
