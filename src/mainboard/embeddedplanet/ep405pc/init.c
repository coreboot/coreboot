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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
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
#include <ppc4xx.h>
#include <arch/io.h>
#include <timer.h>

void
board_init(void)
{
	/*
	 * Configure FLASH
	 */
	mtebc(pb0cr, 0xFC0DC000);
	mtebc(pb0ap, 0x02000000);

	/*
	 * Configure NVTRC/BCSR
	 */
	mtebc(pb4cr, 0xF4058000);
	mtebc(pb4ap, 0x04050000);
	
	/*
	 * Enable FLASH, NVRAM, POR
	 */
	outb(0x9C, 0xF4000002);

	/*
	 * Enable UART0
	 */
	outb(0x20, 0xF4000003);

	/*
	 * Cycle LEDs to show something is happening...
	 */
	outb(0x07, 0xF4000009);
	udelay(100000);
	outb(0x0B, 0xF4000009);
	udelay(100000);
	outb(0x0D, 0xF4000009);
	udelay(100000);
	outb(0x0E, 0xF4000009);
}
