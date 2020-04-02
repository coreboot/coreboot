/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <soc/gpio.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6791d/nct6791d.h>
#include "include/gpio.h"

static void early_config_superio(void)
{
	const pnp_devfn_t serial_dev = PNP_DEV(0x2e, NCT6791D_SP1);
	nuvoton_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}

static void early_config_gpio(void)
{
	/* This is a hack for FSP because it does things in MemoryInit()
	 * which it shouldn't do. We have to prepare certain gpios here
	 * because of the brokenness in FSP. */
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

void bootblock_mainboard_init(void)
{
	early_config_gpio();
}

void bootblock_mainboard_early_init(void)
{
	early_config_superio();
}
