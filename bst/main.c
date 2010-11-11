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
#include "bst.h"

#define BUFFER_SIZE 64

#define PRINTN(A, B) {  sbuffer = BUFFER_SIZE; \
						memset(buffer, 0, sizeof buffer); \
						printf("\"%s\": \"%s\"\n", B, \
							 bst_get(A, B, &buffer, &sbuffer) ? \
								buffer : "<NULL>"); \
					 }


int main(int argc, char **argv)
{
    char * test = "this is a test. Let's see if it works!";
    char buffer[BUFFER_SIZE];
    size_t sbuffer;
    node * root = NULL;

    /* set a few nodes to test the tree:
     *  - Add adds an element, and fails when it already exists;
     *  - Set adds an element, updating it if already exists;
     *  - Update only updates existing elements. */
    root = bst_set(root, "test", test, strlen(test) + 1);
    bst_set(root, "test2", "1234567890", 5);
    bst_set(root, "test3", "aucdef", 2);
    if (bst_add(root, "test", NULL, 0) != NULL)
        fprintf(stderr, "add should fail with existing keys\n");
    bst_update(root, "test3", "abcdef", 4);

    /* this is how you query the tree:
     * - set sbuffer to the buffer size;
     * - optionally, clean the buffer;
     * - get() will modify sbuffer to reflect the actual size of the
     *   data copied to buffer.
     */
    PRINTN(root, "test");
    PRINTN(root, "test2");
    root = bst_delete(root, "test");
    PRINTN(root, "test");
    PRINTN(root, "test3");

    return 0;
}
