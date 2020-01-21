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
#include "bp_common.h"

namespace ycsbc {
    class BPPmem : public StoredsBase {
    public:
        BPPmem(const char *path) {
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
        struct bp_pmem_root *root_p = NULL;

        int check();

        int is_node_full(int nk);

        PMEMoid create_node(int _is_leaf);

        PMEMoid create_record(void *_value);

        char *search(PMEMoid current_node_oid, uint64_t key);

        void split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid);

        void insert_not_full(PMEMoid node_oid, uint64_t key, void *value);

        bool update_if_found(PMEMoid current_node_oid, uint64_t key, void *value);

        void recursive_free(PMEMoid current_node_oid);

        void print_leaf_chain();
    };

    /**
     * BPPmem::check -- (internal) checks if btree has been initialized
     */
    int BPPmem::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: btree not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * BPPmem::print_leaf_chain -- (internal) print the leaf chain to check the integrity of bplus-tree
     */
    void BPPmem::print_leaf_chain() {
        struct bp_pmem_node *current_node = (struct bp_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        while (current_node != NULL && !current_node->is_leaf) {
            current_node = (struct bp_pmem_node *) pmemobj_direct(current_node->pointers[0]);
        }

        while (current_node != NULL) {
            printf("[");
            for (int i = 0; i < current_node->nk; i += 1) {
                struct bp_record * record_ptr = (struct bp_record *) pmemobj_direct(current_node->pointers[i]);
                printf(" <%ld, %s>", current_node->keys[i], record_ptr->value);
            }
            printf("] -> ");
            current_node = (struct bp_pmem_node *) pmemobj_direct(current_node->next);
        }
        printf("\n");
    }

    /**
     * BPPmem::is_node_full -- (internal) checks if btree node contains max possible <key-value> pairs
     */
    int BPPmem::is_node_full(int nk) {
        return nk == BP_MAX_KEYS ? 1 : 0;
    }

    /**
     * BPPmem::create_node -- (internal) create new btree node
     */
    inline PMEMoid BPPmem::create_node(int _is_leaf) {
        PMEMoid new_node_oid;
        pmemobj_alloc(pop, &new_node_oid, sizeof(struct bp_pmem_node), BP_NODE_TYPE, NULL, NULL);
        
        if (new_node_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: memory allocation failed\n", __FUNCTION__);
            exit(0);
        }

        struct bp_pmem_node *new_node_prt = (struct bp_pmem_node *) pmemobj_direct(new_node_oid);

        new_node_prt->is_leaf = _is_leaf;
        new_node_prt->nk = 0;
        new_node_prt->next = new_node_prt->previous = OID_NULL;

        return new_node_oid;
    }

    /**
     * BPPmem::create_record -- (internal) create new record for leaf node
     */
    inline PMEMoid BPPmem::create_record(void *_value) {
        PMEMoid new_record_oid;
        pmemobj_alloc(pop, &new_record_oid, sizeof(struct bp_record), BP_ENTRY_TYPE, NULL, NULL);
        
        if (new_record_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: memory allocation failed\n", __FUNCTION__);
            exit(0);
        }

        struct bp_record *new_record_ptr = (struct bp_record *) pmemobj_direct(new_record_oid);
        memcpy(new_record_ptr->value, (char *) _value, strlen((char *) _value) + 1);
        return new_record_oid;
    }

    /**
     * BPPmem::init -- initialize btree
     */
    int BPPmem::init(const char *path) {
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, BP_LAYOUT_NAME, PMEM_BP_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        } else {
            if ((pop = pmemobj_open(path, BP_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct bp_pmem_root));
        root_p = (struct bp_pmem_root *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
            exit(0);
        }

        root_p->root_node_oid = create_node(BP_LEAF_NODE_TRUE_FLAG);    //root is also a leaf
        pmemobj_persist(pop, &root_p->root_node_oid, sizeof(struct bp_pmem_node));
        check();
        return 1;
    }

    /**
     * BPPmem::scan -- perform range query
     */
    int BPPmem::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();

        struct bp_pmem_node *current_node = (struct bp_pmem_node *) pmemobj_direct(root_p->root_node_oid);

        //going down upto the leaf
        while(current_node != NULL && !current_node->is_leaf) {
            int i = 0;

            while (i < current_node->nk && key > current_node->keys[i]) {
                i += 1;
            }
            current_node = (struct bp_pmem_node *) pmemobj_direct(current_node->pointers[i]);
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
                    struct bp_record * record_ptr = (struct bp_record *) pmemobj_direct(current_node->pointers[i]);
                    tmp.push_back(std::make_pair(current_node->keys[i], record_ptr->value));
                    result.push_back(tmp);

                    len_count += 1;
                    if(len_count == len) break;
                    i += 1;
                }
                if(len_count == len) break;
                current_node = (struct bp_pmem_node *) pmemobj_direct(current_node->next);
                i = 0;
            }
        }

        return 1;
    }

    /**
     * BPPmem::search -- (internal) search the node contains the key and return the value
     */
    char *BPPmem::search(PMEMoid current_node_oid, uint64_t key) {
        int i = 0;
        struct bp_pmem_node *current_node_ptr = (struct bp_pmem_node *) pmemobj_direct(current_node_oid);

        //todo: it is possible to apply binary search here to make the search faster
        while(i < current_node_ptr->nk && key > current_node_ptr->keys[i]) {
            i += 1;
        }

        //we reached to leaf
        if (current_node_ptr->is_leaf) {
            //check if we found the key
            if (i < current_node_ptr->nk && key == current_node_ptr->keys[i]) {
                //key found, return the value
                struct bp_record * record_ptr = (struct bp_record *) pmemobj_direct(current_node_ptr->pointers[i]);
                return record_ptr->value;
            }
            //key not found
            return NULL;
        }

        //the node is not leaf, move to the proper child node
        if (i < current_node_ptr->nk && key == current_node_ptr->keys[i]) i += 1;
        return search(current_node_ptr->pointers[i], key);
    }

    /**
     * BPPmem::read -- read 'value' of 'key' from btree and place it into '&result'
     */
    int BPPmem::read(const uint64_t key, void *&result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();
        result = search(root_p->root_node_oid, key);
        return 1;
    }

    /**
     * BPPmem::update -- update 'value' of 'key' into btree, will insert the 'value' if 'key' not exists
     */
    int BPPmem::update(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        check();
        return insert(key, value);
    }

    /**
     * BPPmem::update_if_found -- (internal) search the key and if exist, update the value
     */
    bool BPPmem::update_if_found(PMEMoid current_node_oid, uint64_t key, void *value) {
        int i = 0;
        struct bp_pmem_node *current_node_ptr = (struct bp_pmem_node *) pmemobj_direct(current_node_oid);

        //todo: it is possible to apply binary search here to make the search faster
        while(i < current_node_ptr->nk && key > current_node_ptr->keys[i]) {
            i += 1;
        }

        //we reached to leaf
        if (current_node_ptr->is_leaf) {
            //check if we found the key
            if (i < current_node_ptr->nk && key == current_node_ptr->keys[i]) {
                //key found, update value and return
                struct bp_record * record_ptr = (struct bp_record *) pmemobj_direct(current_node_ptr->pointers[i]);
                pmemobj_memcpy_persist(pop, record_ptr->value, (char *) value, strlen((char *) value) + 1);
                return true;
            }
            //key not found
            return false;
        }

        //the node is not leaf, move to the proper child node
        if (i < current_node_ptr->nk && key == current_node_ptr->keys[i]) i += 1;
        return update_if_found(current_node_ptr->pointers[i], key, value);
    }

    /**
     * BPPmem::split_node -- (internal) split the children of the child node equally with the new sibling node
     *
     * so, after this split, both the child and sibling node will hold BP_MIN_DEGREE children,
     * one children will be pushed to the parent node.
     *
     * this function will be called when the child node is full and become idx'th child of the parent,
     * the new sibling node will be the (idx+1)'th child of the parent.
     */
    void BPPmem::split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid) {
        struct bp_pmem_node *parent_ptr = (struct bp_pmem_node *) pmemobj_direct(parent_oid);
        struct bp_pmem_node *child_ptr = (struct bp_pmem_node *) pmemobj_direct(child_oid);

        if (child_ptr->is_leaf) {
            //new right-sibling node will get the same status as child
            PMEMoid sibling_oid = create_node(child_ptr->is_leaf);
            struct bp_pmem_node *sibling_ptr = (struct bp_pmem_node *) pmemobj_direct(sibling_oid);
            //new right-sibling child will hold the BP_MIN_DEGREE entries of child node
            sibling_ptr->nk = BP_MIN_DEGREE;

            //transfer the last BP_MIN_DEGREE entries of child node to it's sibling node
            for(int i=0; i<BP_MIN_DEGREE; i+=1) {
                sibling_ptr->keys[i] = child_ptr->keys[i + BP_MIN_DEGREE - 1];
                sibling_ptr->pointers[i] = child_ptr->pointers[i + BP_MIN_DEGREE - 1];
            }

            child_ptr->nk = BP_MIN_DEGREE - 1;

            //as parent node is going to get a new child at (idx+1)-th place, make a room for it
            for(int i = parent_ptr->nk; i >= idx+1; i -= 1) {
                parent_ptr->pointers[i+1] = parent_ptr->pointers[i];
            }

            //place sibling node as parent's children
            parent_ptr->pointers[idx+1] = sibling_oid;

            //a entry of child node will move to the parent node, make a room for it
            for(int i = parent_ptr->nk-1; i >= idx; i -= 1) {
                parent_ptr->keys[i+1] = parent_ptr->keys[i];
            }

            //place the middle entry of child node to parent node
            parent_ptr->keys[idx] = sibling_ptr->keys[0];

            //parent now hold a new entry, so increasing the number of keys
            parent_ptr->nk += 1;

            //adjust leaf chain pointers
            sibling_ptr->next = child_ptr->next;
            if (sibling_ptr->next.off != 0) {
                struct bp_pmem_node *sibling_next_ptr = (struct bp_pmem_node *) pmemobj_direct(sibling_ptr->next);
                sibling_next_ptr->previous = sibling_oid;
                pmemobj_persist(pop, sibling_next_ptr, sizeof(struct bp_pmem_node));
            }
            child_ptr->next = sibling_oid;
            sibling_ptr->previous = child_oid;

            //persist changes
            pmemobj_persist(pop, sibling_ptr, sizeof(struct bp_pmem_node));
            pmemobj_persist(pop, child_ptr, sizeof(struct bp_pmem_node));
            pmemobj_persist(pop, parent_ptr, sizeof(struct bp_pmem_node));
        }
        else {
            PMEMoid sibling_oid = create_node(child_ptr->is_leaf);    //new sibling node will get the same status as child
            struct bp_pmem_node *sibling_ptr = (struct bp_pmem_node *) pmemobj_direct(sibling_oid);

            sibling_ptr->nk = BP_MIN_DEGREE - 1;   //new sibling child will hold the (BP_MIN_DEGREE - 1) entries of child node

            //transfer the last (BP_MIN_DEGREE - 1) entries of child node to it's sibling node
            for(int i=0; i<BP_MIN_DEGREE-1; i+=1) {
                sibling_ptr->keys[i] = child_ptr->keys[i + BP_MIN_DEGREE];
            }

            //as child is an internal node, transfer the last BP_MIN_DEGREE chiddren of child node to it's sibling node
            for(int i=0; i<BP_MIN_DEGREE; i+=1) {
                sibling_ptr->pointers[i] = child_ptr->pointers[i + BP_MIN_DEGREE];
                //todo: it is possible to free few of child's memory, or can try with pmemobj_memmove
            }

            child_ptr->nk = BP_MIN_DEGREE - 1;

            //as parent node is going to get a new child at (idx+1)-th place, make a room for it
            for(int i = parent_ptr->nk; i >= idx+1; i -= 1) {
                parent_ptr->pointers[i+1] = parent_ptr->pointers[i];
            }

            //place sibling node as parent's children
            parent_ptr->pointers[idx+1] = sibling_oid;

            //a entry of child node will move to the parent node, make a room for it
            for(int i = parent_ptr->nk-1; i >= idx; i -= 1) {
                parent_ptr->keys[i+1] = parent_ptr->keys[i];
            }

            //place the middle entry of child node to parent node
            parent_ptr->keys[idx] = child_ptr->keys[BP_MIN_DEGREE - 1];

            //parent now hold a new entry, so increasing the number of keys
            parent_ptr->nk += 1;

            //persist changes
            pmemobj_persist(pop, sibling_ptr, sizeof(struct bp_pmem_node));
            pmemobj_persist(pop, child_ptr, sizeof(struct bp_pmem_node));
            pmemobj_persist(pop, parent_ptr, sizeof(struct bp_pmem_node));
        }
    }

    /**
     * BPPmem::insert_not_full -- (internal) inserts <key, value> pair into node
     * when this function called, we can assume that the node has space to hold new data
     */
    void BPPmem::insert_not_full(PMEMoid node_oid, uint64_t key, void *value) {
        struct bp_pmem_node *node_ptr = (struct bp_pmem_node *) pmemobj_direct(node_oid);
        int i = node_ptr->nk - 1;

        // if node is a leaf, insert the data to actual position and return
        if(node_ptr->is_leaf) {
            while(i>=0 && node_ptr->keys[i] > key) {
                node_ptr->keys[i+1] = node_ptr->keys[i];
                node_ptr->pointers[i+1] = node_ptr->pointers[i];
                i -= 1;
            }

            node_ptr->keys[i+1] = key;
            node_ptr->pointers[i+1] = create_record(value);
            node_ptr->nk += 1;

            //persist the changes
            pmemobj_persist(pop, node_ptr, sizeof(struct bp_pmem_node));
            return;
        }

        // the node is not a leaf
        // find the child which is going to store the new data
        while(i>=0 && node_ptr->keys[i] > key) {
            i -= 1;
        }

        //check if the child is full
        if(is_node_full(((struct bp_pmem_node *) pmemobj_direct(node_ptr->pointers[i+1]))->nk)) {
            //child is full, need to split
            split_node(i+1, node_oid, node_ptr->pointers[i+1]);

            //after the split, child's middle entry is pushed to parent
            //decide which children will hold the new <key,value> pair
            if(node_ptr->keys[i+1] < key) {
                i += 1;
            }
        }

        insert_not_full(node_ptr->pointers[i+1], key, value);
    }

    /**
     * BPPmem::insert -- inserts <key, value> pair into btree, will update the 'value' if 'key' already exists
     */
    int BPPmem::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);

        // if the key already exist in btree, update the value and return
        bool is_updated = update_if_found(root_p->root_node_oid, key, value);
        if(is_updated) return 1;        //we found the key, and value has been updated

        struct bp_pmem_node *root_node_ptr = (struct bp_pmem_node *) pmemobj_direct(root_p->root_node_oid);
        // if root is full
        if(is_node_full(root_node_ptr->nk)) {
            int idx = 0;

            PMEMoid new_root_oid = create_node(BP_LEAF_NODE_FALSE_FLAG);    //root is not a leaf anymore
            struct bp_pmem_node *new_root_ptr = (struct bp_pmem_node *) pmemobj_direct(new_root_oid);

            new_root_ptr->pointers[idx] = root_p->root_node_oid;
            split_node(idx, new_root_oid, root_p->root_node_oid);

            //new_root is holding two children now, decide which children will hold the new <key,value> pair
            if(new_root_ptr->keys[idx] < key) {
                idx += 1;
            }

            //new_root->children[idx] will definitely have space now, go ahead and insert the data to proper place
            insert_not_full(new_root_ptr->pointers[idx], key, value);
            pmemobj_persist(pop, new_root_ptr, sizeof(struct bp_pmem_node));  //persist changes on new_root_ptr

            //update the root
            root_p->root_node_oid = new_root_oid;

            //as root_p->root_node_oid updated, reassign root_p
            root_node_ptr = (struct bp_pmem_node *) pmemobj_direct(root_p->root_node_oid);
            pmemobj_persist(pop, root_node_ptr, sizeof(struct bp_pmem_node));    //persist changes on root_p
        }
        else {
            //root is not full, go ahead and insert the data to proper place
            insert_not_full(root_p->root_node_oid, key, value);
        }

        return 1;
    }

    /**
     * BPPmem::recursive_free -- recursively visit all the btree nodes and de-allocate memory
     */
    void BPPmem::recursive_free(PMEMoid current_node_oid) {
        struct bp_pmem_node *current_node_ptr = (struct bp_pmem_node *) pmemobj_direct(current_node_oid);
        //base case
        if(current_node_ptr->is_leaf) {
            //free the current_node's memory and return
            if(current_node_ptr->next.off != 0) {
                struct bp_pmem_node *next_node_ptr = (struct bp_pmem_node *) pmemobj_direct(current_node_ptr->next);
                next_node_ptr->previous = current_node_ptr->previous;
                pmemobj_persist(pop, next_node_ptr, sizeof(struct bp_pmem_node));
                pmemobj_free(&current_node_ptr->next);
                current_node_ptr->next = OID_NULL;
            }
            if(current_node_ptr->previous.off != 0) {
                struct bp_pmem_node *prev_node_ptr = (struct bp_pmem_node *) pmemobj_direct(current_node_ptr->previous);
                prev_node_ptr->next = current_node_ptr->next;
                pmemobj_persist(pop, prev_node_ptr, sizeof(struct bp_pmem_node));
                pmemobj_free(&current_node_ptr->previous);
                current_node_ptr->previous = OID_NULL;
            }
            for (int i = 0; i < current_node_ptr->nk; i += 1) {
                if(current_node_ptr->pointers[i].off != 0) {
                    pmemobj_free(&current_node_ptr->pointers[i]);
                    current_node_ptr->pointers[i] = OID_NULL;
                }
            }
            pmemobj_free(&current_node_oid);
            current_node_oid = OID_NULL;
            return;
        }

        //recursively call all the child nodes
        for(int i=0; i<current_node_ptr->nk+1; i+=1) {
            if(current_node_ptr->pointers[i].off != 0) {
                recursive_free(current_node_ptr->pointers[i]);
            }
        }

        //free the current_node's memory
        pmemobj_free(&current_node_oid);
        current_node_oid = OID_NULL;
    }

    /**
     * BPPmem::destroy -- destructor
     */
    void BPPmem::destroy() {
        check();

        recursive_free(root_p->root_node_oid);
        root_p->root_node_oid = OID_NULL;
        pmemobj_free(&root_oid);
        root_oid = OID_NULL;

        pmemobj_close(pop);
    }
}   //ycsbc