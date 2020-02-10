//
// Created by aislam6 on 11/13/19.
//

#ifndef YCSB_STOREDSBENCH_LINKEDLIST_COMMON_H
#define YCSB_STOREDSBENCH_LINKEDLIST_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /* size of the pmem object pool -- 1 GB */
    #define PMEM_LL_POOL_SIZE ((size_t) (1 << 30))

    /* name of layout in the pool */
    #define LL_LAYOUT_NAME "linkedlist_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* declaration of dram data-structures */
    struct ll_dram_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        struct ll_dram_node *next;
    } __attribute__ ((aligned (8)));

    /* declaration of pmem data-structures */
    struct ll_pmem_root {
        PMEMoid head;
        PMEMoid tail;
    };

    struct ll_pmem_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        PMEMoid next;
    };

    /* declaration of pmem concurrent data-structures */
    struct ll_pmem_concurrent_mlock_root {
        PMEMoid head;
        PMEMoid tail;

        PMEMmutex mlock;
    };

    /**
     * types -- types of linkedlist pmem allocations
     */
    enum ll_alloc_types {
        LL_NODE_TYPE,
        LL_MAX_TYPES
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_LINKEDLIST_COMMON_H
