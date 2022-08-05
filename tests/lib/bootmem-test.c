/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <commonlib/coreboot_tables.h>
#include <device/device.h>
#include <device/resource.h>
#include <memrange.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>
#include <tests/test.h>

/* Stubs defined to satisfy linker dependencies */
void cbmem_add_bootmem(void)
{
}

void bootmem_arch_add_ranges(void)
{
}

struct bootmem_ranges_t {
	uint64_t start;
	uint64_t size;
	uint32_t type;
};

/* Define symbols for regions required by bootmem.
   Define constants for regions that do not need to be defined in the executable.
   There is no need for region memory, just start, end and size symbols are required.
   Only used values are defined. */
#define ZERO_REGION_START ((uintptr_t)0x0)
#define ZERO_REGION_SIZE  ((uintptr_t)0x10000)

TEST_REGION_UNALLOCATED(program, 0x10000000, 0x40000);
#define PROGRAM_START ((uintptr_t)_program)
#define PROGRAM_SIZE  REGION_SIZE(program)

#define CACHEABLE_START	((uintptr_t)0x10000000ULL)
#define CACHEABLE_SIZE	((uintptr_t)0x100000000ULL)
#define CACHEABLE_END	((uintptr_t)(CACHEABLE_START + CACHEABLE_SIZE))

/* Stack region end address is hardcoded because `<const> - <symbol>` does not work in GCC */
TEST_REGION_UNALLOCATED(stack, 0x10040000, 0x1000);
#define STACK_START ((uintptr_t)_stack)
#define STACK_SIZE  REGION_SIZE(stack)
#define STACK_END   ((uintptr_t)(0x10040000 + 0x1000))

#define RESERVED_START	((uintptr_t)0x100000000ULL)
#define RESERVED_SIZE	((uintptr_t)0x100000)
#define RESERVED_END	((uintptr_t)(RESERVED_START + RESERVED_SIZE))

TEST_REGION_UNALLOCATED(ramstage, 0x10000000, 0x41000);
#define RAMSTAGE_START  ((uintptr_t)_ramstage)
#define RAMSTAGE_SIZE	REGION_SIZE(ramstage)

#define CACHEABLE_START_TO_RESERVED_START_SIZE (RESERVED_START - CACHEABLE_START)
#define RESERVED_END_TO_CACHEABLE_END_SIZE (CACHEABLE_END - RESERVED_END)
#define STACK_END_TO_RESERVED_START_SIZE (RESERVED_START - STACK_END)


/* Bootmem layout for tests
 *
 * Regions marked with asterisks (***) are not visible for OS
 *
 *     +------------------ZERO-----------------+ <-0x0
 *     |                                       |
 *     +---------------------------------------+ <-0x10000
 *
 *     +-------+----CACHEABLE_MEMORY---------+-+ <-0x10000000
 *     |       |        ***PROGRAM***        | |
 *     |       +-----------------------------+ | <-0x10040000
 *     |       |         ***STACK***         | |
 *     |       +-----------------------------+ | <-0x10041000
 *     |                                       |
 *     |                                       |
 *     |                                       |
 *     |       +-------RESERVED_MEMORY-------+ | <-0x100000000
 *     |       |                             | |
 *     |       |                             | |
 *     |       |                             | |
 *     |       +-----------------------------+ | <-0x100100000
 *     |                                       |
 *     |                                       |
 *     +---------------------------------------+ <-0x110000000
 *
 * Ramstage covers PROGRAM and STACK regions.
 */
struct bootmem_ranges_t os_ranges_mock[] = {
	[0] = { .start = ZERO_REGION_START, .size = ZERO_REGION_SIZE,
		.type = BM_MEM_RAM},
	[1] = { .start = CACHEABLE_START, .size = CACHEABLE_START_TO_RESERVED_START_SIZE,
		.type = BM_MEM_RAM },
	[2] = { .start = RESERVED_START, .size = RESERVED_SIZE,
		.type = BM_MEM_RESERVED },
	[3] = { .start = RESERVED_END, .size = RESERVED_END_TO_CACHEABLE_END_SIZE,
		.type = BM_MEM_RAM },
};

struct bootmem_ranges_t ranges_mock[] = {
	[0] = { .start = ZERO_REGION_START, .size = ZERO_REGION_SIZE,
		.type = BM_MEM_RAM },
	[1] = { .start = RAMSTAGE_START, .size = RAMSTAGE_SIZE,
		.type = BM_MEM_RAMSTAGE },
	[2] = { .start = STACK_END, .size = STACK_END_TO_RESERVED_START_SIZE,
		.type = BM_MEM_RAM },
	[3] = { .start = RESERVED_START, .size = RESERVED_SIZE,
		.type = BM_MEM_RESERVED },
	[4] = { .start = RESERVED_END, .size = RESERVED_END_TO_CACHEABLE_END_SIZE,
		.type = BM_MEM_RAM },
};

struct bootmem_ranges_t *os_ranges = os_ranges_mock;
struct bootmem_ranges_t *ranges = ranges_mock;

/* Note that second region overlaps first */
struct resource res_mock[] = {
	{ .base = ZERO_REGION_START, .size = ZERO_REGION_SIZE, .next = &res_mock[1],
	  .flags = IORESOURCE_CACHEABLE | IORESOURCE_MEM | IORESOURCE_ASSIGNED },
	{ .base = CACHEABLE_START, .size = CACHEABLE_SIZE, .next = &res_mock[2],
	  .flags = IORESOURCE_CACHEABLE | IORESOURCE_MEM | IORESOURCE_ASSIGNED },
	{ .base = RESERVED_START, .size = RESERVED_SIZE, .next = NULL,
	  .flags = IORESOURCE_RESERVE | IORESOURCE_MEM | IORESOURCE_ASSIGNED }
};

/* Device simulating RAM */
struct device mem_device_mock = {
	.enabled = 1,
	.resource_list = res_mock,
	.next = NULL
};

struct device *all_devices = &mem_device_mock;

/* Simplified version for the purpose of tests */
static uint32_t bootmem_to_lb_tag(const enum bootmem_type tag)
{
	switch (tag) {
	case BM_MEM_RAM:
		return LB_MEM_RAM;
	case BM_MEM_RESERVED:
		return LB_MEM_RESERVED;
	default:
		return LB_MEM_RESERVED;
	}
}

static void test_bootmem_write_mem_table(void **state)
{
	/* Space for 10 lb_mem entries to be safe */
	const size_t lb_mem_max_size = sizeof(struct lb_memory)
					+ 10 * sizeof(struct lb_memory_range);
	const size_t expected_allocation_size =
			(sizeof(struct lb_memory)
				+ ARRAY_SIZE(os_ranges_mock) * sizeof(struct lb_memory_range));
	const size_t required_unused_space_size = lb_mem_max_size - expected_allocation_size;
	int i;
	struct lb_memory *lb_mem;
	/* Allocate buffer and fill it. Use it to ensure correct size of space used
	   by bootmem_write_memory_table() */
	u8 sentinel_value_buffer[required_unused_space_size];
	memset(sentinel_value_buffer, 0x77, required_unused_space_size);

	lb_mem = malloc(lb_mem_max_size);
	lb_mem->tag = LB_TAG_MEMORY;
	lb_mem->size = sizeof(*lb_mem);
	/* Fill rest of buffer with sentinel value */
	memset(((u8 *)lb_mem) + expected_allocation_size, 0x77, required_unused_space_size);

	bootmem_write_memory_table(lb_mem);

	/* There should be only `os_ranges_mock` entries visible in coreboot table */
	assert_int_equal(lb_mem->size, sizeof(*lb_mem) +
			ARRAY_SIZE(os_ranges_mock) * sizeof(struct lb_memory_range));
	assert_memory_equal(sentinel_value_buffer,
			((u8 *)lb_mem) + expected_allocation_size,
			required_unused_space_size);

	for (i = 0; i < lb_mem->size / sizeof(struct lb_memory_range); i++) {
		assert_int_equal(lb_mem->map[i].start, os_ranges[i].start);
		assert_int_equal(lb_mem->map[i].size, os_ranges[i].size);
		assert_int_equal(lb_mem->map[i].type, bootmem_to_lb_tag(os_ranges[i].type));
	}

	free(lb_mem);
}

int os_bootmem_walk_cnt;
int bootmem_walk_cnt;

static bool verify_os_bootmem_walk(const struct range_entry *r, void *arg)
{
	assert_int_equal(range_entry_base(r), os_ranges[os_bootmem_walk_cnt].start);
	assert_int_equal(range_entry_size(r), os_ranges[os_bootmem_walk_cnt].size);
	assert_int_equal(range_entry_tag(r), os_ranges[os_bootmem_walk_cnt].type);

	os_bootmem_walk_cnt++;

	return true;
}

static bool verify_bootmem_walk(const struct range_entry *r, void *arg)
{
	assert_int_equal(range_entry_base(r), ranges[bootmem_walk_cnt].start);
	assert_int_equal(range_entry_size(r), ranges[bootmem_walk_cnt].size);
	assert_int_equal(range_entry_tag(r), ranges[bootmem_walk_cnt].type);

	bootmem_walk_cnt++;

	return true;
}

static bool count_entries_os_bootmem_walk(const struct range_entry *r, void *arg)
{
	os_bootmem_walk_cnt++;

	return true;
}

static bool count_entries_bootmem_walk(const struct range_entry *r, void *arg)
{
	bootmem_walk_cnt++;

	return true;
}

/* This function initializes bootmem using bootmem_write_memory_table().
   bootmem_init() is not accessible directly because it is static. */
static void init_memory_table_library(void)
{
	struct lb_memory *lb_mem;

	/* Allocate space for 10 lb_mem entries to be safe */
	lb_mem = malloc(sizeof(*lb_mem) + 10 * sizeof(struct lb_memory_range));
	lb_mem->tag = LB_TAG_MEMORY;
	lb_mem->size = sizeof(*lb_mem);

	/* We need to call this only to initialize library */
	bootmem_write_memory_table(lb_mem);

	free(lb_mem);
}

static void test_bootmem_add_range(void **state)
{
	init_memory_table_library();

	os_bootmem_walk_cnt = 0;
	bootmem_walk_os_mem(count_entries_os_bootmem_walk, NULL);
	assert_int_equal(os_bootmem_walk_cnt, 4);

	bootmem_walk_cnt = 0;
	bootmem_walk(count_entries_bootmem_walk, NULL);
	assert_int_equal(bootmem_walk_cnt, 5);

	expect_assert_failure(
		bootmem_add_range(ALIGN_UP(PROGRAM_START, 4096),
				  ALIGN_DOWN(PROGRAM_SIZE / 2, 4096),
				  BM_MEM_ACPI)
	);

	os_bootmem_walk_cnt = 0;
	bootmem_walk_os_mem(count_entries_os_bootmem_walk, NULL);
	assert_int_equal(os_bootmem_walk_cnt, 4);

	bootmem_walk_cnt = 0;
	bootmem_walk(count_entries_bootmem_walk, NULL);
	assert_int_equal(bootmem_walk_cnt, 6);

	/* Do not expect assert failure as BM_MEM_RAMSTAGE should not be added to os_bootmem */
	bootmem_add_range(ALIGN_UP(STACK_END + 4096, 4096),
			  ALIGN_DOWN(STACK_END_TO_RESERVED_START_SIZE / 2, 4096),
			  BM_MEM_RAMSTAGE);

	os_bootmem_walk_cnt = 0;
	bootmem_walk_os_mem(count_entries_os_bootmem_walk, NULL);
	assert_int_equal(os_bootmem_walk_cnt, 4);

	/* Two entries are added because added range is in middle of another */
	bootmem_walk_cnt = 0;
	bootmem_walk(count_entries_bootmem_walk, NULL);
	assert_int_equal(bootmem_walk_cnt, 8);
}

static void test_bootmem_walk(void **state)
{
	init_memory_table_library();

	os_bootmem_walk_cnt = 0;
	bootmem_walk_os_mem(verify_os_bootmem_walk, NULL);
	assert_int_equal(os_bootmem_walk_cnt, 4);

	bootmem_walk_cnt = 0;
	bootmem_walk(verify_bootmem_walk, NULL);
	assert_int_equal(bootmem_walk_cnt, 5);
}

static void test_bootmem_region_targets_type(void **state)
{
	int ret;
	u64 subregion_start;
	u64 subregion_size;

	init_memory_table_library();

	/* Single whole region */
	ret = bootmem_region_targets_type(RAMSTAGE_START, RAMSTAGE_SIZE, BM_MEM_RAMSTAGE);
	assert_int_equal(ret, 1);

	/* Expect fail because of incorrect bootmem_type */
	ret = bootmem_region_targets_type(RAMSTAGE_START, RAMSTAGE_SIZE, BM_MEM_RESERVED);
	assert_int_equal(ret, 0);

	/* Range covering one more byte than one region */
	ret = bootmem_region_targets_type(RAMSTAGE_START, RAMSTAGE_SIZE + 1, BM_MEM_RAMSTAGE);
	assert_int_equal(ret, 0);

	/* Expect success for subregion of ramstage stretching from point in program range
	   to point in stack range. */
	subregion_start = PROGRAM_START + PROGRAM_SIZE / 4;
	subregion_size = STACK_END - STACK_SIZE / 4 - subregion_start;
	ret = bootmem_region_targets_type(subregion_start, subregion_size, BM_MEM_RAMSTAGE);
	assert_int_equal(ret, 1);

	/* Expect fail for range covering more than one tag as there is no BM_MEM_CACHEABLE */
	subregion_start = STACK_START + STACK_SIZE / 2;
	subregion_size = RESERVED_START + RESERVED_SIZE / 4 * 3 - subregion_start;
	ret = bootmem_region_targets_type(subregion_start, subregion_size, BM_MEM_RAM);
	assert_int_equal(ret, 0);

	/* Middle of range should not fail */
	ret = bootmem_region_targets_type(RESERVED_START + RESERVED_SIZE / 4,
					  RESERVED_SIZE / 2, BM_MEM_RESERVED);
	assert_int_equal(ret, 1);

	/* Subsection of range bordering end edge */
	ret = bootmem_region_targets_type(RESERVED_END + RESERVED_END_TO_CACHEABLE_END_SIZE / 2,
					  RESERVED_END_TO_CACHEABLE_END_SIZE / 2, BM_MEM_RAM);
	assert_int_equal(ret, 1);

	/* Region touching zero */
	ret = bootmem_region_targets_type(ZERO_REGION_START, ZERO_REGION_SIZE, BM_MEM_RAM);
	assert_int_equal(ret, 1);

	/* Expect failure when passing zero as size. */
	ret = bootmem_region_targets_type(ZERO_REGION_START, 0, BM_MEM_RAM);
	assert_int_equal(ret, 0);
	ret = bootmem_region_targets_type(RESERVED_START, 0, BM_MEM_RESERVED);
	assert_int_equal(ret, 0);
}

/* Action function used to check alignment of size and base of allocated ranges */
static bool verify_bootmem_allocate_buffer(const struct range_entry *r, void *arg)
{
	if (range_entry_tag(r) == BM_MEM_PAYLOAD) {
		assert_true(IS_ALIGNED(range_entry_base(r), 4096));
		assert_true(IS_ALIGNED(range_entry_size(r), 4096));
	}

	return true;
}


static void test_bootmem_allocate_buffer(void **state)
{
	void *buf;
	void *prev;

	init_memory_table_library();

	/* All allocated buffers should be below 32bit boundary */
	buf = bootmem_allocate_buffer(1ULL << 32);
	assert_null(buf);

	/* Try too big size for our BM_MEM_RAM range below 32bit boundary */
	buf = bootmem_allocate_buffer(RESERVED_START - PROGRAM_START);
	assert_null(buf);

	/* Two working cases */
	buf = bootmem_allocate_buffer(0xE0000000);
	assert_non_null(buf);
	assert_int_equal(1, bootmem_region_targets_type((uintptr_t)buf,
							0xE0000000, BM_MEM_PAYLOAD));
	assert_in_range((uintptr_t)buf, CACHEABLE_START + RAMSTAGE_SIZE, RESERVED_START);
	/* Check if allocated (payload) ranges have their base and size aligned */
	bootmem_walk(verify_bootmem_allocate_buffer, NULL);

	prev = buf;
	buf = bootmem_allocate_buffer(0xF000000);
	assert_non_null(buf);
	assert_int_equal(1, bootmem_region_targets_type((uintptr_t)buf,
							0xF000000, BM_MEM_PAYLOAD));
	assert_in_range((uintptr_t)buf, CACHEABLE_START + RAMSTAGE_SIZE, RESERVED_START);
	/* Check if newly allocated buffer does not overlap with previously allocated range */
	assert_not_in_range((uintptr_t)buf, (uintptr_t)prev, (uintptr_t)prev + 0xE0000000);
	/* Check if allocated (payload) ranges have their base and size aligned */
	bootmem_walk(verify_bootmem_allocate_buffer, NULL);

	/* Run out of memory for new allocations */
	buf = bootmem_allocate_buffer(0x1000000);
	assert_null(buf);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_bootmem_write_mem_table),
		cmocka_unit_test(test_bootmem_add_range),
		cmocka_unit_test(test_bootmem_walk),
		cmocka_unit_test(test_bootmem_allocate_buffer),
		cmocka_unit_test(test_bootmem_region_targets_type)
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
