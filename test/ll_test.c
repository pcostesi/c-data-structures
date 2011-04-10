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
#include <stdlib.h>
#include <string.h>
#include "ll.h"
#include "CUnit/Basic.h"

#define BUFFER_SIZE 64

int init_ll_suite(void) {
    return 0;
}


int clean_ll_suite(void) {
    return 0;
}

int print_content(size_t size, int i, const void * v, void * d){
    printf("[%d]>> %*s\n", i, (int) size, (const char *) v);
    return 0;
}

void print_haiku(llist * list){
    char buffer[BUFFER_SIZE];
    printf("*~~( Haiku )~~~~~~~*\n");
    printf("~                  ~\n");
    for(; list != NULL; list = ll_next(list)){
        ll_get(list, buffer, BUFFER_SIZE);
        printf("~ %-16s ~\n", buffer);
    }
    printf("~                  ~\n");
    printf("*~~~~~~~~~~~~~~~~~~*\n");
}

void test_ll(void)
{
    /* TODO: CU_ASSERT me */

    llist * list = NULL;
    llist * list2 = NULL;
    char *haiku[] = {"this is unix foo",
                     "ll_get the data",
                     "buffer explodes."};
    list = ll_insert(list, haiku[0], strlen(haiku[0]) + 1);
    ll_append(list, haiku[1], strlen(haiku[1]) + 1);
    ll_append(list, haiku[2], strlen(haiku[2]) + 1);

    print_haiku(list);

    ll_update(ll_prev(ll_tail(list)), "ll_update tests", \
            strlen("ll_update tests") + 1);

    print_haiku(list);

    list2 = list;
    list = ll_remove(list);
    print_haiku(list);

    ll_free(list2);
    ll_free(list);
}
