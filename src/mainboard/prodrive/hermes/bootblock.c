/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <console/console.h>
#include <gpio.h>

#include "gpio.h"

void bootblock_mainboard_early_init(void)
{
	/* This is a hack for FSP because it does things in MemoryInit()
	   which it shouldn't do. We have to prepare certain gpios here
	   because of the brokenness in FSP. */
	program_early_gpio_pads();
}

void bootblock_mainboard_init(void)
{
	u8 bmc_hsi = (gpio_get(GPP_K13) << 3) |
		(gpio_get(GPP_K12) << 2) |
		(gpio_get(GPP_K14) << 1) |
		(gpio_get(GPP_K16) << 0);

	printk(BIOS_INFO, "BMC HSI 0x%x\n", bmc_hsi);
}
