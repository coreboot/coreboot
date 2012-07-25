/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <arch/io.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/vga_io.h>

#include "vx900.h"

#define CHROME_9_HD_MIN_FB_SIZE   8
#define CHROME_9_HD_MAX_FB_SIZE 512

/* Helper to determine the framebuffer size */
u32 chrome9hd_fb_size(void)
{
	static u32 fb_size = 0;
	/* We do some PCI and CMOS IO to find our value, so if we've already
	 * found it, save some time */
	if(fb_size != 0)
		return fb_size;
	/* FIXME: read fb_size from CMOS, but until that is implemented, start
	 * from 512MB */
	u32 sizem = 512;

	/* The minimum framebuffer size is 8MB. */
	sizem = max(sizem, CHROME_9_HD_MIN_FB_SIZE);

	const device_t mcu = dev_find_device(PCI_VENDOR_ID_VIA,
		PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	/* We have two limitations on the maximum framebuffer size:
	 * 1) (Sanity) No more that 1/4 of system RAM
	 * 2) (Hardware limitation) No larger than DRAM in last rank
	 * Check both of these limitations and apply them to our framebuffer */
	u32 tomm = (pci_read_config16(mcu, 0x88) & 0x07ff) << (24-20);
	u32 max_sizem = tomm >> 2;
	if(sizem > max_sizem) {
		printk(BIOS_ALERT, "The framebuffer size of of %dMB is larger"
			" than 1/4 of available memory.\n"
			" Limiting framebuffer to %dMB\n", sizem, max_sizem);
		sizem = max_sizem;
	}

	/* Now handle limitation #2
	 * Look at the ending address of the memory ranks, from last to first,
	 * until we find one that is not zero. That is our last rank, and its
	 * size is the limit of our framebuffer. */
	int i;
	for(i = VX900_MAX_MEM_RANKS - 1; i > -1; i--) {
		u8 reg8 = pci_read_config8(mcu, 0x40 + 1);
		if(reg8 == 0)
			continue;
		/* We've reached the last populated rank */
		u8 ranksize = reg8 - pci_read_config8(mcu, 0x48 + i);
		max_sizem = ranksize >> 6;
	};
	if(sizem > max_sizem) {
		printk(BIOS_ALERT, "The framebuffer size of of %dMB is larger"
		" than size of the last DRAM rank.\n"
		" Limiting framebuffer to %dMB\n", sizem, max_sizem);
		sizem = max_sizem;
	}

	/* Now round the framebuffer size to the closest power of 2 */
	u8 fb_pow = 0;
	while(sizem >> fb_pow) fb_pow ++;
	fb_pow --;
	sizem = (1 << fb_pow);
	/* We store the framebuffer size in bytes, for simplicity */
	fb_size = sizem << 20;
	return fb_size;
}

#if 0 && CONFIG_OPTION_ROM_RUN_YABEL
#include <x86emu/x86emu.h>
static int vx900_int15_handler_yabel(void)
{
	int res=-1;
	printk(BIOS_DEBUG, "%s %0x\n", __func__, M.x86.R_AX & 0xffff);
	switch(M.x86.R_AX & 0xffff) {
		case 0x5f19:
			M.x86.R_AX=0x5f;
			M.x86.R_CX=0x03;
			res=0;
			break;
		case 0x5f18:
		{
			/*
			 * BL Bit[7:4]
			 * Memory Data Rate
			 * 0000: 66MHz
			 * 0001: 100MHz
			 * 0010: 133MHz
			 * 0011: 200MHz ( DDR200 )
			 * 0100: 266MHz ( DDR266 )
			 * 0101: 333MHz ( DDR333 )
			 * 0110: 400MHz ( DDR400 )
			 * 0111: 533MHz ( DDR I/II 533
			 * 1000: 667MHz ( DDR I/II 667)
			 * Bit[3:0]
			 * N:  Frame Buffer Size 2^N  MB
			 */
			u8 i;
			device_t dev;
			dev = dev_find_slot(0, PCI_DEVFN(0, 3));
			i = pci_read_config8(dev, 0xa1);
			i = (i & 0x70);
			i = i >> 4;
			if (i == 0) {
				M.x86.R_AX = 0x00;	//not support 5f18
				break;
			}
			i = i + 2;
			M.x86.R_BX = (u32) i;
			i = pci_read_config8(dev, 0x90);
			i = (i & 0x07);
			i = i + 3;
			i = i << 4;
			M.x86.R_BX = M.x86.R_BX + ((u32) i);
			M.x86.R_AX = 0x5f;
			res = 0;
			break;
		}
		case 0x5f00:
			M.x86.R_AX = 0x005f;
			res = 0;
			break;
		case 0x5f01:
			M.x86.R_AX = 0x5f;
			M.x86.R_CX = (M.x86.R_CX & 0xffffff00 ) | 2; // panel type =  2 = 1024 * 768
			res = 0;
			break;
		case 0x5f02:
			M.x86.R_AX=0x5f;
			M.x86.R_BX= (M.x86.R_BX & 0xffff0000) | 2;
			M.x86.R_CX= (M.x86.R_CX & 0xffff0000) | 0x401;  // PAL + crt only
			M.x86.R_DX= (M.x86.R_DX & 0xffff0000) | 0;  // TV Layout - default
			res=0;
			break;
		case 0x5f0f:
			M.x86.R_AX = 0x005f;
			res = 0;
			break;
		default:
			printk(BIOS_DEBUG, "Unsupported INT15 call %04x!\n",
			       M.x86.R_AX & 0xffff);
			M.x86.R_AX = 0;
			break;
	}
	return res;
}

static void vx900_vga_set_int15_handler(void)
{
	printk(BIOS_DEBUG, "Our int15 handler is at %p\n",
	       &vx900_int15_handler_yabel);
	typedef int (* yabel_handleIntFunc)(void);
	extern yabel_handleIntFunc yabel_intFuncArray[256];
	yabel_intFuncArray[0x15] = vx900_int15_handler_yabel;
}
#endif

static void chrome9hd_set_sid_vid(u16 vendor, u16 device)
{
	vga_sr_write(0x36, vendor >> 8);   /* SVID high byte */
	vga_sr_write(0x35, vendor & 0xff); /* SVID low  byte */
	vga_sr_write(0x38, device >> 8);   /*  SID high byte */
	vga_sr_write(0x37, device & 0xff); /*  SID low  byte */
}

static void chrome9hd_biosguide_init_seq(device_t dev)
{
	device_t mcu = dev_find_device(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	device_t host = dev_find_device(PCI_VENDOR_ID_VIA,
					PCI_DEVICE_ID_VIA_VX900_HOST_BR, 0);
	print_debug("1\n");
	/* Step 1 - Enable VGA controller */
	/* FIXME: This is the VGA hole @ 640k-768k, and the vga port io
	 * We need the port IO, but can we disable the memory hole? */
	print_debug("1.a\n");
	pci_mod_config8(mcu, 0xa4, 0, 0x80); /* VGA memory hole */

	print_debug("2\n");
	/* Step 2 - Forward MDA cycles to GFX */
	pci_mod_config8(host, 0x4e, 1<<1, 0); /* FIXME */

	print_debug("3\n");
	/* Step 3 - Enable GFX I/O space */
	pci_mod_config8(dev, PCI_COMMAND, 0, PCI_COMMAND_IO);

	print_debug("4\n");
	/* Step 4 - Enable video subsystem */
	vga_enable_mask(1<<0, 1<<0);

	print_debug("5\n");
	/* Step 5 - Unlock accessing of IO space */
	vga_sr_write(0x10, 0x01);

	/* Step 8 - Enable memory base register on the GFX */
	if(uma_memory_base == 0)
		die("uma_memory_base not set. Abandon ship!\n");
	printk(BIOS_DEBUG, "UMA base @0x%.10llx\n", uma_memory_base);
	vga_sr_write(0x6d, (uma_memory_base >> 21) & 0xff); /* base 28:21 */
	vga_sr_write(0x6e, (uma_memory_base >> 29) & 0xff); /* base 36:29 */
	vga_sr_write(0x6f, 0x00); /* base what what in the butt ?? */

	/* Step 9 - Set SID/VID */
	chrome9hd_set_sid_vid(0x1106, 0x7122);

	/* Prime PLL FIXME: bad comment */
	//vga_sr_mask(0x3c, 1<<2, 1<<2);

	//VGA IO Address Select. 3B5 or 3D5?
	//vga_misc_mask(1<<0, 1<<0);

	//enable Base VGA 16 Bits Decode
	//pci_mod_config8(host, 0x4e, 0, 1<<4);
}

static void dump_pci_device(device_t dev)
{
	int i;
	for (i = 0; i <= 255; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0) {
			print_debug_hex8(i);
			print_debug_char(':');
		}
		val = pci_read_config8(dev, i);
		if((i & 7) == 0) print_debug(" |");
		print_debug_char(' ');
		print_debug_hex8(val);
		if ((i & 0x0f) == 0x0f) {
			print_debug("\n");
		}
	}
}

static void chrome9hd_init(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== Chrome9 HD INIT\n");
	print_debug("======================================================\n");

	chrome9hd_biosguide_init_seq(dev);

	//vx900_vga_set_int15_handler();

	u32 fb_address = pci_read_config32(dev, PCI_BASE_ADDRESS_2);
	fb_address &= ~0x0F;
	if (!fb_address) {
		printk(BIOS_WARNING, "Chrome: No FB BAR assigned!\n");
		return;
	}

	printk(BIOS_INFO, "Chrome: Using %dMB Framebuffer at 0x%08X.\n",
	       256, fb_address);

	/* Poison the framebuffer */
//	size_t i, j;
//	for(i = 0; i < 256; i++) {
//		u32 * fb = (u32*) (u32)(fb_address + i * (1<<20));
//		for(j = 0; j < (1<<20)>>2; j++) {
//			fb[j] = 0xdeadbeef;
//		}
//	}

	printk(BIOS_DEBUG, "Initializing VGA...\n");

	pci_dev_init(dev);

	printk(BIOS_DEBUG, "Enable VGA console\n");
	//vga_enable_console();

	dump_pci_device(dev);
}

static void chrome9hd_enable(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== Chrome9 HD ENABLE\n");
	print_debug("======================================================\n");

	device_t mcu = dev_find_device(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	/* FIXME: here? -=- ACLK 250Mhz */
	pci_mod_config8(mcu, 0xbb, 0, 0x01);
}

static void chrome9hd_disable(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== Chrome9 HD DISABLE\n");
	print_debug("======================================================\n");

	device_t mcu = dev_find_device(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	/* Disable GFX - This step effectively renders the GFX inert
	 * It won't even show up as a PCI device during enumeration */
	pci_mod_config8(mcu, 0xa1, 1<<7, 0);
}

static void chrome9hd_read_resources(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== Chrome9 HD READ RESOURCES\n");
	print_debug("======================================================\n");

	/* Mirror mirror, shiny glass, tell me that is not my ass */
	u32 fb_size = chrome9hd_fb_size() >> 20;

	uma_resource(dev, 0x18, uma_memory_base>>10, uma_memory_size>>10);

	u8 fb_pow = 0;
	while(fb_size >> fb_pow) fb_pow ++;
	fb_pow --;

	/* Step 6 - Let MCU know the framebuffer size */
	device_t mcu = dev_find_device(PCI_VENDOR_ID_VIA,
				       PCI_DEVICE_ID_VIA_VX900_MEMCTRL, 0);
	pci_mod_config8(mcu, 0xa1, 7<<4, (fb_pow - 2) <<4);

	/* Step 7 - Let GFX know the framebuffer size (through PCI and IOCTL)
	 * The size we set here affects the behavior of BAR2, and the amount of
	 * MMIO space it requests. The default is 512MB, so if we don't set this
	 * before reading the resources, we could waste space below 4G */
	pci_write_config8(dev, 0xb2, ((0xff << (fb_pow - 2)) & ~(1<<7)) );
	vga_sr_write(0x68, (0xff << (fb_pow - 1)) );
	/* And also that the framebuffer is in the syste, RAM */
	pci_mod_config8(dev, 0xb0, 0, 1<<0);

	pci_dev_read_resources(dev);
}

static void chrome9hd_set_resources(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== Chrome9 HD SET RESOURCES\n");
	print_debug("======================================================\n");

	pci_dev_set_resources(dev);
}

static void chrome9hd_enable_resources(device_t dev)
{
	print_debug("======================================================\n");
	print_debug("== Chrome9 HD ENABLE RESOURCES\n");
	print_debug("======================================================\n");

	pci_dev_enable_resources(dev);
}

static struct device_operations chrome9hd_operations = {
	.read_resources = chrome9hd_read_resources,
	.set_resources = chrome9hd_set_resources,
	.enable_resources = chrome9hd_enable_resources,
	.init = chrome9hd_init,
	.disable = chrome9hd_disable,
	.enable = chrome9hd_enable,
	.ops_pci = 0,
};

static const struct pci_driver chrome9hd_driver __pci_driver = {
	.ops = &chrome9hd_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX900_VGA,
};