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

#include "ht_common.h"

namespace ycsbc {
    class HtDram : public StoredsBase {
    public:
        HtDram(const char *path) {
            HtDram::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~HtDram();

    private:
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
    inline int HtDram::check() {
        if (root_p == NULL || root_p->buckets == NULL) {
            fprintf(stderr, "[%s]: FATAL: hashtable not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
    * print -- prints complete hashtable state
    */
    void HtDram::print() {
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
    uint64_t HtDram::hash_function(uint64_t value) {
        uint32_t a = root_p->hash_fun_coeff_a;
        uint32_t b = root_p->hash_fun_coeff_b;
        uint64_t p = root_p->hash_fun_coeff_p;
        size_t len = root_p->buckets->nbuckets;

        return ((value * a + b) % p) % len;
    }

    /*
     * reinit -- reinitialize the hashtable with a new number of buckets, not implemented yet
     */
    int HtDram::reinit(size_t new_len) {
        return 1;
    }

    /*
     * init -- hashtable initializer
     */
    int HtDram::init(const char *path) {
        size_t len = INIT_BUCKETS_NUM;

        root_p = (struct dram_hashtable *) malloc(sizeof(struct dram_hashtable));
        root_p->seed = (uint32_t)time(NULL);
        do {
            root_p->hash_fun_coeff_a = (uint32_t) rand();
        } while(root_p->hash_fun_coeff_a == 0);

        root_p->hash_fun_coeff_b = (uint32_t) rand();
        root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;
        root_p->buckets = (struct dram_buckets *) malloc(sizeof(struct dram_buckets));
        root_p->buckets->nbuckets = len;
        root_p->buckets->bucket = (struct dram_entry **) calloc(len, sizeof(struct dram_entry));

        return 1;
    }

    int HtDram::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * read -- read 'value' of 'key' and place it into '&result'
     */
    int HtDram::read(const uint64_t key, void *&result) {
        check();

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
        return 1;
    }

    /**
     * update -- update 'value' of 'key' into the hashtable, will insert the 'value' if 'key' not exists
     */
    int HtDram::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * new_entry -- create new hashtable entry
     */
    struct dram_entry* HtDram::new_entry(uint64_t key, const char* value) {
        struct dram_entry *entry_p = (struct dram_entry *) malloc(sizeof(struct dram_entry));
        entry_p->key = key;
        memcpy(entry_p->value, (char *) value, strlen((char *) value) + 1);
        entry_p->next = (struct dram_entry *) malloc(sizeof(struct dram_entry));
        return entry_p;
    }

    /**
     * insert -- inserts 'value' into the hashtable, will update the 'value' if 'key' already exists
     */
    int HtDram::insert(const uint64_t key, void *value) {
        check();

        struct dram_buckets *buckets_p = root_p->buckets;
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        uint64_t hash_value = hash_function(key);

        for(struct dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found! replace the value and return
                memcpy(entry_p->value, (char *) value, strlen((char *) value) + 1);
                return 1;
            }
        }

        //key not found! need to insert data into bucket[hash_value]
        struct dram_entry *entry_p = new_entry(key, (const char*) value);
        entry_p->next = buckets_p->bucket[hash_value];
        buckets_p->bucket[hash_value] = entry_p;
        return 1;
    }

    /**
     * free_entry_chain -- free memory allocated in a hashtable bucket entry chain
     */
    void HtDram::free_entry_chain(struct dram_entry *head) {
        struct dram_entry* entry_p;
        while(head != NULL) {
            entry_p = head;
            head = head->next;
            free(entry_p);
        }
    }

    /**
     * free -- destructor
     */
    void HtDram::destroy() {
        check();
        for (int i=0; i < INIT_BUCKETS_NUM; i+=1) {
            struct dram_entry *entry_p = root_p->buckets->bucket[i];
            if (entry_p != NULL) {
                free_entry_chain(entry_p);
            }
        }
        free(root_p->buckets->bucket);
        free(root_p->buckets);
        free(root_p);
    }
}   //ycsbc
