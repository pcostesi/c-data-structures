/*
 *      ll.c
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
#include "ll.h"

struct List{
    size_t size;
    void * val;
    struct List * next;
    struct List * prev;
};

llist * ll_new(const void * val, size_t size){
    llist * list = malloc(sizeof(llist));
    if (list == NULL)
        return NULL;
    size = size == 0 ? strlen(val) + 1 : size;
    list->val = malloc(size);
    if (list->val == NULL){
        free(list);
        return NULL;
    }
    list->size = size;
    memcpy(list->val, val, size);
    list->next = NULL;
    list->prev = NULL;
    return list;
}

llist * ll_append(llist * list, const void * val, size_t size){
    return ll_insert(ll_tail(list), val, size);
}

llist * ll_remove(llist * list){
    llist * prev = list->prev;
    llist * next = list->next;
    if (prev)
        prev->next = next;
    if (next)
        next->prev = prev;
    list->prev = NULL;
    list->next = NULL;
    return prev ? prev : next;
}

llist * ll_tail(llist * list){
    if (list != NULL)
        while(list->next)
            list = list->next;
    return list;
}

llist * ll_head(llist * list){
    if (list != NULL)
        while(list->prev)
            list = list->prev;
    return list;
}

llist * ll_paste(llist * list, llist * n){
    llist * tail = ll_tail(n);
    if (tail)
        tail->next = list->next;
    if (list->next)
        list->next->prev = tail;
    n->prev = list;
    list->next = n;
    return n;
}

llist * ll_update(llist * list, const void * val, size_t size){
    void * aux = malloc(size);
    if (aux == NULL)
        return NULL;
    free(list->val);
    list->val = aux;
    memcpy(aux, val, size);
    list->size = size;
    return list;
}

llist * ll_insert(llist * list, const void * val, size_t size){
    llist * n = ll_new(val, size);
    if (n == NULL)
        return NULL;
    return list != NULL ? ll_paste(list, n): n;
}

void ll_free(llist * list){
    llist * aux;
    for(; list != NULL; list = aux){
        aux = list->next;
        free(list->val);
        free(list);
    }
}

size_t ll_get(llist * list, void * buffer, size_t size){
    /* get at most size bytes, unless size is zero (then we default to
     * list->size. */
    if (list == NULL || buffer == NULL)
        return 0;
    size = size == 0 || list->size < size ? list->size : size;
    memcpy(buffer, list->val, size);
    return size;
}

llist * ll_next(llist * list){
    return list->next;
}

llist * ll_prev(llist * list){
    return list->prev;
}


llist * ll_filter(llist * list, ll_filter_f * f){
    llist * iter;
    llist * result = NULL;

    for (iter = list; iter != NULL; iter = ll_next(iter))
        if ((*f)(iter->val, iter->size))
            result = result ? result : ll_insert(result, iter->val, iter->size);

    return result;
}

void ll_split(llist ** list, llist ** newlist, ll_filter_f * f){
    llist * iter = *list, * aux;
    llist * tail = ll_tail(*newlist);

        while (iter){
            if ((*f)(iter->val, iter->size)){
                aux = iter;
                iter = ll_next(iter);
                *list = ll_remove(aux);
                tail = ll_paste(tail, aux);
            } else {
                iter = ll_next(iter);
            }
        }
        *newlist = ll_head(tail);
}
