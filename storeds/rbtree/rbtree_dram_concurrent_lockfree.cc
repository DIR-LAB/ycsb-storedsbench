//
// Created by anarayanan on 10/25/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include<stdbool.h>
#include "rbtree_common.h"
#include<bits/stdc++.h>
#include<vector>
#include "libatomic_ops/src/atomic_ops.h"

using namespace std;



    #define PMEM_RB_POOL_SIZE ((size_t) (1 << 30))

    /* name of layout in the pool */
    #define RB_LAYOUT_NAME "rbtree_layout"

    /* default length for value */
    #define DEFAULT_VALUE_LEN 128

    /**
     * rbtree_color -- rbtree color list
     */
    enum rbtree_color {
        RED,
        BLACK
    };

    /* declaration of dram data-structures */
    struct rbtree_dram_node {
        uint64_t key;
        char value[DEFAULT_VALUE_LEN];
        struct rbtree_dram_node *left;
        struct rbtree_dram_node *right;
        struct rbtree_dram_node *parent;
        bool color;
        int* flag;
        char padding[7];
        //rbtree_dram_node():flag(new int(0)){};
    } __attribute__ ((aligned (8)));


class RbtreeDram_Concurrent {
    public:
        RbtreeDram_Concurrent(const char *path) {
            RbtreeDram_Concurrent::init(path);
        }

        int init(const char *path);

        int read(const uint64_t key, void *&result);

        int update(const uint64_t key, void *value);

        int insert(const uint64_t key, void *value);

        void levelorder();

        void destroy();

        ~RbtreeDram_Concurrent();

    private:
        /* Private Data */
        struct rbtree_dram_node *root_p = NULL;

        int check();

        void lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result);

        void rotate_left(struct rbtree_dram_node *&ptr);

        void rotate_right(struct rbtree_dram_node *&ptr);

        struct rbtree_dram_node *bst_upsert(struct rbtree_dram_node *current_node, uint64_t key, void *value);

        void fix_violation(struct rbtree_dram_node *&current_node);

        void free_node(rbtree_dram_node *&current_node);

        struct rbtree_dram_node *create_new_node(uint64_t key, void *value);

        bool setup_local_area_for_insert(rbtree_dram_node* x,vector<rbtree_dram_node*>& free_nodes);

        struct rbtree_dram_node* move_local_upward(rbtree_dram_node* x, vector<rbtree_dram_node*>& working);
    };

    /**
     * RbtreeDram_Concurrent::init -- initialize root of tree to NULL
     */
    int RbtreeDram_Concurrent::init(const char *path) {
        root_p = NULL;
        return 1;
    }

    /**
     * RbtreeDram_Concurrent::check -- (internal) check whether rb-tree is initialized or not
     */
    int RbtreeDram_Concurrent::check() {
        if (root_p == NULL) {
            fprintf(stderr, "[%s]: FATAL: RBTREE not initialized yet\n", __FUNCTION__);
            assert(0);
        }
        return 1;
    }

    /**
     * RbtreeDram_Concurrent::lookup -- (internal) Read and return Value for a given Key of RBTree
     */
    void RbtreeDram_Concurrent::lookup(struct rbtree_dram_node *current_node, uint64_t key, void *&result) {
        //base case
        if (current_node == NULL) {
            return;
        }

        if (current_node->key == key) {
            //data found
            result = current_node->value;
            return;
        }

        if (current_node->key > key) {
            return lookup(current_node->left, key, result);
        } else {
            return lookup(current_node->right, key, result);
        }
    }

    /**
     * RbtreeDram_Concurrent::read -- Read RBTree DRAM and return value into the result variable.
     */
    int RbtreeDram_Concurrent::read(const uint64_t key, void *&result) {
        check();
        //uint64_t uint64_key = strtoull(key, NULL, 0);
        lookup(root_p, key, result);

        return 1;
    }

    /**
     * RbtreeDram_Concurrent::update -- if key exists, update <key, value> pair. if key not exist, insert <key, value pair>
     */
    int RbtreeDram_Concurrent::update(const uint64_t key, void *value) {
        check();
        insert(key, value);
        return 1;
    }

    /**
     * RbtreeDram_Concurrent::rotate_left -- (internal) Rotate Sub-Tree of RBTree to the Left
     */
    void RbtreeDram_Concurrent::rotate_left(struct rbtree_dram_node *&ptr) {
        struct rbtree_dram_node *right_p = ptr->right;
        ptr->right = right_p->left;
        if (ptr->right != NULL) {
            ptr->right->parent = ptr;
        }
        right_p->parent = ptr->parent;

        if (ptr->parent == NULL) {
            root_p = right_p;
        }
        else if (ptr == ptr->parent->left) {
            ptr->parent->left = right_p;
        }
        else {
            ptr->parent->right = right_p;
        }
        right_p->left = ptr;
        ptr->parent = right_p;
    }

    /**
     * RbtreeDram_Concurrent::rotate_right -- (internal) Rotate Sub-Tree of RBTree to right
     */
    void RbtreeDram_Concurrent::rotate_right(struct rbtree_dram_node *&ptr) {
        struct rbtree_dram_node *left_p = ptr->left;
        ptr->left = left_p->right;
        if (ptr->left != NULL) {
            ptr->left->parent = ptr;
        }
        left_p->parent = ptr->parent;

        if (ptr->parent == NULL) {
            root_p = left_p;
        }
        else if (ptr == ptr->parent->left) {
            ptr->parent->left = left_p;
        }
        else {
            ptr->parent->right = left_p;
        }
        left_p->right = ptr;
        ptr->parent = left_p;
    }

    /**
     * RbtreeDram_Concurrent::bst_upsert -- (internal) update the value if key already exist and return NULL as a signature of update
     * if the key not exist, insert a new node as like as normal unbalanced bst and return the newly inserted node
     * will update the balance in later scope
     */
    struct rbtree_dram_node *RbtreeDram_Concurrent::bst_upsert(struct rbtree_dram_node *current_node, uint64_t key, void *value) {
        while (current_node != NULL) {
            if (key == current_node->key) {
                //key already exist, update the value and return
                memcpy(current_node->value, (char *) value, strlen((char *) value) + 1);

                //return status: updated!
                return NULL;
            } else if (key < current_node->key) {
                if (current_node->left == NULL) {
                    struct rbtree_dram_node *new_node = create_new_node(key, value);
                    current_node->left = new_node;
                    new_node->parent = current_node;

                    //return status: inserted!
                    return new_node;
                } else {
                    current_node = current_node->left;
                }
            } else {
                if (current_node->right == NULL) {
                    struct rbtree_dram_node *new_node = create_new_node(key, value);
                    current_node->right = new_node;
                    new_node->parent = current_node;

                    //return status: inserted!
                    return new_node;
                } else {
                    current_node = current_node->right;
                }
            }
        }

        fprintf(stderr, "[%s]: FATAL: failed to insert or update!\n", __FUNCTION__);
        assert(0);
    }

    /**
     * RbtreeDram_Concurrent::fix_violation -- (internal) Re-balance RB-Tree.
     */
    void RbtreeDram_Concurrent::fix_violation(struct rbtree_dram_node *&current_node) {
        struct rbtree_dram_node *parent_pt = NULL;
        struct rbtree_dram_node *grand_parent_pt = NULL;
        struct rbtree_dram_node *uncle_pt = NULL;
        vector<rbtree_dram_node*> local_area;

        while ((current_node != root_p) && (current_node->color != BLACK) && (current_node->parent->color == RED)) {
            //cout<<"loop iter1"<<endl;
            parent_pt = current_node->parent;
            grand_parent_pt = current_node->parent->parent;
            local_area.push_back(current_node);
            local_area.push_back(parent_pt);
            local_area.push_back(grand_parent_pt);

            /* case: parent is left child of grandparent node */
            if (parent_pt == grand_parent_pt->left) {
               uncle_pt = grand_parent_pt->right;

               local_area.push_back(uncle_pt);

                /* case: uncle is also red, only recoloring is required */
                if (uncle_pt != NULL && uncle_pt->color == RED) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    move_local_upward(current_node,local_area);
                    
                    current_node = grand_parent_pt;
                }
                else {
                    /* case: current_node is right child of its parent -> left-rotation required */
                    if (current_node == parent_pt->right) {
                        rotate_left(parent_pt);
                        current_node = parent_pt;
                        parent_pt = current_node->parent;
                    }
                    /* case: current_node is now become the left child of its parent -> right-rotation required */
                    rotate_right(grand_parent_pt);

                    bool parent_color = parent_pt->color;
                    parent_pt->color = grand_parent_pt->color;
                    grand_parent_pt->color = parent_color;

                    current_node = parent_pt;
                }
            }

            /* case: parent is right child of grandparent node */
            else {
                uncle_pt = grand_parent_pt->left;

                local_area.push_back(uncle_pt);
                /* case: uncle is also red, only recoloring is required */
                if ((uncle_pt != NULL) && (uncle_pt->color == RED)) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    move_local_upward(current_node,local_area);
                    current_node = grand_parent_pt;
                }
                else {
                    /* case: current_node is left child of its parent -> right-rotation required */
                    if (current_node == parent_pt->left) {
                        rotate_right(parent_pt);
                        current_node = parent_pt;
                        parent_pt = current_node->parent;
                    }

                    /* case: current_node is right child of its parent -> left-rotation required */
                    rotate_left(grand_parent_pt);

                    bool parent_color = parent_pt->color;
                    parent_pt->color = grand_parent_pt->color;
                    grand_parent_pt->color = parent_color;

                    current_node = parent_pt;
                }
            }
        }

        root_p->color = BLACK;
        for(int i=0;i<local_area.size();i++){
            if(local_area[i]!=NULL){
                local_area[i]->flag = new int(0);
            }
        }
    }

    /**
     * RbtreeDram_Concurrent::create_new_node -- (internal) allocate memory for new node
     */
    struct rbtree_dram_node *RbtreeDram_Concurrent::create_new_node(uint64_t key, void *value) {
        //allocate memory to new node
        struct rbtree_dram_node *new_node = (struct rbtree_dram_node *) malloc(sizeof(struct rbtree_dram_node));

        new_node->left = new_node->right = new_node->parent = NULL;
        new_node->color = RED;
        new_node->key = key;
        strcpy(new_node->value, (char *) value);

        return new_node;
    }

    /**
     * RbtreeDram_Concurrent::insert -- if key not exist, insert <key, value pair>. if key exists, update <key, value> pair.
     */
    int RbtreeDram_Concurrent::insert(const uint64_t key, void *value) {
        //printf("[%s]: PARAM: key: %s, value: %s\n", __func__, key, (char *) value);
        //uint64_t uint64_key = strtoull(key, NULL, 0);

        //root is NULL, insert to root node
        if (root_p == NULL) {
            root_p = create_new_node(key, value);
            root_p->flag = new int(0);

            //fix violation will update the color
            fix_violation(root_p);
            return 1;
        }

        // Do a normal BST insert
        
        struct rbtree_dram_node *new_node = create_new_node(key, value);
        new_node->flag = new int(0);
        new_node->left = NULL;
        new_node->right = NULL;
        new_node->parent= NULL;
        struct rbtree_dram_node* temp1,*temp2;
        vector<rbtree_dram_node*> free_nodes;
        //cout<<endl<<"before viofix"<<endl;
        new_node->flag = new int(1);
        while(true){
            temp1 = root_p;
            temp2 = NULL;
            while(temp1!=NULL){
                //cout<<"entersecond loop"<<endl;
                //cout<<endl<<temp1->key;
                temp2 = temp1;
                if(key<temp1->key){
                    temp1 = temp1->left;
                    //cout<<"goleft"<<endl;
                }
                else{
                    temp1 = temp1->right;
                    //if(temp1==NULL)
                    //    break;

                    //cout<<"goright"<<endl;

                }
                //cout<<"temp3NULL"<<" ";
                ////cout<<(temp1==NULL);
            }
            //cout<<"escape loop"<<endl;
            if(!setup_local_area_for_insert(temp2,free_nodes)){
                //cout<<"enter setuplocal area"<<endl;
                temp2->flag = new int(0);
            }
            else{
                break;
            }
            
        }
        //cout<<"escape big loop";
        new_node->parent = temp2;
        if(temp2==NULL){
            root_p = new_node;
        }
        else if(key<temp2->key){
            temp2->left = new_node;
        }
        else{
            temp2->right = new_node;
        }
 
        if(new_node->left!=NULL){
            new_node->left->parent = new_node;
        }

        if(new_node->right!=NULL){
            new_node->right->parent = new_node;
        }
        new_node->color = RED;
        //struct rbtree_dram_node *new_node = bst_upsert(root_p, key, value);
        //cout<<endl<<"LOCKED NODES, FIXING VIOL";
       
        
        //cout<<endl<<"LEVELORDER"<<endl;
        //levelorder();
        //cout<<endl<<"endlevelorder"<<endl;
        if (new_node) fix_violation(new_node);
        for(int i=0;i<free_nodes.size();i++){
            //cout<<"freebef";
            //free_nodes[i]->flag = new int(0);
            //cout<<"freeaf";
        }
        ////cout<<(free_nodes[0]==NULL);
        //cout<<endl<<"freenodessize"<<free_nodes.size()<<endl;
        //cout<<"DONE VIOL"<<endl;
        return 1;
    }

    /**
     * RbtreeDram_Concurrent::free_node -- (internal) recursively free the tree node's memory
     */
    void RbtreeDram_Concurrent::free_node(rbtree_dram_node *&current) {
        if (current != NULL) {
            free_node(current->left);
            free_node(current->right);

            free(current);
        }
    }

   bool RbtreeDram_Concurrent::setup_local_area_for_insert(rbtree_dram_node* x,vector<rbtree_dram_node*>& free_nodes) {
    if(x==NULL){
        return true;
    }

    rbtree_dram_node* this_ptr = x;
    rbtree_dram_node* parent_ptr = this_ptr->parent;
    rbtree_dram_node* uncle_ptr;
    if(parent_ptr ==NULL){
        return true;
    }
    //this_ptr->flag = new int(0);
    //bool race_result = __sync_bool_compare_and_swap(this_ptr->flag,0,1);
    
    ////cout<<" "<<__sync_bool_compare_and_swap(this_ptr->flag,0,1)<<endl;

    //cout<<"fails at this ptr"<<endl; 
    if(!__sync_bool_compare_and_swap(this_ptr->flag,0,1)){
        return false;
    }

    //cout<<"fails at this parent ptr"<<endl; 
    if(!__sync_bool_compare_and_swap(parent_ptr->flag, 0,1)){
        return false;
    }

    if(parent_ptr!= this_ptr->parent){
        parent_ptr->flag = new int(0);
        return false;
    }

    if(this_ptr == parent_ptr->left){
        uncle_ptr = parent_ptr->right;
    }

    else{
        uncle_ptr = parent_ptr->left;
    }
    //cout<<endl<<"before uncle"<<endl;

    //cout<<"fails at this uncle ptr"<<endl; 
    if(uncle_ptr!=NULL && !__sync_bool_compare_and_swap(uncle_ptr->flag,0,1)){
        rbtree_dram_node* now_parent_ptr = this_ptr->parent;
        now_parent_ptr->flag=new int(0);
        return false;
    }

    free_nodes.push_back(this_ptr);
    free_nodes.push_back(parent_ptr);
    free_nodes.push_back(uncle_ptr);
    return true;


}

   rbtree_dram_node*  RbtreeDram_Concurrent::move_local_upward(rbtree_dram_node* x, vector<rbtree_dram_node*>& working){
    rbtree_dram_node* parent_ptr = x->parent;
    rbtree_dram_node* grandparent_ptr = parent_ptr->parent;
    rbtree_dram_node* uncle_ptr;
    if(parent_ptr == grandparent_ptr->left){
        uncle_ptr = grandparent_ptr->right;
    }
    else{
        uncle_ptr = grandparent_ptr->left;
    }

    rbtree_dram_node* updated_this, *updated_parent = NULL, *updated_uncle = NULL, *updated_grandparent = NULL;
    updated_this = grandparent_ptr;


    while(updated_this->parent!=NULL){
        updated_parent = updated_this->parent;
        if(!__sync_bool_compare_and_swap(updated_parent->flag,0,1)){
            continue;
        }
        updated_grandparent = updated_parent->parent;
        if(updated_grandparent==NULL){
            break;
        }
        if(!__sync_bool_compare_and_swap(updated_grandparent->flag,0,1)){
            updated_parent->flag = new int(0);
            continue;
        }

        if(updated_parent == updated_grandparent->left){
            updated_uncle = updated_grandparent->right;
        }
        else{
            updated_uncle = updated_grandparent->left;
        }

        if(updated_uncle!=NULL && __sync_bool_compare_and_swap(updated_uncle->flag,0,1)){
            updated_grandparent->flag = new int(0);
            updated_parent->flag = new int(0);
            continue;
        }
        break;
    }

    working.push_back(updated_this);
    working.push_back(updated_parent);
    working.push_back(updated_grandparent);
    working.push_back(updated_uncle);

    return updated_uncle;



}

   void RbtreeDram_Concurrent::levelorder(){
       queue<rbtree_dram_node *> q;
       q.push(root_p);
       while(!q.empty()){
          int sz = q.size();
          for(int i=0;i<sz;i++){
              rbtree_dram_node* temp = q.front();
              cout<<temp->key<<" "<<temp->color<<" ";
              q.pop();
              if(temp->left!=NULL){
                  q.push(temp->left);
              }
              if(temp->right!=NULL){
                  q.push(temp->right);
              }
              
          }
          cout<<endl;
           
       }
   }

    /**
     * RbtreeDram_Concurrent::destroy -- Free Space of RBTree.
     */
    void RbtreeDram_Concurrent::destroy() {
        check();
        free_node(root_p);
    }

