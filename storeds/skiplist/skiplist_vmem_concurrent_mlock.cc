//
// Created by aislam6 on 10/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "skiplist_common.h"

namespace ycsbc {
    class SkiplistVmemConcurrentMLock : public StoredsBase {
    public:
        SkiplistVmemConcurrentMLock(const char *path) {
            SkiplistVmemConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~SkiplistVmemConcurrentMLock();

    private:
        /* Private Data */
        VMEM *vmp;
        pthread_mutex_t mutex_lock_;
        struct sk_dram_node *head = NULL;

        int check();

        void print();

        void find(uint64_t uint64_key, struct sk_dram_node **path);

        void insert_node(struct sk_dram_node *new_node, struct sk_dram_node *path[SKIPLIST_LEVELS_NUM]);

        void remove_node(struct sk_dram_node *path[SKIPLIST_LEVELS_NUM]);

        void remove_free(uint64_t key);
    };

    /*
     * check -- (internal) checks if skiplist is initialized or not
     */
    int SkiplistVmemConcurrentMLock::check() {
        if (head == NULL) {
            fprintf(stderr, "[%s]: FATAL: skiplist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * init -- initialize skiplist
     */
    int SkiplistVmemConcurrentMLock::init(const char *path) {
        srand((uint32_t) time(NULL));
        if ((vmp = vmem_create(path, PMEM_SKIPLIST_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }

        if(pthread_mutex_init(&mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }

        if ((head = ((struct sk_dram_node *) vmem_malloc(vmp, sizeof(struct sk_dram_node)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        for(int i=0; i<SKIPLIST_LEVELS_NUM; i+=1) {
            head->next[i] = NULL;
        }
        check();
        return 1;
    }

    /**
     * print -- (internal) level-wise print all the <key-value> pairs stored in skiplist
     */
    void SkiplistVmemConcurrentMLock::print() {
        int current_level = SKIPLIST_LEVELS_NUM - 1;

        while(current_level >= SKIPLIST_BASE_LEVEL) {
            struct sk_dram_node *current_node = head->next[current_level];
            printf("level[%d]", current_level);
            while(current_node != NULL) {
                printf(" <%lu, %s> ->", current_node->entry.key, current_node->entry.value);
                current_node = current_node->next[current_level];
            }
            printf("<END>\n");
            current_level -= 1;
        }
    }

    int SkiplistVmemConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        struct sk_dram_node *path[SKIPLIST_LEVELS_NUM], *possible_found;
        find(key, path);
        possible_found = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];
        int len_count = 0;
        while(possible_found != NULL) {
            std::vector <DB::Kuint64VstrPair> tmp;
            tmp.push_back(std::make_pair(possible_found->entry.key, possible_found->entry.value));
            result.push_back(tmp);

            len_count += 1;
            if(len_count == len) break;
            possible_found = possible_found->next[SKIPLIST_BASE_LEVEL];
        }

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * find -- (internal) returns path to searched node
     * or if node doesn't exist, it will return path to place where key should be.
     */
    void SkiplistVmemConcurrentMLock::find(uint64_t uint64_key, struct sk_dram_node **path) {
        //printf("[%s] called in\n", __func__);
        struct sk_dram_node *active_node = head;
        for(int current_level = SKIPLIST_LEVELS_NUM - 1; current_level >= 0; current_level -= 1) {
            struct sk_dram_node *current_node = active_node->next[current_level];
            while(current_node != NULL && current_node->entry.key < uint64_key) {
                active_node = current_node;
                current_node = active_node->next[current_level];
            }
            path[current_level] = active_node;
        }
    }

    /**
     * read -- read value of key and sets into result
     */
    int SkiplistVmemConcurrentMLock::read(const uint64_t key, void *&result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        struct sk_dram_node *path[SKIPLIST_LEVELS_NUM], *possible_found;
        find(key, path);
        possible_found = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];
        if(possible_found != NULL && possible_found->entry.key == key) {
            result = possible_found->entry.value;
        }

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * update -- if key exist, update the value part in skiplist
     * if key doesn't exist, insert new key-value pair into skiplist
     */
    int SkiplistVmemConcurrentMLock::update(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);
        return insert(key, value);
    }

    /**
     * insert_node -- (internal) insert new node in selected places
     */
    void SkiplistVmemConcurrentMLock::insert_node(struct sk_dram_node *new_node, struct sk_dram_node *path[SKIPLIST_LEVELS_NUM]) {
        int current_level = SKIPLIST_BASE_LEVEL;

        // certainly assign node to level 0, and randomly for other levels
        do {
            new_node->next[current_level] = path[current_level]->next[current_level];
            path[current_level]->next[current_level] = new_node;
        } while(++current_level < SKIPLIST_LEVELS_NUM && !(rand() % 2));
    }

    /**
     * insert -- insert new key-value pair into skiplist
     * if key already exist, update the value part in skiplist
     */
    int SkiplistVmemConcurrentMLock::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %ld, value: %s\n\n", __func__, key, (char *) value);
        check();

        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;
        struct sk_dram_node *path[SKIPLIST_LEVELS_NUM], *possible_found;

        find(key, path);
        possible_found = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];

        if(possible_found != NULL && possible_found->entry.key == key) {
            //key already exist, update value
            strcpy(possible_found->entry.value, (char *) value);
        }
        else {
            //key not exist, insert the new node
            struct sk_dram_node *new_node;
            if ((new_node = ((struct sk_dram_node *) vmem_malloc(vmp, sizeof(struct sk_dram_node)))) == NULL) {
                fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
                exit(1);
            }
            new_node->entry.key = key;
            strcpy(new_node->entry.value, (char *) value);
            insert_node(new_node, path);
        }

        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /*
     * remove_node -- (internal) removes selected node
     */
    void SkiplistVmemConcurrentMLock::remove_node(struct sk_dram_node *path[SKIPLIST_LEVELS_NUM]) {
        struct sk_dram_node *possible_remove = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];
        for(int i=SKIPLIST_BASE_LEVEL; i<SKIPLIST_LEVELS_NUM; i+=1) {
            if(path[i]->next[i] != NULL && (path[i]->next[i]->entry.key == possible_remove->entry.key)) {
                //struct sk_dram_node *tmp = path[i]->next[i];
                path[i]->next[i] = possible_remove->next[i];
                //free(tmp);
            }
        }
        vmem_free(vmp, possible_remove);
        //todo: need careful analysis if this memory de-allocation is the right way or not!
    }

    /*
     * remove_free -- (internal) removes key-value pair from the list
     */
    void SkiplistVmemConcurrentMLock::remove_free(uint64_t key) {
        struct sk_dram_node *path[SKIPLIST_LEVELS_NUM], *possible_remove;
        find(key, path);
        possible_remove = path[SKIPLIST_BASE_LEVEL]->next[SKIPLIST_BASE_LEVEL];
        if(possible_remove != NULL && possible_remove->entry.key == key) {
            remove_node(path);
        }
    }

    /**
     * destroy -- deallocate memory of skiplist
     */
    void SkiplistVmemConcurrentMLock::destroy() {
        check();

        while(head->next[0] != NULL) {
            struct sk_dram_node *next = head->next[0];
            remove_free(next->entry.key);
        }
        vmem_free(vmp, head);
        head = NULL;
        pthread_mutex_destroy(&mutex_lock_);
        vmem_delete(vmp);
    }
}   //ycsbc