//
// Created by aislam6 on 11/11/19.
//

#ifndef YCSB_STOREDSBENCH_BTREE_COMMON_H
#define YCSB_STOREDSBENCH_BTREE_COMMON_H

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
    #define PMEM_BTREE_POOL_SIZE ((size_t) (1 << 30))

    /* name of layout in the pool */
    #define BTREE_LAYOUT_NAME "btree_layout"

    /* types of allocations */
    enum btree_alloc_types {
        BTREE_NODE_TYPE,
        BTREE_ENTRY_TYPE,

        BTREE_MAX_TYPES
    };

    /* declaration of data-structures */
    struct entry {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
    } __attribute__ ((aligned (64)));

    struct btree_pmem_node {
        //flag to check if the node is leaf or not
        bool is_leaf;

        //current number of keys
        int nk;

        //array of <key-value> entries
        struct entry entries[MAX_KEYS];

        //array of child pointer
        PMEMoid children[MAX_CHILDREN];
    };

    struct btree_dram_node {
        //flag to check if the node is leaf or not
        bool is_leaf;

        //current number of keys
        int nk;

        //array of <key-value> entries
        struct entry entries[MAX_KEYS];

        //array of child pointer
        struct btree_dram_node *children[MAX_CHILDREN];
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BTREE_COMMON_H
