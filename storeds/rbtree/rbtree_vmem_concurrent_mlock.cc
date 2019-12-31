//
// Created by anarayanan on 10/25/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include<stdbool.h>
#include <libvmem.h>
#include "rbtree_common.h"

namespace ycsbc {
    class RbtreeVmemConcurrentMLock : public StoredsBase {
    public:
        RbtreeVmemConcurrentMLock(const char *path) {
            RbtreeVmemConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void levelorder();

        void destroy();

        ~RbtreeVmemConcurrentMLock();

    private:
        /* Private Data */
        VMEM *vmp;
        pthread_mutex_t mutex_lock_;
        struct rbtree_dram_node *root_p = NULL;

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
     * RbtreeVmemConcurrentMLock::init -- initialize root of tree to NULL
     */
    int RbtreeVmemConcurrentMLock::init(const char *path) {
        if ((vmp = vmem_create(path, PMEM_RB_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }

        if(pthread_mutex_init(&mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }
        root_p = NULL;
        return 1;
    }

    /**
     * RbtreeVmemConcurrentMLock::check -- (internal) check whether rb-tree is initialized or not
     */
    int RbtreeVmemConcurrentMLock::check() {
        if (root_p == NULL) {
            fprintf(stderr, "[%s]: FATAL: RBTREE not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreeVmemConcurrentMLock::lookup -- (internal) Read and return Value for a given Key of RBTree
     */
    void RbtreeVmemConcurrentMLock::lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result) {
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

    int RbtreeVmemConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * RbtreeVmemConcurrentMLock::read -- Read RBTree DRAM and return value into the result variable.
     */
    int RbtreeVmemConcurrentMLock::read(const uint64_t key, void *&result) {
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;
        lookup(root_p, key, result);
        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * RbtreeVmemConcurrentMLock::update -- if key exists, update <key, value> pair. if key not exist, insert <key, value pair>
     */
    int RbtreeVmemConcurrentMLock::update(const uint64_t key, void *value) {
        check();
        return insert(key, value);
    }

    /**
     * RbtreeVmemConcurrentMLock::rotate_left -- (internal) Rotate Sub-Tree of RBTree to the Left
     */
    void RbtreeVmemConcurrentMLock::rotate_left(struct rbtree_dram_node *&ptr) {
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
     * RbtreeVmemConcurrentMLock::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreeVmemConcurrentMLock::rotate_right(struct rbtree_dram_node *&ptr) {
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
     * RbtreeVmemConcurrentMLock::bst_upsert -- (internal) update the value if key already exist and return NULL as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node
     * will update the balance in later scope
     */
    struct rbtree_dram_node *RbtreeVmemConcurrentMLock::bst_upsert(struct rbtree_dram_node *current_node, uint64_t key, void *value) {
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
     * RbtreeVmemConcurrentMLock::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreeVmemConcurrentMLock::fix_violation(struct rbtree_dram_node *&current_node) {
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
     * RbtreeVmemConcurrentMLock::create_new_node -- (internal) allocate memory for new node
     */
    struct rbtree_dram_node *RbtreeVmemConcurrentMLock::create_new_node(uint64_t key, void *value) {
        //allocate memory to new node
        struct rbtree_dram_node *new_node;
        if ((new_node = (struct rbtree_dram_node *) vmem_malloc(vmp, sizeof(struct rbtree_dram_node))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        new_node->left = new_node->right = new_node->parent = NULL;
        new_node->color = RED;
        new_node->key = key;
        strcpy(new_node->value, (char *) value);

        return new_node;
    }

    /**
     * RbtreeVmemConcurrentMLock::insert -- if key not exist, insert <key, value pair>. if key exists, update <key, value> pair.
     */
    int RbtreeVmemConcurrentMLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        //root is null, insert to root node
        if (root_p == NULL) {
            root_p = create_new_node(key, value);

            //fix violation will update the color
            fix_violation(root_p);
            pthread_mutex_unlock(&mutex_lock_);
            return 1;
        }

        // Do a normal BST insert
        struct rbtree_dram_node *new_node = bst_upsert(root_p, key, value);
        if (new_node) fix_violation(new_node);
        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * RbtreeVmemConcurrentMLock::free_node -- (internal) recursively free the tree node's memory
     */
    void RbtreeVmemConcurrentMLock::free_node(rbtree_dram_node *&current) {
        if (current != NULL) {
            free_node(current->left);
            free_node(current->right);

            vmem_free(vmp, current);
        }
    }

    /**
     * RbtreeVmemConcurrentMLock::destroy -- Free Space of RBTree.
     */
    void RbtreeVmemConcurrentMLock::destroy() {
        check();
        free_node(root_p);
        root_p = NULL;
        pthread_mutex_destroy(&mutex_lock_);
        vmem_delete(vmp);
    }
}   //ycsbc
