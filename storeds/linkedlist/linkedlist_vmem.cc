//
// Created by aislam6 on 10/14/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <libvmem.h>

#include "linkedlist_common.h"

namespace ycsbc {
    class LinkedlistVmem : public StoredsBase {
    public:
        LinkedlistVmem(const char *path) {
            LinkedlistVmem::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~LinkedlistVmem();

    private:
        /* Private Data */
        VMEM *vmp;
        struct ll_dram_node *head;
        struct ll_dram_node *tail;

        int check();

        struct ll_dram_node *create_node(const uint64_t key, void *value);
    };

    int LinkedlistVmem::check() {
        if (head == NULL) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    // Check if this is the correct implementation for the init function for a linkedlist
    int LinkedlistVmem::init(const char *path) {
        head = NULL;
        tail = NULL;
        if ((vmp = vmem_create(path, PMEM_LL_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }
        return 1;
    }

    int LinkedlistVmem::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    int LinkedlistVmem::read(const uint64_t key, void *&result) {
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

    int LinkedlistVmem::update(const uint64_t key, void *value) {
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

    inline struct ll_dram_node *LinkedlistVmem::create_node(const uint64_t key, void *value) {
        struct ll_dram_node *new_node;
        if ((new_node = ((struct ll_dram_node *) vmem_malloc(vmp, sizeof(struct ll_dram_node)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        new_node->key = key;
        strcpy(new_node->value, (const char *) value);
        return new_node;
    }

    int LinkedlistVmem::insert(const uint64_t key, void *value) {
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

    void LinkedlistVmem::destroy() {
        struct ll_dram_node *current_node;

        if(tail != NULL) {
            vmem_free(vmp, tail);
            tail = NULL;
        }
        while (head != NULL) {
            current_node = head;
            head = head->next;
            vmem_free(vmp, current_node);
            current_node = NULL;
        }
        vmem_delete(vmp);
    }
}   //ycsbc