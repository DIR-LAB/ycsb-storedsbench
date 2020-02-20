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
    class ArrayVmem : public StoredsBase {
    public:
        ArrayVmem(const char *path) {
            ArrayVmem::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~ArrayVmem();

    private:
        /* Private Data */
        VMEM *vmp;
        char (*array)[DEFAULT_VALUE_LEN] __attribute__ ((aligned (8)));

        int check();
    };

    int ArrayVmem::check() {
        if (array == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            return (-1);
        }
        return 1;
    }

    int ArrayVmem::init(const char *path) {
        if ((vmp = vmem_create(path, PMEM_ARRAY_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
		    exit(1);
	    }

        if ((array = (char (*)[DEFAULT_VALUE_LEN]) vmem_malloc(vmp, ARRAY_SIZE * DEFAULT_VALUE_LEN)) == NULL) {
		    fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
		    exit(1);
	    }

        return 1;
    }

    int ArrayVmem::read(const uint64_t key, void *&result) {
        check();
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (key % ARRAY_SIZE);
        result = array[offset];
        return 1;
    }

    int ArrayVmem::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        check();
        int offset = (int) (key % ARRAY_SIZE);
        for(int i=0; i<len && i<ARRAY_SIZE; i+=1) {
            std::vector <DB::Kuint64VstrPair> tmp;
            tmp.push_back(std::make_pair((offset + i), array[offset + i]));
            result.push_back(tmp);
        }
        return 1;
    }

    int ArrayVmem::update(const uint64_t key, void *value) {
        check();
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (key % ARRAY_SIZE);
        char _dummy_read[DEFAULT_VALUE_LEN];
        strcpy(_dummy_read, array[offset]);

        strcpy(array[offset], (const char *) value);
        return 1;
    }

    int ArrayVmem::insert(const uint64_t key, void *value) {
        check();
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (key % ARRAY_SIZE);
        strcpy(array[offset], (const char *) value);
        return 1;
    }

    void ArrayVmem::destroy() {
        check();
        vmem_free(vmp, array);
        vmem_delete(vmp);
    }
}