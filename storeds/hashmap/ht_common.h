//
// Created by aislam6 on 11/13/19.
//

#ifndef YCSB_STOREDSBENCH_HT_COMMON_H
#define YCSB_STOREDSBENCH_HT_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /*
     * Data Structure Section
     */

    /* size of the pmem object pool -- 1 GB */
    #define PM_HASHTABLE_POOL_SIZE ((size_t) (1 << 30))

    /* name of layout in the pool */
    #define HT_LAYOUT_NAME "ht_layout"

    /* large prime number used as a hashing function coefficient */
    #define HASH_FUNC_COEFF_P 32212254719ULL

    /* initial number of buckets */
    #define INIT_BUCKETS_NUM 8192

    /* number of values in a bucket which trigger hashtable rebuild check */
    #define MIN_HASHSET_THRESHOLD 5

    /* number of values in a bucket which force hashtable rebuild */
    #define MAX_HASHSET_THRESHOLD 10

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /* types of allocations */
    enum ht_alloc_types {
        HT_ROOT_TYPE,
        HT_ENTRY_TYPE,
        HT_BUCKETS_TYPE,

        HT_MAX_TYPES
    };

    /* declaration of dram data-structures */
    struct dram_entry {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];

        /* next entry list pointer */
        struct dram_entry *next;
    } __attribute__ ((aligned (8)));

    struct dram_buckets {
        /* number of buckets */
        size_t nbuckets;

        /* array of lists */
        struct dram_entry **bucket;
    };

    struct dram_hashtable {
        /* random number generator seed */
        uint32_t seed;

        /* hash function coefficients */
        uint32_t hash_fun_coeff_a;	//hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_b;
        uint64_t hash_fun_coeff_p;

        /* buckets */
        struct dram_buckets *buckets;
    };

    /* declaration of pmem data-structures */
    struct pmem_entry {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];

        /* next entry list pointer */
        PMEMoid next;
    };

    struct pmem_buckets {
        /* number of buckets */
        size_t nbuckets;

        /* array of lists */
        PMEMoid bucket[];
    };

    struct pmem_hashtable {
        /* random number generator seed */
        uint32_t seed;

        /* hash function coefficients */
        uint32_t hash_fun_coeff_a;	//hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_b;
        uint64_t hash_fun_coeff_p;

        /* buckets */
        PMEMoid buckets;
    };

    struct pmem_hashtable_concurrent_lock {
        /* random number generator seed */
        uint32_t seed;

        /* hash function coefficients */
        uint32_t hash_fun_coeff_a;	//hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_b;
        uint64_t hash_fun_coeff_p;

        /* buckets */
        PMEMoid buckets;

        /* lock for concurrency */
        PMEMrwlock rwlock;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_HT_COMMON_H
