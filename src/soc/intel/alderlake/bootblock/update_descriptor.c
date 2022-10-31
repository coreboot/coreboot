/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/mmio.h>
#include <cf9_reset.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <fmap.h>
#include <intelblocks/pmclib.h>
#include <soc/bootblock.h>
#include <types.h>

/* Flash Master 1 : HOST/BIOS */
#define FLMSTR1			0x80

/* Flash signature Offset */
#define FLASH_SIGN_OFFSET	0x10
#define FLMSTR_WR_SHIFT_V2	20
#define FLASH_VAL_SIGN		0xFF0A55A

/* It checks whether host (Flash Master 1) has write access to the Descriptor Region or not */
static bool is_descriptor_writeable(uint8_t *desc)
{
	/* Check flash has valid signature */
	if (read32((void *)(desc + FLASH_SIGN_OFFSET)) != FLASH_VAL_SIGN) {
		printk(BIOS_ERR, "Flash Descriptor is not valid\n");
		return 0;
	}

	/* Check host has write access to the Descriptor Region */
	if (!((read32((void *)(desc + FLMSTR1)) >> FLMSTR_WR_SHIFT_V2) & BIT(0))) {
		printk(BIOS_ERR, "Host doesn't have write access to Descriptor Region\n");
		return 0;
	}

	return 1;
}

void configure_descriptor(struct descriptor_byte *bytes, size_t num)
{
	uint8_t si_desc_buf[CONFIG_SI_DESC_REGION_SZ];
	struct region_device desc_rdev;
	bool update_required = false;

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

	for (size_t i = 0; i < num; i++) {
		size_t offset = bytes[i].offset;
		uint8_t desired_value = bytes[i].desired_value;
		printk(BIOS_DEBUG, "Current value of Descriptor byte 0x%zx: 0x%x\n",
		       offset, si_desc_buf[offset]);
		if (si_desc_buf[offset] != desired_value) {
			update_required = true;
			si_desc_buf[offset] = desired_value;
		}
	}

	if (!update_required) {
		printk(BIOS_DEBUG, "Update of Descriptor is not required!\n");
		return;
	}

	if (rdev_eraseat(&desc_rdev, 0, CONFIG_SI_DESC_REGION_SZ) != CONFIG_SI_DESC_REGION_SZ) {
		printk(BIOS_ERR, "Failed to erase Descriptor Region area\n");
		return;
	}

	if (rdev_writeat(&desc_rdev, si_desc_buf, 0, CONFIG_SI_DESC_REGION_SZ)
			!= CONFIG_SI_DESC_REGION_SZ) {
		printk(BIOS_ERR, "Failed to update Descriptor Region\n");
		return;
	}

	printk(BIOS_DEBUG, "Update of Descriptor successful, trigger GLOBAL RESET\n");

	pmc_global_reset_enable(true);
	do_full_reset();
	die("Failed to trigger GLOBAL RESET\n");
}
