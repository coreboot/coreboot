/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include "gpio.h"

static void init_mainboard(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
};

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// Don't remap RootPorts
	params->PcieRpFunctionSwap = 0;
	params->CpuPcieRpFunctionSwap = 0;
}
