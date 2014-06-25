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
#include <cbfs.h>
#include <console/console.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <arch/stages.h>

#include "pinmux.h"
#include "power.h"

void main(void)
{
	void *entry;

	// enable pinmux clamp inputs
	clamp_tristate_inputs();

	// enable JTAG at the earliest stage
	enable_jtag();

	clock_early_uart();

	// Serial out, tristate off.
	pinmux_set_config(PINMUX_KB_ROW9_INDEX, PINMUX_KB_ROW9_FUNC_UA3);
	// Serial in, tristate_on.
	pinmux_set_config(PINMUX_KB_ROW10_INDEX, PINMUX_KB_ROW10_FUNC_UA3 |
						 PINMUX_PULL_UP |
						 PINMUX_INPUT_ENABLE);
	// Mux some pins away from uart A.
	pinmux_set_config(PINMUX_UART2_CTS_N_INDEX,
			  PINMUX_UART2_CTS_N_FUNC_UB3 |
			  PINMUX_INPUT_ENABLE);
	pinmux_set_config(PINMUX_UART2_RTS_N_INDEX,
			  PINMUX_UART2_RTS_N_FUNC_UB3);

	if (CONFIG_BOOTBLOCK_CONSOLE) {
		console_init();
		exception_init();
		printk(BIOS_INFO, "T132: Bootblock here\n");
	}

	clock_init();

	printk(BIOS_INFO, "T132 bootblock: Clock init done\n");

	bootblock_mainboard_init();

	printk(BIOS_INFO, "T132 bootblock: Mainboard bootblock init done\n");

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/romstage");

	if (entry) {
		printk(BIOS_INFO, "T132 bootblock: jumping to romstage\n");
		stage_exit(entry);
	} else {
		printk(BIOS_INFO, "T132 bootblock: fallback/romstage not found\n");
	}

	hlt();
}
