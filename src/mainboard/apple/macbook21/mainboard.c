/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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
#include <delay.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <northbridge/intel/i945/i945.h>
#include <pc80/mc146818rtc.h>
#include <arch/x86/include/arch/acpigen.h>
#include <smbios.h>
#include <x86emu/x86emu.h>
#define PANEL INT15_5F35_CL_DISPLAY_DEFAULT

#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
static int int15_handler(void)
{
	/* The right way to do this is to move this handler code into
	 * the mainboard or northbridge code.
	 * TODO: completely move to mainboards / chipsets.
	 */
	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
	       __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x5f35: /* Boot Display */
		X86_AX = 0x005f; // Success
		X86_CL = PANEL;
		break;
	case 0x5f40: /* Boot Panel Type */
		X86_AX = 0x005f; // Success
		X86_CL = 3;
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

int get_cst_entries(acpi_cstate_t **entries)
{
	return 0;
}

static void mainboard_init(device_t dev)
{
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

