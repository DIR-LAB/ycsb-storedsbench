//
// Created by aislam6 on 11/13/19.
//

#ifndef YCSB_STOREDSBENCH_HL_COMMON_H
#define YCSB_STOREDSBENCH_HL_COMMON_H

#include <libpmemobj.h>

namespace ycsbc {
    /*
     * Data Structure Section
     */

    /*structure size: 144 Bytes*/
    struct hl_dram_entry {
        uint64_t key;   //8 Bytes

        /* pointer to the linkedlist */
        void *ll_p;    //8 Bytes

        /* next entry list pointer */
        struct hl_dram_entry *next;    //8 Bytes
    } __attribute__ ((aligned (8)));

    /*structure size: 16 Bytes*/
    struct hl_dram_buckets {
        /* number of buckets */
        size_t nbuckets;    //8 Bytes

        /* array of lists */
        struct hl_dram_entry **bucket; //8 Bytes
    };

    /*structure size: 28 Bytes*/
    struct dram_hashlist {
        /* random number generator seed */
        uint32_t seed;  //4 Bytes

        /* hash function coefficients */
        //hash_fun_a can not contain value '0'
        uint32_t hash_fun_coeff_a;  //4 Bytes
        uint32_t hash_fun_coeff_b;  //4 Bytes
        uint64_t hash_fun_coeff_p;  //8 Bytes

        /* buckets */
        struct hl_dram_buckets *buckets;   //8 Bytes
    };

    struct hashlist_vmem_pmlock {
        pthread_mutex_t mutex_lock_;
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_HL_COMMON_H
