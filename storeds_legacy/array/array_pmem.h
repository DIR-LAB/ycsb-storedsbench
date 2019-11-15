/*
 * array-pmem.c
 */

#ifndef array_pmem_h
#define array_pmem_h

int array_pmem_init(const char *path);
int array_pmem_read(const char *key, void *&result);
int array_pmem_update(const char *key, void *value);
int array_pmem_insert(const char *key, void *value);
void array_pmem_free();

#endif /* array_pmem_h */