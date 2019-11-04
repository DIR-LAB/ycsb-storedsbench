//
// Created by aislam6 on 10/14/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "linkedlist_dram.h"

const int size = 1000000;
const int default_value_len = 101;

struct node {
    uint64_t key;
    char value[default_value_len];
    struct node *next;
};

static struct node *head = NULL;
static struct node *tail = NULL;

int linkedlist_dram_check() {
    if (head == NULL) {
        fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

// Check if this is the correct implementation for the init function for a linkedlist
int linkedlist_dram_init(const char *path) {
    head = NULL;
    tail = NULL;
    return 1;
}

int linkedlist_dram_read(const char *key, void *&result) {
    //printf("[%s]: key: %s\n", __FUNCTION__, key);
    linkedlist_dram_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = head;

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            result = current_node->value;
            break;
        }
        current_node = current_node->next;
    }

    return 1;
}

int linkedlist_dram_update(const char *key, void *value) {
    //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);
    linkedlist_dram_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = head;

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            strcpy(current_node->value, (const char *) value);
            break;
        }
        current_node = current_node->next;
    }
    return 1;
}

int linkedlist_dram_insert(const char *key, void *value) {
    //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);
    //linkedlist_dram_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *new_node = (struct node *) malloc(sizeof(struct node));
    new_node->key = uint64_key;
    strcpy(new_node->value, (const char *) value);

    if(head == NULL) {
        head = new_node;
        tail = new_node;
    }
    else {
        tail->next = new_node;
        tail = tail->next;
    }

    return 1;
}

void linkedlist_dram_free() {
    struct node *current_node;

    if(tail != NULL) free(tail);
    while (head != NULL) {
        current_node = head;
        head = head->next;
        free(current_node);
    }
}