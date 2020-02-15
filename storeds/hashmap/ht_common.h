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
    #define PM_HASHTABLE_POOL_SIZE ((size_t) ((size_t) 1 << 31))

    /* name of layout in the pool */
    #define HT_LAYOUT_NAME "ht_layout"

    /* large prime number used as a hashing function coefficient */
    #define HASH_FUNC_COEFF_P 32212254719ULL

    /* initial number of buckets */
    //#define INIT_BUCKETS_NUM 8192   //load factor: 12.20
    #define INIT_BUCKETS_NUM 1333334   //load factor: .75

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
    /*structure size: 144 Bytes*/
    struct dram_entry {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes

        /* next entry list pointer */
        struct dram_entry *next;    //8 Bytes
    } __attribute__ ((aligned (8)));

    /*structure size: 16 Bytes*/
    struct dram_buckets {
        /* number of buckets */
        size_t nbuckets;    //8 Bytes

        /* array of lists */
        struct dram_entry **bucket; //8 Bytes
    };

    /*structure size: 28 Bytes*/
    struct dram_hashtable {
        /* random number generator seed */
        uint32_t seed;  //4 Bytes

        /* hash function coefficients */
        //hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_a;  //4 Bytes
        uint32_t hash_fun_coeff_b;  //4 Bytes
        uint64_t hash_fun_coeff_p;  //8 Bytes

        /* buckets */
        struct dram_buckets *buckets;   //8 Bytes
    };

    /* declaration of pmem data-structures */
    /*structure size: 152 Bytes*/
    struct pmem_entry {
        uint64_t key;   //8 Bytes
        char value[DEFAULT_VALUE_LEN];  //128 Bytes

        /* next entry list pointer */
        PMEMoid next;   //16 Bytes
    };

    struct pmem_buckets {
        /* number of buckets */
        size_t nbuckets;    //8 Bytes

        /* array of lists */
        PMEMoid bucket[];   //(16 * 8192) Bytes
        //todo: this sizing is bit confusing
    };

    /*structure size: 36 Bytes*/
    struct pmem_hashtable {
        /* random number generator seed */
        uint32_t seed;  //4 Bytes

        /* hash function coefficients */
        //hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_a;  //4 Bytes
        uint32_t hash_fun_coeff_b;  //4 Bytes
        uint64_t hash_fun_coeff_p;  //8 Bytes

        /* buckets */
        PMEMoid buckets;    //16 Bytes
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

    struct pmem_hashtable_concurrent_mlock {
        /* random number generator seed */
        uint32_t seed;

        /* hash function coefficients */
        uint32_t hash_fun_coeff_a;	//hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_b;
        uint64_t hash_fun_coeff_p;

        /* buckets */
        PMEMoid buckets;

        /* lock for concurrency */
        PMEMmutex mlock;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_HT_COMMON_H
