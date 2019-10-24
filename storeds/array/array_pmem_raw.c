/*
 * array-pmem.c
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
static int is_pmem;
static int fd;

const int pmem_array_size = 1024 * 1024;
const int default_value_len = 101;

struct array_elm {
    char value[default_value_len];
};

/**
 * array_pmem_init -- initialize the array
 */
int array_pmem_init(const char *path){
    if ((fd = open(path, O_CREAT|O_RDWR, 0666)) < 0) {
		perror("open");
		exit(1);
	}

    /* allocate the pmem */
	if ((errno = posix_fallocate(fd, 0, PM_ARRAY_POOL_SIZE)) != 0) {
		perror("posix_fallocate");
		exit(1);
	}

    /* memory map it */
	if ((pmemaddr = pmem_map(fd)) == NULL) {
		perror("pmem_map");
		exit(1);
	}

    close(fd);

    /* determine if range is true pmem */
	is_pmem = pmem_is_pmem(pmemaddr, PM_ARRAY_POOL_SIZE);

    return 1;
}

/**
 * array_pmem_read -- read data from array[key] and set the result in result
 */
int array_pmem_read(const char *key, void *result){    
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemaddr + offset * sizeof(struct array_elm));
    result = ptr->value;

    return 1;
}

/**
 * array_pmem_update -- update data to array[key] by value
 */
int array_pmem_update(const char *key, void *value){
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemaddr + offset * sizeof(struct array_elm));
    pmem_memcpy(ptr, (const char *) value, sizeof(struct array_elm));
    return 1;
}

/**
 * array_pmem_insert -- insert data value into array[key]
 */
int array_pmem_insert(const char *key, void *value){
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemaddr + offset * sizeof(struct array_elm));
    pmem_memcpy(ptr, (const char *) value, sizeof(struct array_elm));
    return 1;
}

/**
 * array_pmem_free -- destructor
 */
void array_pmem_free() {
    return;
}