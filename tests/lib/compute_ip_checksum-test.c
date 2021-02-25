/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <string.h>
#include <stdlib.h>
#include <types.h>
#include <ip_checksum.h>

static const uint8_t test_data_simple[] = {
	0x64, 0x3b, 0x33, 0x17, 0x34, 0x74, 0x62, 0x30,
	0x75, 0x73, 0xf3, 0x11, 0x30, 0x2c, 0x34, 0x35,
	0x6d, 0x39, 0x69, 0x32, 0x23, 0x24, 0x76, 0x71,
	0x77, 0x30, 0x39, 0x75, 0x76, 0x35, 0x71, 0x32,
	0x40, 0x46, 0x34, 0x34, 0xBB, 0x03, 0x66, 0x52
};
static const size_t test_data_simple_sz = ARRAY_SIZE(test_data_simple);
static const unsigned long test_data_simple_checksum = 0x4267;

static uint8_t test_data_zeros[1024];
static const size_t test_data_zeros_sz = ARRAY_SIZE(test_data_zeros);
static const unsigned long test_data_zeros_checksum = 0xFFFF;

static int setup_test_group(void **state)
{
	memset(test_data_zeros, 0, test_data_zeros_sz);

	return 0;
}

static void test_compute_ip_checksum_zero_length(void **state)
{
	unsigned long res = compute_ip_checksum(test_data_simple, 0);

	/* Expect checksum to be in initial state as there are were no data provided. */
	assert_int_equal(0xFFFF, res);
}

static void test_compute_ip_checksum_zero_buffer(void **state)
{
	unsigned long res = compute_ip_checksum(test_data_zeros, test_data_zeros_sz);
	assert_int_equal(test_data_zeros_checksum, res);
}

static void test_compute_ip_checksum_simple_data(void **state)
{
	unsigned long res;
	unsigned long check_res;
	const size_t helper_buffer_size = sizeof(uint8_t) * (test_data_simple_sz + 2);
	char *helper_buffer = malloc(helper_buffer_size);

	/* Self test */
	assert_non_null(helper_buffer);

	/* Expect function to generate the same checksum as stored in */
	res = compute_ip_checksum(test_data_simple, test_data_simple_sz);
	assert_int_equal(test_data_simple_checksum, res);

	/* Copy test data and checksum to new buffer. Expect computed checksum to be zero,
	   as it proves the data and the checksum are correct. */
	memcpy(helper_buffer, test_data_simple, test_data_simple_sz);
	helper_buffer[helper_buffer_size - 2] = res & 0xFF;
	helper_buffer[helper_buffer_size - 1] = (res >> 8) & 0xFF;
	check_res = compute_ip_checksum(helper_buffer, helper_buffer_size);
	assert_int_equal(0, check_res);

	free(helper_buffer);
}

static void test_add_ip_checksums_empty_values(void **state)
{
	unsigned long res;

	res = add_ip_checksums(0, 0xFFFF, 0xFFFF);
	assert_int_equal(0xFFFF, res);

	res = add_ip_checksums(1, 0xFFFF, 0xFFFF);
	assert_int_equal(0xFFFF, res);
}

static void test_add_ip_checksums(void **state)
{
	unsigned long res_1 = compute_ip_checksum(test_data_simple, test_data_simple_sz / 2);
	unsigned long res_2 = compute_ip_checksum(test_data_simple + test_data_simple_sz / 2,
							test_data_simple_sz / 2);
	unsigned long res_sum = add_ip_checksums(test_data_simple_sz / 2, res_1, res_2);

	assert_int_equal(test_data_simple_checksum, res_sum);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_compute_ip_checksum_zero_length),
		cmocka_unit_test(test_compute_ip_checksum_zero_buffer),
		cmocka_unit_test(test_compute_ip_checksum_simple_data),

		cmocka_unit_test(test_add_ip_checksums_empty_values),
		cmocka_unit_test(test_add_ip_checksums),
	};

	return cmocka_run_group_tests(tests, setup_test_group, NULL);
}
