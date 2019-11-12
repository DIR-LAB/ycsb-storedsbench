//
// Created by aislam6 on 10/27/19.
//

#ifndef YCSB_STOREDSBENCH_BTREE_DRAM_H
#define YCSB_STOREDSBENCH_BTREE_DRAM_H

#include "../storeds_db_client.h"
#include "btree_common.h"

namespace ycsbc {
    class BTreeDram : public StoredsDbClient {
    public:
        BTreeDram(const char *path) {
            init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

    private:
        struct btree_dram_node *root;

        int check();

        int is_node_full(int nk);

        struct btree_dram_node *create_node(bool _is_leaf);

        char *search(struct btree_dram_node *current_node, uint64_t key);

        void split_node(int idx, struct btree_dram_node *parent, struct btree_dram_node *child);

        void insert_not_full(struct btree_dram_node *node, uint64_t key, void *value);

        bool update_if_found(struct btree_dram_node *current_node, uint64_t key, void *value);

        void recursive_free(struct btree_dram_node *current_node);
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BTREE_DRAM_H