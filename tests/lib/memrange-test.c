/* SPDX-License-Identifier: GPL-2.0-only */

#include <tests/test.h>

#include <device/device.h>
#include <device/resource.h>
#include <commonlib/helpers.h>
#include <memrange.h>

#define MEMRANGE_ALIGN (POWER_OF_2(12))

enum mem_types {
	/* Avoid using 0 to verify that UUT really sets this memory,
	   but keep value small, as this will be an index in the table */
	CACHEABLE_TAG = 0x10,
	RESERVED_TAG,
	READONLY_TAG,
	INSERTED_TAG,
	HOLE_TAG,
	UNASSIGNED_TAG,
	END_OF_RESOURCES
};

/* Indices of entries matters, since it must reflect mem_types enum */
struct resource res_mock_1[] = {
	[UNASSIGNED_TAG] = {.base = 0x0,
			    .size = 0x8000,
			    .next = &res_mock_1[CACHEABLE_TAG],
			    .flags = IORESOURCE_MEM | IORESOURCE_PREFETCH},
	[CACHEABLE_TAG] = {.base = 0xE000,
			   .size = 0xF2000,
			   .next = &res_mock_1[RESERVED_TAG],
			   .flags = IORESOURCE_CACHEABLE | IORESOURCE_MEM |
				   IORESOURCE_ASSIGNED },
	[RESERVED_TAG] = {.base = 4ULL * GiB,
			  .size = 4ULL * KiB,
			  .next = &res_mock_1[READONLY_TAG],
			  .flags = IORESOURCE_RESERVE | IORESOURCE_MEM |
				  IORESOURCE_ASSIGNED },
	[READONLY_TAG] = {.base = 0xFF0000,
			  .size = 0x10000,
			  .next = NULL,
			  .flags = IORESOURCE_READONLY | IORESOURCE_MEM |
				  IORESOURCE_ASSIGNED }
};

/* Boundary 1 byte below 4GiB and 1 byte above 4GiB. */
struct resource res_mock_2[] = {
	[CACHEABLE_TAG] = {.base = 0x1000000,
			   .size = 4ULL * GiB - 0x1000001ULL,
			   .next = &res_mock_2[RESERVED_TAG],
			   .flags = IORESOURCE_CACHEABLE | IORESOURCE_MEM |
				   IORESOURCE_ASSIGNED },
	[RESERVED_TAG] = {.base = 4ULL * GiB + 1ULL,
			  .size = 4ULL * GiB,
			  .next = &res_mock_2[READONLY_TAG],
			  .flags = IORESOURCE_RESERVE | IORESOURCE_MEM |
				  IORESOURCE_ASSIGNED },
	[READONLY_TAG] = {.base = 0,
			  .size = 0x10000,
			  .next = NULL,
			  .flags = IORESOURCE_READONLY | IORESOURCE_MEM |
				  IORESOURCE_ASSIGNED }
};

/* Boundary crossing 4GiB. */
struct resource res_mock_3[] = {
	[CACHEABLE_TAG] = {.base = 0xD000,
			   .size = 0xF3000,
			   .next = &res_mock_3[RESERVED_TAG],
			   .flags = IORESOURCE_CACHEABLE | IORESOURCE_MEM |
				   IORESOURCE_ASSIGNED },
	[RESERVED_TAG] = {.base = 1ULL * GiB,
			  .size = 4ULL * GiB,
			  .next = &res_mock_3[READONLY_TAG],
			  .flags = IORESOURCE_RESERVE | IORESOURCE_MEM |
				  IORESOURCE_ASSIGNED },
	[READONLY_TAG] = {.base = 0xFF0000,
			  .size = 0x10000,
			  .next = NULL,
			  .flags = IORESOURCE_READONLY | IORESOURCE_MEM |
				  IORESOURCE_ASSIGNED}
};


struct device mock_device = {.enabled = 1};

/* Fake memory devices handle */
struct device *all_devices = &mock_device;

int setup_test_1(void **state)
{
	*state = res_mock_1;
	mock_device.resource_list = &res_mock_1[UNASSIGNED_TAG];

	return 0;
}

int setup_test_2(void **state)
{
	*state = res_mock_2;
	mock_device.resource_list = &res_mock_2[CACHEABLE_TAG];

	return 0;
}

int setup_test_3(void **state)
{
	*state = res_mock_3;
	mock_device.resource_list = &res_mock_3[CACHEABLE_TAG];

	return 0;
}

resource_t get_aligned_base(struct resource *res, struct range_entry *entry)
{
	return ALIGN_DOWN(res[range_entry_tag(entry)].base, MEMRANGE_ALIGN);
}

resource_t get_aligned_end(struct resource *res, struct range_entry *entry)
{
	resource_t end = res[range_entry_tag(entry)].base + res[range_entry_tag(entry)].size
			 + (res[range_entry_tag(entry)].base - range_entry_base(entry));
	return ALIGN_UP(end, MEMRANGE_ALIGN);
}

/*
 * This test verifies memranges_init(), memranges_add_resources() and memranges_teardown()
 * functions. It covers basic functionality of memrange library - implementation of creating
 * memrange structure from resources available on the platform and method for free'ing
 * allocated memory.
 *
 * Example memory ranges (res_mock1) for test_memrange_basic.
 * Ranges marked with asterisks (***) are not added to the test_memrange.
 *
 *     +-------UNASSIGNED_TAG--------+ <-0x0
 *     |                             |
 *     +-----------------------------+ <-0x8000
 *
 *
 *
 *     +--------CACHEABLE_TAG--------+ <-0xE000
 *     |                             |
 *     |                             |
 *     |                             |
 *     +-----------------------------+ <-0x100000
 *
 *
 *
 *     +-----***READONLY_TAG***------+ <-0xFF0000
 *     |                             |
 *     |                             |
 *     |                             |
 *     +-----------------------------+ <-0x1000000
 *
 *
 *     +--------RESERVED_TAG---------+ <-0x100000000
 *     |                             |
 *     +-----------------------------+ <-0x100001000
 */
static void test_memrange_basic(void **state)
{
	int counter = 0;
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	const unsigned long prefetchable = IORESOURCE_PREFETCH;
	struct range_entry *ptr;
	struct memranges test_memrange;
	struct resource *res_mock = *state;
	resource_t prev_base = 0;

	memranges_init_empty(&test_memrange, NULL, 0);
	memranges_add_resources(&test_memrange, prefetchable, prefetchable, UNASSIGNED_TAG);
	memranges_add_resources(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);

	/* There should be two entries, since cacheable and reserved regions are not neighbors.
	   Besides these two, a region with an unassigned tag is defined, to emulate an unmapped
	   PCI BAR resource. This resource is not mapped into host physical address and hence
	   should not be picked up by memranges_add_resources().*/

	memranges_each_entry(ptr, &test_memrange)
	{
		assert_in_range(range_entry_tag(ptr), CACHEABLE_TAG, RESERVED_TAG);
		assert_int_equal(range_entry_base(ptr), get_aligned_base(res_mock, ptr));

		assert_int_equal(range_entry_end(ptr), get_aligned_end(res_mock, ptr));

		/* Ranges have to be returned in increasing order */
		assert_true(prev_base <= range_entry_base(ptr));

		prev_base = range_entry_base(ptr);
		counter++;
	};
	assert_int_equal(counter, 2);
	counter = 0;

	/* Remove initial memrange */
	memranges_teardown(&test_memrange);
	memranges_each_entry(ptr, &test_memrange) counter++;
	assert_int_equal(counter, 0);
}

/*
 * This test verifies memranges_clone(), memranges_insert() and memranges_update_tag()
 * functions. All operations are performed on cloned memrange. One of the most important thing
 * to check, is that memrange_insert() should remove all ranges which are covered by the newly
 * inserted one.
 *
 * Example memory ranges (res_mock1) for test_memrange_clone_insert.
 * Ranges marked with asterisks (***) are not added to the clone_memrange.
 * Ranges marked with (^) have tag value changed during test.
 *
 *                +--------CACHEABLE_TAG--------+ <-0xE000
 *         +------|----INSERTED_TAG----------+  | <-0xF000
 *         |      |  (^READONLY_TAG^)        |  |
 *         |      |                          |  |
 *         |      +-----------------------------+ <-0x100000
 *         +---------------------------------+    <-0x101000
 *
 *
 *                +-----***READONLY_TAG***------+ <-0xFF0000
 *                |                             |
 *                |                             |
 *                |                             |
 *                +-----------------------------+ <-0x1000000
 *
 *
 *         +------+---------RESERVED_TAG-----+--+ <-0x100000000
 *         |      |                          |  |
 *         |      +-----------------------------+ <-0x100001000
 *         +-----------INSERTED_TAG----------+    <-0x100002000
 */
static void test_memrange_clone_insert(void **state)
{
	int counter = 0;
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	struct range_entry *ptr;
	struct memranges test_memrange, clone_memrange;
	struct resource *res_mock = *state;
	const resource_t new_range_begin_offset = 1ULL << 12;

	memranges_init(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);

	memranges_clone(&clone_memrange, &test_memrange);
	memranges_teardown(&test_memrange);

	/* Verify that new one is really a clone */
	memranges_each_entry(ptr, &clone_memrange)
	{
		assert_in_range(range_entry_tag(ptr), CACHEABLE_TAG, END_OF_RESOURCES - 1);
		assert_int_equal(range_entry_base(ptr), get_aligned_base(res_mock, ptr));

		assert_int_equal(range_entry_end(ptr), get_aligned_end(res_mock, ptr));

		counter++;
	};
	assert_int_equal(counter, 2);
	counter = 0;

	/* Insert new range, which will overlap with first region. */
	memranges_insert(&clone_memrange, res_mock[CACHEABLE_TAG].base + new_range_begin_offset,
			 res_mock[CACHEABLE_TAG].size, INSERTED_TAG);

	/* Three ranges should be there - CACHEABLE(shrunk), INSERTED and RESERVED */
	memranges_each_entry(ptr, &clone_memrange)
	{
		resource_t expected_end;

		if (range_entry_tag(ptr) == CACHEABLE_TAG) {
			assert_int_equal(range_entry_base(ptr), res_mock[CACHEABLE_TAG].base);

			expected_end = res_mock[CACHEABLE_TAG].base + new_range_begin_offset;
			assert_int_equal(range_entry_end(ptr), expected_end);
		}
		if (range_entry_tag(ptr) == INSERTED_TAG) {
			assert_int_equal(range_entry_base(ptr),
					 res_mock[CACHEABLE_TAG].base + new_range_begin_offset);

			expected_end = res_mock[CACHEABLE_TAG].base + new_range_begin_offset
				       + res_mock[CACHEABLE_TAG].size;
			assert_int_equal(range_entry_end(ptr),
					 ALIGN_UP(expected_end, MEMRANGE_ALIGN));
		}
		counter++;
	}
	assert_int_equal(counter, 3);
	counter = 0;

	/* Insert new region, which will shadow readonly range.
	 * Additionally verify API for updating tags */
	memranges_update_tag(&clone_memrange, INSERTED_TAG, READONLY_TAG);

	memranges_each_entry(ptr, &clone_memrange)
	{
		resource_t expected_end;

		assert_int_not_equal(range_entry_tag(ptr), INSERTED_TAG);
		if (range_entry_tag(ptr) == READONLY_TAG) {
			assert_int_equal(range_entry_base(ptr),
					 res_mock[CACHEABLE_TAG].base + new_range_begin_offset);

			expected_end = res_mock[CACHEABLE_TAG].base + new_range_begin_offset
				       + res_mock[CACHEABLE_TAG].size;
			assert_int_equal(range_entry_end(ptr),
					 ALIGN_UP(expected_end, MEMRANGE_ALIGN));
		}
	};

	/* Check if alignment (4KiB) is properly applied, that is begin - DOWN and end - UP */
	memranges_insert(&clone_memrange, res_mock[RESERVED_TAG].base + 0xAD,
			 res_mock[RESERVED_TAG].size, INSERTED_TAG);

	memranges_each_entry(ptr, &clone_memrange)
	{
		resource_t expected_end;

		assert_int_not_equal(range_entry_tag(ptr), RESERVED_TAG);
		if (range_entry_tag(ptr) == INSERTED_TAG) {
			assert_int_equal(
				range_entry_base(ptr),
				ALIGN_DOWN(res_mock[RESERVED_TAG].base, MEMRANGE_ALIGN));

			expected_end = ALIGN_DOWN(res_mock[RESERVED_TAG].base, MEMRANGE_ALIGN)
				       + new_range_begin_offset + res_mock[RESERVED_TAG].size;
			expected_end = ALIGN_UP(expected_end, MEMRANGE_ALIGN);

			assert_int_equal(range_entry_end(ptr), expected_end);
		}
		counter++;
	}
	assert_int_equal(counter, 3);

	/* Free clone */
	memranges_teardown(&clone_memrange);
}

/*
 * This test verifies memranges_fill_holes_up_to() and memranges_create_hole(). Idea of the test
 * is to fill all holes, so that we end up with contiguous address space fully covered by
 * entries. Then, holes are created on the border of two different regions
 *
 * Example memory ranges (res_mock1) for test_memrange_holes.
 * Space marked with (/) is not covered by any region at the end of the test.
 *
 *     +--------CACHEABLE_TAG--------+ <-0xE000
 *     |                             |
 *     |                             |
 *   //|/////////////////////////////| <-0xFF000
 *   //+-----------HOLE_TAG----------+ <-0x100000
 *   //|/////////////////////////////| <-0x101000
 *     |                             |
 *     |                             |
 *     |                             |
 *     |                             |
 *     +--------RESERVED_TAG---------+ <-0x100000000
 *     |                             |
 *     +-----------------------------+ <-0x100001000
 */
static void test_memrange_holes(void **state)
{
	int counter = 0;
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	struct range_entry *ptr;
	struct range_entry *hole_ptr = NULL;
	struct memranges test_memrange;
	struct resource *res_mock = *state;
	int holes_found = 0;
	resource_t last_range_end = 0;
	const resource_t holes_fill_end = res_mock[RESERVED_TAG].base;

	memranges_init(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);

	/* Count holes in ranges */
	memranges_each_entry(ptr, &test_memrange)
	{
		if (!last_range_end) {
			last_range_end = range_entry_end(ptr);
			continue;
		}


		if (range_entry_base(ptr) != last_range_end) {
			holes_found++;
			last_range_end = range_entry_end(ptr);
		}

		if (range_entry_base(ptr) >= holes_fill_end)
			break;
	}

	/* Create range entries which covers continuous memory range
	   (but with different tags) */
	memranges_fill_holes_up_to(&test_memrange, holes_fill_end, HOLE_TAG);

	memranges_each_entry(ptr, &test_memrange)
	{
		if (range_entry_tag(ptr) == HOLE_TAG) {
			assert_int_equal(range_entry_base(ptr),
					 ALIGN_UP(res_mock[CACHEABLE_TAG].base
							  + res_mock[CACHEABLE_TAG].size,
						  MEMRANGE_ALIGN));
			assert_int_equal(range_entry_end(ptr), holes_fill_end);
			/* Store pointer to HOLE_TAG region for future use */
			hole_ptr = ptr;
		}
		counter++;
	}
	assert_int_equal(counter, 2 + holes_found);

	/* If test data does not have any holes in it then terminate this test */
	if (holes_found == 0)
		return;

	assert_non_null(hole_ptr);
	counter = 0;

	/* Create hole crossing the border of two range entries */
	const resource_t new_cacheable_end = ALIGN_DOWN(
		res_mock[CACHEABLE_TAG].base + res_mock[CACHEABLE_TAG].size - 4 * KiB,
		MEMRANGE_ALIGN);
	const resource_t new_hole_begin =
		ALIGN_UP(range_entry_base(hole_ptr) + 4 * KiB, MEMRANGE_ALIGN);
	const resource_t ranges_diff = new_hole_begin - new_cacheable_end;

	memranges_create_hole(&test_memrange, new_cacheable_end, ranges_diff);

	memranges_each_entry(ptr, &test_memrange)
	{
		switch (range_entry_tag(ptr)) {
		case CACHEABLE_TAG:
			assert_int_equal(range_entry_base(ptr), res_mock[CACHEABLE_TAG].base);
			assert_int_equal(range_entry_end(ptr), new_cacheable_end);
			break;
		case RESERVED_TAG:
			assert_int_equal(range_entry_base(ptr), res_mock[RESERVED_TAG].base);
			assert_int_equal(range_entry_end(ptr),
					 res_mock[RESERVED_TAG].base
						 + res_mock[RESERVED_TAG].size);
			break;
		case HOLE_TAG:
			assert_int_equal(range_entry_base(ptr), new_hole_begin);
			assert_int_equal(range_entry_end(ptr), res_mock[RESERVED_TAG].base);
			break;
		default:
			break;
		}
		counter++;
	}
	assert_int_equal(counter, 3);

	memranges_teardown(&test_memrange);
}

/*
 * This test verifies memranges_steal() function. Simple check is done by attempt so steal some
 * memory from region with READONLY_TAG.
 *
 * Example memory ranges (res_mock1) for test_memrange_steal.
 * Space marked with (/) is stolen during the test.
 *
 *     +--------CACHEABLE_TAG--------+ <-0xE000
 *     |                             |
 *     |                             |
 *     |                             |
 *     +-----------------------------+ <-0x100000
 *
 *
 *
 *     +--------READONLY_TAG---------+ <-0xFF0000  <-stolen_base
 *     |/////////////////////////////|             <-stolen_base + 0x4000
 *     |                             |
 *     |                             |
 *     +-----------------------------+ <-0x1000000
 *
 *
 *     +--------RESERVED_TAG---------+ <-0x100000000
 *     |                             |
 *     +-----------------------------+ <-0x100001000
 */
static void test_memrange_steal(void **state)
{
	bool status = false;
	resource_t stolen;
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	const unsigned long readonly = IORESOURCE_READONLY;
	const resource_t stolen_range_size = 0x4000;
	struct memranges test_memrange;
	struct resource *res_mock = *state;
	struct range_entry *ptr;
	size_t count = 0;

	memranges_init(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources(&test_memrange, readonly, readonly, READONLY_TAG);

	status = memranges_steal(&test_memrange,
				 res_mock[RESERVED_TAG].base + res_mock[RESERVED_TAG].size,
				 stolen_range_size, 12, READONLY_TAG, &stolen);
	assert_true(status);
	assert_in_range(stolen, res_mock[READONLY_TAG].base,
			res_mock[READONLY_TAG].base + res_mock[READONLY_TAG].size);

	memranges_each_entry(ptr, &test_memrange)
	{
		if (range_entry_tag(ptr) == READONLY_TAG) {
			assert_int_equal(range_entry_base(ptr),
					 ALIGN_DOWN(res_mock[READONLY_TAG].base, MEMRANGE_ALIGN)
						 + stolen_range_size);
		}
		count++;
	}
	assert_int_equal(count, 3);
	count = 0;

	/* Check if inserting range in previously stolen area will merge it. */
	memranges_insert(&test_memrange, res_mock[READONLY_TAG].base + 0xCC, stolen_range_size,
			 READONLY_TAG);
	memranges_each_entry(ptr, &test_memrange)
	{
		if (range_entry_tag(ptr) == READONLY_TAG) {
			assert_int_equal(
				range_entry_base(ptr),
				ALIGN_DOWN(res_mock[READONLY_TAG].base, MEMRANGE_ALIGN));
			assert_int_equal(
				range_entry_end(ptr),
				ALIGN_UP(range_entry_base(ptr) + res_mock[READONLY_TAG].size,
					 MEMRANGE_ALIGN));
		}
		count++;
	}
	assert_int_equal(count, 3);
	count = 0;

	memranges_teardown(&test_memrange);
}

/* Utility function checking number of entries and alignment of their base and end pointers */
static void check_range_entries_count_and_alignment(struct memranges *ranges,
						    size_t ranges_count, resource_t alignment)
{
	size_t count = 0;
	struct range_entry *ptr;

	memranges_each_entry(ptr, ranges)
	{
		assert_true(IS_ALIGNED(range_entry_base(ptr), alignment));
		assert_true(IS_ALIGNED(range_entry_end(ptr), alignment));

		count++;
	}
	assert_int_equal(ranges_count, count);
}

/* This test verifies memranges_init*() and memranges_teardown() functions.
   Added ranges are checked correct count and alignment. */
static void test_memrange_init_and_teardown(void **state)
{
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	const unsigned long readonly = IORESOURCE_READONLY;
	struct memranges test_memrange;
	struct range_entry range_entries[4] = {0};

	/* Test memranges_init() correctness */
	memranges_init(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources(&test_memrange, readonly, readonly, READONLY_TAG);

	/* Expect all entries to be aligned to 4KiB (2^12) */
	check_range_entries_count_and_alignment(&test_memrange, 3, MEMRANGE_ALIGN);

	/* Expect ranges list to be empty after teardown */
	memranges_teardown(&test_memrange);
	assert_true(memranges_is_empty(&test_memrange));


	/* Test memranges_init_with_alignment() correctness with alignment of 1KiB (2^10) */
	memranges_init_with_alignment(&test_memrange, cacheable, cacheable, CACHEABLE_TAG, 10);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources(&test_memrange, readonly, readonly, READONLY_TAG);

	check_range_entries_count_and_alignment(&test_memrange, 3, POWER_OF_2(10));

	memranges_teardown(&test_memrange);
	assert_true(memranges_is_empty(&test_memrange));


	/* Test memranges_init_empty() correctness */
	memranges_init_empty(&test_memrange, &range_entries[0], ARRAY_SIZE(range_entries));
	assert_true(memranges_is_empty(&test_memrange));

	memranges_add_resources(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources(&test_memrange, readonly, readonly, READONLY_TAG);

	check_range_entries_count_and_alignment(&test_memrange, 3, MEMRANGE_ALIGN);

	memranges_teardown(&test_memrange);
	assert_true(memranges_is_empty(&test_memrange));


	/* Test memranges_init_with_alignment() correctness with alignment of 8KiB (2^13) */
	memranges_init_empty_with_alignment(&test_memrange, &range_entries[0],
					    ARRAY_SIZE(range_entries), 13);
	assert_true(memranges_is_empty(&test_memrange));

	memranges_add_resources(&test_memrange, cacheable, cacheable, CACHEABLE_TAG);
	memranges_add_resources(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources(&test_memrange, readonly, readonly, READONLY_TAG);

	check_range_entries_count_and_alignment(&test_memrange, 3, POWER_OF_2(13));

	memranges_teardown(&test_memrange);
	assert_true(memranges_is_empty(&test_memrange));
}

/* Filter function accepting ranges having memory resource flag */
static int memrange_filter_mem_only(struct device *dev, struct resource *res)
{
	/* Accept only memory resources */
	return res->flags & IORESOURCE_MEM;
}

/* Filter function rejecting ranges having memory resource flag */
static int memrange_filter_non_mem(struct device *dev, struct resource *res)
{
	/* Accept only memory resources */
	return !(res->flags & IORESOURCE_MEM);
}

/* This test verifies memranges_add_resources_filter() function by providing filter functions
   which accept or reject ranges. */
static void test_memrange_add_resources_filter(void **state)
{
	const unsigned long cacheable = IORESOURCE_CACHEABLE;
	const unsigned long reserved = IORESOURCE_RESERVE;
	struct memranges test_memrange;
	struct range_entry *ptr;
	size_t count = 0;
	size_t accepted_tags[] = {CACHEABLE_TAG, RESERVED_TAG};

	/* Check if filter accepts range correctly */
	memranges_init(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources_filter(&test_memrange, cacheable, cacheable, CACHEABLE_TAG,
				       memrange_filter_mem_only);

	/* Check if filter accepted desired range. */
	memranges_each_entry(ptr, &test_memrange)
	{
		assert_in_set(range_entry_tag(ptr), accepted_tags, ARRAY_SIZE(accepted_tags));
		assert_true(IS_ALIGNED(range_entry_base(ptr), MEMRANGE_ALIGN));
		assert_true(IS_ALIGNED(range_entry_end(ptr), MEMRANGE_ALIGN));
		count++;
	}
	assert_int_equal(2, count);
	count = 0;
	memranges_teardown(&test_memrange);

	/* Check if filter rejects range correctly */
	memranges_init(&test_memrange, reserved, reserved, RESERVED_TAG);
	memranges_add_resources_filter(&test_memrange, cacheable, cacheable, CACHEABLE_TAG,
				       memrange_filter_non_mem);

	check_range_entries_count_and_alignment(&test_memrange, 1, MEMRANGE_ALIGN);

	memranges_teardown(&test_memrange);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_memrange_basic),
		cmocka_unit_test(test_memrange_clone_insert),
		cmocka_unit_test(test_memrange_holes),
		cmocka_unit_test(test_memrange_steal),
		cmocka_unit_test(test_memrange_init_and_teardown),
		cmocka_unit_test(test_memrange_add_resources_filter),
	};

	return cmocka_run_group_tests_name(__TEST_NAME__ "(Boundary on 4GiB)", tests,
					   setup_test_1, NULL)
	       + cmocka_run_group_tests_name(__TEST_NAME__ "(Boundaries 1 byte from 4GiB)",
					     tests, setup_test_2, NULL)
	       + cmocka_run_group_tests_name(__TEST_NAME__ "(Range over 4GiB boundary)", tests,
					     setup_test_3, NULL);
}
