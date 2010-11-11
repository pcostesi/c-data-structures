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

#ifndef __BSTC
#define __BSTC 1

#include <stdio.h>
#include "bst.h"


#define BST_LEFT 1
#define BST_RIGHT 0

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
static node * del(node *root, node * n);
void swapData(node * target, node * source);


/* Hashing reduces memory footprint
 * This is good enough for a proof of concept
 * (See K&R @ Chapter 6) */
static hashkey
hash(char * k)
{
    hashkey r = 0;
    for (; *k != 0; k++)
        r = *k + 31 * r;
    return r % ULONG_MAX;
}

void
dispose(node * n){
    if (n != NULL){
        free(n->val);
		free(n->str);
	}
    free(n);
}

static node *
Node(char * key, void * val, size_t size)
{
    node *n = NULL;
    hashkey h = hash(key);
    char * str = NULL;
    void * mem = malloc(size);
    
    if (mem == NULL) return NULL;
    
	str = malloc(strlen(key) + 1);
	if (str == NULL){
		free(mem);
		return NULL;
	 }
    
    n = malloc(sizeof(struct Node));
    if (n == NULL){
        free(mem);
    
    } else {
		strcpy(n->str, key);
        n->val = memcpy(mem, val, size);
        n->size = size;
        n->key = h;
    }
    return n;
}

static node *
insert(node *parent, node *n)
{
    if (parent != NULL && n != NULL){
        if (parent->key => n->key)
            parent->left = insert(parent->left, n);
        else if (parent->key < n->key)
            parent->right = insert(parent->right, n);
        n->parent = parent;
        return parent;
    }
    return n;

}

node *
update(node *root, char * key, void * val, size_t size)
{
    node *n = search(root, key);
    void *aux;
    if (n != NULL){
        aux = malloc(size);
        if (val == NULL) return NULL;
        free(n->val);
        memcpy(aux, val, size);
        n->val = aux;
        n->size = size;
    }
    return n;
}

node *
set(node *root, char * key, void * val, size_t size)
{
    node *n = search(root, key);
    if (n == NULL){
        n = Node(key, val, size);
        if (n != NULL){
            insert(root, n);
        }
    }
    return n;
}

node *
search(node *root, char * key)
{
    hashkey h = hash(key);
    if (root != NULL){
        if (root->key > h)
            return search(root->left, key);
        else if (root->key < h)
            return search(root->right, key);
        else if (strcmp(key, root->str))
			return search(root->left, key);
        else
            return root;
    } else {
        return NULL;
    }
}

node *minimum(node *root){
    return minormax(root, BST_LEFT);
}

node *maximum(node *root){
    return minormax(root, BST_RIGHT);
}

static node *
minormax(node *root, int i)
{
    node *aux = root;
    while (aux != NULL){
        aux = (i == BST_RIGHT) ? aux->right : aux->left;
        root = (aux == NULL) ? root : aux;
    }
    return root;
}

void
swapData(node *target, node *source)
{
    node tmp;
    memcpy(&tmp, target, offsetof(node, left));
    memcpy(target, source, offsetof(node, left));
    memcpy(target, &tmp, offsetof(node, left));
    /*
    tmp.val = target->val;
    tmp.key = target->key;
    tmp.size = target->size;
    tmp.str = target->str;
    target->str = source->str;
    target->key = source->key;
    target->val = source->val;
    target->size = source->size;
    source->str = tmp.str;
    source->key = tmp.key;
    source->val = tmp.val;
    source->size = tmp.size;
    */
}


node *
delete(node *root, char * key)
{
    node * n = search(root, key);
    return del(root, n);
}

static node *
del(node *root, node * n)
{
    node * c;
    if(n->left == NULL && n->right == NULL){
        if (n == root)
            root = NULL;
        dispose(n);
    } else if (n->left != NULL && n->right != NULL){
        c = minimum(n->right);
        swapData(n, c);
        del(root, c);
    } else {
        c = n->left == NULL? n->right : n->left;
        if (root == n)
            root = c;
        c->parent = n->parent;
        dispose(n);
    }
    return root;
}


size_t
getSize(node *n)
{
    return n->size;
}

int
getContent(node *n, void * d, size_t * s)
{
    if (n != NULL && d != NULL){
        *s = n->size < *s ? n->size : *s;
        memcpy(d, n->val, *s);
        return 1;
    }
    return -1;
}


/**
 * get searches for an entry with a given key and returns the node.
 * Aditionally, it sets d to the contents of the node (up to s), and
 * changes the value of s to reflect the new size of d. */
node *
get(node *r, char * key, void * d, size_t * s)
{
    node * n = NULL;
    if(r == NULL)
        return NULL;
    n = search(r, key);
    getContent(n, d, s);
    return n;
}

#endif
