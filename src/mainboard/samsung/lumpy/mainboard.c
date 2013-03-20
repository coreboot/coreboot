/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <ec/smsc/mec1308/ec.h>
#include "hda_verb.h"
#include "ec.h"
#include "onboard.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);

	/* Enable EC ACPI mode for the OS before resume */
	send_ec_command(EC_SMI_DISABLE);
	send_ec_command(EC_ACPI_ENABLE);
}

#if defined(CONFIG_PCI_OPTION_ROM_RUN_REALMODE) && CONFIG_PCI_OPTION_ROM_RUN_REALMODE
static int int15_handler(void)
{
	int res=0;

	printk(BIOS_DEBUG, "%s: INT15 function %04x!\n",
			__func__, X86_EAX & 0xffff);

	switch(X86_EAX & 0xffff) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffffff00;
		X86_ECX |= 0x00;
		res = 1;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV
		 *  bit 2 = EFP *
		 *  bit 3 = LFP
		 *  bit 4 = CRT2
		 *  bit 5 = TV2
		 *  bit 6 = EFP2 *
		 *  bit 7 = LFP2
		 */
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffff0000;
		X86_ECX |= 0x0000;
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
		X86_EAX &= 0xffff0000;
		X86_EAX |= 0x005f;
		X86_ECX &= 0xffff0000;
		X86_ECX |= 0x0001;
		res = 1;
		break;
	case 0x5f70:
		switch ((X86_ECX >> 8) & 0xff) {
		case 0:
			/* Get Mux */
			X86_EAX &= 0xffff0000;
			X86_EAX |= 0x005f;
			X86_ECX &= 0xffff0000;
			X86_ECX |= 0x0000;
			res = 1;
			break;
		case 1:
			/* Set Mux */
			X86_EAX &= 0xffff0000;
			X86_EAX |= 0x005f;
			X86_ECX &= 0xffff0000;
			X86_ECX |= 0x0000;
			res = 1;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			X86_EAX &= 0xffff0000;
			X86_EAX |= 0x005f;
			X86_ECX &= 0xffff0000;
			X86_ECX |= 0x0000;
			res = 1;
			break;
		default:
			/* FIXME: Interrupt was not handled, but return success? */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
				((X86_ECX >> 8) & 0xff));
			return 1;
		}
		break;

        default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
				X86_EAX & 0xffff);
		break;
	}
	return res;
}
#endif

#if defined(CONFIG_PCI_OPTION_ROM_RUN_YABEL) && CONFIG_PCI_OPTION_ROM_RUN_YABEL
static int int15_handler(void)
{
	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
			  __func__, X86_AX, X86_BX, X86_CX, X86_DX);

	switch (X86_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		X86_AX = 0x005f;
		X86_CL = 0x00;
		break;
	case 0x5f35:
		/*
		 * Boot Display Device Hook:
		 *  bit 0 = CRT
		 *  bit 1 = TV
		 *  bit 2 = EFP *
		 *  bit 3 = LFP
		 *  bit 4 = CRT2
		 *  bit 5 = TV2
		 *  bit 6 = EFP2 *
		 *  bit 7 = LFP2
		 */
		X86_AX = 0x005f;
		X86_CX = 0x0000;
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
		X86_CX = 1;
		break;
	case 0x5f70:
		switch (X86_CH) {
		case 0:
			/* Get Mux */
			printk(BIOS_DEBUG, "Get Mux\n");
			X86_AX = 0x005f;
			X86_CL = 0;
			break;
		case 1:
			printk(BIOS_DEBUG, "Set Mux\n");
			/* Set Mux */
			X86_AX = 0x005f;
			X86_CX = 0;
			break;
		case 2:
			printk(BIOS_DEBUG, "Get SG Mode\n");
			/* Get SG/Non-SG mode */
			X86_AX = 0x005f;
			X86_CX = 0;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
				X86_CH);
			return 0;
		}
		break;
	default:
		/* Interrupt was not handled */
		printk(BIOS_DEBUG, "Unknown INT15 function: 0x%04x\n",
			X86_AX);
		return 0;
	}

	/* Interrupt handled */
	return 1;
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
	lumpy_ec_init();
}

static int lumpy_smbios_type41_irq(int *handle, unsigned long *current,
				   const char *name, u8 irq, u8 addr)
{
	struct smbios_type41 *t = (struct smbios_type41 *)*current;
	int len = sizeof(struct smbios_type41);

	memset(t, 0, sizeof(struct smbios_type41));
	t->type = SMBIOS_ONBOARD_DEVICES_EXTENDED_INFORMATION;
	t->handle = *handle;
	t->length = len - 2;
	t->reference_designation = smbios_add_string(t->eos, name);
	t->device_type = SMBIOS_DEVICE_TYPE_OTHER;
	t->device_status = 1;
	t->device_type_instance = irq;
	t->segment_group_number = 0;
	t->bus_number = addr;
	t->function_number = 0;
	t->device_number = 0;

	len = t->length + smbios_string_table_len(t->eos);
	*current += len;
	*handle += 1;
	return len;
}


static int lumpy_onboard_smbios_data(device_t dev, int *handle,
				     unsigned long *current)
{
	int len = 0;

	len += lumpy_smbios_type41_irq(handle, current,
				       LUMPY_LIGHTSENSOR_NAME,
				       LUMPY_LIGHTSENSOR_IRQ,
				       LUMPY_LIGHTSENSOR_I2C_ADDR);

	len += lumpy_smbios_type41_irq(handle, current,
				       LUMPY_TRACKPAD_NAME,
				       LUMPY_TRACKPAD_IRQ,
				       LUMPY_TRACKPAD_I2C_ADDR);

	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = lumpy_onboard_smbios_data;
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
	verb_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

