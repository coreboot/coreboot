/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <tests/test.h>

/* stub */
static int log_level = 0;
int get_log_level(void)
{
	return log_level;
}

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
		log_level = combinations[i].log_lvl;
		assert_int_equal(combinations[i].behavior,
			console_log_level(combinations[i].msg_lvl));
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_console_log_level),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}
