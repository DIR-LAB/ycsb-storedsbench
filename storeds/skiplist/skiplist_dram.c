//
// Created by aislam6 on 10/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "../ex_common.h"
#include "skiplist_dram.h"

/*  
 * Data Structure Section
 */

/* default length for value */
#define DEFAULT_VALUE_LEN 101

/* number of levels in skiplist */
#define SKIPLIST_LEVELS_NUM 16

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

/*
 * skiplist_dram_check -- (internal) checks if skiplist is initialized or not
 */
int skiplist_dram_check() {
	if (head == NULL) {
        fprintf(stderr, "[%s]: FATAL: skiplist not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/**
 * skiplist_dram_init -- initialize skiplist
 */
int skiplist_dram_init(const char *path) {
    srand((uint32_t) time(NULL));
    head = (struct sk_node *) malloc(sizeof(struct sk_node));
    for(int i=0; i<SKIPLIST_LEVELS_NUM; i+=1) {
        head->next[i] = NULL;
    }
    skiplist_dram_check();
    return 1;
}

/**
 * skiplist_dram_print -- (internal) level-wise print all the <key-value> pairs stored in skiplist
 */
void skiplist_dram_print() {
    int current_level = SKIPLIST_LEVELS_NUM - 1;

    while(current_level >= SKIPLIST_BASE_LEVEL) {
        struct sk_node *current_node = head->next[current_level];
        printf("level[%d]", current_level);
        while(current_node != NULL) {
            printf(" <%lu, %s> ->", current_node->entry.key, current_node->entry.value);
            current_node = current_node->next[current_level];
        }
        printf("<END>\n");
        current_level -= 1;
    }
}

/**
 * skiplist_dram_find -- (internal) returns path to searched node
 * or if node doesn't exist, it will return path to place where key should be.
 */
void skiplist_dram_find(uint64_t uint64_key, struct sk_node **path) {
    //printf("[%s] called in\n", __func__);
    struct sk_node *active_node = head;
    for(int current_level = SKIPLIST_LEVELS_NUM - 1; current_level >= 0; current_level -= 1) {
        struct sk_node *current_node = active_node->next[current_level];
        while(current_node != NULL && current_node->entry.key < uint64_key) {
            active_node = current_node;
            current_node = active_node->next[current_level];
        }
        path[current_level] = active_node;
    }
}

/**
 * skiplist_dram_read -- read value of key and sets into result
 */
int skiplist_dram_read(const char *key, void *result) {
    skiplist_dram_check();
    //printf("[%s]: PARAM: key: %s\n", __func__, key);

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct sk_node *path[SKIPLIST_LEVELS_NUM], *possible_found;
    skiplist_dram_find(uint64_key, path);
    possible_found = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];
    if(possible_found != NULL && possible_found->entry.key == uint64_key) {
        result = possible_found->entry.value;
    }
    //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) result);
    return 1;
}

/**
 * skiplist_dram_update -- if key exist, update the value part in skiplist
 * if key doesn't exist, insert new key-value pair into skiplist
 */
int skiplist_dram_update(const char *key, void *value) {
    skiplist_dram_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);

    skiplist_dram_insert(key, value);
    return 1;
}

/**
 * skiplist_dram_insert_node -- (internal) insert new node in selected places
 */
void skiplist_dram_insert_node(struct sk_node *new_node, struct sk_node *path[SKIPLIST_LEVELS_NUM]) {
    int current_level = SKIPLIST_BASE_LEVEL;

    // certainly assign node to level 0, and randomly for other levels
    do {
        new_node->next[current_level] = path[current_level]->next[current_level];
        path[current_level]->next[current_level] = new_node;
    } while(++current_level < SKIPLIST_LEVELS_NUM && !(rand() % 2));
}

/**
 * skiplist_dram_insert -- insert new key-value pair into skiplist
 * if key already exist, update the value part in skiplist
 */
int skiplist_dram_insert(const char *key, void *value) {
    skiplist_dram_check();
    //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);
    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct sk_node *path[SKIPLIST_LEVELS_NUM], *possible_found;

    skiplist_dram_find(uint64_key, path);
    possible_found = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];

    if(possible_found != NULL && possible_found->entry.key == uint64_key) {
        //key already exist, update value
        strcpy(possible_found->entry.value, (char *) value);
    }
    else {
        //key not exist, insert the new node
        struct sk_node *new_node = (struct sk_node *) malloc(sizeof(struct sk_node));
        new_node->entry.key = uint64_key;
        strcpy(new_node->entry.value, (char *) value);
        skiplist_dram_insert_node(new_node, path);
    }
    return 1;
}

/**
 * skiplist_dram_free -- deallocate memory of skiplist
 */
void skiplist_dram_free() {
    skiplist_dram_check();
    int current_level = SKIPLIST_LEVELS_NUM - 1;
    
    while(current_level >= SKIPLIST_BASE_LEVEL) {
        struct sk_node *current_node = head->next[current_level];
        printf("level[%d]\n", current_level);
        while(current_node != NULL) {
            struct sk_node *tmp = current_node;
            current_node = current_node->next[current_level];
            free(tmp);
            tmp = NULL;
        }
        current_level -= 1;
    }
    free(head);
    head = NULL;
}