/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <stage_cache.h>

#define CBMEM_SIZE (32 * KiB)

/* CBMEM top pointer used by implementation. */
extern uintptr_t _cbmem_top_ptr;

void cbmem_run_init_hooks(int is_recovery)
{
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

int setup_test(void **state)
{
	void *cbmem_top_ptr = malloc(CBMEM_SIZE);

	if (!cbmem_top_ptr)
		return -1;

	_cbmem_top_ptr = (uintptr_t)cbmem_top_ptr + CBMEM_SIZE;
	clear_cbmem();
	cbmem_initialize_empty();
	return 0;
}

int teardown_test(void **state)
{
	if (_cbmem_top_ptr && (_cbmem_top_ptr - CBMEM_SIZE))
		free((void *)(_cbmem_top_ptr - CBMEM_SIZE));

	_cbmem_top_ptr = 0;
	return 0;
}

/* This function is used as prog_entry of struct prog to prevent potential calls to unaccessible
   or incorrect addresses. */
void prog_entry_mock(void *arg)
{
}

/* This test checks if stage_cache_add() correctly adds CBMEM_ID_STAGE_x_META
   and CBMEM_ID_STAGEx_CACHE entries to cbmem. stage_cache_add() must create meta
   entry containing load address, entry address and argument for it. It also must
   copy buffer pointer pointed by start pointer of prog struct to cache entry. */
void test_stage_cache_add(void **state)
{
	const int id = 12;
	int arg = 0xC14;
	struct stage_cache *meta = NULL;
	uint8_t *prog_data_buf = NULL;
	const size_t data_sz = 4 * KiB;
	uint8_t *data = malloc(data_sz);
	struct prog prog_data = {0};

	assert_non_null(data);
	memset(data, 0xDB, data_sz);
	prog_data = (struct prog)PROG_INIT(PROG_ROMSTAGE, "test_prog");
	prog_set_area(&prog_data, data, data_sz);
	prog_set_entry(&prog_data, prog_entry_mock, &arg);

	stage_cache_add(id, &prog_data);

	meta = cbmem_find(CBMEM_ID_STAGEx_META + id);
	assert_non_null(meta);
	assert_int_equal(meta->load_addr, (uintptr_t)prog_start(&prog_data));
	assert_int_equal(meta->entry_addr, (uintptr_t)prog_entry(&prog_data));
	assert_int_equal(meta->arg, (uintptr_t)prog_entry_arg(&prog_data));

	prog_data_buf = cbmem_find(CBMEM_ID_STAGEx_CACHE + id);
	assert_non_null(prog_data_buf);
	assert_memory_equal(data, prog_data_buf, data_sz);

	free(data);
}

/* This test checks if stage_cache_add_raw() correctly creates entry with data from
   provided buffer. Data should be accessible using cbmem_find() with
   (CBMEM_ID_STAGEx_RAW + id) parameter. */
void test_stage_cache_add_raw(void **state)
{
	const int id = 55;
	const size_t data_sz = 8 * KiB;
	uint8_t *data = malloc(data_sz);
	uint8_t *data_raw = NULL;

	assert_non_null(data);
	memset(data, 0x91, data_sz);

	stage_cache_add_raw(id, data, data_sz);

	data_raw = cbmem_find(CBMEM_ID_STAGEx_RAW + id);
	assert_non_null(data_raw);
	assert_memory_equal(data_raw, data, data_sz);

	free(data);
}


/* This test checks if stage_cache_get_raw() correctly extracts base and size of previously
   added entry.  */
void test_stage_cache_get_raw(void **state)
{
	const int id = 23;
	const size_t data_sz = 3 * KiB;
	uint8_t *data = malloc(data_sz);
	size_t data_out_sz = 0;
	uint8_t *data_out = NULL;

	assert_non_null(data);
	memset(data, 0x3c, data_sz);
	stage_cache_add_raw(id, data, data_sz);

	stage_cache_get_raw(id, (void **)&data_out, &data_out_sz);

	assert_int_equal(data_sz, data_out_sz);
	assert_memory_equal(data, data_out, data_sz);

	free(data);
}

/* This test checks if stage_cache_load_stage() correctly loads previously added stage data
   and its metadata. */
void test_stage_cache_load_stage(void **state)
{
	int id = 0xCC;
	struct prog prog_out = {0};
	const size_t data_sz = 7 * KiB;
	uint8_t *data = malloc(data_sz);
	uint8_t *data_bak = malloc(data_sz);
	struct prog prog_data = {0};
	int arg = 0x33224455;

	assert_non_null(data);
	assert_non_null(data_bak);
	memset(data, 0x45, data_sz);

	prog_data = (struct prog)PROG_INIT(PROG_RAMSTAGE, "test_prog");
	prog_set_area(&prog_data, data, data_sz);
	prog_set_entry(&prog_data, prog_entry_mock, &arg);
	stage_cache_add(id, &prog_data);

	/* Copy current data to backup buffer and clear current buffer */
	memcpy(data_bak, data, data_sz);
	memset(data, 0, data_sz);

	/* Load stage data. Data should be returned to the same buffer. */
	stage_cache_load_stage(id, &prog_out);

	/* Data should be same as it was before */
	assert_memory_equal(data, data_bak, data_sz);
	assert_int_equal(prog_start(&prog_data), prog_start(&prog_out));
	assert_int_equal(prog_size(&prog_data), prog_size(&prog_out));
	assert_ptr_equal(prog_entry(&prog_data), prog_entry(&prog_out));
	assert_ptr_equal(prog_entry_arg(&prog_data), prog_entry_arg(&prog_out));

	free(data_bak);
	free(data);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_stage_cache_add,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_stage_cache_add_raw,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_stage_cache_get_raw,
						setup_test, teardown_test),
		cmocka_unit_test_setup_teardown(test_stage_cache_load_stage,
						setup_test, teardown_test),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
