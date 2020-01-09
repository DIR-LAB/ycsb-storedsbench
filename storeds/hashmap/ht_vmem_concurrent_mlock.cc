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
#include <libvmem.h>
#include "ht_common.h"

namespace ycsbc {
    class HtVmemConcurrentMLock : public StoredsBase {
    public:
        HtVmemConcurrentMLock(const char *path) {
            HtVmemConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~HtVmemConcurrentMLock();

    private:
        /* Private Data */
        VMEM *vmp;
        pthread_mutex_t mutex_lock_;
        struct dram_hashtable *root_p;

        int check();

        void print();

        uint64_t hash_function(uint64_t value);

        int reinit(size_t new_len);

        struct dram_entry* new_entry(uint64_t key, const char* value);

        void free_entry_chain(struct dram_entry *head);
    };

    /*
     * check -- checks if hashtable has been initialized
     */
    inline int HtVmemConcurrentMLock::check() {
        if (root_p == NULL || root_p->buckets == NULL) {
            fprintf(stderr, "[%s]: FATAL: hashtable not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
    * print -- prints complete hashtable state
    */
    void HtVmemConcurrentMLock::print() {
        srand((uint32_t) time(NULL));
        check();

        struct dram_buckets *buckets_p = root_p->buckets;

        printf("a: %d, b: %d, p: %lu\n", root_p->hash_fun_coeff_a, root_p->hash_fun_coeff_b, root_p->hash_fun_coeff_p);
        printf("total buckets: %lu\n", buckets_p->nbuckets);

        for (size_t i = 0; i < buckets_p->nbuckets; ++i) {
            if (buckets_p->bucket[i] == NULL) continue;

            int num = 0;
            printf("bucket #%lu:", i);
            for (struct dram_entry *entry_p = buckets_p->bucket[i]; entry_p != NULL; entry_p = entry_p->next) {
                printf(" <%lu, %s>", entry_p->key, entry_p->value);
                num++;
            }
            printf(" (%d)\n", num);
        }
    }

    /*
     * hash_function_dram -- the simplest hashing function,
     * see https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
     */
    uint64_t HtVmemConcurrentMLock::hash_function(uint64_t value) {
        uint32_t a = root_p->hash_fun_coeff_a;
        uint32_t b = root_p->hash_fun_coeff_b;
        uint64_t p = root_p->hash_fun_coeff_p;
        size_t len = root_p->buckets->nbuckets;

        return ((value * a + b) % p) % len;
    }

    /*
     * reinit -- reinitialize the hashtable with a new number of buckets, not implemented yet
     */
    int HtVmemConcurrentMLock::reinit(size_t new_len) {
        return 1;
    }

    /*
     * init -- hashtable initializer
     */
    int HtVmemConcurrentMLock::init(const char *path) {
        if ((vmp = vmem_create(path, PM_HASHTABLE_POOL_SIZE)) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_create failed\n", __FUNCTION__);
            exit(1);
        }

        if(pthread_mutex_init(&mutex_lock_, NULL) != 0) {
            fprintf(stderr, "[%s]: FATAL: Mutex-Lock failed to initialize\n", __FUNCTION__);
            assert(0);
        }

        size_t len = INIT_BUCKETS_NUM;
        if ((root_p = ((struct dram_hashtable *) vmem_malloc(vmp, sizeof(struct dram_hashtable)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        root_p->seed = (uint32_t)time(NULL);
        do {
            root_p->hash_fun_coeff_a = (uint32_t) rand();
        } while(root_p->hash_fun_coeff_a == 0);

        root_p->hash_fun_coeff_b = (uint32_t) rand();
        root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;
        if ((root_p->buckets = ((struct dram_buckets *) vmem_malloc(vmp, sizeof(struct dram_buckets)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        root_p->buckets->nbuckets = len;
        if ((root_p->buckets->bucket = ((struct dram_entry **) vmem_calloc(vmp, len, sizeof(struct dram_entry)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }

        return 1;
    }

    int HtVmemConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * read -- read 'value' of 'key' and place it into '&result'
     */
    int HtVmemConcurrentMLock::read(const uint64_t key, void *&result) {
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        struct dram_buckets *buckets_p = root_p->buckets;
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        uint64_t hash_value = hash_function(key);

        for(struct dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found! put it to result and return
                result = entry_p->value;
                break;
            }
        }
        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * update -- update 'value' of 'key' into the hashtable, will insert the 'value' if 'key' not exists
     */
    int HtVmemConcurrentMLock::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * new_entry -- create new hashtable entry
     */
    struct dram_entry* HtVmemConcurrentMLock::new_entry(uint64_t key, const char* value) {
        struct dram_entry *entry_p;
        if ((entry_p = ((struct dram_entry *) vmem_malloc(vmp, sizeof(struct dram_entry)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        entry_p->key = key;
        memcpy(entry_p->value, (char *) value, strlen((char *) value) + 1);
        if ((entry_p->next = ((struct dram_entry *) vmem_malloc(vmp, sizeof(struct dram_entry)))) == NULL) {
            fprintf(stderr, "[%s]: FATAL: vmem_malloc failed\n", __FUNCTION__);
            exit(1);
        }
        return entry_p;
    }

    /**
     * insert -- inserts 'value' into the hashtable, will update the 'value' if 'key' already exists
     */
    int HtVmemConcurrentMLock::insert(const uint64_t key, void *value) {
        check();
        if (pthread_mutex_lock(&mutex_lock_) != 0) return 0;

        struct dram_buckets *buckets_p = root_p->buckets;
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        uint64_t hash_value = hash_function(key);

        for(struct dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found! replace the value and return
                memcpy(entry_p->value, (char *) value, strlen((char *) value) + 1);
                pthread_mutex_unlock(&mutex_lock_);
                return 1;
            }
        }

        //key not found! need to insert data into bucket[hash_value]
        struct dram_entry *entry_p = new_entry(key, (const char*) value);
        entry_p->next = buckets_p->bucket[hash_value];
        buckets_p->bucket[hash_value] = entry_p;
        pthread_mutex_unlock(&mutex_lock_);
        return 1;
    }

    /**
     * free_entry_chain -- free memory allocated in a hashtable bucket entry chain
     */
    void HtVmemConcurrentMLock::free_entry_chain(struct dram_entry *head) {
        struct dram_entry* entry_p;
        while(head != NULL) {
            entry_p = head;
            head = head->next;
            vmem_free(vmp, entry_p);
        }
    }

    /**
     * free -- destructor
     */
    void HtVmemConcurrentMLock::destroy() {
        check();
        for (int i=0; i < INIT_BUCKETS_NUM; i+=1) {
            struct dram_entry *entry_p = root_p->buckets->bucket[i];
            if (entry_p != NULL) {
                free_entry_chain(entry_p);
            }
        }
        vmem_free(vmp, root_p->buckets->bucket);
        vmem_free(vmp, root_p->buckets);
        vmem_free(vmp, root_p);
        pthread_mutex_destroy(&mutex_lock_);
        vmem_delete(vmp);
    }
}   //ycsbc
