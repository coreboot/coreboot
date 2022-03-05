/* SPDX-License-Identifier: GPL-2.0.-only */

#include <libpayload-config.h>
#include <cbfs.h>
#include <cbfs_glue.h>
#include <commonlib/bsd/cb_err.h>
#include <commonlib/bsd/cbfs_mdata.h>
#include <endian.h>
#include <mocks/cbfs_util.h>
#include <stdlib.h>
#include <string.h>
#include <sysinfo.h>
#include <tests/test.h>

#include "../libcbfs/cbfs.c"

/* Mocks */

unsigned long virtual_offset = 0;
struct sysinfo_t lib_sysinfo;

unsigned long ulzman(const unsigned char *src, unsigned long srcn, unsigned char *dst,
		     unsigned long dstn)
{
	assert_true(dstn != 0);
	check_expected(srcn);
	check_expected(dstn);
	memcpy(dst, src, dstn);
	return dstn;
}

size_t ulz4fn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	assert_non_null(dstn);
	check_expected(srcn);
	check_expected(dstn);
	memcpy(dst, src, dstn);
	return dstn;
}

static size_t test_fmap_offset = 0;
static size_t test_fmap_size = 0;
static enum cb_err test_fmap_result = CB_SUCCESS;

static void set_fmap_locate_area_results(size_t offset, size_t size, size_t result)
{
	test_fmap_offset = offset;
	test_fmap_size = size;
	test_fmap_result = result;
}

enum cb_err fmap_locate_area(const char *name, size_t *offset, size_t *size)
{
	*offset = test_fmap_offset;
	*size = test_fmap_size;
	return test_fmap_result;
}

enum cb_err cbfs_mcache_lookup(const void *mcache, size_t mcache_size, const char *name,
			       union cbfs_mdata *mdata_out, size_t *data_offset_out)
{
	assert_non_null(mcache);
	assert_true(mcache_size > 0 && mcache_size % CBFS_MCACHE_ALIGNMENT == 0);
	assert_non_null(mdata_out);
	assert_non_null(data_offset_out);

	check_expected(name);

	enum cb_err ret = mock_type(enum cb_err);
	if (ret != CB_SUCCESS)
		return ret;

	memcpy(mdata_out, mock_ptr_type(const union cbfs_mdata *), sizeof(union cbfs_mdata));
	*data_offset_out = mock_type(size_t);
	return CB_SUCCESS;
}

static void expect_cbfs_mcache_lookup(const char *name, enum cb_err err,
				      const union cbfs_mdata *mdata, size_t data_offset_out)
{
	expect_string(cbfs_mcache_lookup, name, name);
	will_return(cbfs_mcache_lookup, err);

	if (err == CB_SUCCESS) {
		will_return(cbfs_mcache_lookup, mdata);
		will_return(cbfs_mcache_lookup, data_offset_out);
	}
}

enum cb_err cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			size_t *data_offset_out, struct vb2_hash *metadata_hash)
{
	assert_non_null(dev);
	check_expected(name);

	enum cb_err ret = mock_type(enum cb_err);
	if (ret != CB_SUCCESS)
		return ret;

	memcpy(mdata_out, mock_ptr_type(const union cbfS_mdata *), sizeof(union cbfs_mdata));
	*data_offset_out = mock_type(size_t);
	return CB_SUCCESS;
}

static void expect_cbfs_lookup(const char *name, enum cb_err err, const union cbfs_mdata *mdata,
			       size_t data_offset_out)
{
	expect_string(cbfs_lookup, name, name);
	will_return(cbfs_lookup, err);

	if (err == CB_SUCCESS) {
		will_return(cbfs_lookup, mdata);
		will_return(cbfs_lookup, data_offset_out);
	}
}

const void *cbfs_find_attr(const union cbfs_mdata *mdata, uint32_t attr_tag, size_t size_check)
{
	return mock_ptr_type(void *);
}

static bool force_single_boot_device_size_failure = false;

ssize_t boot_device_read(void *buf, size_t offset, size_t size)
{
	memcpy(buf, (void *)offset, size);
	if (force_single_boot_device_size_failure) {
		force_single_boot_device_size_failure = false;
		return CB_ERR;
	}
	return size;
}

/* Utils */

static size_t get_cbfs_file_size(const void *file_ptr)
{
	const struct cbfs_file *f = file_ptr;
	return be32toh(f->offset) + be32toh(f->len);
}

static void create_cbfs(const struct cbfs_test_file *files[], const size_t nfiles,
			uint8_t *buffer, const size_t buffer_size)
{
	uint8_t *data_ptr = buffer;
	size_t file_size = 0;
	memset(buffer, 0, buffer_size);
	for (size_t i = 0; i < nfiles; ++i) {
		if (files[i] == NULL) {
			file_size = CBFS_ALIGNMENT;
			assert_true(&data_ptr[file_size] < &buffer[buffer_size]);
		} else {
			file_size = get_cbfs_file_size(files[i]);
			assert_true(&data_ptr[file_size] < &buffer[buffer_size]);
			memcpy(data_ptr, files[i], file_size);
		}
		data_ptr = &data_ptr[file_size];
		data_ptr = &buffer[ALIGN_UP((uintptr_t)data_ptr - (uintptr_t)buffer,
					    CBFS_ALIGNMENT)];
	}
}

static size_t get_created_cbfs_file_start_offset(const struct cbfs_test_file *files[],
						 const size_t nfile)
{
	size_t offset_out = 0;
	size_t offset = 0;
	for (size_t i = 0; i < nfile; ++i) {
		offset = files[i] ? get_cbfs_file_size(files[i]) : CBFS_ALIGNMENT;
		offset_out = ALIGN_UP(offset_out + offset, CBFS_ALIGNMENT);
	}
	return offset_out;
}

/* Setup */

static uint8_t
	aligned_cbfs_ro_buffer[(sizeof(struct cbfs_test_file) + CBFS_ALIGNMENT * 50)] __aligned(
		CBFS_ALIGNMENT);
static const size_t aligned_cbfs_ro_buffer_size = sizeof(aligned_cbfs_ro_buffer);
static uint8_t
	aligned_cbfs_rw_buffer[(sizeof(struct cbfs_test_file) + CBFS_ALIGNMENT * 50)] __aligned(
		CBFS_ALIGNMENT);
static const size_t aligned_cbfs_rw_buffer_size = sizeof(aligned_cbfs_rw_buffer);

static uint8_t *unaligned_cbfs_ro_buffer = &aligned_cbfs_ro_buffer[5];
static const size_t unaligned_cbfs_ro_buffer_size = aligned_cbfs_ro_buffer_size - 5;
static uint8_t *unaligned_cbfs_rw_buffer = &aligned_cbfs_rw_buffer[5];
static const size_t unaligned_cbfs_rw_buffer_size = aligned_cbfs_rw_buffer_size - 5;

struct cbfs_test_state {
	uint8_t *cbfs_ro_buf;
	uint64_t cbfs_ro_size;
	uint8_t *cbfs_rw_buf;
	uint64_t cbfs_rw_size;

	size_t mcache_ro_offset;
	size_t mcache_ro_size;
	size_t mcache_rw_offset;
	size_t mcache_rw_size;

	struct cbfs_test_setup {
		bool unaligned;
		bool init_ro;
		bool init_rw;
	} ex;
};


/* Because of how CMocka works, it should be called in the test function, or in the setup
   function only if CBFS API capable of initializing RO CBFS boot device is called. */
static void setup_cbfs_boot_device(struct cbfs_test_state *s)
{
	set_fmap_locate_area_results(0, 0, CB_SUCCESS);
	lib_sysinfo.cbfs_ro_mcache_offset = 0;
	lib_sysinfo.cbfs_ro_mcache_size = 0;
	memset((void *)cbfs_get_boot_device(true), 0, sizeof(struct cbfs_boot_device));
	if (s->ex.init_ro) {
		set_fmap_locate_area_results((size_t)s->cbfs_ro_buf, s->cbfs_ro_size,
					     CB_SUCCESS);
		lib_sysinfo.cbfs_ro_mcache_offset = s->mcache_ro_offset;
		lib_sysinfo.cbfs_ro_mcache_size = s->mcache_ro_size;
	}

	lib_sysinfo.cbfs_offset = 0;
	lib_sysinfo.cbfs_size = 0;
	lib_sysinfo.cbfs_rw_mcache_offset = 0;
	lib_sysinfo.cbfs_rw_mcache_size = 0;
	memset((void *)cbfs_get_boot_device(false), 0, sizeof(struct cbfs_boot_device));
	if (s->ex.init_rw) {
		lib_sysinfo.cbfs_offset = (uint64_t)s->cbfs_rw_buf;
		lib_sysinfo.cbfs_size = s->cbfs_rw_size;
		lib_sysinfo.cbfs_rw_mcache_offset = s->mcache_rw_offset;
		lib_sysinfo.cbfs_rw_mcache_size = s->mcache_rw_size;
	}
}

static int setup_cbfs_test(void **state)
{
	struct cbfs_test_state *s = calloc(1, sizeof(*s));

	if (!s)
		return 1;

	if (*state)
		memcpy(&s->ex, *state, sizeof(s->ex));

	if (s->ex.init_ro) {
		if (s->ex.unaligned) {
			s->cbfs_ro_buf = unaligned_cbfs_ro_buffer;
			s->cbfs_ro_size = unaligned_cbfs_ro_buffer_size;
		} else {
			s->cbfs_ro_buf = aligned_cbfs_ro_buffer;
			s->cbfs_ro_size = aligned_cbfs_ro_buffer_size;
		}
	}

	if (s->ex.init_rw) {
		if (s->ex.unaligned) {
			s->cbfs_rw_buf = unaligned_cbfs_rw_buffer;
			s->cbfs_rw_size = unaligned_cbfs_rw_buffer_size;
		} else {
			s->cbfs_rw_buf = aligned_cbfs_rw_buffer;
			s->cbfs_rw_size = aligned_cbfs_rw_buffer_size;
		}
	}

	*state = s;

	return 0;
}

static int teardown_cbfs_test(void **state)
{
	if (*state)
		free(*state);

	return 0;
}

/* Tests */

static void test_cbfs_boot_device_init(void **state)
{
	const struct cbfs_boot_device *cbd = NULL;

	/* No valid RO, should fail */
	set_fmap_locate_area_results(0, 0, CB_ERR);
	lib_sysinfo.cbfs_offset = 0;
	lib_sysinfo.cbfs_size = 0;
	lib_sysinfo.cbfs_rw_mcache_size = 0;
	lib_sysinfo.cbfs_rw_mcache_offset = 0;
	lib_sysinfo.cbfs_ro_mcache_offset = 0;
	lib_sysinfo.cbfs_ro_mcache_size = 0;
	assert_int_equal(NULL, cbfs_get_boot_device(true));
	assert_null(cbfs_ro_map("file", NULL));

	/* Valid RO */
	set_fmap_locate_area_results(0x12345678, 0x90ABCDEF, CB_SUCCESS);
	lib_sysinfo.cbfs_ro_mcache_offset = 0x600D41C3;
	lib_sysinfo.cbfs_ro_mcache_size = 0xBADBEEFF;
	cbd = cbfs_get_boot_device(true);
	assert_non_null(cbd);
	assert_int_equal(0x12345678, cbd->dev.offset);
	assert_int_equal(0x90ABCDEF, cbd->dev.size);
	assert_int_equal(0xBADBEEFF, cbd->mcache_size);
	assert_int_equal(0x600D41C3, cbd->mcache);

	lib_sysinfo.cbfs_offset = 0xAABBCCDD;
	lib_sysinfo.cbfs_size = 0x1000;
	lib_sysinfo.cbfs_rw_mcache_offset = 0x8F8F8F8F;
	lib_sysinfo.cbfs_rw_mcache_size = 0x500;
	cbd = cbfs_get_boot_device(false);
	assert_non_null(cbd);
	assert_int_equal(0xAABBCCDD, cbd->dev.offset);
	assert_int_equal(0x1000, cbd->dev.size);
	assert_int_equal(0x8F8F8F8F, cbd->mcache);
	assert_int_equal(0x500, cbd->mcache_size);
}

/* This test checks cbfs_map() basic cases and covers only RW CBFS. */
void test_cbfs_map(void **state)
{
	struct cbfs_test_state *s = *state;
	void *mapping = NULL;
	size_t size_out = 0;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_1, &test_file_2, NULL, &test_file_int_3,
		&test_file_int_2, NULL,		NULL, &test_file_1,
	};
	uint8_t *cbfs_buf = NULL;
	size_t foffset = 0;

	setup_cbfs_boot_device(s);
	cbfs_buf = s->cbfs_rw_buf;
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_rw_buf, s->cbfs_rw_size);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 0);
	expect_cbfs_lookup(TEST_DATA_INT_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_memory_equal(test_data_int_1, mapping, TEST_DATA_INT_1_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 1);
	expect_cbfs_lookup(TEST_DATA_2_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_2.header.offset));
	will_return(cbfs_find_attr, &test_file_2.attrs_and_data);
	expect_value(ulzman, srcn, TEST_DATA_2_SIZE);
	expect_value(ulzman, dstn, TEST_DATA_2_SIZE);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_2_SIZE, size_out);
	assert_memory_equal(test_data_2, mapping, TEST_DATA_2_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 3);
	expect_cbfs_lookup(TEST_DATA_INT_3_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_3.header.offset));
	will_return(cbfs_find_attr, &test_file_int_3.attrs_and_data);
	expect_value(ulz4fn, srcn, TEST_DATA_INT_3_SIZE);
	expect_value(ulz4fn, dstn, TEST_DATA_INT_3_SIZE);
	mapping = cbfs_map(TEST_DATA_INT_3_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_3_SIZE, size_out);
	assert_memory_equal(test_data_int_3, mapping, TEST_DATA_INT_3_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 4);
	expect_cbfs_lookup(TEST_DATA_INT_2_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_2.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_map(TEST_DATA_INT_2_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_2_SIZE, size_out);
	assert_memory_equal(test_data_int_2, mapping, TEST_DATA_INT_2_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 7);
	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_1_SIZE, size_out);
	assert_memory_equal(test_data_1, mapping, TEST_DATA_1_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_cbfs_lookup("invalid_file", CB_CBFS_NOT_FOUND, 0, 0);
	if (s->ex.init_rw && CONFIG(LP_ENABLE_CBFS_FALLBACK))
		expect_cbfs_lookup("invalid_file", CB_CBFS_NOT_FOUND, 0, 0);
	mapping = cbfs_map("invalid_file", &size_out);
	assert_null(mapping);
}

static void test_cbfs_invalid_compression_algo(void **state)
{
	struct cbfs_test_state *s = *state;
	void *mapping = NULL;
	size_t size_out = 0;
	uint8_t *cbfs_buf = NULL;
	struct cbfs_test_file *f;
	struct cbfs_file_attr_compression *comp;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_2,
	};

	setup_cbfs_boot_device(s);
	cbfs_buf = s->cbfs_rw_buf;
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_rw_buf, s->cbfs_rw_size);

	f = (struct cbfs_test_file *)cbfs_buf;
	comp = (struct cbfs_file_attr_compression *)&f->attrs_and_data[0];
	comp->compression = 0xFFFFFFF0;

	size_out = 0;
	expect_cbfs_lookup(TEST_DATA_2_FILENAME, CB_SUCCESS, (const union cbfs_mdata *)cbfs_buf,
			   be32toh(test_file_1.header.offset));
	will_return(cbfs_find_attr, comp);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_null(mapping);
}

static void test_cbfs_io_error(void **state)
{
	struct cbfs_test_state *s = *state;
	setup_cbfs_boot_device(s);

	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_CBFS_IO, 0, 0);
	assert_null(cbfs_map(TEST_DATA_1_FILENAME, NULL));
}

static void test_cbfs_successful_fallback_to_ro(void **state)
{
	struct cbfs_test_state *s = *state;
	void *mapping = NULL;
	size_t size_out = 0;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_1,	  &test_file_2,	    &test_file_int_1,
		&test_file_int_1, &test_file_int_2, &test_file_int_3,
	};
	uint8_t *cbfs_buf = NULL;
	size_t foffset = 0;

	if (!CONFIG(LP_ENABLE_CBFS_FALLBACK)) {
		print_message("Skipping test, because LP_ENABLE_CBFS_FALLBACK == 0\n");
		skip();
	}

	setup_cbfs_boot_device(s);
	cbfs_buf = s->cbfs_ro_buf;
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_ro_buf, s->cbfs_ro_size);
	if (s->ex.init_rw)
		create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files) - 2, s->cbfs_rw_buf,
			    s->cbfs_rw_size);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 1);
	expect_cbfs_lookup(TEST_DATA_2_FILENAME, CB_CBFS_NOT_FOUND, 0, 0);
	expect_cbfs_lookup(TEST_DATA_2_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_2.header.offset));
	will_return(cbfs_find_attr, &test_file_2.attrs_and_data);
	expect_value(ulzman, srcn, TEST_DATA_2_SIZE);
	expect_value(ulzman, dstn, TEST_DATA_2_SIZE);
	mapping = cbfs_map(TEST_DATA_2_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_2_SIZE, size_out);
	assert_memory_equal(test_data_2, mapping, TEST_DATA_2_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 5);
	expect_cbfs_lookup(TEST_DATA_INT_3_FILENAME, CB_CBFS_NOT_FOUND, 0, 0);
	expect_cbfs_lookup(TEST_DATA_INT_3_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_3.header.offset));
	will_return(cbfs_find_attr, &test_file_int_3.attrs_and_data);
	expect_value(ulz4fn, srcn, TEST_DATA_INT_3_SIZE);
	expect_value(ulz4fn, dstn, TEST_DATA_INT_3_SIZE);
	mapping = cbfs_map(TEST_DATA_INT_3_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_3_SIZE, size_out);
	assert_memory_equal(test_data_int_3, mapping, TEST_DATA_INT_3_SIZE);
	cbfs_unmap(mapping);
}

static void test_cbfs_load(void **state)
{
	struct cbfs_test_state *s = *state;
	size_t size_out = 0;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_1, &test_file_2, NULL, &test_file_int_3,
		&test_file_int_2, NULL,		NULL, &test_file_1,
	};
	uint8_t *cbfs_buf = NULL;
	uint8_t load_buf[1 * KiB];
	size_t foffset = 0;

	setup_cbfs_boot_device(s);
	cbfs_buf = s->cbfs_rw_buf;
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_rw_buf, s->cbfs_rw_size);

	/* Successful load */
	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 0);
	expect_cbfs_lookup(TEST_DATA_INT_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	size_out = cbfs_load(TEST_DATA_INT_1_FILENAME, load_buf, sizeof(load_buf));
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_memory_equal(test_data_int_1, load_buf, TEST_DATA_INT_1_SIZE);

	/* Buffer too small */
	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 7);
	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	size_out = cbfs_load(TEST_DATA_1_FILENAME, load_buf, TEST_DATA_1_SIZE / 2);
	assert_int_equal(0, size_out);
}

static void test_cbfs_map_with_mcache(void **state)
{
	struct cbfs_test_state *s = *state;
	void *mapping = NULL;
	size_t size_out = 0;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_2, &test_file_1,	    NULL,
		&test_file_int_3, &test_file_int_1, &test_file_2,
	};
	uint8_t *cbfs_buf = NULL;
	size_t foffset = 0;

	/* Will not be accessed, just needs to be valid. */
	s->mcache_ro_offset = ALIGN_UP(0x1000, CBFS_MCACHE_ALIGNMENT);
	s->mcache_ro_size = ALIGN_UP(0x500, CBFS_MCACHE_ALIGNMENT);
	s->mcache_rw_offset = ALIGN_UP(0x3000, CBFS_MCACHE_ALIGNMENT);
	s->mcache_rw_size = ALIGN_UP(0x600, CBFS_MCACHE_ALIGNMENT);
	setup_cbfs_boot_device(s);
	cbfs_buf = s->cbfs_rw_buf;
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_rw_buf, s->cbfs_rw_size);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 4);
	expect_cbfs_mcache_lookup(TEST_DATA_INT_1_FILENAME, CB_SUCCESS,
				  (const union cbfs_mdata *)&cbfs_buf[foffset],
				  foffset + be32toh(test_file_int_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_map(TEST_DATA_INT_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_memory_equal(test_data_int_1, mapping, TEST_DATA_INT_1_SIZE);
	cbfs_unmap(mapping);
}

static void test_cbfs_boot_device_read_failure(void **state)
{
	struct cbfs_test_state *s = *state;
	void *mapping = NULL;
	size_t size_out = 0;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_3, &test_file_1,	    NULL,
		&test_file_int_3, &test_file_int_1, &test_file_2,
	};
	uint8_t *cbfs_buf = NULL;
	size_t foffset = 0;

	setup_cbfs_boot_device(s);
	cbfs_buf = s->cbfs_rw_buf;
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_rw_buf, s->cbfs_rw_size);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 1);
	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	force_single_boot_device_size_failure = true;
	mapping = cbfs_map(TEST_DATA_1_FILENAME, &size_out);
	assert_null(mapping);
}

/* This test uses RW CBFS only */
static void test_cbfs_unverified_area_map(void **state)
{
	struct cbfs_test_state *s = *state;
	void *mapping = NULL;
	size_t size_out = 0;
	const struct cbfs_test_file *cbfs_files[] = {
		&test_file_int_1, &test_file_2, NULL, &test_file_int_3,
		&test_file_int_2, NULL,		NULL, &test_file_1,
	};
	uint8_t *cbfs_buf = NULL;
	size_t foffset = 0;

	cbfs_buf = s->cbfs_rw_buf;
	set_fmap_locate_area_results((size_t)cbfs_buf, s->cbfs_rw_size, CB_SUCCESS);
	create_cbfs(cbfs_files, ARRAY_SIZE(cbfs_files), s->cbfs_rw_buf, s->cbfs_rw_size);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 0);
	expect_cbfs_lookup(TEST_DATA_INT_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_unverified_area_map("TEST_AREA", TEST_DATA_INT_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_1_SIZE, size_out);
	assert_memory_equal(test_data_int_1, mapping, TEST_DATA_INT_1_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 1);
	expect_cbfs_lookup(TEST_DATA_2_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_2.header.offset));
	will_return(cbfs_find_attr, &test_file_2.attrs_and_data);
	expect_value(ulzman, srcn, TEST_DATA_2_SIZE);
	expect_value(ulzman, dstn, TEST_DATA_2_SIZE);
	mapping = cbfs_unverified_area_map("TEST_AREA", TEST_DATA_2_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_2_SIZE, size_out);
	assert_memory_equal(test_data_2, mapping, TEST_DATA_2_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 3);
	expect_cbfs_lookup(TEST_DATA_INT_3_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_3.header.offset));
	will_return(cbfs_find_attr, &test_file_int_3.attrs_and_data);
	expect_value(ulz4fn, srcn, TEST_DATA_INT_3_SIZE);
	expect_value(ulz4fn, dstn, TEST_DATA_INT_3_SIZE);
	mapping = cbfs_unverified_area_map("TEST_AREA", TEST_DATA_INT_3_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_3_SIZE, size_out);
	assert_memory_equal(test_data_int_3, mapping, TEST_DATA_INT_3_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 4);
	expect_cbfs_lookup(TEST_DATA_INT_2_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_int_2.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_unverified_area_map("TEST_AREA", TEST_DATA_INT_2_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_INT_2_SIZE, size_out);
	assert_memory_equal(test_data_int_2, mapping, TEST_DATA_INT_2_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	foffset = get_created_cbfs_file_start_offset(cbfs_files, 7);
	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&cbfs_buf[foffset],
			   foffset + be32toh(test_file_1.header.offset));
	will_return(cbfs_find_attr, NULL);
	mapping = cbfs_unverified_area_map("TEST_AREA", TEST_DATA_1_FILENAME, &size_out);
	assert_non_null(mapping);
	assert_int_equal(TEST_DATA_1_SIZE, size_out);
	assert_memory_equal(test_data_1, mapping, TEST_DATA_1_SIZE);
	cbfs_unmap(mapping);

	size_out = 0;
	expect_cbfs_lookup("invalid_file", CB_CBFS_NOT_FOUND, 0, 0);
	mapping = cbfs_unverified_area_map("TEST_AREA", "invalid_file", &size_out);
	assert_null(mapping);
}

#define TEST_CBFS_NAME_ALIGN_RO_RW(fn, test_name, enable_unaligned, enable_init_ro,            \
				   enable_init_rw)                                             \
	((struct CMUnitTest){                                                                  \
		.name = (test_name),                                                           \
		.test_func = (fn),                                                             \
		.setup_func = setup_cbfs_test,                                                 \
		.teardown_func = teardown_cbfs_test,                                           \
		.initial_state =                                                               \
			&(struct cbfs_test_setup){                                             \
				.unaligned = enable_unaligned,                                 \
				.init_ro = enable_init_ro,                                     \
				.init_rw = enable_init_rw,                                     \
			},                                                                     \
	})

#define TEST_CBFS_LOOKUP(fn)                                                                   \
	EMPTY_WRAP(TEST_CBFS_NAME_ALIGN_RO_RW(fn, #fn ", RW, aligned", false, false, true),    \
		   TEST_CBFS_NAME_ALIGN_RO_RW(fn, #fn ", RW, unaligned", true, false, true))

#define TEST_CBFS_RO_FALLBACK(fn)                                                              \
	EMPTY_WRAP(TEST_CBFS_NAME_ALIGN_RO_RW(fn, #fn ", RW+RO, aligned", false, true, true),  \
		   TEST_CBFS_NAME_ALIGN_RO_RW(fn, #fn ", RW+RO, unaligned", true, true, true), \
		   TEST_CBFS_NAME_ALIGN_RO_RW(fn, #fn ", RO, aligned", false, true, false),    \
		   TEST_CBFS_NAME_ALIGN_RO_RW(fn, #fn ", RO, unaligned", true, true, false))


int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_cbfs_boot_device_init),
		TEST_CBFS_LOOKUP(test_cbfs_map),
		TEST_CBFS_LOOKUP(test_cbfs_invalid_compression_algo),
		TEST_CBFS_LOOKUP(test_cbfs_io_error),
		TEST_CBFS_RO_FALLBACK(test_cbfs_successful_fallback_to_ro),
		TEST_CBFS_LOOKUP(test_cbfs_load),
		TEST_CBFS_LOOKUP(test_cbfs_map_with_mcache),
		TEST_CBFS_LOOKUP(test_cbfs_boot_device_read_failure),
		TEST_CBFS_LOOKUP(test_cbfs_unverified_area_map),
	};

	return lp_run_group_tests(tests, NULL, NULL);
}
