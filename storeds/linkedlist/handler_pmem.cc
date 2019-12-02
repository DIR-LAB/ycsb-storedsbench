//
// Created by Anirudh on 11/27/2019.
//

#include<iostream>
#include "linkedlist_pmem_concurrent_lockfree.cc"
#include "linkedlist_common.h"
#include<pthread.h>
#include<vector>


using namespace std;

const char* path = "pth";
LinkedlistPmem_CAS* ll = new LinkedlistPmem_CAS(path);
struct thread_args{
    char* key;
    char* value;
    char* value2;
};
void* insert_ll(void* targs){
    char* key = ((struct thread_args *)targs)->key;
    char* value = ((struct thread_args *)targs)->value;
    cout<<key<<" "<<value<<" "<<endl;
    while(ll->insert(key,value)!=1){
        
    }
}
int main(){
    //LinkedlistDram_CAS ll1("");
    int numinsert = 3;
    pthread_t thread_nums[numinsert];
    char* f1 = "1";
    char* f2 = "123";
    //vector<vector<char*> > inserts;
    //inserts.push_back({"1","123"});
    //inserts.push_back({"20","223"});
    //inserts.push_back({"10","123"});
    char* inserts[3][2] = { {"100","123"}, {"40","123"}, {"60","123"}}; 
    //ll->insert(f1,f2);
    //insert_ll(f1,f2);
    f1 = "20";
    f2 = "234";
    struct thread_args* targs[3];
    //targs->value = f2;
    //targs->key = f1;
    //ll->insert(f1,f2);
    
    //cout<<targs->key<<" "<<targs->value<<endl;
    targs[0] =  (struct thread_args *)malloc(sizeof(struct thread_args));
    targs[0]->key = inserts[0][0];
    targs[0]->value = inserts[0][1];
    insert_ll((void*)targs[0]);
    for(int i=1;i<3;i++){
        targs[i] =  (struct thread_args *)malloc(sizeof(struct thread_args));
        
        targs[i]->value = inserts[i][1];
        targs[i]->key = inserts[i][0];
        pthread_create(&thread_nums[i],NULL,insert_ll,(void*)targs[i]);
    }

    for(int i=1;i<3;i++){
        pthread_join(thread_nums[i],NULL);
    }
    /*
    f1 = "10";
    f2 = "234";
    insert_ll(f1,f2);
    f1 = "8";
    f2 = "234";
    insert_ll(f1,f2);
    */
    ll_pmem_node* head = (struct ll_pmem_node *)pmemobj_direct(ll->root_p->head);
    cout<<endl;
    while(head!=NULL){
        cout<<head->key<<endl;
	head = (struct ll_pmem_node *)pmemobj_direct(head->next);
    }
    cout<<"inserted";
    
   
    //ll.insert("2","123");
    return 1;
}
