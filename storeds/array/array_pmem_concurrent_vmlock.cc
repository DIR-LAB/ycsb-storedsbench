/*
 * array-pmem.c
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <libpmemobj.h>
#include <pthread.h>

#include "../../lib/ex_common.h"
#include "array_common.h"

namespace ycsbc {
    class ArrayPmemConcurrentVMLock : public StoredsBase {
    public:
        ArrayPmemConcurrentVMLock(const char *path) {
            ArrayPmemConcurrentVMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~ArrayPmemConcurrentVMLock();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        pthread_mutex_t mutex_lock_;
        struct array_pmem_root *root_p = NULL;

        int check();

        void print_toid(struct array_info *info);
    };

    /**
     * check -- check if pmem array has been initialized
     */
    int ArrayPmemConcurrentVMLock::check() {
        if (root_p->array.off == 0) {
            fprintf(stderr, "[%s]: FATAL: array not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * print_toid -- print array of TOID(struct array_pmem_elm) type
     */
    void ArrayPmemConcurrentVMLock::print_toid(struct array_info *info) {
        for (int i = 0; i < ARRAY_SIZE; i++) {
            struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + i * sizeof(struct array_pmem_elm));
            printf("array element [%d] -> %s\n", i, ptr->value);
        }
    }

    /**
     * init -- initialize the array
     */
    int ArrayPmemConcurrentVMLock::init(const char *path){
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, ARRAY_LAYOUT_NAME, PMEM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        } else {
            if ((pop = pmemobj_open(path, ARRAY_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct array_pmem_root));
        root_p = (struct array_pmem_root *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }
        size_t array_size = sizeof(struct array_pmem_elm) * ARRAY_SIZE;
        pmemobj_alloc(pop, &root_p->array, array_size, ARRAY_TYPE, NULL, NULL);
        pmemobj_persist(pop, root_p, sizeof(struct array_pmem_root));

        if(pthread_mutex_init(&mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }

        check();
        return 1;
    }

    int ArrayPmemConcurrentVMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        for(int i=0; i<len && i<ARRAY_SIZE; i+=1) {
            std::vector <DB::Kuint64VstrPair> tmp;
            struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + (offset + i) * sizeof(struct array_pmem_elm));
            tmp.push_back(std::make_pair((offset + i), ptr->value));
            result.push_back(tmp);
        }

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * read -- read data from array[key] and set the result in result
     */
    int ArrayPmemConcurrentVMLock::read(const uint64_t key, void *&result){
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (key % ARRAY_SIZE);

        struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_pmem_elm));
        result = ptr->value;

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * update -- update data to array[key] by value
     */
    int ArrayPmemConcurrentVMLock::update(const uint64_t key, void *value){
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_pmem_elm));
        char _dummy_read[DEFAULT_VALUE_LEN];
        strcpy(_dummy_read, ptr->value);

        strcpy(ptr->value, (const char *) value);
        pmemobj_persist(pop, ptr, sizeof(struct array_pmem_elm));

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * insert -- insert data value into array[key]
     */
    int ArrayPmemConcurrentVMLock::insert(const uint64_t key, void *value){
        //printf("%s %s\n", key, (const char *) value);
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        struct array_pmem_elm *ptr = (struct array_pmem_elm *) ((char *)pmemobj_direct(root_p->array) + offset * sizeof(struct array_pmem_elm));
        strcpy(ptr->value, (const char *) value);
        pmemobj_persist(pop, ptr, sizeof(struct array_pmem_elm));

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * destroy -- destructor
     */
    void ArrayPmemConcurrentVMLock::destroy() {
        check();
        pmemobj_free(&root_p->array);
        root_p->array = OID_NULL;
        pmemobj_free(&root_oid);
        root_oid = OID_NULL;

        pthread_mutex_destroy(&mutex_lock_);

        pmemobj_close(pop);
    }
}   //ycsbc