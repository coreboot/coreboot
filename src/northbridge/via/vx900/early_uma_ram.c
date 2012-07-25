/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

# include "early_vx900.h"

#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>

static void vx900_gfx_write8(u8 where, u8 what)
{
	//u16 one_step = (what << 8) | where;
	//outw(one_step, 0x3c4);
	outb(where, 0x3c4);
	outb(what, 0x3c5);
}
/*
static void vx900_gfx_mod8(u8 where, u8 clr_bits, u8 set_bits)
{
	outb(where, 0x3c4);
	u8 what = inb(0x3c5);
	what &= ~clr_bits;
	what |= set_bits;
	outb(what, 0x3c5);
}
*/
void vx900_dram_set_gfx_resources(void)
{
	printk(BIOS_SPEW, "Setting up GFX mem area \n");
	/* FIXME: enable VGA or not? */
	/* u32 fbuff_size = vga_decide_framebuffer_size();
	 * if(fbuff_size == 0) {
	 *	Do not initialize the IGP
	 * 	return;
	 * } */

	/* FIXME: decide what the framebuffer size is, don't assume it's 512M */
	u32 fb_size = 256;

	u8 fb_pow = 0;
	while(fb_size >> fb_pow) fb_pow ++;
	fb_pow --;

	/* Step 1 - Enable VGA controller */
	pci_mod_config8(MCU, 0xa1, 0, 0x80);
	/* FIXME: This is the VGA hole @ 640k-768k, and the vga port io
	 * We need the port IO, but can we disable the memory hole? */
	pci_mod_config8(MCU, 0xa4, 0, 0x80); /* VGA memory hole */

	device_t d0f0 = PCI_DEV(0,0,0);
	/* Step 2 - Forward MDA to GFX */
	pci_mod_config8(d0f0, 0x4e, 0, 1<<1);

	/* Step 3 - Turn on GFX I/O space */
	pci_mod_config8(GFX, 0x04, 0, 1<<0);

	/* Step 4 - Enable video subsystem */
	u8 io8 = inb(0x3c3);
	io8 |= 1<<0;
	outb(io8, 0x3c3);
	//3410-20-KKL-04 +S
	//VGA IO Address Select. 3B5 or 3D5?
	io8 = inb(0x03cc);
	io8 |= 0x01;
	outb(io8, 0x3c2);
	//3410-20-KKL-04 +E

	/* Step 5 - Unlock accessing of IO space */
	vx900_gfx_write8(0x10, 0x01);

	/* Step 6 - Let MCU know the framebuffer size */
	pci_mod_config8(MCU, 0xa1, 7<<4, (fb_pow - 2) <<4);
	/* Step 7 - Let GFX know the framebuffer size (through PCI and IOCTL) */
	pci_write_config8(GFX, 0xb2, ((0xff << (fb_pow - 2)) & ~(1<<7)) );
	vx900_gfx_write8(0x68, (0xff << (fb_pow - 1)) );

	/* Step 8 - Enable memory base register on the GFX */
	const u64 tom = ( pci_read_config16(MCU, 0x88) & 0x07ff ) << 24;
	const u64 fb_base = tom - (fb_size << 20);
	vx900_gfx_write8(0x6d, (fb_base >> 21) & 0xff); /* base 28:21 */
	vx900_gfx_write8(0x6e, (fb_base >> 29) & 0xff); /* base 36:29 */
	vx900_gfx_write8(0x6f, 0x00); /* base what what in the butt ?? */

	/* Step 9 - Set SID/VID */
	vx900_gfx_write8(0x36, 0x11); /* SVID high byte */
	vx900_gfx_write8(0x35, 0x06); /* SVID low  byte */
	vx900_gfx_write8(0x38, 0x71); /*  SID high byte */
	vx900_gfx_write8(0x37, 0x22); /*  SID low  byte */

	//set VGA memory selection
	pci_write_config8(GFX, 0xb0,0x01);

	//enable Base VGA 16 Bits Decode
	pci_mod_config8(d0f0, 0x4e, 0, 1<<4);

	/* That was it for the BIOS guide steps */
	/* FIXME: Do we need these canucking IO bars? */
	//pci_write_config32(GFX, 0x10, 0xf8000008);
	//pci_write_config32(GFX, 0x14, 0xfc000000);
	//pci_write_config32(GFX, 0x18, fb_base);

	/* Direct framebuffer access
	 * FIXME: framebuffer base should be decided based on TOM and size */
	/* Let the MCU know of the frambuffer base */
	//(MCU, 0xa0, 0x0ffe, 0x0200);
}