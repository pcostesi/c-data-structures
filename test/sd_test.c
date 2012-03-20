/*
 *      sd_test.c
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

#include <stdio.h>
#include <string.h>
#include "sd.h"

#include "CUnit/Basic.h"

int init_sd_suite(void) {
    return 0;
}

int clean_sd_suite(void) {
    return 0;
}

int eachf_sd(sd_key k, const void * v, void * d){
    return 0;
}

void test_sd(void)
{
	sd_key key = {10, 10, 42, 42, 0};
	char * message = "hello world";
	char * message2 = "hello worldd";
    sd * t = sd_new(SD_DEFAULTS);
    CU_ASSERT(NULL != t);
    
    /* test that we fail when we need to fail */
    CU_ASSERT(NULL == sd_get(t, key));
    CU_ASSERT(NULL == sd_update(t, key, message));
	CU_ASSERT(NULL == sd_del(t, key));
    
    /*set the dict key*/
    CU_ASSERT(NULL != sd_set(t, key, message));
    
    CU_ASSERT(message == sd_get(t, key));
    CU_ASSERT(NULL != sd_update(t, key, message2));
    sd_each(t, eachf_sd, NULL);
	CU_ASSERT(NULL != sd_del(t, key));
    
    sd_free(t);
}

