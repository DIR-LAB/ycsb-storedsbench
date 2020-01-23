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
    #define PMEM_SKIPLIST_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define SK_LAYOUT_NAME "sklist_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

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

    /*structure size: 136 Bytes*/
    struct sk_entry {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes
    } __attribute__ ((aligned (8)));

    /* declaration of dram data-structures */
    /*structure size: 264 Bytes*/
    struct sk_dram_node {
        /* skiplist entry */
        struct sk_entry entry;  //136 Bytes

        /* next entry pointer */
        struct sk_dram_node *next[SKIPLIST_LEVELS_NUM]; //16*8 Bytes => 128 Bytes
    };

    /* declaration of pmem data-structures */
    /*structure size: 392 Bytes*/
    struct sk_pmem_node {
        /* skiplist entry */
        struct sk_entry entry;  //136 Bytes

        /* next entry pointer */
        PMEMoid next[SKIPLIST_LEVELS_NUM];  //16*16 Bytes => 256 Bytes
    };

    struct sk_pmem_root {
        PMEMoid root_node_oid;
    };

    struct sk_pmem_root_concurrent_lock {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct sk_pmem_root_concurrent_mlock {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_SKIPLIST_COMMON_H
