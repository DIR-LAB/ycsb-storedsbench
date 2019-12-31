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
#include "btree_common.h"

namespace ycsbc {
    class BTreeVmemConcurrentMLock : public StoredsBase {
    public:
        BTreeVmemConcurrentMLock(const char *path) {
            BTreeVmemConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~BTreeVmemConcurrentMLock();

    private:
        /* Private Data */
        VMEM *vmp;
        struct btree_dram_node *root;

        int check();

        int is_node_full(int nk);

        struct btree_dram_node *create_node(int _is_leaf);

        char *search(struct btree_dram_node *current_node, uint64_t key);

        void split_node(int idx, struct btree_dram_node *parent, struct btree_dram_node *child);

        void insert_not_full(struct btree_dram_node *node, uint64_t key, void *value);

        bool update_if_found(struct btree_dram_node *current_node, uint64_t key, void *value);

        void recursive_free(struct btree_dram_node *current_node);
    };

    /*
     * btree_dram_check -- (internal) checks if btree has been initialized
     */
    inline int BTreeVmemConcurrentMLock::check() {
        if (root == NULL) {
            fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * btree_dram_is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
     */
    inline int BTreeVmemConcurrentMLock::is_node_full(int nk) {
        return nk == MAX_KEYS ? 1 : 0;
    }

    /*
     * btree_dram_init -- initialize btree
     */
    int BTreeVmemConcurrentMLock::init(const char *path) {
        if ((vmp = vmem_create(path, PMEM_BTREE_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }
        root = NULL;
        return 1;
    }

    /*
     * btree_dram_create_node -- (internal) create new btree node
     */
    inline struct btree_dram_node *BTreeVmemConcurrentMLock::create_node(int _is_leaf) {
        struct btree_dram_node *new_node_p;
        if ((new_node_p = ((struct btree_dram_node *) vmem_malloc(vmp, sizeof(struct btree_dram_node)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        new_node_p->is_leaf = _is_leaf;
        new_node_p->nk = 0;

        return new_node_p;
    }

    int BTreeVmemConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * btree_dram_search -- (internal) search the node contains the key and return the value
     */
    char *BTreeVmemConcurrentMLock::search(struct btree_dram_node *current_node, uint64_t key) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while(i<current_node->nk && key > current_node->entries[i].key) {
            i += 1;
        }

        //check if we found the key
        if(key == current_node->entries[i].key) {
            //key found, return the value
            return current_node->entries[i].value;
        }

        if(current_node->is_leaf) return NULL;     //we reached to leaf, key not found

        //the node is not leaf, move to the proper child node
        return search(current_node->children[i], key);
    }

    /**
     * btree_dram_read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BTreeVmemConcurrentMLock::read(const uint64_t key, void *&result) {
        //cout << "read ... " << key  << endl;
        check();

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        //cout << uint64_key << endl;
        result = search(root, key);
        //cout << result << endl;
        return 1;
    }

    /**
     * btree_dram_update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BTreeVmemConcurrentMLock::update(const uint64_t key, void *value) {
        check();
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        insert(key, value);
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
    void BTreeVmemConcurrentMLock::split_node(int idx, struct btree_dram_node *parent, struct btree_dram_node *child) {
        struct btree_dram_node *sibling = create_node(child->is_leaf);    //new sibling node will get the same status as child
        sibling->nk = MIN_DEGREE - 1;   //new sibling child will hold the (MIN_DEGREE - 1) entries of child node

        //transfer the last (MIN_DEGREE - 1) entries of child node to it's sibling node
        for(int i=0; i<MIN_DEGREE-1; i+=1) {
            sibling->entries[i] = child->entries[i + MIN_DEGREE];
        }

        //if child is an internal node, transfer the last (MIN_DEGREE) chiddren of child node to it's sibling node
        if(child->is_leaf == LEAF_NODE_FALSE_FLAG) {
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
    void BTreeVmemConcurrentMLock::insert_not_full(struct btree_dram_node *node, uint64_t key, void *value) {
        int i = node->nk - 1;

        // if node is a leaf, insert the data to actual position and return
        if(node->is_leaf) {
            while(i>=0 && node->entries[i].key > key) {
                node->entries[i+1] = node->entries[i];
                i -= 1;
            }

            node->entries[i+1].key = key;
            memcpy(node->entries[i+1].value, (char *) value, strlen((char *) value) + 1);
            node->nk += 1;
            return;
        }

        // the node is not a leaf
        // find the child which is going to store the new data
        while(i>=0 && node->entries[i].key > key) {
            i -= 1;
        }

        //check if the child is full
        if(is_node_full(node->children[i+1]->nk)) {
            //child is full, need to split
            split_node(i+1, node, node->children[i+1]);

            //after the split, child's middle entry is pushed to parent
            //decide which children will hold the new <key,value> pair
            if(node->entries[i+1].key < key) {
                i += 1;
            }
        }
        insert_not_full(node->children[i+1], key, value);
    }

    /**
     * update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BTreeVmemConcurrentMLock::update_if_found(struct btree_dram_node *current_node, uint64_t key, void *value) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while(i<current_node->nk && key > current_node->entries[i].key) {
            i += 1;
        }

        //check if we found the key
        if(key == current_node->entries[i].key) {
            //key found, update value and return
            memcpy(current_node->entries[i].value, (char *) value, strlen((char *) value) + 1);
            return true;
        }

        if(current_node->is_leaf) return false;     //we reached to leaf, key not found

        //the node is not leaf, move to the proper child node
        return update_if_found(current_node->children[i], key, value);
    }

    /**
     * btree_dram_insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
     */
    int BTreeVmemConcurrentMLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        // if btree is empty, create root
        if(root == NULL) {
            root = create_node(LEAF_NODE_TRUE_FLAG);    //root is also a leaf
            root->entries[0].key = key;
            memcpy(root->entries[0].value, (char *) value, strlen((char *) value) + 1);
            root->nk = 1;
            return 1;
        }

        // if the key already exist in btree, update the value and return
        bool is_updated = update_if_found(root, key, value);
        if(is_updated) return 1;        //we found the key, and value has been updated

        // if root is full
        if(is_node_full(root->nk)) {
            int idx = 0;
            struct btree_dram_node *new_root = create_node(LEAF_NODE_FALSE_FLAG);    //root is not a leaf anymore
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
     * btree_dram_recursive_free -- recursively visit all the btree nodes and de-allocate memory
     */
    void BTreeVmemConcurrentMLock::recursive_free(struct btree_dram_node *current_node) {
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
    void BTreeVmemConcurrentMLock::destroy() {
        //std::cout << "destroy called" << std::endl;
        check();

        recursive_free(root);
        root = NULL;
        vmem_delete(vmp);
    }
}   //ycsbc