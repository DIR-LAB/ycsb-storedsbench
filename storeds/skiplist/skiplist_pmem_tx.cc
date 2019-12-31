//
// Created by aislam6 on 10/16/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <time.h>
#include <libpmemobj.h>

#include "../../lib/ex_common.h"
#include "skiplist_common.h"

namespace ycsbc {
    class SkiplistPmemTx : public StoredsBase {
    public:
        SkiplistPmemTx(const char *path) {
            SkiplistPmemTx::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void destroy();

        ~SkiplistPmemTx();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct sk_pmem_root *root_p = NULL;

        int check();

        void remove_free(uint64_t key);

        void remove_node(PMEMoid path[SKIPLIST_LEVELS_NUM]);

        void insert_node(PMEMoid new_node_oid, PMEMoid path_oid[SKIPLIST_LEVELS_NUM]);

        void find(uint64_t key, PMEMoid *path);
    };

    /*
     * check -- (internal) checks if skiplist is initialized or not
     */
    int SkiplistPmemTx::check() {
        if (root_oid.off == 0) {
            fprintf(stderr, "[%s]: FATAL: skiplist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * init -- initialize skiplist
     */
    int SkiplistPmemTx::init(const char *path) {
        srand((uint32_t) time(NULL));

        if (file_exists(path) != 0) {
            if ((pop = pmemobj_create(path, SK_LAYOUT_NAME, PMEM_SKIPLIST_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
                fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }
        else {
            if ((pop = pmemobj_open(path, SK_LAYOUT_NAME)) == NULL) {
                fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
                exit(0);
            }
        }

        root_oid = pmemobj_root(pop, sizeof(struct sk_pmem_root));
        root_p = (struct sk_pmem_root *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("[%s]: FATAL: The Root Object Not Initalized Yet, Exit!\n", __func__);
            exit(0);
        }

        TX_BEGIN(pop) {
            pmemobj_tx_add_range_direct(root_p, sizeof(struct sk_pmem_root));
            root_p->root_node_oid = pmemobj_tx_alloc(sizeof(struct sk_pmem_node), SK_NODE_TYPE);
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END

        check();
        return 1;
    }

    int SkiplistPmemTx::scan(const uint64_t key, int len, std::vector <std::vector<DB::Kuint64VstrPair>> &result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();

        PMEMoid path_oid[SKIPLIST_LEVELS_NUM], possible_found_oid;
        find(key, path_oid);
        possible_found_oid = ((struct sk_pmem_node *) pmemobj_direct(path_oid[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
        int len_count = 0;
        while(possible_found_oid.off != 0) {
            struct sk_pmem_node *possible_found_ptr = (struct sk_pmem_node *) pmemobj_direct(possible_found_oid);
            std::vector <DB::Kuint64VstrPair> tmp;
            tmp.push_back(std::make_pair(possible_found_ptr->entry.key, possible_found_ptr->entry.value));
            result.push_back(tmp);

            len_count += 1;
            if(len_count == len) break;
            possible_found_oid = possible_found_ptr->next[SKIPLIST_BASE_LEVEL];
        }
        return 1;
    }

    /**
     * find -- (internal) returns path to searched node
     * or if node doesn't exist, it will return path to place where key should be.
     */
    void SkiplistPmemTx::find(uint64_t key, PMEMoid *path) {
        PMEMoid active_node_oid = root_p->root_node_oid;
        for(int current_level = SKIPLIST_LEVELS_NUM - 1; current_level >= 0; current_level -= 1) {
            PMEMoid current_node_oid = ((struct sk_pmem_node *) pmemobj_direct(active_node_oid))->next[current_level];
            while(current_node_oid.off != 0 && ((struct sk_pmem_node *) pmemobj_direct(current_node_oid))->entry.key < key) {
                active_node_oid = current_node_oid;
                current_node_oid = ((struct sk_pmem_node *) pmemobj_direct(active_node_oid))->next[current_level];
            }
            path[current_level] = active_node_oid;
        }
    }

    /**
     * read -- read value of key and sets into result
     */
    int SkiplistPmemTx::read(const uint64_t key, void *&result) {
        //printf("[%s]: PARAM: key: %s\n", __func__, key);
        check();

        PMEMoid path_oid[SKIPLIST_LEVELS_NUM], possible_found_oid;
        //todo: need to pass by refference
        find(key, path_oid);
        possible_found_oid = ((struct sk_pmem_node *) pmemobj_direct(path_oid[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
        if(possible_found_oid.off != 0) {
            struct sk_pmem_node *possible_found_ptr = (struct sk_pmem_node *) pmemobj_direct(possible_found_oid);
            if(possible_found_ptr->entry.key == key) {
                result = possible_found_ptr->entry.value;
            }
        }
        return 1;
    }

    /**
     * update -- if key exist, update the value part in skiplist
     * if key doesn't exist, insert new key-value pair into skiplist
     */
    int SkiplistPmemTx::update(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);
        return insert(key, value);
    }

    /**
     * insert_node -- (internal) insert new node in selected places
     * this function must be called from a transaction block
     */
    void SkiplistPmemTx::insert_node(PMEMoid new_node_oid, PMEMoid path_oid[SKIPLIST_LEVELS_NUM]) {
        int current_level = SKIPLIST_BASE_LEVEL;

        // certainly assign node to level 0, and randomly for other levels
        do {
            struct sk_pmem_node *path_current_level_ptr = (struct sk_pmem_node *) pmemobj_direct(path_oid[current_level]);
            struct sk_pmem_node *new_node_ptr = (struct sk_pmem_node *) pmemobj_direct(new_node_oid);
            pmemobj_tx_add_range_direct(&path_current_level_ptr->next[current_level], sizeof(struct sk_pmem_node));
            new_node_ptr->next[current_level] = path_current_level_ptr->next[current_level];
            path_current_level_ptr->next[current_level] = new_node_oid;
        } while(++current_level < SKIPLIST_LEVELS_NUM && !(rand() % 2));
    }

    /**
     * insert -- insert new key-value pair into skiplist
     * if key already exist, update the value part in skiplist
     */
    int SkiplistPmemTx::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n\n", __func__, key, (char *) value);
        check();

        PMEMoid path_oid[SKIPLIST_LEVELS_NUM], possible_found_oid;
        find(key, path_oid);
        possible_found_oid = ((struct sk_pmem_node *) pmemobj_direct(path_oid[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];

        struct sk_pmem_node *possible_found_ptr = (struct sk_pmem_node *) pmemobj_direct(possible_found_oid);
        if(possible_found_oid.off != 0 && possible_found_ptr->entry.key == key) {
            //key already exist, update value
            TX_BEGIN(pop) {
                pmemobj_tx_add_range_direct(&possible_found_ptr->entry, sizeof(struct sk_entry));
                memcpy(possible_found_ptr->entry.value, (char *) value, strlen((char *) value) + 1);
            } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
            } TX_END
        }
        else {
            //key not exist, insert the new node
            TX_BEGIN(pop) {
                PMEMoid new_node_oid = pmemobj_tx_alloc(sizeof(struct sk_pmem_node), SK_NODE_TYPE);
                struct sk_pmem_node *new_node_ptr = (struct sk_pmem_node *) pmemobj_direct(new_node_oid);
                new_node_ptr->entry.key = key;
                memcpy(new_node_ptr->entry.value, (char *) value, strlen((char *) value) + 1);
                insert_node(new_node_oid, path_oid);
            } TX_ONABORT {
                    fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
                    abort();
            } TX_END
        }
        return 1;
    }

    /*
     * remove_node -- (internal) removes selected node
     * this function must be called from a transaction block
     */
    void SkiplistPmemTx::remove_node(PMEMoid path[SKIPLIST_LEVELS_NUM]) {
        PMEMoid possible_remove = ((struct sk_pmem_node *) pmemobj_direct(path[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
        struct sk_pmem_node *possible_remove_ptr = (struct sk_pmem_node *) pmemobj_direct(possible_remove);
        for(int i=SKIPLIST_BASE_LEVEL; i<SKIPLIST_LEVELS_NUM; i+=1) {
            struct sk_pmem_node *path_cur_level_ptr = (struct sk_pmem_node *) pmemobj_direct(path[i]);
            struct sk_pmem_node *path_cur_level_next_ptr = (struct sk_pmem_node *) pmemobj_direct(path_cur_level_ptr->next[i]);
            if(path_cur_level_ptr->next[i].off != 0 && (path_cur_level_next_ptr->entry.key == possible_remove_ptr->entry.key)) {
                pmemobj_tx_add_range_direct(&path_cur_level_ptr->next[i], sizeof(struct sk_pmem_node));
                //struct sk_pmem_node *tmp = path[i]->next[i];
                path_cur_level_ptr->next[i] = possible_remove_ptr->next[i];
                //free(tmp);
            }
        }
        //pmemobj_tx_add_range_direct(&possible_remove, sizeof(struct sk_pmem_node));
        //pmemobj_tx_free(possible_remove);
        //todo: need careful analysis if this memory de-allocation is the right way or not!
    }

    /*
     * remove_free -- (internal) removes key-value pair from the list
     * this function must be called from a transaction block
     */
    void SkiplistPmemTx::remove_free(uint64_t key) {
        PMEMoid path[SKIPLIST_LEVELS_NUM], possible_remove;
        find(key, path);
        possible_remove = ((struct sk_pmem_node *) pmemobj_direct(path[SKIPLIST_BASE_LEVEL]))->next[SKIPLIST_BASE_LEVEL];
        if(possible_remove.off != 0
           && ((struct sk_pmem_node *) pmemobj_direct(possible_remove))->entry.key == key) {
            remove_node(path);
        }
    }

    /**
     * free -- deallocate memory of skiplist
     */
    //todo: need to work on this
    void SkiplistPmemTx::destroy() {
        check();
        TX_BEGIN(pop) {
            struct sk_pmem_node *root_node_p = (struct sk_pmem_node *) pmemobj_direct(root_p->root_node_oid);
            while(root_node_p->next[0].off != 0) {
                PMEMoid next_oid = root_node_p->next[0];
                remove_free(((struct sk_pmem_node *) pmemobj_direct(next_oid))->entry.key);
            }
            pmemobj_tx_add_range_direct(root_p, sizeof(struct sk_pmem_root));
            pmemobj_tx_free(root_oid);
            root_oid = OID_NULL;
        } TX_ONABORT {
            fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
            abort();
        } TX_END
        pmemobj_close(pop);
    }
}   //ycsbc