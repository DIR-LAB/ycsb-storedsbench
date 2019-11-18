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
    #define DEFAULT_VALUE_LEN 101

    /* declaration of dram data-structures */
    struct ll_dram_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        struct ll_dram_node *next;
    };

    /* declaration of pmem data-structures */
    struct ll_pmem_root {
        PMEMoid head;
        PMEMoid tail;
    };

    /* declaration of pmem data-structures */
    struct ll_pmem_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        PMEMoid next;
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