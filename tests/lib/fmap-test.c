/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <string.h>

#include <tests/test.h>

#include <fmap.h>
#include <commonlib/region.h>

#include <tests/lib/fmap/fmap_data.h>
#include <tests/lib/fmap/fmap_config.h>

static struct mem_region_device mem_rdev_rw;
static struct mem_region_device mem_rdev_ro;
static char *flash_buffer = NULL;
static size_t flash_buffer_size = 0;

static void prepare_flash_buffer(void)
{
	/* Prepare flash buffer with dummy data and FMAP */
	flash_buffer = malloc(FMAP_SECTION_FLASH_SIZE);
	flash_buffer_size = FMAP_SECTION_FLASH_SIZE;

	/* Fill first part of buffer with dummy data */
	for (int i = 0; i < FMAP_SECTION_FMAP_START; ++i)
		flash_buffer[i] = 'a' + i % ('z' - 'a');

	/* Copy FMAP section into buffer */
	memcpy(flash_buffer + FMAP_SECTION_FMAP_START, tests_fmap_bin, FMAP_SIZE);

	/* Fill rest of buffer with dummy data */
	for (int i = FMAP_SECTION_FMAP_START + FMAP_SECTION_FMAP_SIZE;
			i < FMAP_SECTION_FLASH_SIZE; ++i)
		flash_buffer[i] = 'a' + i % ('z' - 'a');
}

static int setup_fmap(void **state)
{
	prepare_flash_buffer();

	mem_rdev_rw = (struct mem_region_device)
			MEM_REGION_DEV_RW_INIT(flash_buffer, FMAP_SECTION_FLASH_SIZE);

	mem_rdev_ro = (struct mem_region_device)
			MEM_REGION_DEV_RO_INIT(flash_buffer, FMAP_SECTION_FLASH_SIZE);

	return 0;
}

static int teardown_fmap(void **state)
{
	struct mem_region_device empty = {
		.base = NULL,
		.rdev = {
			.root = NULL,
			.ops = NULL,
			.region = {
				.offset = 0,
				.size = 0
			}
		}
	};

	mem_rdev_rw = empty;
	mem_rdev_ro = empty;

	free(flash_buffer);
	flash_buffer = NULL;
	flash_buffer_size = 0;

	return 0;
}

void boot_device_init(void)
{
	/* Setup in unit test setup function */
}

const struct region_device *boot_device_ro(void)
{
	return &mem_rdev_rw.rdev;
}

const struct region_device *boot_device_rw(void)
{
	return &mem_rdev_rw.rdev;
}

static void test_fmap_locate_area_as_rdev(void **state)
{
	const char buffer[] = "abcdefghijk0123456789";
	struct region_device rdev;

	assert_int_not_equal(-1, fmap_locate_area_as_rdev("RO_VPD", &rdev));
	assert_int_equal(FMAP_SECTION_RO_VPD_START, region_device_offset(&rdev));
	assert_int_equal(FMAP_SECTION_RO_VPD_SIZE, region_device_sz(&rdev));

	/* Check if locating area second time works */
	assert_int_not_equal(-1, fmap_locate_area_as_rdev("RO_VPD", &rdev));
	assert_int_equal(FMAP_SECTION_RO_VPD_START, region_device_offset(&rdev));
	assert_int_equal(FMAP_SECTION_RO_VPD_SIZE, region_device_sz(&rdev));

	assert_int_not_equal(-1, fmap_locate_area_as_rdev("RECOVERY_MRC_CACHE", &rdev));
	assert_int_equal(FMAP_SECTION_RECOVERY_MRC_CACHE_START, region_device_offset(&rdev));
	assert_int_equal(FMAP_SECTION_RECOVERY_MRC_CACHE_SIZE, region_device_sz(&rdev));

	/* Expect error when writing to read-only area */
	assert_int_equal(-1, rdev_writeat(&rdev, buffer,
					region_device_offset(&rdev), sizeof(buffer)));

	/* Expect error when looking for incorrect area */
	assert_int_equal(-1, fmap_locate_area_as_rdev("NONEXISTENT_AREA", &rdev));
	assert_int_equal(-1, fmap_locate_area_as_rdev("", &rdev));
	assert_int_equal(-1, fmap_locate_area_as_rdev(NULL, &rdev));

	/* Function fmap_locate_area_as_rdev is not tested with NULL
	   as region_device pointer as it is not allowed. */
}

static void test_fmap_locate_area_as_rdev_rw(void **state)
{
	struct region_device rdev;
	size_t ro_rw_section_size = FMAP_SECTION_MISC_RW_SIZE;
	char *buffer1 = malloc(ro_rw_section_size);
	char *buffer2 = malloc(ro_rw_section_size);
	char *dummy_data = malloc(ro_rw_section_size);

	/* Fill buffer with dummy data */
	for (int i = 0; i < ro_rw_section_size; ++i)
		dummy_data[i] = '0' + i % ('9' - '0');

	assert_int_not_equal(-1, fmap_locate_area_as_rdev_rw("RW_SECTION_A", &rdev));
	assert_int_equal(FMAP_SECTION_RW_SECTION_A_START, region_device_offset(&rdev));
	assert_int_equal(FMAP_SECTION_RW_SECTION_A_SIZE, region_device_sz(&rdev));

	/* Check if locating area second time works */
	assert_int_not_equal(-1, fmap_locate_area_as_rdev_rw("RW_SECTION_A", &rdev));
	assert_int_equal(FMAP_SECTION_RW_SECTION_A_START, region_device_offset(&rdev));
	assert_int_equal(FMAP_SECTION_RW_SECTION_A_SIZE, region_device_sz(&rdev));

	assert_int_not_equal(-1, fmap_locate_area_as_rdev_rw("MISC_RW", &rdev));
	assert_int_equal(FMAP_SECTION_MISC_RW_START, region_device_offset(&rdev));
	assert_int_equal(FMAP_SECTION_MISC_RW_SIZE, region_device_sz(&rdev));


	/* Expect error when looking for incorrect area */
	assert_int_equal(-1, fmap_locate_area_as_rdev_rw("NONEXISTENT_AREA", &rdev));
	assert_int_equal(-1, fmap_locate_area_as_rdev_rw("", &rdev));

	/* Expect error when passing invalid references */
	assert_int_equal(-1, fmap_locate_area_as_rdev_rw(NULL, &rdev));

	/* Function fmap_locate_area_as_rdev_rw is not tested with NULL
	   as region_device pointer as it is not allowed. */

	/* Test if returned section region device is writable */
	assert_int_not_equal(-1, fmap_locate_area_as_rdev_rw("MISC_RW", &rdev));
	assert_int_equal(ro_rw_section_size,
			rdev_readat(&rdev, buffer1, 0, ro_rw_section_size));
	assert_int_equal(ro_rw_section_size,
			rdev_writeat(&rdev, dummy_data, 0, ro_rw_section_size));
	/* Check if written data is visible and correct after locating area as RO */
	assert_int_not_equal(-1, fmap_locate_area_as_rdev("MISC_RW", &rdev));
	assert_int_equal(ro_rw_section_size,
			rdev_readat(&rdev, buffer2, 0, ro_rw_section_size));
	assert_memory_not_equal(buffer1, buffer2, ro_rw_section_size);
	assert_memory_equal(dummy_data, buffer2, ro_rw_section_size);

	free(buffer1);
	free(buffer2);
	free(dummy_data);
}

static void test_fmap_locate_area(void **state)
{
	struct region ar;

	/* Try to locate named area */
	assert_int_not_equal(-1, fmap_locate_area("COREBOOT", &ar));
	assert_int_equal(FMAP_SECTION_COREBOOT_START, region_offset(&ar));
	assert_int_equal(FMAP_SECTION_COREBOOT_SIZE, region_sz(&ar));

	/* Check if locating area second time works */
	assert_int_not_equal(-1, fmap_locate_area("COREBOOT", &ar));
	assert_int_equal(FMAP_SECTION_COREBOOT_START, region_offset(&ar));
	assert_int_equal(FMAP_SECTION_COREBOOT_SIZE, region_sz(&ar));

	/* Look for another area */
	assert_int_not_equal(-1, fmap_locate_area("GBB", &ar));
	assert_int_equal(FMAP_SECTION_GBB_START, region_offset(&ar));
	assert_int_equal(FMAP_SECTION_GBB_SIZE, region_sz(&ar));

	/* Expect error when looking for incorrect area */
	assert_int_equal(-1, fmap_locate_area("NONEXISTENT_AREA", &ar));
	assert_int_equal(-1, fmap_locate_area("", &ar));
	assert_int_equal(-1, fmap_locate_area(NULL, &ar));

	/* Expect error when passing invalid region pointer */
	assert_int_equal(-1, fmap_locate_area("SHARED_DATA", NULL));
}

static void test_fmap_find_region_name(void **state)
{
	(void)state;
	struct region ar;
	char found_area_name[FMAP_STRLEN] = "";
	const char *area_name = "RW_PRESERVE";

	/* Find area by name */
	assert_int_not_equal(-1, fmap_locate_area(area_name, &ar));

	/* Find name of previously located region */
	assert_int_not_equal(-1, fmap_find_region_name(&ar, found_area_name));
	assert_string_equal(area_name, found_area_name);

	/* Expect error when passing invalid buffer */
	assert_int_equal(-1, fmap_find_region_name(&ar, NULL));

	/* Expect error when passing invalid region pointer */
	assert_int_equal(-1, fmap_find_region_name(NULL, found_area_name));

	/* Try to find area outside of flash region */
	ar.offset = FMAP_SECTION_FLASH_START + FMAP_SECTION_FLASH_SIZE + 0x100;
	ar.size = 0x1000;
	assert_int_equal(-1, fmap_find_region_name(&ar, found_area_name));

	/* Try to find area with correct offset and incorrect size */
	ar.offset = FMAP_SECTION_COREBOOT_START;
	ar.size = FMAP_SECTION_COREBOOT_SIZE / 4;
	assert_int_equal(-1, fmap_find_region_name(&ar, found_area_name));

	/* Try to find area with correct size and incorrect offset */
	ar.offset = FMAP_SECTION_GBB_START - 0x100;
	ar.size = FMAP_SECTION_GBB_START;
	assert_int_equal(-1, fmap_find_region_name(&ar, found_area_name));

	/* Try to find area with correct offset overlapping with another area */
	ar.offset = FMAP_SECTION_MISC_RW_START;
	ar.size = FMAP_SECTION_MISC_RW_START + 0x1000;
	assert_int_equal(-1, fmap_find_region_name(&ar, found_area_name));
}

static void test_fmap_read_area(void **state)
{
	const unsigned int section_size = FMAP_SECTION_RW_SECTION_A_SIZE;
	const unsigned int section_start = FMAP_SECTION_RW_SECTION_A_START;
	char *buffer = malloc(section_size);

	/* Find and read area data. Compare with memory device simulating flash. */
	assert_int_equal(section_size, fmap_read_area("RW_SECTION_A", buffer, section_size));
	assert_memory_equal(flash_buffer + section_start, buffer, section_size);

	/* Expect error when reading incorrect area */
	assert_int_equal(-1, fmap_read_area("NONEXISTENT_SECTION", buffer, section_size));
	assert_int_equal(-1, fmap_read_area("", buffer, section_size));
	assert_int_equal(-1, fmap_read_area(NULL, buffer, section_size));

	/* Function fmap_read_area is not tested with NULL
	   as output buffer pointer as it is not allowed. */

	free(buffer);
}

static void test_fmap_overwrite_area(void **state)
{
	const char *section_name = "FW_MAIN_A";
	const unsigned int section_size = FMAP_SECTION_FW_MAIN_A_SIZE;
	char *buffer1 = malloc(section_size);
	char *buffer2 = malloc(section_size);
	char *new_data = malloc(section_size / 2);
	char *zero_buffer = malloc(section_size / 2);
	memset(zero_buffer, 0, section_size / 2);
	memset(new_data, 0x42, section_size / 2);

	/* Save buffer for future comparisons */
	assert_int_equal(section_size, fmap_read_area(section_name, buffer1, section_size));

	/* Overwrite part of section. */
	assert_int_equal(section_size / 2,
			fmap_overwrite_area(section_name, new_data, section_size / 2));

	/* Read and check if memory has changed as expected */
	assert_int_equal(section_size, fmap_read_area(section_name, buffer2, section_size));
	assert_memory_not_equal(buffer1, buffer2, section_size);
	/* Check if requested section area was overwritten properly */
	assert_memory_equal(buffer2, new_data, section_size / 2);
	/* Check if rest of section was zero-filled */
	assert_memory_equal(buffer2 + (section_size / 2), zero_buffer, section_size / 2);

	/* Expect error when overwriting incorrect section */
	assert_int_equal(-1, fmap_overwrite_area("NONEXISTENT_SECTION",
							new_data, section_size / 2));
	assert_int_equal(-1, fmap_overwrite_area(NULL, new_data, section_size / 2));

	/* Function fmap_overwrite_area is not tested with NULL
	   as input buffer pointer as it is not allowed. */

	free(buffer1);
	free(buffer2);
	free(new_data);
	free(zero_buffer);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_fmap_locate_area_as_rdev,
						setup_fmap, teardown_fmap),
		cmocka_unit_test_setup_teardown(test_fmap_locate_area_as_rdev_rw,
						setup_fmap, teardown_fmap),
		cmocka_unit_test_setup_teardown(test_fmap_locate_area,
						setup_fmap, teardown_fmap),
		cmocka_unit_test_setup_teardown(test_fmap_find_region_name,
						setup_fmap, teardown_fmap),
		cmocka_unit_test_setup_teardown(test_fmap_read_area,
						setup_fmap, teardown_fmap),
		cmocka_unit_test_setup_teardown(test_fmap_overwrite_area,
						setup_fmap, teardown_fmap),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
