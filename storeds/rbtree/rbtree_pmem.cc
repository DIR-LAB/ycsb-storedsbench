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
    class RbtreePmem : public StoredsBase {
    public:
        RbtreePmem(const char *path) {
            RbtreePmem::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~RbtreePmem();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct rbtree_pmem_root *root_p = NULL;

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
     * RbtreePmem::check -- checks if the root object has been initialized
     */
    int RbtreePmem::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: rbtree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreePmem::init -- rbtree initializer
     */
    int RbtreePmem::init(const char *path) {
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

        root_oid = pmemobj_root(pop, sizeof(struct rbtree_pmem_root));
        root_p = (struct rbtree_pmem_root *) pmemobj_direct(root_oid);

        if (root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }

        return 1;
    }

    int RbtreePmem::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * RbtreePmem::lookup -- (internal) recursively search the value of a key
     */
    void RbtreePmem::lookup(PMEMoid current_node, uint64_t key, void *&result) {
        //base case
        if (current_node.off == 0) {
            return;
        }

        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node);
        if (current_node_p->key == key) {
            //data found
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
     * RbtreePmem::read -- read 'value' of 'key' and place it into '&result'
     */
    int RbtreePmem::read(const uint64_t key, void *&result) {
        check();

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p->root_node_oid, key, result);
        return 1;
    }

    /**
     * RbtreePmem::update -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmem::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * RbtreePmem::create_new_node -- (internal) allocate memory for new node
     */
    PMEMoid RbtreePmem::create_new_node(uint64_t key, void *value) {
        //prepare new in-memory node
        struct rbtree_pmem_node *in_memory_node_p = (struct rbtree_pmem_node *) malloc(sizeof(struct rbtree_pmem_node));
        in_memory_node_p->left = in_memory_node_p->right = in_memory_node_p->parent = OID_NULL;
        in_memory_node_p->color = RED;
        in_memory_node_p->key = key;
        strcpy(in_memory_node_p->value, (char *) value);

        //copy in-memory node to pmem-node
        PMEMoid new_node_oid;
        pmemobj_alloc(pop, &new_node_oid, sizeof(struct rbtree_pmem_node), RB_NODE_TYPE, NULL, NULL);
        struct rbtree_pmem_node *pmem_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);
        memcpy(pmem_node_p, in_memory_node_p, sizeof(struct rbtree_pmem_node));

        //freeing in-memory node
        free(in_memory_node_p);

        return new_node_oid;
    }

    /**
     * RbtreePmem::bst_upsert -- (internal) update the value if key already exist and return a NULL OID as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node OID
     * will update the balance in later scope
     */
    PMEMoid RbtreePmem::bst_upsert(PMEMoid current_node_oid, uint64_t key, void *value) {
        while (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            if (key == current_node_p->key) {
                //key already exist, update the value and return
                pmemobj_memcpy_persist(pop, current_node_p->value, (char *) value, strlen((char *) value) + 1);

                //return status: updated!
                return OID_NULL;
            } else if (key < current_node_p->key) {
                if (current_node_p->left.off == 0) {
                    PMEMoid new_node_oid = create_new_node(key, value);
                    struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);

                    current_node_p->left = new_node_oid;
                    new_node_p->parent = current_node_oid;
                    pmemobj_persist(pop, &current_node_p->left, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &new_node_oid, sizeof(struct rbtree_pmem_node));

                    //return status: inserted!
                    return new_node_oid;
                } else {
                    current_node_oid = current_node_p->left;
                }
            } else {
                if (current_node_p->right.off == 0) {
                    PMEMoid new_node_oid = create_new_node(key, value);
                    struct rbtree_pmem_node *new_node_p = (struct rbtree_pmem_node *) pmemobj_direct(new_node_oid);

                    current_node_p->right = new_node_oid;
                    new_node_p->parent = current_node_oid;
                    pmemobj_persist(pop, &current_node_p->right, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &new_node_oid, sizeof(struct rbtree_pmem_node));

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
     * RbtreePmem::rotate_left -- (internal) Rebalance RB-Tree.
     */
    void RbtreePmem::rotate_left(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        PMEMoid old_right_node_oid = current_node_p->right;
        struct rbtree_pmem_node *old_right_node_p = (struct rbtree_pmem_node *) pmemobj_direct(old_right_node_oid);
        current_node_p->right = old_right_node_p->left;
        //pmemobj_persist(pop, &current_node_p->right, sizeof(struct rbtree_pmem_node));

        if (current_node_p->right.off != 0) {
            struct rbtree_pmem_node *new_right_node_p = (struct rbtree_pmem_node *) pmemobj_direct(
                    current_node_p->right);
            new_right_node_p->parent = current_node_oid;
            pmemobj_persist(pop, &new_right_node_p->parent, sizeof(struct rbtree_pmem_node));
        }

        old_right_node_p->parent = current_node_p->parent;
        //pmemobj_persist(pop, &old_right_node_p->parent, sizeof(struct rbtree_pmem_node));

        if (current_node_p->parent.off == 0) {
            root_p->root_node_oid = old_right_node_oid;
            pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct rbtree_pmem_node));
        } else {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);

            if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                parent_node_p->left = old_right_node_oid;
                pmemobj_persist(pop, &parent_node_p->left, sizeof(struct rbtree_pmem_node));
            } else {
                parent_node_p->right = old_right_node_oid;
                pmemobj_persist(pop, &parent_node_p->right, sizeof(struct rbtree_pmem_node));
            }
        }
        old_right_node_p->left = current_node_oid;
        //pmemobj_persist(pop, &old_right_node_p->left, sizeof(struct rbtree_pmem_node));
        current_node_p->parent = old_right_node_oid;
        //pmemobj_persist(pop, &current_node_p->parent, sizeof(struct rbtree_pmem_node));

        pmemobj_persist(pop, &old_right_node_oid, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &current_node_oid, sizeof(struct rbtree_pmem_node));
    }

    /**
     * RbtreePmem::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreePmem::rotate_right(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        PMEMoid old_left_node_oid = current_node_p->left;
        struct rbtree_pmem_node *old_left_node_p = (struct rbtree_pmem_node *) pmemobj_direct(old_left_node_oid);
        current_node_p->left = old_left_node_p->right;
        //pmemobj_persist(pop, &current_node_p->left, sizeof(struct rbtree_pmem_node));

        if (current_node_p->left.off != 0) {
            struct rbtree_pmem_node *new_left_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_p->left);
            new_left_node_p->parent = current_node_oid;
            pmemobj_persist(pop, &new_left_node_p->parent, sizeof(struct rbtree_pmem_node));
        }
        old_left_node_p->parent = current_node_p->parent;
        //pmemobj_persist(pop, &old_left_node_p->parent, sizeof(struct rbtree_pmem_node));

        if (current_node_p->parent.off == 0) {
            root_p->root_node_oid = old_left_node_oid;
            pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct rbtree_pmem_node));
        }
        else {
            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);

            if (OID_EQUALS(current_node_oid, parent_node_p->left)) {
                parent_node_p->left = old_left_node_oid;
                pmemobj_persist(pop, &parent_node_p->left, sizeof(struct rbtree_pmem_node));
            } else {
                parent_node_p->right = old_left_node_oid;
                pmemobj_persist(pop, &parent_node_p->right, sizeof(struct rbtree_pmem_node));
            }
        }

        old_left_node_p->right = current_node_oid;
        //pmemobj_persist(pop, &old_left_node_p->right, sizeof(struct rbtree_pmem_node));
        current_node_p->parent = old_left_node_oid;
        //pmemobj_persist(pop, &current_node_p->parent, sizeof(struct rbtree_pmem_node));

        pmemobj_persist(pop, &old_left_node_oid, sizeof(struct rbtree_pmem_node));
        pmemobj_persist(pop, &current_node_oid, sizeof(struct rbtree_pmem_node));
    }

    /**
     * RbtreePmem::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreePmem::fix_violation(PMEMoid &current_node_oid) {
        struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);

        while ((!OID_EQUALS(current_node_oid, root_p->root_node_oid))
               && (current_node_p->color != BLACK)
               && (((struct rbtree_pmem_node *) pmemobj_direct(current_node_p->parent))->color == RED)) {

            //printf("current node pointer: %p\n", current_node_p);

            PMEMoid parent_node_oid = current_node_p->parent;
            struct rbtree_pmem_node *parent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(parent_node_oid);

            PMEMoid grandparent_node_oid = parent_node_p->parent;
            struct rbtree_pmem_node *grandparent_node_p = (struct rbtree_pmem_node *) pmemobj_direct(
                    grandparent_node_oid);

            /* case: parent is left child of grandparent node */
            if (OID_EQUALS(parent_node_oid, grandparent_node_p->left)) {
                PMEMoid uncle_node_oid = grandparent_node_p->right;
                struct rbtree_pmem_node *uncle_node_p = (struct rbtree_pmem_node *) pmemobj_direct(uncle_node_oid);

                /* case: uncle is also red, only recoloring is required */
                if (uncle_node_oid.off != 0 && uncle_node_p->color == RED) {
                    //update the colors
                    grandparent_node_p->color = RED;
                    parent_node_p->color = BLACK;
                    uncle_node_p->color = BLACK;

                    pmemobj_persist(pop, grandparent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, parent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, uncle_node_p, sizeof(struct rbtree_pmem_node));

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

                    pmemobj_persist(pop, &parent_node_oid, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &grandparent_node_oid, sizeof(struct rbtree_pmem_node));
                    
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
                    grandparent_node_p->color = RED;
                    parent_node_p->color = BLACK;
                    uncle_node_p->color = BLACK;

                    pmemobj_persist(pop, grandparent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, parent_node_p, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, uncle_node_p, sizeof(struct rbtree_pmem_node));

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

                    pmemobj_persist(pop, &parent_node_oid, sizeof(struct rbtree_pmem_node));
                    pmemobj_persist(pop, &grandparent_node_oid, sizeof(struct rbtree_pmem_node));

                    current_node_oid = parent_node_oid;
                    current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
                }
            }
        }

        struct rbtree_pmem_node *root_node_p = (struct rbtree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        root_node_p->color = BLACK;
        pmemobj_persist(pop, root_node_p, sizeof(struct rbtree_pmem_node));
    }

    /**
     * RbtreePmem::insert -- update the value if key already exist
     * if the key not exist, insert a new node and balance the tree
     */
    int RbtreePmem::insert(const uint64_t key, void *value) {
        check();

        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        //root node is null, insert to root node
        if (root_p->root_node_oid.off == 0) {
            root_p->root_node_oid = create_new_node(key, value);

            //fix violation will update the color and persist it
            fix_violation(root_p->root_node_oid);
            return 1;
        }

        // Do a normal BST insert
        PMEMoid new_node_oid = bst_upsert(root_p->root_node_oid, key, value);
        if (!OID_IS_NULL(new_node_oid)) fix_violation(new_node_oid);
        return 1;
    }

    /**
     * RbtreePmem::free_node -- (internal) recursively free the tree node's memory
     */
    void RbtreePmem::free_node(PMEMoid current_node_oid) {
        if (current_node_oid.off != 0) {
            struct rbtree_pmem_node *current_node_p = (struct rbtree_pmem_node *) pmemobj_direct(current_node_oid);
            free_node(current_node_p->left);
            free_node(current_node_p->right);

            pmemobj_free(&current_node_oid);
        }
    }

    /**
     * RbtreePmem::destroy -- destructor
     */
    void RbtreePmem::destroy() {
        check();
        free_node(root_p->root_node_oid);
        root_p->root_node_oid = OID_NULL;
        pmemobj_free(&root_oid);
        root_oid = OID_NULL;
        pmemobj_close(pop);
    }
}   //ycsbc