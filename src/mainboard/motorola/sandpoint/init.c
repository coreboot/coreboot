/*
 * Copyright (C) 2003, Greg Watson <gwatson@lanl.gov>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * Do very early board initialization:
 *
 * - Configure External Bus (EBC)
 * - Setup Flash
 * - Setup NVRTC
 * - Setup Board Control and Status Registers (BCSR)
 * - Enable UART0 for debugging
 */

#include <ppc_asm.tmpl>
#include <ppc.h>
#include <arch/io.h>
#include <printk.h>
#include <uart8250.h>

void pnp_output(char address, char data)
{
	outb(address, CONFIG_PNP_CFGADDR);
	outb(data, CONFIG_PNP_CFGDATA);
}

void
board_init(void)
{
	/*
	 * Enable UART0
	 *
	 * NOTE: this configuration assumes that the PCI/ISA IO
	 * address space is properly configured by default on board
	 * reset. While this seems to be the case with the X3, it may not
 	 * always work.
	 */
	pnp_output(0x07, 6); /* LD 6 = UART0 */
	pnp_output(0x30, 0); /* Dectivate */
	pnp_output(0x60, CONFIG_TTYS0_BASE >> 8); /* IO Base */
	pnp_output(0x61, CONFIG_TTYS0_BASE & 0xFF); /* IO Base */
	pnp_output(0x30, 1); /* Activate */
	uart8250_init(CONFIG_TTYS0_BASE, 115200/CONFIG_TTYS0_BAUD, CONFIG_TTYS0_LCS);
}

void
board_init2(void)
{
	printk_info("Sandpoint initialized...\n");
}
