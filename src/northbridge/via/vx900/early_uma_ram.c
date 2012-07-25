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
#include <cbfs.h>
#include <device/pci_def.h>
#include <stddef.h>

/* These are mainly arbitration values for the MCU to arbitrate bandwidth
 * between CPU requests and GFX requests. They are set such that the GFX will
 * get more bandwidth when needed (i.e. 3D graphics)
 * If the GFX is idle, the CPU will still be able to use all available memory
 * bandwidth. */
static pci_reg8 mcu_vga_arbiter_config[] = {
	{0xa2, 0xb4}, /* Arbitration - GFX */
	{0xa6, 0x01}, /* Enable 8QW request merging from GFX*/
	{0xb0, 0xb8}, /* Arbitration - Video decoder */
	{0xb1, 0x22}, /* Enable 8QW request merging from Video decoder */
	{0xb8, 0x11}, /* Priority adjustemnt for Video Decoder */
	{0xb9, 0x11}, /* Arbitration - Video Decoder Queue -  I */
	{0xba, 0x11}, /* Arbitration - Video Decoder Queue - II */
};

static void vx900_gfx_dram_arbitration(void)
{
	size_t i;
	for(i = 0; i < (sizeof(mcu_vga_arbiter_config)/sizeof(pci_reg8)); i++)
	{
		pci_write_config8(MCU, mcu_vga_arbiter_config[i].addr,
				  mcu_vga_arbiter_config[i].val);
	}
}

void vx900_dram_set_gfx_resources(void)
{
	printk(BIOS_SPEW, "Setting up GFX mem area \n");
	/* FIXME: enable VGA or not? */
	/* u32 fbuff_size = vga_decide_framebuffer_size();
	 * if(fbuff_size == 0) {
	 *	Do not initialize the IGP
	 * 	return;
	 * } */


	/* Step 1 - Enable VGA controller */
//	pci_mod_config8(MCU, 0xa1, 0, 0x80);
	/* FIXME: This is the VGA hole @ 640k-768k, and the vga port io
	 * We need the port IO, but can we disable the memory hole? */
//	pci_mod_config8(MCU, 0xa4, 0, 0x80); /* VGA memory hole */

	//device_t d0f0 = PCI_DEV(0,0,0);
	/* Step 2 - Forward MDA to GFX */
//	pci_mod_config8(d0f0, 0x4e, 1<<1, 0); /* FIXME */

	/* Step 3 - Enable GFX I/O space */
//	pci_mod_config8(GFX, PCI_COMMAND, 0, PCI_COMMAND_IO);

	/* Step 4 - Enable video subsystem */
//	u8 io8 = inb(0x3c3);
//	io8 |= 1<<0;
//	outb(io8, 0x3c3);
	//3410-20-KKL-04 +S

	/* Step 5 - Unlock accessing of IO space */
//	vx900_gfx_write8(0x10, 0x01);

	/* Prime PLL FIXME: bad comment */
	//vx900_gfx_mod8(0x3c, 0, 1<<2);
	//vx900_gfx_mod8(0x3c, 0, 0);

	//VGA IO Address Select. 3B5 or 3D5?
	u8 io8 = inb(0x03cc);
	io8 |= 0x01;
	outb(io8, 0x3c2);
	//3410-20-KKL-04 +E

	/* Bandwidth, bandwidth, bandwidth */
	vx900_gfx_dram_arbitration();

	/* FIXME: here? -=- ACLK 250Mhz */
	pci_mod_config8(MCU, 0xbb, 0, 0x01);

	/* Step 6 - Let MCU know the framebuffer size */
//	pci_mod_config8(MCU, 0xa1, 7<<4, (fb_pow - 2) <<4);
	/* Step 7 - Let GFX know the framebuffer size (through PCI and IOCTL) */
//	pci_write_config8(GFX, 0xb2, ((0xff << (fb_pow - 2)) & ~(1<<7)) );
//	vx900_gfx_write8(0x68, (0xff << (fb_pow - 1)) );

	/* Step 8 - Enable memory base register on the GFX */
//	const u64 tom = ( pci_read_config16(MCU, 0x88) & 0x07ff ) << 24;
//	const u64 fb_base = tom - (fb_size << 20);
//	vx900_gfx_write8(0x6d, (fb_base >> 21) & 0xff); /* base 28:21 */
//	vx900_gfx_write8(0x6e, (fb_base >> 29) & 0xff); /* base 36:29 */
//	vx900_gfx_write8(0x6f, 0x00); /* base what what in the butt ?? */

	/* Step 9 - Set SID/VID */
//	vx900_gfx_set_sid_vid(0x1106, 0x7122);

	/* VGA framebuffer is system-local (in system RAM) */
//	pci_write_config8(GFX, 0xb0, 0x01);

	//enable Base VGA 16 Bits Decode
	//pci_mod_config8(HOST_CTR, 0x4e, 0, 1<<4);

	/* That was it for the BIOS guide steps */
	/* FIXME: Do we need these canucking IO bars? */
	//pci_write_config32(GFX, PCI_BASE_ADDRESS_0, 0xf8000008);
	//pci_write_config32(GFX, PCI_BASE_ADDRESS_1, 0xfc000000);
	//pci_write_config32(GFX, PCI_BASE_ADDRESS_2, fb_base | 0x08);

	/* Direct framebuffer access
	 * FIXME: framebuffer base should be decided based on TOM and size */
	/* Let the MCU know of the frambuffer base, for direct access by CPU */
	//(MCU, 0xa0, 0x0ffe, 0x0200);
}