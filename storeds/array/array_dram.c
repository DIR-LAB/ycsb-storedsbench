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

int array_dram_init(const char *path){
    array = malloc (size * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "malloc failed\n");
        return(-1);
    }
    return 1;
}

int array_dram_read(const char *key, void *result){
    randIndex = rand() % 1000000;
    for ( int i = 0; i < size; i++) {
        counter = array[i];
    }
    return 1;
}
int array_dram_scan(const char *key, int len, void *result){
    for ( int i = 0; i < size; i++) {
        counter = array[i];
    }
    return 1;
}
int array_dram_update(const char *key, void *value){
    for ( int i = 0; i < size; i++) {
        array[i] = i;
    }
    return 1;
}
int array_dram_insert(const char *key, void *value){
    for ( int i = 0; i < size; i++) {
        array[i] = i;
    }
    return 1;
}
int array_dram_delete(const char *key){
    for ( int i = 0; i < size; i++) {
        array[i] = 0;
    }
    return 1;
}