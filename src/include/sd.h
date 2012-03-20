/*
 *      sd.h
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


#ifndef __HSD
#define __HSD 1


#include <stddef.h>
#include <limits.h>


#define SD_KEY_SIZE (5 * sizeof(int))
#define SD_LOW 0.4
#define SD_HIGH 0.75
#define SD_MAX ULONG_MAX
#define SD_MIN (1 << 8)
#define SD_DEFAULTS SD_MIN, SD_MAX, SD_LOW, SD_HIGH


typedef unsigned char sd_key[SD_KEY_SIZE];

typedef int (*sd_eachf)(sd_key k, const void * v, void * d);
typedef struct SimpleDict sd;

sd * 	sd_new(size_t min, size_t max, float low, float high);
void    sd_free(sd * t);
void *  sd_get(sd * t, sd_key key);
sd *    sd_set(sd * t, sd_key key, void * val);
sd *    sd_del(sd * t, sd_key key);
sd *    sd_update(sd * t, sd_key key, void * val);
float   sd_set_low(sd * t, float ratio);
float   sd_set_high(sd * t, float ratio);
int     sd_set_min(sd * t, size_t size);
int     sd_set_max(sd * t, size_t size);
int     sd_each(sd * t, sd_eachf f, void * d);
#endif
