//
// Created by aislam6 on 10/14/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "linkedlist_dram.h"

const int size = 1000000;
const int default_value_len = 101;


typedef struct Node{
    uint64_t newKey;
    char newValue[default_value_len];
    struct Node *next;
};

struct Node *head = NULL;
struct Node *tail = NULL;



int check () {
    if (head == NULL) {
        fprintf(stderr, "malloc failed\n");
        return (-1);
    }
    return 1;
}

// Check if this is the correct implementation for the init function for a linkedlist
int linkedlist_dram_init(const char *path) {
    if (head == NULL) {
        fprintf(stderr, "malloc failed\n");
    }

    for (int i = 0; i < size; i++) {
        *head->newValue = (char *) malloc(default_value_len * sizeof(char *));
    }
    return 1;
}

int linkedlist_dram_read(const char *key, void *result) {
    check();
    struct Node *current_node = head;
    
    while(current_node != Ngi tULL) {
        result = &current_node->next;
    }


    return 1;
}

int linkedlist_dram_update(const char *key, void *value) {
    check();
    struct Node* current_node = head;

    current_node = head;
    while(current_node->next != NULL) {
        if(current_node->newKey = key) {
            strcpy(current_node->newKey, (const char *) value); 
        }

        current_node = current_node->next;
    }

    return 1;
}

int linkedlist_dram_insert(const char *key, void *value) {
    check();
    struct Node* node = (struct Node*) malloc(sizeof(node));
    node->newKey = key;
    strcpy(node->newKey,(const char *) value);

    if (head == NULL) {
       tail = node;
       head = tail;
    }

    while (tail->next != NULL) {
        tail = tail->next;
    } 
    tail->next = node;

    return 1;
}

void linkedlist_dram_free() {
    struct Node *current_node = head;

    while (head != NULL) {
        current_node = head;
        head = head->next;
        free(current_node);
    }
}