/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
#include <x86emu/x86emu.h>
#endif
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include "hda_verb.h"
#include "onboard.h"
#include "ec.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <device/pci.h>
#include <ec/google/chromeec/ec.h>

/* placeholder for evenual link post. Not sure what we'll
 * do but it will look nice
 */
void mainboard_post(u8 value)
{
	/*
	 * What you DO NOT want to do: push every post to the EC backlight.
	 * it seems cute but in practice it looks like a hardware failure.
	 * I'm leaving this here so we don't make this mistake again later.
	 * And it seems to break the SMP startup.
	 * google_chromeec_post(value);
	 */
}

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);
}

#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
static int int15_handler(void)
{
	int res = 0;

	printk(BIOS_DEBUG, "%s: INT15 function %04x!\n",
			__func__, X86_AX);

	switch(X86_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		X86_AX = 0x005f;
		X86_CL = 0x00; /* Use video bios default */
		res = 1;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV (eDP)
		 *  bit 2 = EFP
		 *  bit 3 = LFP
		 *  bit 4 = CRT2
		 *  bit 5 = TV2 (eDP)
		 *  bit 6 = EFP2
		 *  bit 7 = LFP2
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0000; /* Use video bios default */
		res = 1;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0003; /* eDP */
		res = 1;
		break;
	case 0x5f70:
		switch (X86_CH) {
		case 0:
			/* Get Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 1:
			/* Set Mux */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			X86_AX = 0x005f;
			X86_CX = 0x0000;
			res = 1;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
				X86_CH);
			break;
		}
		break;
	case 0x5fac:
		res = 1;
		break;
        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", X86_AX);
		break;
	}
	return res;
}
#endif

/* Audio Setup */

extern const u32 * cim_verb_data;
extern u32 cim_verb_data_size;
extern const u32 * pc_beep_verbs;
extern u32 pc_beep_verbs_size;

static void verb_setup(void)
{
	cim_verb_data = mainboard_cim_verb_data;
	cim_verb_data_size = sizeof(mainboard_cim_verb_data);
	pc_beep_verbs = mainboard_pc_beep_verbs;
	pc_beep_verbs_size = mainboard_pc_beep_verbs_size;
}

static void mainboard_init(device_t dev)
{
	/* Initialize the Embedded Controller */
	link_ec_init();

	if (google_chromeec_get_board_version() == 0) {
		/* If running on proto1 - enable reversion of gpio11. */
		u32 gpio_inv;
		u16 gpio_base = pci_read_config16
			(dev_find_slot(0, PCI_DEVFN(0x1f, 0)), GPIO_BASE) &
			0xfffc;
		u16 gpio_inv_addr = gpio_base + GPI_INV;
		gpio_inv = inl(gpio_inv_addr);
		outl(gpio_inv | (1 << 11), gpio_inv_addr);
	}
}

static int link_onboard_smbios_data(device_t dev, int *handle,
				     unsigned long *current)
{
	int len = 0;

	len += smbios_write_type41(
		current, handle,
		LINK_LIGHTSENSOR_NAME,		/* name */
		LINK_LIGHTSENSOR_IRQ,		/* instance */
		0,				/* segment */
		LINK_LIGHTSENSOR_I2C_ADDR,	/* bus */
		0,				/* device */
		0);				/* function */

	len += smbios_write_type41(
		current, handle,
		LINK_TRACKPAD_NAME,		/* name */
		LINK_TRACKPAD_IRQ,		/* instance */
		0,				/* segment */
		LINK_TRACKPAD_I2C_ADDR,		/* bus */
		0,				/* device */
		0);				/* function */

	len += smbios_write_type41(
		current, handle,
		LINK_TOUCHSCREEN_NAME,		/* name */
		LINK_TOUCHSCREEN_IRQ,		/* instance */
		0,				/* segment */
		LINK_TOUCHSCREEN_I2C_ADDR,	/* bus */
		0,				/* device */
		0);				/* function */

	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = link_onboard_smbios_data;
#if CONFIG_PCI_ROM_RUN || CONFIG_VGA_ROM_RUN
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
	verb_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
