//
// Created by Anirudh on 11/27/2019.
//

#include<iostream>
#include "rbtree_pmem_concurrent_lockfree.cc"
#include "rbtree_common.h"
#include<pthread.h>
#include<vector>
#include <chrono>
#include<stdlib.h>
using namespace std::chrono;


using namespace std;

// size of array 
#define MAX 4 
  
// maximum number of threads 
#define MAX_THREAD 20

// int a[] = { 1111, 1105, 1077, 1010, 912, 814, 715, 618, 520, 422, 325, 227, 130, 64, 110, 220, 1, 5, 7, 10, 12, 14, 15, 18, 20, 22, 25, 27, 30, 64, 110, 220 }; 
int a[] = {1111, 1105, 1077, 1010, 912, 814, 715, 618, 520, 422, 325, 227, 130, 64, 110, 220, 1, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 }; 

//int a[] = { 1111, 1105, 1077, 1010, 912, 814, 715, 618, 520, 422, 325, 227, 130, 64, 110, 220 }; 
int sum[4] = { 0 }; 
int part = 0; 

const char* path = "/pmem/pth";
RbtreePmem_Concurrent* ll = new RbtreePmem_Concurrent(path);

void* sum_array(void* arg) 
{ 
  
    // Each thread computes sum of 1/4th of array 
    int thread_part = part++; 
    
    for (int i = thread_part * (MAX / 4); i < (thread_part + 1) * (MAX / 4); i++) 
    { 
        int ranm = random()%10;
        string abc = to_string(ranm);
        while(ll->insert(a[i],(char*)abc.c_str())!=1){
        
        }
    }
} 
  
// Driver Code 
int main() 
{ 
    pthread_t threads[MAX_THREAD]; 
    ll->insert(0,(char *)"5");
    // Creating 4 threads 
    for (int i = 0; i < MAX_THREAD; i++) 
    {
        pthread_create(&threads[i], NULL, sum_array, (void*)NULL); 
        //    pthread_join(threads[i], NULL); 
    }
  
    // joining 4 threads i.e. waiting for all 4 threads to complete 
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL); 

    }
     
  
    // adding sum of all 4 parts 
    ll->levelorder();
  
    return 0; 
} 