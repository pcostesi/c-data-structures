/*
 *      ht_test.c
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

