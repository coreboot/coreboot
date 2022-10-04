
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <cpu/x86/name.h>
#include <device/device.h>
#include <device/mmio.h>
#include <intelblocks/cse.h>
#include <pc80/i8254.h>
#include <soc/intel/common/reset.h>
#include <soc/ramstage.h>
#include <fmap.h>
#include <smbios.h>
#include <string.h>

static bool need_global_reset = false;

/* Flash Master 1 : HOST/BIOS */
#define FLMSTR1			0x80

#define FLASH_SIGNATURE_OFFSET	0x10
#define FLMSTR_WR_SHIFT_V2	20
#define FLASH_SIGNATURE_VAL	0x0FF0A55A

#define SI_DESC_SIZE		(4 * KiB)
#define SI_DESC_REGION		"SI_DESC"

const char *smbios_mainboard_product_name(void)
{
	char processor_name[49];

	fill_processor_name(processor_name);

	if (strstr(processor_name, "i3-10110U") != NULL)
		return "VP4630";
	else if (strstr(processor_name, "i5-10210U") != NULL)
		return "VP4650";
	else if (strstr(processor_name, "i7-10810U") != NULL)
		return "VP4670";
	else
		return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

static void mainboard_final(void *unused)
{
	if (CONFIG(BEEP_ON_BOOT))
		beep(1500, 100);
}

/* It checks whether host (Flash Master 1) has write access to the Descriptor Region or not */
static bool is_descriptor_writeable(uint8_t *desc)
{
	/* Check flash has valid signature */
	if (read32((void *)(desc + FLASH_SIGNATURE_OFFSET)) != FLASH_SIGNATURE_VAL) {
		printk(BIOS_ERR, "Flash Descriptor is not valid\n");
		printk(BIOS_ERR, "Descriptor needs to be fixed to ensure proper operation\n");
		return false;
	}

	/* Check host has write access to the Descriptor Region */
	if (!((read32((void *)(desc + FLMSTR1)) >> FLMSTR_WR_SHIFT_V2) & BIT(0))) {
		printk(BIOS_ERR, "Host doesn't have write access to Descriptor Region\n");
		return false;
	}

	return true;
}

static void configure_descriptor_for_lpc_tpm(void)
{
	uint8_t si_desc_buf[SI_DESC_SIZE];
	struct region_device desc_rdev;

	if (fmap_locate_area_as_rdev_rw(SI_DESC_REGION, &desc_rdev) < 0) {
		printk(BIOS_ERR, "Failed to locate %s in the FMAP\n", SI_DESC_REGION);
		return;
	}

	if (rdev_readat(&desc_rdev, si_desc_buf, 0, SI_DESC_SIZE) != SI_DESC_SIZE) {
		printk(BIOS_ERR, "Failed to read Descriptor Region from SPI Flash\n");
		return;
	}

	if (!is_descriptor_writeable(si_desc_buf))
		return;

	/* Disable SPI TPM if necessary */
	if ((si_desc_buf[0x1f0] & 0xfe) == si_desc_buf[0x1f0]) {
		printk(BIOS_DEBUG, "Update of Descriptor is not required!\n");
		return;
	}

	si_desc_buf[0x1f0] &= 0xfe;

	if (rdev_eraseat(&desc_rdev, 0, SI_DESC_SIZE) != SI_DESC_SIZE) {
		printk(BIOS_ERR, "Failed to erase Descriptor Region area\n");
		return;
	}

	if (rdev_writeat(&desc_rdev, si_desc_buf, 0, SI_DESC_SIZE) != SI_DESC_SIZE) {
		printk(BIOS_ERR, "Failed to update Descriptor Region\n");
		return;
	}

	printk(BIOS_DEBUG, "Update of Descriptor successful\n");
	need_global_reset = true;
}

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	/* Call it right before Silicon Init, so that we avoid EOP */
	configure_descriptor_for_lpc_tpm();
	cse_enable_ptt(false);
	/*
	 * We wait with global reset after descriptor update until PTT state change to avoid
	 * double global reset. In case PTT was already disabled or cse_enable_ptt will fail
	 * for some reason, but descriptor has been updated we need to do global reset here,
	 * otherwise cse_enable_ptt will do the global reset and the branch below won't be
	 * reached.
	 */
	if (need_global_reset)
		do_global_reset();
}

struct chip_operations mainboard_ops = {
	.final = mainboard_final,
};
