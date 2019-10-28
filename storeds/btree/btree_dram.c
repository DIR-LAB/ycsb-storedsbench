//
// Created by aislam6 on 10/27/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "btree_dram.h"

/* minimum degree - every node (except root) must contain (MIN_DEGREE - 1) keys */
/* all nodes (including root) may contain at most (2*MIN_DEGREE - 1) keys */
#define MIN_DEGREE 3

/* maximum keys a node can hold */
#define MAX_KEYS (2 * MIN_DEGREE - 1)

/* maximum children a node can have */
#define MAX_CHILDREN (2 * MIN_DEGREE)

/* default length for value */
#define DEFAULT_VALUE_LEN 101

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
    struct entry *entries;

    //array if child pointer
    struct btree_node **children;
};

/* Static Global Data */
struct btree_node *root;

/*
 * btree_dram_check -- (internal) checks if btree has been initialized
 */
int btree_dram_check() {
    if (root == NULL) {
        fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/*
 * btree_dram_is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
 */
int btree_dram_is_node_full(int nk) {
    return nk == MAX_KEYS ? 1 : 0;
}

/*
 * btree_dram_init -- initialize btree
 */
int btree_dram_init(const char *path) {
    root = NULL;
    return 1;
}

/*
 * btree_dram_create_node -- (internal) create new btree node
 */
struct btree_node *btree_dram_create_node(bool _is_leaf) {
    struct btree_node *new_node_p = (struct btree_node *) malloc(sizeof(struct btree_node));
    new_node_p->is_leaf = _is_leaf;
    new_node_p->nk = 0;

    new_node_p->entries = (struct entry *) malloc(MAX_KEYS * sizeof(struct entry));
    new_node_p->children = (struct btree_node **) malloc((MAX_CHILDREN) * sizeof(struct entry));

    return new_node_p;
}

/**
 * btree_dram_read -- read 'value' of 'key' from btree and place it into '&result'
 */
int btree_dram_read(const char *key, void *result) {
    btree_dram_check();
    return 1;
}

/**
 * btree_dram_update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
 */
int btree_dram_update(const char *key, void *value) {
    btree_dram_check();
    return 1;
}

/**
 * btree_dram_split_node -- (internal) split the children of the child node equally with the new sibling node
 * 
 * so, after this split, both the child and sibling node will hold MIN_DEGREE children,
 * one children will be pushed to the parent node.
 * 
 * this function will be called when the child node is full and become idx'th child of the parent,
 * the new sibling node will be the (idx+1)'th child of the parent.
 */
void btree_dram_split_node(int idx, struct btree_node *parent, struct btree_node *child) {
    struct btree_node *sibling = btree_dram_create_node(child->is_leaf);    //new sibling node will get the same status as child
    sibling->nk = MIN_DEGREE - 1;   //new sibling child will hold the (MIN_DEGREE - 1) entries of child node

    //transfer the last (MIN_DEGREE - 1) entries of child node to it's sibling node
    for(int i=0; i<MIN_DEGREE-1; i+=1) {
        sibling->entries[i] = child->entries[i + MIN_DEGREE];
    }

    //if child is an internal node, transfer the last (MIN_DEGREE) chiddren of child node to it's sibling node
    if(child->is_leaf == false) {
        for(int i=0; i<MIN_DEGREE; i+=1) {
            sibling->children[i] = child->children[i + MIN_DEGREE];
        }
    }

    child->nk = MIN_DEGREE - 1;

    //as parent node is going to get a new child at (idx+1)-th place, make a room for it
    for(int i=parent->nk; i>=idx+1; i-=1) {
        parent->children[i+1] = parent->children[i];
    }

    //place sibling node as parent's children
    parent->children[idx+1] = sibling;

    //a entry of child node will move to the parent node, make a room for it
    for(int i=parent->nk-1; i>=idx; i-=1) {
        parent->entries[i+1] = parent->entries[i];
    }

    //place the middle entry of child node to parent node
    parent->entries[idx] = child->entries[MIN_DEGREE - 1];

    //parent now hold a new entry, so increasing the number of keys
    parent->nk += 1;
}

/**
 * btree_dram_insert_not_full -- (internal) inserts <key, value> pair into node
 * when this function called, we can assume that the node has space to hold new data
 */
void btree_dram_insert_not_full(struct btree_node *node, uint64_t key, void *value) {
    int i = node->nk - 1;

    // if node is a leaf, insert the data to actual position and return
    if(node->is_leaf) {
        while(i>=0 && node->entries[i].key > key) {
            node->entries[i+1] = node->entries[i];
            i -= 1;
        }

        node->entries[i+1].key = key;
        memcpy(node->entries[i+1].value, (char *) value, strlen((char *) value));
        return;
    }

    // the node is not a leaf
    // find the child which is going to store the new data
    while(i>=0 && node->entries[i].key > key) {
        i -= 1;
    }

    //check if the child is full
    if(btree_dram_is_node_full(node->children[i+1]->nk)) {
        //child is full, need to split
        btree_dram_split_node(i+1, node, node->children[i+1]);

        //after the split, child's middle entry is pushed to parent
        //decide which children will hold the new <key,value> pair
        if(node->entries[i+1].key < uint64_key) {
            i += 1;
        }
    }
    btree_dram_insert_not_full(node->entries[i+1], key, value);
}

/**
 * btree_dram_insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
 */
int btree_dram_insert(const char *key, void *value) {
    uint64_t uint64_key = strtoull(key, NULL, 0);

    // if btree is empty, create root
    if(root == NULL) {
        root = btree_dram_create_node(true);    //root is also a leaf
        root->entries[0].key = uint64_key;
        memcpy(root->entries[0].value, (char *) value, strlen((char *) value));
        root->nk = 1;
        return 1;
    }

    //todo: if the key already exist in btree, update the value and return
    // struct btree_node *found_node = btree_dram_find_node(uint64_key);
    // if(found_node != NULL) {
    //     memcpy(found_node->entries[0].value, (char *) value, strlen((char *) value));
    // }

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

    return 1;
}

/**
 * btree_dram_free -- destructor
 */
void btree_dram_free() {
    //
}