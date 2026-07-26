/* SPDX-License-Identifier: GPL-2.0-only */

#include <crc_byte.h>
#include <device/dram/ddr3.h>
#include <device/dram/ddr4.h>
#include <device/dram/ddr5.h>
#include <spd.h>
#include <spd_bin.h>
#include <spd_cache.h>
#include <stdlib.h>
#include <string.h>
#include <tests/test.h>
#include <tests/lib/spd_cache_data.h>

/*
 * Select the DRAM-generation-specific test data and the serial number offset
 * that get_spd_sn() (and the code under test) uses for the generation being
 * tested. check_if_dimm_changed() is only exercised for DDR4 and DDR5.
 */
#if __TEST_SPD_CACHE_DDR == 4
#define TEST_SPD_SN_OFF		DDR4_SPD_SN_OFF
#define TEST_SPD_LEN		SPD_SIZE_MAX_DDR4
#define fill_spd_cache_dram	fill_spd_cache_ddr4
#define spd_data_dram_1		spd_data_ddr4_1
#define spd_data_dram_1_sz	spd_data_ddr4_1_sz
#define spd_data_dram_2		spd_data_ddr4_2
#define spd_data_dram_2_sz	spd_data_ddr4_2_sz
#elif __TEST_SPD_CACHE_DDR == 5
#define TEST_SPD_SN_OFF		DDR5_SPD_SN_OFF
#define TEST_SPD_LEN		CONFIG_DIMM_SPD_SIZE
#define fill_spd_cache_dram	fill_spd_cache_ddr5
#define spd_data_dram_1		spd_data_ddr5_1
#define spd_data_dram_1_sz	spd_data_ddr5_1_sz
#define spd_data_dram_2		spd_data_ddr5_2
#define spd_data_dram_2_sz	spd_data_ddr5_2_sz
#else
/*
 * The check_if_dimm_changed() tests below are not run for DDR3, but they are
 * still compiled. Provide DDR4 fallbacks so the file builds for DDR3. Only
 * TEST_SPD_LEN is actually used by a test that runs for DDR3.
 */
#define TEST_SPD_SN_OFF		DDR4_SPD_SN_OFF
#define TEST_SPD_LEN		SPD_SIZE_MAX_DDR3
#define fill_spd_cache_dram	fill_spd_cache_ddr4
#define spd_data_dram_1		spd_data_ddr4_1
#define spd_data_dram_1_sz	spd_data_ddr4_1_sz
#define spd_data_dram_2		spd_data_ddr4_2
#define spd_data_dram_2_sz	spd_data_ddr4_2_sz
#endif

struct region_device flash_rdev_rw;
static char *flash_buffer = NULL;
static size_t flash_buffer_size = 0;

static int setup_spd_cache(void **state)
{
	flash_buffer_size = SC_SPD_TOTAL_LEN + SC_CRC_LEN;
	flash_buffer = malloc(flash_buffer_size);

	if (flash_buffer == NULL) {
		flash_buffer_size = 0;
		return -1;
	}

	rdev_chain_mem_rw(&flash_rdev_rw, flash_buffer, flash_buffer_size);
	return 0;
}

static int setup_spd_cache_test(void **state)
{
	memset(flash_buffer, 0xff, flash_buffer_size);
	return 0;
}

static int teardown_spd_cache(void **state)
{
	rdev_chain_mem_rw(&flash_rdev_rw, NULL, 0);
	free(flash_buffer);
	flash_buffer = NULL;
	flash_buffer_size = 0;
	return 0;
}


int fmap_locate_area_as_rdev(const char *name, struct region_device *area)
{
	return rdev_chain(area, &flash_rdev_rw, 0, flash_buffer_size);
}

/* This test verifies if load_spd_cache() correctly loads spd_cache pointer and size
   from provided region_device. Memory region device is returned by our
   fmap_locate_area_as_rdev() override. */
static void test_load_spd_cache(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	assert_ptr_equal(flash_buffer, spd_cache);
	assert_int_equal(SC_SPD_TOTAL_LEN + SC_CRC_LEN, spd_cache_sz);
}

static void calc_spd_cache_crc(uint8_t *spd_cache)
{
	*(uint16_t *)(spd_cache + SC_CRC_OFFSET) = CRC(spd_cache, SC_SPD_TOTAL_LEN, crc16_byte);
}

__attribute__((unused)) static void fill_spd_cache_ddr3(uint8_t *spd_cache, size_t spd_cache_sz)
{
	assert_true(spd_cache_sz >= (spd_data_ddr3_1_sz + sizeof(uint16_t)));

	memcpy(spd_cache, spd_data_ddr3_1, spd_data_ddr3_1_sz);
	/* update_spd_cache() pads the slots of absent DIMMs with 0xff. */
	memset(spd_cache + spd_data_ddr3_1_sz, 0xff, spd_cache_sz - spd_data_ddr3_1_sz);
	calc_spd_cache_crc(spd_cache);
}

__attribute__((unused)) static void fill_spd_cache_ddr4(uint8_t *spd_cache, size_t spd_cache_sz)
{
	assert_true(spd_cache_sz
		    >= (spd_data_ddr4_1_sz + spd_data_ddr4_2_sz + sizeof(uint16_t)));

	memcpy(spd_cache, spd_data_ddr4_1, spd_data_ddr4_1_sz);
	memcpy(spd_cache + spd_data_ddr4_1_sz, spd_data_ddr4_2, spd_data_ddr4_2_sz);
	/* update_spd_cache() pads the slots of absent DIMMs with 0xff. */
	memset(spd_cache + spd_data_ddr4_1_sz + spd_data_ddr4_2_sz, 0xff,
	       spd_cache_sz - (spd_data_ddr4_1_sz + spd_data_ddr4_2_sz));
	calc_spd_cache_crc(spd_cache);
}

__attribute__((unused)) static void fill_spd_cache_ddr5(uint8_t *spd_cache, size_t spd_cache_sz)
{
	assert_true(spd_cache_sz
		    >= (spd_data_ddr5_1_sz + spd_data_ddr5_2_sz + sizeof(uint16_t)));

	memcpy(spd_cache, spd_data_ddr5_1, spd_data_ddr5_1_sz);
	memcpy(spd_cache + spd_data_ddr5_1_sz, spd_data_ddr5_2, spd_data_ddr5_2_sz);
	/* update_spd_cache() pads the slots of absent DIMMs with 0xff. */
	memset(spd_cache + spd_data_ddr5_1_sz + spd_data_ddr5_2_sz, 0xff,
	       spd_cache_sz - (spd_data_ddr5_1_sz + spd_data_ddr5_2_sz));
	calc_spd_cache_crc(spd_cache);
}

/* Fill the cache with SPD data of the DRAM generation being tested. */
static void fill_spd_cache(uint8_t *spd_cache, size_t spd_cache_sz)
{
#if __TEST_SPD_CACHE_DDR == 3
	fill_spd_cache_ddr3(spd_cache, spd_cache_sz);
#elif __TEST_SPD_CACHE_DDR == 4
	fill_spd_cache_ddr4(spd_cache, spd_cache_sz);
#elif __TEST_SPD_CACHE_DDR == 5
	fill_spd_cache_ddr5(spd_cache, spd_cache_sz);
#endif
}

static void test_spd_fill_from_cache(void **state)
{
	struct spd_block blk;
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));

	/* Empty spd cache */
	assert_int_equal(CB_ERR, spd_fill_from_cache(spd_cache, &blk));

	fill_spd_cache(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));
	assert_int_equal(TEST_SPD_LEN, blk.len);
}

/*
 * For a DRAM type we don't know the SPD length of, fall back to the size of a
 * cache slot, which is what get_spd_smbus() would have reported.
 */
static void test_spd_fill_from_cache_unsupported_dram_type(void **state)
{
	struct spd_block blk;
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));

	fill_spd_cache(spd_cache, spd_cache_sz);
	/* Some DRAM generation this code doesn't know about yet. */
	spd_cache[SC_SPD_OFFSET(0) + SPD_MEMORY_TYPE] = 0xfe;

	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));
	assert_int_equal(CONFIG_DIMM_SPD_SIZE, blk.len);
}


static void test_spd_cache_is_valid(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));

	/* Empty, incorrect SPD */
	assert_false(spd_cache_is_valid(spd_cache, spd_cache_sz));

	fill_spd_cache(spd_cache, spd_cache_sz);
	assert_true(spd_cache_is_valid(spd_cache, spd_cache_sz));
}


/* Used for setting `sn` parameter value */
static u32 get_spd_sn_ret_sn[SC_SPD_NUMS] = {0};
static size_t get_spd_sn_ret_sn_idx = 0;
/* Implementation for testing purposes.  */
enum cb_err get_spd_sn(u8 addr, u32 *sn)
{
	*sn = get_spd_sn_ret_sn[get_spd_sn_ret_sn_idx];
	get_spd_sn_ret_sn_idx = (get_spd_sn_ret_sn_idx + 1) % ARRAY_SIZE(get_spd_sn_ret_sn);

	return mock_type(enum cb_err);
}

static void get_sn_from_spd_cache(uint8_t *spd_cache, u32 arr[])
{
	for (int i = 0; i < SC_SPD_NUMS; ++i)
		arr[i] = *(u32 *)(spd_cache + SC_SPD_OFFSET(i) + TEST_SPD_SN_OFF);
}

/*
 * check_if_dimm_changed() is only used with DDR4 and DDR5, so these tests are
 * not built for DDR3.
 */
__attribute__((unused)) static void test_check_if_dimm_changed_not_changed(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0x51, 0x52, 0x53},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	fill_spd_cache_dram(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));

	get_sn_from_spd_cache(spd_cache, get_spd_sn_ret_sn);
	get_spd_sn_ret_sn_idx = 0;
	will_return_count(get_spd_sn, CB_SUCCESS, SC_SPD_NUMS);
	assert_false(check_if_dimm_changed(spd_cache, &blk));
}

__attribute__((unused)) static void test_check_if_dimm_changed_sn_error(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0x51, 0x52, 0x53},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	fill_spd_cache_dram(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));

	/* Simulate error */
	will_return_count(get_spd_sn, CB_ERR, 1);
	assert_true(check_if_dimm_changed(spd_cache, &blk));
}

__attribute__((unused)) static void test_check_if_dimm_changed_sodimm_lost(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0x51, 0x52, 0x53},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	fill_spd_cache_dram(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));
	get_sn_from_spd_cache(spd_cache, get_spd_sn_ret_sn);
	memset(spd_cache + spd_data_dram_1_sz, 0xff, spd_data_dram_2_sz);

	get_spd_sn_ret_sn_idx = 0;
	will_return_always(get_spd_sn, CB_SUCCESS);
	assert_true(check_if_dimm_changed(spd_cache, &blk));
}

__attribute__((unused)) static void test_check_if_dimm_changed_new_sodimm(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0x51, 0x52, 0x53},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	fill_spd_cache_dram(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));
	get_sn_from_spd_cache(spd_cache, get_spd_sn_ret_sn);
	/* A DIMM shows up in a slot that the cache records as empty. */
	get_spd_sn_ret_sn[2] = 0x43211234;

	get_spd_sn_ret_sn_idx = 0;
	will_return_always(get_spd_sn, CB_SUCCESS);
	assert_true(check_if_dimm_changed(spd_cache, &blk));
}

/*
 * A cache entry with a DRAM type whose serial number offset we don't know must
 * be treated as changed, instead of comparing against a guessed offset.
 */
__attribute__((unused))
static void test_check_if_dimm_changed_unsupported_dram_type(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0x51, 0x52, 0x53},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	fill_spd_cache_dram(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));

	get_sn_from_spd_cache(spd_cache, get_spd_sn_ret_sn);
	/* Some DRAM generation this code doesn't know about yet. */
	spd_cache[SC_SPD_OFFSET(0) + SPD_MEMORY_TYPE] = 0xfe;

	get_spd_sn_ret_sn_idx = 0;
	will_return_always(get_spd_sn, CB_SUCCESS);
	assert_true(check_if_dimm_changed(spd_cache, &blk));
}

__attribute__((unused)) static void test_check_if_dimm_changed_sn_changed(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0x51, 0x52, 0x53},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	fill_spd_cache_dram(spd_cache, spd_cache_sz);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));
	get_sn_from_spd_cache(spd_cache, get_spd_sn_ret_sn);
	*(u32 *)(spd_cache + SC_SPD_OFFSET(0) + TEST_SPD_SN_OFF) = 0x43211234;

	get_spd_sn_ret_sn_idx = 0;
	will_return_always(get_spd_sn, CB_SUCCESS);
	assert_true(check_if_dimm_changed(spd_cache, &blk));
}

__attribute__((unused)) static void test_check_if_dimm_changed_with_nonexistent(void **state)
{
	uint8_t *spd_cache;
	size_t spd_cache_sz;
	struct spd_block blk = {.addr_map = {0x50, 0, 0, 0},
				.spd_array = {0}, .len = 0};

	assert_int_equal(CB_SUCCESS, load_spd_cache(&spd_cache, &spd_cache_sz));
	memcpy(spd_cache, spd_data_dram_1, spd_data_dram_1_sz);
	memset(spd_cache + spd_data_dram_1_sz, 0xff, spd_cache_sz - spd_data_dram_1_sz);
	calc_spd_cache_crc(spd_cache);
	assert_int_equal(CB_SUCCESS, spd_fill_from_cache(spd_cache, &blk));

	get_sn_from_spd_cache(spd_cache, get_spd_sn_ret_sn);
	get_spd_sn_ret_sn_idx = 0;
	will_return_always(get_spd_sn, CB_SUCCESS);
	assert_false(check_if_dimm_changed(spd_cache, &blk));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_load_spd_cache, setup_spd_cache_test),
		cmocka_unit_test_setup(test_spd_fill_from_cache, setup_spd_cache_test),
		cmocka_unit_test_setup(test_spd_fill_from_cache_unsupported_dram_type,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_spd_cache_is_valid, setup_spd_cache_test),
#if __TEST_SPD_CACHE_DDR == 4 || __TEST_SPD_CACHE_DDR == 5
		cmocka_unit_test_setup(test_check_if_dimm_changed_not_changed,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_check_if_dimm_changed_sn_error,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_check_if_dimm_changed_sodimm_lost,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_check_if_dimm_changed_new_sodimm,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_check_if_dimm_changed_sn_changed,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_check_if_dimm_changed_with_nonexistent,
				       setup_spd_cache_test),
		cmocka_unit_test_setup(test_check_if_dimm_changed_unsupported_dram_type,
				       setup_spd_cache_test),
#endif
	};

	return cb_run_group_tests(tests, setup_spd_cache, teardown_spd_cache);
}
