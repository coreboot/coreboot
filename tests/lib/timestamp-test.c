/* SPDX-License-Identifier: GPL-2.0-only */

#include "../lib/timestamp.c"
#include <commonlib/bsd/helpers.h>
#include <tests/test.h>
#include "stubs/timestamp.h"

/* Timestamp region definition */
#define TIMESTAMP_REGION_SIZE (1 * KiB)
TEST_REGION(timestamp, TIMESTAMP_REGION_SIZE);

void test_timestamp_init(void **state)
{
	timestamp_init(1000);

	assert_non_null(glob_ts_table);
}

void test_timestamp_add(void **state)
{
	const int base_multipler = 2000;
	const int timestamp_base = 1000;
	struct timestamp_entry *entry;
	int i;

	timestamp_init(timestamp_base);

	timestamp_add(TS_ROMSTAGE_START, base_multipler);

	assert_int_equal(1, glob_ts_table->num_entries);

	entry = &glob_ts_table->entries[0];
	assert_int_equal(1, entry->entry_id);
	assert_int_equal(base_multipler - timestamp_base, /* Added timestamp reduced by base */
			 entry->entry_stamp);

	/* Add few timestamps to check if all of them will be added properly */
	for (i = 1; i < 10; ++i)
		timestamp_add(i + 1, base_multipler * (i + 1));

	assert_int_equal(10, glob_ts_table->num_entries);

	for (i = 0; i < 10; ++i) {
		entry = &glob_ts_table->entries[i];
		assert_int_equal(i + 1, entry->entry_id);
		assert_int_equal(base_multipler * (i + 1) - timestamp_base, entry->entry_stamp);
	}
}

void test_timestamp_add_now(void **state)
{
	const int base_multipler = 2000;
	const int timestamp_base = 1000;
	struct timestamp_entry *entry;

	/* Initialize with base timestamp of 1000.
	 * This value will be subtracted from each timestamp
	 * when adding it.
	 */
	timestamp_init(timestamp_base);

	dummy_timestamp_set(base_multipler);

	timestamp_add_now(TS_ROMSTAGE_START);

	assert_int_equal(1, glob_ts_table->num_entries);

	entry = &glob_ts_table->entries[0];

	assert_int_equal(1, entry->entry_id);
	assert_int_equal(base_multipler - timestamp_base, /* Added timestamp reduced by base */
			 entry->entry_stamp);
}

void test_timestamp_rescale_table(void **state)
{
	const int base_multipler = 1000;
	int i;

	timestamp_init(0);

	/* Add few timestamps to check if all of them will be rescaled properly */
	for (i = 1; i <= 10; ++i)
		timestamp_add(i, base_multipler * i);

	/* Check if all entries were added to table */
	assert_int_equal(10, glob_ts_table->num_entries);

	timestamp_rescale_table(2, 4);

	/* Check if there is the same number of entries */
	assert_int_equal(10, glob_ts_table->num_entries);

	for (i = 0; i < glob_ts_table->num_entries; ++i)
		assert_int_equal(base_multipler * (i + 1) / 4 * 2,
				 glob_ts_table->entries[i].entry_stamp);
}

void test_get_us_since_boot(void **state)
{
	const int base_multipler = 10000;
	const int timestamp_base = 1000;
	const int freq_base = 100;

	timestamp_init(timestamp_base);
	dummy_timestamp_set(base_multipler);
	dummy_timestamp_tick_freq_mhz_set(freq_base);
	/* There is a need to update this field manually, because cbmem hooks are not used. */
	glob_ts_table->tick_freq_mhz = freq_base;

	assert_int_equal((base_multipler - timestamp_base) / freq_base, get_us_since_boot());
}

int setup_timestamp_and_freq(void **state)
{
	dummy_timestamp_set(0);
	dummy_timestamp_tick_freq_mhz_set(1);

	return 0;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_timestamp_init, setup_timestamp_and_freq),
		cmocka_unit_test_setup(test_timestamp_add, setup_timestamp_and_freq),
		cmocka_unit_test_setup(test_timestamp_add_now, setup_timestamp_and_freq),
		cmocka_unit_test_setup(test_timestamp_rescale_table, setup_timestamp_and_freq),
		cmocka_unit_test_setup(test_get_us_since_boot, setup_timestamp_and_freq),
	};

#if CONFIG(COLLECT_TIMESTAMPS)
	return cb_run_group_tests(tests, NULL, NULL);
#else
	return 0;
#endif
}
