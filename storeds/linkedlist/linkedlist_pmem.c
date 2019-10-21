//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "linkedlist_pmem.h"

/* size of the pmem object pool -- 1 GB */
#define PM_ARRAY_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "linkedlist_layout"

/* Static Global Data */
static PMEMobjpool *pop = NULL;
static PMEMoid root_oid;
static struct ll_root *root_p = NULL;

const int pmem_array_size = 1000000;
const int default_value_len = 101;

struct ll_root {
    PMEMoid head;
    PMEMoid tail;
};

struct node {
    uint64_t key;
    char value[default_value_len];
    PMEMoid next;
};

/**
 * types -- types of allocations
 */
enum types {
    NODE_TYPE,
    MAX_TYPES
};

int linkedlist_pmem_check() {
    if (root_p->head.off == 0) {
        fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

int linkedlist_pmem_init(const char *path) {
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    } else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    }

    root_oid = pmemobj_root(pop, sizeof(struct ll_root));
    root_p = (struct ll_root *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
        exit(0);
    }
    //printf("initialization done\n");
    return 1;
}

int linkedlist_pmem_read(const char *key, void *result) {
    //printf("read: %s\n", key);
    linkedlist_pmem_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = (struct node *) pmemobj_direct(root_p->head);

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            result = current_node->value;
            break;
        }
        current_node = (struct node *) pmemobj_direct(current_node->next);
    }
    //printf("read result: %s\n", (char *) result);
    return 1;
}

int linkedlist_pmem_update(const char *key, void *value) {
    //printf("update: %s %s\n", key, (char *) value);
    linkedlist_pmem_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = (struct node *) pmemobj_direct(root_p->head);

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            pmemobj_memcpy_persist(pop, current_node->value, (const char *) value, strlen((char *) value));
            break;
        }
        current_node = (struct node *) pmemobj_direct(current_node->next);
    }
    return 1;
}

int linkedlist_pmem_insert(const char *key, void *value) {
    //printf("insert: %s %s\n", key, (char *) value);
    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *in_memory_node_ptr = (struct node *) malloc(sizeof(struct node));
    in_memory_node_ptr->key = uint64_key;
    strcpy(in_memory_node_ptr->value, (const char *) value);

    PMEMoid new_node_oid;
    pmemobj_zalloc(pop, &new_node_oid, sizeof(struct node), NODE_TYPE);
    struct node *pmem_node_ptr = (struct node *) pmemobj_direct(new_node_oid);
    pmemobj_memcpy_persist(pop, pmem_node_ptr, in_memory_node_ptr, sizeof(struct node));
    free(in_memory_node_ptr);

    if(root_p->head.off == 0) {
        //printf("here i come 3\n");
        root_p->head = new_node_oid;
        root_p->tail = new_node_oid;
        pmemobj_persist(pop, &root_p->head, sizeof(struct node));
        pmemobj_persist(pop, &root_p->tail, sizeof(struct node));
        /*pmemobj_alloc(pop, &root_p->head, sizeof(struct node), NODE_TYPE, NULL, NULL);
        pmemobj_memcpy_persist(pop, pmemobj_direct(root_p->head), in_memory_node_ptr, sizeof(struct node));

        root_p->tail = root_p->head;*/
    }
    else {
        ((struct node *) pmemobj_direct(root_p->tail))->next = new_node_oid;
        root_p->tail = ((struct node *) pmemobj_direct(root_p->tail))->next;
        pmemobj_persist(pop, &root_p->tail, sizeof(struct node));
        /*PMEMoid tail_next = ((struct node *) pmemobj_direct(root_p->tail))->next;
        pmemobj_alloc(pop, &tail_next, sizeof(struct node), NODE_TYPE, NULL, NULL);
        pmemobj_memcpy_persist(pop, pmemobj_direct(tail_next), in_memory_node_ptr, sizeof(struct node));
        root_p->tail = tail_next;*/
    }
    /*free(in_memory_node_ptr);*/
    return 1;
}

void linkedlist_pmem_free() {
    linkedlist_pmem_check();
    PMEMoid current_node;

    while (root_p->head.off != 0) {
        //printf("in the loop\n");
        current_node = root_p->head;
        root_p->head = ((struct node *) pmemobj_direct(root_p->head))->next;
        pmemobj_free(&current_node);
    }
    //printf("end of loop\n");
    pmemobj_free(&(root_p->tail));
    root_p->head = OID_NULL;
    root_p->tail = OID_NULL;
    root_oid = OID_NULL;
    pmemobj_close(pop);
}