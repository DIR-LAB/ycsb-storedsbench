//
// Created by aislam6 on 11/13/19.
//

#ifndef YCSB_STOREDSBENCH_SKIPLIST_COMMON_H
#define YCSB_STOREDSBENCH_SKIPLIST_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {

    /*
     * Data Structure Section
     */

    /* size of the pmem object pool -- 1 GB */
    #define PMEM_SKIPLIST_POOL_SIZE ((size_t) (1 << 30))

    /* name of layout in the pool */
    #define SK_LAYOUT_NAME "sklist_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 101

    /* number of levels in skiplist */
    #define SKIPLIST_LEVELS_NUM 16

    /* base level of skiplist */
    #define SKIPLIST_BASE_LEVEL 0

    /* types of allocations */
    enum sk_alloc_types {
        SK_ROOT_TYPE,
        SK_ENTRY_TYPE,
        SK_NODE_TYPE,

        SK_MAX_TYPES
    };

    struct sk_entry {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
    };

    /* declaration of dram data-structures */
    struct sk_dram_node {
        /* skiplist entry */
        struct sk_entry entry;

        /* next entry pointer */
        struct sk_dram_node *next[SKIPLIST_LEVELS_NUM];
    };

    /* declaration of pmem data-structures */
    struct sk_pmem_node {
        /* skiplist entry */
        struct sk_entry entry;

        /* next entry pointer */
        PMEMoid next[SKIPLIST_LEVELS_NUM];
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_SKIPLIST_COMMON_H
