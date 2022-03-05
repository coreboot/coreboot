/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <cbfs_glue.h>
#include <string.h>
#include <mocks/cbfs_util.h>
#include <tests/test.h>

#include "../libcbfs/cbfs.c"

/* Mocks */

unsigned long virtual_offset = 0;
struct sysinfo_t lib_sysinfo;

size_t vb2_digest_size(enum vb2_hash_algorithm hash_alg)
{
	if (hash_alg != VB2_HASH_SHA256) {
		fail_msg("Unsupported hash algorithm: %d\n", hash_alg);
		return 0;
	}

	return VB2_SHA256_DIGEST_SIZE;
}

vb2_error_t vb2_hash_verify(const void *buf, uint32_t size, const struct vb2_hash *hash)
{
	check_expected_ptr(buf);
	check_expected(size);

	assert_int_equal(hash->algo, VB2_HASH_SHA256);

	if (!memcmp(hash->sha256, good_hash, sizeof(good_hash)))
		return VB2_SUCCESS;

	if (!memcmp(hash->sha256, bad_hash, sizeof(bad_hash)))
		return VB2_ERROR_SHA_MISMATCH;

	fail_msg("%s called with bad hash", __func__);
	return VB2_ERROR_SHA_MISMATCH;
}

unsigned long ulzman(const unsigned char *src, unsigned long srcn, unsigned char *dst,
		     unsigned long dstn)
{
	fail_msg("Unexpected call to %s", __func__);
	return 0;
}

size_t ulz4fn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	fail_msg("Unexpected call to %s", __func__);
	return 0;
}

enum cb_err cbfs_mcache_lookup(const void *mcache, size_t mcache_size, const char *name,
			       union cbfs_mdata *mdata_out, size_t *data_offset_out)
{
	return CB_CBFS_CACHE_FULL;
}

enum cb_err cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			size_t *data_offset_out, struct vb2_hash *metadata_hash)
{
	assert_non_null(dev);
	check_expected(name);

	enum cb_err ret = mock_type(enum cb_err);
	if (ret != CB_SUCCESS)
		return ret;

	memcpy(mdata_out, mock_ptr_type(const union cbfs_mdata *), sizeof(union cbfs_mdata));
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

enum cb_err fmap_locate_area(const char *name, size_t *offset, size_t *size)
{
	*offset = 0;
	*size = 0;
	return CB_SUCCESS;
}

ssize_t boot_device_read(void *buf, size_t offset, size_t size)
{
	/* Offset should be based on an address from lib_sysinfo.cbfs_offset */
	memcpy(buf, (void *)offset, size);

	return size;
}

const struct vb2_hash *cbfs_file_hash(const union cbfs_mdata *mdata)
{
	return mock_ptr_type(const struct vb2_hash *);
}

/* Utils */

static void clear_cbfs_boot_devices(void)
{
	lib_sysinfo.cbfs_ro_mcache_offset = 0;
	lib_sysinfo.cbfs_ro_mcache_size = 0;
	lib_sysinfo.cbfs_offset = 0;
	lib_sysinfo.cbfs_size = 0;
	lib_sysinfo.cbfs_rw_mcache_offset = 0;
	lib_sysinfo.cbfs_rw_mcache_size = 0;
	memset((void *)cbfs_get_boot_device(true), 0, sizeof(struct cbfs_boot_device));
	memset((void *)cbfs_get_boot_device(false), 0, sizeof(struct cbfs_boot_device));
}

void set_cbfs(uint64_t offset, size_t size)
{
	clear_cbfs_boot_devices();
	lib_sysinfo.cbfs_offset = offset;
	lib_sysinfo.cbfs_size = size;
}

/* Tests */

static int setup_test_cbfs(void **state)
{
	clear_cbfs_boot_devices();
	return 0;
}

static void test_cbfs_map_no_hash(void **state)
{
	void *mapping = NULL;
	size_t size = 0;

	set_cbfs((uint64_t)&file_no_hash, sizeof(file_no_hash));

	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&file_no_hash,
			   be32toh(file_no_hash.header.offset));
	will_return(cbfs_find_attr, NULL);

	if (CONFIG(LP_CBFS_VERIFICATION)) {
		/* File with no hash. No hash causes hash mismatch by default,
		   so mapping will not be completed successfully. */
		will_return(cbfs_file_hash, NULL);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_null(mapping);
	} else {
		mapping = cbfs_map(TEST_DATA_1_FILENAME, &size);
		assert_non_null(mapping);
		assert_int_equal(TEST_DATA_1_SIZE, size);
		assert_memory_equal(test_data_1, mapping, size);
		cbfs_unmap(mapping);
	}
}

static void test_cbfs_map_valid_hash(void **state)
{
	void *mapping = NULL;
	size_t size = 0;
	struct vb2_hash hash = {
		.algo = VB2_HASH_SHA256,
	};
	memcpy(&hash.sha256, good_hash, VB2_SHA256_DIGEST_SIZE);

	set_cbfs((uint64_t)&file_valid_hash, sizeof(file_valid_hash));

	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&file_valid_hash,
			   be32toh(file_valid_hash.header.offset));
	will_return(cbfs_find_attr, NULL);


	if (CONFIG(LP_CBFS_VERIFICATION)) {
		will_return(cbfs_file_hash, &hash);
		expect_memory(vb2_hash_verify, buf,
			      &file_valid_hash.attrs_and_data[HASH_ATTR_SIZE], HASH_ATTR_SIZE);
		expect_value(vb2_hash_verify, size, TEST_DATA_1_SIZE);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, &size);
		assert_non_null(mapping);
		assert_int_equal(TEST_DATA_1_SIZE, size);
		assert_memory_equal(mapping, &file_valid_hash.attrs_and_data[HASH_ATTR_SIZE],
				    size);
	} else {
		mapping = cbfs_map(TEST_DATA_1_FILENAME, &size);
		assert_non_null(mapping);
		assert_int_equal(TEST_DATA_1_SIZE, size);
		assert_memory_equal(test_data_1, mapping, size);
		cbfs_unmap(mapping);
	}
}

static void test_cbfs_map_invalid_hash(void **state)
{
	void *mapping = NULL;
	size_t size = 0;
	struct vb2_hash hash = {
		.algo = VB2_HASH_SHA256,
	};
	memcpy(&hash.sha256, bad_hash, VB2_SHA256_DIGEST_SIZE);

	set_cbfs((uint64_t)&file_broken_hash, sizeof(file_broken_hash));

	expect_cbfs_lookup(TEST_DATA_1_FILENAME, CB_SUCCESS,
			   (const union cbfs_mdata *)&file_broken_hash,
			   be32toh(file_broken_hash.header.offset));
	will_return(cbfs_find_attr, NULL);

	if (CONFIG(LP_CBFS_VERIFICATION)) {
		will_return(cbfs_file_hash, &hash);
		expect_memory(vb2_hash_verify, buf,
			      &file_broken_hash.attrs_and_data[HASH_ATTR_SIZE], HASH_ATTR_SIZE);
		expect_value(vb2_hash_verify, size, TEST_DATA_1_SIZE);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_null(mapping);
	} else {
		mapping = cbfs_map(TEST_DATA_1_FILENAME, &size);
		assert_non_null(mapping);
		assert_int_equal(TEST_DATA_1_SIZE, size);
		assert_memory_equal(test_data_1, mapping, size);
		cbfs_unmap(mapping);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_cbfs_map_no_hash, setup_test_cbfs),
		cmocka_unit_test_setup(test_cbfs_map_valid_hash, setup_test_cbfs),
		cmocka_unit_test_setup(test_cbfs_map_invalid_hash, setup_test_cbfs),
	};

	return lp_run_group_tests(tests, NULL, NULL);
}
