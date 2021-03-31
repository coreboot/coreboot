/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>
#include <uuid.h>

/* This test checks if parse_uuid() correctly parses UUID strings with correct format.
   lib/uuid does not check UUID version nor variant.
   If that changes tests should be updated as appropriate. */
static void test_parse_uuid_correct(void **state)
{
	/* lib/uuid does not check UUID version nor variant.
	   If that changes tests should be updated as appropriate. */
	uint8_t uuid_buf[UUID_LEN] = { 0 };
	const char *uuid_correct = "00112233-4455-6677-8899-aabbccddeeff";
	const uint8_t uuid_correct_bin[UUID_LEN] = {
		0x33, 0x22, 0x11, 0x00, 0x55, 0x44, 0x77, 0x66,
		0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
	};
	const char *uuid_zereos = "00000000-0000-0000-0000-000000000000";
	const uint8_t uuid_zeros_bin[UUID_LEN] = { 0 };
	const char *uuid_ff = "ffffffff-ffff-ffff-ffff-ffffffffffff";
	const uint8_t uuid_ff_bin[UUID_LEN] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	};

	assert_int_equal(0, parse_uuid(uuid_buf, uuid_correct));
	assert_memory_equal(uuid_correct_bin, uuid_buf, UUID_LEN);

	assert_int_equal(0, parse_uuid(uuid_buf, uuid_zereos));
	assert_memory_equal(uuid_zeros_bin, uuid_buf, UUID_LEN);

	assert_int_equal(0, parse_uuid(uuid_buf, uuid_ff));
	assert_memory_equal(uuid_ff_bin, uuid_buf, UUID_LEN);
}

/* This test validates parse_uuid() feeding it with incorrectly-formatted UUID strings.
   No version/variant tests are performed due to lack of handling these properties
   by lib/uuid. If that changes tests should be updated as appropriate.*/
static void test_parse_uuid_incorrect(void **state)
{
	uint8_t uuid_buf[UUID_LEN] = { 0 };
	/* First separator at incorrect index */
	const char *uuid_incorrect_1 = "ab372d0-34cc1-1337-688b-2211337777ee";
	/* Incorrect characters */
	const char *uuid_incorrect_2 = "abbce711-5@51-7777-ffff-ahchbh999911";
	/* Too long uuid string */
	const char *uuid_incorrect_3 = "a7a38502-7465-9cbd-98ea-73003906dad5dd";
	/* Too short uuid string */
	const char *uuid_incorrect_4 = "72abd839-7109-64bd-cfef-a8266";

	assert_int_equal(-1, parse_uuid(uuid_buf, uuid_incorrect_1));
	assert_int_equal(-1, parse_uuid(uuid_buf, uuid_incorrect_2));
	assert_int_equal(-1, parse_uuid(uuid_buf, uuid_incorrect_3));
	assert_int_equal(-1, parse_uuid(uuid_buf, uuid_incorrect_4));
}

static void test_guid(void **state)
{
	guid_t guid = GUID_INIT(0x1F4D66BB, 0xDEAD, 0xBEEF,
				0x08, 0xAB, 0x21, 0x37, 0xBB, 0x07, 0xDD, 0x4C);
	guid_t guid_copy;
	const guid_t guid_bin = { {
			0xBB, 0x66, 0x4D, 0x1F,
			0xAD, 0xDE,
			0xEF, 0xBE,
			0x08, 0xAB, 0x21, 0x37, 0xBB, 0x07, 0xDD, 0x4C
	} };

	/* GUID produced using GUID_INIT() macro should be identical to guid_bin
	   Check it by comparing memory directly and using provided comparison function */
	assert_memory_equal(guid.b, guid_bin.b, UUID_LEN);
	assert_int_equal(0, guidcmp(&guid, &guid_bin));

	/* Copy GUID and compare it with original */
	guidcpy(&guid_copy, &guid);
	assert_memory_equal(guid.b, guid_copy.b, UUID_LEN);
	assert_int_equal(0, guidcmp(&guid, &guid_copy));

	/* Binary negate bits of one of GUIDs bytes.
	   Expect comparison to fail as GUIDs should differ */
	guid_copy.b[7] = ~guid_copy.b[7];
	assert_int_not_equal(0, guidcmp(&guid, &guid_copy));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_parse_uuid_correct),
		cmocka_unit_test(test_parse_uuid_incorrect),
		cmocka_unit_test(test_guid),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
