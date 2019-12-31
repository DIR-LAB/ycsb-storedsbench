//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <libpmemobj.h>

#include "../../lib/ex_common.h"
#include "ht_common.h"

namespace ycsbc {
    class HtPmemTxConcurrentMLock : public StoredsBase {
    public:
        HtPmemTxConcurrentMLock(const char *path) {
            init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct pmem_hashtable_concurrent_mlock *root_p = NULL;

        int check();

        void print();

        uint64_t hash_function(uint64_t value);

        int reinit(size_t new_len);
    };

    /*
     * check -- checks if hashtable has been initialized
     */
    int HtPmemTxConcurrentMLock::check() {
        if (root_p->buckets.off == 0) {
            fprintf(stderr, "[%s]: FATAL: hashtable not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /*
     * print -- prints complete hashtable state
     */
    void HtPmemTxConcurrentMLock::print() {
        check();

        PMEMoid buckets_oid = root_p->buckets;
        struct pmem_buckets * buckets_p = ((struct pmem_buckets *) pmemobj_direct(buckets_oid));
        PMEMoid entry_oid = OID_NULL;

        printf("a: %d, b: %d, p: %lu\n", root_p->hash_fun_coeff_a, root_p->hash_fun_coeff_b, root_p->hash_fun_coeff_p);
        printf("total buckets: %lu\n", buckets_p->nbuckets);

        for (size_t i = 0; i < buckets_p->nbuckets; ++i) {
            if (buckets_p->bucket[i].off == 0) continue;

            int num = 0;
            printf("bucket #%lu:", i);
            for (entry_oid = buckets_p->bucket[i]; entry_oid.off != 0; entry_oid = ((struct pmem_entry *) pmemobj_direct(entry_oid))->next) {
                struct pmem_entry *entry_p = (struct pmem_entry *) pmemobj_direct(entry_oid);
                printf(" <%lu, %s>", entry_p->key, entry_p->value);
                num++;
            }
            printf(" (%d)\n", num);
        }
    }

    /*
     * hash_function -- the simplest hashing function,
     * see https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
     */
    uint64_t HtPmemTxConcurrentMLock::hash_function(uint64_t value) {
        uint32_t a = root_p->hash_fun_coeff_a;
        uint32_t b = root_p->hash_fun_coeff_b;
        uint64_t p = root_p->hash_fun_coeff_p;
        size_t len = ((struct pmem_buckets *) pmemobj_direct(root_p->buckets))->nbuckets;

        return ((value * a + b) % p) % len;
    }

    /*
     * reinit -- reinitialize the hashtable with a new number of buckets,
     * not implemented yet
     */
    int HtPmemTxConcurrentMLock::reinit(size_t new_len) {
        return 1;
    }

    /*
     * init -- hashtable initializer
     */
    int HtPmemTxConcurrentMLock::init(const char *path) {
        srand((uint32_t) time(NULL));

        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, HT_LAYOUT_NAME, PM_HASHTABLE_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        } else {
            if ((pop = pmemobj_open(path, HT_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct pmem_hashtable_concurrent_mlock));
        root_p = (struct pmem_hashtable_concurrent_mlock *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
            exit(0);
        }

        size_t len = INIT_BUCKETS_NUM;
        size_t buckets_sz = sizeof(struct pmem_buckets) + len * sizeof(struct pmem_entry);

        TX_BEGIN(pop) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct pmem_hashtable_concurrent_mlock));

            root_p->seed = (uint32_t)time(NULL);
            do {
                root_p->hash_fun_coeff_a = (uint32_t) rand();
            } while(root_p->hash_fun_coeff_a == 0);

            root_p->hash_fun_coeff_b = (uint32_t) rand();
            root_p->hash_fun_coeff_p = HASH_FUNC_COEFF_P;

            root_p->buckets = pmemobj_tx_zalloc(buckets_sz, HT_BUCKETS_TYPE);
            ((struct pmem_buckets *) pmemobj_direct(root_p->buckets))->nbuckets = len;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        check();
        return 1;
    }

    int HtPmemTxConcurrentMLock::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        throw "Scan: function not implemented!";
    }

    /**
     * read -- read 'value' of 'key' and place it into '&result'
     */
    int HtPmemTxConcurrentMLock::read(const uint64_t key, void *&result) {
        check();
        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;

        struct pmem_buckets *buckets_p = (struct pmem_buckets *) pmemobj_direct(root_p->buckets);
        PMEMoid entry_oid = OID_NULL;

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        uint64_t hash_value = hash_function(key);

        for(entry_oid = buckets_p->bucket[hash_value]; entry_oid.off != 0; entry_oid = ((struct pmem_entry *) pmemobj_direct(entry_oid))->next) {
            if(((struct pmem_entry *) pmemobj_direct(entry_oid))->key == key) {
                //key found! replace the value and return
                struct pmem_entry *entry_p = (struct pmem_entry *) pmemobj_direct(entry_oid);
                result = entry_p->value;
            }
        }

        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    /**
     * update -- update 'value' of 'key' into the hashtable,
     * will insert the 'value' if 'key' not exists
     */
    int HtPmemTxConcurrentMLock::update(const uint64_t key, void *value) {
        return insert(key, value);
    }

    /**
     * insert -- inserts 'value' into the hashtable,
     * will update the 'value' if 'key' already exists
     */
    int HtPmemTxConcurrentMLock::insert(const uint64_t key, void *value) {
        check();
        if (pmemobj_mutex_lock(pop, &root_p->mlock) != 0) return 0;

        struct pmem_buckets *buckets_p = (struct pmem_buckets *) pmemobj_direct(root_p->buckets);
        PMEMoid entry_oid = OID_NULL;

        //uint64_t uint64_key = strtoull(key, NULL, 0);
        uint64_t hash_value = hash_function(key);

        for(entry_oid = buckets_p->bucket[hash_value]; entry_oid.off != 0; entry_oid = ((struct pmem_entry *) pmemobj_direct(entry_oid))->next) {
            if(((struct pmem_entry *) pmemobj_direct(entry_oid))->key == key) {
                //key found! replace the value and return
                struct pmem_entry *entry_p = (struct pmem_entry *) pmemobj_direct(entry_oid);
                TX_BEGIN(pop) {
                    pmemobj_tx_add_range_direct(entry_p, sizeof(struct pmem_entry));
                    memcpy(entry_p->value, (char *) value, strlen((char *) value) + 1);
                } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
                } TX_END

                pmemobj_mutex_unlock(pop, &root_p->mlock);
                return 1;
            }
        }

        //key not found! need to insert data into bucket[hash_value]
        TX_BEGIN(pop) {
            pmemobj_tx_add_range_direct(&(buckets_p->bucket[hash_value]), sizeof(struct pmem_entry));

            PMEMoid entry_oid_new = pmemobj_tx_alloc(sizeof(struct pmem_entry), HT_ENTRY_TYPE);
            struct pmem_entry *entry_p = (struct pmem_entry *) pmemobj_direct(entry_oid_new);
            entry_p->key = key;
            memcpy(entry_p->value, (char *) value, strlen((char *) value) + 1);
            entry_p->next = buckets_p->bucket[hash_value];
            buckets_p->bucket[hash_value] = entry_oid_new;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        pmemobj_mutex_unlock(pop, &root_p->mlock);
        return 1;
    }

    /**
     * destroy -- destructor
     */
    void HtPmemTxConcurrentMLock::destroy() {
        TX_BEGIN(pop) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct pmem_hashtable_concurrent_lock));
            PMEMoid buckets_oid = root_p->buckets;
            struct pmem_buckets *buckets_p = ((struct pmem_buckets *) pmemobj_direct(buckets_oid));
            PMEMoid entry_oid = OID_NULL;

            for (size_t i = 0; i < buckets_p->nbuckets; ++i) {
                for (entry_oid = buckets_p->bucket[i]; entry_oid.off != 0; ) {
                    PMEMoid current_entry_oid = entry_oid;
                    entry_oid = ((struct pmem_entry *) pmemobj_direct(entry_oid))->next;

                    pmemobj_tx_free(current_entry_oid);
                }
            }
            pmemobj_tx_free(root_p->buckets);
            root_p->buckets = OID_NULL;
            pmemobj_tx_free(root_oid);
            root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        pmemobj_close(pop);
    }
}   //ycsbc