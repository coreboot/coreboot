/* SPDX-License-Identifier: GPL-2.0-only */

/* Include malloc() and memalign() source code and alter its name to indicate the functions
   source origin. */
#define calloc cb_calloc
#define malloc cb_malloc
#define free cb_free
#define memalign cb_memalign
#undef __noreturn
#define __noreturn

#include "../lib/malloc.c"

#undef calloc
#undef malloc
#undef free
#undef memalign
#undef __noreturn
#define __noreturn __attribute__((noreturn))

#include <stdlib.h>
#include <tests/test.h>
#include <commonlib/helpers.h>
#include <types.h>
#include <symbols.h>

/* 4 MiB */
#define TEST_HEAP_SZ 0x400000

/* Heap region setup */
__weak extern uint8_t _test_heap[];
__weak extern uint8_t _etest_heap[];
TEST_REGION(test_heap, TEST_HEAP_SZ);
TEST_SYMBOL(_heap, _test_heap);
TEST_SYMBOL(_eheap, _etest_heap);

void die(const char *msg, ...)
{
	function_called();
}

static int setup_test(void **state)
{
	free_mem_ptr = &_heap;
	free_mem_end_ptr = &_eheap;
	free_last_alloc_ptr = &_heap;

	return 0;
}

static int setup_calloc_test(void **state)
{
	memset(&_heap, 0xFF, TEST_HEAP_SZ);
	return setup_test(state);
}

static void test_malloc_out_of_memory(void **state)
{
	/* Expect die() call if out of memory */
	expect_function_call(die);
	cb_malloc(TEST_HEAP_SZ);
}

static void test_malloc_zero(void **state)
{
	void *ptr1 = cb_malloc(0);
	void *ptr2 = cb_malloc(0);
	void *ptr3 = cb_malloc(0);

	/* Expect malloc(0) to return the same pointer as there are no bytes
	   to be added to the heap */
	assert_ptr_equal(ptr1, ptr2);
	assert_ptr_equal(ptr2, ptr3);
}

static void test_malloc_multiple_small_allocations(void **state)
{
	/* Make multiple small allocations (smaller than alignment)
	   Expect no call to die(), as this allocations should be small
	   enough to fit in provided memory */
	void *prev;
	void *curr = cb_malloc(3);
	assert_non_null(curr);
	for (int i = 0; i < 1000; ++i) {
		prev = curr;
		curr = cb_malloc(3);
		assert_non_null(curr);
		assert_true(prev < curr);
	}
}

static void test_memalign_different_alignments(void **state)
{
	void *ptr1 = cb_memalign(4, 30);
	void *ptr2 = cb_memalign(16, 22);
	void *ptr3 = cb_memalign(8, 64);

	assert_true((uintptr_t)ptr1 % 4 == 0);
	assert_true((uintptr_t)ptr2 % 16 == 0);
	assert_true((uintptr_t)ptr3 % 8 == 0);
}

static void test_memalign_out_of_memory(void **state)
{
	expect_function_call(die);
	cb_memalign(16, TEST_HEAP_SZ);
}

static void test_memalign_zero(void **state)
{
	void *ptr1 = cb_memalign(16, 0);
	void *ptr2 = cb_memalign(7, 0);
	void *ptr3 = cb_memalign(11, 0);

	/* Expect memalign(x, 0) to return the same pointer as there are no bytes
	   to be added to the heap */
	assert_ptr_equal(ptr1, ptr2);
	assert_ptr_equal(ptr2, ptr3);
}

static void test_memalign_multiple_small_allocations(void **state)
{
	/* Make multiple small allocations (smaller than alignment)
	   Expect no call to die(), as this allocations should be small
	   enough to fit in provided memory. There should also be no error
	   when allocating memory with different align values. */
	void *prev;
	void *curr = cb_memalign(3, 3);
	assert_non_null(curr);
	for (int i = 0; i < 1000; ++i) {
		/* Expect new pointer larger than previously allocated and aligned to provided
		   value. Alignment has to be power of 2 to be applied correctly. */
		prev = curr;
		curr = cb_memalign(2u << (i % 6), 3);
		assert_non_null(curr);
		assert_true(prev < curr);
		assert_true((uintptr_t)curr % (2u << (i % 6)) == 0);
	}
}

static void test_calloc_memory_is_zeroed(void **state)
{
	const size_t nitems = 42;
	const size_t size = sizeof(uint32_t);
	void *ptr = cb_calloc(nitems, size);
	assert_non_null(ptr);

	for (size_t i = 0; i < nitems; i++) {
		const uint32_t *p = (const uint32_t *)ptr + i;
		assert_int_equal(*p, 0);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_malloc_out_of_memory, setup_test),
		cmocka_unit_test_setup(test_malloc_zero, setup_test),
		cmocka_unit_test_setup(test_malloc_multiple_small_allocations, setup_test),
		cmocka_unit_test_setup(test_memalign_different_alignments, setup_test),
		cmocka_unit_test_setup(test_memalign_out_of_memory, setup_test),
		cmocka_unit_test_setup(test_memalign_zero, setup_test),
		cmocka_unit_test_setup(test_memalign_multiple_small_allocations, setup_test),
		cmocka_unit_test_setup(test_calloc_memory_is_zeroed, setup_calloc_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
