/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <crc_byte.h>

static const uint8_t test_data_u8[] = {
	0x2f, 0x8f, 0x2d, 0x06, 0xc2, 0x11, 0x0c, 0xaf,
	0xd7, 0x4b, 0x48, 0x71, 0xce, 0x3c, 0xfe, 0x29,
	0x90, 0xf6, 0x33, 0x6d, 0x79, 0x23, 0x9d, 0x84,
	0x58, 0x5c, 0xcc, 0xf1, 0xa1, 0xf2, 0x39, 0x22,
	0xdc, 0x63, 0xe0, 0x44, 0x0a, 0x95, 0x36, 0xee,
	0x53, 0xb3, 0x61, 0x2c, 0x4a, 0xf4, 0x8b, 0x32,
	0xeb, 0x94, 0x86, 0x55, 0x41, 0x27, 0xa4, 0xbd,
	0x0f, 0xc1, 0x4f, 0xfb, 0xb6, 0xa3, 0xc5, 0x38,
	0x99, 0xfc, 0xca, 0xf8, 0x8e, 0x72, 0xaa, 0xed,
	0x6b, 0xb2, 0xd3, 0xd4, 0xd6, 0x81, 0x7d, 0x24,
	0x56, 0x9f, 0x7a, 0x21, 0x67, 0xac, 0x6a, 0x98,
	0xf7, 0xd1, 0xad, 0x01, 0xdb, 0xc6, 0x80, 0x34,
	0x8d, 0x51, 0x60, 0x3e, 0xd2, 0x52, 0x0e, 0x26,
	0x12, 0xb1, 0x13, 0xa2, 0x88, 0x04, 0x66, 0xb0,
	0x3b, 0xc8, 0x1b, 0x7f, 0x92, 0x4e, 0xb8, 0xe9,
	0x70, 0xe3, 0xfa, 0x76, 0x3a, 0xa7, 0x4c, 0x25,
	0x91, 0x54, 0x19, 0xea, 0x50, 0x37, 0xd8, 0xb4,
	0x47, 0x49, 0xbf, 0xc4, 0xb7, 0xd0, 0x93, 0xda,
	0x6c, 0x03, 0x9b, 0x15, 0xbb, 0xfd, 0xe7, 0xdd,
	0x2e, 0x31, 0x68, 0x46, 0xa0, 0x43, 0xcd, 0x08,
	0x8c, 0xff, 0x40, 0xcf, 0x1a, 0x7c, 0x69, 0x59,
	0xc0, 0x5b, 0x83, 0x17, 0x10, 0x14, 0x9e, 0x1d,
	0xc3, 0xa6, 0x5f, 0x4d, 0x9c, 0xa5, 0x73, 0x77,
	0x87, 0x96, 0x65, 0x0b, 0xec, 0xc7, 0xd9, 0x85,
	0x1c, 0xae, 0x18, 0x5e, 0x09, 0x78, 0x2b, 0x82,
	0x1f, 0xe6, 0xc9, 0x64, 0x6f, 0x20, 0x16, 0x57,
	0x9a, 0xbe, 0xd5, 0xe2, 0x89, 0x3f, 0xdf, 0xe4,
	0x7e, 0xde, 0x30, 0xa9, 0x74, 0xe5, 0xab, 0x07,
	0x35, 0x5d, 0x2a, 0x28, 0xcb, 0xf0, 0x8a, 0xef,
	0x5a, 0xe1, 0x75, 0x42, 0xf9, 0xba, 0x02, 0xbc,
	0xf5, 0x45, 0x05, 0x0d, 0x3d, 0x62, 0xb9, 0x00,
	0x7b, 0x1e, 0xe8, 0xb5, 0x97, 0x6e, 0xa8, 0xf3,
};
static const size_t test_data_u8_sz = ARRAY_SIZE(test_data_u8);
static const uint8_t test_data_crc7_checksum = 0x30;
static const uint16_t test_data_crc16_checksum = 0x63;
static const uint32_t test_data_crc32_checksum = 0x93;

static void test_crc7_byte_zeros(void **state)
{
	uint8_t crc_value = 0u;

	/* Expect zero as crc value after calculating it for single zero byte */
	crc_value = crc7_byte(crc_value, 0);
	assert_int_equal(0, crc_value);

	/* Expect zero crc for zero-byte stream */
	for (size_t i = 0; i < 3000; ++i) {
		crc_value = crc7_byte(crc_value, 0);
		assert_int_equal(0, crc_value);
	}
}

static void test_crc7_byte_same_byte_twice_different_value(void **state)
{
	uint8_t crc_value = 0u;

	/* Expect value to change after feeding crc function with the same byte twice. */
	crc_value = crc7_byte(crc_value, 0xAD);
	assert_int_equal(0x2C, crc_value);

	crc_value = crc7_byte(crc_value, 0xAD);
	assert_int_equal(0x90, crc_value);
}

static void test_crc7_byte_repeat_stream(void **state)
{
	uint8_t crc_value_1 = 0u;
	uint8_t crc_value_2 = 0u;
	const size_t iterations = 25600;

	/* Calculate CRC7 twice for the same data and expect the same result.
	   Ensure deterministic and repeatable behavior. */
	for (size_t i = 0; i < iterations; i++) {
		const uint8_t value = (i & 0xFF) ^ ((i >> 8) & 0xFF);
		crc_value_1 = crc7_byte(crc_value_1, value);
	}

	for (size_t i = 0; i < iterations; i++) {
		const uint8_t value = (i & 0xFF) ^ ((i >> 8) & 0xFF);
		crc_value_2 = crc7_byte(crc_value_2, value);
	}

	assert_int_equal(crc_value_1, crc_value_2);
}

static void test_crc7_byte_single_bit_difference(void **state)
{
	uint8_t crc_value_1 = 0u;
	uint8_t crc_value_2 = 0u;

	for (size_t i = 0; i < 1000; ++i) {
		crc_value_1 = crc7_byte(crc_value_1, (i % 128) << 1);
		crc_value_2 = crc7_byte(crc_value_2, (i % 128) << 1);
	}
	crc_value_1 = crc7_byte(crc_value_1, 0xF0);
	crc_value_2 = crc7_byte(crc_value_2, 0xF1);

	/* Expect different CRC values for byte streams differing by one bit. */
	assert_int_not_equal(crc_value_1, crc_value_2);
}

static void test_crc7_byte_static_data(void **state)
{
	uint8_t crc_value = CRC(test_data_u8, test_data_u8_sz, crc7_byte);

	assert_int_equal(test_data_crc7_checksum, crc_value);

	/* Calculating CRC of data with its CRC should yield zero if data
	   and/or checksum is correct */
	assert_int_equal(0, crc7_byte(crc_value, test_data_crc7_checksum));
}

static void test_crc16_byte_zeros(void **state)
{
	uint16_t crc_value = 0u;

	/* Expect zero as crc value after calculating it for single zero byte */
	crc_value = crc16_byte(crc_value, 0);
	assert_int_equal(0, crc_value);

	/* Expect zero crc for zero-byte stream */
	for (size_t i = 0; i < 3000; ++i) {
		crc_value = crc16_byte(crc_value, 0);
		assert_int_equal(0, crc_value);
	}
}

static void test_crc16_same_data_twice_different_value(void **state)
{
	uint16_t crc_value = 0u;

	/* Expect value to change after feeding crc function with the same byte twice. */
	crc_value = crc16_byte(crc_value, 0xDF);
	assert_int_equal(0x3a92, crc_value);

	crc_value = crc16_byte(crc_value, 0xDF);
	assert_int_equal(0x3f8b, crc_value);
}

static void test_crc16_byte_repeat_stream(void **state)
{
	uint8_t crc_value_1 = 0u;
	uint8_t crc_value_2 = 0u;
	const size_t iterations = 17777;

	/* Calculate CRC16 twice for the same data and expect the same result.
	   Ensure deterministic and repeatable behavior. */
	for (size_t i = 0; i < iterations; i++) {
		const uint8_t value = (i & 0xFF) ^ ((i >> 8) & 0xFF);
		crc_value_1 = crc16_byte(crc_value_1, value);
	}

	for (size_t i = 0; i < iterations; i++) {
		const uint8_t value = (i & 0xFF) ^ ((i >> 8) & 0xFF);
		crc_value_2 = crc16_byte(crc_value_2, value);
	}

	assert_int_equal(crc_value_1, crc_value_2);
}

static void test_crc16_byte_single_bit_difference(void **state)
{
	uint8_t crc_value_1 = 0u;
	uint8_t crc_value_2 = 0u;

	for (size_t i = 0; i < 2000; ++i) {
		crc_value_1 = crc16_byte(crc_value_1, (i % 128) << 1);
		crc_value_2 = crc16_byte(crc_value_2, (i % 128) << 1);
	}
	crc_value_1 = crc16_byte(crc_value_1, 0x1A);
	crc_value_2 = crc16_byte(crc_value_2, 0x0A);

	/* Expect different CRC values for byte streams differing by one bit. */
	assert_int_not_equal(crc_value_1, crc_value_2);
}

static void test_crc16_byte_static_data(void **state)
{
	uint8_t crc_value = CRC(test_data_u8, test_data_u8_sz, crc16_byte);

	assert_int_equal(test_data_crc16_checksum, crc_value);

	/* Calculating CRC of data with its CRC should yield zero if data
	   and/or checksum is correct */
	assert_int_equal(0,
			crc16_byte(crc16_byte(crc_value, test_data_crc16_checksum >> 8),
					test_data_crc16_checksum & 0xFF));
}

static void test_crc32_byte_zeros(void **state)
{
	uint16_t crc_value = 0u;

	/* Expect zero as crc value after calculating it for single zero byte */
	crc_value = crc32_byte(crc_value, 0);
	assert_int_equal(0, crc_value);

	/* Expect zero crc for zero-byte stream */
	for (size_t i = 0; i < 1553; ++i) {
		crc_value = crc32_byte(crc_value, 0);
		assert_int_equal(0, crc_value);
	}
}

static void test_crc32_same_data_twice_different_value(void **state)
{
	uint16_t crc_value = 0u;

	/* Expect value to change after feeding crc function with the same byte twice. */
	crc_value = crc32_byte(crc_value, 0xDF);
	assert_int_equal(0xf654, crc_value);

	crc_value = crc32_byte(crc_value, 0xDF);
	assert_int_equal(0xa254, crc_value);
}

static void test_crc32_byte_repeat_stream(void **state)
{
	uint8_t crc_value_1 = 0u;
	uint8_t crc_value_2 = 0u;
	const size_t iterations = 8935;

	/* Calculate CRC16 twice for the same data and expect the same result.
	   Ensure deterministic and repeatable behavior. */
	for (size_t i = 0; i < iterations; i++) {
		const uint8_t value = (i & 0xFF) ^ ((i >> 8) & 0xFF);
		crc_value_1 = crc32_byte(crc_value_1, value);
	}

	for (size_t i = 0; i < iterations; i++) {
		const uint8_t value = (i & 0xFF) ^ ((i >> 8) & 0xFF);
		crc_value_2 = crc32_byte(crc_value_2, value);
	}

	assert_int_equal(crc_value_1, crc_value_2);
}

static void test_crc32_byte_single_bit_difference(void **state)
{
	uint8_t crc_value_1 = 0u;
	uint8_t crc_value_2 = 0u;

	for (size_t i = 0; i < 1338; ++i) {
		crc_value_1 = crc32_byte(crc_value_1, (i % 128) << 1);
		crc_value_2 = crc32_byte(crc_value_2, (i % 128) << 1);
	}
	crc_value_1 = crc32_byte(crc_value_1, 0x33);
	crc_value_2 = crc32_byte(crc_value_2, 0x32);

	/* Expect different CRC values for byte streams differing by one bit. */
	assert_int_not_equal(crc_value_1, crc_value_2);
}

static void test_crc32_byte_static_data(void **state)
{
	uint8_t crc_value = CRC(test_data_u8, test_data_u8_sz, crc32_byte);

	assert_int_equal(test_data_crc32_checksum, crc_value);

	/* Calculating CRC of data with its CRC should yield zero if data
	   and/or checksum is correct */
	for (int i = 0; i < 4; ++i)
		crc_value = crc32_byte(crc_value, (test_data_crc32_checksum >> 8 * i) & 0xFF);
	assert_int_equal(0, crc_value);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_crc7_byte_zeros),
		cmocka_unit_test(test_crc7_byte_same_byte_twice_different_value),
		cmocka_unit_test(test_crc7_byte_repeat_stream),
		cmocka_unit_test(test_crc7_byte_single_bit_difference),
		cmocka_unit_test(test_crc7_byte_static_data),

		cmocka_unit_test(test_crc16_byte_zeros),
		cmocka_unit_test(test_crc16_same_data_twice_different_value),
		cmocka_unit_test(test_crc16_byte_repeat_stream),
		cmocka_unit_test(test_crc16_byte_single_bit_difference),
		cmocka_unit_test(test_crc16_byte_static_data),

		cmocka_unit_test(test_crc32_byte_zeros),
		cmocka_unit_test(test_crc32_same_data_twice_different_value),
		cmocka_unit_test(test_crc32_byte_repeat_stream),
		cmocka_unit_test(test_crc32_byte_single_bit_difference),
		cmocka_unit_test(test_crc32_byte_static_data),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
