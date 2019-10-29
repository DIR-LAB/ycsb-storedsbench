//
// Created by aislam6 on 10/27/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "btree_pmem_tx.h"

/*  
 * Data Structure Section
 */

/* size of the pmem object pool -- 1 GB */
#define PMEM_BTREE_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "btree_layout"

/* minimum degree - every node (except root) must contain (MIN_DEGREE - 1) keys */
/* all nodes (including root) may contain at most (2*MIN_DEGREE - 1) keys */
#define MIN_DEGREE 3

/* maximum keys a node can hold */
#define MAX_KEYS (2 * MIN_DEGREE - 1)

/* maximum children a node can have */
#define MAX_CHILDREN (2 * MIN_DEGREE)

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* types of allocations */
enum types {
    NODE_TYPE,
    ENTRY_TYPE,

    MAX_TYPES
};

/* declaration of data-structures */
struct entry {
    uint64_t key;
	char value[DEFAULT_VALUE_LEN];
};

struct btree_node {
    //flag to check if the node is leaf or not
    bool is_leaf;

    //current number of keys
    int nk;

    //array of <key-value> entries
    struct entry entries[MAX_KEYS];

    //array of child pointer
    PMEMoid children[MAX_CHILDREN];
};

/* Static Global Data */
static PMEMobjpool *pop = NULL;
static PMEMoid root_oid;
static struct btree_node *root_p = NULL;

/*
 * btree_pmem_tx_check -- (internal) checks if btree has been initialized
 */
int btree_pmem_tx_check() {
    if (root_oid.off == 0) {
        fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/*
 * btree_pmem_tx_is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
 */
int btree_pmem_tx_is_node_full(int nk) {
    return nk == MAX_KEYS ? 1 : 0;
}

/*
 * btree_pmem_tx_init -- initialize btree
 */
int btree_pmem_tx_init(const char *path) {
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PMEM_BTREE_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    } else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            exit(0);
        }
    }

	root_oid = pmemobj_root(pop, sizeof(struct btree_node));
    root_p = (struct btree_node *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
        exit(0);
    }

    btree_pmem_tx_check();
    return 1;
}

/*
 * btree_pmem_tx_create_node -- (internal) create new btree node
 * this function must be called from a transaction block
 */
PMEMoid btree_pmem_tx_create_node(bool _is_leaf) {
    PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct btree_node), NODE_TYPE);
    struct btree_node *new_node_prt = (struct btree_node *) pmemobj_direct(new_node_oid);
    
    //struct btree_node *new_node_p = (struct btree_node *) malloc(sizeof(struct btree_node));
    new_node_prt->is_leaf = _is_leaf;
    new_node_prt->nk = 0;

    // new_node_prt->entries = pmemobj_tx_alloc(MAX_KEYS * sizeof(struct entry), ENTRY_TYPE);
    // new_node_prt->children = pmemobj_tx_alloc((MAX_CHILDREN) * sizeof(struct btree_node), NODE_TYPE);

    return new_node_oid;
}

/**
 * btree_pmem_tx_search -- (internal) search the node contains the key and return the value
 */
char *btree_pmem_tx_search(PMEMoid current_node_oid, uint64_t key) {
    int i = 0;
    struct btree_node *current_node_ptr = (struct btree_node *) pmemobj_direct(current_node_oid);

    //todo: it is possible to apply binary search here to make the search faster
    //while(i<current_node_ptr->nk && key > current_node->entries[i].key) {
    while(i < current_node_ptr->nk && key > current_node_ptr->entries[i].key) {
        i += 1;
    }

    //check if we found the key
    if(key == current_node_ptr->entries[i].key) {
        //key found, return the value
        return current_node_ptr->entries[i].value;
    }

    if(current_node_ptr->is_leaf) return NULL;     //we reached to leaf, key not found

    //the node is not leaf, move to the proper child node
    return btree_pmem_tx_search(current_node_ptr->children[i], key);
}

/**
 * btree_pmem_tx_read -- read 'value' of 'key' from btree and place it into '&result'
 */
int btree_pmem_tx_read(const char *key, void *result) {
    btree_pmem_tx_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    result = btree_pmem_tx_search(root_oid, uint64_key);
    return 1;
}

/**
 * btree_pmem_tx_update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
 */
int btree_pmem_tx_update(const char *key, void *value) {
    btree_pmem_tx_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
    btree_pmem_tx_insert(key, value);
    return 1;
}

/**
 * btree_pmem_tx_split_node -- (internal) split the children of the child node equally with the new sibling node
 * 
 * so, after this split, both the child and sibling node will hold MIN_DEGREE children,
 * one children will be pushed to the parent node.
 * 
 * this function will be called when the child node is full and become idx'th child of the parent,
 * the new sibling node will be the (idx+1)'th child of the parent.
 */
void btree_pmem_tx_split_node(int idx, struct btree_node *parent, struct btree_node *child) {
}

/**
 * btree_dram_insert_not_full -- (internal) inserts <key, value> pair into node
 * when this function called, we can assume that the node has space to hold new data
 */
void btree_pmem_tx_insert_not_full(struct btree_node *node, uint64_t key, void *value) {
}

/**
 * btree_pmem_tx_update_if_found -- (internal) search the key and if exist, update the value
 */
bool btree_pmem_tx_update_if_found(struct btree_node *current_node, uint64_t key, void *value) {
    return true;
}

/**
 * btree_pmem_tx_insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
 */
int btree_pmem_tx_insert(const char *key, void *value) {
    //todo: start from here
    /*
    //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
    uint64_t uint64_key = strtoull(key, NULL, 0);

    // if btree is empty, create root
    if(root == NULL) {
        root = btree_dram_create_node(true);    //root is also a leaf
        root->entries[0].key = uint64_key;
        memcpy(root->entries[0].value, (char *) value, strlen((char *) value));
        root->nk = 1;
        return 1;
    }

    // if the key already exist in btree, update the value and return
    bool is_updated = update_if_found(root, uint64_key, value);
    if(is_updated) return 1;        //we found the key, and value has been updated

    // if root is full
    if(btree_dram_is_node_full(root->nk)) {
        int idx = 0;
        struct btree_node *new_root = btree_dram_create_node(false);    //root is not a leaf anymore
        new_root->children[idx] = root;
        btree_dram_split_node(idx, new_root, root);

        //new_root is holding two children now, decide which children will hold the new <key,value> pair
        if(new_root->entries[idx].key < uint64_key) {
            idx += 1;
        }

        //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
        btree_dram_insert_not_full(new_root->children[idx], uint64_key, value);
        //update the root
        root = new_root;
    }
    else {
        //root is not full, go ahead and insert the data to proper place
        btree_dram_insert_not_full(root, uint64_key, value);
    }
    */

    return 1;
}

/**
 * btree_pmem_tx_recursive_free -- recursively visit all the btree nodes and de-allocate memory
 * this function must be called in a transaction block
 */
void btree_pmem_tx_recursive_free(PMEMoid current_node_oid) {
    struct btree_node *current_node_ptr = (struct btree_node *) pmemobj_direct(current_node_oid);
    //base case
    if(current_node_ptr->is_leaf) {
        //free the current_node's memory and return
        pmemobj_tx_add_range_direct(&current_node_oid, sizeof(struct btree_node));
        pmemobj_tx_free(current_node_oid);
        return;
    }

    //recursively call all the child nodes
    for(int i=0; i<current_node_ptr->nk+1; i+=1) {
        btree_pmem_tx_recursive_free(current_node_ptr->children[i]);
    }

    //free the current_node's memory
    pmemobj_tx_add_range_direct(&current_node_oid, sizeof(struct btree_node));
    pmemobj_tx_free(current_node_oid);
}

/**
 * btree_pmem_tx_free -- destructor
 */
void btree_pmem_tx_free() {
    btree_pmem_tx_check();

    TX_BEGIN(pop) {
        btree_pmem_tx_recursive_free(root_oid);
        root_oid = OID_NULL;
    } TX_ONABORT {
        fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
        abort();
    } TX_END
}