//
// Created by aislam6 on 10/27/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <libvmem.h>
#include "bt_common.h"

namespace ycsbc {
    class BTVmem : public StoredsBase {
    public:
        BTVmem(const char *path) {
            BTVmem::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~BTVmem();

    private:
        /* Private Data */
        VMEM *vmp;
        struct bt_dram_node *root;

        int check();

        int is_node_full(int nk);

        struct bt_dram_node *create_node(int _is_leaf);

        struct bt_entry *create_entry(uint64_t _key, void *_value);

        char *search(struct bt_dram_node *current_node, uint64_t key);

        void split_node(int idx, struct bt_dram_node *parent, struct bt_dram_node *child);

        void insert_not_full(struct bt_dram_node *node, uint64_t key, void *value);

        bool update_if_found(struct bt_dram_node *current_node, uint64_t key, void *value);

        void recursive_free(struct bt_dram_node *current_node);

        void print_node(struct bt_dram_node *current_node);
    };

    /**
     * BPDram::print_leaf_chain -- (internal) print the leaf chain to check the integrity of bplus-tree
     */
    void BTVmem::print_node(struct bt_dram_node *current_node) {
        if(current_node == NULL) return;

        printf("[");
        for (int i = 0; i < current_node->nk; i += 1) {
            printf(" <%ld, %s>", current_node->entries[i]->key, current_node->entries[i]->value);
        }
        printf("]\n");

        for (int i = 0; i <= current_node->nk; i += 1) {
            print_node(current_node->children[i]);
        }
    }

    /*
     * btree_dram_check -- (internal) checks if btree has been initialized
     */
    inline int BTVmem::check() {
        if (root == NULL) {
            fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * btree_dram_is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
     */
    inline int BTVmem::is_node_full(int nk) {
        return nk == BT_MAX_KEYS ? 1 : 0;
    }

    /*
     * btree_dram_init -- initialize btree
     */
    int BTVmem::init(const char *path) {
        if ((vmp = vmem_create(path, PMEM_BT_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }
        root = NULL;
        return 1;
    }

    /**
     * btree_dram_create_node -- (internal) create new btree node
     */
    inline struct bt_dram_node *BTVmem::create_node(int _is_leaf) {
        struct bt_dram_node *new_node_p;
        if ((new_node_p = ((struct bt_dram_node *) vmem_malloc(vmp, sizeof(struct bt_dram_node)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        new_node_p->is_leaf = _is_leaf;
        new_node_p->nk = 0;
        return new_node_p;
    }

    /**
     * BTVmem::create_entry -- (internal) create new entry
     */
    inline struct bt_entry *BTVmem::create_entry(uint64_t _key, void *_value) {
        struct bt_entry *new_entry_p;
        if ((new_entry_p = ((struct bt_entry *) vmem_malloc(vmp, sizeof(struct bt_entry)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        new_entry_p->key = _key;
        memcpy(new_entry_p->value, (char *) _value, strlen((char *) _value) + 1);
        return new_entry_p;
    }

    int BTVmem::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * btree_dram_search -- (internal) search the node contains the key and return the value
     */
    char *BTVmem::search(struct bt_dram_node *current_node, uint64_t key) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while(i<current_node->nk && key > current_node->entries[i]->key) {
            i += 1;
        }

        //check if we found the key
        if(i < current_node->nk && key == current_node->entries[i]->key) {
            //key found, return the value
            return current_node->entries[i]->value;
        }

        if(current_node->is_leaf) {
            return NULL;     //we reached to leaf, key not found
        }

        //the node is not leaf, move to the proper child node
        return search(current_node->children[i], key);
    }

    /**
     * btree_dram_read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BTVmem::read(const uint64_t key, void *&result) {
        //printf("[%s]: PARAM: key: %ld\n", __func__, key);
        check();
        result = search(root, key);
        return 1;
    }

    /**
     * btree_dram_update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BTVmem::update(const uint64_t key, void *value) {
        check();
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        return insert(key, value);
    }

    /**
     * btree_dram_split_node -- (internal) split the children of the child node equally with the new sibling node
     *
     * so, after this split, both the child and sibling node will hold BT_MIN_DEGREE children,
     * one children will be pushed to the parent node.
     *
     * this function will be called when the child node is full and become idx'th child of the parent,
     * the new sibling node will be the (idx+1)'th child of the parent.
     */
    void BTVmem::split_node(int idx, struct bt_dram_node *parent, struct bt_dram_node *child) {
        struct bt_dram_node *sibling = create_node(child->is_leaf);    //new sibling node will get the same status as child
        sibling->nk = BT_MIN_DEGREE - 1;   //new sibling child will hold the (BT_MIN_DEGREE - 1) entries of child node

        //transfer the last (BT_MIN_DEGREE - 1) entries of child node to it's sibling node
        for(int i=0; i<BT_MIN_DEGREE-1; i+=1) {
            sibling->entries[i] = child->entries[i + BT_MIN_DEGREE];
        }

        //if child is an internal node, transfer the last (BT_MIN_DEGREE) chiddren of child node to it's sibling node
        if(child->is_leaf == LEAF_NODE_FALSE_FLAG) {
            for(int i=0; i<BT_MIN_DEGREE; i+=1) {
                sibling->children[i] = child->children[i + BT_MIN_DEGREE];
            }
        }

        child->nk = BT_MIN_DEGREE - 1;

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
        parent->entries[idx] = child->entries[BT_MIN_DEGREE - 1];

        //parent now hold a new entry, so increasing the number of keys
        parent->nk += 1;
    }

    /**
     * btree_dram_insert_not_full -- (internal) inserts <key, value> pair into node
     * when this function called, we can assume that the node has space to hold new data
     */
    void BTVmem::insert_not_full(struct bt_dram_node *node, uint64_t key, void *value) {
        int i = node->nk - 1;

        // if node is a leaf, insert the data to actual position and return
        if(node->is_leaf) {
            while(i>=0 && node->entries[i]->key > key) {
                node->entries[i+1] = node->entries[i];
                i -= 1;
            }

            node->entries[i+1] = create_entry(key, value);
            node->nk += 1;
            return;
        }

        // the node is not a leaf
        // find the child which is going to store the new data
        while(i>=0 && node->entries[i]->key > key) {
            i -= 1;
        }

        //check if the child is full
        if(is_node_full(node->children[i+1]->nk)) {
            //child is full, need to split
            split_node(i+1, node, node->children[i+1]);

            //after the split, child's middle entry is pushed to parent
            //decide which children will hold the new <key,value> pair
            if(node->entries[i+1]->key < key) {
                i += 1;
            }
        }
        insert_not_full(node->children[i+1], key, value);
    }

    /**
     * update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BTVmem::update_if_found(struct bt_dram_node *current_node, uint64_t key, void *value) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while(i<current_node->nk && key > current_node->entries[i]->key) {
            i += 1;
        }

        //check if we found the key
        if(i < current_node->nk && key == current_node->entries[i]->key) {
            //key found, update value and return
            memcpy(current_node->entries[i]->value, (char *) value, strlen((char *) value) + 1);
            return true;
        }

        if(current_node->is_leaf) return false;     //we reached to leaf, key not found

        //the node is not leaf, move to the proper child node
        return update_if_found(current_node->children[i], key, value);
    }

    /**
     * btree_dram_insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
     */
    int BTVmem::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %ld, value: %s\n", __func__, key, (char *) value);

        // if btree is empty, create root
        if(root == NULL) {
            root = create_node(LEAF_NODE_TRUE_FLAG);    //root is also a leaf
            root->entries[0] = create_entry(key, value);
            root->nk = 1;
            return 1;
        }

        // if the key already exist in btree, update the value and return
        bool is_updated = update_if_found(root, key, value);
        if(is_updated) return 1;        //we found the key, and value has been updated

        // if root is full
        if(is_node_full(root->nk)) {
            int idx = 0;
            struct bt_dram_node *new_root = create_node(LEAF_NODE_FALSE_FLAG);    //root is not a leaf anymore
            new_root->children[idx] = root;
            split_node(idx, new_root, root);

            //new_root is holding two children now, decide which children will hold the new <key,value> pair
            if(new_root->entries[idx]->key < key) {
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
     * btree_dram_recursive_free -- recursively visit all the btree nodes and de-allocate memory
     */
    void BTVmem::recursive_free(struct bt_dram_node *current_node) {
        //base case
        if(current_node->is_leaf) {
            vmem_free(vmp, current_node);
            return;
        }

        //recursively call all the child nodes
        for(int i=0; i<current_node->nk+1; i+=1) {
            recursive_free(current_node->children[i]);
        }

        //free the memory
        vmem_free(vmp, current_node);
    }

    /**
     * btree_dram_free -- destructor
     */
    void BTVmem::destroy() {
        check();

        recursive_free(root);
        root = NULL;
        vmem_delete(vmp);
    }
}   //ycsbc