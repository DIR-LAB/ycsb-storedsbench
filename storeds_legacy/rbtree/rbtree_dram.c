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

enum rbtree_color {RED,BLACK};
const int rbtree_default_value_len = 101;
struct rbtree_entry 
{ 
    uint64_t key; 
    char value[rbtree_default_value_len];
    struct rbtree_entry *left, *right, *parent; 
    bool color;
  
}; 

static struct rbtree_entry *groot = NULL;


/*
 * Initialize root of tree to NULL
 */
int rbtree_dram_init(const char *path) {
    groot = NULL;
    return 1;
}

/*
 * Check Whether RBTree is initialized or not
 */
int rbtree_dram_check() {
    if (groot == NULL) {
        fprintf(stderr, "[%s]: FATAL: RBTREE not initialized yet\n", __FUNCTION__);
        assert(0);
    }
    return 1;
}

/*
 * Read and return Value for a given Key of RBTree
 */
bool print_tree_value(struct rbtree_entry *root,uint64_t key,void *&result)
{
    bool ttest = root->key==key;
    if(ttest)
    {
        result = root->value;
        return true;
    }
    if(root->left!=NULL)
    {
        if(print_tree_value(root->left,key,result)==true)
        {
            return true;
        }
    }
    if(root->right!=NULL)
    {
        if(print_tree_value(root->right,key,result)==true)
        {
            return true;
        }
    }
    return false;
    
}

/*
 * Read and update Value for a given Key of RBTree
 */
bool update_tree_value(struct rbtree_entry *root,uint64_t key,void* value)
{
    bool ttest = root->key==key;
    if(ttest)
    {
        strcpy(root->value, (const char *) value);
     
        return true;

    }
    
    if(root->left!=NULL)
        if(update_tree_value(root->left,key,value)==true)
            return true;
    if(root->right!=NULL)
        if(update_tree_value(root->right,key,value)==true)
            return true;
    
    return false;
}

/*
 * Read RBTree DRAM and return value into the result variable.
 */
int rbtree_dram_read(const char* key, void *&result) {
    uint64_t uint64_key = strtoull(key, NULL, 0);
    rbtree_dram_check();
    struct rbtree_entry* cur = groot;
    print_tree_value(cur,uint64_key,result);
    return 1;
}

/*
 * Rotate Sub-Tree of RBTree to the Left
 */
void rotateLeft(struct rbtree_entry *root, struct rbtree_entry *pt) 
{ 
    struct rbtree_entry *pt_right = pt->right;   
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

/*
 * Rotate Sub-Tree of RBTree to right
 */
void rotateRight(struct rbtree_entry *root, struct rbtree_entry *pt) 
{ 
    struct rbtree_entry *pt_left = pt->left; 
    pt->left = pt_left->right; 
    if (pt->left != NULL) 
    {
        pt->left->parent = pt; 
    }
    pt_left->parent = pt->parent; 
    if (pt->parent == NULL) 
    {
        root = pt_left; 
    }
    else if (pt == pt->parent->left) 
    {
        pt->parent->left = pt_left; 
    }
    else
    {
        pt->parent->right = pt_left; 
    }
    pt_left->right = pt; 
    pt->parent = pt_left; 
} 


/*
 * Insert into Binary Search Tree, before doing fix to balance to make RBTree.
 */
struct rbtree_entry* BSTInsert(struct rbtree_entry* root, struct rbtree_entry *pt) 
{ 
    if (root == NULL) 
    {
       return pt; 
    }
  
    /* Otherwise, recur down the tree */
    if (pt->key < root->key) 
    {     
        root->left  = BSTInsert(root->left, pt); 
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
 * Rebalance RB-Tree. This operation can be done in relaxed or active manner.
 */
void fixViolation(struct rbtree_entry *root, struct rbtree_entry *pt) 
{ 
    struct rbtree_entry *parent_pt = NULL; 
    struct rbtree_entry *grand_parent_pt = NULL; 
  
    while ((pt != root) && (pt->color != BLACK) && 
           (pt->parent->color == RED)) 
    { 
  
        parent_pt = pt->parent; 
        grand_parent_pt = pt->parent->parent; 
  
        /*  Case : A 
            Parent of pt is left child of Grand-parent of pt */
        if (parent_pt == grand_parent_pt->left) 
        { 
  
            struct rbtree_entry *uncle_pt = grand_parent_pt->right; 
  
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
            struct rbtree_entry *uncle_pt = grand_parent_pt->left; 
  
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






/*
 * Read and Update Value for a given Key of RB-Tree
 */
int rbtree_dram_update(const char *key, void *value) {
    uint64_t uint64_key = strtoull(key, NULL, 0);
    update_tree_value(groot,uint64_key,value);
    return 1;
}

/*
 * Insert Value into RBTree
 */
int rbtree_dram_insert(const char *key, void *value) {
    uint64_t uint64_key = strtoull(key,NULL,0);
    struct rbtree_entry *pt = (struct rbtree_entry *)malloc(sizeof(struct rbtree_entry)); //new Node
    pt->left = pt->right=pt->parent = NULL;
    pt->color = RED;
    pt->key = uint64_key;
    strcpy(pt->value,(const char *)value);
    // Do a normal BST insert 
    groot = BSTInsert(groot,pt); 
    fixViolation(groot,pt);
    return 1;
}

/*
 * Free Space of RBTree.
 */
void rbtree_dram_free() {
}
