/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <fmap.h>
#include <commonlib/region.h>
#include <cpu/intel/cpu_ids.h>
#include <intelblocks/pmclib.h>
#include <cf9_reset.h>

#define SI_DESC_REGION		"SI_DESC"
#define SI_DESC_REGION_SZ	4096
#define PMC_DESC_7_BYTE3	0xc32

/* It updates PMC Descriptor in the Descriptor Region */
static void configure_pmc_descriptor(void)
{
	uint8_t si_desc_buf[SI_DESC_REGION_SZ];
	struct region_device desc_rdev;

	if (fmap_locate_area_as_rdev_rw(SI_DESC_REGION, &desc_rdev) < 0) {
		printk(BIOS_ERR, "Failed to locate %s in the FMAP\n", SI_DESC_REGION);
		return;
	}

	if (rdev_readat(&desc_rdev, si_desc_buf, 0, SI_DESC_REGION_SZ) != SI_DESC_REGION_SZ) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return;
	}

	if (si_desc_buf[PMC_DESC_7_BYTE3] == 0x40) {
		si_desc_buf[PMC_DESC_7_BYTE3] = 0x44;

		if (rdev_eraseat(&desc_rdev, 0, SI_DESC_REGION_SZ) != SI_DESC_REGION_SZ) {
			printk(BIOS_ERR, "Failed to erase Descriptor Region area\n");
			return;
		}

		if (rdev_writeat(&desc_rdev, si_desc_buf, 0, SI_DESC_REGION_SZ)
				!= SI_DESC_REGION_SZ) {
			printk(BIOS_ERR, "Failed to update Descriptor Region\n");
			return;
		}

		printk(BIOS_DEBUG, "Update of PMC Descriptor successful, trigger GLOBAL RESET\n");

		pmc_global_reset_enable(1);
		do_full_reset();
		die("Failed to trigger GLOBAL RESET\n");
	}

	printk(BIOS_DEBUG, "Update of PMC Descriptor is not required!\n");
}

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;
	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

void bootblock_mainboard_init(void)
{
	if (cpu_get_cpuid() == CPUID_ALDERLAKE_A0)
		configure_pmc_descriptor();
}
