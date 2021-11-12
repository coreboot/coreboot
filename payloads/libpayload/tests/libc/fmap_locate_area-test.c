/* SPDX-License-Identifier: GPL-2.0-only */

#include "../libc/fmap.c"

#include <libpayload.h>
#include <tests/test.h>


/* Mocks */
struct sysinfo_t lib_sysinfo;
unsigned long virtual_offset = 0;

static void reset_fmap_cache(void)
{
	_fmap_cache = NULL;
}

static int setup_fmap_test(void **state)
{
	reset_fmap_cache();
	lib_sysinfo.fmap_cache = 0;
	return 0;
}

static void test_fmap_locate_area_no_fmap_available(void **state)
{
	size_t offset = 0;
	size_t size = 0;

	assert_int_equal(-1, fmap_locate_area("COREBOOT", &offset, &size));
}

static void test_fmap_locate_area_incorrect_signature(void **state)
{
	size_t offset = 0;
	size_t size = 0;
	struct fmap mock_fmap = {
		.signature = "NOT_MAP",
	};
	lib_sysinfo.fmap_cache = (uintptr_t)&mock_fmap;

	assert_int_equal(-1, fmap_locate_area("COREBOOT", &offset, &size));
}

static void test_fmap_locate_area_success(void **state)
{
	size_t offset = 0;
	size_t size = 0;
	struct fmap mock_fmap = {
		.signature = FMAP_SIGNATURE,
		.ver_major = 1,
		.ver_minor = 1,
		.base = 0xAABB,
		.size = 0x10000,
		.nareas = 3,
	};
	struct fmap_area area_1 = {
		.size = 0x1100,
		.offset = 0x11,
		.name = {'F', 'I', 'R', 'S', 'T', '_', 'A', 'R', 'E', 'A', 0},
		.flags = 0,
	};
	struct fmap_area area_2 = {
		.size = 0x2200,
		.offset = 0x1111,
		.name = {'S', 'E', 'C', 'O', 'N', 'D', '_', 'A', 'R', 'E', 'A', 0},
		.flags = 0,
	};
	struct fmap_area area_3 = {
		.size = 0x100,
		.offset = 0x3311,
		.name = {'T', 'H', 'I', 'R', 'D', '_', 'A', 'R', 'E', 'A', 0},
		.flags = 0,
	};
	u8 fmap_buffer[sizeof(struct fmap) + 3 * sizeof(struct fmap_area)];
	memcpy(fmap_buffer, &mock_fmap, sizeof(mock_fmap));
	memcpy(&fmap_buffer[sizeof(mock_fmap)], &area_1, sizeof(area_1));
	memcpy(&fmap_buffer[sizeof(mock_fmap) + sizeof(area_1)], &area_2, sizeof(area_2));
	memcpy(&fmap_buffer[sizeof(mock_fmap) + sizeof(area_1) + sizeof(area_2)], &area_3,
	       sizeof(area_3));

	/* Cache only */
	reset_fmap_cache();
	lib_sysinfo.fmap_cache = (uintptr_t)fmap_buffer;

	assert_int_equal(0, fmap_locate_area("FIRST_AREA", &offset, &size));
	assert_int_equal(area_1.offset, offset);
	assert_int_equal(area_1.size, size);

	assert_int_equal(0, fmap_locate_area("THIRD_AREA", &offset, &size));
	assert_int_equal(area_3.offset, offset);
	assert_int_equal(area_3.size, size);

	assert_int_equal(0, fmap_locate_area("SECOND_AREA", &offset, &size));
	assert_int_equal(area_2.offset, offset);
	assert_int_equal(area_2.size, size);

	reset_fmap_cache();
}

#define FMAP_LOCATE_AREA_TEST(fn) cmocka_unit_test_setup(fn, setup_fmap_test)

int main(void)
{
	const struct CMUnitTest tests[] = {
		FMAP_LOCATE_AREA_TEST(test_fmap_locate_area_no_fmap_available),
		FMAP_LOCATE_AREA_TEST(test_fmap_locate_area_incorrect_signature),
		FMAP_LOCATE_AREA_TEST(test_fmap_locate_area_success),
	};

	return lp_run_group_tests(tests, NULL, NULL);
}
