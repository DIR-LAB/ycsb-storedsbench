//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>

#include "../../lib/ex_common.h"
#include "rbtree_common.h"

namespace ycsbc {
    class RbtreePmemTxConcurrentLock : public StoredsBase {
    public:
        RbtreePmemTxConcurrentLock(const char *path) {
            RbtreePmemTxConcurrentLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~RbtreePmemTxConcurrentLock();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct rbtree_pmem_concurrent_lock_root *root_p = NULL;

        int check();

        void lookup(PMEMoid current_node, uint64_t key, void *&result);

        PMEMoid create_new_node(uint64_t key, void *value);

        PMEMoid bst_upsert(PMEMoid current_node_oid, uint64_t key, void *value);

        void rotate_left(PMEMoid &new_node_oid);

        void rotate_right(PMEMoid &new_node_oid);

        void fix_violation(PMEMoid &current_node);

        void free_node(PMEMoid current_node_oid);
    };

    /**
     * RbtreePmemTxConcurrentLock::check -- checks if the root object has been initialized
     */
    int RbtreePmemTxConcurrentLock::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: rbtree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreePmemTxConcurrentLock::init -- rbtree initializer
     */
    int RbtreePmemTxConcurrentLock::init(const char *path) {
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, RB_LAYOUT_NAME, PMEM_RB_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        } else {
            if ((pop = pmemobj_open(path, RB_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct rbtree_pmem_concurrent_lock_root));
        root_p = (struct rbtree_pmem_concurrent_lock_root *) pmemobj_direct(root_oid);

        if (root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }

        return 1;
    }

    int RbtreePmemTxConcurrentLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * RbtreePmemTxConcurrentLock::lookup -- (internal) recursively search the value of a key
     */
    void RbtreePmemTxConcurrentLock::lookup(PMEMoid current_node, uint64_t key, void *&result) {
        //base case
        if (current_node.off == 0) {
            return;
        }

        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node);
        //data found
        if (current_node_p->key == key) {
            result = current_node_p->value;
            return;
        }

        if (current_node_p->key > key) {
            return lookup(current_node_p->left, key, result);
        } else {
            return lookup(current_node_p->right, key, result);
        }
    }

    /**
     * RbtreePmemTxConcurrentLock::read -- read 'value' of 'key' and place it into '&result'
     */
    int RbtreePmemTxConcurrentLock::read(const uint64_t key, void *&result) {
        check();

        if (pmemobj_rwlock_rdlock(pop, &root_p->rwlock) != 0) return 0;

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p->root_node_oid, key, result);
        pmemobj_rwlock_unlock(pop, &root_p->rwlock);

        return 1;
    }

    /**
     * RbtreePmemTxConcurrentLock::update -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmemTxConcurrentLock::update(const uint64_t key, void *value) {
        //check();
        return insert(key, value);
    }

    /**
     * RbtreePmemTxConcurrentLock::create_new_node -- (internal) allocate memory for new node
     * this function must be called from a transaction block
     */
    PMEMoid RbtreePmemTxConcurrentLock::create_new_node(uint64_t key, void *value) {
        //prepare new in-memory node
        struct rbtree_pmem_node *in_memory_node_p = (struct rbtree_pmem_node *) malloc(sizeof(struct rbtree_pmem_node));
        in_memory_node_p->left = in_memory_node_p->right = in_memory_node_p->parent = OID_NULL;
        in_memory_node_p->color = RED;
        in_memory_node_p->key = key;
        strcpy(in_memory_node_p->value, (char *) value);

        //copy in-memory node to pmem-node
        PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct rbtree_pmem_node), RB_NODE_TYPE);
        pmemobj_tx_add_range(new_node_oid, 0, sizeof(struct rbtree_pmem_node));
        struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);
        memcpy(new_node_p, in_memory_node_p, sizeof(struct rbtree_pmem_node));

        //freeing in-memory node
        free(in_memory_node_p);

        return new_node_oid;
    }

    /**
     * RbtreePmemTxConcurrentLock::bst_upsert -- (internal) update the value if key already exist and return a NULL OID as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node OID
     * will update the balance in later scope
     */
    PMEMoid RbtreePmemTxConcurrentLock::bst_upsert(PMEMoid current_node_oid, uint64_t key, void *value) {
        while (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            if (key == current_node_p->key) {
                //key already exist, update the value and return
                TX_BEGIN(pop) {
                    pmemobj_tx_add_range_direct(current_node_p, sizeof(struct rbtree_pmem_node));
                    memcpy(current_node_p->value, (char *) value, strlen((char *) value) + 1);
                } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
                } TX_END

                //return status: updated!
                return OID_NULL;
            } else if (key < current_node_p->key) {
                if (current_node_p->left.off == 0) {
                    PMEMoid new_node_oid = OID_NULL;
                    TX_BEGIN(pop) {
                        pmemobj_tx_add_range_direct(current_node_p, sizeof(struct rbtree_pmem_node));
                        new_node_oid = create_new_node(key, value);
                        struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);

                        current_node_p->left = new_node_oid;
                        new_node_p->parent = current_node_oid;
                    } TX_ONABORT {
                        if(new_node_oid.off != 0) {
                            pmemobj_free(&new_node_oid);
                        }
                        fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                        abort();
                    } TX_END

                    //return status: inserted!
                    return new_node_oid;
                } else {
                    current_node_oid = current_node_p->left;
                }
            } else {
                if (current_node_p->right.off == 0) {
                    PMEMoid new_node_oid = OID_NULL;
                    TX_BEGIN(pop) {
                        pmemobj_tx_add_range_direct(current_node_p, sizeof(struct rbtree_pmem_node));
                        new_node_oid = create_new_node(key, value);
                        struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);

                        current_node_p->right = new_node_oid;
                        new_node_p->parent = current_node_oid;
                    } TX_ONABORT {
                        if(new_node_oid.off != 0) {
                            pmemobj_free(&new_node_oid);
                        }
                        fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                        abort();
                    } TX_END

                    //return status: inserted!
                    return new_node_oid;
                } else {
                    current_node_oid = current_node_p->right;
                }
            }
        }

        fprintf(stderr, "[%s]: FATAL: failed to insert or update!\n", __FUNCTION__);
        assert(0);
    }

    /**
     * RbtreePmemTxConcurrentLock::rotate_left -- (internal) Rebalance RB-Tree.
     * this function must be called from a transaction block
     */
    void RbtreePmemTxConcurrentLock::rotate_left(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
        pmemobj_tx_add_range_direct(current_node_p, sizeof(struct rbtree_pmem_node));

        PMEMoid old_right_node_oid = current_node_p->right;
        struct rbtree_pmem_node *old_right_node_p = (struct rbtree_pmem_node *) pmemobj_direct(old_right_node_oid);
        pmemobj_tx_add_range_direct(old_right_node_p, sizeof(struct rbtree_pmem_node));

        current_node_p->right = old_right_node_p->left;

        if (current_node_p->right.off != 0) {
            struct rbtree_pmem_node *new_right_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->right);
            pmemobj_tx_add_range_direct(new_right_node_p, sizeof(struct rbtree_pmem_node));
            new_right_node_p->parent = current_node_oid;
        }

        old_right_node_p->parent = current_node_p->parent;

        if (current_node_p->parent.off == 0) {
            pmemobj_tx_add_range(root_p->root_node_oid, 0, sizeof(struct rbtree_pmem_node));
            root_p->root_node_oid = old_right_node_oid;
        } else {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);
            pmemobj_tx_add_range_direct(parent_node_p, sizeof(struct rbtree_pmem_node));

            if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                parent_node_p->left = old_right_node_oid;
            } else {
                parent_node_p->right = old_right_node_oid;
            }
        }
        old_right_node_p->left = current_node_oid;
        current_node_p->parent = old_right_node_oid;
    }

    /**
     * RbtreePmemTxConcurrentLock::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     * this function must be called from a transaction block
     */
    void RbtreePmemTxConcurrentLock::rotate_right(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
        pmemobj_tx_add_range_direct(current_node_p, sizeof(struct rbtree_pmem_node));

        PMEMoid old_left_node_oid = current_node_p->left;
        struct rbtree_pmem_node *old_left_node_p = (struct rbtree_pmem_node *) pmemobj_direct(old_left_node_oid);
        pmemobj_tx_add_range_direct(old_left_node_p, sizeof(struct rbtree_pmem_node));

        current_node_p->left = old_left_node_p->right;

        if (current_node_p->left.off != 0) {
            struct rbtree_pmem_node *new_left_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->left);
            pmemobj_tx_add_range_direct(new_left_node_p, sizeof(struct rbtree_pmem_node));

            new_left_node_p->parent = current_node_oid;
        }
        old_left_node_p->parent = current_node_p->parent;

        if (current_node_p->parent.off == 0) {
            pmemobj_tx_add_range(root_p->root_node_oid, 0, sizeof(struct rbtree_pmem_node));
            root_p->root_node_oid = old_left_node_oid;
        }
        else {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);
            pmemobj_tx_add_range_direct(parent_node_p, sizeof(struct rbtree_pmem_node));

            if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                parent_node_p->left = old_left_node_oid;
            } else {
                parent_node_p->right = old_left_node_oid;
            }
        }

        old_left_node_p->right = current_node_oid;
        current_node_p->parent = old_left_node_oid;
    }

    /**
     * RbtreePmemTxConcurrentLock::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreePmemTxConcurrentLock::fix_violation(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        while ((!OID_EQUALS(current_node_oid, root_p->root_node_oid))
            && (current_node_p->color != BLACK)
            && (((struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent))->color == RED)) {
            TX_BEGIN(pop) {
                PMEMoid parent_node_oid = current_node_p->parent;
                struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);
                pmemobj_tx_add_range_direct(parent_node_p, sizeof(struct rbtree_pmem_node));

                PMEMoid grandparent_node_oid = parent_node_p->parent;
                struct rbtree_pmem_node *grandparent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(grandparent_node_oid);
                pmemobj_tx_add_range_direct(grandparent_node_p, sizeof(struct rbtree_pmem_node));

                /* case: parent is left child of grandparent node */
                if (OID_EQUALS(parent_node_oid, grandparent_node_p->left)) {
                    PMEMoid uncle_node_oid = grandparent_node_p->right;
                    struct rbtree_pmem_node *uncle_node_p = (struct rbtree_pmem_node *) pmemobj_direct(uncle_node_oid);

                    /* case: uncle is also red, only recoloring is required */
                    if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                        pmemobj_tx_add_range_direct(uncle_node_p, sizeof(struct rbtree_pmem_node));

                        //update the colors
                        grandparent_node_p->color = RED;
                        parent_node_p->color = BLACK;
                        uncle_node_p->color = BLACK;

                        current_node_oid = grandparent_node_oid;
                        current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                    } else {
                        /* case: current_node is right child of its parent -> left-rotation required */
                        if (OID_EQUALS(current_node_oid, parent_node_p->right)) {
                            rotate_left(parent_node_oid);

                            current_node_oid = parent_node_oid;
                            current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                            parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent);
                        }
                        /* case: current_node is now become the left child of its parent -> right-rotation required */
                        rotate_right(grandparent_node_oid);
                        
                        //std::swap(parent_node_p->color, grandparent_node_p->color);
                        bool parent_node_color = parent_node_p->color;
                        parent_node_p->color = grandparent_node_p->color;
                        grandparent_node_p->color = parent_node_color;
                        
                        current_node_oid = parent_node_oid;
                        current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                    }
                }
                /* case: parent is right child of grandparent node */
                else {
                    PMEMoid uncle_node_oid = grandparent_node_p->left;
                    struct rbtree_pmem_node *uncle_node_p = (struct rbtree_pmem_node *) pmemobj_direct(uncle_node_oid);

                    /* case: uncle is also red, only recoloring is required */
                    if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                        pmemobj_tx_add_range_direct(uncle_node_p, sizeof(struct rbtree_pmem_node));

                        //update the colors
                        grandparent_node_p->color = RED;
                        parent_node_p->color = BLACK;
                        uncle_node_p->color = BLACK;

                        current_node_oid = grandparent_node_oid;
                        current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                    } else {
                        /* case: current_node is left child of its parent -> right-rotation required */
                        if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                            rotate_right(parent_node_oid);

                            current_node_oid = parent_node_oid;
                            current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                            parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent);
                        }

                        /* case: current_node is right child of its parent -> left-rotation required */
                        rotate_left(grandparent_node_oid);
                        
                        //std::swap(parent_node_p->color, grandparent_node_p->color);
                        bool parent_node_color = parent_node_p->color;
                        parent_node_p->color = grandparent_node_p->color;
                        grandparent_node_p->color = parent_node_color;

                        current_node_oid = parent_node_oid;
                        current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                    }
                }
            } TX_ONABORT {
                fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                abort();
            } TX_END
        }

        TX_BEGIN(pop) {
            struct rbtree_pmem_node *root_node_p = (struct rbtree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
            pmemobj_tx_add_range_direct(root_node_p, sizeof(struct rbtree_pmem_node));
            root_node_p->color = BLACK;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
    }

    /**
     * RbtreePmemTxConcurrentLock::insert -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmemTxConcurrentLock::insert(const uint64_t key, void *value) {
        check();
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);

        if (pmemobj_rwlock_wrlock(pop, &root_p->rwlock) != 0) return 0;
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        //root node is null, insert to root node
        if (root_p->root_node_oid.off == 0) {
            TX_BEGIN(pop) {
                pmemobj_tx_add_range_direct(root_p, sizeof(struct rbtree_pmem_concurrent_lock_root));
                root_p->root_node_oid = create_new_node(key, value);
            } TX_ONABORT {
                if(root_p->root_node_oid.off != 0) {
                    pmemobj_free(&root_p->root_node_oid);
                }
                fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                abort();
            } TX_END

            //fix violation will update the color and persist it
            fix_violation(root_p->root_node_oid);
            pmemobj_rwlock_unlock(pop, &root_p->rwlock);
            return 1;
        }

        // Do a normal BST insert
        PMEMoid new_node_oid = bst_upsert(root_p->root_node_oid, key, value);
        if (!OID_IS_NULL(new_node_oid)) fix_violation(new_node_oid);
        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * RbtreePmemTxConcurrentLock::free_node -- (internal) recursively free the tree node's memory
     * this function must be called from a transaction block
     */
    void RbtreePmemTxConcurrentLock::free_node(PMEMoid current_node_oid) {
        if (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            free_node(current_node_p->left);
            free_node(current_node_p->right);

            //todo: putting tx on each node takes too much time, removing this will not actually freeing the memory
            TX_BEGIN(pop) {
                pmemobj_tx_add_range_direct(current_node_p, sizeof(struct rbtree_pmem_node));
                pmemobj_tx_free(current_node_oid);
            } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
            } TX_END
        }
    }

    /**
     * RbtreePmemTxConcurrentLock::destroy -- destructor
     */
    void RbtreePmemTxConcurrentLock::destroy() {
        check();

        free_node(root_p->root_node_oid);
        TX_BEGIN(pop) {
            pmemobj_tx_add_range_direct(root_p, sizeof(struct rbtree_pmem_concurrent_lock_root));
            root_p->root_node_oid = OID_NULL;
            pmemobj_tx_free(root_oid);
            root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        pmemobj_close(pop);
    }
}   //ycsbc