/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <console/console.h>
#include <symbols.h>
#include <soc/sdram.h>

#define DRAM_TEST_LEN 0x8000000

static void test_ddr(void)
{
	if (IS_ENABLED(CONFIG_CYGNUS_SDRAM_TEST_DDR)) {
		uint32_t *test_buffer = (uint32_t *) _dram;
		uint32_t len = DRAM_TEST_LEN;
		uint32_t i;
		uint32_t fail_count = 0;

		printk(BIOS_INFO, "test ddr start from 0x%p to 0x%p\n", test_buffer,
			test_buffer + len);

		for (i = 0; i < len; i++)
			*(test_buffer + i) = i;

		for (i = 0; i < len; i++) {
			int val = *(test_buffer + i);

			if ((i % 0x10000) == 0)
				printk(BIOS_INFO, "#");

			if (i != val) {
				printk(BIOS_ERR, "\ntest_ddr: @ 0x%p: %d != %d\n",
					test_buffer + i, i, val);
				fail_count++;
			}
		}
		printk(BIOS_INFO, "\ntest ddr end: fail=%d\n", fail_count);
	}
}

void sdram_init(void)
{
	printk(BIOS_INFO, "sdram initialization is in progress...\n");
	ddr_init2();
	printk(BIOS_INFO, "sdram initialization is completed.\n");

	test_ddr();
}

uint32_t sdram_size_mb(void)
{
	return CONFIG_DRAM_SIZE_MB;
}
