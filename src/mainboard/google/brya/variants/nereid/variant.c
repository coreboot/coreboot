/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <console/console.h>
#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>
#include <soc/bootblock.h>

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1A_HDMI)))
		return "vbt-nereid_hdmi.bin";

	return "vbt.bin";
}

void variant_update_descriptor(void)
{
	/* TypeCPort2Config = "No Thunderbolt" */
	struct descriptor_byte typec_bytes[] = {
		{ 0xc76, 0xb7 },
		{ 0xc77, 0xb6 },
		{ 0xc7c, 0xee },
		{ 0xca0, 0x0c },
	};

	/* TypeCPort2Config = "DP Fixed Connection" */
	struct descriptor_byte hdmi_bytes[] = {
		{ 0xc76, 0x75 },
		{ 0xc77, 0xc4 },
		{ 0xc7c, 0x1e },
		{ 0xca0, 0x0e },
	};

	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1A_HDMI))) {
		printk(BIOS_INFO, "Configuring descriptor for HDMI\n");
		configure_descriptor(hdmi_bytes, ARRAY_SIZE(hdmi_bytes));
	} else {
		printk(BIOS_INFO, "Configuring descriptor for Type-C\n");
		configure_descriptor(typec_bytes, ARRAY_SIZE(typec_bytes));
	}
}
