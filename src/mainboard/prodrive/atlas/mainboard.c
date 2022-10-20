/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <stdint.h>
#include <gpio.h>
#include <arch/io.h>
#include <string.h>
#include <smbios.h>

#include "gpio.h"

void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	const u8 mc = dimm->ctrlr_num;
	const u8 ch = dimm->channel_num;
	const u8 mm = dimm->dimm_num;

	char dev_loc[40] = { "\x00" };
	snprintf(dev_loc, sizeof(dev_loc), "SO-DIMM %c%u", 'A' + mc, mm);
	t->device_locator = smbios_add_string(t->eos, dev_loc);

	char bnk_loc[40] = { "\x00" };
	snprintf(bnk_loc, sizeof(bnk_loc), "BANK-%u-%u-%u", mc, ch, mm);
	t->bank_locator = smbios_add_string(t->eos, bnk_loc);
}

void smbios_fill_dimm_asset_tag(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	const u8 mc = dimm->ctrlr_num;
	const u8 ch = dimm->channel_num;
	const u8 mm = dimm->dimm_num;

	char tag[40] = { "\x00" };
	snprintf(tag, sizeof(tag), "MC-%u-CH-%u-DIMM-%u", mc, ch, mm);
	t->asset_tag = smbios_add_string(t->eos, tag);
}

static uint8_t get_hsid(void)
{
	const gpio_t hsid_gpios[] = {
		GPP_A8,
		GPP_F19,
		GPP_H23,
		GPP_H19,
	};
	return gpio_base2_value(hsid_gpios, ARRAY_SIZE(hsid_gpios));
}

static void mainboard_init(void *chip_info)
{
	configure_gpio_pads();
	printk(BIOS_INFO, "HSID: 0x%x\n", get_hsid());
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
