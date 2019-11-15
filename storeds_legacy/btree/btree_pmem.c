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
#include "btree_pmem.h"

/*  
 * Data Structure Section
 */

/* size of the pmem object pool -- 1 GB */
#define PMEM_BTREE_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "btree_layout"

/* minimum degree - every node (except root) must contain (MIN_DEGREE - 1) keys */
/* all nodes (including root) may contain at most (2*MIN_DEGREE - 1) keys */
#define MIN_DEGREE 9

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
static int total_nodes = 0;         //store the number of nodes the btree have

/*
 * btree_pmem_check -- (internal) checks if btree has been initialized
 */
int btree_pmem_check() {
    if (root_oid.off == 0) {
        fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/*
 * btree_pmem_is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
 */
int btree_pmem_is_node_full(int nk) {
    return nk == MAX_KEYS ? 1 : 0;
}

/*
 * btree_pmem_create_node -- (internal) create new btree node
 */
PMEMoid btree_pmem_create_node(bool _is_leaf) {
    total_nodes += 1;
    PMEMoid new_node_oid;
    pmemobj_alloc(pop, &new_node_oid, sizeof(struct btree_node), NODE_TYPE, NULL, NULL);
    struct btree_node *new_node_prt = (struct btree_node *) pmemobj_direct(new_node_oid);
    
    new_node_prt->is_leaf = _is_leaf;
    new_node_prt->nk = 0;

    return new_node_oid;
}

/*
 * btree_pmem_init -- initialize btree
 */
int btree_pmem_init(const char *path) {
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
    root_oid = btree_pmem_create_node(true);    //root is also a leaf
    root_p = (struct btree_node *) pmemobj_direct(root_oid);

    if(root_p == NULL) {
        printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
        exit(0);
    }

    pmemobj_persist(pop, root_p, sizeof(struct btree_node));
    btree_pmem_check();
    return 1;
}

/**
 * btree_pmem_search -- (internal) search the node contains the key and return the value
 */
char *btree_pmem_search(PMEMoid current_node_oid, uint64_t key) {
    int i = 0;
    struct btree_node *current_node_ptr = (struct btree_node *) pmemobj_direct(current_node_oid);

    //todo: it is possible to apply binary search here to make the search faster
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
    return btree_pmem_search(current_node_ptr->children[i], key);
}

/**
 * btree_pmem_read -- read 'value' of 'key' from btree and place it into '&result'
 */
int btree_pmem_read(const char *key, void *&result) {
    btree_pmem_check();
    //printf("[%s]: PARAM: key: %s\n", __func__, key);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    result = btree_pmem_search(root_oid, uint64_key);
    return 1;
}

/**
 * btree_pmem_update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
 */
int btree_pmem_update(const char *key, void *value) {
    btree_pmem_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
    btree_pmem_insert(key, value);
    return 1;
}

/**
 * btree_pmem_split_node -- (internal) split the children of the child node equally with the new sibling node
 * 
 * so, after this split, both the child and sibling node will hold MIN_DEGREE children,
 * one children will be pushed to the parent node.
 * 
 * this function will be called when the child node is full and become idx'th child of the parent,
 * the new sibling node will be the (idx+1)'th child of the parent.
 */
void btree_pmem_split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid) {
    struct btree_node *parent_ptr = (struct btree_node *) pmemobj_direct(parent_oid);
    struct btree_node *child_ptr = (struct btree_node *) pmemobj_direct(child_oid);

    PMEMoid sibling_oid = btree_pmem_create_node(child_ptr->is_leaf);    //new sibling node will get the same status as child
    struct btree_node *sibling_ptr = (struct btree_node *) pmemobj_direct(sibling_oid);

    sibling_ptr->nk = MIN_DEGREE - 1;   //new sibling child will hold the (MIN_DEGREE - 1) entries of child node

    //transfer the last (MIN_DEGREE - 1) entries of child node to it's sibling node
    for(int i=0; i<MIN_DEGREE-1; i+=1) {
        sibling_ptr->entries[i] = child_ptr->entries[i + MIN_DEGREE];
        //todo: it is possible to free few of child's memory, or can try with pmemobj_memmove
    }

    //if child is an internal node, transfer the last (MIN_DEGREE) chiddren of child node to it's sibling node
    if(child_ptr->is_leaf == false) {
        for(int i=0; i<MIN_DEGREE; i+=1) {
            sibling_ptr->children[i] = child_ptr->children[i + MIN_DEGREE];
            //todo: it is possible to free few of child's memory, or can try with pmemobj_memmove
        }
    }

    child_ptr->nk = MIN_DEGREE - 1;

    //as parent node is going to get a new child at (idx+1)-th place, make a room for it
    for(int i = parent_ptr->nk; i >= idx+1; i -= 1) {
        parent_ptr->children[i+1] = parent_ptr->children[i];
    }

    //place sibling node as parent's children
    parent_ptr->children[idx+1] = sibling_oid;

    //a entry of child node will move to the parent node, make a room for it
    for(int i = parent_ptr->nk-1; i >= idx; i -= 1) {
        parent_ptr->entries[i+1] = parent_ptr->entries[i];
    }

    //place the middle entry of child node to parent node
    parent_ptr->entries[idx] = child_ptr->entries[MIN_DEGREE - 1];

    //parent now hold a new entry, so increasing the number of keys
    parent_ptr->nk += 1;

    //persist changes
    pmemobj_persist(pop, sibling_ptr, sizeof(struct btree_node));
    pmemobj_persist(pop, child_ptr, sizeof(struct btree_node));
    pmemobj_persist(pop, parent_ptr, sizeof(struct btree_node));
}

/**
 * btree_pmem_insert_not_full -- (internal) inserts <key, value> pair into node
 * when this function called, we can assume that the node has space to hold new data
 */
void btree_pmem_insert_not_full(PMEMoid node_oid, uint64_t key, void *value) {
    struct btree_node *node_ptr = (struct btree_node *) pmemobj_direct(node_oid);
    int i = node_ptr->nk - 1;

    // if node is a leaf, insert the data to actual position and return
    if(node_ptr->is_leaf) {
        while(i>=0 && node_ptr->entries[i].key > key) {
            node_ptr->entries[i+1] = node_ptr->entries[i];
            i -= 1;
        }

        node_ptr->entries[i+1].key = key;
        memcpy(node_ptr->entries[i+1].value, (char *) value, strlen((char *) value) + 1);
        node_ptr->nk += 1;

        //persist the changes
        pmemobj_persist(pop, node_ptr, sizeof(struct btree_node));
        return;
    }

    // the node is not a leaf
    // find the child which is going to store the new data
    while(i>=0 && node_ptr->entries[i].key > key) {
        i -= 1;
    }

    //check if the child is full
    if(btree_pmem_is_node_full(((struct btree_node *) pmemobj_direct(node_ptr->children[i+1]))->nk)) {
        //child is full, need to split
        btree_pmem_split_node(i+1, node_oid, node_ptr->children[i+1]);

        //after the split, child's middle entry is pushed to parent
        //decide which children will hold the new <key,value> pair
        if(node_ptr->entries[i+1].key < key) {
            i += 1;
        }
    }
    
    btree_pmem_insert_not_full(node_ptr->children[i+1], key, value);
}

/**
 * btree_pmem_update_if_found -- (internal) search the key and if exist, update the value
 */
bool btree_pmem_update_if_found(PMEMoid current_node_oid, uint64_t key, void *value) {
    int i = 0;
    struct btree_node *current_node_ptr = (struct btree_node *) pmemobj_direct(current_node_oid);

    //todo: it is possible to apply binary search here to make the search faster
    while(i < current_node_ptr->nk && key > current_node_ptr->entries[i].key) {
        i += 1;
    }

    //check if we found the key
    if(key == current_node_ptr->entries[i].key) {
        //key found, update value and return
        memcpy(current_node_ptr->entries[i].value, (char *) value, strlen((char *) value) + 1);
        pmemobj_persist(pop, &current_node_ptr->entries[i], strlen((char *) value));
        return true;
    }

    if(current_node_ptr->is_leaf) return false;     //basecase: we reached to leaf, key not found

    //the node is not leaf, move to the proper child node
    return btree_pmem_update_if_found(current_node_ptr->children[i], key, value);
}

/**
 * btree_pmem_insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
 */
int btree_pmem_insert(const char *key, void *value) {
    btree_pmem_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
    
    uint64_t uint64_key = strtoull(key, NULL, 0);

    // if the key already exist in btree, update the value and return
    bool is_updated = btree_pmem_update_if_found(root_oid, uint64_key, value);
    if(is_updated) return 1;        //we found the key, and value has been updated

    // if root is full
    if(btree_pmem_is_node_full(root_p->nk)) {
        int idx = 0;

        PMEMoid new_root_oid = btree_pmem_create_node(false);    //root is not a leaf anymore
        struct btree_node *new_root_ptr = (struct btree_node *) pmemobj_direct(new_root_oid);
        
        new_root_ptr->children[idx] = root_oid;
        btree_pmem_split_node(idx, new_root_oid, root_oid);

        //new_root is holding two children now, decide which children will hold the new <key,value> pair
        if(new_root_ptr->entries[idx].key < uint64_key) {
            idx += 1;
        }

        //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
        btree_pmem_insert_not_full(new_root_ptr->children[idx], uint64_key, value);
        pmemobj_persist(pop, new_root_ptr, sizeof(struct btree_node));  //persist changes on new_root_ptr

        //update the root
        root_oid = new_root_oid;

        //as root_oid updated, reassign root_p
        root_p = (struct btree_node *) pmemobj_direct(root_oid);
        pmemobj_persist(pop, root_p, sizeof(struct btree_node));    //persist changes on root_p
    }
    else {
        //root is not full, go ahead and insert the data to proper place
        btree_pmem_insert_not_full(root_oid, uint64_key, value);
    }

    return 1;
}

/**
 * btree_pmem_recursive_free -- recursively visit all the btree nodes and de-allocate memory
 */
void btree_pmem_recursive_free(PMEMoid current_node_oid) {
    struct btree_node *current_node_ptr = (struct btree_node *) pmemobj_direct(current_node_oid);
    //base case
    if(current_node_ptr->is_leaf) {
        //free the current_node's memory and return
        pmemobj_free(&current_node_oid);
        return;
    }

    //recursively call all the child nodes
    for(int i=0; i<current_node_ptr->nk+1; i+=1) {
        if(current_node_ptr->children[i].off != 0) {
            btree_pmem_recursive_free(current_node_ptr->children[i]);
        }
    }

    //free the current_node's memory
    pmemobj_free(&current_node_oid);
}

/**
 * btree_pmem_free -- destructor
 */
void btree_pmem_free() {
    btree_pmem_check();

    btree_pmem_recursive_free(root_oid);
    root_oid = OID_NULL;
}