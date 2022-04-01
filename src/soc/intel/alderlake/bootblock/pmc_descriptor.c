/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/cpu.h>
#include <arch/mmio.h>
#include <cf9_reset.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/intel/cpu_ids.h>
#include <fmap.h>
#include <intelblocks/pmclib.h>
#include <soc/bootblock.h>
#include <types.h>

#define PMC_DESC_7_BYTE3	0xc32

/* Flash Master 1 : HOST/BIOS */
#define FLMSTR1			0x80

/* Flash signature Offset */
#define FLASH_SIGN_OFFSET	0x10
#define FLMSTR_WR_SHIFT_V2	20
#define FLASH_VAL_SIGN		0xFF0A55A

/* It checks whether host(Flash Master 1) has write access to the Descriptor Region or not */
static int is_descriptor_writeable(uint8_t *desc)
{
	/* Check flash has valid signature */
	if (read32((void *)(desc + FLASH_SIGN_OFFSET)) != FLASH_VAL_SIGN) {
		printk(BIOS_DEBUG, "Flash Descriptor is not valid\n");
		return 0;
	}

	/* Check host has write access to the Descriptor Region */
	if (!((read32((void *)(desc + FLMSTR1)) >> FLMSTR_WR_SHIFT_V2) & BIT(0))) {
		printk(BIOS_DEBUG, "Host doesn't have write access to Descriptor Region\n");
		return 0;
	}

	return 1;
}

/* It updates PMC Descriptor in the Descriptor Region */
void configure_pmc_descriptor(void)
{
	uint8_t si_desc_buf[CONFIG_SI_DESC_REGION_SZ];
	struct region_device desc_rdev;

	if (cpu_get_cpuid() != CPUID_ALDERLAKE_J0)
		return;

	if (fmap_locate_area_as_rdev_rw(CONFIG_SI_DESC_REGION, &desc_rdev) < 0) {
		printk(BIOS_ERR, "Failed to locate %s in the FMAP\n", CONFIG_SI_DESC_REGION);
		return;
	}

	if (rdev_readat(&desc_rdev, si_desc_buf, 0, CONFIG_SI_DESC_REGION_SZ) !=
			CONFIG_SI_DESC_REGION_SZ) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return;
	}

	if (!is_descriptor_writeable(si_desc_buf))
		return;

	if (si_desc_buf[PMC_DESC_7_BYTE3] != 0x40) {
		printk(BIOS_DEBUG, "Update of PMC Descriptor is not required!\n");
		return;
	}

	si_desc_buf[PMC_DESC_7_BYTE3] = 0x44;

	if (rdev_eraseat(&desc_rdev, 0, CONFIG_SI_DESC_REGION_SZ) != CONFIG_SI_DESC_REGION_SZ) {
		printk(BIOS_ERR, "Failed to erase Descriptor Region area\n");
		return;
	}

	if (rdev_writeat(&desc_rdev, si_desc_buf, 0, CONFIG_SI_DESC_REGION_SZ)
			!= CONFIG_SI_DESC_REGION_SZ) {
		printk(BIOS_ERR, "Failed to update Descriptor Region\n");
		return;
	}

	printk(BIOS_DEBUG, "Update of PMC Descriptor successful, trigger GLOBAL RESET\n");

	pmc_global_reset_enable(true);
	do_full_reset();
	die("Failed to trigger GLOBAL RESET\n");
}
