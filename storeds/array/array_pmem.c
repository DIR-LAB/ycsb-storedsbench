/*
 * array-pmem.c
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>
#include <time.h>

#include "ex_common.h"
#include "array_pmem.h"

#define TOID_ARRAY(x) TOID(x)
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define MAX_BUFFLEN 30
#define MAX_TYPE_NUM 8

POBJ_LAYOUT_BEGIN(array);
POBJ_LAYOUT_TOID(array, int);
POBJ_LAYOUT_TOID(array, PMEMoid);
POBJ_LAYOUT_TOID(array, struct array_info);
POBJ_LAYOUT_END(array);

static PMEMobjpool *pop;
static TOID(struct array_info) array_info;

struct array_info {
    PMEMoid array;
};

struct array_info *info = NULL;
const int pmem_array_size = 1000000;

int pmem_array_check(){
    if ((info->array).off == 0) {
        fprintf(stderr, "array not initialized yet\n");
        return (-1);
    }
    return 1;
}

int array_pmem_init(const char *path){
    pop = NULL;
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, POBJ_LAYOUT_NAME(array), PMEMOBJ_MIN_POOL, CREATE_MODE_RW)) == NULL) {
            printf("failed to create pool\n");
            return 1;
        }
    } else {
        if ((pop = pmemobj_open(path, POBJ_LAYOUT_NAME(array))) == NULL) {
            printf("failed to open pool\n");
            return 1;
        }
    }

    POBJ_ZNEW(pop, &array_info, struct array_info);
    info = D_RW(array_info);
    POBJ_ALLOC(pop, &info->array, int, sizeof(int) * pmem_array_size, NULL, NULL);
    if ((info->array).off == 0) {
        fprintf(stderr, "POBJ_ALLOC\n");
        return -1;
    }
    pmemobj_persist(pop, info, sizeof(*info));
    return 1;
}

int array_pmem_read(const char *key, void *result){
    pmem_array_check();
    TOID(int) array;
    TOID_ASSIGN(array, info->array);
    int counter = D_RO(array)[atoi(key) % pmem_array_size];
    result = &counter;
    return 1;
}

int array_pmem_update(const char *key, void *value){
    pmem_array_check();
    TOID(int) array;
    TOID_ASSIGN(array, info->array);
    int offset = atoi(key) % pmem_array_size;
    D_RW(array)[offset] = atoi((const char *) value);
    pmemobj_persist(pop, &D_RW(array)[offset], sizeof(int));
    return 1;
}

int array_pmem_insert(const char *key, void *value){
    pmem_array_check();
    TOID(int) array;
    TOID_ASSIGN(array, info->array);
    int offset = atoi(key) % pmem_array_size;
    D_RW(array)[offset] = atoi((const char *) value);
    pmemobj_persist(pop, &D_RW(array)[offset], sizeof(int));
    return 1;
}

static void free_int() {
    TOID(int) array;
    TOID_ASSIGN(array, info->array);
    POBJ_FREE(&array);
    POBJ_FREE(&array_info);
}