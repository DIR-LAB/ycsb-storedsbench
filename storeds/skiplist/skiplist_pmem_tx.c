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
#define PMEM_SKIPLIST_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "sklist_layout"

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* number of levels in skiplist */
#define SKIPLIST_LEVELS_NUM 16

/* base level of skiplist */
#define SKIPLIST_BASE_LEVEL 0

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

/* Static Global Data */
static PMEMobjpool *pop = NULL;
static PMEMoid root_oid;
static struct sk_node *root_p = NULL;

/*
 * skiplist_pmem_tx_check -- (internal) checks if skiplist is initialized or not
 */
int skiplist_pmem_tx_check() {
	if (root_oid.off == 0) {
        fprintf(stderr, "[%s]: FATAL: skiplist not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/**
 * skiplist_pmem_tx_init -- initialize skiplist
 */
int skiplist_pmem_tx_init(const char *path) {
    srand((uint32_t) time(NULL));

    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PMEM_SKIPLIST_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    }
    else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    }

	root_oid = pmemobj_root(pop, sizeof(struct sk_node));
    root_p = (struct sk_node *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
        exit(0);
    }

    // TX_BEGIN(pop) {
    //     pmemobj_tx_add_range(root_oid, 0, sizeof(struct sk_node));
        
    // } TX_ONABORT {
	// 	fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
	// 	abort();
	// } TX_END

    skiplist_pmem_tx_check();
    return 1;
}

/**
 * skiplist_pmem_tx_find -- (internal) returns path to searched node
 * or if node doesn't exist, it will return path to place where key should be.
 */
void skiplist_pmem_tx_find(uint64_t uint64_key, PMEMoid *path) {
    PMEMoid active_node_oid = root_oid;
    for(int current_level = SKIPLIST_LEVELS_NUM - 1; current_level >= 0; current_level -= 1) {
        PMEMoid current_node_oid = ((struct sk_node *) pmemobj_direct(active_node_oid))->next[current_level];
        while(current_node_oid.off != 0 && ((struct sk_node *) pmemobj_direct(current_node_oid))->entry.key < uint64_key) {
            active_node_oid = current_node_oid;
            current_node_oid = ((struct sk_node *) pmemobj_direct(active_node_oid))->next[current_level];
        }
        path[current_level] = active_node_oid;
    }
}

/**
 * skiplist_pmem_tx_read -- read value of key and sets into result
 */
int skiplist_pmem_tx_read(const char *key, void *result) {
    skiplist_pmem_tx_check();
    //printf("[%s]: PARAM: key: %s\n", __func__, key);

    uint64_t uint64_key = strtoull(key, NULL, 0);

    PMEMoid path_oid[SKIPLIST_LEVELS_NUM], possible_found_oid;
    //todo: need to pass by refference
    skiplist_pmem_tx_find(uint64_key, path_oid);
    possible_found_oid = ((struct sk_node *) pmemobj_direct(path_oid[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
    if(possible_found_oid.off != 0) {
        struct sk_node *possible_found_ptr = (struct sk_node *) pmemobj_direct(possible_found_oid);
        if(possible_found_ptr->entry.key == uint64_key) {
            result = possible_found_ptr->entry.value;
        }
    }
    //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) result);
    return 1;
}

/**
 * skiplist_pmem_tx_update -- if key exist, update the value part in skiplist
 * if key doesn't exist, insert new key-value pair into skiplist
 */
int skiplist_pmem_tx_update(const char *key, void *value) {
    skiplist_pmem_tx_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);

    skiplist_pmem_tx_insert(key, value);
    return 1;
}

/**
 * skiplist_pmem_tx_insert_node -- (internal) insert new node in selected places
 * this function must be called from a transaction block
 */
void skiplist_pmem_tx_insert_node(PMEMoid new_node_oid, PMEMoid path_oid[SKIPLIST_LEVELS_NUM]) {
    int current_level = SKIPLIST_BASE_LEVEL;

    // certainly assign node to level 0, and randomly for other levels
    do {
        struct sk_node *path_current_level_ptr = (struct sk_node *) pmemobj_direct(path_oid[current_level]);
        struct sk_node *new_node_ptr = (struct sk_node *) pmemobj_direct(new_node_oid);
        pmemobj_tx_add_range_direct(&path_current_level_ptr->next[current_level], sizeof(struct sk_node));
        new_node_ptr->next[current_level] = path_current_level_ptr->next[current_level];
        path_current_level_ptr->next[current_level] = new_node_oid;
    } while(++current_level < SKIPLIST_LEVELS_NUM && !(rand() % 2));
}

/**
 * skiplist_pmem_tx_insert -- insert new key-value pair into skiplist
 * if key already exist, update the value part in skiplist
 */
int skiplist_pmem_tx_insert(const char *key, void *value) {
    skiplist_pmem_tx_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);
    uint64_t uint64_key = strtoull(key, NULL, 0);
    PMEMoid path_oid[SKIPLIST_LEVELS_NUM], possible_found_oid;

    skiplist_pmem_tx_find(uint64_key, path_oid);
    possible_found_oid = ((struct sk_node *) pmemobj_direct(path_oid[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];

    struct sk_node *possible_found_ptr = (struct sk_node *) pmemobj_direct(possible_found_oid);
    if(possible_found_oid.off != 0 && possible_found_ptr->entry.key == uint64_key) {
        //key already exist, update value
        TX_BEGIN(pop) {
            pmemobj_tx_add_range_direct(possible_found_ptr, sizeof(struct sk_entry));
            memcpy(possible_found_ptr->entry.value, (char *) value, strlen((char *) value));
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
    }
    else {
        //key not exist, insert the new node
        TX_BEGIN(pop) {
            PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct sk_node), NODE_TYPE);
            struct sk_node *new_node_ptr = (struct sk_node *) pmemobj_direct(new_node_oid);
            new_node_ptr->entry.key = uint64_key;
            memcpy(new_node_ptr->entry.value, (char *) value, strlen((char *) value));
            skiplist_pmem_tx_insert_node(new_node_oid, path_oid);
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
    }
    return 1;
}

/*
 * skiplist_pmem_tx_remove_node -- (internal) removes selected node
 * this function must be called from a transaction block
 */
void skiplist_pmem_tx_remove_node(PMEMoid path[SKIPLIST_LEVELS_NUM]) {
    PMEMoid possible_remove = ((struct sk_node *) pmemobj_direct(path[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
    struct sk_node *possible_remove_ptr = (struct sk_node *) pmemobj_direct(possible_remove);
    for(int i=SKIPLIST_BASE_LEVEL; i<SKIPLIST_LEVELS_NUM; i+=1) {
        struct sk_node *path_cur_level_ptr = (struct sk_node *) pmemobj_direct(path[i]);
        struct sk_node *path_cur_level_next_ptr = (struct sk_node *) pmemobj_direct(path_cur_level_ptr->next[i]);
        if(path_cur_level_ptr->next[i].off != 0 && (path_cur_level_next_ptr->entry.key == possible_remove_ptr->entry.key)) {
            pmemobj_tx_add_range_direct(&path_cur_level_ptr->next[i], sizeof(struct sk_node));
            //struct sk_node *tmp = path[i]->next[i];
            path_cur_level_ptr->next[i] = possible_remove_ptr->next[i];
            //free(tmp);
        }
    }
    //pmemobj_tx_add_range_direct(&possible_remove, sizeof(struct sk_node));
    //pmemobj_tx_free(possible_remove);
    //todo: need careful analysis if this memory de-allocation is the right way or not!
}

/*
 * skiplist_pmem_tx_remove_free -- (internal) removes key-value pair from the list
 * this function must be called from a transaction block
 */
void skiplist_pmem_tx_remove_free(uint64_t key) {
    PMEMoid path[SKIPLIST_LEVELS_NUM], possible_remove;
    skiplist_pmem_tx_find(key, path);
    possible_remove = ((struct sk_node *) pmemobj_direct(path[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
    if(possible_remove.off != 0 
        && ((struct sk_node *) pmemobj_direct(possible_remove))->entry.key == key) {
        skiplist_pmem_tx_remove_node(path);
    }
}

/**
 * skiplist_pmem_tx_free -- deallocate memory of skiplist
 */
//todo: need to work on this
void skiplist_pmem_tx_free() {
    skiplist_pmem_tx_check();
    TX_BEGIN(pop) {
        while(root_p->next[0].off != 0) {
            PMEMoid next_oid = root_p->next[0];
            skiplist_pmem_tx_remove_free(((struct sk_node *) pmemobj_direct(next_oid))->entry.key);
        }
        pmemobj_tx_add_range_direct(root_p, sizeof(struct sk_node));
        pmemobj_tx_free(root_oid);
        root_oid = OID_NULL;
    } TX_ONABORT {
        fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
        abort();
    } TX_END
}