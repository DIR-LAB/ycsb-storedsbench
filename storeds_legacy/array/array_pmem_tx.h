/*
 * array_pmem_tx.h
 */

#ifndef array_pmem_tx_h
#define array_pmem_tx_h

int array_pmem_tx_init(const char *path);
int array_pmem_tx_read(const char *key, void *&result);
int array_pmem_tx_update(const char *key, void *value);
int array_pmem_tx_insert(const char *key, void *value);
void array_pmem_tx_free();

#endif /* array_pmem_h */