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
    class LinkedlistDram : public StoredsBase {
    public:
        LinkedlistDram(const char *path) {
            LinkedlistDram::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~LinkedlistDram();

    private:
        /* Private Data */
        struct ll_dram_node *head;
        struct ll_dram_node *tail;

        int check();

        struct ll_dram_node *create_node(const uint64_t key, void *value);
    };

    int LinkedlistDram::check() {
        if (head == NULL) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    // Check if this is the correct implementation for the init function for a linkedlist
    int LinkedlistDram::init(const char *path) {
        head = NULL;
        tail = NULL;
        return 1;
    }

    int LinkedlistDram::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    int LinkedlistDram::read(const uint64_t key, void *&result) {
        //printf("[%s]: key: %s\n", __FUNCTION__, key);
        check();

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_dram_node *current_node = head;

        while (current_node != NULL) {
            if (current_node->key == key) {
                result = current_node->value;
                break;
            }
            current_node = current_node->next;
        }

        return 1;
    }

    int LinkedlistDram::update(const uint64_t key, void *value) {
        //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);
        check();

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_dram_node *current_node = head;

        while (current_node != NULL) {
            if (current_node->key == key) {
                strcpy(current_node->value, (const char *) value);
                break;
            }
            current_node = current_node->next;
        }
        return 1;
    }

    inline struct ll_dram_node *LinkedlistDram::create_node(const uint64_t key, void *value) {
        struct ll_dram_node *new_node = (struct ll_dram_node *) malloc(sizeof(struct ll_dram_node));
        new_node->key = key;
        strcpy(new_node->value, (const char *) value);
        return new_node;
    }

    int LinkedlistDram::insert(const uint64_t key, void *value) {
        //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);

        struct ll_dram_node *new_node = create_node(key, value);
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

    void LinkedlistDram::destroy() {
        struct ll_dram_node *current_node;

        if(tail != NULL) free(tail);
        while (head != NULL) {
            current_node = head;
            head = head->next;
            free(current_node);
        }
    }
}   //ycsbc