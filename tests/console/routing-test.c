/* SPDX-License-Identifier: GPL-2.0-only */

#include "../console/init.c"

#include <console/console.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <tests/test.h>

struct log_combinations_t {
	int log_lvl;
	int msg_lvl;
	int behavior;
} combinations[] = {
	{.log_lvl = -1, .msg_lvl = BIOS_ERR, .behavior = CONSOLE_LOG_NONE},
	{.log_lvl = -1, .msg_lvl = BIOS_SPEW, .behavior = CONSOLE_LOG_NONE},

	{.log_lvl = BIOS_DEBUG, .msg_lvl = BIOS_ERR, .behavior = CONSOLE_LOG_ALL},
	{.log_lvl = BIOS_DEBUG, .msg_lvl = BIOS_DEBUG, .behavior = CONSOLE_LOG_ALL},
	{.log_lvl = BIOS_DEBUG, .msg_lvl = BIOS_SPEW, .behavior = CONSOLE_LOG_NONE},

	{.log_lvl = BIOS_SPEW, .msg_lvl = BIOS_ERR, .behavior = CONSOLE_LOG_ALL},
	{.log_lvl = BIOS_SPEW, .msg_lvl = BIOS_DEBUG, .behavior = CONSOLE_LOG_ALL},
	{.log_lvl = BIOS_SPEW, .msg_lvl = BIOS_SPEW, .behavior = CONSOLE_LOG_ALL},

#if CONFIG(CONSOLE_CBMEM)
	{.log_lvl = BIOS_WARNING, .msg_lvl = BIOS_ERR, .behavior = CONSOLE_LOG_ALL},
	{.log_lvl = BIOS_WARNING, .msg_lvl = BIOS_DEBUG, .behavior = CONSOLE_LOG_FAST},
	{.log_lvl = BIOS_WARNING, .msg_lvl = BIOS_SPEW, .behavior = CONSOLE_LOG_NONE},

#else
	{.log_lvl = BIOS_WARNING, .msg_lvl = BIOS_ERR, .behavior = CONSOLE_LOG_ALL},
	{.log_lvl = BIOS_WARNING, .msg_lvl = BIOS_DEBUG, .behavior = CONSOLE_LOG_NONE},
	{.log_lvl = BIOS_WARNING, .msg_lvl = BIOS_SPEW, .behavior = CONSOLE_LOG_NONE},
#endif
};


static void test_console_log_level(void **state)
{
	for (int i = 0; i < ARRAY_SIZE(combinations); i++) {
		console_loglevel = combinations[i].log_lvl;
		assert_int_equal(combinations[i].behavior,
				 console_log_level(combinations[i].msg_lvl));
	}
}

static int setup_console_log_level(void **state)
{
	console_inited = 1;
	return 0;
}

static int teardown_console_log_level(void **state)
{
	console_inited = 0;
	return 0;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup_teardown(test_console_log_level, setup_console_log_level,
						teardown_console_log_level),
	};

	return cb_run_group_tests(tests, NULL, NULL);
}
