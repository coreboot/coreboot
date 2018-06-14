/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.  <support@cavium.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <console/console.h>
#include <soc/sdram.h>

#include <libbdk-arch/bdk-warn.h>
#include <libbdk-arch/bdk-csrs-rst.h>
#include <libbdk-boot/bdk-watchdog.h>
#include <libbdk-dram/bdk-dram-config.h>
#include <libbdk-dram/bdk-dram-test.h>
#include <libbdk-hal/bdk-config.h>
#include <libbdk-hal/bdk-utils.h>
#include <libbdk-hal/bdk-l2c.h>
#include <libdram/libdram-config.h>

size_t sdram_size_mb(void)
{
	return bdk_dram_get_size_mbytes(0);
}

/* based on bdk_boot_dram() */
void sdram_init(void)
{
	printk(BIOS_DEBUG, "Initializing DRAM\n");

	/**
	 * FIXME: second arg is actually a desired frequency if set (the
	 * function usually obtains frequency via the config). That might
	 * be useful if FDT or u-boot env is too cumbersome.
	 */
	int mbytes = bdk_dram_config(0, 0);
	if (mbytes < 0) {
		bdk_error("N0: Failed DRAM init\n");
		die("DRAM INIT FAILED !\n");
	}

	/* Poke the watchdog */
	bdk_watchdog_poke();

	/* Report DRAM status */
	printf("N0: DRAM:%s\n", bdk_dram_get_info_string(0));

	/* See if we should test this node's DRAM during boot */
	int test_dram = bdk_config_get_int(BDK_CONFIG_DRAM_BOOT_TEST, 0);
	if (test_dram == 1) {
		static const u8 tests[] = {13, 0, 1};
		for (size_t i = 0; i < ARRAY_SIZE(tests); i++) {
			/* Run the address test to make sure DRAM works */
			if (bdk_dram_test(tests[i], 4 * MiB,
					  sdram_size_mb() * MiB - 4 * MiB,
					  BDK_DRAM_TEST_NO_STATS |
					  BDK_DRAM_TEST_NODE0)) {
				printk(BIOS_CRIT, "%s: Failed DRAM test.\n",
				       __func__);
			}
			bdk_watchdog_poke();
		}
	} else {
		/* Run the address test to make sure DRAM works */
		if (bdk_dram_test(13, 4 * MiB,
			  sdram_size_mb() * MiB - 4 * MiB,
			  BDK_DRAM_TEST_NO_STATS |
			  BDK_DRAM_TEST_NODE0)) {
			/**
			 * FIXME(dhendrix): This should be handled by mainboard
			 * code since we don't necessarily have a BMC to report
			 * to. Also, we need to figure out if we need to keep
			 * going as to avoid getting into a boot loop.
			 */
			// bdk_boot_status(BDK_BOOT_STATUS_REQUEST_POWER_CYCLE);
			printk(BIOS_CRIT, "%s: Failed DRAM test.\n", __func__);
		}
	}

	bdk_watchdog_poke();

	/* Unlock L2 now that DRAM works */
	if (0 == bdk_numa_master()) {
		uint64_t l2_size = bdk_l2c_get_cache_size_bytes(0);
		BDK_TRACE(INIT, "Unlocking L2\n");
		bdk_l2c_unlock_mem_region(0, 0, l2_size);
		bdk_watchdog_poke();
	}

	printk(BIOS_INFO, "SDRAM initialization finished.\n");
}
