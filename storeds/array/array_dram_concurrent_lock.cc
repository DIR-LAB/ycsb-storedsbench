//
//  array_dram.c
//  DRAM Performance
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include "array_common.h"

namespace ycsbc{
    class ArrayDramConcurrentLock : public StoredsBase {
    public:
        ArrayDramConcurrentLock(const char *path) {
            ArrayDramConcurrentLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~ArrayDramConcurrentLock();

    private:
        /* Private Data */
        char (*array)[DEFAULT_VALUE_LEN] __attribute__ ((aligned (8)));
        pthread_rwlock_t rwlock;

        int check();
    };

    int ArrayDramConcurrentLock::check() {
        if (array == NULL) {
            fprintf(stderr, "malloc failed\n");
            return (-1);
        }
        return 1;
    }

    int ArrayDramConcurrentLock::init(const char *path) {
        if(pthread_rwlock_init(&rwlock, 0) != 0) {
            fprintf(stderr, "[%s]: FATAL: RW Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }

        array = (char (*)[DEFAULT_VALUE_LEN]) malloc(ARRAY_SIZE * DEFAULT_VALUE_LEN);
        if (array == NULL) {
            fprintf(stderr, "malloc failed\n");
            return (-1);
        }
        return 1;
    }

    int ArrayDramConcurrentLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();
        if (pthread_rwlock_rdlock(&rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        for(int i=0; i<len && i<ARRAY_SIZE; i+=1) {
            std::vector <DB::Kuint64VstrPair> tmp;
            tmp.push_back(std::make_pair((offset + i), array[offset + i]));
            result.push_back(tmp);
        }

        pthread_rwlock_unlock(&rwlock);
        return 1;
    }

    int ArrayDramConcurrentLock::read(const uint64_t key, void *&result) {
        check();
        if (pthread_rwlock_rdlock(&rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        result = array[offset];

        pthread_rwlock_unlock(&rwlock);
        return 1;
    }

    int ArrayDramConcurrentLock::update(const uint64_t key, void *value) {
        check();
        if (pthread_rwlock_wrlock(&rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        char _dummy_read[DEFAULT_VALUE_LEN];
        strcpy(_dummy_read, array[offset]);

        strcpy(array[offset], (const char *) value);

        pthread_rwlock_unlock(&rwlock);
        return 1;
    }

    int ArrayDramConcurrentLock::insert(const uint64_t key, void *value) {
        check();
        if (pthread_rwlock_wrlock(&rwlock) != 0) return 0;

        int offset = (int) (key % ARRAY_SIZE);
        strcpy(array[offset], (const char *) value);

        pthread_rwlock_unlock(&rwlock);
        return 1;
    }

    void ArrayDramConcurrentLock::destroy() {
        check();
        free(array);
        pthread_rwlock_destroy(&rwlock);
    }
}