//
//  array_dram.c
//  DRAM Performance
//
#include <stdio.h>
#include <stdlib.h>
#include "array_dram.h"
#include <time.h>
#include <math.h>

int *array = NULL;
const int size = 1000000;

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
    array = (int *) malloc(size * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "malloc failed\n");
        return (-1);
    }
    return 1;
}

int array_dram_read(const char *key, void *result) {
    check();
    result = &array[atol(key) % size];
    return 1;
}

int array_dram_update(const char *key, void *value) {
    check();
    array[atol(key) % size] = atoi((const char *) value);
    return 1;
}

int array_dram_insert(const char *key, void *value) {
    check();
    array[atol(key) % size] = atoi((const char *) value);
    return 1;
}