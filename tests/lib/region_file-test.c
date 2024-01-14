/* SPDX-License-Identifier: GPL-2.0-only */

#include "../lib/region_file.c"

#include <tests/test.h>
#include <stdlib.h>
#include <string.h>
#include <commonlib/region.h>
#include <tests/lib/region_file_data.h>

static void clear_region_file(struct region_device *rdev)
{
	memset(rdev_mmap_full(rdev), 0xff, REGION_FILE_BUFFER_SIZE);
}

static int setup_region_file_test_group(void **state)
{
	void *mem_buffer = malloc(REGION_FILE_BUFFER_SIZE);
	struct region_device *dev = malloc(sizeof(struct region_device));

	if (mem_buffer == NULL || dev == NULL) {
		free(mem_buffer);
		free(dev);
		return -1;
	}

	rdev_chain_mem_rw(dev, mem_buffer, REGION_FILE_BUFFER_SIZE);
	*state = dev;

	clear_region_file(dev);

	return 0;
}

static int teardown_region_file_test_group(void **state)
{
	struct region_device *dev = *state;
	void *mem_buffer = rdev_mmap_full(dev);

	free(mem_buffer);
	free(dev);

	return 0;
}

/* This function clears buffer associated with used region_device, so tests will be in clear
   state at the beginning and leave no trace after successful execution. The cost of memsetting
   everything twice is known, but acceptable as it grants safety and makes tests independent. */
static int setup_teardown_region_file_test(void **state)
{
	struct region_device *dev = *state;

	clear_region_file(dev);

	return 0;
}

static void test_region_file_init_empty(void **state)
{
	struct region_device *rdev = *state;
	struct region_file regf;

	/* Test general approach using valid mem_region_device with buffer filled with 0xff.
	   Parameters cannot be NULL. */
	assert_int_equal(0, region_file_init(&regf, rdev));
	assert_int_equal(RF_EMPTY, regf.slot);
}

static void test_region_file_init_invalid_metadata(void **state)
{
	struct region_device *rdev = *state;
	uint16_t *mem_buffer16 = (uint16_t *)rdev_mmap_full(rdev);
	struct region_file regf;

	/* Set number of metadata blocks to 0 */
	mem_buffer16[0] = 0;
	assert_int_equal(0, region_file_init(&regf, rdev));
	assert_int_equal(RF_NEED_TO_EMPTY, regf.slot);
}

static void test_region_file_init_valid_no_data(void **state)
{
	struct region_device *rdev = *state;
	uint16_t *mem_buffer16 = (uint16_t *)rdev_mmap_full(rdev);
	struct region_file regf;

	/* Manually allocate 4 metadata blocks and no data. */
	mem_buffer16[0] = 4;
	assert_int_equal(0, region_file_init(&regf, rdev));
	assert_int_equal(0, regf.slot);
}

static void test_region_file_init_invalid_data_offset(void **state)
{
	struct region_device *rdev = *state;
	uint16_t *mem_buffer16 = (uint16_t *)rdev_mmap_full(rdev);
	struct region_file regf;

	/* Manually allocate 4 metadata blocks and no data. */
	mem_buffer16[0] = 4;
	mem_buffer16[1] = 4;
	assert_int_equal(0, region_file_init(&regf, rdev));
	assert_int_equal(RF_NEED_TO_EMPTY, regf.slot);

	/* Set data size to be larger than region */
	mem_buffer16[0] = 4;
	mem_buffer16[1] = 4 + 4096;
	assert_int_equal(0, region_file_init(&regf, rdev));
	assert_int_equal(RF_NEED_TO_EMPTY, regf.slot);
}

static void test_region_file_init_correct_data_offset(void **state)
{
	struct region_device *rdev = *state;
	uint16_t *mem_buffer16 = (uint16_t *)rdev_mmap_full(rdev);
	struct region_file regf;

	/* Set data size to 8 blocks which is correct value. */
	mem_buffer16[0] = 4;
	mem_buffer16[1] = 4 + 8;
	assert_int_equal(0, region_file_init(&regf, rdev));
	assert_int_equal(1, regf.slot);
}

static void test_region_file_init_real_data(void **state)
{
	struct region_device rdev;
	struct region_file regf;

	rdev_chain_mem_rw(&rdev, region_file_data_buffer1, REGION_FILE_BUFFER_SIZE);

	/* Check on real example with one update */
	assert_int_equal(0, region_file_init(&regf, &rdev));
	/* There is one update available */
	assert_int_equal(1, regf.slot);


	/* Check on real example with multiple updates */
	rdev_chain_mem_rw(&rdev, region_file_data_buffer2, REGION_FILE_BUFFER_SIZE);
	assert_int_equal(0, region_file_init(&regf, &rdev));
	/* There are three update available */
	assert_int_equal(3, regf.slot);
}

static void test_region_file_init_invalid_region_device(void **state)
{
	struct region_device bad_dev;

	assert_int_equal(rdev_chain_mem_rw(&bad_dev, NULL, 0), -1);
}

static void test_region_file_data(void **state)
{
	/* region_device with empty data buffer */
	struct region_device *mrdev = *state;
	/* region_device with prepared data buffer */
	struct region_device rdev;
	rdev_chain_mem_rw(&rdev, region_file_data_buffer1, REGION_FILE_BUFFER_SIZE);

	struct region_file regf;
	struct region_device read_rdev;
	int ret;

	/* Check if region_file_data() fails to return region_device for empty region_file */
	ret = region_file_init(&regf, mrdev);
	assert_int_equal(0, ret);
	ret = region_file_data(&regf, &read_rdev);
	assert_int_equal(-1, ret);

	/* Check if region_file_data() correctly returns region_device for hardcoded
	   region_file data with update of 256 bytes */
	ret = region_file_init(&regf, &rdev);
	assert_int_equal(0, ret);
	ret = region_file_data(&regf, &read_rdev);
	assert_int_equal(0, ret);
	assert_int_equal(region_device_sz(&read_rdev),
			 ALIGN_UP(region_file_data_buffer1_update_sz, 16));
}

static void test_region_file_update_data(void **state)
{
	struct region_device *rdev = *state;
	struct region_file regf;
	struct region_device read_rdev;
	const size_t dummy_data_size = 256;
	uint8_t dummy_data[dummy_data_size];
	uint8_t output_buffer[dummy_data_size];
	int ret;

	for (int i = 0; i < dummy_data_size; ++i)
		dummy_data[i] = 'A' + i % ('Z' - 'A');

	ret = region_file_init(&regf, rdev);
	assert_int_equal(0, ret);

	/* Write half of buffer, read it and check, if it is the same.
	   region_file_update_data() should be able to deal with empty region_file. */
	ret = region_file_update_data(&regf, dummy_data, dummy_data_size / 2);
	assert_int_equal(0, ret);
	region_file_data(&regf, &read_rdev);
	assert_int_equal(ALIGN_UP(dummy_data_size / 2, 16), region_device_sz(&read_rdev));
	rdev_readat(&read_rdev, output_buffer, 0, dummy_data_size / 2);
	assert_memory_equal(dummy_data, output_buffer, dummy_data_size / 2);

	/* Update data to a bigger size */
	ret = region_file_update_data(&regf, dummy_data, dummy_data_size);
	assert_int_equal(0, ret);
	region_file_data(&regf, &read_rdev);
	assert_int_equal(ALIGN_UP(dummy_data_size, 16), region_device_sz(&read_rdev));
	rdev_readat(&read_rdev, output_buffer, 0, dummy_data_size);
	assert_memory_equal(dummy_data, output_buffer, dummy_data_size);

	/* Update data to smaller size and check if it was properly stored */
	ret = region_file_update_data(&regf, dummy_data, dummy_data_size / 2 + 3);
	assert_int_equal(0, ret);
	region_file_data(&regf, &read_rdev);
	assert_int_equal(ALIGN_UP(dummy_data_size / 2 + 3, 16), region_device_sz(&read_rdev));
	rdev_readat(&read_rdev, output_buffer, 0, dummy_data_size / 2 + 3);
	assert_memory_equal(dummy_data, output_buffer, dummy_data_size / 2 + 3);
}

static void test_region_file_update_data_arr(void **state)
{
	struct region_device *rdev = *state;
	struct region_file regf;
	struct region_device read_rdev;
	const size_t dummy_data_size = 256;
	uint8_t dummy_data[dummy_data_size];
	uint8_t output_buffer[dummy_data_size * 4];
	struct update_region_file_entry update_entries[3];
	const size_t data1_size = dummy_data_size;
	const size_t data2_size = dummy_data_size / 2;
	const size_t data3_size = dummy_data_size / 4 + 3;
	const size_t data1_offset = 0;
	const size_t data2_offset = dummy_data_size / 4 + 2;
	const size_t data3_offset = dummy_data_size / 8 + 5;
	int ret;

	for (int i = 0; i < dummy_data_size; ++i)
		dummy_data[i] = 'A' + i % ('Z' - 'A');

	update_entries[0] = (struct update_region_file_entry){
		.size = data1_size, .data = &dummy_data[data1_offset]};
	update_entries[1] = (struct update_region_file_entry){
		.size = data2_size, .data = &dummy_data[data2_offset]};
	update_entries[2] = (struct update_region_file_entry){
		.size = data3_size, .data = &dummy_data[data3_offset]};

	ret = region_file_init(&regf, rdev);
	assert_int_equal(0, ret);

	/* Write two update blocks as first data state. region_file_update_data_arr() should
	   be able to deal with empty region_file. */
	ret = region_file_update_data_arr(&regf, update_entries, 2);
	assert_int_equal(0, ret);
	region_file_data(&regf, &read_rdev);
	assert_int_equal(ALIGN_UP(data1_size + data2_size, 16), region_device_sz(&read_rdev));
	ret = rdev_readat(&read_rdev, output_buffer, 0, data1_size + data2_size);
	assert_int_equal(data1_size + data2_size, ret);
	assert_memory_equal(&dummy_data[data1_offset], output_buffer, data1_size);
	assert_memory_equal(&dummy_data[data1_offset + data2_offset],
			    &output_buffer[data1_size], data2_size);

	/* Check if new block of data is added correctly */
	ret = region_file_update_data_arr(&regf, update_entries, 3);
	assert_int_equal(0, ret);
	region_file_data(&regf, &read_rdev);
	assert_int_equal(ALIGN_UP(data1_size + data2_size + data3_size, 16),
			 region_device_sz(&read_rdev));
	ret = rdev_readat(&read_rdev, output_buffer, 0, data1_size + data2_size + data3_size);
	assert_int_equal(data1_size + data2_size + data3_size, ret);
	assert_memory_equal(&dummy_data[data1_offset], output_buffer, data1_size);
	assert_memory_equal(&dummy_data[data2_offset], &output_buffer[data1_size], data2_size);
	assert_memory_equal(&dummy_data[data3_offset], &output_buffer[data1_size + data2_size],
			    data3_size);

	/* Check if data is correctly shrunk down to smaller size and different content */
	ret = region_file_update_data_arr(&regf, &update_entries[1], 2);
	assert_int_equal(0, ret);
	region_file_data(&regf, &read_rdev);
	assert_int_equal(ALIGN_UP(data2_size + data3_size, 16), region_device_sz(&read_rdev));
	ret = rdev_readat(&read_rdev, output_buffer, 0, data2_size + data3_size);
	assert_int_equal(data2_size + data3_size, ret);
	assert_memory_equal(&dummy_data[data2_offset], &output_buffer[0], data2_size);
	assert_memory_equal(&dummy_data[data3_offset], &output_buffer[data2_size], data3_size);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_region_file_init_empty,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_init_invalid_metadata,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_init_valid_no_data,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_init_invalid_data_offset,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_init_correct_data_offset,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_init_real_data,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_init_invalid_region_device,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_data,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_update_data,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
		cmocka_unit_test_setup_teardown(test_region_file_update_data_arr,
						setup_teardown_region_file_test,
						setup_teardown_region_file_test),
	};

	return cb_run_group_tests(tests, setup_region_file_test_group,
				  teardown_region_file_test_group);
}
