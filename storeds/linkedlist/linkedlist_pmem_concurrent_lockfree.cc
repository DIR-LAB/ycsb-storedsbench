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

#include "../../lib/ex_common.h"
#include "linkedlist_common.h"

using namespace std;

struct ll_pmem_root {
        PMEMoid head;
        PMEMoid tail;
    };
struct ll_pmem_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        PMEMoid next;
        int* flag;
    };

enum ll_alloc_types {
        LL_NODE_TYPE,
        LL_MAX_TYPES
    };

class LinkedlistPmem_CAS{
    public:
    LinkedlistPmem_CAS(const char *path) {
        LinkedlistPmem_CAS::init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

        ~LinkedlistPmem_CAS();

        /* Private Data */
        PMEMobjpool *pop = NULL;
        PMEMoid root_oid;
        struct ll_pmem_root *root_p = NULL;

        int check();
    };

    int LinkedlistPmem_CAS::check() {
        if (root_p->head.off == 0) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    int LinkedlistPmem_CAS::init(const char *path) {
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

        root_oid = pmemobj_root(pop, sizeof(struct ll_pmem_root));
        root_p = (struct ll_pmem_root *) pmemobj_direct(root_oid);
        if(root_p == NULL) {
            printf("FATAL: The Root Object Not Initalized Yet, Exit!\n");
            exit(0);
        }
        return 1;
    }

    int LinkedlistPmem_CAS::read(const char *key, void *&result) {
        check();

        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_pmem_node *current_node = (struct ll_pmem_node *) pmemobj_direct(root_p->head);

        while (current_node != NULL) {
            if (current_node->key == uint64_key) {
                result = current_node->value;
                break;
            }
            current_node = (struct ll_pmem_node *) pmemobj_direct(current_node->next);
        }
        return 1;
    }

    int LinkedlistPmem_CAS::update(const char *key, void *value) {
        check();

        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_pmem_node *current_node = (struct ll_pmem_node *) pmemobj_direct(root_p->head);

        while (current_node != NULL) {
            if (current_node->key == uint64_key) {
                pmemobj_memcpy_persist(pop, current_node->value, (const char *) value, strlen((char *) value) + 1);
                break;
            }
            current_node = (struct ll_pmem_node *) pmemobj_direct(current_node->next);
        }
        return 1;
    }

    int LinkedlistPmem_CAS::insert(const char *key, void *value) {
        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_pmem_node *in_memory_node_ptr = (struct ll_pmem_node *) malloc(sizeof(struct ll_pmem_node));
        in_memory_node_ptr->key = uint64_key;
        strcpy(in_memory_node_ptr->value, (const char *) value);

        PMEMoid new_node_oid;
        pmemobj_alloc(pop, &new_node_oid, sizeof(struct ll_pmem_node), LL_NODE_TYPE, NULL, NULL);
        struct ll_pmem_node *pmem_node_ptr = (struct ll_pmem_node *) pmemobj_direct(new_node_oid);
        memcpy(pmem_node_ptr, in_memory_node_ptr, sizeof(struct ll_pmem_node));
        pmem_node_ptr->flag = new int(0);
        free(in_memory_node_ptr);
        PMEMoid head_oid = root_p->head;
        PMEMoid tail_oid = root_p->tail;
        ll_pmem_node* head_ptr = (struct ll_pmem_node *)pmemobj_direct(head_oid);
        ll_pmem_node* tail_ptr = (struct ll_pmem_node *)pmemobj_direct(tail_oid);
        if(head_oid.off == 0) {
            head_oid = new_node_oid;
            tail_oid = new_node_oid;
            pmemobj_persist(pop, &head_oid, sizeof(struct ll_pmem_root));
            pmemobj_persist(pop, &tail_oid, sizeof(struct ll_pmem_root));
        }

        else if(uint64_key < head_ptr->key){
            ll_pmem_node* prev_head = head_ptr;
            bool race_result = __sync_bool_compare_and_swap(head_ptr->flag,0,1);
            if(race_result){
                pmem_node_ptr->next = head_oid;
                pmemobj_persist(pop,&pmem_node_ptr->next,sizeof(struct ll_pmem_node));
                head_oid = new_node_oid;
                pmemobj_persist(pop,&head_oid,sizeof(struct ll_pmem_node));
                prev_head->flag = new int(0);
                cout<<endl<<"returned";
                return 1;
            }
            return 0;
        }

        else if(uint64_key > tail_ptr->key){
            ll_pmem_node* prev_tail = tail_ptr;
            bool race_result = __sync_bool_compare_and_swap(tail_ptr->flag,0,1);
            if(race_result){
                tail_ptr->next = new_node_oid;
                pmemobj_persist(pop,&tail_ptr->next,sizeof(struct ll_pmem_node));
                tail_oid = new_node_oid;
                pmemobj_persist(pop,&tail_oid,sizeof(struct ll_pmem_node));

                prev_tail->flag = new int(0);
                cout<<endl<<"returned";
                return 1;
            }
            return 0;
        }
        else {
            ll_pmem_node* temp = head_ptr;
            while(temp->next.off!=0){
                ll_pmem_node* next_ptr = (struct ll_pmem_node *)pmemobj_direct(temp->next);
                if(temp->key<=uint64_key&&next_ptr->key>=uint64_key){
                    cout<<next_ptr->flag<<" "<<next_ptr->flag <<endl;
                    bool race_result = __sync_bool_compare_and_swap(temp->flag,0,1);
                    cout<<race_result<<endl;
                    if(race_result){
                        pmem_node_ptr->next = temp->next;
                        pmemobj_persist(pop,&pmem_node_ptr->next,sizeof(struct ll_pmem_node));
                        temp->next = new_node_oid;
                        pmemobj_persist(pop,&temp->next,sizeof(struct ll_pmem_node));
                        temp->flag = new int(0);

                        return 1;
                    }
                }
                temp = (struct ll_pmem_node *)pmemobj_direct(temp->next);
            }

        }
        cout<<endl;
        cout<<"Failed inserted";
        return 0;
        if(root_p->head.off == 0) {
            root_p->head = new_node_oid;
            root_p->tail = new_node_oid;
            pmemobj_persist(pop, root_p, sizeof(struct ll_pmem_root));
        }
        else {
            ((struct ll_pmem_node *) pmemobj_direct(root_p->tail))->next = new_node_oid;
            root_p->tail = ((struct ll_pmem_node *) pmemobj_direct(root_p->tail))->next;
            pmemobj_persist(pop, &root_p->tail, sizeof(struct ll_pmem_node));
        }
        return 1;
    }

    void LinkedlistPmem_CAS::destroy() {
        check();
        PMEMoid current_node;

        while (root_p->head.off != 0) {
            current_node = root_p->head;
            root_p->head = ((struct ll_pmem_node *) pmemobj_direct(root_p->head))->next;
            pmemobj_free(&current_node);
        }
        if(root_p->tail.off != 0) pmemobj_free(&(root_p->tail));
        root_p->head = OID_NULL;
        root_p->tail = OID_NULL;
        root_oid = OID_NULL;
        pmemobj_close(pop);
    }
