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

/* size of the pmem object pool */
#define PM_ARRAY_POOL_SIZE (160 * 1024 * 1024)

/* name of layout in the pool */
#define LAYOUT_NAME "array_layout"

static PMEMobjpool *pop = NULL;
static PMEMoid root_oid;
static struct array_root *root_p = NULL;

const int pmem_array_size = 1000000;
const int default_value_len = 101;

struct array_root {
    PMEMoid array;
};

struct array_elm {
    char value[default_value_len];
};

/**
 * types -- types of allocations
 */
enum types {
    ARRAY_TYPE,
    ARRAY_ELEMENT_TYPE,
    MAX_TYPES
};

/**
 * pmem_array_check -- check if pmem array has been initialized
 */
int pmem_array_check() {
    if (root_p->array.off == 0) {
        fprintf(stderr, "[%s]: FATAL: array not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/**
 * print_toid -- print array of TOID(struct array_elm) type
 */
void print_toid(struct array_info *info) {
	for (int i = 0; i < pmem_array_size; i++) {
        printf("array element [%d] -> %s\n", i, ((struct array_elm *) pmemobj_direct(((PMEMoid *) pmemobj_direct(root_p->array))[i]))->value);
    }
}

/**
 * array_pmem_init -- initialize the array
 */
int array_pmem_init(const char *path){
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    } else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    }

    root_oid = pmemobj_root(pop, sizeof(struct array_root));
    root_p = (struct array_root *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
        exit(0);
    }
    pmemobj_alloc(pop, &root_p->array, sizeof(struct array_elm) * pmem_array_size, ARRAY_TYPE, NULL, NULL);
    printf("array is ready\n");
    pmemobj_persist(pop, root_p, sizeof(*root_p));

    pmem_array_check();
    return 1;
}

/**
 * array_pmem_read -- read data from array[key] and set the result in result
 */
int array_pmem_read(const char *key, void *result){
    pmem_array_check();
    
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    const char *counter = ((struct array_elm *) pmemobj_direct(((PMEMoid *) pmemobj_direct(root_p->array))[offset]))->value;
    result = &counter;

    return 1;
}

/**
 * array_pmem_update -- update data to array[key] by value
 */
int array_pmem_update(const char *key, void *value){
    pmem_array_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    PMEMoid array_elm_oid = OID_NULL;
    pmemobj_zalloc(pop, &array_elm_oid, sizeof(struct array_elm), ARRAY_ELEMENT_TYPE);
    struct array_elm *array_elm_ptr = (struct array_elm *) pmemobj_direct(array_elm_oid);
    strcpy(array_elm_ptr->value, (const char *) value);
    ((PMEMoid *) pmemobj_direct(root_p->array))[offset] = array_elm_oid;
    
    pmemobj_persist(pop, &(((PMEMoid *) pmemobj_direct(root_p->array))[offset]), sizeof(struct array_elm));
    pmemobj_free(&array_elm_oid);

    return 1;
}

/**
 * array_pmem_insert -- insert data value into array[key]
 */
int array_pmem_insert(const char *key, void *value){
    //printf("%s %s\n", key, (const char *) value);
    pmem_array_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    //todo: need to check later, why the following approach not working?
    /*PMEMoid * array_p = (PMEMoid *) pmemobj_direct(root_p->array);
    struct array_elm * array_elm_ptr = (struct array_elm *) pmemobj_direct(array_p[offset]);
    if(array_elm_ptr->value == NULL) array_elm_ptr->value = malloc(sizeof(value));
    pmemobj_memcpy_persist(pop, array_elm_ptr->value, (const char *) value, sizeof(value));
    pmemobj_persist(pop, array_elm_ptr, sizeof(array_elm_ptr));*/

    PMEMoid array_elm_oid = OID_NULL;
    pmemobj_zalloc(pop, &array_elm_oid, sizeof(struct array_elm), ARRAY_ELEMENT_TYPE);
    struct array_elm *array_elm_ptr = (struct array_elm *) pmemobj_direct(array_elm_oid);
    strcpy(array_elm_ptr->value, (const char *) value);
    ((PMEMoid *) pmemobj_direct(root_p->array))[offset] = array_elm_oid;
    
    pmemobj_persist(pop, &(((PMEMoid *) pmemobj_direct(root_p->array))[offset]), sizeof(struct array_elm));
    pmemobj_free(&array_elm_oid);

    return 1;
}

/*
 * free_toid -- de-allocate array of TOID(struct array_elm) type
 */
void free_toid() {
    for (int i = 0; i < pmem_array_size; i++) {
        pmemobj_free(&((PMEMoid *) pmemobj_direct(root_p->array))[i]);
    }
    root_p->array = OID_NULL;
}

/**
 * array_pmem_free -- destructor
 */
void array_pmem_free() {
    free_toid();
    pmemobj_close(pop);
}