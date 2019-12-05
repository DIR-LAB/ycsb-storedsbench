//
// Created by aislam6 on 11/14/19.
//

#ifndef YCSB_STOREDSBENCH_ARRAY_COMMON_H
#define YCSB_STOREDSBENCH_ARRAY_COMMON_H

namespace ycsbc {
    /* size of the pmem object pool */
    #define PMEM_ARRAY_POOL_SIZE (160 * 1024 * 1024)

    /* name of layout in the pool */
    #define ARRAY_LAYOUT_NAME "array_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* size of the array */
    #define ARRAY_SIZE 1000000

    /* declaration of pmem data-structures */
    struct array_pmem_root {
        PMEMoid array;
    };

    struct array_pmem_elm {
        char value[DEFAULT_VALUE_LEN];
    };

    /**
     * types -- types of pmem allocations
     */
    enum array_pmem_types {
        ARRAY_TYPE,
        ARRAY_ELEMENT_TYPE,
        MAX_TYPES
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_ARRAY_COMMON_H
