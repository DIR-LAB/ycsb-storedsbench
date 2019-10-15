//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <libpmemobj.h>

#include "../ex_common.h"
#include "ht_pmem_tx.h"

/*  
 * Data Structure Section
 */

/* size of the pmem object pool */
#define PM_ARRAY_POOL_SIZE    (160 * 1024 * 1024)

/* name of layout in the pool */
#define LAYOUT_NAME "ht_tx_layout"

/* large prime number used as a hashing function coefficient */
#define HASH_FUNC_COEFF_P 32212254719ULL

/* initial number of buckets */
#define INIT_BUCKETS_NUM 10

/* number of values in a bucket which trigger hashtable rebuild check */
#define MIN_HASHSET_THRESHOLD 5

/* number of values in a bucket which force hashtable rebuild */
#define MAX_HASHSET_THRESHOLD 10

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* types of allocations */
enum types {
    EDGE_TYPE,
    BASE_TYPE,

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
	PMEMoid bucket[INIT_BUCKETS_NUM];
};

struct hashtablel_tx {
	/* random number generator seed */
	uint32_t seed;

	/* hash function coefficients */
	uint32_t hash_fun_a;
	uint32_t hash_fun_b;
	uint64_t hash_fun_p;

	/* number of values inserted */
	uint64_t count;

	/* buckets */
	struct buckets buckets;
};

// POBJ_LAYOUT_BEGIN(ht_tx);
// POBJ_LAYOUT_ROOT(ht_tx, struct ht_root);
// POBJ_LAYOUT_TOID(ht_tx, struct array_elm);
// POBJ_LAYOUT_TOID(ht_tx, PMEMoid);
// POBJ_LAYOUT_TOID(ht_tx, TOID(struct array_elm));
// POBJ_LAYOUT_TOID(ht_tx, struct array_info);
// POBJ_LAYOUT_END(ht_tx);

int ht_pmem_tx_init(const char *path) {
    return 1;
}

int ht_pmem_tx_read(const char *key, void *result) {
    return 1;
}

int ht_pmem_tx_update(const char *key, void *value) {
    return 1;
}

int ht_pmem_tx_insert(const char *key, void *value) {
    return 1;
}

void ht_pmem_tx_free() {
    //
}