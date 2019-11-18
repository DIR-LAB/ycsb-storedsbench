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

/* Static Global Data */
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
        struct array_elm *ptr = (struct array_elm *) ((char *)pmemobj_direct(root_p->array) + i * sizeof(struct array_elm));
        printf("array element [%d] -> %s\n", i, ptr->value);
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
        pmemobj_tx_add_range(root_oid, 0, sizeof(struct array_root));
        
        //note: why not the following one?
        //pmemobj_tx_add_range_direct(pmemobj_direct(root_p->array), sizeof(struct array_elm) * pmem_array_size);

        root_p->array = pmemobj_tx_zalloc(sizeof(struct array_elm) * pmem_array_size, ARRAY_TYPE);
        //pmemobj_persist(pop, &root_p->array, sizeof(PMEMoid));
    } TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END
    pmem_tx_array_check();
    return 1;
}

/**
 * array_pmem_tx_read -- read data from array[key] and set the result in result
 */
int array_pmem_tx_read(const char *key, void *&result){
    pmem_tx_array_check();
    
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    struct array_elm *ptr = (struct array_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_elm));
    result = ptr->value;

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
        struct array_elm *ptr = (struct array_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_elm));
        pmemobj_tx_add_range_direct(ptr, sizeof(struct array_elm));
        //pmemobj_tx_add_range(pmemobj_oid(ptr), 0, sizeof(struct array_elm));
        strcpy(ptr->value, (const char *) value);
        //pmemobj_persist(pop, ptr, sizeof(struct array_elm));
    } TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END

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
        struct array_elm *ptr = (struct array_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_elm));
        pmemobj_tx_add_range_direct(ptr, sizeof(struct array_elm));
        strcpy(ptr->value, (const char *) value);
        //pmemobj_persist(pop, ptr, sizeof(struct array_elm));
    } TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END

    return 1;
}

/**
 * array_pmem_tx_free -- destructor
 */
void array_pmem_tx_free() {
    pmem_tx_array_check();
    TX_BEGIN(pop) {
        pmemobj_tx_add_range(root_oid, 0, sizeof(struct array_root));
        pmemobj_tx_free(root_p->array);
        root_p->array = OID_NULL;
        pmemobj_tx_free(root_oid);
        root_oid = OID_NULL;
    } TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END
    pmemobj_close(pop);
}