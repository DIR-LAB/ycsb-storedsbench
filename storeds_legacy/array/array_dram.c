//
//  array_dram.c
//  DRAM Performance
//
#include <stdio.h>
#include <stdlib.h>
#include "array_dram.h"
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

const int size = 1000000;
const int default_value_len = 101;

char (*array)[default_value_len];

int volatile counter;
int randIndex;

int check(){
    if (array == NULL) {
        fprintf(stderr, "malloc failed\n");
        return (-1);
    }
    return 1;
}

int array_dram_init(const char *path) {
    array = (char (*)[default_value_len]) malloc(size * default_value_len);
    if (array == NULL) {
        fprintf(stderr, "malloc failed\n");
        return (-1);
    }
    return 1;
}

int array_dram_read(const char *key, void *&result) {
    check();
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % size);
    result = array[offset];
    return 1;
}

int array_dram_update(const char *key, void *value) {
    check();
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % size);
    strcpy(array[offset], (const char *) value);
    return 1;
}

int array_dram_insert(const char *key, void *value) {
    check();
    uint64_t uint64_key = strtoull(key, NULL, 0);
    int offset = (int) (uint64_key % size);
    strcpy(array[offset], (const char *) value);
    return 1;
}

void array_dram_free() {
    check();
    free(array);
}