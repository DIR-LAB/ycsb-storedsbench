/*
 * array_pmem_tx.c
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>
#include <time.h>

#include "ex_common.h"
#include "array_pmem_tx.h"

#define TOID_ARRAY(x) TOID(x)
#define COUNT_OF(x) (sizeof(x) / sizeof(x[0]))
#define MAX_BUFFLEN 30
#define MAX_TYPE_NUM 8
#define PM_ARRAY_POOL_SIZE    (160 * 1024 * 1024)

POBJ_LAYOUT_BEGIN(array_tx);
POBJ_LAYOUT_ROOT(array_tx, struct array_root);
POBJ_LAYOUT_TOID(array_tx, struct array_elm);
POBJ_LAYOUT_TOID(array_tx, PMEMoid);
POBJ_LAYOUT_TOID(array_tx, TOID(struct array_elm));
POBJ_LAYOUT_TOID(array_tx, struct array_info);
POBJ_LAYOUT_END(array_tx);

static PMEMobjpool *pop;
static TOID(struct array_info) array_info;
static TOID(struct array_elm) array_elm;
const TOID(struct array_root) root;

//struct array_info *info_tx = NULL;
const int pmem_array_size = 1000000;
const int default_value_len = 101;

//todo: start from here
struct array_root {
    TOID(struct array_info) info;
};


struct array_elm {
	char value[default_value_len];
    //char *value;
};

struct array_info {
    PMEMoid array;
};

int pmem_tx_array_check(){
    const struct array_root *root_p = D_RO(root);
    const struct array_info *info = D_RO(root_p->info);

    if ((info->array).off == 0) {
        fprintf(stderr, "array not initialized yet\n");
        assert(0);
    }
    return 1;
}

/*
 * print_toid_tx -- print array of TOID(struct array_elm) type
 */
void print_toid_tx(struct array_info *info_tx) {
	TOID_ARRAY(TOID(struct array_elm)) array;
	TOID_ASSIGN(array, info_tx->array);
	for (int i = 0; i < pmem_array_size; i++) {
        printf("array_elm = %s\n", D_RO(D_RO(array)[i])->value);
    }
}

/*
 * elm_constructor_tx -- constructor of array_elm type object
 */
/*int elm_constructor_tx(PMEMobjpool *pop, void *ptr, void *arg) {
    //note: how the following block can be put into a tx block
	struct array_elm *obj = (struct array_elm *)ptr;
    obj->value = (char *) malloc(default_value_len * sizeof(char *));
    //note: we will not need this manual persist
	//pmemobj_persist(pop, obj, sizeof(*obj));

	return 0;
}*/

/*
 * alloc_toid_tx -- allocate array of TOID(struct array_elm) type
 */
PMEMoid alloc_toid_tx() {
	TOID_ARRAY(TOID(struct array_elm)) array;
	/*
	 * To allocate persistent array of TOID with user-defined structure type
	 * is necessary to allocate pointer with size equal to number of
	 * elements multiplied by size of TOID of proper type and to allocate
	 * each of elements separately.
	 */
    TX_BEGIN(pop) {
        TX_ADD(array);
        array = TX_ALLOC(TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size);
	    //POBJ_ALLOC(pop, &array, TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size, NULL, NULL);

        if (TOID_IS_NULL(array)) {
            fprintf(stderr, "POBJ_ALLOC\n");
            return OID_NULL;
        }

        /*for (size_t i = 0; i < pmem_array_size; i++) {
            printf("amar ami nai re amar ami nai ...\n");
            //note: how we can use TX_ALLOC instead of the following line?
            POBJ_NEW(pop, &D_RW(array)[i], struct array_elm, elm_constructor_tx, NULL);
            //D_RW(array)[i] = TX_ALLOC(struct array_elm, default_value_len * sizeof(char *));
            printf("%s\n", D_RO(D_RO(array)[i])->value);
            if (TOID_IS_NULL(D_RO(array)[i])) {
                fprintf(stderr, "POBJ_ALLOC\n");
                assert(0);
            }
        }*/
    } TX_END;
	return array.oid;
}

int array_pmem_tx_init(const char *path){
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

    TOID(struct array_root) root = POBJ_ROOT(pop, struct array_root);
    struct array_root *root_p = D_RW(root);
    struct array_info *info_tx = D_RW(root_p->info);

    TX_BEGIN(pop) {
        printf("into transaction\n");
        //todo: need to bring root here
        TX_ADD(root);
        //note: not sure what should be added in the transaction here
        printf("added root to transaction\n");
        //POBJ_ZNEW(pop, &array_info, struct array_info);
        //TOID(struct array_info) ar_info = TX_ZNEW(struct array_info);
        //struct array_root *root_p = D_RW(root);
        //TX_ADD(root_p->info);
        printf("added info to transaction\n");
        //struct array_info *info_tx = D_RW(root_p->info);
        info_tx = D_RW(TX_ZNEW(struct array_info));
        printf("going to allocate other stuffs\n");
        
        //info_tx->array = alloc_toid_tx();
        //info_tx->array = TX_ALLOC(TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size).oid;
        POBJ_ALLOC(pop, &info_tx->array, TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size, NULL, NULL);
    // } TX_END;
        
    // printf("array_elm size: %ld\n", sizeof(TOID(struct array_elm)));
    // printf("info_tx->array size: %ld\n", sizeof(info_tx->array));
    // printf("allocation done\n");
    
    // TX_BEGIN(pop) {

        TOID_ARRAY(TOID(struct array_elm)) array;
        POBJ_ALLOC(pop, &array, TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size, NULL, NULL);

        printf("info_tx->array size: %ld\n", sizeof(info_tx->array));
        printf("allocated size: %ld\n", sizeof(TOID(struct array_elm)) * pmem_array_size);
        printf("size: %ld\n", sizeof(array));
	    if (TOID_IS_NULL(array)) {
		    fprintf(stderr, "POBJ_ALLOC\n");
		    abort();
	    }
        for (size_t i = 0; i < pmem_array_size; i++) {
            printf("->i=%ld\n", i);
            
            //TOID_ASSIGN(array, info_tx->array);
            D_RW(array)[i] = TX_ZNEW(struct array_elm);
		    //POBJ_NEW(pop, &D_RW(info_tx->array)[i], struct array_elm, elm_constructor, NULL);
		    if (TOID_IS_NULL(D_RW(array)[i])) {
			    fprintf(stderr, "POBJ_ALLOC\n");
                //abort();
			    assert(0);
		    }
	    }

        printf("array after loop: %ld\n", sizeof(array));
        info_tx->array = array.oid;

        if(info_tx == NULL) printf("info_tx is null\n");
        else printf("info_tx is not null\n");
        printf("%ld\n", (info_tx->array).off);
        
        //info_tx->array = TX_ALLOC(TOID(struct array_elm), sizeof(TOID(struct array_elm)) * pmem_array_size).oid;
        /*if ((info_tx->array).off == 0) {
            fprintf(stderr, "POBJ_ALLOC\n");
            abort();
            //note: need rollback
        }*/
        printf("here I come ...\n");
        /*for (size_t i = 0; i < pmem_array_size; i++) {
            POBJ_NEW(pop, &D_RW(info_tx->array)[i], struct array_elm, elm_constructor_tx, NULL);
            //D_RW(info_tx->array)[i] = TX_ALLOC(struct array_elm, default_value_len * sizeof(char *));
            printf("%s\n", D_RO(D_RO(array)[i])->value);
            if (TOID_IS_NULL(D_RO(array)[i])) {
                fprintf(stderr, "POBJ_ALLOC\n");
                assert(0);
            }
        }*/

        //pmem_tx_array_check();
        //note: we will not need this manual persist
        //pmemobj_persist(pop, info_tx, sizeof(*info_tx));
    } TX_END;
    printf("transaction done\n");
    return 1;
}

int array_pmem_tx_read(const char *key, void *result){
    pmem_tx_array_check();
    const struct array_root *root_p = D_RO(root);
    const struct array_info *info = D_RO(root_p->info);
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    //note: we don't need transaction support for read operation. is that correct?
    const char *counter = D_RO(D_RO(array)[offset])->value;
    result = &counter;

    return 1;
}

int array_pmem_tx_update(const char *key, void *value){
    pmem_tx_array_check();
    const struct array_root *root_p = D_RO(root);
    const struct array_info *info = D_RO(root_p->info);
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    TX_BEGIN(pop) {
        TX_ADD(array);
        strcpy(D_RW(D_RW(array)[offset])->value, (const char *) value);
    } TX_END;

    return 1;
}

int array_pmem_tx_insert(const char *key, void *value){
    pmem_tx_array_check();
    printf("Insert: %s %s\n", key, (char *)value);
    const struct array_root *root_p = D_RO(root);
    const struct array_info *info = D_RO(root_p->info);
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % pmem_array_size);

    TX_BEGIN(pop) {
        TX_ADD(array);
        //printf("-> %s\n", D_RO(D_RO(array)[offset])->value);
        strcpy(D_RW(D_RW(array)[offset])->value, (const char *) value);
    } TX_END;

    return 1;
}

/*
 * free_toid_tx -- de-allocate array of TOID(struct array_elm) type
 */
void free_toid_tx() {
    const struct array_root *root_p = D_RO(root);
    const struct array_info *info = D_RO(root_p->info);
    TOID_ARRAY(TOID(struct array_elm)) array;
    TOID_ASSIGN(array, info->array);
    /*
	 * When there is persistent array of persistent pointer type allocated,
	 * there is necessary to de-allocate each element, if they were
	 * allocated earlier
	 */
    TX_BEGIN(pop) {
        TX_ADD(array);
        for (int i = 0; i < pmem_array_size; i++) {
            POBJ_FREE(&D_RW(array)[i]);
        }
        POBJ_FREE(&array);
    } TX_END;
}

void array_pmem_tx_free() {
    TX_BEGIN(pop) {
        TX_ADD(root);
        free_toid_tx();
        const struct array_root *root_p = D_RO(root);
        const struct array_info *info = D_RO(root_p->info);
        POBJ_FREE(info);
        //note: now info_tx.array is freed, how to make it null?
        //refference: https://pmem.io/2015/06/17/tx-alloc.html
        POBJ_FREE(root_p);     //note: how to free info_tx here?
    } TX_END;
    pmemobj_close(pop);
}