//
// Created by aislam6 on 11/22/19.
//

#ifndef YCSB_STOREDSBENCH_BP_COMMON_H
#define YCSB_STOREDSBENCH_BP_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /*
     * Data Structure Section
     */

    /* minimum degree - every node (except root) must contain (BP_MIN_DEGREE - 1) keys */
    /* all nodes (including root) may contain at most (2*BP_MIN_DEGREE - 1) keys */
    #define BP_MIN_DEGREE 9

    /* maximum keys a node can hold */
    #define BP_MAX_KEYS (2 * BP_MIN_DEGREE - 1)

    /* maximum children a node can have */
    #define BP_MAX_CHILDREN (2 * BP_MIN_DEGREE)

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* size of the pmem object pool -- 1 GB */
    #define PMEM_BP_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define BP_LAYOUT_NAME "bp_layout"

    #define BP_LEAF_NODE_TRUE_FLAG 1
    #define BP_LEAF_NODE_FALSE_FLAG 0

    /* types of allocations */
    enum bp_alloc_types {
        BP_NODE_TYPE,
        BP_ENTRY_TYPE,

        BP_MAX_TYPES
    };

    /* declaration of data-structures */
    struct bp_record {
        char value[DEFAULT_VALUE_LEN];
    } __attribute__ ((aligned (8)));

    struct bp_dram_node {
        //flag to identify nodes as leaf or internal
        int is_leaf;

        //current number of keys
        int nk;

        //array of key
        uint64_t keys[BP_MAX_KEYS];

        //array of pointers
        //internal nodes will store the children information in this
        //leaf nodes will store the values in this
        void *pointers[BP_MAX_CHILDREN];

        //used for leaf type nodes
        struct bp_dram_node *next;
        struct bp_dram_node *previous;
    };

    struct bp_pmem_root {
        PMEMoid root_node_oid;
    };

    struct bp_pmem_root_concurrent_lock {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct bp_pmem_root_concurrent_mlock {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };

    struct bp_pmem_node {
        //flag to check if the node is leaf or not
        int is_leaf;

        //current number of keys
        int nk;

        //array of key
        uint64_t keys[BP_MAX_KEYS];

        //array of child pointer
        PMEMoid pointers[BP_MAX_CHILDREN];

        //used for leaf type nodes
        PMEMoid next;
        PMEMoid previous;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BP_COMMON_H
