//
// Created by aislam6 on 10/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "skiplist_pmem_tx.h"

/*  
 * Data Structure Section
 */

/* size of the pmem object pool -- 1 GB */
#define PM_SKIPLIST_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "sklist_layout"

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* number of levels in skiplist */
#define SKIPLIST_LEVELS_NUM 4

/* types of allocations */
enum types {
	ROOT_TYPE,
    ENTRY_TYPE,
    NODE_TYPE,

    MAX_TYPES
};

/* declaration of data-structures */
struct sk_entry {
	uint64_t key;
	char value[DEFAULT_VALUE_LEN];
};

struct sk_node {
	/* skiplist entry */
	struct sk_entry entry;

	/* next entry pointer */
	PMEMoid next[SKIPLIST_LEVELS_NUM];
};

int skiplist_pmem_tx_init(const char *path) {
    return 1;
}

int skiplist_pmem_tx_read(const char *key, void *result) {
    return 1;
}

int skiplist_pmem_tx_update(const char *key, void *value) {
    return 1;
}

int skiplist_pmem_tx_insert(const char *key, void *value) {
    return 1;
}

void skiplist_pmem_tx_free() {
    //
}