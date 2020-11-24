/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <mainboard/gpio.h>
#include <superio/aspeed/common/aspeed.h>
#include <superio/aspeed/ast2400/ast2400.h>
#include <console/uart.h>

static void early_config_superio(void)
{
	const pnp_devfn_t serial_dev = PNP_DEV(0x2e, AST2400_SUART1);
	aspeed_enable_serial(serial_dev, CONFIG_TTYS0_BASE);
}

void bootblock_mainboard_early_init(void)
{
	mainboard_configure_early_gpios();
	early_config_superio();
}
