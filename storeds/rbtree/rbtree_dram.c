//
// Created by anarayanan on 10/25/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "rbtree_dram.h"
#include<stdbool.h>
#include<algorithm>

enum Color {RED,BLACK};
const int size = 1000000;
const int default_value_len = 101;
struct Node 
{ 
    uint64_t key; 
    char value[default_value_len];
    struct Node *left, *right, *parent; 
    bool color;
  
    
    // Constructor 
    /*
    Node(int data) 
    { 
       this->data = data; 
       left = right = parent = NULL; 
       this->color = RED; 
    } 
    */
}; 

static struct Node *groot = NULL;



int rbtree_dram_init(const char *path) {
    groot = NULL;
    return 1;
}

int rbtree_dram_check() {
    if (groot == NULL) {
        fprintf(stderr, "[%s]: FATAL: RBTREE not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

bool print_tree_value(struct Node *root,uint64_t key,void *&result)
{
    //printf("\nROOTVAL %d\n",root->value);
    //printf("OUTKEY %d",key);
    //printf("\nINKEY %d",root->key);
    bool ttest = root->key==key;
    if(ttest)
    {
        result = root->value;
        
        
        
        //char* x = "2";
        //result = x;
        return true;

    }
    //result = root->value;
    //return true;
    //printf("\nTTEST FAILED\n");
    
    if(root->left!=NULL)
        if(print_tree_value(root->left,key,result)==true)
            return true;
    if(root->right!=NULL)
        if(print_tree_value(root->right,key,result)==true)
            return true;
    
    return false;
}
bool update_tree_value(struct Node *root,uint64_t key,void* value)
{
    //printf("\nROOTVAL %d\n",root->value);
    //printf("OUTKEY %d",key);
    //printf("\nINKEY %d",root->key);
    bool ttest = root->key==key;
    if(ttest)
    {
        strcpy(root->value, (const char *) value);
        //result = root->value;
        
        
        
        //char* x = "2";
        //result = x;
        return true;

    }
    //result = root->value;
    //return true;
    //printf("\nTTEST FAILED\n");
    
    if(root->left!=NULL)
        if(update_tree_value(root->left,key,value)==true)
            return true;
    if(root->right!=NULL)
        if(update_tree_value(root->right,key,value)==true)
            return true;
    
    return false;
}
int rbtree_dram_read(const char* key, void *&result) {
    //printf("RB READ INIT");
    uint64_t uint64_key = strtoull(key, NULL, 0);
    //printf("REACH KEY PRINT");
    rbtree_dram_check();
    struct Node* cur = groot;
    //printf("KEY %d",uint64_key);
    //har* ret = "2";
    //result = "2";
    //result = ret;
    print_tree_value(cur,uint64_key,result);
    

    return 1;
}


void rotateLeft(struct Node *root, struct Node *pt) 
{ 
    struct Node *pt_right = pt->right; 
  
    pt->right = pt_right->left; 
  
    if (pt->right != NULL) 
        pt->right->parent = pt; 
  
    pt_right->parent = pt->parent; 
  
    if (pt->parent == NULL) 
        root = pt_right; 

    else if (pt == pt->parent->left) 
    {
        pt->parent->left = pt_right; 
    }
  
    else
    {
        pt->parent->right = pt_right; 
    }
  
    pt_right->left = pt; 
    pt->parent = pt_right; 
} 

void rotateRight(struct Node *root, struct Node *pt) 
{ 
    struct Node *pt_left = pt->left; 
  
    pt->left = pt_left->right; 
  
    if (pt->left != NULL) 
        pt->left->parent = pt; 
  
    pt_left->parent = pt->parent; 
  
    if (pt->parent == NULL) 
        root = pt_left; 
  
    else if (pt == pt->parent->left) 
        pt->parent->left = pt_left; 
  
    else
        pt->parent->right = pt_left; 
  
    pt_left->right = pt; 
    pt->parent = pt_left; 
} 


struct Node* BSTInsert(struct Node* root, struct Node *pt) 
{ 
    //printf("\n INSERTING %d \n",pt->key);
    //printf("\nINSERTING VALUE %s",pt->value);
    //return root;
    /* If the tree is empty, return a new nod e */
    if (root == NULL) 
    {
       //printf("\n INSERTING %d \n",pt->key);
       //printf("\nINSERTING VALUE %d",pt->value);

       return pt; 
    }
  
    /* Otherwise, recur down the tree */
    if (pt->key < root->key) 
    { 
        
        root->left  = BSTInsert(root->left, pt); 
        //root->left=pt;
        root->left->parent = root; 
    } 
    else if (pt->key > root->key) 
    { 
        root->right = BSTInsert(root->right, pt); 
        root->right->parent = root; 
    } 
  
    /* return the (unchanged) node pointer */
    return root; 
} 

/*
int BSTInsertnew(struct Node *pt)
{
    printf("\n%d",pt->key);
    printf("\n%s",pt->value);
    if(groot==NULL)
        groot=pt;
    
    else
    {
        printf("\nREACHES ELSE");
        struct Node cur = groot;
        int last = 0;
        bool done = false;
        while(!done)
        {
            if(pt->key<cur->key)
            {
                if(cur->left!=NULL)
                    cur = cur->left;
                else
                {
                    cur->left = pt;
                    done=true;
                }
                
                
            }
            else
            {
                if(cur->left!=NULL)
                    cur = cur->right;
                else
                {
                    cur->right = pt;
                    done=true;
                }
                
            }
            
        }
    }


    return 0;

}
*/


void fixViolation(struct Node *root, struct Node *pt) 
{ 
    struct Node *parent_pt = NULL; 
    struct Node *grand_parent_pt = NULL; 
  
    while ((pt != root) && (pt->color != BLACK) && 
           (pt->parent->color == RED)) 
    { 
  
        parent_pt = pt->parent; 
        grand_parent_pt = pt->parent->parent; 
  
        /*  Case : A 
            Parent of pt is left child of Grand-parent of pt */
        if (parent_pt == grand_parent_pt->left) 
        { 
  
            struct Node *uncle_pt = grand_parent_pt->right; 
  
            /* Case : 1 
               The uncle of pt is also red 
               Only Recoloring required */
            if (uncle_pt != NULL && uncle_pt->color == RED) 
            { 
                grand_parent_pt->color = RED; 
                parent_pt->color = BLACK; 
                uncle_pt->color = BLACK; 
                pt = grand_parent_pt; 
            } 
  
            else
            { 
                /* Case : 2 
                   pt is right child of its parent 
                   Left-rotation required */
                if (pt == parent_pt->right) 
                { 
                    rotateLeft(root, parent_pt); 
                    pt = parent_pt; 
                    parent_pt = pt->parent; 
                } 
  
                /* Case : 3 
                   pt is left child of its parent 
                   Right-rotation required */
                rotateRight(root, grand_parent_pt); 
                std::swap(parent_pt->color, grand_parent_pt->color); 
                pt = parent_pt; 
            } 
        } 
  
        /* Case : B 
           Parent of pt is right child of Grand-parent of pt */
        else
        { 
            struct Node *uncle_pt = grand_parent_pt->left; 
  
            /*  Case : 1 
                The uncle of pt is also red 
                Only Recoloring required */
            if ((uncle_pt != NULL) && (uncle_pt->color == RED)) 
            { 
                grand_parent_pt->color = RED; 
                parent_pt->color = BLACK; 
                uncle_pt->color = BLACK; 
                pt = grand_parent_pt; 
            } 
            else
            { 
                /* Case : 2 
                   pt is left child of its parent 
                   Right-rotation required */
                if (pt == parent_pt->left) 
                { 
                    rotateRight(root, parent_pt); 
                    pt = parent_pt; 
                    parent_pt = pt->parent; 
                } 
  
                /* Case : 3 
                   pt is right child of its parent 
                   Left-rotation required */
                rotateLeft(root, grand_parent_pt); 
                std::swap(parent_pt->color, grand_parent_pt->color); 
                pt = parent_pt; 
            } 
        } 
    } 
  
    root->color = BLACK; 
} 







int rbtree_dram_update(const char *key, void *value) {

    //printf("CALLED");
    uint64_t uint64_key = strtoull(key, NULL, 0);
    update_tree_value(groot,uint64_key,value);
    //printf("\n%d",groot->key);
    //printf("\n%s",groot->value);
    return 1;
}

int rbtree_dram_insert(const char *key, void *value) {
    uint64_t uint64_key = strtoull(key,NULL,0);
    struct Node *pt = (struct Node *)malloc(sizeof(struct Node)); //new Node
    pt->left = pt->right=pt->parent = NULL;
    pt->color = RED;
    pt->key = uint64_key;
    strcpy(pt->value,(const char *)value);

    
    
  
    // Do a normal BST insert 
    groot = BSTInsert(groot,pt); 
    
    fixViolation(groot,pt);

  
    
    
    return 1;
}

void rbtree_dram_free() {
}