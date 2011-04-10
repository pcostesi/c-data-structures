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

#include "CUnit/Basic.h"

int init_bst_suite(void);
int init_ll_suite(void);
int init_ht_suite(void);

void test_bst(void);
void test_ht(void);
void test_ll(void);

int clean_bst_suite(void);
int clean_ll_suite(void);
int clean_ht_suite(void);

int main(int argc, char **argv)
{
    /* Initialize the CUnit test registry */
    if ( CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    CU_pSuite bstSuite = CU_add_suite("BST Suite", init_bst_suite, clean_bst_suite);
    CU_pSuite llSuite = CU_add_suite("LinkedList Suite", init_ll_suite, clean_ll_suite);
    CU_pSuite htSuite = CU_add_suite("HashTable Suite", init_ht_suite, clean_ht_suite);

    if ( NULL == bstSuite || NULL == llSuite || NULL == htSuite ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /*add the test cases to the suites */
    if ((NULL == CU_add_test(bstSuite, "test_bst", test_bst)) || 
        (NULL == CU_add_test(llSuite, "test_ll", test_ll)) ||
        (NULL == CU_add_test(htSuite, "test_ht", test_ht)) ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
