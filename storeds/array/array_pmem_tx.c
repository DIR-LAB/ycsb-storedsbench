/*
 * array_pmem_tx.c -- persistent memory array implementation with transaction support
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>
#include <time.h>

#include "../ex_common.h"
#include "array_pmem_tx.h"

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
 * pmem_tx_array_check -- check if array has been initialized
 */
int pmem_tx_array_check(){
    if (root_p->array.off == 0) {
        fprintf(stderr, "[%s]: FATAL: array not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/**
 * print_toid_tx -- print array of TOID(struct array_elm) type
 */
void print_toid_tx() {
	for (int i = 0; i < pmem_array_size; i++) {
        printf("array element [%d] -> %s\n", i, ((struct array_elm *) pmemobj_direct(((PMEMoid *) pmemobj_direct(root_p->array))[i]))->value);
    }
}

/**
 * array_pmem_tx_init -- initialize the array
 */
int array_pmem_tx_init(const char *path){
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    } else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    }

    root_oid = pmemobj_root(pop, sizeof(struct array_root));
    root_p = (struct array_root *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
        exit(0);
    }
    
    TX_BEGIN(pop) {
        PMEMoid array = pmemobj_tx_alloc(sizeof(struct array_elm) * pmem_array_size, ARRAY_TYPE);
        printf("array is ready\n");
        root_p->array = array;
    } TX_END;
    pmem_tx_array_check();
    return 1;
}

/**
 * array_pmem_tx_read -- read data from array[key] and set the result in result
 */
int array_pmem_tx_read(const char *key, void *result){
    pmem_tx_array_check();
    
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    const char *counter = ((struct array_elm *) pmemobj_direct(((PMEMoid *) pmemobj_direct(root_p->array))[offset]))->value;
    result = &counter;

    return 1;
}

/**
 * array_pmem_tx_update -- update data to array[key] by value
 */
int array_pmem_tx_update(const char *key, void *value){
    pmem_tx_array_check();
    
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    TX_BEGIN(pop) {
        //todo: this would be another way of update/insert. should check it later
        //pmemobj_tx_add_range((PMEMoid) ((PMEMoid *) pmemobj_direct(root_p->array))[offset], 0, sizeof(struct array_elm));
        //strcpy(((struct array_elm *) pmemobj_direct((PMEMoid) ((PMEMoid *) pmemobj_direct(root_p->array))[offset]))->value, (const char *) value);

        PMEMoid array_elm_oid = pmemobj_tx_alloc(sizeof(struct array_elm), ARRAY_ELEMENT_TYPE);
        struct array_elm *array_elm_ptr = (struct array_elm *) pmemobj_direct(array_elm_oid);
        strcpy(array_elm_ptr->value, (const char *) value);

        pmemobj_tx_add_range(root_p->array, 0, sizeof(struct array_root));
        ((PMEMoid *) pmemobj_direct(root_p->array))[offset] = array_elm_oid;
    } TX_END;

    return 1;
}

/**
 * array_pmem_tx_insert -- insert data value into array[key]
 */
int array_pmem_tx_insert(const char *key, void *value){
    pmem_tx_array_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    TX_BEGIN(pop) {
        PMEMoid array_elm_oid = pmemobj_tx_alloc(sizeof(struct array_elm), ARRAY_ELEMENT_TYPE);
        struct array_elm *array_elm_ptr = (struct array_elm *) pmemobj_direct(array_elm_oid);
        strcpy(array_elm_ptr->value, (const char *) value);
        
        //note: how to add transaction_add_range on array[offset] element?
        pmemobj_tx_add_range(root_p->array, 0, sizeof(struct array_root));
        ((PMEMoid *) pmemobj_direct(root_p->array))[offset] = array_elm_oid;
    } TX_END;

    //printf("-> %s\n", ((struct array_elm *) pmemobj_direct(((PMEMoid *) pmemobj_direct(root_p->array))[offset]))->value);
    return 1;
}

/**
 * free_toid_tx -- de-allocate array of TOID(struct array_elm) type
 */
void free_toid_tx() {
    TX_BEGIN(pop) {
        pmemobj_tx_add_range(root_oid, 0, sizeof(struct array_root));
        for (int i = 0; i < pmem_array_size; i++) {
            pmemobj_tx_free(((PMEMoid *) pmemobj_direct(root_p->array))[i]);
        }
        root_p->array = OID_NULL;
    } TX_END;
}

/**
 * array_pmem_tx_free -- destructor
 */
void array_pmem_tx_free() {
    free_toid_tx();
    pmemobj_close(pop);
}