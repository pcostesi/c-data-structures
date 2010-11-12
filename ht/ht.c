/*
 *      ht.c
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



#include "ht.h"

#define LOW 0.4
#define HIGH 0.75
#define HT_MINSIZE (1 << 15)
/* I should *really* use an enum with primes for each table size */
#define HASH(T, K) (t->hash_f(K) % t->buckets_size)


typedef struct KVPair{
    char * key;
    void * val;
    size_t size;
    struct KVPair * next;
} kv;

struct Hashtable{
    hashf * hash_f;
    list * buckets;
    size_t buckets_size;
    size_t used;
    size_t min;
    float low;
    float high;
};

/* (See Chapter 6 @ K&R2) */
static hashkey hash(char * k)
{
    hashkey r = 0;
    for (; *k != 0; k++)
        r = *k + 31 * r;
    return r % ULONG_MAX;
}


ht * ht_new(hashf * f){
    ht t = malloc(sizeof(ht));
    if (t == NULL)
        return NULL;

    t->buckets = calloc(HT_MINSIZE, sizeof(kv *);
    if(t->buckets == NULL){
        free(t);
        return NULL;
    }

    t->buckets_size = HT_MINSIZE;
    t->used = 0;
    t->min = HT_MINSIZE;
    t->low = LOW;
    t->high = HIGH;
    t->hash_f = f ? f : hash;
    return t;
}

static kv * _walk(kv * list, char * key){
    while(list != NULL && !strcmp(key, list->key))
        list = list->next;
    return list;
}


static kv * _new_kv(char * key, void * val, size_t size){
    size_t str_size = 0;
    kv * n = NULL;

    n = malloc(sizeof(kv));
    if (n == NULL)
        goto nod_cleanup;

    str_size = strlen(key) + 1;
    n->key = malloc(str_size);
    if(n->key == NULL)
        goto str_cleanup;

    n->val = malloc(size);
    if(n->val == NULL)
        goto val_cleanup;

    memcpy(n->key, key, str_size);
    memcpy(n->val, val, size);
    n->next = NULL;

    return n;

    /* take that, Dijkstra! This is a well known pattern called RAII */
    val_cleanup:
        free(n->str);
    str_cleanup:
        free(n);
    nod_cleanup:
        return NULL;
}

static kv * _append(kv * list, char * key, void * val, size_t size){
    kv * n = _new_kv(key, val, size);
    if (n == NULL)
        return NULL;
    n->next = list;
    return n;
}

size_t ht_get(ht t, char * key, void * buffer, size_t size){
    kv * list = NULL;
    hashkey hash = HASH(t, key);
    list = t->buckets[hash];
    if (list == NULL)
        return 0;
    list = _walk(list, key);
    if (list == NULL)
        return 0;
    size = size == 0 || size > list->size ? list->size : size;
    memcpy(buffer, list->val, size);
    return size;
}

static void _resize(ht * t){

}

ht * ht_set(ht t, char * key, void * buffer, size_t size){
    kv * list = NULL;
    hashkey hash = HASH(t, key);
    list = t->buckets[hash];
    if(_walk(list, key) != NULL)
        return NULL;
    t->buckets[hash] = _append(list, key, val, size);
    t->used++;
    /* check for boundaries and resize if necessary */
    _resize(t);
    return t;
}

ht * ht_del(ht t, char * key){

}

ht * ht_update(ht t, char * key, void * buffer, size_t size){

}


#undef HASH
