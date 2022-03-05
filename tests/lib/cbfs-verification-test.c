/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <commonlib/bsd/cbfs_private.h>
#include <commonlib/region.h>
#include <string.h>
#include <tests/lib/cbfs_util.h>
#include <tests/test.h>


/* Mocks */

static struct cbfs_boot_device cbd;

const struct cbfs_boot_device *cbfs_get_boot_device(bool force_ro)
{
	check_expected(force_ro);
	return &cbd;
}

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

size_t ulzman(const void *src, size_t srcn, void *dst, size_t dstn)
{
	fail_msg("Unexpected call to %s", __func__);
	return 0;
}

size_t ulz4fn(const void *src, size_t srcn, void *dst, size_t dstn)
{
	fail_msg("Unexpected call to %s", __func__);
	return 0;
}

vb2_error_t vb2_digest_init(struct vb2_digest_context *dc, enum vb2_hash_algorithm hash_alg)
{
	if (hash_alg != VB2_HASH_SHA256) {
		fail_msg("Unsupported hash algorithm: %d\n", hash_alg);
		return VB2_ERROR_SHA_INIT_ALGORITHM;
	}

	return VB2_SUCCESS;
}

vb2_error_t vb2_digest_extend(struct vb2_digest_context *dc, const uint8_t *buf, uint32_t size)
{
	check_expected(buf);
	check_expected(size);
	return VB2_SUCCESS;
}

vb2_error_t vb2_digest_finalize(struct vb2_digest_context *dc, uint8_t *digest, uint32_t size)
{
	memcpy(digest, mock_ptr_type(void *), size);
	return VB2_SUCCESS;
}

/* Original function alias created by test framework. Used for call wrapping in mock below. */
enum cb_err __real_cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			       size_t *data_offset_out, struct vb2_hash *metadata_hash);

enum cb_err cbfs_lookup(cbfs_dev_t dev, const char *name, union cbfs_mdata *mdata_out,
			size_t *data_offset_out, struct vb2_hash *metadata_hash)
{
	const enum cb_err err =
		__real_cbfs_lookup(dev, name, mdata_out, data_offset_out, metadata_hash);
	assert_int_equal(mock_type(enum cb_err), err);
	return err;
}

/* Tests */

static int setup_test_cbfs(void **state)
{
	memset(&cbd, 0, sizeof(cbd));
	return 0;
}

static void test_cbfs_map_no_hash(void **state)
{
	void *mapping = NULL;
	assert_int_equal(0, rdev_chain_mem(&cbd.rdev, &file_no_hash, sizeof(file_no_hash)));

	if (CONFIG(CBFS_VERIFICATION)) {
		/* File with no hash. No hash causes hash mismatch by default,
		   so mapping will not be completed successfully. */
		expect_value(cbfs_get_boot_device, force_ro, false);
		will_return(cbfs_lookup, CB_SUCCESS);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_null(mapping);
	} else {
		expect_value(cbfs_get_boot_device, force_ro, false);
		will_return(cbfs_lookup, CB_SUCCESS);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_ptr_equal(mapping, file_no_hash.attrs_and_data);
	}
}

static void test_cbfs_map_valid_hash(void **state)
{
	void *mapping = NULL;
	assert_int_equal(0,
			 rdev_chain_mem(&cbd.rdev, &file_valid_hash, sizeof(file_valid_hash)));

	if (CONFIG(CBFS_VERIFICATION)) {
		expect_value(cbfs_get_boot_device, force_ro, false);
		expect_value(vb2_hash_verify, buf,
			     &file_valid_hash.attrs_and_data[HASH_ATTR_SIZE]);
		expect_value(vb2_hash_verify, size, TEST_DATA_1_SIZE);
		will_return(cbfs_lookup, CB_SUCCESS);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_ptr_equal(mapping, &file_valid_hash.attrs_and_data[HASH_ATTR_SIZE]);
	} else {
		expect_value(cbfs_get_boot_device, force_ro, false);
		will_return(cbfs_lookup, CB_SUCCESS);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_ptr_equal(mapping, &file_valid_hash.attrs_and_data[HASH_ATTR_SIZE]);
	}
}

static void test_cbfs_map_invalid_hash(void **state)
{
	void *mapping = NULL;
	assert_int_equal(
		0, rdev_chain_mem(&cbd.rdev, &file_broken_hash, sizeof(file_broken_hash)));

	if (CONFIG(CBFS_VERIFICATION)) {
		expect_value(cbfs_get_boot_device, force_ro, false);
		expect_value(vb2_hash_verify, buf,
			     &file_broken_hash.attrs_and_data[HASH_ATTR_SIZE]);
		expect_value(vb2_hash_verify, size, TEST_DATA_1_SIZE);
		will_return(cbfs_lookup, CB_SUCCESS);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_null(mapping);
	} else {
		expect_value(cbfs_get_boot_device, force_ro, false);
		will_return(cbfs_lookup, CB_SUCCESS);
		mapping = cbfs_map(TEST_DATA_1_FILENAME, NULL);
		assert_ptr_equal(mapping, &file_broken_hash.attrs_and_data[HASH_ATTR_SIZE]);
	}
}

void test_init_boot_device_verify(void **state)
{
	struct vb2_hash hash = {.algo = VB2_HASH_SHA256};
	const uint8_t hash_value[VB2_SHA256_DIGEST_SIZE] = {0};
	memset(&cbd, 0, sizeof(cbd));
	assert_int_equal(0,
			 rdev_chain_mem(&cbd.rdev, &file_valid_hash, sizeof(file_valid_hash)));

	if (CONFIG(CBFS_VERIFICATION)) {
		expect_memory(vb2_digest_extend, buf, &file_valid_hash,
			      be32_to_cpu(file_valid_hash.header.offset));
		expect_value(vb2_digest_extend, size,
			     be32_to_cpu(file_valid_hash.header.offset));
		will_return(vb2_digest_finalize, hash_value);
	}

	assert_int_equal(CB_SUCCESS, cbfs_init_boot_device(&cbd, &hash));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_cbfs_map_no_hash, setup_test_cbfs),
		cmocka_unit_test_setup(test_cbfs_map_valid_hash, setup_test_cbfs),
		cmocka_unit_test_setup(test_cbfs_map_invalid_hash, setup_test_cbfs),

		cmocka_unit_test(test_init_boot_device_verify),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
