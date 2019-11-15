/*
 * array-pmem-raw.h
 */

#ifndef array_pmem_raw_h
#define array_pmem_raw_h

int array_pmem_raw_init(const char *path);
int array_pmem_raw_read(const char *key, void *&result);
int array_pmem_raw_update(const char *key, void *value);
int array_pmem_raw_insert(const char *key, void *value);
void array_pmem_raw_free();

#endif /* array_pmem_raw_h */