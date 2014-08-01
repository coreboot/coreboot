/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/exception.h>
#include <arch/hlt.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/apbmisc.h>

#include "pinmux.h"
#include "power.h"

static const struct pad_config uart_console_pads[] = {
	/* Hard coded pad usage for UARTA. */
	PAD_CFG_SFIO(KB_ROW9, 0, UA3),
	PAD_CFG_SFIO(KB_ROW10, PINMUX_INPUT_ENABLE | PINMUX_PULL_UP, UA3),
	/*
	 * Disable UART2 pads as they are default connected to UARTA controller.
	 */
	PAD_CFG_UNUSED(UART2_RXD),
	PAD_CFG_UNUSED(UART2_TXD),
	PAD_CFG_UNUSED(UART2_RTS_N),
	PAD_CFG_UNUSED(UART2_CTS_N),
};

void main(void)
{
	// enable pinmux clamp inputs
	clamp_tristate_inputs();

	// enable JTAG at the earliest stage
	enable_jtag();

	clock_early_uart();

	soc_configure_pads(uart_console_pads, ARRAY_SIZE(uart_console_pads));

	if (CONFIG_BOOTBLOCK_CONSOLE) {
		console_init();
		exception_init();
		printk(BIOS_INFO, "T132: Bootblock here\n");
	}

	clock_init();

	printk(BIOS_INFO, "T132 bootblock: Clock init done\n");

	bootblock_mainboard_init();

	printk(BIOS_INFO, "T132 bootblock: Mainboard bootblock init done\n");

	run_romstage();
}
