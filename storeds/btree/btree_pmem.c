//
// Created by aislam6 on 10/27/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "btree_pmem.h"

/*  
 * Data Structure Section
 */

int btree_pmem_init(const char *path) {
    return  1;
}

int btree_pmem_read(const char *key, void *result) {
    return  1;
}

int btree_pmem_update(const char *key, void *value) {
    return  1;
}

int btree_pmem_insert(const char *key, void *value) {
    return  1;
}

void btree_pmem_free() {
    //
}