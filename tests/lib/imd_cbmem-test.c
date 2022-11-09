/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Include Unit Under Test source code directly instead of linking it.
 * This will allow access to internal structures and data without having
 * to extract them to another header file.
 */
#include "../lib/imd_cbmem.c"

#include <tests/test.h>
#include <stdlib.h>
#include <string.h>
#include <commonlib/bsd/helpers.h>
#include <imd.h>
#include <cbmem.h>
#include <imd_private.h>

#include <tests/lib/imd_cbmem_data.h>

#define CBMEM_ENTRY_ID 0xA001

static void reset_imd(void)
{
	imd.lg.limit = (uintptr_t)NULL;
	imd.lg.r = NULL;
	imd.sm.limit = (uintptr_t)NULL;
	imd.sm.r = NULL;

	cbmem_initialized = 0;
}

/* This implementation allows imd_cbmem module tests without linking lib/cbmem_common.c
   Function indicates to each hook if cbmem is being recovered or not. */
void cbmem_run_init_hooks(int is_recovery)
{
	function_called();
}

uintptr_t cbmem_top_chipset(void)
{
	return _cbmem_top_ptr;
}

static void *get_cbmem_ptr(void)
{
	void *cbmem_top_ptr = (void *)_cbmem_top_ptr;
	if (cbmem_top_ptr)
		return cbmem_top_ptr - CBMEM_SIZE;
	else
		return NULL;
}

static void clear_cbmem(void)
{
	void *ptr = get_cbmem_ptr();
	if (ptr)
		memset(ptr, 0, CBMEM_SIZE);
}

static void reset_and_clear_cbmem(void)
{
	reset_imd();
	clear_cbmem();
}

void prepare_simple_cbmem(void)
{
	reset_and_clear_cbmem();

	expect_function_call(cbmem_run_init_hooks);
	cbmem_initialize_empty();

	cbmem_entry_add(CBMEM_ENTRY_1_ID, CBMEM_ENTRY_1_SIZE);
	cbmem_entry_add(CBMEM_ENTRY_2_ID, CBMEM_ENTRY_2_SIZE);

	cbmem_entry_add(CBMEM_ENTRY_SM_1_ID, CBMEM_ENTRY_SM_1_SIZE);
	cbmem_entry_add(CBMEM_ENTRY_SM_2_ID, CBMEM_ENTRY_SM_2_SIZE);
}

static void test_cbmem_top(void **state)
{
	cbmem_top_init_once();

	if (ENV_ROMSTAGE)
		assert_ptr_equal(cbmem_top_chipset(), cbmem_top());

	if (ENV_POSTCAR || ENV_RAMSTAGE)
		assert_ptr_equal((void *)_cbmem_top_ptr, cbmem_top());
}

static void test_cbmem_initialize_empty(void **state)
{
	const struct cbmem_entry *found;

	/* Expect clean call without recovery */
	expect_function_call(cbmem_run_init_hooks);
	cbmem_initialize_empty();

	found = cbmem_entry_find(SMALL_REGION_ID);
	assert_non_null(found);
	/* Check that cbmem has only root, large and small entry. */
	assert_int_equal(2, ((struct imd_root *)imd.lg.r)->num_entries);
	assert_int_equal(1, ((struct imd_root *)imd.sm.r)->num_entries);
}

static void test_cbmem_initialize_empty_id_size(void **state)
{
	const struct cbmem_entry *entry1, *entry2;

	expect_function_call(cbmem_run_init_hooks);
	cbmem_initialize_empty_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);

	entry1 = cbmem_entry_find(SMALL_REGION_ID);
	entry2 = cbmem_entry_find(CBMEM_ENTRY_ID);

	assert_non_null(entry1);
	assert_non_null(entry2);
	assert_ptr_not_equal(entry1, entry2);
	/* Check that cbmem has root, large, small entries
	   and entry with id passed to init function. */
	assert_int_equal(3, ((struct imd_root *)imd.lg.r)->num_entries);
	assert_int_equal(1, ((struct imd_root *)imd.sm.r)->num_entries);
}

static void test_cbmem_initialize(void **state)
{
	int res;

	/* Expect call to fail as there is no previous cbmem to recover */
	res = cbmem_initialize();
	assert_int_equal(1, res);

	/* Create cbmem with few entries and check if initialization will recover */
	prepare_simple_cbmem();
	reset_imd();
	expect_function_call(cbmem_run_init_hooks);
	res = cbmem_initialize();
	assert_int_equal(0, res);
}

void test_cbmem_initialize_id_size_ramstage(void **state)
{
	int res;
	const struct cbmem_entry *entry1, *entry2;

	/* Expect call to fail as there is no previous cbmem to recover */
	res = cbmem_initialize_id_size(0, 0);
	assert_int_equal(1, res);

	reset_and_clear_cbmem();

	res = cbmem_initialize_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);
	assert_int_equal(1, res);

	/* Initialize empty cbmem with small region and check if next initialization
	   correctly recovers and creates its root entry with small region */
	expect_function_call(cbmem_run_init_hooks);
	cbmem_initialize_empty_id_size(0, 0);
	expect_function_call(cbmem_run_init_hooks);
	res = cbmem_initialize_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);
	assert_int_equal(0, res);

	entry1 = cbmem_entry_find(SMALL_REGION_ID);
	entry2 = cbmem_entry_find(CBMEM_ENTRY_ID);
	assert_non_null(entry1);
	assert_non_null(entry2);
	assert_ptr_not_equal(entry1, entry2);
	/* Check that cbmem has root, large, small entries and entry with id passed
	   to init function. */
	assert_int_equal(3, ((struct imd_root *)imd.lg.r)->num_entries);
	assert_int_equal(1, ((struct imd_root *)imd.sm.r)->num_entries);
}

void test_cbmem_initialize_id_size_romstage(void **state)
{
	int res;
	const struct cbmem_entry *entry1, *entry2;

	/* Expect call to fail as there is no previous cbmem to recover */
	res = cbmem_initialize_id_size(0, 0);
	assert_int_equal(1, res);

	/* Initialize empty cbmem with small region and check if next initialization
	   correctly recovers and creates its root entry with small region */
	expect_function_call(cbmem_run_init_hooks);
	cbmem_initialize_empty_id_size(0, 0);
	expect_function_call(cbmem_run_init_hooks);
	res = cbmem_initialize_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);
	assert_int_equal(0, res);

	entry1 = cbmem_entry_find(SMALL_REGION_ID);
	assert_non_null(entry1);

	/* Romstage locks imd cbmem initialization after recovery,
	   so entry with CBMEM_ENTRY_ID id is not present if it was not recovered. */
	entry2 = cbmem_entry_find(CBMEM_ENTRY_ID);
	assert_null(entry2);

	/* Initialize cbmem with few large and small entries */
	prepare_simple_cbmem();

	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_1_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_2_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_SM_1_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_SM_2_ID));

	reset_imd();

	expect_function_call(cbmem_run_init_hooks);
	res = cbmem_initialize_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);
	assert_int_equal(0, res);

	/* Initialization function should be able to recover entries left in cbmem
	   while having imd structure clean */
	entry1 = cbmem_entry_find(SMALL_REGION_ID);
	assert_non_null(entry1);
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_1_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_2_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_SM_1_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_SM_2_ID));
}

static void test_cbmem_recovery(void **state)
{
	int is_wakeup = 1;

	/* Reset imd, initialize cbmem and add entries for recovery */
	prepare_simple_cbmem();
	expect_function_call(cbmem_run_init_hooks);
	assert_int_equal(0, cbmem_recovery(is_wakeup));

	/* Check that entries have been correctly recovered */
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_1_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_2_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_SM_1_ID));
	assert_non_null(cbmem_entry_find(CBMEM_ENTRY_SM_2_ID));

	is_wakeup = 0;
	expect_function_call(cbmem_run_init_hooks);
	assert_int_equal(0, cbmem_recovery(is_wakeup));

	/* Check that after recovery with is_wakeup equal to 0 the cbmem is empty
	   and in initial state. */
	assert_null(cbmem_entry_find(CBMEM_ENTRY_1_ID));
	assert_null(cbmem_entry_find(CBMEM_ENTRY_2_ID));
	assert_null(cbmem_entry_find(CBMEM_ENTRY_SM_1_ID));
	assert_null(cbmem_entry_find(CBMEM_ENTRY_SM_2_ID));
	/* Check that cbmem has root, large and small entry. */
	assert_int_equal(2, ((struct imd_root *)imd.lg.r)->num_entries);
	assert_int_equal(1, ((struct imd_root *)imd.sm.r)->num_entries);
}

static void test_cbmem_entry_add(void **state)
{
	/* IDs used for testing. Don't have to be sequential.
	   Must not be equal to SMALL_REGION_ID. */
	const int id1 = 0x10;
	const int id2 = 0x11;
	const int id3 = 0x12;
	const struct cbmem_entry *entry1, *entry2;
	const struct cbmem_entry *entry_ret2, *entry_ret3;

	/* cbmem_run_init_hooks() will be called by init functions
	   but this test does not aim to check it */
	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty_id_size(id1, CBMEM_ROOT_SIZE);

	/* Expect NULL while looking for nonexistent entries */
	assert_null(cbmem_entry_find(id2));
	assert_null(cbmem_entry_find(id3));

	entry_ret2 = cbmem_entry_add(id2, CBMEM_ROOT_SIZE);
	/* Expect error when trying to add entry with zero size */
	assert_null(cbmem_entry_add(id3, 0));

	/* Check if entries have been added correctly and are not the same */
	entry1 = cbmem_entry_find(id1);
	entry2 = cbmem_entry_find(id2);
	assert_non_null(entry1);
	assert_non_null(entry2);
	assert_ptr_not_equal(entry1, entry2);
	assert_ptr_equal(entry_ret2, entry2);

	/* Add entry again and make sure that it has been
	   found instead of creating again. */
	entry_ret3 = cbmem_entry_add(id2, CBMEM_ROOT_SIZE / 2);
	assert_ptr_equal(entry_ret2, entry_ret3);
}

static void test_cbmem_add(void **state)
{
	const int id0 = 0x55;
	const int id1 = 0x66;
	const int id2 = 0x77;
	const int id3 = 0x88;
	const int entry1_size = 0x2000;
	const int entry2_size = 0x4d1;
	const int entry3_size = 0x30;
	void *entry1, *entry2, *entry3, *entry4;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty_id_size(id1, entry1_size);
	entry2 = cbmem_add(id2, entry2_size);
	entry3 = cbmem_add(id3, entry3_size);
	entry1 = cbmem_find(id1);

	/* All pointers should be non-null and distinct. */
	assert_non_null(entry1);
	assert_non_null(entry2);
	assert_non_null(entry3);
	assert_ptr_not_equal(entry1, entry2);
	assert_ptr_not_equal(entry1, entry3);
	assert_ptr_not_equal(entry2, entry3);

	/* Adding the same ID should yield the same entry pointer. */
	entry4 = cbmem_add(id2, entry2_size);
	assert_ptr_equal(entry2, entry4);

	/* Expect error while trying to add range with zero size */
	assert_null(cbmem_add(id0, 0));
}

static void test_cbmem_entry_find(void **state)
{
	const int id1 = 0xA0;
	const int id2 = 0xDD;
	const int id3 = 0xBD;
	const size_t entry1_size = CBMEM_ROOT_SIZE;
	const size_t entry2_size = CBMEM_ROOT_SIZE / 2;
	const size_t entry3_size = 6321;
	const struct cbmem_entry *cbm_e1, *cbm_e2, *cbm_e3;
	const struct cbmem_entry *entry1, *entry2, *entry3;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty();
	cbm_e1 = cbmem_entry_add(id1, entry1_size);
	cbm_e2 = cbmem_entry_add(id2, entry2_size);
	cbm_e3 = cbmem_entry_add(id3, entry3_size);

	/* Check pointers correctness and size for each entry */
	entry1 = cbmem_entry_find(id1);
	assert_ptr_equal(cbm_e1, entry1);
	assert_int_equal(0, (uintptr_t)cbmem_entry_start(cbm_e1) % CBMEM_SM_ALIGN);
	assert_int_equal(entry1_size, cbmem_entry_size(entry1));

	entry2 = cbmem_entry_find(id2);
	assert_ptr_equal(cbm_e2, entry2);
	assert_int_equal(0, (uintptr_t)cbmem_entry_start(cbm_e2) % CBMEM_SM_ALIGN);
	assert_int_equal(entry2_size, cbmem_entry_size(entry2));

	entry3 = cbmem_entry_find(id3);
	assert_ptr_equal(cbm_e3, entry3);
	assert_int_equal(0, (uintptr_t)cbmem_entry_start(cbm_e3) % CBMEM_SM_ALIGN);
	assert_int_equal(entry3_size, cbmem_entry_size(entry3));
}

static void test_cbmem_find(void **state)
{
	const int id1 = 0x30;
	const int id2 = 0x22;
	const int id3 = 0x101;
	void *cbm_e1, *cbm_e2, *entry1, *entry2;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty();
	cbm_e1 = cbmem_add(id1, CBMEM_ROOT_SIZE);
	cbm_e2 = cbmem_add(id2, CBMEM_ROOT_SIZE);

	entry1 = cbmem_find(id1);
	assert_non_null(entry1);
	assert_ptr_equal(cbm_e1, entry1);

	entry2 = cbmem_find(id2);
	assert_non_null(entry2);
	assert_ptr_equal(cbm_e2, entry2);

	/* Expect error when looking for non-existent id */
	assert_null(cbmem_find(id3));
}

static void test_cbmem_entry_remove(void **state)
{
	const int id1 = 0x2D;
	const int id2 = 0x3D;
	const int id3 = 0x4D;
	const struct cbmem_entry *cbm_e1, *cbm_e2;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty();
	cbm_e1 = cbmem_entry_add(id1, CBMEM_ROOT_SIZE);
	cbm_e2 = cbmem_entry_add(id2, CBMEM_ROOT_SIZE);

	/* Entries can be removed only in reverse order they have been added. */
	assert_int_equal(-1, cbmem_entry_remove(cbm_e1));
	assert_int_equal(0, cbmem_entry_remove(cbm_e2));
	assert_int_equal(0, cbmem_entry_remove(cbm_e1));

	/* Expect error when removing non-existent entry */
	assert_int_equal(-1, cbmem_entry_remove(cbmem_entry_find(id3)));
}

static void test_cbmem_entry_size(void **state)
{
	const int id1 = 0x4422;
	const int id2 = 0x2137;
	const int id3 = 0xb111;
	const size_t size1 = CBMEM_ROOT_SIZE * 4;
	const size_t size2 = 0x43;
	const size_t size3 = CBMEM_ROOT_SIZE * 8 + 7;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty_id_size(id1, size1);
	assert_non_null(cbmem_entry_add(id2, size2));
	assert_non_null(cbmem_entry_add(id3, size3));

	/* Entry size needs not to be aligned.
	   It has to be the same as provided while adding it. */
	assert_int_equal(size1, cbmem_entry_size(cbmem_entry_find(id1)));
	assert_int_equal(size2, cbmem_entry_size(cbmem_entry_find(id2)));
	assert_int_equal(size3, cbmem_entry_size(cbmem_entry_find(id3)));
}

static void test_cbmem_entry_start(void **state)
{
	const int id1 = 0x62;
	const int id2 = 0x26;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);
	cbmem_entry_find(CBMEM_ENTRY_ID);
	cbmem_entry_add(id1, 0x40);
	cbmem_entry_add(id2, CBMEM_ROOT_SIZE * 2);

	/* Check if start address of found entry is the same
	   as the one returned by cbmem_find() function */
	assert_ptr_equal(cbmem_find(CBMEM_ENTRY_ID),
			 cbmem_entry_start(cbmem_entry_find(CBMEM_ENTRY_ID)));
	assert_ptr_equal(cbmem_find(id1), cbmem_entry_start(cbmem_entry_find(id1)));
	assert_ptr_equal(cbmem_find(id2), cbmem_entry_start(cbmem_entry_find(id2)));
}

/* Reimplementation for testing purposes */
void bootmem_add_range(uint64_t start, uint64_t size, const enum bootmem_type tag)
{
	check_expected(start);
	check_expected(size);
	check_expected(tag);
}

static void test_cbmem_add_bootmem(void **state)
{
	void *base_ptr = NULL;
	size_t size = 0;
	const int id1 = 0xCA;
	const int id2 = 0xEA;
	const int id3 = 0xDA;
	const size_t size1 = 1024;
	const size_t size2 = 128;
	const size_t size3 = 8192;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);
	cbmem_entry_add(id1, size1);
	cbmem_entry_add(id2, size2);
	cbmem_entry_add(id3, size3);

	cbmem_get_region(&base_ptr, &size);
	assert_int_equal(ALIGN_DOWN(_cbmem_top_ptr, LIMIT_ALIGN), base_ptr + size);

	expect_value(bootmem_add_range, start, base_ptr);
	expect_value(bootmem_add_range, size, size);
	expect_value(bootmem_add_range, tag, BM_MEM_TABLE);
	cbmem_add_bootmem();

	/* Check that adding bootmem does not change base or size of cbmem */
	cbmem_get_region(&base_ptr, &size);
	assert_int_equal(ALIGN_DOWN(_cbmem_top_ptr, LIMIT_ALIGN), base_ptr + size);
}

static void test_cbmem_get_region(void **state)
{
	int i;
	void *base_ptr = NULL;
	size_t size = 0;
	size_t size_counter = 0;
	const size_t entry_size = 0x2000;
	const size_t alloc_num = 32;
	const size_t small_entry_size = 64;
	const size_t small_alloc_num = 3;

	ignore_function_calls(cbmem_run_init_hooks);

	cbmem_initialize_empty_id_size(CBMEM_ENTRY_ID, CBMEM_ROOT_SIZE);

	/* Check size and base pointer for empty initialized cbmem */
	cbmem_get_region(&base_ptr, &size);
	assert_non_null(base_ptr);
	assert_int_not_equal(0, size);
	assert_int_equal(CBMEM_ROOT_SIZE + cbmem_overhead_size(), size);
	assert_int_equal(ALIGN_DOWN(_cbmem_top_ptr, LIMIT_ALIGN), base_ptr + size);

	/* Check for multiple big allocations */
	for (i = 1; i <= alloc_num; ++i) {
		const struct cbmem_entry *e = cbmem_entry_add(i, entry_size);
		assert_non_null(e);
		size_counter += cbmem_entry_size(e);

		/* Check if size is correct after each big allocation */
		cbmem_get_region(&base_ptr, &size);
		assert_int_equal(size_counter + cbmem_overhead_size() + CBMEM_ROOT_SIZE, size);
	}

	/* Check for few small allocations. */
	for (; i <= alloc_num + small_alloc_num; ++i) {
		const struct cbmem_entry *e = cbmem_entry_add(i, small_entry_size);
		assert_non_null(e);

		/* Check if size is correct after each small allocation. It should not change
		   as small entries have their region allocated and entry size is selected
		   to fit in it couple of times */
		cbmem_get_region(&base_ptr, &size);
		assert_int_equal(size_counter + cbmem_overhead_size() + CBMEM_ROOT_SIZE, size);
	}
}

static void test_general_data_structure(void **state)
{
	/* Initialize cbmem with few big and small entries, then check if binary data structure
	   is the same as stored in array containing hardcoded dumped cbmem */
	prepare_simple_cbmem();
	assert_memory_equal(get_cbmem_ptr(), test_cbmem_data, CBMEM_SIZE);
}

static int setup_teardown_test_imd_cbmem(void **state)
{
	reset_and_clear_cbmem();
	return 0;
}

static int setup_group_imd_cbmem(void **state)
{
	/* Allocate more data to have space for alignment */
	void *top_ptr = malloc(CBMEM_SIZE + DYN_CBMEM_ALIGN_SIZE);

	if (!top_ptr)
		return -1;

	*state = top_ptr;

	_cbmem_top_ptr = ALIGN_UP((uintptr_t)top_ptr + CBMEM_SIZE, DYN_CBMEM_ALIGN_SIZE);
	return 0;
}

static int teardown_group_imd_cbmem(void **state)
{
	reset_imd();
	free(*state);
	return 0;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_cbmem_top,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_initialize_empty,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_initialize_empty_id_size,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_initialize,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
#if ENV_ROMSTAGE_OR_BEFORE
		cmocka_unit_test_setup_teardown(test_cbmem_initialize_id_size_romstage,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
#else
		cmocka_unit_test_setup_teardown(test_cbmem_initialize_id_size_ramstage,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
#endif
		cmocka_unit_test_setup_teardown(test_cbmem_recovery,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_entry_add,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_add,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_entry_find,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_find,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_entry_remove,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_entry_size,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_entry_start,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_add_bootmem,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_cbmem_get_region,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
		cmocka_unit_test_setup_teardown(test_general_data_structure,
				setup_teardown_test_imd_cbmem,
				setup_teardown_test_imd_cbmem),
	};

	return cb_run_group_tests(tests, setup_group_imd_cbmem, teardown_group_imd_cbmem);
}
