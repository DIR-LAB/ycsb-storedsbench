//
// Created by aislam6 on 10/27/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <libpmemobj.h>

#include "../../lib/ex_common.h"
#include "btree_common.h"

namespace ycsbc {
    class BTreePmemTxConcurrentLock : public StoredsBase {
    public:
        BTreePmemTxConcurrentLock(const char *path) {
            init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct btree_pmem_root_concurrent_lock *root_p = NULL;

        int check();

        int is_node_full(int nk);

        PMEMoid create_node(int _is_leaf);

        char *search(PMEMoid current_node_oid, uint64_t key);

        void split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid);

        void insert_not_full(PMEMoid node_oid, uint64_t key, void *value);

        bool update_if_found(PMEMoid current_node_oid, uint64_t key, void *value);

        void recursive_free(PMEMoid current_node_oid);
    };

    /*
     * btree_pmem_tx_check -- (internal) checks if btree has been initialized
     */
    int BTreePmemTxConcurrentLock::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * btree_pmem_tx_is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
     */
    int BTreePmemTxConcurrentLock::is_node_full(int nk) {
        return nk == BTREE_MAX_KEYS ? 1 : 0;
    }

    /*
     * btree_pmem_tx_create_node -- (internal) create new btree node
     * this function must be called from a transaction block
     */
    PMEMoid BTreePmemTxConcurrentLock::create_node(int _is_leaf) {
        PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct btree_pmem_node), BTREE_NODE_TYPE);
        struct btree_pmem_node *new_node_prt = (struct btree_pmem_node *) pmemobj_direct(new_node_oid);

        //struct btree_node *new_node_p = (struct btree_node *) malloc(sizeof(struct btree_node));
        new_node_prt->is_leaf = _is_leaf;
        new_node_prt->nk = 0;

        // new_node_prt->entries = pmemobj_tx_alloc(BTREE_MAX_KEYS * sizeof(struct entry), BTREE_ENTRY_TYPE);
        // new_node_prt->children = pmemobj_tx_alloc((BTREE_MAX_CHILDREN) * sizeof(struct btree_node), BTREE_NODE_TYPE);

        return new_node_oid;
    }

    /*
     * btree_pmem_tx_init -- initialize btree
     */
    int BTreePmemTxConcurrentLock::init(const char *path) {
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, BTREE_LAYOUT_NAME, PMEM_BTREE_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        } else {
            if ((pop = pmemobj_open(path, BTREE_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct btree_pmem_root_concurrent_lock));
        root_p = (struct btree_pmem_root_concurrent_lock *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
            exit(0);
        }

        TX_BEGIN(pop) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct btree_pmem_root_concurrent_lock));
            root_p->root_node_oid = create_node(LEAF_NODE_TRUE_FLAG);    //root is also a leaf
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        check();
        return 1;
    }

    int BTreePmemTxConcurrentLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * btree_pmem_tx_search -- (internal) search the node contains the key and return the value
     */
    char *BTreePmemTxConcurrentLock::search(PMEMoid current_node_oid, uint64_t key) {
        int i = 0;
        struct btree_pmem_node *current_node_ptr = (struct btree_pmem_node *) pmemobj_direct(current_node_oid);

        //todo: it is possible to apply binary search here to make the search faster
        while(i < current_node_ptr->nk && key > current_node_ptr->entries[i].key) {
            i += 1;
        }

        //check if we found the key
        if(i < current_node_ptr->nk && key == current_node_ptr->entries[i].key) {
            //key found, return the value
            return current_node_ptr->entries[i].value;
        }

        if(current_node_ptr->is_leaf) return NULL;     //we reached to leaf, key not found

        //the node is not leaf, move to the proper child node
        return search(current_node_ptr->children[i], key);
    }

    /**
     * btree_pmem_tx_read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BTreePmemTxConcurrentLock::read(const uint64_t key, void *&result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();

        if (pmemobj_rwlock_rdlock(pop, &root_p->rwlock) != 0) return 0;
        result = search(root_p->root_node_oid, key);
        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * btree_pmem_tx_update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BTreePmemTxConcurrentLock::update(const uint64_t key, void *value) {
        check();
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        return insert(key, value);
    }

    /**
     * btree_pmem_tx_split_node -- (internal) split the children of the child node equally with the new sibling node
     *
     * so, after this split, both the child and sibling node will hold BTREE_MIN_DEGREE children,
     * one children will be pushed to the parent node.
     *
     * this function will be called when the child node is full and become idx'th child of the parent,
     * the new sibling node will be the (idx+1)'th child of the parent.
     *
     * this function must be called from a transaction block
     * snapshot of parent_oid must be taken in caller funciton
     * this function will take snapshots of child_oid and sibling_oid
     */
    void BTreePmemTxConcurrentLock::split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid) {
        struct btree_pmem_node *parent_ptr = (struct btree_pmem_node *) pmemobj_direct(parent_oid);
        struct btree_pmem_node *child_ptr = (struct btree_pmem_node *) pmemobj_direct(child_oid);

        PMEMoid sibling_oid = create_node(child_ptr->is_leaf);    //new sibling node will get the same status as child
        struct btree_pmem_node *sibling_ptr = (struct btree_pmem_node *) pmemobj_direct(sibling_oid);

        pmemobj_tx_add_range_direct(child_ptr, sizeof(struct btree_pmem_node));
        pmemobj_tx_add_range_direct(sibling_ptr, sizeof(struct btree_pmem_node));

        sibling_ptr->nk = BTREE_MIN_DEGREE - 1;   //new sibling child will hold the (BTREE_MIN_DEGREE - 1) entries of child node

        //transfer the last (BTREE_MIN_DEGREE - 1) entries of child node to it's sibling node
        for(int i=0; i<BTREE_MIN_DEGREE-1; i+=1) {
            sibling_ptr->entries[i] = child_ptr->entries[i + BTREE_MIN_DEGREE];
        }

        //if child is an internal node, transfer the last (BTREE_MIN_DEGREE) chiddren of child node to it's sibling node
        if(child_ptr->is_leaf == LEAF_NODE_FALSE_FLAG) {
            for(int i=0; i<BTREE_MIN_DEGREE; i+=1) {
                sibling_ptr->children[i] = child_ptr->children[i + BTREE_MIN_DEGREE];
            }
        }

        child_ptr->nk = BTREE_MIN_DEGREE - 1;

        //as parent node is going to get a new child at (idx+1)-th place, make a room for it
        for(int i = parent_ptr->nk; i >= idx+1; i -= 1) {
            parent_ptr->children[i+1] = parent_ptr->children[i];
        }

        //place sibling node as parent's children
        parent_ptr->children[idx+1] = sibling_oid;

        //a entry of child node will move to the parent node, make a room for it
        for(int i = parent_ptr->nk-1; i >= idx; i -= 1) {
            parent_ptr->entries[i+1] = parent_ptr->entries[i];
        }

        //place the middle entry of child node to parent node
        parent_ptr->entries[idx] = child_ptr->entries[BTREE_MIN_DEGREE - 1];

        //parent now hold a new entry, so increasing the number of keys
        parent_ptr->nk += 1;
    }

    /**
     * btree_pmem_tx_insert_not_full -- (internal) inserts <key, value> pair into node
     * when this function called, we can assume that the node has space to hold new data
     *
     * this function must be called from a transaction block
     */
    void BTreePmemTxConcurrentLock::insert_not_full(PMEMoid node_oid, uint64_t key, void *value) {
        struct btree_pmem_node *node_ptr = (struct btree_pmem_node *) pmemobj_direct(node_oid);
        int i = node_ptr->nk - 1;

        // if node is a leaf, insert the data to actual position and return
        if(node_ptr->is_leaf) {
            while(i>=0 && node_ptr->entries[i].key > key) {
                node_ptr->entries[i+1] = node_ptr->entries[i];
                i -= 1;
            }

            node_ptr->entries[i+1].key = key;
            memcpy(node_ptr->entries[i+1].value, (char *) value, strlen((char *) value) + 1);
            node_ptr->nk += 1;
            return;
        }

        // the node is not a leaf
        // find the child which is going to store the new data
        while(i>=0 && node_ptr->entries[i].key > key) {
            i -= 1;
        }

        //check if the child is full
        if(is_node_full(((struct btree_pmem_node *) pmemobj_direct(node_ptr->children[i+1]))->nk)) {
            //child is full, need to split
            split_node(i+1, node_oid, node_ptr->children[i+1]);

            //after the split, child's middle entry is pushed to parent
            //decide which children will hold the new <key,value> pair
            if(node_ptr->entries[i+1].key < key) {
                i += 1;
            }
        }
        TX_BEGIN(pop) {
            pmemobj_tx_add_range(node_ptr->children[i+1], 0, sizeof(struct btree_pmem_node));
            insert_not_full(node_ptr->children[i+1], key, value);
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
    }

    /**
     * btree_pmem_tx_update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BTreePmemTxConcurrentLock::update_if_found(PMEMoid current_node_oid, uint64_t key, void *value) {
        int i = 0;
        struct btree_pmem_node *current_node_ptr = (struct btree_pmem_node *) pmemobj_direct(current_node_oid);

        //todo: it is possible to apply binary search here to make the search faster
        while(i < current_node_ptr->nk && key > current_node_ptr->entries[i].key) {
            i += 1;
        }

        //check if we found the key
        if(i < current_node_ptr->nk && key == current_node_ptr->entries[i].key) {
            //key found, update value and return
            TX_BEGIN(pop) {
                pmemobj_tx_add_range_direct(&current_node_ptr->entries[i].value, strlen(current_node_ptr->entries[i].value));
                memcpy(current_node_ptr->entries[i].value, (char *) value, strlen((char *) value) + 1);
            } TX_ONABORT {
                fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                abort();
            } TX_END
            return true;
        }

        if(current_node_ptr->is_leaf) return false;     //basecase: we reached to leaf, key not found

        //the node is not leaf, move to the proper child node
        return update_if_found(current_node_ptr->children[i], key, value);
    }

    /**
     * btree_pmem_tx_insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
     */
    int BTreePmemTxConcurrentLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);

        if (pmemobj_rwlock_wrlock(pop, &root_p->rwlock) != 0) return 0;

        // if the key already exist in btree, update the value and return
        bool is_updated = update_if_found(root_p->root_node_oid, key, value);
        if(is_updated) {
            //we found the key, and value has been updated
            pmemobj_rwlock_unlock(pop, &root_p->rwlock);
            return 1;
        }

        struct btree_pmem_node *root_node_ptr = (struct btree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        // if root is full
        if(is_node_full(root_node_ptr->nk)) {
            int idx = 0;

            TX_BEGIN(pop) {
                PMEMoid new_root_oid = create_node(LEAF_NODE_FALSE_FLAG);    //root is not a leaf anymore
                struct btree_pmem_node *new_root_ptr = (struct btree_pmem_node *) pmemobj_direct(new_root_oid);

                pmemobj_tx_add_range_direct(new_root_ptr, sizeof(struct btree_pmem_node));
                new_root_ptr->children[idx] = root_p->root_node_oid;
                split_node(idx, new_root_oid, root_p->root_node_oid);

                //new_root is holding two children now, decide which children will hold the new <key,value> pair
                if(new_root_ptr->entries[idx].key < key) {
                    idx += 1;
                }

                //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
                pmemobj_tx_add_range(new_root_ptr->children[idx], 0, sizeof(struct btree_pmem_node));
                insert_not_full(new_root_ptr->children[idx], key, value);

                //update the root
                pmemobj_tx_add_range_direct(root_p, sizeof(struct btree_pmem_node));
                root_p->root_node_oid = new_root_oid;

                //as root_p->root_node_oid updated, reassign root_p
                root_node_ptr = (struct btree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
            } TX_ONABORT {
                fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                abort();
            } TX_END
        }
        else {
            //root is not full, go ahead and insert the data to proper place
            TX_BEGIN(pop) {
                pmemobj_tx_add_range_direct(&root_p->root_node_oid, sizeof(struct btree_pmem_node));
                insert_not_full(root_p->root_node_oid, key, value);
            } TX_ONABORT {
                fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                abort();
            } TX_END
        }

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * btree_pmem_tx_recursive_free -- recursively visit all the btree nodes and de-allocate memory
     * this function must be called in a transaction block
     */
    void BTreePmemTxConcurrentLock::recursive_free(PMEMoid current_node_oid) {
        struct btree_pmem_node *current_node_ptr = (struct btree_pmem_node *) pmemobj_direct(current_node_oid);
        //base case
        if(current_node_ptr->is_leaf) {
            //free the current_node's memory and return
            pmemobj_tx_add_range_direct(current_node_ptr, sizeof(struct btree_pmem_node));
            pmemobj_tx_free(current_node_oid);
            return;
        }

        //recursively call all the child nodes
        for(int i=0; i<current_node_ptr->nk+1; i+=1) {
            if(current_node_ptr->children[i].off != 0) {
                recursive_free(current_node_ptr->children[i]);
            }
        }

        //free the current_node's memory
        pmemobj_tx_add_range_direct(current_node_ptr, sizeof(struct btree_pmem_node));
        pmemobj_tx_free(current_node_oid);
    }

    /**
     * btree_pmem_tx_free -- destructor
     */
    void BTreePmemTxConcurrentLock::destroy() {
        check();

        TX_BEGIN(pop) {
            recursive_free(root_p->root_node_oid);
            root_p->root_node_oid = OID_NULL;

            pmemobj_tx_add_range_direct(root_p, sizeof(struct btree_pmem_root_concurrent_lock));
            pmemobj_tx_free(root_oid);
            root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        pmemobj_close(pop);
    }
}   //ycsbc