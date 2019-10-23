//
// Created by aislam6 on 10/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../ex_common.h"
#include "skiplist_dram.h"

/*  
 * Data Structure Section
 */

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* number of levels in skiplist */
#define SKIPLIST_LEVELS_NUM 4

/* base level of skiplist */
#define SKIPLIST_BASE_LEVEL 0

/* declaration of data-structures */
struct sk_entry {
	uint64_t key;
	char value[DEFAULT_VALUE_LEN];
};

struct sk_node {
	/* skiplist entry */
	struct sk_entry entry;

	/* next entry pointer */
	struct sk_node *next[SKIPLIST_LEVELS_NUM];
};

static struct sk_node *head = NULL;

int skiplist_dram_init(const char *path) {
    head = (struct sk_node *) malloc(sizeof(struct sk_node));
    return 1;
}

void skiplist_dram_find(uint64_t uint64_key, struct sk_node **path) {
    struct sk_node *current_node = head;
    for(int current_level = SKIPLIST_LEVELS_NUM - 1; current_level >= 0; current_level -= 1) {
        current_node = current_node->next[current_level];
        while(current_node != NULL && current_node->entry.key < uint64_key) {
            current_node = current_node->next[current_level];
        }
        //printf("current_level[%d] => node <%lu, %s>\n", current_level, current_node->entry.key, current_node->entry.value);
        path[current_level] = current_node;
    }
}

int skiplist_dram_read(const char *key, void *result) {
    //todo: add checker method

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct sk_node *path[SKIPLIST_LEVELS_NUM];
    skiplist_dram_find(uint64_key, path);
    if(path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL] != NULL && path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL]->entry.key == uint64_key) {
        result = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL]->entry.value;
    }
    return 1;
}

int skiplist_dram_update(const char *key, void *value) {
    skiplist_dram_insert(key, value);
    return 1;
}

int skiplist_dram_insert(const char *key, void *value) {
    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct sk_node *path[SKIPLIST_LEVELS_NUM];

    skiplist_dram_find(uint64_key, path);

    if(path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL] != NULL && path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL]->entry.key == uint64_key) {
        //key already exist, update value
        strcpy(path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL]->entry.value, (char *) value);
    } else {
        //key not exist, insert the new node

        struct sk_node *new_node = (struct sk_node *) malloc(sizeof(struct sk_node));
        new_node->entry.key = uint64_key;
        strcpy(new_node->entry.value, (char *) value);
    }
    return 1;
}

void skiplist_dram_free() {
    //
}