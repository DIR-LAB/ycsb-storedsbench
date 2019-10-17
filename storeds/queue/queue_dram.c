#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "queue_dram.h"

const int size = 1000000;
const int default_value_len = 101;

//todo: need to fix what queue.read(), queue.insert(), queue.update() function should do???

struct node {
    uint64_t key;
    char value[default_value_len];
    struct node *next;
};

struct node *front = NULL;
struct node *rear = NULL;

int queue_dram_check() {
    if (front == NULL) {
        fprintf(stderr, "[%s]: FATAL: queue not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

// What is the best way to initialize a data structure
int queue_dram_init(const char *path) {
    //do nothing
    return 1;
}

int queue_dram_read(const char *key, void *result) {
    queue_dram_check();
    struct node *current_node = front;
    uint64_t uint64_key = strtoull(key, NULL, 0);

    while (current_node->next != NULL) {
        if (current_node->key == uint64_key) {
            result = current_node->value;
            break;
        }
        current_node = current_node->next;
    }

    return 1;
}

// Implement this
int queue_dram_scan(const char *key, int len, void *result) {
    return 1;
}

int queue_dram_update(const char *key, void *value) {
    struct node *current_node = front;
    uint64_t uint64_key = strtoull(key, NULL, 0);

    current_node = front;
    while (current_node->next != NULL) {
        if (current_node->key == uint64_key) {
            strcpy(current_node->value, (const char *) value);
        }

        current_node = current_node->next;
    }

    return 1;
}

// Should this function also take a head as a parameter 
int queue_dram_insert(const char *key, void *value) {
    queue_dram_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);

    struct node *temp = (struct node *) malloc(sizeof(struct node));
    temp->key = uint64_key;
    strcpy(temp->value, (const char *) value);

    if (front == NULL && rear == NULL) {
        front = rear = temp;
    }

    rear->next = temp;
    rear = temp;

    return 1;
}

void queue_dram_free() {
    struct node *current_node = front;

    while (front != NULL) {
        current_node = front;
        front = front->next;
        free(current_node);
    }
}