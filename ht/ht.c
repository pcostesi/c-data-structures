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


#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "ht.h"

#define LOW 0.4
#define HIGH 0.75
#define MAX ULONG_MAX
#define HT_MINSIZE (1 << 10)
/* I should *really* use an enum with primes for each table size */
#define HASH(T, K) (T->hash_f(K) % T->buckets_size)

typedef struct KVPair kv;

static unsigned int PRIMES[] = {  0, 0, 0, 0, 0, 53, 97, 193, 389, 769, \
                                1543, 3079, 6151, 12289, 24593, 49157, \
                                98317, 196613, 393241, 786433, 1572869, \
                                3145739, 6291469, 12582917, 25165843, \
                                50331653, 100663319, 201326611, 402653189, \
                                805306457, 1610612741};

struct KVPair{
    char * key;
    void * val;
    size_t size;
    struct KVPair * next;
};

struct Hashtable{
    hashf hash_f;
    kv ** buckets;
    size_t buckets_size;
    size_t used;
    size_t min;
    size_t max;
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

static void kv_free_node(kv * elem){
    free(elem->val);
    free(elem->key);
    free(elem);
}

static void kv_free_list(kv * list){
    kv * aux;
    for (; list != NULL; list = aux){
        aux = list->next;
        kv_free_node(list);
    }
}

static kv * _walk(kv * list, char * key){
    while(list != NULL && strcmp(key, list->key))
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

    n->size = size == 0 ? strlen(val) + 1 : size;
    n->val = malloc(n->size);
    if(n->val == NULL)
        goto val_cleanup;

    memcpy(n->key, key, str_size);
    memcpy(n->val, val, n->size);
    n->next = NULL;
    return n;

    /* take that, Dijkstra! This is a well known pattern called RAII
     * (and cross platform. Using __attribute__((cleanup(n)))
     * is gcc-dependent and not exactly what we want) */
    val_cleanup:
        free(n->key);
    str_cleanup:
        free(n);
    nod_cleanup:
        return NULL;
}

static kv * _append(kv * list, char * key, void * val, size_t size){
    kv * n = _new_kv(key, val, size);
    if (n != NULL)
        n->next = list;
    return n;
}

static int _update(kv * elem, void * val, size_t size){
    void * aux = NULL;
    if (elem == NULL)
        return 0;

    size = size == 0 ? strlen(val) + 1 : size;
    aux = realloc(elem->val, size);
    if(aux == NULL)
        return 0;
    elem->val = aux;
    elem->size = size;
    memcpy(elem->val, val, size);
    return 1;
}

static void _reinsert(ht * t, kv ** list, kv * pair, size_t newsize){
    hashkey hash = t->hash_f(pair->key) % newsize;
    pair->next = list[hash];
    list[hash] = pair;
}

static void _rehash_all(ht * t, kv ** newlist, size_t newsize){
    kv * item = NULL, * next = NULL;
    int i;

    for (i = 0; i < t->buckets_size; i++){
        for (item = t->buckets[i]; item != NULL; item = next){
            next = item->next;
            _reinsert(t, newlist, item, newsize);
        }
    }
}

static ht * _resize(ht * t){
    kv ** newlist = NULL;
    float ratio = t->used / t->buckets_size;
    size_t newsize = 0;

    if ((t->low < ratio && ratio < t->high) || /* between low and high */ \
        (t->low > ratio && t->min > t->used) || /* less than min elements */ \
        (t->high < ratio && t->max < t->used)) /* more than max elements */
        return t;

    newsize = 2 * t->used / (t->low + t->high) + 1;
    newlist = calloc(newsize, sizeof(kv **));
    if (newlist == NULL)
        return t;
    _rehash_all(t, newlist, newsize);
    free(t->buckets);
    t->buckets = newlist;
    t->buckets_size = newsize;
    return t;
}

static kv * _del(kv * list, char * key){
    kv * orig = list;
    kv * prev = NULL;
    kv * ret = NULL;
    for (; list != NULL && !strcmp(list->key, key); list = list->next)
        prev = list;

    if (list != NULL){
        if (orig == list){
            ret = list->next;
            kv_free_node(list);
            return ret;
        } else {
            ret = orig;
            prev->next = list->next;
        }
    }
    return NULL;
}

ht * ht_new(hashf f){
    ht * t = malloc(sizeof(ht));
    if (t == NULL)
        return NULL;

    t->buckets = calloc(HT_MINSIZE, sizeof(kv *));
    if(t->buckets == NULL){
        free(t);
        return NULL;
    }

    t->buckets_size = HT_MINSIZE;
    t->used = 0;
    t->min = HT_MINSIZE;
    t->low = LOW;
    t->high = HIGH;
    t->max = MAX;
    t->hash_f = f ? f : &hash;
    return t;
}

size_t ht_get(ht * t, char * key, void * buffer, size_t size){
    kv * list = NULL;
    hashkey hash = HASH(t, key);

    list = t->buckets[hash];
    if (list == NULL)
        return 0;
    list = _walk(list, key);
    if (list == NULL)
        return 0;
    size = size > list->size ? list->size : size;
    memcpy(buffer, list->val, size);

    return size == 0 ? list->size : size;
}

ht * ht_set(ht * t, char * key, void * val, size_t size){
    kv * list = NULL;
    kv * n = NULL;
    hashkey hash = HASH(t, key);

    list = t->buckets[hash];
    if(_walk(list, key) != NULL)
        return NULL;
    n = _append(list, key, val, size);
    if (n == NULL)
        return NULL;
    t->buckets[hash] = n;
    t->used++;

    /* check for boundaries and resize if necessary */
    return _resize(t);
}

ht * ht_del(ht * t, char * key){
    kv * list;
    hashkey hash = HASH(t, key);

    list = _del(t->buckets[hash], key);
    if(list == NULL)
        return NULL;
    t->buckets[hash] = list;
    t->used--;

    return _resize(t);
}

ht * ht_update(ht * t, char * key, void * buffer, size_t size){
    hashkey hash = HASH(t, key);
    kv * list = _walk(t->buckets[hash], key);

    if(!_update(list, buffer, size))
        return NULL;

    return _resize(t);
}

void ht_free(ht * t){
    int i;
    for (i = 0; i < t->buckets_size; i++)
        kv_free_list(t->buckets[i]);
    free(t->buckets);
    free(t);
}

float ht_set_low(ht * t, float ratio){
    float old = t->low;
    if (0 < ratio && ratio < 1){
        t->low = ratio;
    }
    return old;
}

float ht_set_high(ht * t, float ratio){
    float old = t->high;
    if (0 < ratio && ratio < 1){
        t->high = ratio;
    }
    return old;
}

int ht_set_min(ht * t, size_t size){
    int old = t->min;
    if (size > 0)
        t->min = size;
    return old;
}

int ht_set_max(ht * t, size_t size){
    int old = t->max;
    if (size > 0)
        t->max = size;
    return old;
}

#undef HASH
