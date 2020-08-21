/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <intelblocks/pcr.h>
#include <soc/gpio.h>
#include <soc/pcr_ids.h>
#include <smbios.h>
#include <vendorcode/google/chromeos/chromeos.h>

#define SERIAL_IO_PCR_GPPRVRW4	0x60C

static void mainboard_init(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);

	if (CONFIG(DRIVERS_INTEL_MIPI_CAMERA))
		pcr_write32(PID_SERIALIO, SERIAL_IO_PCR_GPPRVRW4, BIT8);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
}

const char *smbios_system_sku(void)
{
	static const char *sku_str = "sku2147483647"; /* sku{0-1} */
	return sku_str;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
