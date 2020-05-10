/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/gpio.h>
#include <variant/gpio.h>
#include <superio/aspeed/common/aspeed.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <delay.h>
#include <console/uart.h>

static void early_config_gpio(void)
{
	/* This is a hack for FSP because it does things in MemoryInit()
	 * which it shouldn't do. We have to prepare certain gpios here
	 * because of the brokenness in FSP. */
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

static void early_config_superio(void)
{
	const pnp_devfn_t serial_dev = PNP_DEV(0x2e, AST2400_SUART1);
	aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_early_init(void)
{
	early_config_gpio();
	early_config_superio();
}
