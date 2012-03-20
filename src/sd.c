/*
 *      sd.c
 *
 *      Copyright 2012:
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
#include <assert.h>
#include "sd.h"

#define MIN(A, B)		((A) < (B) ? (A) : (B))
#define HASH(T, K)		(_reduce((K)) % (T)->buckets_size)

typedef struct SD_KVPair sd_pair;

struct SD_KVPair{
    sd_key key;
    void * val;
    struct SD_KVPair * next;
};

struct SimpleDict{
    sd_pair ** buckets;
    size_t buckets_size;
    size_t used;
    size_t min;
    size_t max;
    float low;
    float high;
    unsigned int write_lock:1;
};

static size_t _reduce(sd_key key){
	size_t hash = 0;
	size_t step = 0;
	size_t * data = (size_t *) &key; 
	for (; step * sizeof(size_t) < sizeof(key); step++){
		hash ^= data[step];
	}
	return hash;
}

static void kv_free_list(sd_pair * list){
    sd_pair * aux = NULL;
    
    for (; list != NULL; list = aux){
        aux = list->next;
        free(list);
    }
}

static int key_is_node(sd_key key, sd_pair * node){
	assert(node != NULL);
	return !memcmp(key, node->key, sizeof(sd_key));
}

static inline sd_pair * _walk(sd_pair * list, sd_key key){

    while(list != NULL && !key_is_node(key, list))
        list = list->next;
    return list;
}


static sd_pair * _append(sd_pair * list, sd_key key, void * val){
    sd_pair * n = NULL;

    n = malloc(sizeof(sd_pair));
    if (n == NULL)
        return NULL;
	memcpy(n->key, key, sizeof(key));
	n->val = val;
    n->next = list;
    return n;
}

static inline void _rehash_all(sd * t, sd_pair ** newlist, size_t newsize){
    sd_pair * item = NULL;
    sd_pair * next = NULL;
    int i = 0;
    unsigned hash = 0;
    
    t->write_lock = 1;
    for (i = 0; i < t->buckets_size; i++){
        for (item = t->buckets[i]; item != NULL; item = next){
            next = item->next;
            
            /* reinsert in the right place */
            hash = _reduce(item->key) % newsize;
			item->next = newlist[hash];
			newlist[hash] = item;
        }
    }
    t->write_lock = 0;
}

static sd * _resize(sd * t){
	assert(t != NULL);
	
    sd_pair ** newlist = NULL;
    float ratio = t->used / t->buckets_size;
    size_t newsize = 0;

    if ((t->low < ratio && ratio < t->high) || /* between low and high */ \
        (t->low > ratio && t->min > t->used) || /* less than min elements */ \
        (t->high < ratio && t->max < t->used)) /* more than max elements */
        return t;

    newsize = 2 * t->used / (t->low + t->high) + 1;
    newlist = calloc(newsize, sizeof(sd_pair **));
    if (newlist == NULL)
        return t;
    _rehash_all(t, newlist, newsize);
    free(t->buckets);
    t->buckets = newlist;
    t->buckets_size = newsize;
    return t;
}

sd * sd_new(size_t min, size_t max, float low, float high){
    sd * t = malloc(sizeof(sd));
    
    if (t == NULL)
        return NULL;
    if (min >= max || low > high)
		return NULL;
    
	t->buckets = calloc(min, sizeof(sd_pair *));
    if(t->buckets == NULL){
        free(t);
        return NULL;
    }

    t->buckets_size = min;
    t->used = 0;
    t->write_lock = 0;
    t->min = min;
    t->low = low;
    t->high = high;
    t->max = max;
    return t;
}

void * sd_get(sd * t, sd_key key){
    sd_pair * list = NULL;
    unsigned hash = 0;
    
    assert(t != NULL);
    hash = HASH(t, key);
    list = _walk(t->buckets[hash], key);
    if (list == NULL)
        return NULL;
    return list->val;
}

sd * sd_set(sd * t, sd_key key, void * val){
    sd_pair * list = NULL;
    sd_pair * n = NULL;
    unsigned hash = 0;
    
    if (val == NULL)
		return NULL;
		
    assert(t != NULL);
    hash = HASH(t, key);
    if (t->write_lock)
        return NULL;

    list = t->buckets[hash];
    if(_walk(list, key) != NULL)
        return NULL;
        
    n = _append(list, key, val);
    if (n == NULL)
        return NULL;
        
    t->buckets[hash] = n;
    t->used++;

    /* check for boundaries and resize if necessary */
    return _resize(t);
}

sd * sd_del(sd * t, sd_key key){
	assert(t != NULL);
    sd_pair * list = NULL;
    sd_pair * prev = NULL;
    unsigned hash = HASH(t, key);
    
    if (t->write_lock)
        return NULL;
        
    list = t->buckets[hash];
    for (; list != NULL && !key_is_node(key, list); list = list->next)
        prev = list;
	
	if (list == NULL)
		return NULL;
	
	if (t->buckets[hash] == list)
		t->buckets[hash] = list->next;
	else
		prev->next = list->next;
	
	free(list);
	
	t->used--;

    return _resize(t);
}

sd * sd_update(sd * t, sd_key key, void * val){
    unsigned hash = 0;
    sd_pair * list = NULL;
    
    hash = HASH(t, key);
    list = _walk(t->buckets[hash], key);
	
	if (list == NULL)
		return NULL;
	
	list->val = val;
    return t;
}

void sd_free(sd * t){
    int i = 0;
    for (i = 0; i < t->buckets_size; i++)
        kv_free_list(t->buckets[i]);
    free(t->buckets);
    free(t);
}

int sd_each(sd * t, sd_eachf f, void * d){
    sd_pair * item = NULL;
    int i = 0;
    int n = 0;

    t->write_lock = 1;
    for (i = 0; i < t->buckets_size; i++){
        for (item = t->buckets[i]; !n && item != NULL; item = item->next){
            n = f(item->key, item->val, d);
            if (n != 0)
				return n;
        }
    }
    t->write_lock = 0;
    return n;
}

float sd_set_low(sd * t, float ratio){
    float old = t->low;
    if (0 < ratio && ratio < 1){
        t->low = ratio;
    }
    return old;
}

float sd_set_high(sd * t, float ratio){
    float old = t->high;
    if (0 < ratio && ratio < 1){
        t->high = ratio;
    }
    return old;
}

int sd_set_min(sd * t, size_t size){
    int old = t->min;
    if (size > 0)
        t->min = size;
    return old;
}

int sd_set_max(sd * t, size_t size){
    int old = t->max;
    if (size > 0)
        t->max = size;
    return old;
}

#undef MIN
