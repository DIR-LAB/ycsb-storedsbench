//
// Created by aislam6 on 10/27/19.
//

#ifndef YCSB_STOREDSBENCH_BTREE_PMEM_H
#define YCSB_STOREDSBENCH_BTREE_PMEM_H

#include "../../lib/ex_common.h"
#include "../storeds_db_client.h"
#include "btree_common.h"

namespace ycsbc {
    class BTreePmem : public StoredsDbClient {
    public:
        BTreePmem(const char *path) {
            init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

    private:
        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct btree_pmem_node *root_p = NULL;
        int total_nodes = 0;         //store the number of nodes the btree have

        int check();

        int is_node_full(int nk);

        PMEMoid create_node(bool _is_leaf);

        char *search(PMEMoid current_node_oid, uint64_t key);

        void split_node(int idx, PMEMoid parent_oid, PMEMoid child_oid);

        void insert_not_full(PMEMoid node_oid, uint64_t key, void *value);

        bool update_if_found(PMEMoid current_node_oid, uint64_t key, void *value);

        void recursive_free(PMEMoid current_node_oid);
    };
}   //ycsbc

#endif //YCSB_STOREDSBENCH_BTREE_PMEM_H