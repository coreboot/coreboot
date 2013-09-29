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

static void hacky_hardcoded_uart_setup_function(void)
{
	int i;

	/*
	 * On poweron, AVP clock source (also called system clock) is set to
	 * PLLP_out0 with frequency set at 1MHz. Before initializing PLLP, we
	 * need to move the system clock's source to CLK_M temporarily. And
	 * then switch it to PLLP_out4 (204MHz) at a later time.
	 */
	write32((0 << 12) | (0 << 8) | (0 << 4) | (0 << 0) | (2 << 28),
		(void *)(0x60006000 + 0x28));

	// wait a little bit (nominally 2-3 us)
	for (i = 0; i < 0x10000; i++)
		__asm__ __volatile__("");

	// Set function.
	setbits_le32((void *)(0x70000000 + 0x3000 + 0x2e0), 3 << 0);
	setbits_le32((void *)(0x70000000 + 0x3000 + 0x2e4), 3 << 0);

	// Output.
	clrbits_le32((void *)(0x70000000 + 0x3000 + 0x2e0), 1 << 5);
	// Input.
	setbits_le32((void *)(0x70000000 + 0x3000 + 0x2e4), 1 << 5);

	// Disable tristate.
	clrbits_le32((void *)(0x70000000 + 0x3000 + 0x2e0), 1 << 4);
	clrbits_le32((void *)(0x70000000 + 0x3000 + 0x2e4), 1 << 4);

	// Assert UART reset and enable clock.
	setbits_le32((void *)(0x60006000 + 4 + 0), 1 << 6);

	// Enable the clock.
	setbits_le32((void *)(0x60006000 + 4 * 4 + 0), 1 << 6);

	// Set the clock source.
	clrbits_le32((void *)(0x60006000 + 0x100 + 4 * 0x1e), 3 << 30);

	// wait a little bit (nominally 2us?)
	for (i = 0; i < 0x10000; i++)
		__asm__ __volatile__("");

	// De-assert reset to UART.
	clrbits_le32((void *)(0x60006000 + 4 + 0), 1 << 6);
}

void main(void)
{
	void *entry;

	hacky_hardcoded_uart_setup_function();

	if (CONFIG_BOOTBLOCK_CONSOLE)
		console_init();

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/romstage");

	hlt();
}
