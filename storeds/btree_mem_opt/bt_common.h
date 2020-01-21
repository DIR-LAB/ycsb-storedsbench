//
// Created by aislam6 on 11/11/19.
//

#ifndef YCSB_STOREDSBENCH_BT_COMMON_H
#define YCSB_STOREDSBENCH_BT_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /*
     * Data Structure Section
     */

    /* minimum degree - every node (except root) must contain (BT_MIN_DEGREE - 1) keys */
    /* all nodes (including root) may contain at most (2*BT_MIN_DEGREE - 1) keys */
    #define BT_MIN_DEGREE 9

    /* maximum keys a node can hold */
    #define BT_MAX_KEYS (2 * BT_MIN_DEGREE - 1)

    /* maximum children a node can have */
    #define BT_MAX_CHILDREN (2 * BT_MIN_DEGREE)

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* size of the pmem object pool -- 1 GB */
    #define PMEM_BT_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define BT_LAYOUT_NAME "bt_layout"

    #define LEAF_NODE_TRUE_FLAG 1
    #define LEAF_NODE_FALSE_FLAG 0

    /* types of allocations */
    enum bt_alloc_types {
        BT_NODE_TYPE,
        BT_ENTRY_TYPE,

        BT_MAX_TYPES
    };

    /* declaration of data-structures */

    /*structure size: 136 Bytes*/
    struct bt_entry {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes
    } __attribute__ ((aligned (8)));

    /*structure size: {8 + (32 * BT_MAX_KEYS) + 16} Bytes*/
    struct bt_pmem_node {
        //flag to check if the node is leaf or not
        int is_leaf;    //4 Bytes

        //current number of keys
        int nk; //4 Bytes

        //array of <key-value> entries
        PMEMoid entries[BT_MAX_KEYS];   //(16 * BT_MAX_KEYS) Bytes

        //array of child pointer
        PMEMoid children[BT_MAX_CHILDREN];   //(16 * BT_MAX_CHILDREN) Bytes
    };

    struct bt_pmem_root {
        PMEMoid root_node_oid;
    };

    struct bt_pmem_root_concurrent_lock {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct bt_pmem_root_concurrent_mlock {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };

    /*structure size: {8 + (16 * BT_MAX_KEYS) + 8} Bytes*/
    struct bt_dram_node {
        //flag to check if the node is leaf or not
        int is_leaf;    //4 Bytes

        //current number of keys
        int nk; //4 Bytes

        //array of <key-value> entries
        struct bt_entry *entries[BT_MAX_KEYS];   //(8 * BT_MAX_KEYS) Bytes

        //array of child pointer
        struct bt_dram_node *children[BT_MAX_CHILDREN];   //(8 * BT_MAX_CHILDREN) Bytes
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BT_COMMON_H
