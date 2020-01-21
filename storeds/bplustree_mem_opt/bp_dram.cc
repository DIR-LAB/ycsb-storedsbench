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

#include "bp_common.h"

namespace ycsbc {
    class BPDram : public StoredsBase {
    public:
        BPDram(const char *path) {
            BPDram::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~BPDram();

    private:
        struct bp_dram_node *root;

        int check();

        int is_node_full(int nk);

        struct bp_dram_node *create_node(int _is_leaf);

        struct bp_record *create_record(void *_value);

        char *search(struct bp_dram_node *current_node, uint64_t key);

        void split_node(int idx, struct bp_dram_node *parent, struct bp_dram_node *child);

        void insert_not_full(struct bp_dram_node *node, uint64_t key, void *value);

        bool update_if_found(struct bp_dram_node *current_node, uint64_t key, void *value);

        void recursive_free(struct bp_dram_node *current_node);

        void print_leaf_chain();
    };

    /**
     * BPDram::check -- (internal) checks if bplus tree has been initialized
     */
    inline int BPDram::check() {
        if (root == NULL) {
            fprintf(stderr, "[%s]: FATAL: bplustree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * BPDram::print_leaf_chain -- (internal) print the leaf chain to check the integrity of bplus-tree
     */
    void BPDram::print_leaf_chain() {
        struct bp_dram_node *current_node = root;
        while (current_node != NULL && !current_node->is_leaf) {
            current_node = (struct bp_dram_node *) current_node->pointers[0];
        }

        while (current_node != NULL) {
            printf("[");
            for (int i = 0; i < current_node->nk; i += 1) {
                printf(" <%ld, %s>", current_node->keys[i], ((struct bp_record *) current_node->pointers[i])->value);
            }
            printf("] -> ");
            current_node = current_node->next;
        }
        printf("\n");
    }

    /**
     * BPDram::is_node_full -- (internal) checks if bplus tree node contains max possible <key-value> pairs
     */
    inline int BPDram::is_node_full(int nk) {
        return nk == BP_MAX_KEYS ? 1 : 0;
    }

    /**
     * BPDram::init -- initialize bplus tree
     */
    int BPDram::init(const char *path) {
        root = NULL;
        return 1;
    }

    /**
     * BPDram::scan -- perform range query
     */
    int BPDram::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();

        struct bp_dram_node *current_node = root;

        //going down upto the leaf
        while(current_node != NULL && !current_node->is_leaf) {
            int i = 0;

            while (i < current_node->nk && key > current_node->keys[i]) {
                i += 1;
            }
            current_node = (struct bp_dram_node *) current_node->pointers[i];
        }

        //reached the leaf
        if(current_node != NULL) {
            int i = 0;
            //find the starting point to start the scan
            while (i < current_node->nk && key > current_node->keys[i]) {
                i += 1;
            }

            int len_count = 0;
            //forward scan through the leaf chain
            while(current_node != NULL) {
                while (i < current_node->nk) {
                    std::vector <DB::Kuint64VstrPair> tmp;
                    tmp.push_back(std::make_pair(current_node->keys[i], ((struct bp_record *) current_node->pointers[i])->value));
                    result.push_back(tmp);

                    len_count += 1;
                    if(len_count == len) break;
                    i += 1;
                }
                if(len_count == len) break;
                current_node = current_node->next;
                i = 0;
            }
        }

        return 1;
    }

    /**
     * BPDram::search -- (internal) search the node contains the key and return the value
     */
    char *BPDram::search(struct bp_dram_node *current_node, uint64_t key) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while (i < current_node->nk && key > current_node->keys[i]) {
            i += 1;
        }

        //we reached to leaf
        if (current_node->is_leaf) {
            //check if we found the key
            if (i < current_node->nk && key == current_node->keys[i]) {
                //key found, return the value
                return ((struct bp_record *) current_node->pointers[i])->value;
            }
            //key not found
            return NULL;
        }

        //the node is not leaf, move to the proper child node
        if (i < current_node->nk && key == current_node->keys[i]) i += 1;
        return search((struct bp_dram_node *) current_node->pointers[i], key);
    }

    /**
     * BPDram::read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BPDram::read(const uint64_t key, void *&result) {
        check();
        result = search(root, key);
        return 1;
    }

    /**
     * BPDram::update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BPDram::update(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        check();
        return insert(key, value);
    }

    /**
     * BPDram::create_node -- (internal) create new bplus tree node
     */
    inline struct bp_dram_node *BPDram::create_node(int _is_leaf) {
        struct bp_dram_node *new_node_p = (struct bp_dram_node *) malloc(sizeof(struct bp_dram_node));

        if(new_node_p == NULL) {
            fprintf(stderr, "[%s]: FATAL: malloc failed\n", __FUNCTION__);
            exit(0);
        }

        new_node_p->is_leaf = _is_leaf;
        new_node_p->nk = 0;
        new_node_p->next = new_node_p->previous = NULL;

        return new_node_p;
    }

    /**
     * BPDram::create_record -- (internal) create new record for leaf node
     */
    inline struct bp_record *BPDram::create_record(void *_value) {
        struct bp_record *new_record_p = (bp_record *) malloc(sizeof(struct bp_record));
        if (new_record_p == NULL) {
            fprintf(stderr, "[%s]: FATAL: malloc failed\n", __FUNCTION__);
            exit(0);
        }

        memcpy(new_record_p->value, (char *) _value, strlen((char *) _value) + 1);
        return new_record_p;
    }

    /**
     * BPDram::update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BPDram::update_if_found(struct bp_dram_node *current_node, uint64_t key, void *value) {
        int i = 0;

        //todo: it is possible to apply binary search here to make the search faster
        while (i < current_node->nk && key > current_node->keys[i]) {
            i += 1;
        }

        //we reached to leaf
        if (current_node->is_leaf) {
            //check if we found the key
            if (i < current_node->nk && key == current_node->keys[i]) {
                //key found, update value and return
                memcpy(((struct bp_record *) current_node->pointers[i])->value, (char *) value, strlen((char *) value) + 1);
                return true;
            }
            //key not found
            return false;
        }

        //the node is not leaf, move to the proper child node
        if (i < current_node->nk && key == current_node->keys[i]) i += 1;
        return update_if_found((struct bp_dram_node *) current_node->pointers[i], key, value);
    }

    /**
     * BPDram::split_node -- (internal) split the children of the child node equally with the new sibling node
     *
     * so, after this split, both the child and sibling node will hold BP_MIN_DEGREE children,
     * one children will be pushed to the parent node.
     *
     * this function will be called when the child node is full and become idx'th child of the parent,
     * the new sibling node will be the (idx+1)'th child of the parent.
     */
    void BPDram::split_node(int idx, struct bp_dram_node *parent, struct bp_dram_node *child) {
        if (child->is_leaf) {
            //new right-sibling node will get the same status as child
            struct bp_dram_node *sibling = create_node(child->is_leaf);
            //new right-sibling node will hold the BP_MIN_DEGREE entries of child node
            sibling->nk = BP_MIN_DEGREE;

            //transfer the last BP_MIN_DEGREE entries of child node to it's sibling node
            for (int i = 0; i < BP_MIN_DEGREE; i += 1) {
                sibling->keys[i] = child->keys[i + BP_MIN_DEGREE - 1];
                sibling->pointers[i] = child->pointers[i + BP_MIN_DEGREE - 1];
            }

            //reduce the number of entries of child node
            child->nk = BP_MIN_DEGREE - 1;

            //as parent node is going to get a new child at (idx+1)-th place, make a room for it
            for (int i = parent->nk; i >= idx + 1; i -= 1) {
                parent->pointers[i + 1] = parent->pointers[i];
            }

            //place sibling node as parent's children
            parent->pointers[idx + 1] = sibling;

            //a entry of child node will move to the parent node, make a room for it
            for (int i = parent->nk - 1; i >= idx; i -= 1) {
                parent->keys[i + 1] = parent->keys[i];
            }

            //place the 0'th key of sibling node to parent node
            parent->keys[idx] = sibling->keys[0];

            //parent now hold a new key, so increasing the number of keys
            parent->nk += 1;

            //adjust leaf chain pointers
            sibling->next = child->next;
            if (sibling->next != NULL) {
                sibling->next->previous = sibling;
            }
            child->next = sibling;
            sibling->previous = child;
        } else {
            //new right-sibling node will get the same status as child
            struct bp_dram_node *sibling = create_node(child->is_leaf);
            //new sibling child will hold the (BP_MIN_DEGREE - 1) entries of child node
            sibling->nk = BP_MIN_DEGREE - 1;

            //transfer the last (BP_MIN_DEGREE - 1) entries of child node to it's sibling node
            for (int i = 0; i < BP_MIN_DEGREE - 1; i += 1) {
                sibling->keys[i] = child->keys[i + BP_MIN_DEGREE];
            }

            //as child is an internal node, transfer the last (BP_MIN_DEGREE) children pointers of child node to it's sibling node
            for (int i = 0; i < BP_MIN_DEGREE; i += 1) {
                sibling->pointers[i] = child->pointers[i + BP_MIN_DEGREE];
            }

            child->nk = BP_MIN_DEGREE - 1;

            //as parent node is going to get a new child at (idx+1)-th place, make a room for it
            for (int i = parent->nk; i >= idx + 1; i -= 1) {
                parent->pointers[i + 1] = parent->pointers[i];
            }

            //place sibling node as parent's children
            parent->pointers[idx + 1] = sibling;

            //a entry of child node will move to the parent node, make a room for it
            for (int i = parent->nk - 1; i >= idx; i -= 1) {
                parent->keys[i + 1] = parent->keys[i];
            }

            //place the middle entry of child node to parent node
            parent->keys[idx] = child->keys[BP_MIN_DEGREE - 1];

            //parent now hold a new entry, so increasing the number of keys
            parent->nk += 1;
        }
    }

    /**
     * BPDram::insert_not_full -- (internal) inserts <key, value> pair into node
     * when this function called, we can assume that the node has space to hold new data
     */
    void BPDram::insert_not_full(struct bp_dram_node *node, uint64_t key, void *value) {
        int i = node->nk - 1;

        // if node is a leaf, insert the data to actual position and return
        if (node->is_leaf) {
            while (i >= 0 && node->keys[i] > key) {
                node->keys[i + 1] = node->keys[i];
                node->pointers[i + 1] = node->pointers[i];
                i -= 1;
            }

            node->keys[i + 1] = key;
            node->pointers[i + 1] = create_record(value);
            node->nk += 1;
            return;
        }

        // the node is not a leaf, find the child which is going to store the new data
        while (i >= 0 && node->keys[i] > key) {
            i -= 1;
        }

        //check if the child is full
        if (is_node_full(((struct bp_dram_node *) node->pointers[i + 1])->nk)) {
            //child is full, need to split
            split_node(i + 1, node, (struct bp_dram_node *) node->pointers[i + 1]);

            //after the split, child's middle entry is pushed to parent
            //decide which children will hold the new <key,value> pair
            if (node->keys[i + 1] < key) {
                i += 1;
            }
        }
        insert_not_full((struct bp_dram_node *) node->pointers[i + 1], key, value);
    }

    /**
     * BPDram::insert -- inserts <key, value> pair into bplus tree, will update the 'value' if 'key' already exists
     */
    int BPDram::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %ld, value: %s\n", __func__, key, (char *) value);

        // if bplus tree is empty, create root
        if (root == NULL) {
            root = create_node(LEAF_NODE_TRUE_FLAG);    //root is also a leaf
            root->keys[0] = key;
            root->pointers[0] = create_record(value);
            root->nk = 1;
            return 1;
        }

        // if the key already exist in bplus tree, update the value and return
        bool is_updated = update_if_found(root, key, value);
        if (is_updated) return 1;

        // if root is full
        if (is_node_full(root->nk)) {
            int idx = 0;
            struct bp_dram_node *new_root = create_node(LEAF_NODE_FALSE_FLAG);    //root is not a leaf anymore
            new_root->pointers[idx] = root;
            split_node(idx, new_root, root);

            //new_root is holding two children now, decide which children will hold the new <key,value> pair
            if (new_root->keys[idx] < key) {
                idx += 1;
            }

            //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
            insert_not_full((struct bp_dram_node *) new_root->pointers[idx], key, value);
            //update the root
            root = new_root;
        } else {
            //root is not full, go ahead and insert the data to proper place
            insert_not_full(root, key, value);
        }

        return 1;
    }

    /**
     * btree_dram_recursive_free -- recursively visit all the btree nodes and de-allocate memory
     */
    void BPDram::recursive_free(struct bp_dram_node *current_node) {
        //base case
        if (current_node->is_leaf) {
            if(current_node->next != NULL) current_node->next->previous = current_node->previous;
            if(current_node->previous != NULL) current_node->previous->next = current_node->next;
            for (int i = 0; i < current_node->nk; i += 1) {
                if(current_node->pointers[i] != NULL) {
                    free(current_node->pointers[i]);
                    current_node->pointers[i] = NULL;
                }
            }
            free(current_node);
            current_node = NULL;
            return;
        }

        //recursively call all the child nodes
        for (int i = 0; i < current_node->nk + 1; i += 1) {
            recursive_free((struct bp_dram_node *) current_node->pointers[i]);
        }

        //free the memory
        free(current_node);
        current_node = NULL;
    }

    /**
     * BPDram::destroy -- destructor
     */
    void BPDram::destroy() {
        check();
        recursive_free(root);
        root = NULL;
    }
}   //ycsbc