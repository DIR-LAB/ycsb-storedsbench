//
// Created by anarayanan on 10/25/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include<stdbool.h>
#include<algorithm>     //todo: remove this after implementing swap
#include<bits/stdc++.h>
#include "rbtree_common.h"

namespace ycsbc {
    class RbtreeDram : public StoredsBase {
    public:
        RbtreeDram(const char *path) {
            RbtreeDram::init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void levelorder();

        void destroy();

        ~RbtreeDram();

    private:
        /* Private Data */
        struct rbtree_dram_node *root_p = NULL;

        int check();

        void lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result);

        bool update_if_found(struct rbtree_dram_node *current_node, uint64_t key, void *value);

        void rotate_left(struct rbtree_dram_node *&ptr);

        void rotate_right(struct rbtree_dram_node *&ptr);

        struct rbtree_dram_node *bst_insert(struct rbtree_dram_node *current_node, struct rbtree_dram_node *new_node);

        void fix_violation(struct rbtree_dram_node *&current_node);

        void free_node(rbtree_dram_node *&current_node);

        struct rbtree_dram_node *create_new_node(uint64_t key, void *value);
    };

    /*
     * init -- initialize root of tree to NULL
     */
    int RbtreeDram::init(const char *path) {
        root_p = NULL;
        return 1;
    }

    /*
     * check -- (internal) check whether rb-tree is initialized or not
     */
    int RbtreeDram::check() {
        if (root_p == NULL) {
            fprintf(stderr, "[%s]: FATAL: RBTREE not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * lookup -- (internal) Read and return Value for a given Key of RBTree
     */
    void RbtreeDram::lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result) {
        //base case
        if(current_node == NULL) {
            return;
        }

        if(current_node->key == key) {
            //data found
            result = current_node->value;
            return;
        }

        if(current_node->key > key) {
            return lookup(current_node->left, key, result);
        }
        else {
            return lookup(current_node->right, key, result);
        }
    }

    /*
     * read -- Read RBTree DRAM and return value into the result variable.
     */
    int RbtreeDram::read(const char* key, void *&result) {
        check();
        uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p, uint64_key, result);
        
        return 1;
    }

    /*
     * update_if_found -- (internal) Read and update Value for a given Key of RBTree
     */
    bool RbtreeDram::update_if_found(struct rbtree_dram_node *current_node, uint64_t key, void *value) {
        //base case
        if(current_node == NULL) {
            return false;
        }

        if(current_node->key == key) {
            memcpy(current_node->value, (char *) value, strlen((char *) value) + 1);
            return true;
        }

        if(current_node->key > key) {
            return update_if_found(current_node->left, key, value);
        }
        else {
            return update_if_found(current_node->right, key, value);
        }
    }

    /*
     * update -- if key exists, update <key, value> pair. if key not exist, insert <key, value pair>
     */
    int RbtreeDram::update(const char *key, void *value) {
        check();
        insert(key, value);
        return 1;
    }

    /*
     * rotate_left -- (internal) Rotate Sub-Tree of RBTree to the Left
     */
    void RbtreeDram::rotate_left(struct rbtree_dram_node *&ptr) {
        struct rbtree_dram_node *right_p = ptr->right;
        ptr->right = right_p->left;
        if (ptr->right != NULL) {
            ptr->right->parent = ptr;
        }
        right_p->parent = ptr->parent;

        if (ptr->parent == NULL) {
            root_p = right_p;
        }
        else if (ptr == ptr->parent->left) {
            ptr->parent->left = right_p;
        }
        else {
            ptr->parent->right = right_p;
        }
        right_p->left = ptr;
        ptr->parent = right_p;
    }

    /*
     * rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreeDram::rotate_right(struct rbtree_dram_node *&ptr) {
        struct rbtree_dram_node *left_p = ptr->left;
        ptr->left = left_p->right;
        if (ptr->left != NULL) {
            ptr->left->parent = ptr;
        }
        left_p->parent = ptr->parent;

        if (ptr->parent == NULL) {
            root_p = left_p;
        }
        else if (ptr == ptr->parent->left) {
            ptr->parent->left = left_p;
        }
        else {
            ptr->parent->right = left_p;
        }
        left_p->right = ptr;
        ptr->parent = left_p;
    }

    /*
     * bst_insert -- (internal) insert into raw bst, will update the balance later scope
     */
    struct rbtree_dram_node* RbtreeDram::bst_insert(struct rbtree_dram_node *current_node, struct rbtree_dram_node *new_node) {
        if (current_node == NULL) {
            return new_node;
        }

        /* Otherwise, recur down the tree */
        if (new_node->key < current_node->key) {
            current_node->left = bst_insert(current_node->left, new_node);
            current_node->left->parent = current_node;
        }
        else {
            current_node->right = bst_insert(current_node->right, new_node);
            current_node->right->parent = current_node;
        }

        /* return the (unchanged) node pointer */
        return current_node;
    }

    /*
     * fix_violation -- (internal) Rebalance RB-Tree. This operation can be done in relaxed or active manner.
     */
    void RbtreeDram::fix_violation(struct rbtree_dram_node *&current_node) {
        struct rbtree_dram_node *parent_pt = NULL;
        struct rbtree_dram_node *grand_parent_pt = NULL;

        while ((current_node != root_p) && (current_node->color != BLACK) && (current_node->parent->color == RED)) {
            parent_pt = current_node->parent;
            grand_parent_pt = current_node->parent->parent;

            /*  Case: Parent of current_node is left child of Grand-parent of current_node */
            if (parent_pt == grand_parent_pt->left) {
                struct rbtree_dram_node *uncle_pt = grand_parent_pt->right;

                /* Case: The uncle of current_node is also red Only Recoloring required */
                if (uncle_pt != NULL && uncle_pt->color == RED) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    current_node = grand_parent_pt;
                }
                else {
                    /* Case: current_node is right child of its parent Left-rotation required */
                    if (current_node == parent_pt->right) {
                        rotate_left(parent_pt);
                        current_node = parent_pt;
                        parent_pt = current_node->parent;
                    }

                    /* Case: current_node is left child of its parent Right-rotation required */
                    rotate_right(grand_parent_pt);
                    //todo: implement swap
                    std::swap(parent_pt->color, grand_parent_pt->color);
                    current_node = parent_pt;
                }
            }

            /* Case: Parent of current_node is right child of Grand-parent of current_node */
            else {
                struct rbtree_dram_node *uncle_pt = grand_parent_pt->left;

                /*  Case: The uncle of current_node is also red Only Recoloring required */
                if ((uncle_pt != NULL) && (uncle_pt->color == RED)) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    current_node = grand_parent_pt;
                }
                else {
                    /* Case: current_node is left child of its parent Right-rotation required */
                    if (current_node == parent_pt->left) {
                        rotate_right(parent_pt);
                        current_node = parent_pt;
                        parent_pt = current_node->parent;
                    }

                    /* Case: current_node is right child of its parent Left-rotation required */
                    rotate_left(grand_parent_pt);
                    //todo: implement swap
                    std::swap(parent_pt->color, grand_parent_pt->color);
                    current_node = parent_pt;
                }
            }
        }

        root_p->color = BLACK;
    }

    /*
     * create_new_node -- (internal) allocate memory for new node
     */
    struct rbtree_dram_node * RbtreeDram::create_new_node(uint64_t key, void *value) {
        //allocate memory to new node
        struct rbtree_dram_node *new_node = (struct rbtree_dram_node *) malloc(sizeof(struct rbtree_dram_node));

        new_node->left = new_node->right = new_node->parent = NULL;
        new_node->color = RED;
        new_node->key = key;
        strcpy(new_node->value, (char *) value);

        return new_node;
    }

    /*
     * insert -- if key not exist, insert <key, value pair>. if key exists, update <key, value> pair.
     */
    int RbtreeDram::insert(const char *key, void *value) {
        uint64_t uint64_key = strtoull(key, NULL, 0);

        if(update_if_found(root_p, uint64_key, value)) return 1;
        struct rbtree_dram_node *new_node = create_new_node(uint64_key, value);

        // Do a normal BST insert
        root_p = bst_insert(root_p, new_node);
        fix_violation(new_node);
        return 1;
    }

    void RbtreeDram::free_node(rbtree_dram_node *&current){
        if(current!=NULL){
            free_node(current->left);
            free_node(current->right);

            free(current);
        }
    }
    /*
     * destroy -- Free Space of RBTree.
     */
    void RbtreeDram::destroy() {
        free_node(root_p);
    }
}   //ycsbc
