//
// Created by aislam6 on 11/22/19.
//

#ifndef YCSB_STOREDSBENCH_BPLUSTREE_COMMON_H
#define YCSB_STOREDSBENCH_BPLUSTREE_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /*
     * Data Structure Section
     */

    /* minimum degree - every node (except root) must contain (MIN_DEGREE - 1) keys */
    /* all nodes (including root) may contain at most (2*MIN_DEGREE - 1) keys */
    #define MIN_DEGREE 9

    /* maximum keys a node can hold */
    #define MAX_KEYS (2 * MIN_DEGREE - 1)

    /* maximum children a node can have */
    #define MAX_CHILDREN (2 * MIN_DEGREE)

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* size of the pmem object pool -- 1 GB */
    #define PMEM_BPLUSTREE_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define BPLUSTREE_LAYOUT_NAME "bplustree_layout"

    #define LEAF_NODE_TRUE_FLAG 1
    #define LEAF_NODE_FALSE_FLAG 0

    /* types of allocations */
    enum bplustree_alloc_types {
        BPLUSTREE_NODE_TYPE,
        BPLUSTREE_ENTRY_TYPE,

        BPLUSTREE_MAX_TYPES
    };

    /* declaration of data-structures */
    struct bplustree_entry {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
    } __attribute__ ((aligned (8)));

    struct bplustree_dram_node {
        //flag to check if the node is leaf or not
        int is_leaf;

        //current number of keys
        int nk;

        //array of <key-value> entries
        struct bplustree_entry entries[MAX_KEYS];

        //array of child pointer
        struct bplustree_dram_node *children[MAX_CHILDREN];

        //used for leaf type nodes
        struct bplustree_dram_node *next;
        struct bplustree_dram_node *previous;
    };

    struct bplustree_pmem_root {
        PMEMoid root_node_oid;
    };

    struct bplustree_pmem_root_concurrent_lock {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct bplustree_pmem_root_concurrent_mlock {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };

    struct bplustree_pmem_node {
        //flag to check if the node is leaf or not
        int is_leaf;

        //current number of keys
        int nk;

        //array of <key-value> entries
        struct bplustree_entry entries[MAX_KEYS];

        //array of child pointer
        PMEMoid children[MAX_CHILDREN];

        //used for leaf type nodes
        PMEMoid next;
        PMEMoid previous;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BPLUSTREE_COMMON_H
