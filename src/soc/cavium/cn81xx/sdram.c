/* SPDX-License-Identifier: GPL-2.0-only */

/*
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
#include <soc/ecam.h>
#include <device/pci_ops.h>
#include <device/mmio.h>
#include <device/pci.h>

size_t sdram_size_mb(void)
{
	return bdk_dram_get_size_mbytes(0);
}

#define BDK_RNM_CTL_STATUS	0
#define BDK_RNM_RANDOM		0x100000

#if ENV_RAMINIT
/* Enable RNG for DRAM init */
static void rnm_init(void)
{
	/* Bus numbers are hardcoded in ASIC. No need to program bridges. */
	pci_devfn_t dev = PCI_DEV(2, 0, 0);

	u64 *bar = (u64 *)ecam0_get_bar_val(dev, 0);
	if (!bar) {
		printk(BIOS_ERR, "RNG: Failed to get BAR0\n");
		return;
	}

	printk(BIOS_DEBUG, "RNG: BAR0 at %p\n", bar);

	u64 reg = read64(&bar[BDK_RNM_CTL_STATUS]);
	/*
	 * Enables the output of the RNG.
	 * Entropy enable for random number generator.
	 */
	reg |= 3;
	write64(&bar[BDK_RNM_CTL_STATUS], reg);

	/* Read back after enable so we know it is done. */
	reg = read64(&bar[BDK_RNM_CTL_STATUS]);
	/*
	 * Errata (RNM-22528) First consecutive reads to RNM_RANDOM return same
	 * value. Before using the random entropy, read RNM_RANDOM at least once
	 * and discard the data
	 */
	reg = read64(&bar[BDK_RNM_RANDOM]);
	printk(BIOS_SPEW, "RNG: RANDOM %llx\n", reg);
	reg = read64(&bar[BDK_RNM_RANDOM]);
	printk(BIOS_SPEW, "RNG: RANDOM %llx\n", reg);
}

/* based on bdk_boot_dram() */
void sdram_init(void)
{
	printk(BIOS_DEBUG, "Initializing DRAM\n");

	rnm_init();

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
#endif
