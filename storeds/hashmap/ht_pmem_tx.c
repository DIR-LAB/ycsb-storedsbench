//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "ht_pmem_tx.h"

/*  
 * Data Structure Section
 */

/* size of the pmem object pool -- 1 GB */
#define PM_HASHTABLE_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "ht_layout"

/* large prime number used as a hashing function coefficient */
#define HASH_FUNC_COEFF_P 32212254719ULL

/* initial number of buckets */
#define INIT_BUCKETS_NUM 8192

/* number of values in a bucket which trigger hashtable rebuild check */
#define MIN_HASHSET_THRESHOLD 5

/* number of values in a bucket which force hashtable rebuild */
#define MAX_HASHSET_THRESHOLD 10

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* types of allocations */
enum types {
	ROOT_TYPE,
    ENTRY_TYPE,
    BUCKETS_TYPE,

    MAX_TYPES
};

/* declaration of data-structures */
struct entry {
	uint64_t key;
	char value[DEFAULT_VALUE_LEN];

	/* next entry list pointer */
	PMEMoid next;
};

struct buckets {
	/* number of buckets */
	size_t nbuckets;

	/* array of lists */
	PMEMoid bucket[];
};

struct hashtable_tx {
	/* random number generator seed */
	uint32_t seed;

	/* hash function coefficients */
	uint32_t hash_fun_coeff_a;	//hash_fun_a can not contain value '0'
	uint32_t hash_fun_coeff_b;
	uint64_t hash_fun_coeff_p;

	/* number of values inserted */
	uint64_t count;

	/* buckets */
	PMEMoid buckets;
};

/* Static Global Data */
static PMEMobjpool *pop = NULL;
static PMEMoid root_oid;
static struct hashtable_tx *root_p = NULL;

/*
 * ht_pmem_tx_check -- checks if hashtable has been initialized
 */
int ht_pmem_tx_check() {
	if (root_p->buckets.off == 0) {
        fprintf(stderr, "[%s]: FATAL: hashtable not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/*
 * ht_pmem_tx_print -- prints complete hashtable state
 */
void ht_pmem_tx_print() {
	ht_pmem_tx_check();

	PMEMoid buckets_oid = root_p->buckets;
	struct buckets * buckets_p = ((struct buckets *) pmemobj_direct(buckets_oid));
	PMEMoid entry_oid = OID_NULL;

	printf("a: %d, b: %d, p: %lu\n", root_p->hash_fun_coeff_a, root_p->hash_fun_coeff_b, root_p->hash_fun_coeff_p);
	printf("total entry count: %lu, buckets: %lu\n", root_p->count, buckets_p->nbuckets);

	for (size_t i = 0; i < buckets_p->nbuckets; ++i) {
		if (buckets_p->bucket[i].off == 0) continue;

		int num = 0;
		printf("bucket #%lu:", i);
		for (entry_oid = buckets_p->bucket[i]; entry_oid.off != 0; entry_oid = ((struct entry *) pmemobj_direct(entry_oid))->next) {
			struct entry *entry_p = (struct entry *) pmemobj_direct(entry_oid);
			printf(" <%lu, %s>", entry_p->key, entry_p->value);
			num++;
		}
		printf(" (%d)\n", num);
	}
}

/*
 * hash_function_tx -- the simplest hashing function,
 * see https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
 */
uint64_t hash_function_tx(uint64_t value) {
	uint32_t a = root_p->hash_fun_coeff_a;
	uint32_t b = root_p->hash_fun_coeff_b;
	uint64_t p = root_p->hash_fun_coeff_p;
	size_t len = ((struct buckets *) pmemobj_direct(root_p->buckets))->nbuckets;

	return ((value * a + b) % p) % len;
}

/*
 * ht_pmem_tx_reinit -- reinitialize the hashtable with a new number of buckets,
 * not implemented yet
 */
int ht_pmem_tx_reinit(size_t new_len) {
	return 1;
}

/*
 * ht_pmem_tx_init -- hashtable initializer
 */
int ht_pmem_tx_init(const char *path) {
    srand((uint32_t) time(NULL));

	if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PM_HASHTABLE_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    } else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    }

	root_oid = pmemobj_root(pop, sizeof(struct hashtable_tx));
    root_p = (struct hashtable_tx *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
        exit(0);
    }

	size_t len = INIT_BUCKETS_NUM;
	size_t buckets_sz = sizeof(struct buckets) + len * sizeof(struct entry);

	TX_BEGIN(pop) {
		pmemobj_tx_add_range(root_oid, 0, sizeof(struct hashtable_tx));

		root_p->seed = (uint32_t)time(NULL);
		do {
			root_p->hash_fun_coeff_a = (uint32_t) rand();
		} while(root_p->hash_fun_coeff_a == 0);

		root_p->hash_fun_coeff_b = (uint32_t) rand();
		root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;

		root_p->count = 0;

		root_p->buckets = pmemobj_tx_zalloc(buckets_sz, BUCKETS_TYPE);
		((struct buckets *) pmemobj_direct(root_p->buckets))->nbuckets = len;
	} TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END

	ht_pmem_tx_check();
    return 1;
}

/**
 * ht_pmem_tx_read -- read 'value' of 'key' and place it into '&result'
 */
int ht_pmem_tx_read(const char *key, void *result) {
	ht_pmem_tx_check();

	struct buckets *buckets_p = (struct buckets *) pmemobj_direct(root_p->buckets);
	PMEMoid entry_oid = OID_NULL;

	uint64_t uint64_key = strtoull(key, NULL, 0);
	uint64_t hash_value = hash_function_tx(uint64_key);

	//iteration_count can be used to check the number of iteration needed to find the value of a single key
	int iteration_count = 0;

	for(entry_oid = buckets_p->bucket[hash_value]; entry_oid.off != 0; entry_oid = ((struct entry *) pmemobj_direct(entry_oid))->next) {
		if(((struct entry *) pmemobj_direct(entry_oid))->key == uint64_key) {
			//key found! replace the value and return
			struct entry *entry_p = (struct entry *) pmemobj_direct(entry_oid);
			result = entry_p->value;
		}
		iteration_count += 1;
	}
    return 1;
}

/**
 * ht_pmem_tx_update -- update 'value' of 'key' into the hashtable,
 * will insert the 'value' if 'key' not exists
 */
int ht_pmem_tx_update(const char *key, void *value) {
	return ht_pmem_tx_insert(key, value);
}

/**
 * ht_pmem_tx_insert -- inserts 'value' into the hashtable,
 * will update the 'value' if 'key' already exists
 */
int ht_pmem_tx_insert(const char *key, void *value) {
	ht_pmem_tx_check();

	struct buckets *buckets_p = (struct buckets *) pmemobj_direct(root_p->buckets);
	PMEMoid entry_oid = OID_NULL;

	uint64_t uint64_key = strtoull(key, NULL, 0);
	uint64_t hash_value = hash_function_tx(uint64_key);

	//iteration_count can be used further to update the size of buckets with condition
	int iteration_count = 0;

	for(entry_oid = buckets_p->bucket[hash_value]; entry_oid.off != 0; entry_oid = ((struct entry *) pmemobj_direct(entry_oid))->next) {
		if(((struct entry *) pmemobj_direct(entry_oid))->key == uint64_key) {
			//key found! replace the value and return
			struct entry *entry_p = (struct entry *) pmemobj_direct(entry_oid);
			TX_BEGIN(pop) {
				pmemobj_tx_add_range_direct(entry_p, sizeof(struct entry));
				memcpy(entry_p->value, (char *) value, strlen((char *) value));
			} TX_ONABORT {
				fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
				abort();
			} TX_END

			return 1;
		}
		iteration_count += 1;
	}

	//key not found! need to insert data into bucket[hash_value]
	TX_BEGIN(pop) {
		pmemobj_tx_add_range_direct(&(buckets_p->bucket[hash_value]), sizeof(struct entry));
		pmemobj_tx_add_range_direct(&root_p->count, sizeof(uint64_t));

		PMEMoid entry_oid_new = pmemobj_tx_alloc(sizeof(struct entry), ENTRY_TYPE);
		struct entry *entry_p = (struct entry *) pmemobj_direct(entry_oid_new);
		entry_p->key = uint64_key;
		memcpy(entry_p->value, (char *) value, strlen((char *) value));
		entry_p->next = buckets_p->bucket[hash_value];
		buckets_p->bucket[hash_value] = entry_oid_new;

		root_p->count += 1;
		iteration_count += 1;
	} TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END

	//todo: acording to the value of 'iteration_count', we can add custom logic to reinitialize the hash table with new bigger bucket size

    return 1;
}

/**
 * ht_pmem_tx_free -- destructor
 */
void ht_pmem_tx_free() {
    TX_BEGIN(pop) {
        pmemobj_tx_add_range(root_oid, 0, sizeof(struct hashtable_tx));
        PMEMoid buckets_oid = root_p->buckets;
        struct buckets *buckets_p = ((struct buckets *) pmemobj_direct(buckets_oid));
        PMEMoid entry_oid = OID_NULL;

        for (size_t i = 0; i < buckets_p->nbuckets; ++i) {
            for (entry_oid = buckets_p->bucket[i]; entry_oid.off != 0; ) {
                PMEMoid current_entry_oid = entry_oid;
                entry_oid = ((struct entry *) pmemobj_direct(entry_oid))->next;

                pmemobj_tx_free(current_entry_oid);
            }
        }
		pmemobj_tx_free(root_p->buckets);
        root_p->buckets = OID_NULL;
		pmemobj_tx_free(root_oid);
		root_oid = OID_NULL;
    } TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END
    pmemobj_close(pop);
}