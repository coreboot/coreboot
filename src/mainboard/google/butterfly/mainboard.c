/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011-2012 The ChromiumOS Authors.  All rights reserved.
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
#if defined(CONFIG_PCI_OPTION_ROM_RUN_YABEL) && CONFIG_PCI_OPTION_ROM_RUN_YABEL
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
#include <ec/quanta/ene_kb3940q/ec.h>
#include <vendorcode/google/chromeos/fmap.h>

static unsigned int search(char *p, char *a, unsigned int lengthp,
			   unsigned int lengtha)
{
	int i, j;

	/* Searching */
	for (j = 0; j <= lengtha - lengthp; j++) {
		for (i = 0; i < lengthp && p[i] == a[i + j]; i++) ;
		if (i >= lengthp)
			return j;
	}
	return lengtha;
}

static unsigned char get_hex_digit(char *offset)
{
	unsigned char retval = 0;

	retval = *offset - '0';
	if (retval > 0x09) {
		retval = *offset - 'A' + 0x0A;
		if (retval > 0x0F)
			retval = *offset - 'a' + 0x0a;
	}
	if (retval > 0x0F) {
		printk(BIOS_DEBUG, "Error: Invalid Hex digit found: %c - 0x%02x\n",
			*offset, (unsigned char)*offset);
		retval = 0;
	}

	return retval;
}

static int get_mac_address(u32 *high_dword, u32 *low_dword,
			   u32 search_address, u32 search_length)
{
	char key[] = "ethernet_mac";
	unsigned int offset;
	int i;

	offset = search(key, (char *)search_address,
			sizeof(key) - 1, search_length);
	if (offset == search_length) {
		printk(BIOS_DEBUG,
		       "Error: Could not locate '%s' in VPD\n", key);
		return 0;
	}
	printk(BIOS_DEBUG, "Located '%s' in VPD\n", key);

	offset += sizeof(key);	/* move to next character */
	*high_dword = 0;

	/* Fetch the MAC address and put the octets in the correct order to
	 * be programmed.
	 *
	 * From RTL8105E_Series_EEPROM-Less_App_Note_1.1
	 * If the MAC address is 001122334455h:
	 * Write 33221100h to I/O register offset 0x00 via double word access
	 * Write 00005544h to I/O register offset 0x04 via double word access
	 */

	for (i = 0; i < 4; i++) {
		*high_dword |= (get_hex_digit((char *)(search_address + offset))
				<< (4 + (i * 8)));
		*high_dword |= (get_hex_digit((char *)(search_address + offset + 1))
				<< (i * 8));
		offset += 3;
	}

	*low_dword = 0;
	for (i = 0; i < 2; i++) {
		*low_dword |= (get_hex_digit((char *)(search_address + offset))
			       << (4 + (i * 8)));
		*low_dword |= (get_hex_digit((char *)(search_address + offset + 1))
			       << (i * 8));
		offset += 3;
	}

	return *high_dword | *low_dword;
}

static void program_mac_address(u16 io_base, u32 search_address,
				u32 search_length)
{
	/* Default MAC Address of A0:00:BA:D0:0B:AD */
	u32 high_dword = 0xD0BA00A0;	/* high dword of mac address */
	u32 low_dword = 0x0000AD0B;	/* low word of mac address as a dword */

	if (search_length != -1)
		get_mac_address(&high_dword, &low_dword, search_address,
				search_length);

	if (io_base) {
		printk(BIOS_DEBUG, "Realtek NIC io_base = 0x%04x\n", io_base);
		printk(BIOS_DEBUG, "Programming MAC Address\n");

		outb(0xc0, io_base + 0x50);	/* Disable register protection */
		outl(high_dword, io_base);
		outl(low_dword, io_base + 0x04);
		outb(0x60, io_base + 54);
		outb(0x00, io_base + 0x50);	/* Enable register protection again */
	}
}

static void program_keyboard_type(u32 search_address, u32 search_length)
{
	char key[] = "keyboard_layout";
	char kbd_jpn[] = "xkb:jp::jpn";
	unsigned int offset;
	char kbd_type = EC_KBD_EN;	/* Default keyboard type is English */

	if (search_length != -1) {

		/*
		 * Search for keyboard_layout identifier
		 * The only options in the EC are Japanese or English.
		 * The English keyboard layout is actually used for multiple
		 * different languages - English, Spanish, French...  Because
		 * of this the code only searches for Japanese, and sets the
		 * keyboard type to English if Japanese is not found.
		 */
		offset = search(key, (char *)search_address, sizeof(key) - 1,
				search_length);
		if (offset != search_length) {
			printk(BIOS_DEBUG, "Located '%s' in VPD\n", key);

			offset += sizeof(key);	/* move to next character */
			search_length = sizeof(kbd_jpn);
			offset = search(kbd_jpn, (char *)(search_address + offset),
					sizeof(kbd_jpn) - 1, search_length);
			if (offset != search_length)
				kbd_type = EC_KBD_JP;
		}
	} else
		printk(BIOS_DEBUG, "Error: Could not locate VPD area\n");


	printk(BIOS_DEBUG, "Setting Keyboard type in EC to ");
	printk(BIOS_DEBUG, (kbd_type == EC_KBD_JP) ? "Japanese" : "English");
	printk(BIOS_DEBUG, ".\n");

	ec_mem_write(EC_KBID_REG, kbd_type);
}

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	outb(0xcb, 0xb2);
}

#if defined(CONFIG_PCI_OPTION_ROM_RUN_REALMODE) && CONFIG_PCI_OPTION_ROM_RUN_REALMODE
static int int15_handler(struct eregs *regs)
{
	int res = -1;

	printk(BIOS_DEBUG, "%s: INT15 function %04x!\n",
	       __func__, regs->eax & 0xffff);

	switch (regs->eax & 0xffff) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 *  0     = video bios default
		 */
		regs->eax &= 0xffff0000;
		regs->eax |= 0x005f;
		regs->ecx &= 0xffffff00;
		regs->ecx |= 0x00;	/* Use video bios default */
		res = 0;
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
		regs->eax &= 0xffff0000;
		regs->eax |= 0x005f;
		regs->ecx &= 0xffff0000;
		regs->ecx |= 0x0000;	/* Use video bios default */
		res = 0;
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		regs->eax &= 0xffff0000;
		regs->eax |= 0x005f;
		regs->ecx &= 0xffff0000;
		regs->ecx |= 0x0001;	/* Int-LVDS */
		res = 0;
		break;
	case 0x5f70:
		switch ((regs->ecx >> 8) & 0xff) {
		case 0:
			/* Get Mux */
			regs->eax &= 0xffff0000;
			regs->eax |= 0x005f;
			regs->ecx &= 0xffff0000;
			regs->ecx |= 0x0000;
			res = 0;
			break;
		case 1:
			/* Set Mux */
			regs->eax &= 0xffff0000;
			regs->eax |= 0x005f;
			regs->ecx &= 0xffff0000;
			regs->ecx |= 0x0000;
			res = 0;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			regs->eax &= 0xffff0000;
			regs->eax |= 0x005f;
			regs->ecx &= 0xffff0000;
			regs->ecx |= 0x0000;
			res = 0;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
			       ((regs->ecx >> 8) & 0xff));
			return 0;
		}
		break;

	default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
		       regs->eax & 0xffff);
		break;
	}
	return res;
}
#endif

#if defined(CONFIG_PCI_OPTION_ROM_RUN_YABEL) && CONFIG_PCI_OPTION_ROM_RUN_YABEL
static int int15_handler(void)
{
	printk(BIOS_DEBUG, "%s: AX=%04x BX=%04x CX=%04x DX=%04x\n",
	       __func__, M.x86.R_AX, M.x86.R_BX, M.x86.R_CX, M.x86.R_DX);

	switch (M.x86.R_AX) {
	case 0x5f34:
		/*
		 * Set Panel Fitting Hook:
		 *  bit 2 = Graphics Stretching
		 *  bit 1 = Text Stretching
		 *  bit 0 = Centering (do not set with bit1 or bit2)
		 */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 0x00;
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
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 0x0000;	/* Use video bios default */
		break;
	case 0x5f51:
		/*
		 * Hook to select active LFP configuration:
		 *  00h = No LVDS, VBIOS does not enable LVDS
		 *  01h = Int-LVDS, LFP driven by integrated LVDS decoder
		 *  02h = SVDO-LVDS, LFP driven by SVDO decoder
		 *  03h = eDP, LFP Driven by Int-DisplayPort encoder
		 */
		M.x86.R_AX = 0x005f;
		M.x86.R_CX = 1;	/* Int-LVDS */
		break;
	case 0x5f70:
		switch (M.x86.R_CH) {
		case 0:
			/* Get Mux */
			M.x86.R_AX = 0x005f;
			M.x86.R_CL = 0;
			break;
		case 1:
			/* Set Mux */
			M.x86.R_AX = 0x005f;
			M.x86.R_CX = 0;
			break;
		case 2:
			/* Get SG/Non-SG mode */
			M.x86.R_AX = 0x005f;
			M.x86.R_CX = 0;
			break;
		default:
			/* Interrupt was not handled */
			printk(BIOS_DEBUG, "Unknown INT15 5f70 function: 0x%02x\n",
			       M.x86.R_CH);
			return 0;
		}
		break;
	default:
		/* Interrupt was not handled */
		printk(BIOS_DEBUG, "Unknown INT15 function: 0x%04x\n",
		       M.x86.R_AX);
		return 0;
	}

	/* Interrupt handled */
	return 1;
}
#endif

#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
static void int15_install(void)
{
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL
	typedef int (*yabel_handleIntFunc)(void);
	extern yabel_handleIntFunc yabel_intFuncArray[256];
	yabel_intFuncArray[0x15] = int15_handler;
#endif
#ifdef CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif
}
#endif

/* Audio Setup */

extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;
extern const u32 *pc_beep_verbs;
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
	char **vpd_region_ptr = NULL;
	u32 search_length = find_fmap_entry("RO_VPD", (void **)vpd_region_ptr);
	u32 search_address = (unsigned long)(*vpd_region_ptr);
	u16 io_base = 0;
	struct device *ethernet_dev = NULL;

	/* Initialize the Embedded Controller */
	butterfly_ec_init();

	/* Program EC Keyboard locale based on VPD data */
	program_keyboard_type(search_address, search_length);

	/* Get NIC's IO base address */
	ethernet_dev = dev_find_device(BUTTERFLY_NIC_VENDOR_ID,
				       BUTTERFLY_NIC_DEVICE_ID, dev);
	if (ethernet_dev != NULL) {
		io_base = pci_read_config16(ethernet_dev, 0x10) & 0xfffe;

		/*
		 * Battery life time - LAN PCIe should enter ASPM L1 to save
		 * power when LAN connection is idle.
		 * enable CLKREQ: LAN pci config space 0x81h=01
		 */
		pci_write_config8(ethernet_dev, 0x81, 0x01);
	}

	if (io_base) {
		/* Program MAC address based on VPD data */
		program_mac_address(io_base, search_address, search_length);

		/*
		 * Program NIC LEDS
		 *
		 * RTL8105E Series EEPROM-Less Application Note,
		 * Section 5.6 LED Mode Configuration
		 *
		 * Step1: Write C0h to I/O register 0x50 via byte access to
		 *        disable 'register protection'
		 * Step2: Write xx001111b to I/O register 0x52 via byte access
		 *        (bit7 is LEDS1 and bit6 is LEDS0)
		 * Step3: Write 0x00 to I/O register 0x50 via byte access to
		 *        enable 'register protection'
		 */
		outb(0xc0, io_base + 0x50);	/* Disable protection */
		outb((BUTTERFLY_NIC_LED_MODE << 6) | 0x0f, io_base + 0x52);
		outb(0x00, io_base + 0x50);	/* Enable register protection */
	}
}

static int butterfly_smbios_type41(int *handle, unsigned long *current,
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

static int butterfly_onboard_smbios_data(device_t dev, int *handle,
					 unsigned long *current)
{
	int len = 0;

	len += butterfly_smbios_type41(handle, current,
				       BUTTERFLY_TRACKPAD_NAME,
				       BUTTERFLY_TRACKPAD_IRQ,
				       BUTTERFLY_TRACKPAD_I2C_ADDR);

	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(device_t dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = butterfly_onboard_smbios_data;
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	int15_install();
#endif
	verb_setup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
