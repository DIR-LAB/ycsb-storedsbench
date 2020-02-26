//
// Created by Anirudh on 11/27/2019.
//

#include<iostream>
#include "rbtree_dram_concurrent_lockfree.cc"
#include "rbtree_common.h"
#include<pthread.h>
#include<vector>
#include <chrono>
using namespace std::chrono;


using namespace std;

const char* path = "pth";
RbtreeDram_Concurrent* ll = new RbtreeDram_Concurrent(path);
struct thread_args{
    int key;
    char* value;
    char* value2;
};
void* insert_ll(void* targs){
    int key = ((struct thread_args *)targs)->key;
    char* value = ((struct thread_args *)targs)->value;
    cout<<key<<" "<<value<<" "<<endl;
    while(ll->insert(key,value)!=1){
        
    }
}
int main(){
    //LinkedlistDram_CAS ll1("");
    cout<<"start main";
    int numinsert = 4;
    pthread_t thread_nums[5];
    char* f1 = "1";
    char* f2 = "123";
    //vector<vector<char*> > inserts;
    //inserts.push_back({"1","123"});
    //inserts.push_back({"20","223"});
    //inserts.push_back({"10","123"});
    char* inserts[5][2] = { {"30","123"}, {"40","123"}, {"1000","123"},{"1299","11"},{"1399","11"}}; 
    //ll->insert(f1,f2);
    //insert_ll(f1,f2);
    f1 = "20";
    f2 = "234";
    struct thread_args* targs[20];
    //targs->value = f2;
    //targs->key = f1;
    //ll->insert(f1,f2);
    
    auto start = high_resolution_clock::now();

    
    //cout<<targs->key<<" "<<targs->value<<endl;
    targs[0] =  (struct thread_args *)malloc(sizeof(struct thread_args));
    targs[0]->key = 20;
    targs[0]->value = inserts[0][1];
    insert_ll((void*)targs[0]);
    for(int i=1;i<10;i++){
        targs[i] =  (struct thread_args *)malloc(sizeof(struct thread_args));
        
        targs[i]->value = "123";
        targs[i]->key = i;
        ll->levelorder();
        pthread_create(&thread_nums[i-1],NULL,insert_ll,(void*)targs[i]);
         pthread_join(thread_nums[i-1],NULL);
    }
    //cout<<"ended insert"<<endl;
    for(int i=1;i<100;i++){
       // pthread_join(thread_nums[i-1],NULL);
    }
    cout<<"started levelorder"<<endl;
    //cout<"levelorder";
    ll->levelorder();

    auto stop = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(stop - start);

    cout << duration.count() << endl;
   
    //ll.insert("2","123");
    return 1;
}
