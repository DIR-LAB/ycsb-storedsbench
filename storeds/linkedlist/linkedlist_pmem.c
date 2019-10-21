//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "linkedlist_pmem.h"

/* size of the pmem object pool */
#define PM_ARRAY_POOL_SIZE (160 * 1024 * 1024)

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
}

struct node {
    uint64_t key;
    char value[default_value_len];
    PMEMoid next;
};

int linkedlist_pmem_check() {
    if (root_p->head == NULL) {
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
    return 1;
}

int linkedlist_pmem_read(const char *key, void *result) {
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
    return 1;
}

int linkedlist_pmem_update(const char *key, void *value) {
    linkedlist_pmem_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = (struct node *) pmemobj_direct(root_p->head);

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            strcpy(current_node->value, (const char *) value);
            break;
        }
        current_node = (struct node *) pmemobj_direct(current_node->next);
    }
    return 1;
}

int linkedlist_pmem_insert(const char *key, void *value) {
    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *new_node = (struct node *) malloc(sizeof(struct node));
    new_node->key = uint64_key;
    strcpy(new_node->value, (const char *) value);

    if(root_p->head == OID_NULL) {
        root_p->head = new_node;
        root_p->tail = new_node;
    }
    else {
        root_p->tail->next = new_node;
        root_p->tail = root_p->tail->next;
    }
    free(new_node);
    return 1;
}

void linkedlist_pmem_free() {
    struct node *head_p = pmemobj_direct(root_p->head);
    struct node *current_node;

    while (head_p != NULL) {
        current_node = head;
        head = head->next;
        free(current_node);
    }
    free(root_p->head);
    free(root_p->tail);
}