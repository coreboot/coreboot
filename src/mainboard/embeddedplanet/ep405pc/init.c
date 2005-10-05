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
#include <ppc4xx.h>
#include <arch/io.h>
#include <timer.h>

void
board_init(void)
{
	/*
	 * Configure Inerrupt Controller
	 */
        mtdcr(uicsr, 0xFFFFFFFF);      /* clear all ints */
        mtdcr(uicer, 0x00000000);      /* disable all ints */
        mtdcr(uiccr, 0x00000000);      /* set all to be non-critical */
        mtdcr(uicpr, 0xFFFFFF80);      /* set int polarities */
        mtdcr(uictr, 0x10000000);      /* set int trigger levels */
        mtdcr(uicvcr, 0x00000001);     /* set vect base=0,INT0 highest pri */
        mtdcr(uicsr, 0xFFFFFFFF);      /* clear all ints */

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
	 * Board Control and Status Register (BCSR) setup
	 */

	/*
	 * BCSR1 - PCI Control
	 */
	out_8((unsigned char *)0xF4000001, 0x80);

	/*
	 * BCSR2 - FLASH, NVRAM and POR Control
	 */
	out_8((unsigned char *)0xF4000002, 0x9C);

	/*
	 * BCSR3 - FENET and UART
	 */
	out_8((unsigned char *)0xF4000003, 0xf0);

	/*
	 * BCSR4 - PCI Status and Masking
	 */
	out_8((unsigned char *)0xF4000004, 0x00);

	/*
	 * BCSR5 - XIRQ Select
	 */
	out_8((unsigned char *)0xF4000005, 0x00);

	/*
	 * BCSR6 - XIRQ Routing
	 */
	out_8((unsigned char *)0xF4000006, 0x07);

	/*
	 * Cycle LEDs to show something is happening...
	 */
	out_8((unsigned char *)0xF4000009, 0x07);
	udelay(100000);
	out_8((unsigned char *)0xF4000009, 0x0B);
	udelay(100000);
	out_8((unsigned char *)0xF4000009, 0x0D);
	udelay(100000);
	out_8((unsigned char *)0xF4000009, 0x0E);
}

void
board_init2(void)
{
}
