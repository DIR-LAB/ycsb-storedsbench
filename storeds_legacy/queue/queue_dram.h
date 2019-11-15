#ifndef YCSB_STOREDSBENCH_QUEUE_DRAM_H
#define YCSB_STOREDSBENCH_QUEUE_DRAM_H

int queue_dram_init(const char *path);

int queue_dram_read(const char *key, void *result);

int queue_dram_scan(const char *key, int len, void *result);

int queue_dram_update(const char *key, void *value);

int queue_dram_insert(const char *key, void *value);

int queue_dram_delete(const char *key);

void queue_dram_free();

#endif //YCSB_STOREDSBENCH_QUEUE_DRAM_H