/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Note: Some of the VGA control registers are located on the memory controller.
   Registers are set both in raminit.c and northbridge.c */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include "chip.h"
#include "northbridge.h"
#include "vgachip.h"

/* PCI Domain 1 Device 0 Function 0 */

#define SR_INDEX	0x3c4
#define SR_DATA		0x3c5
#define CRTM_INDEX	0x3b4
#define CRTM_DATA	0x3b5
#define CRTC_INDEX	0x3d4
#define CRTC_DATA	0x3d5

/* !!FIXME!! These were CONFIG_ options.  Fix it in uma_ram_setting.c too. */
#define VIACONFIG_VGA_PCI_10 0xf8000008
#define VIACONFIG_VGA_PCI_14 0xfc000000

void write_protect_vgabios(void)
{
	device_t dev;

	printk(BIOS_INFO, "write_protect_vgabios\n");
	/* there are two possible devices. Just do both. */
	dev = dev_find_device(PCI_VENDOR_ID_VIA,
			      PCI_DEVICE_ID_VIA_VX855_MEMCTRL, 0);
	if (dev)
		pci_write_config8(dev, 0x80, 0xff);
	/*vx855 no th 0x61 reg */
	/*dev = dev_find_device(PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX855_VLINK, 0);
	   //if(dev)
	   //   pci_write_config8(dev, 0x61, 0xff); */
}

extern u8 acpi_sleep_type;
static void vga_init(device_t dev)
{
	uint8_t reg8;

	print_debug("Initiailizing VGA...\n");
	u8 tmp8;
//A20 OPEN
	tmp8 = inb(0x92);
	tmp8 = tmp8 | 2;
	outb(tmp8, 0x92);

	//*
	//pci_write_config8(dev, 0x04, 0x07);
	//pci_write_config32(dev,0x10, 0xa0000008);
	//pci_write_config32(dev,0x14, 0xdd000000);
	pci_write_config32(dev, 0x10, VIACONFIG_VGA_PCI_10);
	pci_write_config32(dev, 0x14, VIACONFIG_VGA_PCI_14);
	pci_write_config8(dev, 0x3c, 0x0a);	//same with vx855_lpc.c
	//*/
	printk(BIOS_EMERG, "file '%s', line %d\n\n", __FILE__, __LINE__);

#if 1
	printk(BIOS_DEBUG, "INSTALL REAL-MODE IDT\n");
	setup_realmode_idt();
	printk(BIOS_DEBUG, "DO THE VGA BIOS\n");

	do_vgabios();
	if ((acpi_sleep_type == 3)/* || (PAYLOAD_IS_SEABIOS == 0)*/) {
		printk(BIOS_DEBUG, "Enable VGA console\n");
		// remove this function since in cn700 it is said "VGA seems to work without this, but crash & burn with it"
		//but the existense of  vga_enable_console()  seems do not hurt my coreboot. XP+ubuntu s3 can resume with and without this function.
		//and remove it also do not help my s3 problem: desktop screen have some thin black line, after resuming back to win.
		vga_enable_console();
	}
#else
/* Attempt to manually force the rom to load */
	printk(BIOS_DEBUG, "Forcing rom load\n");
	pci_rom_load(dev, 0xfff80000);
	run_bios(dev, 0xc0000);
#endif
	if ((acpi_sleep_type == 3)/* || (PAYLOAD_IS_SEABIOS == 0)*/) {
		/* It's not clear if these need to be programmed before or after
		 * the VGA bios runs. Try both, clean up later */
		/* Set memory rate to 200MHz */
		outb(0x3d, CRTM_INDEX);
		reg8 = inb(CRTM_DATA);
		reg8 &= 0x0f;
		reg8 |= (0x3 << 4);
		outb(0x3d, CRTM_INDEX);
		outb(reg8, CRTM_DATA);

		/* Set framebuffer size to CONFIG_VIDEO_MB mb */
		/*reg8 = (CONFIG_VIDEO_MB/4);
		   outb(0x39, SR_INDEX);
		   outb(reg8, SR_DATA); */
	}
	printk(BIOS_EMERG, "file '%s', line %d\n\n", __FILE__, __LINE__);

}

static struct device_operations vga_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = vga_init,
	.ops_pci = 0,
};

static const struct pci_driver vga_driver __pci_driver = {
	.ops = &vga_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VX855_VGA,
};
