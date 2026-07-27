/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <cpu/cpu.h>
#include <cpu/intel/cpu_ids.h>
#include <device/device.h>
#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>
#include <smbios.h>
#include <soc/gpio.h>
#include <stdint.h>
#include <stdio.h>

static void mainboard_init(void *chip_info)
{
	variant_configure_gpio_pads();
	variant_devtree_update();
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

static void mainboard_enable(struct device *dev)
{

}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};

const char *mainboard_vbt_filename(void)
{
	return "vbt.bin";
}
