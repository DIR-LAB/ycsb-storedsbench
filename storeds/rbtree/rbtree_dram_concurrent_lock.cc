//
// Created by anarayanan on 10/25/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <pthread.h>
#include "rbtree_common.h"

namespace ycsbc {
    class RbtreeDramConcurrentLock : public StoredsBase {
    public:
        RbtreeDramConcurrentLock(const char *path) {
            RbtreeDramConcurrentLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void levelorder();

        void destroy();

        ~RbtreeDramConcurrentLock();

    private:
        /* Private Data */
        struct rbtree_dram_node *root_p = NULL;
        pthread_rwlock_t rwlock;

        int check();

        void lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result);

        void rotate_left(struct rbtree_dram_node *&ptr);

        void rotate_right(struct rbtree_dram_node *&ptr);

        struct rbtree_dram_node *bst_upsert(struct rbtree_dram_node *current_node, uint64_t key, void *value);

        void fix_violation(struct rbtree_dram_node *&current_node);

        void free_node(rbtree_dram_node *&current_node);

        struct rbtree_dram_node *create_new_node(uint64_t key, void *value);
    };

    /**
     * RbtreeDramConcurrentLock::init -- initialize root of tree to NULL
     */
    int RbtreeDramConcurrentLock::init(const char *path) {
        root_p = NULL;
        if(pthread_rwlock_init(&rwlock, 0) != 0) {
            fprintf(stderr, "[%s]: FATAL: RW Lock not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreeDramConcurrentLock::check -- (internal) check whether rb-tree is initialized or not
     */
    int RbtreeDramConcurrentLock::check() {
        if (root_p == NULL) {
            fprintf(stderr, "[%s]: FATAL: RBTREE not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    int RbtreeDramConcurrentLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * RbtreeDramConcurrentLock::lookup -- (internal) Read and return Value for a given Key of RBTree
     */
    void RbtreeDramConcurrentLock::lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result) {
        //base case
        if (current_node == NULL) {
            return;
        }

        if (current_node->key == key) {
            //data found
            result = current_node->value;
            return;
        }

        if (current_node->key > key) {
            return lookup(current_node->left, key, result);
        } else {
            return lookup(current_node->right, key, result);
        }
    }

    /**
     * RbtreeDramConcurrentLock::read -- Read RBTree DRAM and return value into the result variable.
     */
    int RbtreeDramConcurrentLock::read(const uint64_t key, void *&result) {
        check();
        if (pthread_rwlock_rdlock(&rwlock) != 0) return 0;

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p, key, result);
        pthread_rwlock_unlock(&rwlock);

        return 1;
    }

    /**
     * RbtreeDramConcurrentLock::update -- if key exists, update <key, value> pair. if key not exist, insert <key, value pair>
     */
    int RbtreeDramConcurrentLock::update(const uint64_t key, void *value) {
        check();
        return insert(key, value);
    }

    /**
     * RbtreeDramConcurrentLock::rotate_left -- (internal) Rotate Sub-Tree of RBTree to the Left
     */
    void RbtreeDramConcurrentLock::rotate_left(struct rbtree_dram_node *&ptr) {
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

    /**
     * RbtreeDramConcurrentLock::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreeDramConcurrentLock::rotate_right(struct rbtree_dram_node *&ptr) {
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

    /**
     * RbtreeDramConcurrentLock::bst_upsert -- (internal) update the value if key already exist and return NULL as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node
     * will update the balance in later scope
     */
    struct rbtree_dram_node *RbtreeDramConcurrentLock::bst_upsert(struct rbtree_dram_node *current_node, uint64_t key, void *value) {
        while (current_node != NULL) {
            if (key == current_node->key) {
                //key already exist, update the value and return
                memcpy(current_node->value, (char *) value, strlen((char *) value) + 1);

                //return status: updated!
                return NULL;
            } else if (key < current_node->key) {
                if (current_node->left == NULL) {
                    struct rbtree_dram_node *new_node = create_new_node(key, value);
                    current_node->left = new_node;
                    new_node->parent = current_node;

                    //return status: inserted!
                    return new_node;
                } else {
                    current_node = current_node->left;
                }
            } else {
                if (current_node->right == NULL) {
                    struct rbtree_dram_node *new_node = create_new_node(key, value);
                    current_node->right = new_node;
                    new_node->parent = current_node;

                    //return status: inserted!
                    return new_node;
                } else {
                    current_node = current_node->right;
                }
            }
        }

        fprintf(stderr, "[%s]: FATAL: failed to insert or update!\n", __FUNCTION__);
        assert(0);
    }

    /**
     * RbtreeDramConcurrentLock::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreeDramConcurrentLock::fix_violation(struct rbtree_dram_node *&current_node) {
        struct rbtree_dram_node *parent_pt = NULL;
        struct rbtree_dram_node *grand_parent_pt = NULL;

        while ((current_node != root_p) && (current_node->color != BLACK) && (current_node->parent->color == RED)) {
            parent_pt = current_node->parent;
            grand_parent_pt = current_node->parent->parent;

            /* case: parent is left child of grandparent node */
            if (parent_pt == grand_parent_pt->left) {
                struct rbtree_dram_node *uncle_pt = grand_parent_pt->right;

                /* case: uncle is also red, only recoloring is required */
                if (uncle_pt != NULL && uncle_pt->color == RED) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    current_node = grand_parent_pt;
                }
                else {
                    /* case: current_node is right child of its parent -> left-rotation required */
                    if (current_node == parent_pt->right) {
                        rotate_left(parent_pt);
                        current_node = parent_pt;
                        parent_pt = current_node->parent;
                    }
                    /* case: current_node is now become the left child of its parent -> right-rotation required */
                    rotate_right(grand_parent_pt);

                    bool parent_color = parent_pt->color;
                    parent_pt->color = grand_parent_pt->color;
                    grand_parent_pt->color = parent_color;

                    current_node = parent_pt;
                }
            }

            /* case: parent is right child of grandparent node */
            else {
                struct rbtree_dram_node *uncle_pt = grand_parent_pt->left;
                /* case: uncle is also red, only recoloring is required */
                if ((uncle_pt != NULL) && (uncle_pt->color == RED)) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    current_node = grand_parent_pt;
                }
                else {
                    /* case: current_node is left child of its parent -> right-rotation required */
                    if (current_node == parent_pt->left) {
                        rotate_right(parent_pt);
                        current_node = parent_pt;
                        parent_pt = current_node->parent;
                    }

                    /* case: current_node is right child of its parent -> left-rotation required */
                    rotate_left(grand_parent_pt);

                    bool parent_color = parent_pt->color;
                    parent_pt->color = grand_parent_pt->color;
                    grand_parent_pt->color = parent_color;

                    current_node = parent_pt;
                }
            }
        }

        root_p->color = BLACK;
    }

    /**
     * RbtreeDramConcurrentLock::create_new_node -- (internal) allocate memory for new node
     */
    struct rbtree_dram_node *RbtreeDramConcurrentLock::create_new_node(uint64_t key, void *value) {
        //allocate memory to new node
        struct rbtree_dram_node *new_node = (struct rbtree_dram_node *) malloc(sizeof(struct rbtree_dram_node));

        new_node->left = new_node->right = new_node->parent = NULL;
        new_node->color = RED;
        new_node->key = key;
        strcpy(new_node->value, (char *) value);

        return new_node;
    }

    /**
     * RbtreeDramConcurrentLock::insert -- if key not exist, insert <key, value pair>. if key exists, update <key, value> pair.
     */
    int RbtreeDramConcurrentLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);

        if (pthread_rwlock_wrlock(&rwlock) != 0) return 0;
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        //root is null, insert to root node
        if (root_p == NULL) {
            root_p = create_new_node(key, value);

            //fix violation will update the color
            fix_violation(root_p);
            pthread_rwlock_unlock(&rwlock);
            return 1;
        }

        // Do a normal BST insert
        struct rbtree_dram_node *new_node = bst_upsert(root_p, key, value);
        if (new_node) fix_violation(new_node);
        pthread_rwlock_unlock(&rwlock);

        return 1;
    }

    /**
     * RbtreeDramConcurrentLock::free_node -- (internal) recursively free the tree node's memory
     */
    void RbtreeDramConcurrentLock::free_node(rbtree_dram_node *&current) {
        if (current != NULL) {
            free_node(current->left);
            free_node(current->right);

            free(current);
        }
    }

    /**
     * RbtreeDramConcurrentLock::destroy -- Free Space of RBTree.
     */
    void RbtreeDramConcurrentLock::destroy() {
        check();
        free_node(root_p);
        pthread_rwlock_destroy(&rwlock);
    }
}   //ycsbc
