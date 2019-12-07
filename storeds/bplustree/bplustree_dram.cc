//
// Created by aislam6 on 11/22/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>

#include "bplustree_common.h"

using namespace std;

namespace ycsbc {
    class BPlusTreeDram : public StoredsBase {
    public:
        BPlusTreeDram(const char *path) {
            BPlusTreeDram::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~BPlusTreeDram();

    private:
        struct bplustree_dram_node *root;

        int check();

        int is_node_full(int nk);

        struct bplustree_dram_node *create_node(bool _is_leaf);

        char *search(struct bplustree_dram_node *current_node, uint64_t key);

        void split_node(int idx, struct bplustree_dram_node *parent, struct bplustree_dram_node *child);

        void insert_not_full(struct bplustree_dram_node *node, uint64_t key, void *value);

        bool update_if_found(struct bplustree_dram_node *current_node, uint64_t key, void *value);

        void recursive_free(struct bplustree_dram_node *current_node);
    };

    /**
     * BPlusTreeDram::check -- (internal) checks if bplus tree has been initialized
     */
    inline int BPlusTreeDram::check() {
        if (root == NULL) {
            fprintf(stderr, "[%s]: FATAL: bplustree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * BPlusTreeDram::is_node_full -- (internal) checks if bplus tree node contains max possible <key-value> pairs
     */
    inline int BPlusTreeDram::is_node_full(int nk) {
        return nk == MAX_KEYS ? 1 : 0;
    }

    /**
     * BPlusTreeDram::init -- initialize bplus tree
     */
    int BPlusTreeDram::init(const char *path) {
        root = NULL;
        return 1;
    }

    /**
     * BPlusTreeDram::search -- (internal) search the node contains the key and return the value
     */
    char *BPlusTreeDram::search(struct bplustree_dram_node *current_node, uint64_t key) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while(i<current_node->nk && key > current_node->entries[i].key) {
            i += 1;
        }

        //we reached to leaf
        if(current_node->is_leaf) {
            //check if we found the key
            if(i<MAX_KEYS && key == current_node->entries[i].key) {
                //key found, return the value
                return current_node->entries[i].value;
            }
            //key not found
            return NULL;
        }

        //the node is not leaf, move to the proper child node
        return search(current_node->children[i], key);
    }

    /**
     * BPlusTreeDram::read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BPlusTreeDram::read(const uint64_t key, void *&result) {
        check();
        result = search(root, key);
        return 1;
    }

    /**
     * BPlusTreeDram::update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BPlusTreeDram::update(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        check();
        insert(key, value);
        return 1;
    }

    /**
     * BPlusTreeDram::create_node -- (internal) create new bplus tree node
     */
    inline struct bplustree_dram_node *BPlusTreeDram::create_node(bool _is_leaf) {
        struct bplustree_dram_node *new_node_p = (struct bplustree_dram_node *) malloc(sizeof(struct bplustree_dram_node));
        new_node_p->is_leaf = _is_leaf;
        new_node_p->nk = 0;
        new_node_p->parent = new_node_p->next = new_node_p->previous = NULL;

        return new_node_p;
    }

    /**
     * BPlusTreeDram::split_node -- (internal) split the children of the child node equally with the new sibling node
     *
     * so, after this split, both the child and sibling node will hold MIN_DEGREE children,
     * one children will be pushed to the parent node.
     *
     * this function will be called when the child node is full and become idx'th child of the parent,
     * the new sibling node will be the (idx+1)'th child of the parent.
     */
    void BPlusTreeDram::split_node(int idx, struct bplustree_dram_node *parent, struct bplustree_dram_node *child) {
        //
    }

    /**
     * BPlusTreeDram::insert_not_full -- (internal) inserts <key, value> pair into node
     * when this function called, we can assume that the node has space to hold new data
     */
    void BPlusTreeDram::insert_not_full(struct bplustree_dram_node *node, uint64_t key, void *value) {
        //
    }

    /**
     * BPlusTreeDram::update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BPlusTreeDram::update_if_found(struct bplustree_dram_node *current_node, uint64_t key, void *value) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while(i<current_node->nk && key > current_node->entries[i].key) {
            i += 1;
        }

        //we reached to leaf
        if(current_node->is_leaf) {
            //check if we found the key
            if(i<MAX_KEYS && key == current_node->entries[i].key) {
                //key found, update value and return
                memcpy(current_node->entries[i].value, (char *) value, strlen((char *) value) + 1);
                return true;
            }
            //key not found
            return false;
        }

        //the node is not leaf, move to the proper child node
        return update_if_found(current_node->children[i], key, value);
    }

    /**
     * BPlusTreeDram::insert -- inserts <key, value> pair into bplus tree, will update the 'value' if 'key' already exists
     */
    int BPlusTreeDram::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);

        // if bplus tree is empty, create root
        if(root == NULL) {
            root = create_node(true);    //root is also a leaf
            root->entries[0].key = key;
            memcpy(root->entries[0].value, (char *) value, strlen((char *) value) + 1);
            root->nk = 1;
            return 1;
        }

        // if the key already exist in bplus tree, update the value and return
        bool is_updated = update_if_found(root, key, value);
        if(is_updated) return 1;        //we found the key, and value has been updated

        // if root is full
        if(is_node_full(root->nk)) {
            int idx = 0;
            struct bplustree_dram_node *new_root = create_node(false);    //root is not a leaf anymore
            new_root->children[idx] = root;
            split_node(idx, new_root, root);

            //new_root is holding two children now, decide which children will hold the new <key,value> pair
            if(new_root->entries[idx].key < key) {
                idx += 1;
            }

            //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
            insert_not_full(new_root->children[idx], key, value);
            //update the root
            root = new_root;
        }
        else {
            //root is not full, go ahead and insert the data to proper place
            insert_not_full(root, key, value);
        }

        return 1;
    }

    /**
     * BPlusTreeDram::destroy -- destructor
     */
    void BPlusTreeDram::destroy() {
        //
    }
}   //ycsbc