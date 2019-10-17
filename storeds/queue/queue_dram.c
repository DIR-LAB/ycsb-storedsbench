#include "queue_dram.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdint.h>


const int size = 1000000;
const int default_value_len = 101;


typedef struct Node {
    uint64_t newKey;
    char newValue[default_value_len];
    struct Node *next;
    
};

struct Node* front = NULL;
struct Node* rear = NULL;



int check () {
    if (front == NULL) {
        fprintf(stderr, "malloc failed\n");
        return (-1);
    }
  
    return 1;
}

// What is the best way to initialize a data structure
int queue_dram_init(const char *path) {
    
    return 1;
}


int queue_dram_read(const char *key, void *result) {
    check();
    struct Node *current_node = front;

    while (current_node->next != NULL) {
        result = &current_node->next;
    }

    return 1;
}

// Implement this
int queue_dram_scan(const char *key, int len, void * result) {

    return 1;
}


int queue_dram_update(const char *key, void *value) {
    struct Node* current_node = front;

    current_node = front;
    while(current_node->next != NULL) {
        if(current_node->newKey = key) {
            strcpy(current_node->newKey, (const char *) value); 
        }

        current_node = current_node->next;
    }

    return 1;
}


// Should this function also take a head as a parameter 
int queue_dram_insert(const char *key, void *value) {
    check();

    struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
    temp->newKey = key;
    strcpy(temp->newKey,(const char *) value);

    if(front == NULL && rear == NULL) {
        front = rear = temp;
    }

    rear->next = temp;
    rear = temp;

    return 1;
}




void queue_dram_free() {
    struct Node *current_node = front;

    while (front != NULL) {
        current_node = front;
        front = front->next;
        free(current_node);
    }
}