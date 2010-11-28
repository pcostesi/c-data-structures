/*
 *      bstADT.c
 *
 *      Copyright 2010:
 *          Pablo Alejandro Costesich <pcostesi@alu.itba.edu.ar>
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following disclaimer
 *        in the documentation and/or other materials provided with the
 *        distribution.
 *      * Neither the name of the Owner nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>
#include "bst.h"


#define BST_LEFT 1
#define BST_RIGHT 0
#define SET 0
#define ADD 1

typedef unsigned long int hashkey;

struct Node{
    hashkey key;
    char * str;
    void * val;
    size_t size;
    struct Node * left;
    struct Node * parent;
    struct Node * right;
};

static hashkey hash(char * k);
static node * Node(char * key, void * val, size_t size);
static node * insert(node * root, node * n);
static node * minormax(node * root, int i);
static node * _delete(node *root, node * n);
static node * _update(node *n, char * key, void * val, size_t size);
static node * _set(node *root, char * key, void * val, size_t size, int act);
static void swapData(node * target, node * source);


/* Hashing reduces memory footprint and makes searching faster.
 * This is good enough for a proof of concept
 * (See Chapter 6 @ K&R2) */
static hashkey hash(char * k)
{
    hashkey r = 0;
    for (; *k != 0; k++)
        r = *k + 31 * r;
    return r % ULONG_MAX;
}

void bst_dispose(node * n){
    if (n != NULL){
        free(n->val);
        free(n->str);
    }
    free(n);
}

void bst_free(node * n){
    if (n != NULL){
        bst_free(n->left);
        bst_free(n->right);
        bst_dispose(n);
    }
}

static node * Node(char * key, void * val, size_t size)
{
    node *n = NULL;
    hashkey h = hash(key);
    char * str = NULL;
    void * mem;

    size = size == 0 ? strlen(val) + 1 : size;
    mem = malloc(size);

    if (mem == NULL) return NULL;

    str = malloc(strlen(key) + 1);
    if (str == NULL){
        free(mem);
        return NULL;
     }

    n = malloc(sizeof(struct Node));
    if (n == NULL){
        free(mem);
        free(str);
    } else {
        n->str = str;
        strcpy(str, key);
        n->val = memcpy(mem, val, size);
        n->size = size;
        n->key = h;
        n->left = n->right = NULL;
    }
    return n;
}

static node * insert(node *parent, node *n)
{
    if (parent != NULL && n != NULL){
        if (parent->key >= n->key)
            parent->left = insert(parent->left, n);
        else if (parent->key < n->key)
            parent->right = insert(parent->right, n);
        else if (strcmp(parent->str, n->str))
            parent->left = insert(parent->left, n);
        n->parent = parent;
        return parent;
    }
    return n;

}

static node * _update(node *n, char * key, void * val, size_t size)
{
    void *aux;
    size = size == 0 ? strlen(val) + 1 : size;
    aux = malloc(size);
    if (val == NULL) return NULL;
    free(n->val);
    memcpy(aux, val, size);
    n->val = aux;
    n->size = size;
    return n;
}

node * bst_update(node *root, char * key, void * val, size_t size)
{
    node *n = bst_search(root, key);
    if (n != NULL)
        n = _update(n, key, val, size);
    return n;
}

node * bst_add(node *root, char * key, void * val, size_t size)
{
    return _set(root, key, val, size, ADD);
}

node * bst_set(node *root, char * key, void * val, size_t size)
{
    return _set(root, key, val, size, SET);
}

/* Although add and set could be `#define'd, you wouldn't be able to use
 * pointers to functions. */
static node * _set(node *root, char * key, void * val, size_t size, int act)
{
    node *n = bst_search(root, key);
    if (n == NULL){
        n = Node(key, val, size);
        if (n != NULL){
            insert(root, n);
        }
    } else {
        if (act == SET)
            n = _update(n, key, val, size);
        else
            n = NULL;
    }
    return n;
}

node * bst_search(node *root, char * key)
{
    hashkey h = hash(key);
    if (root != NULL){
        if (root->key > h)
            return bst_search(root->left, key);
        else if (root->key < h)
            return bst_search(root->right, key);
        else if (strcmp(key, root->str))
            return bst_search(root->left, key);
        else
            return root;
    } else {
        return NULL;
    }
}

node * bst_minimum(node *root){
    return minormax(root, BST_LEFT);
}

node * bst_maximum(node *root){
    return minormax(root, BST_RIGHT);
}

static node * minormax(node *root, int i)
{
    node *aux = root;
    while (aux != NULL){
        aux = (i == BST_RIGHT) ? aux->right : aux->left;
        root = (aux == NULL) ? root : aux;
    }
    return root;
}

static void swapData(node *target, node *source)
{
    node tmp;
    /* copy the struct until we hit `left' */
    memcpy(&tmp, target, offsetof(node, left));
    memcpy(target, source, offsetof(node, left));
    memcpy(target, &tmp, offsetof(node, left));
}

node * bst_delete(node *root, char * key)
{
    node * n = bst_search(root, key);
    return _delete(root, n);
}

static node * _delete(node *root, node * n)
{
    node * c;
    if(n->left == NULL && n->right == NULL){
        if (n == root)
            root = NULL;
        bst_dispose(n);
    } else if (n->left != NULL && n->right != NULL){
        c = bst_minimum(n->right);
        swapData(n, c);
        _delete(root, c);
    } else {
        c = n->left == NULL? n->right : n->left;
        if (root == n)
            root = c;
        c->parent = n->parent;
        bst_dispose(n);
    }
    return root;
}


size_t bst_node_size(node *n)
{
    return n->size;
}

size_t bst_node_content(node *n, void * d, size_t s)
{
    if (n != NULL && d != NULL){
        s = s == 0 || n->size < s ? n->size : s;
        memcpy(d, n->val, s);
        return s;
    }
    return 0;
}


/**
 * get searches for an entry with a given key and returns the node. */
size_t bst_get(node *r, char * key, void * d, size_t s)
{
    node * n = NULL;
    if(r == NULL)
        return 0;
    n = bst_search(r, key);
    return bst_node_content(n, d, s);
}

#undef BST_LEFT
#undef BST_RIGHT
#undef SET
#undef ADD
