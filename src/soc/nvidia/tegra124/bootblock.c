/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <arch/hlt.h>
#include <arch/io.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>

#include "clock.h"
#include "pinmux.h"

static void hacky_hardcoded_uart_setup_function(void)
{
	// Assert UART reset and enable clock.
	setbits_le32((void *)(0x60006000 + 4 + 0), 1 << 6);

	// Enable the clock.
	setbits_le32((void *)(0x60006000 + 4 * 4 + 0), 1 << 6);

	// Set the clock source.
	clrbits_le32((void *)(0x60006000 + 0x100 + 4 * 0x1e), 3 << 30);

	udelay(2);

	// De-assert reset to UART.
	clrbits_le32((void *)(0x60006000 + 4 + 0), 1 << 6);
}

void main(void)
{
	void *entry;

	set_avp_clock_to_clkm();

	hacky_hardcoded_uart_setup_function();

	// Serial out, tristate off.
	pinmux_set_config(PINMUX_KB_ROW9_INDEX, PINMUX_KB_ROW9_FUNC_UA3);
	// Serial in, tristate_on.
	pinmux_set_config(PINMUX_KB_ROW10_INDEX, PINMUX_KB_ROW10_FUNC_UA3 |
						 PINMUX_TRISTATE |
						 PINMUX_INPUT_ENABLE);

	if (CONFIG_BOOTBLOCK_CONSOLE)
		console_init();

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/romstage");

	hlt();
}
