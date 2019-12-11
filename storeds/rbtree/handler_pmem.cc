//
// Created by Anirudh on 11/27/2019.
//

#include<iostream>
#include "rbtree_pmem_concurrent_lockfree.cc"
#include "rbtree_common.h"
#include<pthread.h>
#include<vector>


using namespace std;

const char* path = "/pmem/pth";
RbtreePmem_Concurrent* ll = new RbtreePmem_Concurrent(path);
struct thread_args{
    char* key;
    char* value;
    char* value2;
};
void* insert_ll(void* targs){
    char* key = ((struct thread_args *)targs)->key;
    char* value = ((struct thread_args *)targs)->value;
    cout<<endl<<key<<" "<<value<<" "<<endl;
    while(ll->insert(strtoull(key,NULL,0),value)!=1){
        
    }
}
int main(){
    //LinkedlistDram_CAS ll1("");
    cout<<"start main";
    int numinsert = 4;
    pthread_t thread_nums[numinsert];
    char* f1 = "1";
    char* f2 = "123";
    //vector<vector<char*> > inserts;
    //inserts.push_back({"1","123"});
    //inserts.push_back({"20","223"});
    //inserts.push_back({"10","123"});
    char* inserts[5][2] = { {"30","123"}, {"40","123"}, {"1000","123"},{"200","11"},{"1399","11"}}; 
    //ll->insert(f1,f2);
    //insert_ll(f1,f2);
    f1 = "20";
    f2 = "234";
    struct thread_args* targs[4];
    //targs->value = f2;
    //targs->key = f1;
    //ll->insert(f1,f2);
    
    //cout<<targs->key<<" "<<targs->value<<endl;
    targs[0] =  (struct thread_args *)malloc(sizeof(struct thread_args));
    targs[0]->key = inserts[0][0];
    targs[0]->value = inserts[0][1];
    insert_ll((void*)targs[0]);
    
    

    for(int i=1;i<5;i++){
        targs[i] =  (struct thread_args *)malloc(sizeof(struct thread_args));
        
        targs[i]->value = inserts[i][1];
        targs[i]->key = inserts[i][0];
        ll->levelorder();
        pthread_create(&thread_nums[i-1],NULL,insert_ll,(void*)targs[i]);
        pthread_join(thread_nums[i-1],NULL);
    }
    //cout<<"ended insert"<<endl;
    for(int i=1;i<5;i++){
        //pthread_join(thread_nums[i],NULL);
    }
    cout<<"started levelorder"<<endl;
    //cout<"levelorder";
    ll->levelorder();
   
    //ll.insert("2","123");
    return 1;
}
