/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <delay.h>
#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
#include <x86emu/x86emu.h>
#endif

#include <ec/acpi/ec.h>
#include "m3885.h"

#define DUMP_RUNTIME_REGISTERS 0

static void backlight_enable(void)
{
#if 0
// Disabled, don't let the X9511 burn out
	int i;

	/* P56 is Brightness Up, and it needs a Pulse instead of a
	 * Level
	 */
	for (i=0; i < 28; i++) {
		//m3885_gpio(M3885_GPIO_PULSE|M3885_GPIO_SET|M3885_GPIO_P56);
		m3885_gpio(M3885_GPIO_PULSE|M3885_GPIO_TOGGLE|M3885_GPIO_P56);
	}
#endif
	printk(BIOS_DEBUG, "Display I/O: 0x%02x\n", inb(0x60f));
}

#define BOOT_DISPLAY_DEFAULT	0
#define BOOT_DISPLAY_CRT	(1 << 0)
#define BOOT_DISPLAY_TV		(1 << 1)
#define BOOT_DISPLAY_EFP	(1 << 2)
#define BOOT_DISPLAY_LCD	(1 << 3)
#define BOOT_DISPLAY_CRT2	(1 << 4)
#define BOOT_DISPLAY_TV2	(1 << 5)
#define BOOT_DISPLAY_EFP2	(1 << 6)
#define BOOT_DISPLAY_LCD2	(1 << 7)

#if CONFIG_VGA_ROM_RUN
static int int15_handler(void)
{
	/* This int15 handler is Intel IGD. specific. Other chipsets need other
	 * handlers. The right way to do this is to move this handler code into
	 * the mainboard or northbridge code.
	 * TODO: completely move to mainboards / chipsets.
	 */
	u8 display_id;

	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x5f35: /* Boot Display */
		X86_AX = 0x005f; // Success
		X86_CL = BOOT_DISPLAY_DEFAULT;
		break;
	case 0x5f40: /* Boot Panel Type */
		/* LCD panel type is SIO GPIO40-43 */
		// display_id = inb(0x60f) & 0x0f;
		display_id = 3;
		// M.x86.R_AX = 0x015f; // Supported but failed
		X86_AX = 0x005f; // Success
		X86_CL = display_id;
		printk(BIOS_DEBUG, "DISPLAY=%x\n", X86_CL);
		break;
	default:
		/* Interrupt was not handled */
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", X86_AX);
		return 0;
	}

	/* Interrupt handled */
	return 1;
}
#endif

#if DUMP_RUNTIME_REGISTERS
static void dump_runtime_registers(void)
{
	int i;

	printk(BIOS_DEBUG, "SuperIO runtime register block:\n");
	for (i=0; i<0x10; i++)
		printk(BIOS_DEBUG, "%02x ", i);
	printk(BIOS_DEBUG, "\n");
	for (i=0; i<0x10; i++)
		printk(BIOS_DEBUG, "%02x ", inb(0x600 +i));
	printk(BIOS_DEBUG, "\n");
}
#endif

static void mainboard_enable(device_t dev)
{
	/* Configure the MultiKey controller */
	// m3885_configure_multikey();

	/* Enable LCD Backlight */
	backlight_enable();

	/* Disable Dummy DCC -> GP45 = 1 */
	outb(inb(0x60f) | (1 << 5), 0x60f);

#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
#if DUMP_RUNTIME_REGISTERS
	dump_runtime_registers();
#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

