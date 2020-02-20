/*
 * array_pmem_tx.c -- persistent memory array implementation with transaction support
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>
#include <time.h>

#include "../../lib/ex_common.h"
#include "array_common.h"

namespace ycsbc {
    class ArrayPmemTxConcurrentLock : public StoredsBase {
    public:
        ArrayPmemTxConcurrentLock(const char *path) {
            ArrayPmemTxConcurrentLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~ArrayPmemTxConcurrentLock();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct array_pmem_concurrent_lock_root *root_p = NULL;

        int check();

        void print_toid_tx();
    };

    /**
     * check -- check if array has been initialized
     */
    int ArrayPmemTxConcurrentLock::check() {
        if (root_p->array.off == 0) {
            fprintf(stderr, "[%s]: FATAL: array not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * print_toid_tx -- print array of TOID(struct array_pmem_elm) type
     */
    void ArrayPmemTxConcurrentLock::print_toid_tx() {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + i * sizeof(struct array_pmem_elm));
            printf("array element [%d] -> %s\n", i, ptr->value);
        }
    }

    /**
     * init -- initialize the array
     */
    int ArrayPmemTxConcurrentLock::init(const char *path){
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, ARRAY_LAYOUT_NAME, PMEM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        } else {
            if ((pop = pmemobj_open(path, ARRAY_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct array_pmem_concurrent_lock_root));
        root_p = (struct array_pmem_concurrent_lock_root *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }

        TX_BEGIN(pop) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct array_pmem_concurrent_lock_root));

            //note: why not the following one?
            //pmemobj_tx_add_range_direct(pmemobj_direct(root_p->array), sizeof(struct array_pmem_elm) * ARRAY_SIZE);

            root_p->array = pmemobj_tx_alloc(sizeof(struct array_pmem_elm) * ARRAY_SIZE, ARRAY_TYPE);
            //pmemobj_persist(pop, &root_p->array, sizeof(PMEMoid));
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        check();
        return 1;
    }

    int ArrayPmemTxConcurrentLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();
        if (pmemobj_rwlock_rdlock(pop, &root_p->rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        for(int i=0; i<len && i<ARRAY_SIZE; i+=1) {
            std::vector <DB::Kuint64VstrPair> tmp;
            struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + (offset + i) * sizeof(struct array_pmem_elm));
            tmp.push_back(std::make_pair((offset + i), ptr->value));
            result.push_back(tmp);
        }

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * read -- read data from array[key] and set the result in result
     */
    int ArrayPmemTxConcurrentLock::read(const uint64_t key, void *&result){
        check();
        if (pmemobj_rwlock_rdlock(pop, &root_p->rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_pmem_elm));
        result = ptr->value;

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * update -- update data to array[key] by value
     */
    int ArrayPmemTxConcurrentLock::update(const uint64_t key, void *value){
        check();
        if (pmemobj_rwlock_wrlock(pop, &root_p->rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        TX_BEGIN(pop) {
            struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_pmem_elm));
            char _dummy_read[DEFAULT_VALUE_LEN];
            strcpy(_dummy_read, ptr->value);

            pmemobj_tx_add_range_direct(ptr, sizeof(struct array_pmem_elm));
            //pmemobj_tx_add_range(pmemobj_oid(ptr), 0, sizeof(struct array_pmem_elm));
            strcpy(ptr->value, (const char *) value);
            //pmemobj_persist(pop, ptr, sizeof(struct array_pmem_elm));
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * insert -- insert data value into array[key]
     */
    int ArrayPmemTxConcurrentLock::insert(const uint64_t key, void *value){
        check();
        if (pmemobj_rwlock_wrlock(pop, &root_p->rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        TX_BEGIN(pop) {
            struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_pmem_elm));
            pmemobj_tx_add_range_direct(ptr, sizeof(struct array_pmem_elm));
            strcpy(ptr->value, (const char *) value);
            //pmemobj_persist(pop, ptr, sizeof(struct array_pmem_elm));
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        pmemobj_rwlock_unlock(pop, &root_p->rwlock);
        return 1;
    }

    /**
     * destroy -- destructor
     */
    void ArrayPmemTxConcurrentLock::destroy() {
        check();
        TX_BEGIN(pop) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct array_pmem_concurrent_lock_root));
            pmemobj_tx_free(root_p->array);
            root_p->array = OID_NULL;
            pmemobj_tx_free(root_oid);
            root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        pmemobj_close(pop);
    }
}   //ycsbc