/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <device/device.h>
#include <drivers/intel/gma/opregion.h>
#include <soc/ramstage.h>
#include <variants.h>

static void starlabs_configure_mainboard(void *unused)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_ENTRY, starlabs_configure_mainboard, NULL);

void __weak starlabs_adl_mainboard_fill_ssdt(const struct device *dev)
{
	(void)dev;
}

static void enable_mainboard(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = starlabs_adl_mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.enable_dev = enable_mainboard,
};
