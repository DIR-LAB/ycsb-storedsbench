/*
 * array-pmem-raw.c
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <libpmem.h>
#include <time.h>

#include "../ex_common.h"
#include "array_pmem_raw.h"

/* size of the pmem object pool */
#define PM_ARRAY_POOL_SIZE (160 * 1024 * 1024)

/* Static Global Data */
static char* pmemaddr;
static size_t mapped_len;
static int is_pmem;

const int pmem_array_size = 1024 * 1024;
const int default_value_len = 101;

struct array_elm {
    char value[default_value_len];
};

/**
 * array_pmem_raw_init -- initialize the array
 */
int array_pmem_raw_init(const char *path){
    /* create a pmem file and memory map it */
	if ((pmemaddr = (char *) pmem_map_file(path, PM_ARRAY_POOL_SIZE, PMEM_FILE_CREATE, 0666, &mapped_len, &is_pmem)) == NULL) {
		perror("pmem_map_file");
		exit(1);
	}

    /* determine if range is true pmem */
	is_pmem = pmem_is_pmem(pmemaddr, PM_ARRAY_POOL_SIZE);

    return 1;
}

/**
 * array_pmem_raw_read -- read data from array[key] and set the result in result
 */
int array_pmem_raw_read(const char *key, void *result){    
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemaddr + offset * sizeof(struct array_elm));
    result = ptr->value;

    return 1;
}

/**
 * array_pmem_raw_update -- update data to array[key] by value
 */
int array_pmem_raw_update(const char *key, void *value){
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemaddr + offset * sizeof(struct array_elm));
    pmem_memcpy_persist(ptr, (const char *) value, sizeof(struct array_elm));
    return 1;
}

/**
 * array_pmem_raw_insert -- insert data value into array[key]
 */
int array_pmem_raw_insert(const char *key, void *value){
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemaddr + offset * sizeof(struct array_elm));
    pmem_memcpy_persist(ptr, (const char *) value, sizeof(struct array_elm));
    return 1;
}

/**
 * array_pmem_raw_free -- destructor
 */
void array_pmem_raw_free() {
    pmem_unmap(pmemaddr, mapped_len);
}