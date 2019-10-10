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
#define PM_ARRAY_POOL_SIZE    (160 * 1024 * 1024)

POBJ_LAYOUT_BEGIN(array);
POBJ_LAYOUT_TOID(array, struct array_elm);
POBJ_LAYOUT_TOID(array, PMEMoid);
POBJ_LAYOUT_TOID(array, TOID(struct array_elm));
POBJ_LAYOUT_TOID(array, struct array_info);
POBJ_LAYOUT_END(array);

static PMEMobjpool *pop;
static TOID(struct array_info) array_info;
static TOID(struct array_elm) array_elm;

struct array_elm {
	char *value;
};

struct array_info {
    PMEMoid array;
};

struct array_info *info = NULL;
const int pmem_array_size = 1000000;
const int default_value_len = 101;

int pmem_array_check(){
    if ((info->array).off == 0) {
        fprintf(stderr, "array not initialized yet\n");
        assert(0);
    }
    return 1;
}

/*
 * print_toid -- print array of TOID(struct array_elm) type
 */
void print_toid(struct array_info *info) {
	TOID_ARRAY(TOID(struct array_elm)) array;
	TOID_ASSIGN(array, info->array);
	for (int i = 0; i < pmem_array_size; i++) {
        printf("array_elm = %s\n", D_RO(D_RO(array)[i])->value);
    }
}

/*
 * elm_constructor -- constructor of array_elm type object
 */
int elm_constructor(PMEMobjpool *pop, void *ptr, void *arg) {
	struct array_elm *obj = (struct array_elm *)ptr;
    obj->value = (char *) malloc(default_value_len * sizeof(char *));
	pmemobj_persist(pop, obj, sizeof(*obj));

	return 0;
}

/*
 * alloc_toid -- allocate array of TOID(struct array_elm) type
 */
PMEMoid alloc_toid() {
	TOID_ARRAY(TOID(struct array_elm)) array;
	/*
	 * To allocate persistent array of TOID with user-defined structure type
	 * is necessary to allocate pointer with size equal to number of
	 * elements multiplied by size of TOID of proper type and to allocate
	 * each of elements separately.
	 */
	POBJ_ALLOC(pop, &array, TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size, NULL, NULL);

	if (TOID_IS_NULL(array)) {
		fprintf(stderr, "POBJ_ALLOC\n");
		return OID_NULL;
	}

	for (size_t i = 0; i < pmem_array_size; i++) {
		POBJ_NEW(pop, &D_RW(array)[i], struct array_elm, elm_constructor, NULL);
		if (TOID_IS_NULL(D_RW(array)[i])) {
			fprintf(stderr, "POBJ_ALLOC\n");
			assert(0);
		}
	}
	return array.oid;
}

int array_pmem_init(const char *path){
    pop = NULL;
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, POBJ_LAYOUT_NAME(array), PM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    } else {
        if ((pop = pmemobj_open(path, POBJ_LAYOUT_NAME(array))) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    }

    POBJ_ZNEW(pop, &array_info, struct array_info);
    info = D_RW(array_info);
    info->array = alloc_toid();
    if ((info->array).off == 0) {
        fprintf(stderr, "POBJ_ALLOC\n");
        return -1;
    }
    pmemobj_persist(pop, info, sizeof(*info));
    return 1;
}

int array_pmem_read(const char *key, void *result){
    pmem_array_check();
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    char *counter = D_RO(D_RO(array)[offset])->value;
    result = &counter;

    return 1;
}

int array_pmem_update(const char *key, void *value){
    pmem_array_check();
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    strcpy(D_RW(D_RW(array)[offset])->value, (const char *) value);
    pmemobj_persist(pop, &D_RW(array)[offset], sizeof(int));

    return 1;
}

int array_pmem_insert(const char *key, void *value){
    pmem_array_check();
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    strcpy(D_RW(D_RW(array)[offset])->value, (const char *) value);
    pmemobj_persist(pop, &D_RW(array)[offset], sizeof(int));

    return 1;
}

/*
 * free_toid -- de-allocate array of TOID(struct array_elm) type
 */
void free_toid() {
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);
    /*
	 * When there is persistent array of persistent pointer type allocated,
	 * there is necessary to de-allocate each element, if they were
	 * allocated earlier
	 */
	for (int i = 0; i < pmem_array_size; i++) {
        POBJ_FREE(&D_RW(array)[i]);
    }
    POBJ_FREE(&array);
}

void array_pmem_free() {
    free_toid();
    POBJ_FREE(&array_info);
    pmemobj_close(pop);
}