//
// Created by aislam6 on 10/14/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "linkedlist_common.h"

namespace ycsbc {
    class LinkedlistDramConcurrentMLock : public StoredsBase {
    public:
        LinkedlistDramConcurrentMLock(const char *path) {
            LinkedlistDramConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~LinkedlistDramConcurrentMLock();

    private:
        /* Private Data */
        struct ll_dram_node *head;
        struct ll_dram_node *tail;
        pthread_mutex_t mutex_lock_;

        int check();

        struct ll_dram_node *create_node(const uint64_t key, void *value);
    };

    int LinkedlistDramConcurrentMLock::check() {
        if (head == NULL) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    // Check if this is the correct implementation for the init function for a linkedlist
    int LinkedlistDramConcurrentMLock::init(const char *path) {
        head = NULL;
        tail = NULL;
        if(pthread_mutex_init(&mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    int LinkedlistDramConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    int LinkedlistDramConcurrentMLock::read(const uint64_t key, void *&result) {
        //printf("[%s]: key: %s\n", __FUNCTION__, key);
        check();

        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;
        struct ll_dram_node *current_node = head;
        while (current_node != NULL) {
            if (current_node->key == key) {
                result = current_node->value;
                break;
            }
            current_node = current_node->next;
        }
        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    int LinkedlistDramConcurrentMLock::update(const uint64_t key, void *value) {
        //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);
        check();

        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;
        struct ll_dram_node *current_node = head;
        while (current_node != NULL) {
            if (current_node->key == key) {
                strcpy(current_node->value, (const char *) value);
                break;
            }
            current_node = current_node->next;
        }
        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    inline struct ll_dram_node *LinkedlistDramConcurrentMLock::create_node(const uint64_t key, void *value) {
        struct ll_dram_node *new_node = (struct ll_dram_node *) malloc(sizeof(struct ll_dram_node));
        new_node->key = key;
        strcpy(new_node->value, (const char *) value);
        return new_node;
    }

    int LinkedlistDramConcurrentMLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);

        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;
        struct ll_dram_node *new_node = create_node(key, value);

        if(head == NULL) {
            head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = tail->next;
        }
        pthread_mutex_unlock(&mutex_lock_);

        return 1;
    }

    void LinkedlistDramConcurrentMLock::destroy() {
        struct ll_dram_node *current_node;

        if(tail != NULL) {
            free(tail);
            tail = NULL;
        }
        while (head != NULL) {
            current_node = head;
            head = head->next;
            free(current_node);
            current_node = NULL;
        }
        pthread_mutex_destroy(&mutex_lock_);
    }
}   //ycsbc