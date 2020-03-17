/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <boardid.h>
#include <drivers/vpd/vpd.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <variant/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#if CONFIG(GENERATE_SMBIOS_TABLES)
#define VPD_KEY_SYSTEM_SERIAL		"serial_number"
#define VPD_KEY_MAINBOARD_SERIAL	"mlb_serial_number"
#define VPD_SERIAL_LEN			64

const char *smbios_system_serial_number(void)
{
	static char serial[VPD_SERIAL_LEN];
	if (vpd_gets(VPD_KEY_SYSTEM_SERIAL, serial, VPD_SERIAL_LEN, VPD_RO))
		return serial;
	return "";
}

const char *smbios_mainboard_serial_number(void)
{
	static char serial[VPD_SERIAL_LEN];
	if (vpd_gets(VPD_KEY_MAINBOARD_SERIAL, serial, VPD_SERIAL_LEN, VPD_RO))
		return serial;
	return "";
}

/* mainboard silk screen shows DIMM-A and DIMM-B */
void smbios_fill_dimm_locator(const struct dimm_info *dimm,
	struct smbios_type17 *t)
{
	switch (dimm->channel_num) {
	case 0:
		t->device_locator = smbios_add_string(t->eos, "DIMM-A");
		break;
	case 1:
		t->device_locator = smbios_add_string(t->eos, "DIMM-B");
		break;
	default:
		t->device_locator = smbios_add_string(t->eos, "UNKNOWN");
		break;
	}
}
#endif

static const struct pad_config gpio_unused[] = {
/* SUSWARN# */		PAD_NC(GPP_A13, NONE),
/* SUSACK# */		PAD_NC(GPP_A15, NONE),
/* M2_SKT2_CFG0 */	PAD_NC(GPP_H12, NONE),
/* M2_SKT2_CFG1 */	PAD_NC(GPP_H13, NONE),
};

static void mainboard_init(void *chip_info)
{
	const struct pad_config *gpio_table;
	size_t num_gpios;

	gpio_table = variant_gpio_table(&num_gpios);
	cnl_configure_pads(gpio_table, num_gpios);

	/* Disable unused pads for devices with board ID > 2 */
	if (board_id() > 2)
		gpio_configure_pads(gpio_unused, ARRAY_SIZE(gpio_unused));
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_inject_dsdt_generator = chromeos_dsdt_generator;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
