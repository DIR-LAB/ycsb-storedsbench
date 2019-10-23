//
// Created by Islam, Abdullah Al Raqibul on 10/15/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/stat.h>
#include <libpmemobj.h>

#include "../ex_common.h"
#include "linkedlist_pmem_tx.h"

/* size of the pmem object pool -- 1 GB */
#define PM_ARRAY_POOL_SIZE ((size_t) (1 << 30))

/* name of layout in the pool */
#define LAYOUT_NAME "linkedlist_layout"

/* Static Global Data */
static PMEMobjpool *pop = NULL;
static PMEMoid root_oid;
static struct ll_root *root_p = NULL;

const int pmem_array_size = 1000000;
const int default_value_len = 101;

struct ll_root {
    PMEMoid head;
    PMEMoid tail;
};

struct node {
    uint64_t key;
    char value[default_value_len];
    PMEMoid next;
};

/**
 * types -- types of allocations
 */
enum types {
    NODE_TYPE,
    MAX_TYPES
};

int linkedlist_pmem_tx_check() {
    if (root_p->head.off == 0) {
        fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

int linkedlist_pmem_tx_init(const char *path) {
    if (file_exists(path) != 0) {
        if ((pop = pmemobj_create(path, LAYOUT_NAME, PM_ARRAY_POOL_SIZE, CREATE_MODE_RW)) == NULL) {
            fprintf(stderr, "failed to create pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    } else {
        if ((pop = pmemobj_open(path, LAYOUT_NAME)) == NULL) {
            fprintf(stderr, "failed to open pool: %s\n", pmemobj_errormsg());
            return 1;
        }
    }

    root_oid = pmemobj_root(pop, sizeof(struct ll_root));
    root_p = (struct ll_root *) pmemobj_direct(root_oid);
    if(root_p == NULL) {
        printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
        exit(0);
    }
    return 1;
}

int linkedlist_pmem_tx_read(const char *key, void *result) {
    linkedlist_pmem_tx_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = (struct node *) pmemobj_direct(root_p->head);

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            result = current_node->value;
            break;
        }
        current_node = (struct node *) pmemobj_direct(current_node->next);
    }
    return 1;
}

int linkedlist_pmem_tx_update(const char *key, void *value) {
    linkedlist_pmem_tx_check();

    uint64_t uint64_key = strtoull(key, NULL, 0);
    struct node *current_node = (struct node *) pmemobj_direct(root_p->head);

    while (current_node != NULL) {
        if (current_node->key == uint64_key) {
            TX_BEGIN(pop) {
                pmemobj_tx_add_range_direct(current_node, sizeof(struct node));
                memcpy(current_node->value, (const char *) value, strlen((char *) value));
            } TX_ONABORT {
				fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
				abort();
			} TX_END
            break;
        }
        current_node = (struct node *) pmemobj_direct(current_node->next);
    }
    return 1;
}

int linkedlist_pmem_tx_insert(const char *key, void *value) {
    TX_BEGIN(pop) {
        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct node *in_memory_node_ptr = (struct node *) malloc(sizeof(struct node));
        in_memory_node_ptr->key = uint64_key;
        strcpy(in_memory_node_ptr->value, (const char *) value);

        PMEMoid new_node_oid;
        pmemobj_zalloc(pop, &new_node_oid, sizeof(struct node), NODE_TYPE);
        struct node *pmem_node_ptr = (struct node *) pmemobj_direct(new_node_oid);
        memcpy(pmem_node_ptr, in_memory_node_ptr, sizeof(struct node));
        free(in_memory_node_ptr);

        if(root_p->head.off == 0) {
            pmemobj_tx_add_range(root_oid, 0, sizeof(struct ll_root));
            root_p->head = new_node_oid;
            root_p->tail = new_node_oid;
        }
        else {
            pmemobj_tx_add_range(root_p->tail, 0, sizeof(struct node));
            ((struct node *) pmemobj_direct(root_p->tail))->next = new_node_oid;
            root_p->tail = ((struct node *) pmemobj_direct(root_p->tail))->next;
        }
    } TX_ONABORT {
        fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
        abort();
    } TX_END
    return 1;
}

void linkedlist_pmem_tx_free() {
    linkedlist_pmem_tx_check();
    PMEMoid current_node;

    TX_BEGIN(pop) {
        pmemobj_tx_add_range(root_oid, 0, sizeof(struct ll_root));
        while (root_p->head.off != 0) {
            current_node = root_p->head;
            root_p->head = ((struct node *) pmemobj_direct(root_p->head))->next;
            pmemobj_tx_free(current_node);
        }
        pmemobj_tx_free(root_p->tail);
        root_p->head = OID_NULL;
        root_p->tail = OID_NULL;
        root_oid = OID_NULL;
    } TX_ONABORT {
		fprintf(stderr, "[%s]: FATAL: transaction aborted: %s\n", __func__, pmemobj_errormsg());
		abort();
	} TX_END
    pmemobj_close(pop);
}