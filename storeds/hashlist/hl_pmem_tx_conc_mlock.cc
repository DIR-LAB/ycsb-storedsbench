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
#include <libpmemobj.h>

#include "hl_common.h"
#include "../linkedlist/linkedlist_common.h"

namespace ycsbc {
    class HlPmemTxConcurrentMLock : public StoredsBase {
    public:
        HlPmemTxConcurrentMLock(const char *path) {
            HlPmemTxConcurrentMLock::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~HlPmemTxConcurrentMLock();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid ll_root_oid;
        struct dram_hashlist *hl_root_p;
        struct ll_pmem_concurrent_mlock_root *ll_root_p = NULL;

        int check();

        uint64_t hash_function(uint64_t value);

        struct hl_dram_entry* new_hash_entry(uint64_t key, void *_ll_p);

        PMEMoid create_ll_node(const uint64_t key, void *value);

        void free_hl_entry_chain(struct hl_dram_entry *head);
    };

    /*
     * check -- checks if hash-list has been initialized
     */
    inline int HlPmemTxConcurrentMLock::check() {
        if (hl_root_p == NULL || hl_root_p->buckets == NULL) {
            fprintf(stderr, "[%s]: FATAL: hash-list not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * hash_function_dram -- the simplest hashing function,
     * see https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
     */
    uint64_t HlPmemTxConcurrentMLock::hash_function(uint64_t value) {
        uint32_t a = hl_root_p->hash_fun_coeff_a;
        uint32_t b = hl_root_p->hash_fun_coeff_b;
        uint64_t p = hl_root_p->hash_fun_coeff_p;
        size_t len = hl_root_p->buckets->nbuckets;

        return ((value * a + b) % p) % len;
    }

    /*
     * init -- hash-list initializer
     */
    int HlPmemTxConcurrentMLock::init(const char *path) {
        //initializing hash-list
        size_t len = INIT_BUCKETS_NUM;

        hl_root_p = (struct dram_hashlist *) malloc(sizeof(struct dram_hashlist));
        hl_root_p->seed = (uint32_t)time(NULL);
        do {
            hl_root_p->hash_fun_coeff_a = (uint32_t) rand();
        } while(hl_root_p->hash_fun_coeff_a == 0);

        hl_root_p->hash_fun_coeff_b = (uint32_t) rand();
        hl_root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;
        hl_root_p->buckets = (struct hl_dram_buckets *) malloc(sizeof(struct hl_dram_buckets));
        hl_root_p->buckets->nbuckets = len;
        hl_root_p->buckets->bucket = (struct hl_dram_entry **) calloc(len, sizeof(struct hl_dram_entry));

        //initializing linked-list
        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, LL_LAYOUT_NAME, PMEM_LL_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        } else {
            if ((pop = pmemobj_open(path, LL_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                return 1;
            }
        }

        ll_root_oid = pmemobj_root(pop, sizeof(struct ll_pmem_concurrent_mlock_root));
        ll_root_p = (struct ll_pmem_concurrent_mlock_root *) pmemobj_direct(ll_root_oid);
        if(ll_root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }

        return 1;
    }

    int HlPmemTxConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * read -- read 'value' of 'key' and place it into '&result'
     */
    int HlPmemTxConcurrentMLock::read(const uint64_t key, void *&result) {
        check();

        if (pmemobj_mutex_lock(pop, &ll_root_p->mlock) != 0) return 0;
        struct hl_dram_buckets *buckets_p = hl_root_p->buckets;
        uint64_t hash_value = hash_function(key);

        for(struct hl_dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found! put it to result and return
                result = ((ll_pmem_node *) entry_p->ll_p)->value;
                break;
            }
        }

        pmemobj_mutex_unlock(pop, &ll_root_p->mlock);
        return 1;
    }

    /**
     * update -- update 'value' of 'key' into the hash-list, will insert the 'value' if 'key' not exists
     */
    int HlPmemTxConcurrentMLock::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * new_hash_entry -- create new hash-list entry
     */
    struct hl_dram_entry* HlPmemTxConcurrentMLock::new_hash_entry(uint64_t key, void *_ll_p) {
        struct hl_dram_entry *entry_p = (struct hl_dram_entry *) malloc(sizeof(struct hl_dram_entry));
        entry_p->key = key;
        entry_p->ll_p = _ll_p;
        entry_p->next = (struct hl_dram_entry *) malloc(sizeof(struct hl_dram_entry));
        return entry_p;
    }

    inline PMEMoid HlPmemTxConcurrentMLock::create_ll_node(const uint64_t key, void *value) {
        PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct ll_pmem_node), LL_NODE_TYPE);
        struct ll_pmem_node *pmem_node_ptr = (struct ll_pmem_node *) pmemobj_direct(new_node_oid);
        pmem_node_ptr->key = key;
        memcpy(pmem_node_ptr->value, (char *) value, strlen((char *) value) + 1);

        return new_node_oid;
    }

    /**
     * insert -- inserts 'value' into the hash-list, will update the 'value' if 'key' already exists
     */
    int HlPmemTxConcurrentMLock::insert(const uint64_t key, void *value) {
        check();

        if (pmemobj_mutex_lock(pop, &ll_root_p->mlock) != 0) return 0;
        //first search in the hash-list
        struct hl_dram_buckets *buckets_p = hl_root_p->buckets;
        uint64_t hash_value = hash_function(key);

        for(struct hl_dram_entry *entry_p = buckets_p->bucket[hash_value]; entry_p != NULL; entry_p = entry_p->next) {
            if(entry_p->key == key) {
                //key found in the hash-list! replace the value in the linked-list and return
                TX_BEGIN(pop) {
                    pmemobj_tx_add_range_direct((ll_pmem_node *) entry_p->ll_p, sizeof(struct ll_pmem_node));
                    memcpy(((ll_pmem_node *) entry_p->ll_p)->value, (char *) value, strlen((char *) value) + 1);
                } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
                } TX_END
                
                pmemobj_mutex_unlock(pop, &ll_root_p->mlock);
                return 1;
            }
        }

        //key not found!
        //entry to linked-list
        PMEMoid new_node_oid;
        TX_BEGIN(pop) {
            new_node_oid = create_ll_node(key, value);
            if(ll_root_p->head.off == 0) {
                pmemobj_tx_add_range(ll_root_oid, 0, sizeof(struct ll_pmem_concurrent_mlock_root));
                ll_root_p->head = new_node_oid;
                ll_root_p->tail = new_node_oid;
            }
            else {
                pmemobj_tx_add_range(ll_root_p->tail, 0, sizeof(struct ll_pmem_node));
                ((struct ll_pmem_node *) pmemobj_direct(ll_root_p->tail))->next = new_node_oid;
                ll_root_p->tail = ((struct ll_pmem_node *) pmemobj_direct(ll_root_p->tail))->next;
            }
        } TX_ONABORT {
                fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                abort();
        } TX_END

        //entry to hash-list
        struct hl_dram_entry *entry_p = new_hash_entry(key, (struct ll_pmem_node *) pmemobj_direct(new_node_oid));
        entry_p->next = buckets_p->bucket[hash_value];
        buckets_p->bucket[hash_value] = entry_p;

        pmemobj_mutex_unlock(pop, &ll_root_p->mlock);
        return 1;
    }

    /**
     * free_entry_chain -- free memory allocated in a hash-list bucket entry chain
     */
    void HlPmemTxConcurrentMLock::free_hl_entry_chain(struct hl_dram_entry *head) {
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
    void HlPmemTxConcurrentMLock::destroy() {
        check();

        //free the hashlist
        for (int i=0; i < INIT_BUCKETS_NUM; i+=1) {
            struct hl_dram_entry *entry_p = hl_root_p->buckets->bucket[i];
            if (entry_p != NULL) {
                free_hl_entry_chain(entry_p);
            }
        }
        free(hl_root_p->buckets->bucket);
        free(hl_root_p->buckets);
        free(hl_root_p);

        //free the linkedlist
        PMEMoid current_node;

        TX_BEGIN(pop) {
            pmemobj_tx_add_range(ll_root_oid, 0, sizeof(struct ll_pmem_concurrent_mlock_root));
            while (ll_root_p->head.off != 0) {
                current_node = ll_root_p->head;
                ll_root_p->head = ((struct ll_pmem_node *) pmemobj_direct(ll_root_p->head))->next;
                pmemobj_tx_free(current_node);
            }
            if(ll_root_p->tail.off != 0) pmemobj_tx_free(ll_root_p->tail);
            ll_root_p->head = OID_NULL;
            ll_root_p->tail = OID_NULL;
            ll_root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        pmemobj_close(pop);
    }
}   //ycsbc
