/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SELF_TEST_H_
#define _SELF_TEST_H_

#include <commonlib/self_test.h>
#include <stdbool.h>

struct self_test_t {
	/*
	 * A 1-based unique number to identify a self test
	 */
	uint32_t id;
	/*
	 * Boolean pre-check condition to determine if the test should be registered
	 * (e.g., CONFIG(SELF_TEST_INTEL_CPU_UCODE)). Must be set to true for the test to run.
	 */
	bool pre_check;
	/*
	 * Test override function to perform the test.
	 * the return type of the function should be in struct st_status
	 */
	struct st_status (*exec)(void);
	/*
	 * Specify the Boot State to execute this test, e.g. -
	 * 1. BS_DEV_ENABLE, BS_ON_ENTRY <------- After PCI Enumeration
	 * 2. BS_PAYLOAD_LOAD, BS_ON_EXIT <------ Before Ready to Boot on Normal Boot Path
	 * 3. BS_OS_RESUME, BS_ON_ENTRY  <------- Before Ready to Boot on S3 resume Path
	 */
	boot_state_t state;
	boot_state_sequence_t when;
};

void register_selftest(const struct self_test_t *tests);

#define REGISTER_SELFTEST(tests)							\
	static void register_selftest_##tests(void *unused)				\
	{										\
		register_selftest(tests);						\
	}										\
	BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, register_selftest_##tests, NULL)

#define st_info(STR...) printk(BIOS_INFO, STR)

#define st_debug(...)						\
	do {							\
		if (CONFIG(SELF_TEST_DEBUG))			\
			printk(BIOS_DEBUG, __VA_ARGS__);	\
	} while (0)

#endif /* _SELF_TEST_H_ */
