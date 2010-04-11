/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2010 Joseph Smith <joe@settoplinux.org>
 * Copyright (C) 2010 Stefan Reinauer <stepan@openbios.org>
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

#include <console/console.h>
#include <device/device.h>
#include <boot/tables.h>
#include <delay.h>
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
#include <x86emu/x86emu.h>
#endif
#include <arch/coreboot_tables.h>
#include <arch/io.h>
#include "chip.h"

int add_mainboard_resources(struct lb_memory *mem)
{
	return add_northbridge_resources(mem);
}

// setting the bit disables the led.
#define PARPORT_GPIO_LED_GREEN	(1 << 0)
#define PARPORT_GPIO_LED_ORANGE	(1 << 1)
#define PARPORT_GPIO_LED_RED	(1 << 2)
#define PARPORT_GPIO_IR_PORT	(1 << 6)

static u8 get_parport_gpio(void)
{
	return inb(0x378);
}

static void set_parport_gpio(u8 gpios)
{
	outb(gpios, 0x378);
}

static void parport_gpios(void)
{
	u8 pp_gpios = get_parport_gpio();

	/* disable red led */
	pp_gpios |= PARPORT_GPIO_LED_RED;
	set_parport_gpio(pp_gpios);

	pp_gpios = get_parport_gpio();

	printk(BIOS_DEBUG, "IP1000 GPIOs:\n");
	printk(BIOS_DEBUG, "  GPIO mask:  %02x\n", pp_gpios);
	printk(BIOS_DEBUG, "  green led:  %s\n", 
			(pp_gpios & PARPORT_GPIO_LED_GREEN) ? "off" : "on");
	printk(BIOS_DEBUG, "  orange led: %s\n", 
			(pp_gpios & PARPORT_GPIO_LED_ORANGE) ? "off" : "on");
	printk(BIOS_DEBUG, "  red led:    %s\n", 
			(pp_gpios & PARPORT_GPIO_LED_RED) ? "off" : "on");
	printk(BIOS_DEBUG, "  IR port:    %s\n", 
			(pp_gpios & PARPORT_GPIO_IR_PORT) ? "off" : "on");
}

static void flash_gpios(void)
{
	u32 flash_base = 0xfff80000;
	u8 manufacturer_id, device_id;

	// reset mode
	write8(flash_base, 0xff);
	udelay(10);
	// read id
	write8(flash_base, 0x90);
	udelay(10);
	manufacturer_id = read8(flash_base);
	device_id = read8(flash_base + 1);
	// reset mode
	write8(flash_base, 0xff);
	udelay(10);

	if ((manufacturer_id == 0x20) && 
		((device_id == 0x2c) || (device_id == 0x2d))) {
		printk(BIOS_DEBUG, "Detected ST M50FW0%c0 flash:\n",
				(device_id==0x2c)?'4':'8');
		u8 fgpi = read8(0xffbc0100);
		printk(BIOS_DEBUG, "  FGPI0 [%c] FGPI1 [%c] FGPI2 [%c] FGPI3 [%c] FGPI4 [%c]",
			(fgpi & (1 << 0)) ? 'X' : ' ',
			(fgpi & (1 << 1)) ? 'X' : ' ',
			(fgpi & (1 << 2)) ? 'X' : ' ',
			(fgpi & (1 << 3)) ? 'X' : ' ',
			(fgpi & (1 << 4)) ? 'X' : ' ');
	} else {
		printk(BIOS_DEBUG, "No ST M50FW040/M50FW080 flash, don't read FGPI.\n");
	}
}


#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
static int int15_handler(void)
{
	u8 display_id;
#define BOOT_DISPLAY_DEFAULT	0
#define BOOT_DISPLAY_CRT	(1 << 0)
#define BOOT_DISPLAY_TV		(1 << 1)
#define BOOT_DISPLAY_EFP	(1 << 2)
#define BOOT_DISPLAY_LCD	(1 << 3)
#define BOOT_DISPLAY_CRT2	(1 << 4)
#define BOOT_DISPLAY_TV2	(1 << 5)
#define BOOT_DISPLAY_EFP2	(1 << 6)
#define BOOT_DISPLAY_LCD2	(1 << 7)

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, M.x86.R_AX, M.x86.R_BX, M.x86.R_CX, M.x86.R_DX);

	switch (M.x86.R_AX) {
	case 0x5f35: /* Boot Display */
		M.x86.R_AX = 0x005f; // Success
		// M.x86.R_CL = BOOT_DISPLAY_TV;
		M.x86.R_CL = BOOT_DISPLAY_DEFAULT;
		break;
	case 0x5f36: /* Boot TV Format Hook */
		printk(BIOS_DEBUG, "Boot TV Format Hook. TODO\n");
		/* Interrupt was not handled */
		return 0;
	case 0x5f31: /* Post Completion Hook */
	case 0x5f33: /* Hook After Mode Set */
	case 0x5f34: /* Set Panel Fitting Hook */
	case 0x5f38: /* Hook Before Mode Set */
	case 0x5f45: /* Hook Before VESA VBE/DDC */
	case 0x5f46: /* Hook Before VESA VBE/PM */
	case 0x5f47: /* Notif Display Switch Hook */
	case 0x5f65: /* Local Memory Initialization Hook */
		/* Interrupt was not handled */
		return 0;
	default:
		/* Interrupt was not handled */
		return 0;
	}

	/* Interrupt handled */
	return 1;
}

static void int15_install(void)
{
	typedef int (* yabel_handleIntFunc)(void);
	extern yabel_handleIntFunc yabel_intFuncArray[256];
	yabel_intFuncArray[0x15] = int15_handler;
}
#endif

static void mainboard_init(device_t dev)
{
	parport_gpios();
	flash_gpios();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
	/* Install custom int15 handler for VGA OPROM */
	int15_install();
#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	CHIP_NAME("THOMSON IP1000 Mainboard")
};
