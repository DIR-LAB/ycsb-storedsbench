//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "hl_common.h"
#include "../linkedlist/linkedlist_common.h"

namespace ycsbc {
    class HlVmemConcurrentPMLock : public StoredsBase {
    public:
        HlVmemConcurrentPMLock(const char *path) {
            HlVmemConcurrentPMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~HlVmemConcurrentPMLock();

    private:
        /* Private Data */
        VMEM *vmp;
        struct hashlist_vmem_pmlock *pmem_lock_p = NULL;
        struct dram_hashlist *root_p;
        struct ll_dram_node *head;
        struct ll_dram_node *tail;

        int check();

        uint64_t hash_function(uint64_t value);

        struct hl_dram_entry* new_hash_entry(uint64_t key, void *_ll_p);

        struct ll_dram_node* create_ll_node(const uint64_t key, void *value);

        void free_hl_entry_chain(struct hl_dram_entry *head);
    };

    /*
     * check -- checks if hash-list has been initialized
     */
    inline int HlVmemConcurrentPMLock::check() {
        if (root_p == NULL || root_p->buckets == NULL) {
            fprintf(stderr, "[%s]: FATAL: hash-list not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * hash_function_dram -- the simplest hashing function,
     * see https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
     */
    uint64_t HlVmemConcurrentPMLock::hash_function(uint64_t value) {
        uint32_t a = root_p->hash_fun_coeff_a;
        uint32_t b = root_p->hash_fun_coeff_b;
        uint64_t p = root_p->hash_fun_coeff_p;
        size_t len = root_p->buckets->nbuckets;

        return ((value * a + b) % p) % len;
    }

    /*
     * init -- hash-list initializer
     */
    int HlVmemConcurrentPMLock::init(const char *path) {
        //initializing hash-list
        size_t len = INIT_BUCKETS_NUM;

        root_p = (struct dram_hashlist *) malloc(sizeof(struct dram_hashlist));
        root_p->seed = (uint32_t)time(NULL);
        do {
            root_p->hash_fun_coeff_a = (uint32_t) rand();
        } while(root_p->hash_fun_coeff_a == 0);

        root_p->hash_fun_coeff_b = (uint32_t) rand();
        root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;
        root_p->buckets = (struct hl_dram_buckets *) malloc(sizeof(struct hl_dram_buckets));
        root_p->buckets->nbuckets = len;
        root_p->buckets->bucket = (struct hl_dram_entry **) calloc(len, sizeof(struct hl_dram_entry));

        //initializing linked-list
        head = NULL;
        tail = NULL;

        if ((vmp = vmem_create(path, PMEM_BP_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }

        //initializing lock
        if ((pmem_lock_p = (struct hashlist_vmem_pmlock *) vmem_malloc(vmp, sizeof(struct hashlist_vmem_pmlock))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        if(pthread_mutex_init(&pmem_lock_p->mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }

        return 1;
    }

    int HlVmemConcurrentPMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * read -- read 'value' of 'key' and place it into '&result'
     */
    int HlVmemConcurrentPMLock::read(const uint64_t key, void *&result) {
        check();

        if (pthread_mutex_lock(&pmem_lock_p->mutex_lock_) != 0) return 0;
        struct hl_dram_buckets *buckets_p = root_p->buckets;
        uint64_t hash_value = hash_function(key);

        for(struct hl_dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found! put it to result and return
                result = ((ll_dram_node *) entry_p->ll_p)->value;
                break;
            }
        }

        pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
        return 1;
    }

    /**
     * update -- update 'value' of 'key' into the hash-list, will insert the 'value' if 'key' not exists
     */
    int HlVmemConcurrentPMLock::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * new_hash_entry -- create new hash-list entry
     */
    struct hl_dram_entry* HlVmemConcurrentPMLock::new_hash_entry(uint64_t key, void *_ll_p) {
        struct hl_dram_entry *entry_p = (struct hl_dram_entry *) malloc(sizeof(struct hl_dram_entry));
        entry_p->key = key;
        entry_p->ll_p = _ll_p;
        entry_p->next = (struct hl_dram_entry *) malloc(sizeof(struct hl_dram_entry));
        return entry_p;
    }

    inline struct ll_dram_node *HlVmemConcurrentPMLock::create_ll_node(const uint64_t key, void *value) {
        struct ll_dram_node *new_node;
        if ((new_node = ((struct ll_dram_node *) vmem_malloc(vmp, sizeof(struct ll_dram_node)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        new_node->key = key;
        strcpy(new_node->value, (const char *) value);
        return new_node;
    }

    /**
     * insert -- inserts 'value' into the hash-list, will update the 'value' if 'key' already exists
     */
    int HlVmemConcurrentPMLock::insert(const uint64_t key, void *value) {
        check();

        if (pthread_mutex_lock(&pmem_lock_p->mutex_lock_) != 0) return 0;
        //first search in the hash-list
        struct hl_dram_buckets *buckets_p = root_p->buckets;
        uint64_t hash_value = hash_function(key);

        for(struct hl_dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found in the hash-list! replace the value in the linked-list and return
                memcpy(((ll_dram_node *) entry_p->ll_p)->value, (char *) value, strlen((char *) value) + 1);

                pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
                return 1;
            }
        }

        //key not found!
        //entry to linked-list
        struct ll_dram_node *new_node = create_ll_node(key, value);
        if(head == NULL) {
            head = new_node;
            tail = new_node;
        }
        else {
            tail->next = new_node;
            tail = tail->next;
        }

        //entry to hash-list
        struct hl_dram_entry *entry_p = new_hash_entry(key, new_node);
        entry_p->next = buckets_p->bucket[hash_value];
        buckets_p->bucket[hash_value] = entry_p;

        pthread_mutex_unlock(&pmem_lock_p->mutex_lock_);
        return 1;
    }

    /**
     * free_entry_chain -- free memory allocated in a hash-list bucket entry chain
     */
    void HlVmemConcurrentPMLock::free_hl_entry_chain(struct hl_dram_entry *head) {
        struct hl_dram_entry* entry_p;
        while(head != NULL) {
            entry_p = head;
            head = head->next;
            free(entry_p);
        }
    }

    /**
     * free -- destructor
     */
    void HlVmemConcurrentPMLock::destroy() {
        check();

        //free the hashlist
        for (int i=0; i < INIT_BUCKETS_NUM; i+=1) {
            struct hl_dram_entry *entry_p = root_p->buckets->bucket[i];
            if (entry_p != NULL) {
                free_hl_entry_chain(entry_p);
            }
        }
        free(root_p->buckets->bucket);
        free(root_p->buckets);
        free(root_p);

        //free the linkedlist
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
        
        pthread_mutex_destroy(&pmem_lock_p->mutex_lock_);
        vmem_free(vmp, pmem_lock_p);
        pmem_lock_p = NULL;

        vmem_delete(vmp);
    }
}   //ycsbc
