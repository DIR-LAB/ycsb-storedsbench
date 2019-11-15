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

#include "array_common.h"

namespace ycsbc{
    class ArrayDram : public StoredsBase {
    public:
        ArrayDram(const char *path) {
            ArrayDram::init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

        ~ArrayDram();

    private:
        /* Private Data */
        char (*array)[DEFAULT_VALUE_LEN];

        int check();
    };

    int ArrayDram::check() {
        if (array == NULL) {
            fprintf(stderr, "malloc failed\n");
            return (-1);
        }
        return 1;
    }

    int ArrayDram::init(const char *path) {
        array = (char (*)[DEFAULT_VALUE_LEN]) malloc(ARRAY_SIZE * DEFAULT_VALUE_LEN);
        if (array == NULL) {
            fprintf(stderr, "malloc failed\n");
            return (-1);
        }
        return 1;
    }

    int ArrayDram::read(const char *key, void *&result) {
        check();
        uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (uint64_key % ARRAY_SIZE);
        result = array[offset];
        return 1;
    }

    int ArrayDram::update(const char *key, void *value) {
        check();
        uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (uint64_key % ARRAY_SIZE);
        strcpy(array[offset], (const char *) value);
        return 1;
    }

    int ArrayDram::insert(const char *key, void *value) {
        check();
        uint64_t uint64_key = strtoull(key, NULL, 0);
        int offset = (int) (uint64_key % ARRAY_SIZE);
        strcpy(array[offset], (const char *) value);
        return 1;
    }

    void ArrayDram::destroy() {
        check();
        free(array);
    }
}