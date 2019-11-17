//
// Created by aislam6 on 11/15/19.
//

#ifndef YCSB_STOREDSBENCH_RBTREE_COMMON_H
#define YCSB_STOREDSBENCH_RBTREE_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /* size of the pmem object pool -- 1 GB */
    #define PMEM_RB_POOL_SIZE ((size_t) (1 << 30))

    /* name of layout in the pool */
    #define RB_LAYOUT_NAME "rbtree_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 101

    /**
     * rbtree_color -- rbtree color list
     */
    enum rbtree_color {
        RED,
        BLACK
    };

    /* declaration of dram data-structures */
    struct rbtree_dram_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        struct rbtree_dram_node *left;
        struct rbtree_dram_node *right;
        struct rbtree_dram_node *parent;
        bool color;
    };

    /* declaration of pmem data-structures */
    struct rbtree_pmem_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        PMEMoid left;
        PMEMoid right;
        PMEMoid parent;
        bool color;
    };

    struct rbtree_pmem_root {
        PMEMoid root;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_RBTREE_COMMON_H
