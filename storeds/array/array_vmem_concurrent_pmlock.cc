//
//  array_vmem.c
//  Volatile Persistent Memory Performance
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <libvmem.h>

#include "array_common.h"

namespace ycsbc{
    class ArrayVmemConcurrentPMLock : public StoredsBase {
    public:
        ArrayVmemConcurrentPMLock(const char *path) {
            ArrayVmemConcurrentPMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~ArrayVmemConcurrentPMLock();

    private:
        /* Private Data */
        VMEM *vmp;
        struct array_vmem_concurrent_pmlock *pmem_lock_p = NULL;
        char (*array)[DEFAULT_VALUE_LEN] __attribute__ ((aligned (8)));

        int check();
    };

    int ArrayVmemConcurrentPMLock::check() {
        if (array == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            return (-1);
        }
        return 1;
    }

    int ArrayVmemConcurrentPMLock::init(const char *path) {
        if ((vmp = vmem_create(path, PMEM_ARRAY_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
		    exit(1);
	    }

        if ((pmem_lock_p = (struct array_vmem_concurrent_pmlock *) vmem_malloc(vmp, sizeof(struct array_vmem_concurrent_pmlock))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        if(pthread_mutex_init(&pmem_lock_p->mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }

        if ((array = (char (*)[DEFAULT_VALUE_LEN]) vmem_malloc(vmp, ARRAY_SIZE * DEFAULT_VALUE_LEN)) == NULL) {
		    fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
		    exit(1);
	    }

        return 1;
    }

    int ArrayVmemConcurrentPMLock::read(const uint64_t key, void *&result) {
        check();
        if (pthread_mutex_lock(&pmem_lock_p->mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        result = array[offset];

        pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
        return 1;
    }

    int ArrayVmemConcurrentPMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();
        if (pthread_mutex_lock(&pmem_lock_p->mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        for(int i=0; i<len && i<ARRAY_SIZE; i+=1) {
            std::vector <DB::Kuint64VstrPair> tmp;
            tmp.push_back(std::make_pair((offset + i), array[offset + i]));
            result.push_back(tmp);
        }

        pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
        return 1;
    }

    int ArrayVmemConcurrentPMLock::update(const uint64_t key, void *value) {
        check();
        if (pthread_mutex_lock(&pmem_lock_p->mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        char _dummy_read[DEFAULT_VALUE_LEN];
        strcpy(_dummy_read, array[offset]);

        strcpy(array[offset], (const char *) value);

        pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
        return 1;
    }

    int ArrayVmemConcurrentPMLock::insert(const uint64_t key, void *value) {
        check();
        if (pthread_mutex_lock(&pmem_lock_p->mutex_lock_) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        strcpy(array[offset], (const char *) value);

        pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
        return 1;
    }

    void ArrayVmemConcurrentPMLock::destroy() {
        check();
        vmem_free(vmp, array);

        pthread_mutex_destroy(&pmem_lock_p->mutex_lock_);
        vmem_free(vmp, pmem_lock_p);
        pmem_lock_p = NULL;

        vmem_delete(vmp);
    }
}