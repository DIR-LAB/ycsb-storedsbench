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
#include "bplustree_common.h"

namespace ycsbc {
    class BPlusTreePmemConcurrentLock : public StoredsBase {
    public:
        BPlusTreePmemConcurrentLock(const char *path) {
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
        struct bplustree_pmem_root_concurrent_lock *root_p = NULL;

        int check();

        int is_node_full(int nk);

        PMEMoid create_node(int _is_leaf);

        char *search(PMEMoid current_node_oid, uint64_t key);

        void split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid);

        void insert_not_full(PMEMoid node_oid, uint64_t key, void *value);

        bool update_if_found(PMEMoid current_node_oid, uint64_t key, void *value);

        void recursive_free(PMEMoid current_node_oid);

        void print_leaf_chain();
    };

    /**
     * BPlusTreePmemConcurrentLock::check -- (internal) checks if btree has been initialized
     */
    int BPlusTreePmemConcurrentLock::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * BPlusTreePmemConcurrentLock::print_leaf_chain -- (internal) print the leaf chain to check the integrity of bplus-tree
     */
    void BPlusTreePmemConcurrentLock::print_leaf_chain() {
        struct bplustree_pmem_node *current_node = (struct bplustree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        while (current_node != NULL && !current_node->is_leaf) {
            current_node = (struct bplustree_pmem_node *) pmemobj_direct(current_node->children[0]);
        }

        while (current_node != NULL) {
            printf("[");
            for (int i = 0; i < current_node->nk; i += 1) {
                printf(" <%ld, %s>", current_node->entries[i].key, current_node->entries[i].value);
            }
            printf("] -> ");
            current_node = (struct bplustree_pmem_node *) pmemobj_direct(current_node->next);
        }
        printf("\n");
    }

    /**
     * BPlusTreePmemConcurrentLock::is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
     */
    int BPlusTreePmemConcurrentLock::is_node_full(int nk) {
        return nk == BPLUSTREE_MAX_KEYS ? 1 : 0;
    }

    /**
     * BPlusTreePmemConcurrentLock::create_node -- (internal) create new btree node
     */
    PMEMoid BPlusTreePmemConcurrentLock::create_node(int _is_leaf) {
        PMEMoid new_node_oid;
        pmemobj_alloc(pop, &new_node_oid, sizeof(struct bplustree_pmem_node), BTREE_NODE_TYPE, NULL, NULL);
        struct bplustree_pmem_node *new_node_prt = (struct bplustree_pmem_node *) pmemobj_direct(new_node_oid);

        new_node_prt->is_leaf = _is_leaf;
        new_node_prt->nk = 0;
        new_node_prt->next = new_node_prt->previous = OID_NULL;

        return new_node_oid;
    }

    /**
     * BPlusTreePmemConcurrentLock::init -- initialize btree
     */
    int BPlusTreePmemConcurrentLock::init(const char *path) {
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, BPLUSTREE_LAYOUT_NAME, PMEM_BPLUSTREE_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        } else {
            if ((pop = pmemobj_open(path, BPLUSTREE_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct bplustree_pmem_root_concurrent_lock));
        root_p = (struct bplustree_pmem_root_concurrent_lock *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
            exit(0);
        }

        root_p->root_node_oid = create_node(LEAF_NODE_TRUE_FLAG);    //root is also a leaf
        pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct bplustree_pmem_node));
        check();
        return 1;
    }

    /**
     * BPlusTreePmemConcurrentLock::scan -- perform range query
     */
    int BPlusTreePmemConcurrentLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();

        if (pmemobj_rwlock_rdlock(pop, &root_p->rwlock) != 0) return 0;
        struct bplustree_pmem_node *current_node = (struct bplustree_pmem_node *) pmemobj_direct(root_p->root_node_oid);

        //going down upto the leaf
        while(current_node != NULL && !current_node->is_leaf) {
            int i = 0;

            while (i < current_node->nk && key > current_node->entries[i].key) {
                i += 1;
            }
            current_node = (struct bplustree_pmem_node *) pmemobj_direct(current_node->children[i]);
        }

        //reached the leaf
        if(current_node != NULL) {
            int i = 0;
            //find the starting point to start the scan
            while (i < current_node->nk && key > current_node->entries[i].key) {
                i += 1;
            }

            int len_count = 0;
            //forward scan through the leaf chain
            while(current_node != NULL) {
                while (i < current_node->nk) {
                    std::vector <DB::Kuint64VstrPair> tmp;
                    tmp.push_back(std::make_pair(current_node->entries[i].key, current_node->entries[i].value));
                    result.push_back(tmp);

                    len_count += 1;
                    if(len_count == len) break;
                    i += 1;
                }
                if(len_count == len) break;
                current_node = (struct bplustree_pmem_node *) pmemobj_direct(current_node->next);
                i = 0;
            }
        }

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * BPlusTreePmemConcurrentLock::search -- (internal) search the node contains the key and return the value
     */
    char *BPlusTreePmemConcurrentLock::search(PMEMoid current_node_oid, uint64_t key) {
        int i = 0;
        struct bplustree_pmem_node *current_node_ptr = (struct bplustree_pmem_node *) pmemobj_direct(current_node_oid);

        //todo: it is possible to apply binary search here to make the search faster
        while(i < current_node_ptr->nk && key > current_node_ptr->entries[i].key) {
            i += 1;
        }

        //we reached to leaf
        if (current_node_ptr->is_leaf) {
            //check if we found the key
            if (i < current_node_ptr->nk && key == current_node_ptr->entries[i].key) {
                //key found, return the value
                return current_node_ptr->entries[i].value;
            }
            //key not found
            return NULL;
        }

        //the node is not leaf, move to the proper child node
        if (i < current_node_ptr->nk && key == current_node_ptr->entries[i].key) i += 1;
        return search(current_node_ptr->children[i], key);
    }

    /**
     * BPlusTreePmemConcurrentLock::read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BPlusTreePmemConcurrentLock::read(const uint64_t key, void *&result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();
        if (pmemobj_rwlock_rdlock(pop, &root_p->rwlock) != 0) return 0;
        result = search(root_p->root_node_oid, key);
        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * BPlusTreePmemConcurrentLock::update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BPlusTreePmemConcurrentLock::update(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        check();
        return insert(key, value);
    }

    /**
     * BPlusTreePmemConcurrentLock::update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BPlusTreePmemConcurrentLock::update_if_found(PMEMoid current_node_oid, uint64_t key, void *value) {
        int i = 0;
        struct bplustree_pmem_node *current_node_ptr = (struct bplustree_pmem_node *) pmemobj_direct(current_node_oid);

        //todo: it is possible to apply binary search here to make the search faster
        while(i < current_node_ptr->nk && key > current_node_ptr->entries[i].key) {
            i += 1;
        }

        //we reached to leaf
        if (current_node_ptr->is_leaf) {
            //check if we found the key
            if (i < current_node_ptr->nk && key == current_node_ptr->entries[i].key) {
                //key found, update value and return
                pmemobj_memcpy_persist(pop, current_node_ptr->entries[i].value, (char *) value, strlen((char *) value) + 1);
                return true;
            }
            //key not found
            return false;
        }

        //the node is not leaf, move to the proper child node
        if (i < current_node_ptr->nk && key == current_node_ptr->entries[i].key) i += 1;
        return update_if_found(current_node_ptr->children[i], key, value);
    }

    /**
     * BPlusTreePmemConcurrentLock::split_node -- (internal) split the children of the child node equally with the new sibling node
     *
     * so, after this split, both the child and sibling node will hold BPLUSTREE_MIN_DEGREE children,
     * one children will be pushed to the parent node.
     *
     * this function will be called when the child node is full and become idx'th child of the parent,
     * the new sibling node will be the (idx+1)'th child of the parent.
     */
    void BPlusTreePmemConcurrentLock::split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid) {
        struct bplustree_pmem_node *parent_ptr = (struct bplustree_pmem_node *) pmemobj_direct(parent_oid);
        struct bplustree_pmem_node *child_ptr = (struct bplustree_pmem_node *) pmemobj_direct(child_oid);

        if (child_ptr->is_leaf) {
            //new right-sibling node will get the same status as child
            PMEMoid sibling_oid = create_node(child_ptr->is_leaf);
            struct bplustree_pmem_node *sibling_ptr = (struct bplustree_pmem_node *) pmemobj_direct(sibling_oid);
            //new right-sibling child will hold the BPLUSTREE_MIN_DEGREE entries of child node
            sibling_ptr->nk = BPLUSTREE_MIN_DEGREE;

            //transfer the last BPLUSTREE_MIN_DEGREE entries of child node to it's sibling node
            for(int i=0; i<BPLUSTREE_MIN_DEGREE; i+=1) {
                sibling_ptr->entries[i] = child_ptr->entries[i + BPLUSTREE_MIN_DEGREE - 1];
            }

            child_ptr->nk = BPLUSTREE_MIN_DEGREE - 1;

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
            parent_ptr->entries[idx].key = sibling_ptr->entries[0].key;

            //parent now hold a new entry, so increasing the number of keys
            parent_ptr->nk += 1;

            //adjust leaf chain pointers
            sibling_ptr->next = child_ptr->next;
            if (sibling_ptr->next.off != 0) {
                struct bplustree_pmem_node *sibling_next_ptr = (struct bplustree_pmem_node *) pmemobj_direct(sibling_ptr->next);
                sibling_next_ptr->previous = sibling_oid;
                pmemobj_persist(pop, sibling_next_ptr, sizeof(struct bplustree_pmem_node));
            }
            child_ptr->next = sibling_oid;
            sibling_ptr->previous = child_oid;

            //persist changes
            pmemobj_persist(pop, sibling_ptr, sizeof(struct bplustree_pmem_node));
            pmemobj_persist(pop, child_ptr, sizeof(struct bplustree_pmem_node));
            pmemobj_persist(pop, parent_ptr, sizeof(struct bplustree_pmem_node));
        }
        else {
            PMEMoid sibling_oid = create_node(child_ptr->is_leaf);    //new sibling node will get the same status as child
            struct bplustree_pmem_node *sibling_ptr = (struct bplustree_pmem_node *) pmemobj_direct(sibling_oid);

            sibling_ptr->nk = BPLUSTREE_MIN_DEGREE - 1;   //new sibling child will hold the (BPLUSTREE_MIN_DEGREE - 1) entries of child node

            //transfer the last (BPLUSTREE_MIN_DEGREE - 1) entries of child node to it's sibling node
            for(int i=0; i<BPLUSTREE_MIN_DEGREE-1; i+=1) {
                sibling_ptr->entries[i] = child_ptr->entries[i + BPLUSTREE_MIN_DEGREE];
            }

            //as child is an internal node, transfer the last BPLUSTREE_MIN_DEGREE chiddren of child node to it's sibling node
            for(int i=0; i<BPLUSTREE_MIN_DEGREE; i+=1) {
                sibling_ptr->children[i] = child_ptr->children[i + BPLUSTREE_MIN_DEGREE];
                //todo: it is possible to free few of child's memory, or can try with pmemobj_memmove
            }

            child_ptr->nk = BPLUSTREE_MIN_DEGREE - 1;

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
            parent_ptr->entries[idx] = child_ptr->entries[BPLUSTREE_MIN_DEGREE - 1];

            //parent now hold a new entry, so increasing the number of keys
            parent_ptr->nk += 1;

            //persist changes
            pmemobj_persist(pop, sibling_ptr, sizeof(struct bplustree_pmem_node));
            pmemobj_persist(pop, child_ptr, sizeof(struct bplustree_pmem_node));
            pmemobj_persist(pop, parent_ptr, sizeof(struct bplustree_pmem_node));
        }
    }

    /**
     * BPlusTreePmemConcurrentLock::insert_not_full -- (internal) inserts <key, value> pair into node
     * when this function called, we can assume that the node has space to hold new data
     */
    void BPlusTreePmemConcurrentLock::insert_not_full(PMEMoid node_oid, uint64_t key, void *value) {
        struct bplustree_pmem_node *node_ptr = (struct bplustree_pmem_node *) pmemobj_direct(node_oid);
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

            //persist the changes
            pmemobj_persist(pop, node_ptr, sizeof(struct bplustree_pmem_node));
            return;
        }

        // the node is not a leaf
        // find the child which is going to store the new data
        while(i>=0 && node_ptr->entries[i].key > key) {
            i -= 1;
        }

        //check if the child is full
        if(is_node_full(((struct bplustree_pmem_node *) pmemobj_direct(node_ptr->children[i+1]))->nk)) {
            //child is full, need to split
            split_node(i+1, node_oid, node_ptr->children[i+1]);

            //after the split, child's middle entry is pushed to parent
            //decide which children will hold the new <key,value> pair
            if(node_ptr->entries[i+1].key < key) {
                i += 1;
            }
        }

        insert_not_full(node_ptr->children[i+1], key, value);
    }

    /**
     * BPlusTreePmemConcurrentLock::insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
     */
    int BPlusTreePmemConcurrentLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        if (pmemobj_rwlock_wrlock(pop, &root_p->rwlock) != 0) return 0;

        // if the key already exist in btree, update the value and return
        bool is_updated = update_if_found(root_p->root_node_oid, key, value);
        if(is_updated) {
            //we found the key, and value has been updated
            pmemobj_rwlock_unlock(pop, &root_p->rwlock);
            return 1;
        }

        struct bplustree_pmem_node *root_node_ptr = (struct bplustree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        // if root is full
        if(is_node_full(root_node_ptr->nk)) {
            int idx = 0;

            PMEMoid new_root_oid = create_node(LEAF_NODE_FALSE_FLAG);    //root is not a leaf anymore
            struct bplustree_pmem_node *new_root_ptr = (struct bplustree_pmem_node *) pmemobj_direct(new_root_oid);

            new_root_ptr->children[idx] = root_p->root_node_oid;
            split_node(idx, new_root_oid, root_p->root_node_oid);

            //new_root is holding two children now, decide which children will hold the new <key,value> pair
            if(new_root_ptr->entries[idx].key < key) {
                idx += 1;
            }

            //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
            insert_not_full(new_root_ptr->children[idx], key, value);
            pmemobj_persist(pop, new_root_ptr, sizeof(struct bplustree_pmem_node));  //persist changes on new_root_ptr

            //update the root
            root_p->root_node_oid = new_root_oid;

            //as root_p->root_node_oid updated, reassign root_p
            root_node_ptr = (struct bplustree_pmem_node *) pmemobj_direct(root_p->root_node_oid);
            pmemobj_persist(pop, root_node_ptr, sizeof(struct bplustree_pmem_node));    //persist changes on root_p
        }
        else {
            //root is not full, go ahead and insert the data to proper place
            insert_not_full(root_p->root_node_oid, key, value);
        }

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * BPlusTreePmemConcurrentLock::recursive_free -- recursively visit all the btree nodes and de-allocate memory
     */
    void BPlusTreePmemConcurrentLock::recursive_free(PMEMoid current_node_oid) {
        struct bplustree_pmem_node *current_node_ptr = (struct bplustree_pmem_node *) pmemobj_direct(current_node_oid);
        //base case
        if(current_node_ptr->is_leaf) {
            //free the current_node's memory and return
            pmemobj_free(&current_node_oid);
            return;
        }

        //recursively call all the child nodes
        for(int i=0; i<current_node_ptr->nk+1; i+=1) {
            if(current_node_ptr->children[i].off != 0) {
                recursive_free(current_node_ptr->children[i]);
            }
        }

        //free the current_node's memory
        pmemobj_free(&current_node_oid);
    }

    /**
     * BPlusTreePmemConcurrentLock::destroy -- destructor
     */
    void BPlusTreePmemConcurrentLock::destroy() {
        check();

        recursive_free(root_p->root_node_oid);
        root_p->root_node_oid = OID_NULL;
        pmemobj_free(&root_oid);
        root_oid = OID_NULL;

        pmemobj_close(pop);
    }
}   //ycsbc