/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <soc/bootblock.h>

/*
 * TODO(b/229022567): This is a workaround which will be removed once we
 * implement a proper solution for configuring the descriptor differently for
 * different SKUs.
 */
void variant_update_descriptor(void)
{
	/*
	 * UfsCont1Config = "Disabled"
	 * IshSupported = "No"
	 */
	struct descriptor_byte emmc_bytes[] = {
		{ 0x1f8, 0x55 },
		{ 0x1f9, 0x55 },
		{ 0xc18, 0x89 },
		{ 0xc1d, 0xb8 },
	};

	/*
	 * UfsCont1Config = "X2"
	 * IshSupported = "Yes"
	 */
	struct descriptor_byte ufs_bytes[] = {
		{ 0x1f8, 0x95 },
		{ 0x1f9, 0x59 },
		{ 0xc18, 0x09 },
		{ 0xc1d, 0x28 },
	};

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		printk(BIOS_INFO, "Configuring descriptor for UFS\n");
		configure_descriptor(ufs_bytes, ARRAY_SIZE(ufs_bytes));
	} else {
		printk(BIOS_INFO, "Configuring descriptor for eMMC\n");
		configure_descriptor(emmc_bytes, ARRAY_SIZE(emmc_bytes));
	}
}
