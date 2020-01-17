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

    /* minimum degree - every node (except root) must contain (BTREE_MIN_DEGREE - 1) keys */
    /* all nodes (including root) may contain at most (2*BTREE_MIN_DEGREE - 1) keys */
    #define BTREE_MIN_DEGREE 9

    /* maximum keys a node can hold */
    #define BTREE_MAX_KEYS (2 * BTREE_MIN_DEGREE - 1)

    /* maximum children a node can have */
    #define BTREE_MAX_CHILDREN (2 * BTREE_MIN_DEGREE)

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* size of the pmem object pool -- 1 GB */
    #define PMEM_BTREE_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define BTREE_LAYOUT_NAME "btree_layout"

    #define LEAF_NODE_TRUE_FLAG 1
    #define LEAF_NODE_FALSE_FLAG 0

    /* types of allocations */
    enum btree_alloc_types {
        BTREE_NODE_TYPE,
        BTREE_ENTRY_TYPE,

        BTREE_MAX_TYPES
    };

    /* declaration of data-structures */

    /*structure size: 136 Bytes*/
    struct entry {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes
    } __attribute__ ((aligned (8)));

    /*structure size: {8 + (152 * BTREE_MAX_KEYS) + 16} Bytes*/
    struct btree_pmem_node {
        //flag to check if the node is leaf or not
        int is_leaf;    //4 Bytes

        //current number of keys
        int nk; //4 Bytes

        //array of <key-value> entries
        struct entry entries[BTREE_MAX_KEYS];   //(136 * BTREE_MAX_KEYS) Bytes

        //array of child pointer
        PMEMoid children[BTREE_MAX_CHILDREN];   //(16 * BTREE_MAX_CHILDREN) Bytes
    };

    struct btree_pmem_root {
        PMEMoid root_node_oid;
    };

    struct btree_pmem_root_concurrent_lock {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct btree_pmem_root_concurrent_mlock {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };

    /*structure size: {8 + (144 * BTREE_MAX_KEYS) + 8} Bytes*/
    struct btree_dram_node {
        //flag to check if the node is leaf or not
        int is_leaf;    //4 Bytes

        //current number of keys
        int nk; //4 Bytes

        //array of <key-value> entries
        struct entry entries[BTREE_MAX_KEYS];   //(136 * BTREE_MAX_KEYS) Bytes

        //array of child pointer
        struct btree_dram_node *children[BTREE_MAX_CHILDREN];   //(8 * BTREE_MAX_CHILDREN) Bytes
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BTREE_COMMON_H
