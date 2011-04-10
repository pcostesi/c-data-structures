/*
//      main.c
//
//      Copyright 2010 Pablo Alejandro Costesich <pcostesi@alu.itba.edu.ar>
//
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.
*/

#include <stdio.h>
#include <string.h>
#include "ht.h"

#include "CUnit/Basic.h"

int init_ht_suite(void) {
    return 0;
}

int clean_ht_suite(void) {
    return 0;
}

/* Awesomely horrible hash function, but easy to test. */
hashkey hasher(char * key){
    printf("Hashing %s\n", key);
    hashkey code = 0;
    for (; *key != 0; key++)
        code += *key;
    printf("Code is %d\n", (int)code);
    return code;
}

int eachf(size_t size, const char * k, const void * v, void * d){
    printf("\t'%s' : %*s\n", k, (int) size, (const char *) v);
    return 0;
}

void test_ht(void)
{
    ht * t = ht_new(NULL);
    size_t bsize;
    char buffer[] = "GARBAGEGARBAGEGARBAGEGARBAGEGARBAGEGARBAGEGARBAGE";

    bsize = 1 + strlen(buffer);

    /* Test Get */
    CU_ASSERT( NULL != ht_set(t, "test", "this is a test string", 0));
    CU_ASSERT(ht_get(t, "test", &buffer, bsize));

    /* Test Set */
    CU_ASSERT(NULL == ht_set(t, "test", "t", 0));

    /* Test update */
    CU_ASSERT(NULL != ht_update(t, "test", "this is another test string", 0));
    CU_ASSERT(ht_get(t, "test", &buffer, bsize));

    /* Freeing the table */
    ht_free(t);


    t = ht_new(hasher);
    CU_ASSERT(ht_set(t, "test", "this is a test string", 0) != NULL);
    CU_ASSERT(ht_get(t, "test", &buffer, 0));
    
    /* TODO: Assert me*/
    ht_each(t, eachf, NULL);

    ht_free(t);
}

