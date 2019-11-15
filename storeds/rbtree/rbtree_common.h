//
// Created by aislam6 on 11/15/19.
//

#ifndef YCSB_STOREDSBENCH_RBTREE_COMMON_H
#define YCSB_STOREDSBENCH_RBTREE_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
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
        struct rbtree_dram_node *left, *right, *parent;
        bool color;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_RBTREE_COMMON_H
