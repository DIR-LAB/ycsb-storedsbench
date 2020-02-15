//
// Created by aislam6 on 11/15/19.
//

#ifndef YCSB_STOREDSBENCH_RBTREE_COMMON_H
#define YCSB_STOREDSBENCH_RBTREE_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /* size of the pmem object pool -- 1 GB */
    #define PMEM_RB_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define RB_LAYOUT_NAME "rbtree_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /**
     * rbtree_color -- rbtree color list
     */
    enum rbtree_color {
        RED,
        BLACK
    };

    /* declaration of dram data-structures */
    /*structure size: 168 Bytes*/
    struct rbtree_dram_node {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes
        struct rbtree_dram_node *left;  //8 Bytes
        struct rbtree_dram_node *right; //8 Bytes
        struct rbtree_dram_node *parent;    //8 Bytes
        bool color; //1 Bytes
        char padding[7];    //7 Bytes
    } __attribute__ ((aligned (8)));

    /* declaration of pmem data-structures */
    /*structure size: 192 Bytes*/
    struct rbtree_pmem_node {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes
        PMEMoid left;   //16 Bytes
        PMEMoid right;  //16 Bytes
        PMEMoid parent; //16 Bytes
        bool color; //1 Bytes
        char padding[7];    //7 Bytes
    };

    struct rbtree_pmem_root {
        PMEMoid root_node_oid;
    };

    struct rbtree_pmem_concurrent_lock_root {
        PMEMoid root_node_oid;
        PMEMrwlock rwlock;
    };

    struct rbtree_pmem_concurrent_mlock_root {
        PMEMoid root_node_oid;
        PMEMmutex mlock;
    };

    struct rbtree_vmem_concurrent_pmlock {
        pthread_mutex_t mutex_lock_;
    };

    /**
     * types -- types of linkedlist pmem allocations
     */
    enum rbtree_alloc_types {
        RB_NODE_TYPE,
        RB_MAX_TYPES
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_RBTREE_COMMON_H
