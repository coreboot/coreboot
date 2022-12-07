/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/nhlt.h>

#include "gpio.h"

#define SUBSYSTEM_ID 0x1AE0006B

static const char *oem_id_maxim = "GOOGLE";
static const char *oem_table_id_maxim = "EVEMAX";

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static unsigned long mainboard_write_acpi_tables(
	const struct device *device, unsigned long current, acpi_rsdp_t *rsdp)
{
	uintptr_t start_addr;
	uintptr_t end_addr;
	struct nhlt *nhlt;

	start_addr = current;

	nhlt = nhlt_init();
	if (!nhlt)
		return start_addr;

	nhlt->subsystem_id = SUBSYSTEM_ID;

	/* 4 Channel DMIC array */
	if (nhlt_soc_add_rt5514(nhlt, AUDIO_LINK_SSP0, 4, 1))
		printk(BIOS_ERR, "Couldn't add rt5514.\n");

	/* RT5663 Headset codec */
	if (nhlt_soc_add_rt5663(nhlt, AUDIO_LINK_SSP1))
		printk(BIOS_ERR, "Couldn't add headset codec.\n");

	/* MAXIM98927 Smart Amps for left and right channel */
	/* Render time_slot is 0 and feedback time_slot is 2 */
	if (nhlt_soc_add_max98927(nhlt, AUDIO_LINK_SSP0, 0, 2))
		printk(BIOS_ERR, "Couldn't add max98927\n");

	end_addr = nhlt_soc_serialize_oem_overrides(nhlt, start_addr,
				    oem_id_maxim, oem_table_id_maxim, 0);

	if (end_addr != start_addr)
		acpi_add_table(rsdp, (void *)start_addr);

	return end_addr;
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

static void mainboard_chip_init(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
