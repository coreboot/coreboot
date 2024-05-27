/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <smbios.h>
#include <stdio.h>
#include <types.h>

#include "gpio.h"
#include "vpd.h"

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

static const char *get_formatted_pn(void)
{
	static char buffer[32 + ATLAS_SN_PN_LENGTH] = {0};
	const char *prefix = "P/N: ";
	snprintf(buffer, sizeof(buffer), "%s%s", prefix, get_emi_eeprom_vpd()->part_number);
	return buffer;
}

static void mainboard_smbios_strings(struct device *dev, struct smbios_type11 *t)
{
	t->count = smbios_add_string(t->eos, get_formatted_pn());
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	const struct emi_eeprom_vpd *eeprom = get_emi_eeprom_vpd();
	const bool sleep_enable = eeprom->profile != ATLAS_PROF_REALTIME_PERFORMANCE ? 1 : 0;
	acpigen_ssdt_override_sleep_states(false, false,
					   CONFIG(HAVE_ACPI_RESUME) && sleep_enable,
					   sleep_enable);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->get_smbios_strings = mainboard_smbios_strings;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init       = mainboard_init,
	.enable_dev = mainboard_enable,
};
