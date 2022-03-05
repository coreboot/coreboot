/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <commonlib/bsd/cbfs_mdata.h>
#include <commonlib/bsd/cbfs_private.h>
#include <commonlib/region.h>
#include <string.h>
#include <tests/lib/cbfs_util.h>
#include <tests/test.h>


static struct cbfs_boot_device cbd;

static u8 aligned_cbfs_buffer[(sizeof(struct cbfs_test_file) + CBFS_ALIGNMENT) * 10] __aligned(
	CBFS_ALIGNMENT);

static u8 *unaligned_cbfs_buffer = &aligned_cbfs_buffer[3];
static uintptr_t unaligned_cbfs_buffer_size = sizeof(aligned_cbfs_buffer) - 3;

static u8 cbfs_mcache[TEST_MCACHE_SIZE] __aligned(CBFS_MCACHE_ALIGNMENT);

/* Add files to CBFS buffer. NULL in files list equals to one CBFS_ALIGNMENT of spacing. */
static int create_cbfs(const struct cbfs_test_file *files[], const size_t nfiles, u8 *buffer,
		       const size_t buffer_size)
{
	u8 *data_ptr = buffer;
	size_t file_size = 0;
	memset(buffer, 0, buffer_size);

	for (size_t i = 0; i < nfiles; ++i) {
		if (files[i] == NULL) {
			file_size = CBFS_ALIGNMENT;
			assert_true(&data_ptr[file_size] < &buffer[buffer_size]);
		} else {
			file_size = be32_to_cpu(files[i]->header.len)
				    + be32_to_cpu(files[i]->header.offset);
			assert_true(&data_ptr[file_size] < &buffer[buffer_size]);
			memcpy(data_ptr, files[i], file_size);
		}

		data_ptr = &data_ptr[file_size];
		const uintptr_t offset = (uintptr_t)data_ptr - (uintptr_t)buffer;
		data_ptr = &buffer[ALIGN_UP(offset, CBFS_ALIGNMENT)];
	}

	return 0;
}

/* Mocks */

const struct cbfs_boot_device *cbfs_get_boot_device(bool force_ro)
{
	return &cbd;
}

size_t ulzman(const void *src, size_t srcn, void *dst, size_t dstn)
{
	check_expected(srcn);
	check_expected(dstn);
	memcpy(dst, src, dstn);
	return dstn;
}

size_t ulz4fn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	check_expected(srcn);
	check_expected(dstn);
	memcpy(dst, src, dstn);
	return dstn;
}

extern enum cb_err __real_cbfs_lookup(cbfs_dev_t dev, const char *name,
				      union cbfs_mdata *mdata_out, size_t *data_offset_out,
				      struct vb2_hash *metadata_hash);

enum cb_err cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			size_t *data_offset_out, struct vb2_hash *metadata_hash)
{
	const enum cb_err err =
		__real_cbfs_lookup(dev, name, mdata_out, data_offset_out, metadata_hash);
	assert_int_equal(err, mock_type(enum cb_err));
	return err;
}

extern enum cb_err __real_cbfs_mcache_lookup(const void *mcache, size_t mcache_size,
					  const char *name, union cbfs_mdata *mdata_out,
					  size_t *data_offset_out);

enum cb_err cbfs_mcache_lookup(const void *mcache, size_t mcache_size, const char *name,
			       union cbfs_mdata *mdata_out, size_t *data_offset_out)
{
	const enum cb_err err = __real_cbfs_mcache_lookup(mcache, mcache_size, name, mdata_out,
						       data_offset_out);
	assert_int_equal(err, mock_type(enum cb_err));
	return err;
}

extern void *__real_mem_pool_alloc(struct mem_pool *mp, size_t sz);

void *mem_pool_alloc(struct mem_pool *mp, size_t sz)
{
	check_expected(sz);
	assert_ptr_equal(mp, &cbfs_cache);
	return __real_mem_pool_alloc(mp, sz);
}

extern void __real_mem_pool_free(struct mem_pool *mp, void *p);

void mem_pool_free(struct mem_pool *mp, void *p)
{
	check_expected(p);
	assert_ptr_equal(mp, &cbfs_cache);
	return __real_mem_pool_free(mp, p);
}

static u8 cbmem_test_buf[2 * MiB];

void *cbmem_add(u32 id, u64 size)
{
	check_expected(id);
	check_expected(size);
	return cbmem_test_buf;
}

/* Setup, teardown and utils */

struct cbfs_test_state_ex {
	u32 file_type;
	u32 file_length;
	enum cb_err lookup_result;
};

struct cbfs_test_state {
	u8 *cbfs_buf;
	size_t cbfs_size;

	/* Optionals */
	struct cbfs_test_state_ex ex;
};

static int setup_test_cbfs_aligned(void **state)
{
	struct cbfs_test_state *s = malloc(sizeof(struct cbfs_test_state));

	if (!s)
		return 1;
	s->cbfs_buf = aligned_cbfs_buffer;
	s->cbfs_size = sizeof(aligned_cbfs_buffer);
	memset(&s->ex, 0, sizeof(s->ex));

	/* Prestate */
	if (*state != NULL)
		s->ex = *((struct cbfs_test_state_ex *)*state);

	*state = s;

	rdev_chain_mem(&cbd.rdev, aligned_cbfs_buffer, sizeof(aligned_cbfs_buffer));
	memset(aligned_cbfs_buffer, 0, sizeof(aligned_cbfs_buffer));

	cbd.mcache = cbfs_mcache;
	cbd.mcache_size = TEST_MCACHE_SIZE;

	return 0;
}

static int setup_test_cbfs_unaligned(void **state)
{
	struct cbfs_test_state *s = malloc(sizeof(struct cbfs_test_state));

	if (!s)
		return 1;
	s->cbfs_buf = unaligned_cbfs_buffer;
	s->cbfs_size = unaligned_cbfs_buffer_size;
	memset(&s->ex, 0, sizeof(s->ex));

	/* Prestate */
	if (*state != NULL)
		s->ex = *((struct cbfs_test_state_ex *)*state);

	*state = s;

	rdev_chain_mem(&cbd.rdev, unaligned_cbfs_buffer, unaligned_cbfs_buffer_size);
	memset(unaligned_cbfs_buffer, 0, unaligned_cbfs_buffer_size);

	cbd.mcache = cbfs_mcache;
	cbd.mcache_size = TEST_MCACHE_SIZE;

	return 0;
}

static int teardown_test_cbfs(void **state)
{
	free(*state);
	memset(&cbd, 0, sizeof(cbd));
	return 0;
}

/* Utils */

static void expect_lookup_result(enum cb_err res)
{
	if (CONFIG(NO_CBFS_MCACHE))
		will_return(cbfs_lookup, (res));
	else
		will_return(cbfs_mcache_lookup, (res));
}

/* Tests */

/* Test case for cbfs_map() function. Validate file searching in the correct CBFS */
static void test_cbfs_map(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_1, &test_file_2, NULL, &test_file_int_2,
		&test_file_1,	  NULL,		NULL, &test_file_int_3,
	};
	assert_int_equal(
		0, create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_buf, s->cbfs_size));
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	/* Existing files */
	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_1_SIZE, size_out);
	assert_memory_equal(mapping, test_data_1, TEST_DATA_1_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_value(ulzman, srcn, TEST_DATA_2_SIZE);
	expect_value(ulzman, dstn, TEST_DATA_2_SIZE);
	expect_value(mem_pool_alloc, sz, TEST_DATA_2_SIZE);
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_2_SIZE, size_out);
	assert_memory_equal(mapping, test_data_2, TEST_DATA_2_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_memory_equal(mapping, test_data_int_1, TEST_DATA_INT_1_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	/* Do not pass output pointer to size. It should work correctly. */
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, NULL);
	assert_non_null(mapping);
	assert_memory_equal(mapping, test_data_int_2, TEST_DATA_INT_2_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_value(ulz4fn, srcn, TEST_DATA_INT_3_SIZE);
	expect_value(ulz4fn, dstn, TEST_DATA_INT_3_SIZE);
	expect_value(mem_pool_alloc, sz, TEST_DATA_INT_3_SIZE);
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_3_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_3_SIZE, size_out);
	assert_memory_equal(mapping, test_data_int_3, TEST_DATA_INT_3_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	/* Nonexistent files */
	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map("unknown_fname", &size_out);
	assert_ptr_equal(NULL, mapping);
	assert_int_equal(0, size_out);

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map("", &size_out);
	assert_ptr_equal(NULL, mapping);
	assert_int_equal(0, size_out);
}

static void test_cbfs_cbmem_alloc(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	const struct cbfs_test_file *cbfs_files[] = {
		NULL, &test_file_1,	&test_file_2,	  &test_file_int_1,
		NULL, &test_file_int_2, &test_file_int_3, NULL,
	};
	assert_int_equal(
		0, create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_buf, s->cbfs_size));
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	/* Existing files */
	expect_lookup_result(CB_SUCCESS);
	expect_value(cbmem_add, id, 0x0101);
	expect_value(cbmem_add, size, TEST_DATA_1_SIZE);
	mapping = cbfs_cbmem_alloc(TEST_DATA_1_FILENAME, 0x0101, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_1_SIZE, size_out);
	assert_memory_equal(mapping, test_data_1, TEST_DATA_1_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	/* Do not pass output pointer to size. It should work correctly. */
	expect_value(ulzman, srcn, TEST_DATA_2_SIZE);
	expect_value(ulzman, dstn, TEST_DATA_2_SIZE);
	expect_lookup_result(CB_SUCCESS);
	expect_value(cbmem_add, id, 0x0102);
	expect_value(cbmem_add, size, TEST_DATA_2_SIZE);
	mapping = cbfs_cbmem_alloc(TEST_DATA_2_FILENAME, 0x0102, NULL);
	assert_non_null(mapping);
	assert_memory_equal(mapping, test_data_2, TEST_DATA_2_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	expect_value(cbmem_add, id, 0x0201);
	expect_value(cbmem_add, size, TEST_DATA_INT_1_SIZE);
	mapping = cbfs_cbmem_alloc(TEST_DATA_INT_1_FILENAME, 0x0201, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_memory_equal(mapping, test_data_int_1, TEST_DATA_INT_1_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	expect_value(cbmem_add, id, 0x0202);
	expect_value(cbmem_add, size, TEST_DATA_INT_2_SIZE);
	mapping = cbfs_cbmem_alloc(TEST_DATA_INT_2_FILENAME, 0x0202, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_2_SIZE, size_out);
	assert_memory_equal(mapping, test_data_int_2, TEST_DATA_INT_2_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_value(ulz4fn, srcn, TEST_DATA_INT_3_SIZE);
	expect_value(ulz4fn, dstn, TEST_DATA_INT_3_SIZE);
	expect_lookup_result(CB_SUCCESS);
	expect_value(cbmem_add, id, 0x0203);
	expect_value(cbmem_add, size, TEST_DATA_INT_2_SIZE);
	mapping = cbfs_cbmem_alloc(TEST_DATA_INT_3_FILENAME, 0x0203, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_3_SIZE, size_out);
	assert_memory_equal(mapping, test_data_int_3, TEST_DATA_INT_3_SIZE);

	expect_value(mem_pool_free, p, mapping);
	cbfs_unmap(mapping);

	/* Nonexistent files */
	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_cbmem_alloc("nothing-file", 0x0301, &size_out);
	assert_null(mapping);

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_cbmem_alloc("", 0x0302, &size_out);
	assert_null(mapping);
}

static void test_cbfs_image_not_aligned(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_1,
		&test_file_2,
	};
	assert_int_equal(0, create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), &s->cbfs_buf[5],
					s->cbfs_size - 5));
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_null(mapping);

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_file_not_aligned(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;

	memcpy(s->cbfs_buf, &test_file_int_2, sizeof(test_file_int_2));
	memcpy(&s->cbfs_buf[ALIGN_UP(sizeof(test_file_int_2), CBFS_ALIGNMENT) + 5],
	       &test_file_1, sizeof(test_file_1));
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_ptr_equal(mapping,
			 &s->cbfs_buf[offsetof(struct cbfs_test_file, attrs_and_data)]);

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_garbage_data_before_aligned_file(void **state)
{
	void *mapping;
	size_t size_out;
	const char garbage[] =
		"NOT so USEFUL DaTa BYTES that should have at least CBFS_ALIGNMENT bytes";
	const size_t garbage_sz = CBFS_ALIGNMENT;
	struct cbfs_test_state *s = *state;

	/* Garbage data size has to be aligned to CBFS_ALIGNMENT */
	memcpy(s->cbfs_buf, garbage, garbage_sz);
	memcpy(&s->cbfs_buf[garbage_sz], &test_file_int_2, sizeof(test_file_int_2));
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_ptr_equal(
		mapping,
		&s->cbfs_buf[garbage_sz + offsetof(struct cbfs_test_file, attrs_and_data)]);
}

static void test_cbfs_garbage_data_before_unaligned_file(void **state)
{
	void *mapping;
	size_t size_out;
	const char garbage[] =
		"NOT so USEFUL DaTa BYTES that should have at least CBFS_ALIGNMENT + 3 bytes";
	const size_t garbage_sz = CBFS_ALIGNMENT + 3;
	struct cbfs_test_state *s = *state;

	assert_true(garbage_sz == (CBFS_ALIGNMENT + 3));
	memcpy(s->cbfs_buf, garbage, garbage_sz);
	memcpy(&s->cbfs_buf[garbage_sz], &test_file_int_2, sizeof(test_file_int_2));
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_file_bigger_than_rdev(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	/* File with length equal to region_device size will go beyond it */
	f->header.len = cpu_to_be32(s->cbfs_size);

	/* Initialization and mcache building will succeed, because it only does access file
	   headers, and not actual data */
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	/* Lookup should not succeed, because data is too long, so reading it later would cause
	   memory access issues */
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_fail_beyond_rdev(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	size_t second_file_start = ALIGN_UP(sizeof(test_file_1), CBFS_ALIGNMENT);

	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	memcpy(&s->cbfs_buf[second_file_start], &test_file_2, s->ex.file_length);
	assert_true((second_file_start + s->ex.file_length) <= region_sz(&cbd.rdev.region));
	/* Adjust size of region device to cut everything after selected offset */
	cbd.rdev.region.size = second_file_start + s->ex.file_length;

	/* CBFS initialization should not fail if last file is not valid */
	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(test_file_1.header.offset)]);
	assert_int_equal(size_out, TEST_DATA_1_SIZE);

	size_out = 0;
	if (s->ex.lookup_result == CB_SUCCESS) {
		expect_value(ulzman, srcn, TEST_DATA_2_SIZE);
		expect_value(ulzman, dstn, TEST_DATA_2_SIZE);
		expect_value(mem_pool_alloc, sz, TEST_DATA_2_SIZE);
	}
	expect_lookup_result(s->ex.lookup_result);
	cbfs_map(TEST_DATA_2_FILENAME, &size_out);
}

static void test_cbfs_unaligned_file_in_the_middle(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	size_t second_file_start = ALIGN_UP(sizeof(test_file_1), CBFS_ALIGNMENT) + 5;
	size_t third_file_start =
		ALIGN_UP(sizeof(test_file_int_1) + second_file_start, CBFS_ALIGNMENT);

	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	memcpy(&s->cbfs_buf[second_file_start], &test_file_int_1, sizeof(test_file_int_1));
	memcpy(&s->cbfs_buf[third_file_start], &test_file_int_2, sizeof(test_file_int_2));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(test_file_1.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_1.header.len));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_null(mapping);

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_ptr_equal(
		mapping,
		&s->cbfs_buf[third_file_start + be32_to_cpu(test_file_int_2.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_int_2.header.len));
}

static void test_cbfs_overlapping_files(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	size_t second_file_start = ALIGN_UP(sizeof(test_file_1), CBFS_ALIGNMENT);
	size_t third_file_start =
		ALIGN_UP(sizeof(test_file_int_1) + second_file_start, CBFS_ALIGNMENT);
	size_t second_file_size =
		third_file_start + sizeof(test_file_int_2) - second_file_start;
	struct cbfs_test_file *f;

	/* Third file is inside second file, thus it should not be found */
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	memcpy(&s->cbfs_buf[second_file_start], &test_file_int_1, sizeof(test_file_int_1));
	memcpy(&s->cbfs_buf[third_file_start], &test_file_int_2, sizeof(test_file_int_2));
	f = (struct cbfs_test_file *)&s->cbfs_buf[second_file_start];
	f->header.len = cpu_to_be32(second_file_size);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(test_file_1.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_1.header.len));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_ptr_equal(
		mapping,
		&s->cbfs_buf[second_file_start + be32_to_cpu(test_file_int_1.header.offset)]);
	assert_int_equal(size_out, second_file_size);

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_incorrect_file_in_the_middle(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	size_t second_file_start = ALIGN_UP(sizeof(test_file_1), CBFS_ALIGNMENT);
	size_t third_file_start =
		ALIGN_UP(sizeof(test_file_int_1) + second_file_start, CBFS_ALIGNMENT);
	struct cbfs_test_file *f;

	/* Zero offset is illegal. File is not correct */
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	memcpy(&s->cbfs_buf[second_file_start], &test_file_int_1, sizeof(test_file_int_1));
	memcpy(&s->cbfs_buf[third_file_start], &test_file_int_2, sizeof(test_file_int_2));
	f = (struct cbfs_test_file *)&s->cbfs_buf[second_file_start];
	f->header.offset = cpu_to_be32(0);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(test_file_1.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_1.header.len));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_null(mapping);

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_ptr_equal(
		mapping,
		&s->cbfs_buf[third_file_start + be32_to_cpu(test_file_int_2.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_int_2.header.len));
}

static void test_cbfs_two_files_with_same_name(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	size_t second_file_start = ALIGN_UP(sizeof(test_file_1), CBFS_ALIGNMENT);
	size_t third_file_start =
		ALIGN_UP(sizeof(test_file_1) + second_file_start, CBFS_ALIGNMENT);

	/* Only first occurrence of file will be found */
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	memcpy(&s->cbfs_buf[second_file_start], &test_file_1, sizeof(test_file_1));
	memcpy(&s->cbfs_buf[third_file_start], &test_file_int_1, sizeof(test_file_int_1));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(test_file_1.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_1.header.len));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_ptr_equal(
		mapping,
		&s->cbfs_buf[third_file_start + be32_to_cpu(test_file_int_1.header.offset)]);
	assert_int_equal(size_out, be32_to_cpu(test_file_int_1.header.len));
}

static void test_cbfs_filename_not_terminated(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;
	const char fname[] = "abcdefghijklmnop";

	assert_true(sizeof(test_file_1.filename) == strlen(fname));
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	memcpy(f->filename, fname, strlen(fname));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	/* Filename is too long and does not include NULL-terminator. */
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(fname, &size_out);
	assert_null(mapping);
}

static void test_cbfs_filename_terminated_but_too_long(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	/* Filename length in header offset field is too short by one to include
	   NULL-terminator of filename */
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.offset = cpu_to_be32(offsetof(struct cbfs_test_file, filename)
				       + strlen(TEST_DATA_1_FILENAME));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_attributes_offset_larger_than_offset(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	/* Require attributes for this test */
	assert_true(be32_to_cpu(test_file_2.header.attributes_offset) != 0);
	memcpy(s->cbfs_buf, &test_file_2, sizeof(test_file_2));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.attributes_offset = cpu_to_be32(sizeof(struct cbfs_file) + FILENAME_SIZE
						  + sizeof(struct cbfs_file_attr_compression));
	f->header.offset = cpu_to_be32(sizeof(struct cbfs_file) + FILENAME_SIZE);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_attributes_offset_cut_off_at_len(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	/* Require attributes for this test */
	assert_true(be32_to_cpu(test_file_2.header.attributes_offset) != 0);
	memcpy(s->cbfs_buf, &test_file_2, sizeof(test_file_2));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.attributes_offset =
		cpu_to_be32(offsetof(struct cbfs_test_file, attrs_and_data)
			    + offsetof(struct cbfs_file_attribute, len));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	/* No attributes will be found, because attributes_offset value is too big to cover
	   cbfs_file_attribute tag. Compression attribute of ths file will not be found, and
	   that is why there is no need to call expect_value(ulzma).
	   However, file will be found, because the offset is correct. */
	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(f->header.offset)]);
	assert_int_equal(size_out, TEST_DATA_2_SIZE);
}

static void test_cbfs_attributes_offset_cut_off_at_data(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	/* Require attributes for this test */
	assert_true(be32_to_cpu(test_file_2.header.attributes_offset) != 0);
	memcpy(s->cbfs_buf, &test_file_2, sizeof(test_file_2));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.attributes_offset = cpu_to_be32(sizeof(struct cbfs_file) + FILENAME_SIZE
						  + offsetof(struct cbfs_file_attribute, data));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	/* No attributes will be found, because attributes_offset value is too big to cover
	   cbfs_file_attribute tag and length. Compression attribute of ths file will not be
	   found, and that is why there is no need to call expect_value(ulzma).
	   However, file will be found, because the offset is correct. */
	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(f->header.offset)]);
	assert_int_equal(size_out, TEST_DATA_2_SIZE);
}

static void test_cbfs_attributes_offset_smaller_than_file_struct(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	assert_true(be32_to_cpu(test_file_2.header.attributes_offset) != 0);
	memcpy(s->cbfs_buf, &test_file_2, sizeof(test_file_2));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.attributes_offset = cpu_to_be32(sizeof(struct cbfs_file) / 2);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_offset_smaller_than_header_size(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	assert_true(be32_to_cpu(test_file_int_1.header.attributes_offset) == 0);
	memcpy(s->cbfs_buf, &test_file_int_1, sizeof(test_file_int_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.offset = cpu_to_be32(sizeof(struct cbfs_file) / 2);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_attributes_offset_is_zero(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;

	assert_true(be32_to_cpu(test_file_int_1.header.attributes_offset) == 0);
	memcpy(s->cbfs_buf, &test_file_int_1, sizeof(test_file_int_1));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_SUCCESS);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_ptr_equal(mapping, &s->cbfs_buf[be32_to_cpu(test_file_int_1.header.offset)]);
}

static void test_cbfs_offset_is_zero(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	assert_true(be32_to_cpu(test_file_int_1.header.attributes_offset) == 0);
	memcpy(s->cbfs_buf, &test_file_int_1, sizeof(test_file_int_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.offset = cpu_to_be32(0);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_attributes_too_large(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	assert_true(be32_to_cpu(test_file_2.header.attributes_offset) != 0);
	memcpy(s->cbfs_buf, &test_file_2, sizeof(test_file_2));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	/* Offset determines size of header and attributes. CBFS module uses cbfs_mdata union to
	   store it, so offset (thus attributes) bigger than it should cause an error in the
	   lookup code. */
	f->header.offset =
		cpu_to_be32(be32_to_cpu(f->header.offset) + sizeof(union cbfs_mdata));

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_null(mapping);
}

/* Requires cbfs_test_state.ex.file_length to be set */
static void test_cbfs_file_length(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	assert_true(be32_to_cpu(test_file_1.header.attributes_offset) == 0);
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.len = cpu_to_be32(s->ex.file_length);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_attributes_offset_uint32_max(void **state)
{
	void *mapping;
	size_t size_out;
	struct cbfs_test_state *s = *state;
	struct cbfs_test_file *f;

	assert_true(be32_to_cpu(test_file_1.header.attributes_offset) == 0);
	memcpy(s->cbfs_buf, &test_file_1, sizeof(test_file_1));
	f = (struct cbfs_test_file *)s->cbfs_buf;
	f->header.attributes_offset = cpu_to_be32(UINT32_MAX);

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, NULL));

	size_out = 0;
	expect_lookup_result(CB_CBFS_NOT_FOUND);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_null(mapping);
}

#define CBFS_LOOKUP_NAME_SETUP_PRESTATE_COMMON_TEST(name, test_fn, setup_fn, prestate)         \
	{                                                                                      \
		(name), (test_fn), (setup_fn), teardown_test_cbfs, (prestate),                 \
	}

#define CBFS_LOOKUP_NAME_PRESTATE_TEST(name, test_fn, prestate)                                \
	EMPTY_WRAP(                                                                            \
		CBFS_LOOKUP_NAME_SETUP_PRESTATE_COMMON_TEST(                                   \
			("aligned, " name), (test_fn), setup_test_cbfs_aligned, (prestate)),   \
		CBFS_LOOKUP_NAME_SETUP_PRESTATE_COMMON_TEST(("unaligned, " name), (test_fn),   \
							    setup_test_cbfs_unaligned,         \
							    (prestate)))

#define CBFS_LOOKUP_TEST(test_fn) CBFS_LOOKUP_NAME_PRESTATE_TEST(#test_fn, test_fn, NULL)

#define CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV(name, file_len, lookup_res)                          \
	EMPTY_WRAP(CBFS_LOOKUP_NAME_PRESTATE_TEST(name ", CBFS_TYPE_RAW",                      \
						  test_cbfs_fail_beyond_rdev,                  \
						  (&(struct cbfs_test_state_ex){               \
							  .file_type = CBFS_TYPE_RAW,          \
							  .file_length = (file_len),           \
							  .lookup_result = (lookup_res),       \
						  })),                                         \
		   CBFS_LOOKUP_NAME_PRESTATE_TEST(name ", CBFS_TYPE_NULL",                     \
						  test_cbfs_fail_beyond_rdev,                  \
						  (&(struct cbfs_test_state_ex){               \
							  .file_type = CBFS_TYPE_NULL,         \
							  .file_length = (file_len),           \
							  .lookup_result = (lookup_res),       \
						  })))

#define CBFS_LOOKUP_TEST_FILE_LENGTH(file_len)                                                 \
	CBFS_LOOKUP_NAME_PRESTATE_TEST("test_cbfs_file_length, " #file_len,                    \
				       test_cbfs_file_length,                                  \
				       (&(struct cbfs_test_state_ex){                          \
					       .file_length = (file_len),                      \
				       }))

int main(void)
{
	const struct CMUnitTest cbfs_lookup_aligned_and_unaligned_tests[] = {
		CBFS_LOOKUP_TEST(test_cbfs_map),
		CBFS_LOOKUP_TEST(test_cbfs_cbmem_alloc),

		CBFS_LOOKUP_TEST(test_cbfs_image_not_aligned),
		CBFS_LOOKUP_TEST(test_cbfs_file_not_aligned),

		CBFS_LOOKUP_TEST(test_cbfs_garbage_data_before_aligned_file),
		CBFS_LOOKUP_TEST(test_cbfs_garbage_data_before_unaligned_file),

		CBFS_LOOKUP_TEST(test_cbfs_file_bigger_than_rdev),

		/* Correct file */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV("File fitting in rdev",
						  sizeof(struct cbfs_test_file), CB_SUCCESS),

		/* Attributes beyond rdev */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV(
			"Attributes and data beyond rdev",
			offsetof(struct cbfs_test_file, attrs_and_data), CB_CBFS_NOT_FOUND),

		/* Attributes except tag beyond rdev */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV(
			"Attributes except tag beyond rdev",
			offsetof(struct cbfs_test_file, attrs_and_data)
				- offsetof(struct cbfs_file_attribute, len),
			CB_CBFS_NOT_FOUND),

		/* Attributes except tag and len beyond rdev */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV(
			"Attributes except tag and len beyond rdev",
			offsetof(struct cbfs_test_file, attrs_and_data)
				- offsetof(struct cbfs_file_attribute, data),
			CB_CBFS_NOT_FOUND),

		/* Filename beyond rdev */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV("Filename beyond rdev",
						  offsetof(struct cbfs_test_file, filename),
						  CB_CBFS_NOT_FOUND),

		/* Part of filename beyond rdev */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV("Part of filename beyond rdev",
						  offsetof(struct cbfs_test_file, filename)
							  + FILENAME_SIZE / 2,
						  CB_CBFS_NOT_FOUND),

		/* Part of cbfs_file struct beyond rdev */
		CBFS_LOOKUP_TEST_FAIL_BEYOND_RDEV("Part of cbfs_file struct beyond rdev",
						  offsetof(struct cbfs_test_file, filename) / 2,
						  CB_CBFS_NOT_FOUND),

		CBFS_LOOKUP_TEST(test_cbfs_unaligned_file_in_the_middle),
		CBFS_LOOKUP_TEST(test_cbfs_overlapping_files),
		CBFS_LOOKUP_TEST(test_cbfs_incorrect_file_in_the_middle),

		CBFS_LOOKUP_TEST(test_cbfs_two_files_with_same_name),

		CBFS_LOOKUP_TEST(test_cbfs_filename_not_terminated),
		CBFS_LOOKUP_TEST(test_cbfs_filename_terminated_but_too_long),

		CBFS_LOOKUP_TEST(test_cbfs_attributes_offset_larger_than_offset),
		CBFS_LOOKUP_TEST(test_cbfs_attributes_offset_cut_off_at_len),
		CBFS_LOOKUP_TEST(test_cbfs_attributes_offset_cut_off_at_data),

		CBFS_LOOKUP_TEST(test_cbfs_attributes_offset_smaller_than_file_struct),

		CBFS_LOOKUP_TEST(test_cbfs_offset_smaller_than_header_size),
		CBFS_LOOKUP_TEST(test_cbfs_attributes_offset_is_zero),
		CBFS_LOOKUP_TEST(test_cbfs_offset_is_zero),
		CBFS_LOOKUP_TEST(test_cbfs_attributes_too_large),

		CBFS_LOOKUP_TEST_FILE_LENGTH(UINT32_MAX),
		CBFS_LOOKUP_TEST_FILE_LENGTH(UINT32_MAX
					     - offsetof(struct cbfs_test_file, attrs_and_data)),
		CBFS_LOOKUP_TEST_FILE_LENGTH(
			UINT32_MAX - offsetof(struct cbfs_test_file, attrs_and_data) / 2),
		CBFS_LOOKUP_TEST_FILE_LENGTH(
			UINT32_MAX - offsetof(struct cbfs_test_file, attrs_and_data) * 2),
		CBFS_LOOKUP_TEST_FILE_LENGTH(
			UINT32_MAX - offsetof(struct cbfs_test_file, attrs_and_data) - 1),
		CBFS_LOOKUP_TEST_FILE_LENGTH(
			UINT32_MAX - offsetof(struct cbfs_test_file, attrs_and_data) + 1),

		CBFS_LOOKUP_TEST(test_cbfs_attributes_offset_uint32_max),
	};

	return cb_run_group_tests(cbfs_lookup_aligned_and_unaligned_tests, NULL, NULL);
}
