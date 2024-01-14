/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/region.h>
#include <string.h>
#include <tests/test.h>

/* We'd like to test overflow conditions, but for tests size_t is dependent on the HOSTCC
   architecture. We use this to normalize the available address space to [VAL(0x0):VAL(0xf)). */
#define VAL(v) ((size_t)(v##ULL << (sizeof(size_t) * 8 - 4)))

static void test_region(void **state)
{
	/* Self-test: make sure VAL() overflow works as intended. */
	assert_true(VAL(5) + VAL(10) > VAL(10));
	assert_true(VAL(7) + VAL(10) < VAL(10));

	struct region outer = {.offset = VAL(2), .size = VAL(4)};
	assert_int_equal(region_offset(&outer), VAL(2));
	assert_int_equal(region_sz(&outer), VAL(4));
	assert_int_equal(region_last(&outer), VAL(6) - 1);

	struct region inner = {.offset = VAL(3), .size = VAL(2)};
	assert_true(region_is_subregion(&outer, &inner));

	struct region touching_bottom = {.offset = VAL(2), .size = VAL(1)};
	assert_true(region_is_subregion(&outer, &touching_bottom));

	struct region touching_top = {.offset = VAL(5), .size = VAL(1)};
	assert_true(region_is_subregion(&outer, &touching_top));

	struct region overlap_bottom = {.offset = VAL(1), .size = VAL(2)};
	assert_false(region_is_subregion(&outer, &overlap_bottom));

	struct region overlap_top = {.offset = VAL(5), .size = VAL(2)};
	assert_false(region_is_subregion(&outer, &overlap_top));

	struct region below = {.offset = 0, .size = VAL(1)};
	assert_false(region_is_subregion(&outer, &below));

	struct region above = {.offset = VAL(0xf), .size = VAL(1)};
	assert_false(region_is_subregion(&outer, &above));
}

static void *mock_mmap(const struct region_device *rdev, size_t offset, size_t size)
{
	check_expected_ptr(rdev);
	check_expected(offset);
	check_expected(size);

	return mock_ptr_type(void *);
}

static int mock_unmap(const struct region_device *rdev, void *mapping)
{
	check_expected_ptr(rdev);
	check_expected_ptr(mapping);

	return mock();
}

static ssize_t mock_readat(const struct region_device *rdev, void *buffer, size_t offset,
			   size_t size)
{
	check_expected_ptr(rdev);
	check_expected_ptr(buffer);
	check_expected(offset);
	check_expected(size);

	ssize_t ret = mock();
	if (!ret)
		return size;
	else
		return ret;
}

static ssize_t mock_writeat(const struct region_device *rdev, const void *buffer, size_t offset,
			    size_t size)
{
	check_expected_ptr(rdev);
	check_expected_ptr(buffer);
	check_expected(offset);
	check_expected(size);

	ssize_t ret = mock();
	if (!ret)
		return size;
	else
		return ret;
}

static ssize_t mock_eraseat(const struct region_device *rdev, size_t offset, size_t size)
{
	check_expected_ptr(rdev);
	check_expected(offset);
	check_expected(size);

	ssize_t ret = mock();
	if (!ret)
		return size;
	else
		return ret;
}

struct region_device_ops mock_rdev_ops = {
	.mmap = mock_mmap,
	.munmap = mock_unmap,
	.readat = mock_readat,
	.writeat = mock_writeat,
	.eraseat = mock_eraseat,
};

struct region_device mock_rdev = REGION_DEV_INIT(&mock_rdev_ops, 0, ~(size_t)0);
void *mmap_result = (void *)0x12345678;
const size_t mock_size = 256;
u8 mock_buffer[256];

static void test_rdev_basics(void **state)
{
	assert_int_equal(region_device_offset(&mock_rdev), 0);
	assert_int_equal(region_device_sz(&mock_rdev), ~(size_t)0);
	assert_int_equal(region_device_last(&mock_rdev), ~(size_t)0 - 1);
}

/*
 * This function sets up defaults for the mock_rdev_ops functions so we don't have to explicitly
 * mock every parameter every time. cmocka doesn't really work well for this sort of use case
 * and won't let you override these anymore once they're set (because these are stored as
 * queues, not stacks, and once you store an "infinite" element the test can never proceed
 * behind it), so tests will always have to enqueue any custom values they may need for the rest
 * of the test function before calling this.
 */
static void rdev_mock_defaults(void)
{
	will_return_maybe(mock_mmap, mmap_result);
	will_return_maybe(mock_unmap, 0);
	will_return_maybe(mock_readat, 0);
	will_return_maybe(mock_writeat, 0);
	will_return_maybe(mock_eraseat, 0);

	expect_value_count(mock_mmap, rdev, &mock_rdev, -2);
	expect_value_count(mock_unmap, rdev, &mock_rdev, -2);
	expect_value_count(mock_readat, rdev, &mock_rdev, -2);
	expect_value_count(mock_writeat, rdev, &mock_rdev, -2);
	expect_value_count(mock_eraseat, rdev, &mock_rdev, -2);

	expect_value_count(mock_readat, buffer, &mock_buffer, -2);
	expect_value_count(mock_writeat, buffer, &mock_buffer, -2);

	expect_value_count(mock_mmap, offset, 0, -2);
	expect_value_count(mock_readat, offset, 0, -2);
	expect_value_count(mock_writeat, offset, 0, -2);
	expect_value_count(mock_eraseat, offset, 0, -2);

	expect_value_count(mock_mmap, size, mock_size, -2);
	expect_value_count(mock_readat, size, mock_size, -2);
	expect_value_count(mock_writeat, size, mock_size, -2);
	expect_value_count(mock_eraseat, size, mock_size, -2);

	expect_value_count(mock_unmap, mapping, mmap_result, -2);
}

static void test_rdev_success(void **state)
{
	expect_value(mock_mmap, size, region_device_sz(&mock_rdev));

	rdev_mock_defaults();

	assert_ptr_equal(rdev_mmap_full(&mock_rdev), mmap_result);

	assert_ptr_equal(rdev_mmap(&mock_rdev, 0, mock_size), mmap_result);
	assert_int_equal(rdev_munmap(&mock_rdev, mmap_result), 0);
	assert_int_equal(rdev_readat(&mock_rdev, mock_buffer, 0, mock_size), mock_size);
	assert_int_equal(rdev_writeat(&mock_rdev, mock_buffer, 0, mock_size), mock_size);
	assert_int_equal(rdev_eraseat(&mock_rdev, 0, mock_size), mock_size);
}

static void test_rdev_failure(void **state)
{
	will_return(mock_mmap, NULL);
	will_return(mock_unmap, -1);
	will_return(mock_readat, -1);
	will_return(mock_writeat, -1);
	will_return(mock_eraseat, -1);

	rdev_mock_defaults();

	assert_null(rdev_mmap(&mock_rdev, 0, mock_size));
	assert_int_equal(rdev_munmap(&mock_rdev, mmap_result), -1);
	assert_int_equal(rdev_readat(&mock_rdev, mock_buffer, 0, mock_size), -1);
	assert_int_equal(rdev_writeat(&mock_rdev, mock_buffer, 0, mock_size), -1);
	assert_int_equal(rdev_eraseat(&mock_rdev, 0, mock_size), -1);
}

static void test_rdev_wrap(void **state)
{
	struct region_device child;
	const size_t offs = VAL(0xf);
	const size_t wrap_size = VAL(2);
	/* Known API limitation -- can't exactly touch address space limit from below. */
	const size_t fit_size = VAL(1) - 1;

	/* For the 'wrap' cases, the underlying rdev_ops aren't even called, so only add
	   expectations for the 'fit' cases. */
	expect_value(mock_mmap, offset, offs);
	expect_value(mock_readat, offset, offs);
	expect_value(mock_writeat, offset, offs);
	expect_value(mock_eraseat, offset, offs);

	expect_value(mock_mmap, size, fit_size);
	expect_value(mock_readat, size, fit_size);
	expect_value(mock_writeat, size, fit_size);
	expect_value(mock_eraseat, size, fit_size);

	rdev_mock_defaults();

	/* Accesses to regions that wrap around the end of the address space should fail. */
	assert_null(rdev_mmap(&mock_rdev, offs, wrap_size));
	assert_int_equal(rdev_readat(&mock_rdev, mock_buffer, offs, wrap_size), -1);
	assert_int_equal(rdev_writeat(&mock_rdev, mock_buffer, offs, wrap_size), -1);
	assert_int_equal(rdev_eraseat(&mock_rdev, offs, wrap_size), -1);
	assert_int_equal(rdev_chain(&child, &mock_rdev, offs, wrap_size), -1);

	/* Just barely touching the end of the address space (and the rdev) should be fine. */
	assert_ptr_equal(rdev_mmap(&mock_rdev, offs, fit_size), mmap_result);
	assert_int_equal(rdev_readat(&mock_rdev, mock_buffer, offs, fit_size), fit_size);
	assert_int_equal(rdev_writeat(&mock_rdev, mock_buffer, offs, fit_size), fit_size);
	assert_int_equal(rdev_eraseat(&mock_rdev, offs, fit_size), fit_size);
	assert_int_equal(rdev_chain(&child, &mock_rdev, offs, fit_size), 0);
}

static void test_rdev_chain(void **state)
{
	struct region_device child;
	const size_t child_offs = VAL(2);
	const size_t child_size = VAL(4);
	const size_t offs = VAL(1);
	const size_t ovrflw_size = child_size - offs + 1;

	/* The mock_size test is the only one that will go through to underlying rdev_ops. */
	expect_value(mock_mmap, offset, child_offs + offs);
	expect_value(mock_readat, offset, child_offs + offs);
	expect_value(mock_writeat, offset, child_offs + offs);
	expect_value(mock_eraseat, offset, child_offs + offs);

	rdev_mock_defaults();

	/* First a quick test for rdev_chain_full(). */
	assert_int_equal(rdev_chain_full(&child, &mock_rdev), 0);
	assert_int_equal(region_device_sz(&child), region_device_sz(&mock_rdev));
	assert_int_equal(region_device_offset(&child), region_device_offset(&mock_rdev));
	assert_int_equal(rdev_relative_offset(&mock_rdev, &child), 0);

	/* Remaining tests use rdev chained to [child_offs:child_size) subregion. */
	assert_int_equal(rdev_chain(&child, &mock_rdev, child_offs, child_size), 0);
	assert_int_equal(region_device_sz(&child), child_size);
	assert_int_equal(region_device_offset(&child), child_offs);
	assert_int_equal(region_device_last(&child), child_offs + child_size - 1);
	assert_int_equal(rdev_relative_offset(&mock_rdev, &child), child_offs);
	assert_int_equal(rdev_relative_offset(&child, &mock_rdev), -1);

	/* offs + mock_size < child_size, so will succeed. */
	assert_ptr_equal(rdev_mmap(&child, offs, mock_size), mmap_result);
	assert_int_equal(rdev_munmap(&child, mmap_result), 0);
	assert_int_equal(rdev_readat(&child, mock_buffer, offs, mock_size), mock_size);
	assert_int_equal(rdev_writeat(&child, mock_buffer, offs, mock_size), mock_size);
	assert_int_equal(rdev_eraseat(&child, offs, mock_size), mock_size);

	/* offs + ovrflw_size > child_size, so will fail. */
	assert_null(rdev_mmap(&child, offs, ovrflw_size));
	assert_int_equal(rdev_readat(&child, mock_buffer, offs, ovrflw_size), -1);
	assert_int_equal(rdev_writeat(&child, mock_buffer, offs, ovrflw_size), -1);
	assert_int_equal(rdev_eraseat(&child, offs, ovrflw_size), -1);

	/* Using child_size as offset, the start of the area will already be out of range. */
	assert_null(rdev_mmap(&child, child_size, mock_size));
	assert_int_equal(rdev_readat(&child, mock_buffer, child_size, mock_size), -1);
	assert_int_equal(rdev_writeat(&child, mock_buffer, child_size, mock_size), -1);
	assert_int_equal(rdev_eraseat(&child, child_size, mock_size), -1);
}

static void test_rdev_double_chain(void **state)
{
	struct region_device first, second;
	const size_t first_offs = VAL(2);
	const size_t first_size = VAL(6);
	const size_t second_offs = VAL(2);
	const size_t second_size = VAL(2);
	const size_t offs = VAL(1);
	const size_t ovrflw_size = second_size - offs + 1;

	/* The mock_size test is the only one that will go through to underlying rdev_ops. */
	expect_value(mock_mmap, offset, first_offs + second_offs + offs);
	expect_value(mock_readat, offset, first_offs + second_offs + offs);
	expect_value(mock_writeat, offset, first_offs + second_offs + offs);
	expect_value(mock_eraseat, offset, first_offs + second_offs + offs);

	rdev_mock_defaults();

	/* First, chain an rdev to root over [first_offs:first_size). */
	assert_int_equal(rdev_chain(&first, &mock_rdev, first_offs, first_size), 0);

	/* Trying to chain a second to first beyond its end should fail. */
	assert_int_equal(rdev_chain(&second, &first, second_offs, first_size), -1);

	/* Chain second to first at [second_offs:second_size). */
	assert_int_equal(rdev_chain(&second, &first, second_offs, second_size), 0);
	assert_int_equal(rdev_relative_offset(&first, &second), second_offs);
	assert_int_equal(rdev_relative_offset(&mock_rdev, &second), first_offs + second_offs);

	/* offs + mock_size < second_size, so will succeed. */
	assert_ptr_equal(rdev_mmap(&second, offs, mock_size), mmap_result);
	assert_int_equal(rdev_munmap(&second, mmap_result), 0);
	assert_int_equal(rdev_readat(&second, mock_buffer, offs, mock_size), mock_size);
	assert_int_equal(rdev_writeat(&second, mock_buffer, offs, mock_size), mock_size);
	assert_int_equal(rdev_eraseat(&second, offs, mock_size), mock_size);

	/* offs + ovrflw_size > second_size, so will fail. */
	assert_null(rdev_mmap(&second, offs, ovrflw_size));
	assert_int_equal(rdev_readat(&second, mock_buffer, offs, ovrflw_size), -1);
	assert_int_equal(rdev_writeat(&second, mock_buffer, offs, ovrflw_size), -1);
	assert_int_equal(rdev_eraseat(&second, offs, ovrflw_size), -1);

	/* offs + second_size + offs way out of range. */
	assert_null(rdev_mmap(&second, second_size + offs, mock_size));
	assert_int_equal(rdev_readat(&second, mock_buffer, second_size + offs, mock_size), -1);
	assert_int_equal(rdev_writeat(&second, mock_buffer, second_size + offs, mock_size), -1);
	assert_int_equal(rdev_eraseat(&second, second_size + offs, mock_size), -1);
}

static void test_mem_rdev(void **state)
{
	const size_t size = 256;
	u8 backing[size];
	u8 scratch[size];
	int i;
	struct region_device mem;
	rdev_chain_mem_rw(&mem, backing, size);

	/* Test writing to and reading from full mapping. */
	memset(backing, 0xa5, size);
	u8 *mapping = rdev_mmap_full(&mem);
	assert_non_null(mapping);
	for (i = 0; i < size; i++)
		assert_int_equal(mapping[i], 0xa5);
	memset(mapping, 0x5a, size);
	for (i = 0; i < size; i++)
		assert_int_equal(backing[i], 0x5a);
	assert_int_equal(rdev_munmap(&mem, mapping), 0);

	/* Test read/write/erase of single bytes. */
	for (i = 0; i < size; i++) {
		u8 val = i + 0xaa;
		scratch[0] = val;
		assert_int_equal(rdev_writeat(&mem, &scratch, i, 1), 1);
		assert_int_equal(backing[i], val);
		assert_int_equal(scratch[0], val);
		val = i + 0x55;
		backing[i] = val;
		assert_int_equal(rdev_readat(&mem, &scratch, i, 1), 1);
		assert_int_equal(scratch[0], val);
		assert_int_equal(backing[i], val);
		assert_int_equal(rdev_eraseat(&mem, i, 1), 1);
		assert_int_equal(backing[i], 0);
	}

	/* Test read/write/erase of larger chunk. */
	size_t offs = 0x47;
	size_t chunk = 0x72;
	memset(backing, 0, size);
	memset(scratch, 0, size);
	memset(scratch + offs, 0x39, chunk);
	assert_int_equal(rdev_writeat(&mem, scratch + offs, offs, chunk), chunk);
	assert_memory_equal(backing, scratch, size);
	memset(backing, 0, size);
	assert_int_equal(rdev_readat(&mem, scratch + offs, offs, chunk), chunk);
	assert_memory_equal(backing, scratch, size);
	memset(scratch + offs + 1, 0, chunk - 1);
	assert_int_equal(rdev_eraseat(&mem, offs + 1, chunk - 1), chunk - 1);
	assert_memory_equal(backing, scratch, size);

	/* Test mapping of larger chunk. */
	memset(backing, 0, size);
	mapping = rdev_mmap(&mem, offs, chunk);
	assert_non_null(mapping);
	memset(scratch, 0x93, size);
	memcpy(mapping, scratch, chunk);
	memset(scratch, 0, size);
	memset(scratch + offs, 0x93, chunk);
	assert_memory_equal(backing, scratch, size);
	assert_int_equal(rdev_munmap(&mem, mapping), 0);
	assert_memory_equal(backing, scratch, size);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_region),
		cmocka_unit_test(test_rdev_basics),
		cmocka_unit_test(test_rdev_success),
		cmocka_unit_test(test_rdev_failure),
		cmocka_unit_test(test_rdev_wrap),
		cmocka_unit_test(test_rdev_chain),
		cmocka_unit_test(test_rdev_double_chain),
		cmocka_unit_test(test_mem_rdev),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
