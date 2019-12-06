//
// Created by Anirudh on 11/27/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "linkedlist_common.h"
#include "libatomic_ops/src/atomic_ops.h"

   #define DEFAULT_VALUE_LEN 101
using namespace std;
struct ll_dram_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        struct ll_dram_node *next;
        int* flag;
};
class LinkedlistDram_CAS {
    public:
        LinkedlistDram_CAS(const char *path) {
            cout<<"constructor init"<<endl;
            LinkedlistDram_CAS::init(path);
        }

        int init(const char *path);

        int read(const char *key, void *&result);

        int update(const char *key, void *value);

        int insert(const char *key, void *value);

        void destroy();

        ~LinkedlistDram_CAS();

   // private:
        /* Private Data */
        struct ll_dram_node *head;
        struct ll_dram_node *tail;

        int check();
    };

    int LinkedlistDram_CAS::check() {
        if (head == NULL) {
            fprintf(stderr, "[%s]: FATAL: linkedlist not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    // Check if this is the correct implementation for the init function for a linkedlist
    int LinkedlistDram_CAS::init(const char *path) {
        head = NULL;
        tail = NULL;
        return 1;
    }

    int LinkedlistDram_CAS::read(const char *key, void *&result) {
        //printf("[%s]: key: %s\n", __FUNCTION__, key);
        check();

        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_dram_node *current_node = head;

        while (current_node != NULL) {
            if (current_node->key == uint64_key) {
                result = current_node->value;
                break;
            }
            current_node = current_node->next;
        }

        return 1;
    }

    int LinkedlistDram_CAS::update(const char *key, void *value) {
        //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);
        check();

        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_dram_node *current_node = head;

        while (current_node != NULL) {
            if (current_node->key == uint64_key) {
                strcpy(current_node->value, (const char *) value);
                break;
            }
            current_node = current_node->next;
        }
        return 1;
    }

    int LinkedlistDram_CAS::insert(const char *key, void *value) {
        //printf("[%s]: key: %s, value: %s\n", __FUNCTION__, key, (char *) value);
        //check();
        uint64_t uint64_key = strtoull(key, NULL, 0);
        struct ll_dram_node *new_node = (struct ll_dram_node *) malloc(sizeof(struct ll_dram_node));
        new_node->key = uint64_key;
        new_node->flag = new int(0);
        strcpy(new_node->value, (const char *) value);
        

        if(head == NULL) {
            cout<<"if  cond"<<endl;
            //ll_dram_node* thisnode = head;
            //bool race_result = __sync_bool_compare_and_swap(head,NULL,new_node);
            bool race_result = true;
            if(race_result){
                tail = new_node;
                head = new_node;
                return 1;
            }
            return 0;
        }

        else if(uint64_key < head->key){
            ll_dram_node* prev_head = head;
            bool race_result = __sync_bool_compare_and_swap(head->flag,0,1);
            if(race_result){
                new_node->next = head;
                head = new_node;
                prev_head->flag = new int(0); 
                cout<<endl<<"returned";
                return 1;
            }
            return 0;
        }

        else if(uint64_key > tail->key){
            ll_dram_node* prev_tail = tail;
            bool race_result = __sync_bool_compare_and_swap(tail->flag,0,1);
            if(race_result){
                tail->next = new_node;
                tail = new_node;
                prev_tail->flag = new int(0); 
                cout<<endl<<"returned";
                return 1;
            }
            return 0;
        }
        else {
            ll_dram_node* temp = head;
            while(temp->next!=NULL){
                if(temp->key<=uint64_key&&temp->next->key>=uint64_key){
                    ll_dram_node* next_node = temp->next;
                   cout<<temp->next->flag<<" "<<next_node->flag <<endl;
                   bool race_result = __sync_bool_compare_and_swap(temp->flag,0,1);
                    cout<<race_result<<endl;
                    if(race_result){
                        new_node->next = next_node;
                        temp->next = new_node;
                        temp->flag = new int(0);
                        
                        return 1;
                    }
                }
                temp = temp->next;
            }
            
        }
        cout<<endl;
        cout<<"Failed inserted";
        return 0;
    }

    void LinkedlistDram_CAS::destroy() {
        struct ll_dram_node *current_node;

        if(tail != NULL) free(tail);
        while (head != NULL) {
            current_node = head;
            head = head->next;
            free(current_node);
        }
        pmemobj_close(pop);
    }
