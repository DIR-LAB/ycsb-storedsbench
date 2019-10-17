//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "ht_dram.h"

/* large prime number used as a hashing function coefficient */
#define HASH_FUNC_COEFF_P 32212254719ULL

/* initial number of buckets */
#define INIT_BUCKETS_NUM 1000

/* number of values in a bucket which trigger hashtable rebuild check */
#define MIN_HASHSET_THRESHOLD 5

/* number of values in a bucket which force hashtable rebuild */
#define MAX_HASHSET_THRESHOLD 10

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* declaration of data-structures */
struct entry {
	uint64_t key;
	char value[DEFAULT_VALUE_LEN];

	/* next entry list pointer */
	struct entry *next;
};

struct buckets {
	/* number of buckets */
	size_t nbuckets;

	/* array of lists */
	struct entry **bucket;
};

struct hashtable {
	/* random number generator seed */
	uint32_t seed;

	/* hash function coefficients */
	uint32_t hash_fun_coeff_a;	//hash_fun_a can not contain value '0'
	uint32_t hash_fun_coeff_b;
	uint64_t hash_fun_coeff_p;

	/* number of values inserted */
	uint64_t count;

	/* buckets */
	struct buckets *buckets;
};

/* Static Global Data */
static struct hashtable *root_p = NULL;

/*
 * ht_dram_check -- checks if hashtable has been initialized
 */
int ht_dram_check() {
	if (root_p == NULL || root_p->buckets == NULL) {
        fprintf(stderr, "[%s]: FATAL: hashtable not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/*
 * ht_dram_print -- prints complete hashtable state
 */
void ht_dram_print() {
	ht_dram_check();

	struct buckets *buckets_p = root_p->buckets;

	printf("a: %d, b: %d, p: %lu\n", root_p->hash_fun_coeff_a, root_p->hash_fun_coeff_b, root_p->hash_fun_coeff_p);
	printf("total entry count: %lu, buckets: %lu\n", root_p->count, buckets_p->nbuckets);

	for (size_t i = 0; i < buckets_p->nbuckets; ++i) {
		if (buckets_p->bucket[i] == NULL) continue;

		int num = 0;
		printf("bucket #%lu:", i);
		for (struct entry *entry_p = buckets_p->bucket[i]; entry_p != NULL; entry_p = entry_p->next) {
			printf(" <%lu, %s>", entry_p->key, entry_p->value);
			num++;
		}
		printf(" (%d)\n", num);
	}
}

/*
 * hash_function_dram -- the simplest hashing function,
 * see https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
 */
uint64_t dram_hash_function(uint64_t value) {
	uint32_t a = root_p->hash_fun_coeff_a;
	uint32_t b = root_p->hash_fun_coeff_b;
	uint64_t p = root_p->hash_fun_coeff_p;
	size_t len = root_p->buckets->nbuckets;

	return ((value * a + b) % p) % len;
}

/*
 * ht_dram_reinit -- reinitialize the hashtable with a new number of buckets, not implemented yet
 */
int ht_dram_reinit(size_t new_len) {
	return 1;
}

/*
 * ht_dram_init -- hashtable initializer
 */
int ht_dram_init(const char *path) {
    size_t len = INIT_BUCKETS_NUM;

    root_p = (struct hashtable *) malloc(sizeof(struct hashtable));
    root_p->seed = (uint32_t)time(NULL);
    do {
        root_p->hash_fun_coeff_a = (uint32_t) rand();
    } while(root_p->hash_fun_coeff_a == 0);

    root_p->hash_fun_coeff_b = (uint32_t) rand();
    root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;
    root_p->count = 0;
    root_p->buckets = (struct buckets *) malloc(sizeof(struct buckets));
    root_p->buckets->nbuckets = len;
    root_p->buckets->bucket = (struct entry **) calloc(len, sizeof(struct entry));

    return 1;
}

/**
 * ht_dram_read -- read 'value' of 'key' and place it into '&result'
 */
int ht_dram_read(const char *key, void *result) {
	ht_dram_check();

	struct buckets *buckets_p = root_p->buckets;
	uint64_t uint64_key = strtoull(key, NULL, 0);
	uint64_t hash_value = dram_hash_function(uint64_key);

	//iteration_count can be used to check the number of iteration needed to find the value of a single key
	int iteration_count = 0;

	for(struct entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
		if(entry_p->key == uint64_key) {
			//key found! replace the value and return
			result = entry_p->value;
		}
		iteration_count += 1;
	}
    return 1;
}

/**
 * ht_dram_update -- update 'value' of 'key' into the hashtable, will insert the 'value' if 'key' not exists
 */
int ht_dram_update(const char *key, void *value) {
    return ht_dram_insert(key, value);
}

/**
 * ht_dram_new_entry -- create new hashtable entry
 */
struct entry* ht_dram_new_entry(uint64_t key, const char* value) {
    struct entry *entry_p = (struct entry *) malloc(sizeof(struct entry));
    entry_p->key = key;
    memcpy(entry_p->value, (char *) value, sizeof(value));
	entry_p->next = (struct entry *) malloc(sizeof(struct entry));
    return entry_p;
}

/**
 * ht_dram_insert -- inserts 'value' into the hashtable, will update the 'value' if 'key' already exists
 */
int ht_dram_insert(const char *key, void *value) {
	ht_dram_check();

	struct buckets *buckets_p = root_p->buckets;
	uint64_t uint64_key = strtoull(key, NULL, 0);
	uint64_t hash_value = dram_hash_function(uint64_key);

	//iteration_count can be used further to update the size of buckets with condition
	int iteration_count = 0;

	for(struct entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
		if(entry_p->key == uint64_key) {
			//key found! replace the value and return
			memcpy(entry_p->value, (char *) value, strlen((char *) value));
			return 1;
		}
		iteration_count += 1;
	}

	//key not found! need to insert data into bucket[hash_value]
	struct entry *entry_p = ht_dram_new_entry(uint64_key, (const char*) value);
	entry_p->next = buckets_p->bucket[hash_value];
	buckets_p->bucket[hash_value] = entry_p;

	root_p->count += 1;
	iteration_count += 1;

	//note: acording to the value of 'iteration_count', 
	//we can add custom logic to reinitialize the hash table with new bigger bucket size
    return 1;
}

/**
 * ht_dram_free_entry_chain -- free memory allocated in a hashtable bucket entry chain
 */
void ht_dram_free_entry_chain(struct entry *head) {
	struct entry* entry_p;
	while(head != NULL) {
	    entry_p = head;
	    head = head->next;
	    free(entry_p);
	}
}

/**
 * ht_dram_free -- destructor
 */
void ht_dram_free() {
    for (int i=0; i < INIT_BUCKETS_NUM; i+=1) {
        struct entry *entry_p = root_p->buckets->bucket[i];
        if (entry_p != NULL) {
            ht_dram_free_entry_chain(entry_p);
        }
    }
	free(root_p->buckets->bucket);
    free(root_p->buckets);
    free(root_p);
}
