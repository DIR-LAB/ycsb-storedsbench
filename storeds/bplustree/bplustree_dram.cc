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

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

        ~BPlusTreeDram();

    private:
        struct bplustree_dram_node *root;

        int check();

        int is_node_full(int nk);

        struct bplustree_dram_node *create_node(bool _is_leaf);

        /*char *search(struct bplustree_dram_node *current_node, uint64_t key);

        void split_node(int idx, struct bplustree_dram_node *parent, struct bplustree_dram_node *child);

        void insert_not_full(struct bplustree_dram_node *node, uint64_t key, void *value);

        bool update_if_found(struct bplustree_dram_node *current_node, uint64_t key, void *value);

        void recursive_free(struct bplustree_dram_node *current_node);*/
    };

    /*
     * BPlusTreeDram::check -- (internal) checks if bplus tree has been initialized
     */
    inline int BPlusTreeDram::check() {
        if (root == NULL) {
            fprintf(stderr, "[%s]: FATAL: bplustree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * BPlusTreeDram::is_node_full -- (internal) checks if bplus tree node contains max possible <key-value> pairs
     */
    inline int BPlusTreeDram::is_node_full(int nk) {
        return nk == MAX_KEYS ? 1 : 0;
    }

    /*
     * BPlusTreeDram::init -- initialize bplus tree
     */
    int BPlusTreeDram::init(const char *path) {
        root = NULL;
        return 1;
    }

    /*
     * BPlusTreeDram::create_node -- (internal) create new bplus tree node
     */
    inline struct bplustree_dram_node *BPlusTreeDram::create_node(bool _is_leaf) {
        struct bplustree_dram_node *new_node_p = (struct bplustree_dram_node *) malloc(sizeof(struct bplustree_dram_node));
        new_node_p->is_leaf = _is_leaf;
        new_node_p->nk = 0;

        return new_node_p;
    }

    /**
     * BPlusTreeDram::insert -- inserts <key, value> pair into bplus tree, will update the 'value' if 'key' already exists
     */
    int BPlusTreeDram::insert(const char *key, void *value) {
        /*//printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        uint64_t uint64_key = strtoull(key, NULL, 0);

        // if bplus tree is empty, create root
        if(root == NULL) {
            root = create_node(true);    //root is also a leaf
            root->entries[0].key = uint64_key;
            memcpy(root->entries[0].value, (char *) value, strlen((char *) value) + 1);
            root->nk = 1;
            return 1;
        }

        // if the key already exist in bplus tree, update the value and return
        bool is_updated = update_if_found(root, uint64_key, value);
        if(is_updated) return 1;        //we found the key, and value has been updated

        // if root is full
        if(is_node_full(root->nk)) {
            int idx = 0;
            struct bplustree_dram_node *new_root = create_node(false);    //root is not a leaf anymore
            new_root->children[idx] = root;
            split_node(idx, new_root, root);

            //new_root is holding two children now, decide which children will hold the new <key,value> pair
            if(new_root->entries[idx].key < uint64_key) {
                idx += 1;
            }

            //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
            insert_not_full(new_root->children[idx], uint64_key, value);
            //update the root
            root = new_root;
        }
        else {
            //root is not full, go ahead and insert the data to proper place
            insert_not_full(root, uint64_key, value);
        }*/

        return 1;
    }
}   //ycsbc