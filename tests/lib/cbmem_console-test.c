/* SPDX-License-Identifier: GPL-2.0-only */

#include "../lib/cbmem_console.c"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <tests/test.h>


#if ENV_ROMSTAGE_OR_BEFORE
/* Weak references have to be here, so TEST_REGION macro will work properly */
__weak extern u8 _preram_cbmem_console[];
__weak extern u8 _epreram_cbmem_console[];

#define PRERAM_CBMEM_CONSOLE_SIZE (1 * KiB)
TEST_REGION(preram_cbmem_console, PRERAM_CBMEM_CONSOLE_SIZE);
#endif

/* Disable init hooks. This test does not need them. */
void cbmem_run_init_hooks(int is_recovery)
{
	(void)is_recovery;
}

int setup_cbmemc(void **state)
{
	cbmemc_init();
	return 0;
}

int teardown_cbmemc(void **state)
{
	current_console->size = 0;
	return 0;
}

void test_cbmemc_init(void **state)
{
	cbmemc_init();

	/* Check if console structure was created. */
	assert_non_null(current_console);
}

void test_cbmemc_tx_byte(void **state)
{
	int i;
	u32 cursor;
	const unsigned char data[] = "Random testing string\n"
				"`1234567890-=~!@#$%^&*()_+\n"
				"abcdefghijklmnopqrstuvwxyz\n"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";

	for (i = 0; i < ARRAY_SIZE(data); ++i)
		cbmemc_tx_byte(data[i]);

	cursor = current_console->cursor & CURSOR_MASK;

	assert_int_equal(ARRAY_SIZE(data), cursor);

	/* Check if all characters were added correctly. */
	assert_memory_equal(data, current_console->body, ARRAY_SIZE(data));
}

void test_cbmemc_tx_byte_overflow(void **state)
{
	int i;
	u32 cursor;
	u32 flags;
	const uint32_t console_size = current_console->size;
	const unsigned char data[] = "Another random string\n"
				"abcdefghijklmnopqrstuvwxyz\n"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
				"`1234567890-=~!@#$%^&*()_+\n";
	const int data_size = ARRAY_SIZE(data) - 1;
	const int data_stream_length = console_size + data_size;
	const int overflow_bytes = data_stream_length % console_size;
	unsigned char *check_buffer =
			(unsigned char *)malloc(sizeof(unsigned char) * console_size);

	/* Fill console buffer */
	for (i = 0; i < console_size; ++i)
		cbmemc_tx_byte(data[i % data_size]);

	/* Store buffer for checking */
	memcpy(check_buffer, current_console->body, console_size);

	assert_memory_equal(current_console->body, data, overflow_bytes);

	/* Add more data to console buffer to override older bytes */
	for (; i < data_stream_length; ++i)
		cbmemc_tx_byte(data[i % data_size]);

	cursor = current_console->cursor & CURSOR_MASK;
	flags = current_console->cursor & ~CURSOR_MASK;

	/* Check if there was a overflow in buffer */
	assert_int_equal(OVERFLOW, flags & OVERFLOW);

	/* Check if cursor got back to the beginning of a buffer */
	assert_int_equal(data_size, cursor);

	/* Check if overflow buffer was overwritten */
	assert_memory_not_equal(current_console->body,
			data,
			overflow_bytes);

	/* Check if rest of the buffer contents, that should not be overridden,
	 * is the same.
	 */
	assert_memory_equal(&current_console->body[overflow_bytes],
			check_buffer + overflow_bytes,
			console_size - overflow_bytes);

	free(check_buffer);
}

int main(void)
{
#if ENV_ROMSTAGE_OR_BEFORE
	const char *test_name = "cbmem_console-test-romstage";
#else
	const char *test_name = "cbmem_console-test-ramstage";
#endif

	const struct CMUnitTest tests[] = {
		cmocka_unit_test_teardown(test_cbmemc_init, teardown_cbmemc),
		cmocka_unit_test_setup_teardown(test_cbmemc_tx_byte,
						setup_cbmemc, teardown_cbmemc),
		cmocka_unit_test_setup_teardown(test_cbmemc_tx_byte_overflow,
						setup_cbmemc, teardown_cbmemc),
	};

	return cmocka_run_group_tests_name(test_name, tests, NULL, NULL);
}
