/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _TESTS_TEST_H
#define _TESTS_TEST_H

/*
 * Standard test header that should be included in all tests. For now it just encapsulates the
 * include dependencies for Cmocka. Test-specific APIs that are so generic we would want them
 * available everywhere could also be added here.
 */

#include <arch/types.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

/* Helper macro to aviud checkpatch errors for some macros */
#define EMPTY_WRAP(...) __VA_ARGS__

/*
 * Set symbol value and make it global.
 */
#define TEST_SYMBOL(symbol, value) asm(".set " #symbol ", " #value "\n\t.globl " #symbol)

/*
 * Define memory region for testing purpose.
 *
 * Create buffer with specified name and size.
 * Create end symbol for it.
 */
#define TEST_REGION(region, size) uint8_t _##region[size];                                     \
	TEST_SYMBOL(_e##region, _##region + size);                                             \
	TEST_SYMBOL(_##region##_size, size)

/*
 * Set start, end and size symbols describing region without allocating memory for it.
 */
#define TEST_REGION_UNALLOCATED(region, start, size) EMPTY_WRAP(                               \
	TEST_SYMBOL(_##region, start);                                                         \
	TEST_SYMBOL(_e##region, _##region + size);                                             \
	TEST_SYMBOL(_##region##_size, size)                                                    \
)

/* Wrapper for running cmocka test groups using name provided by build system in __TEST_NAME__
   This should be used instead of cmocka_run_group_tests(). If there is a need to use custom
   group name, then please use cmocka_run_group_tests_name(). */
#define lp_run_group_tests(group_tests, group_setup, group_teardown)                           \
	cmocka_run_group_tests_name((__TEST_NAME__ "(" #group_tests ")"), group_tests,         \
				    group_setup, group_teardown)

#endif /* _TESTS_TEST_H */
