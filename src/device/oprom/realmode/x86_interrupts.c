/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001 Ronald G. Minnich
 * Copyright (C) 2005 Nick.Barker9@btinternet.com
 * Copyright (C) 2007-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <string.h>
#include <console/console.h>
#include <arch/io.h>
#include <arch/registers.h>
#include "x86.h"
/* we use x86emu's register file representation */
#include <x86emu/regs.h>

// errors go in AH. Just set these up so that word assigns
// will work. KISS.
enum {
	PCIBIOS_SUCCESSFUL = 0x0000,
	PCIBIOS_UNSUPPORTED = 0x8100,
	PCIBIOS_BADVENDOR = 0x8300,
	PCIBIOS_NODEV = 0x8600,
	PCIBIOS_BADREG = 0x8700
};

int int10_handler(void)
{
	int res=0;
	static u8 cursor_row=0, cursor_col=0;
	switch((X86_EAX & 0xff00)>>8) {
	case 0x01: // Set cursor shape
		res = 1;
		break;
	case 0x02: // Set cursor position
		if (cursor_row != ((X86_EDX >> 8) & 0xff) ||
		    cursor_col >= (X86_EDX & 0xff)) {
			printk(BIOS_INFO, "\n");
		}
		cursor_row = (X86_EDX >> 8) & 0xff;
		cursor_col = X86_EDX & 0xff;
		res = 1;
		break;
	case 0x03: // Get cursor position
		X86_EAX &= 0x00ff;
		X86_ECX = 0x0607;
		X86_EDX = (cursor_row << 8) | cursor_col;
		res = 1;
		break;
	case 0x06: // Scroll up
		printk(BIOS_INFO, "\n");
		res = 1;
		break;
	case 0x08: // Get Character and Mode at Cursor Position
		X86_EAX = 0x0f00 | 'A'; // White on black 'A'
		res = 1;
		break;
	case 0x09: // Write Character and attribute
	case 0x0e: // Write Character
		printk(BIOS_INFO, "%c", X86_EAX & 0xff);
		res = 1;
		break;
	case 0x0f: // Get video mode
		X86_EAX = 0x5002; //80x25
		X86_EBX &= 0x00ff;
		res = 1;
		break;
        default:
		printk(BIOS_WARNING, "Unknown INT10 function %04x!\n",
				X86_EAX & 0xffff);
		break;
	}
	return res;
}

int int12_handler(void)
{
	X86_EAX = 64 * 1024;
	return 1;
}

int int16_handler(void)
{
	int res=0;
	switch((X86_EAX & 0xff00)>>8) {
	case 0x00: // Check for Keystroke
		X86_EAX = 0x6120; // Space Bar, Space
		res = 1;
		break;
	case 0x01: // Check for Keystroke
		X86_EFLAGS |= 1<<6; // Zero Flag set (no key available)
		res = 1;
		break;
        default:
		printk(BIOS_WARNING, "Unknown INT16 function %04x!\n",
				X86_EAX & 0xffff);
		break;
	}
	return res;
}

#define PCI_CONFIG_SPACE_TYPE1	(1 << 0)
#define PCI_SPECIAL_CYCLE_TYPE1	(1 << 4)

int int1a_handler(void)
{
	unsigned short func = (unsigned short)X86_EAX;
	int retval = 1;
	unsigned short devid, vendorid, devfn;
	/* Use short to get rid of garbage in upper half of 32-bit register */
	short devindex;
	unsigned char bus;
	struct device *dev;
	u32 dword;
	u16 word;
	u8 byte, reg;

	switch (func) {
	case 0xb101: /* PCIBIOS Check */
		X86_EDX = 0x20494350;	/* ' ICP' */
		X86_EAX &= 0xffff0000; /* Clear AH / AL */
		X86_EAX |= PCI_CONFIG_SPACE_TYPE1 | PCI_SPECIAL_CYCLE_TYPE1;
		// last bus in the system. Hard code to 255 for now.
		// dev_enumerate() does not seem to tell us (publicly)
		X86_ECX = 0xff;
		X86_EDI = 0x00000000;	/* protected mode entry */
		retval = 1;
		break;
	case 0xb102: /* Find Device */
		devid = X86_ECX;
		vendorid = X86_EDX;
		devindex = X86_ESI;
		dev = 0;
		while ((dev = dev_find_device(vendorid, devid, dev))) {
			if (devindex <= 0)
				break;
			devindex--;
		}
		if (dev) {
			unsigned short busdevfn;
			X86_EAX &= 0xffff00ff; /* Clear AH */
			X86_EAX |= PCIBIOS_SUCCESSFUL;
			// busnum is an unsigned char;
			// devfn is an int, so we mask it off.
			busdevfn = (dev->bus->secondary << 8)
			    | (dev->path.pci.devfn & 0xff);
			printk(BIOS_DEBUG, "0x%x: return 0x%x\n", func, busdevfn);
			X86_EBX = busdevfn;
			retval = 1;
		} else {
			X86_EAX &= 0xffff00ff; /* Clear AH */
			X86_EAX |= PCIBIOS_NODEV;
			retval = 0;
		}
		break;
	case 0xb10a: /* Read Config Dword */
	case 0xb109: /* Read Config Word */
	case 0xb108: /* Read Config Byte */
	case 0xb10d: /* Write Config Dword */
	case 0xb10c: /* Write Config Word */
	case 0xb10b: /* Write Config Byte */
		devfn = X86_EBX & 0xff;
		bus = X86_EBX >> 8;
		reg = X86_EDI;
		dev = dev_find_slot(bus, devfn);
		if (!dev) {
			printk(BIOS_DEBUG, "0x%x: BAD DEVICE bus %d devfn 0x%x\n", func, bus, devfn);
			// Or are we supposed to return PCIBIOS_NODEV?
			X86_EAX &= 0xffff00ff; /* Clear AH */
			X86_EAX |= PCIBIOS_BADREG;
			retval = 0;
			return retval;
		}
		switch (func) {
		case 0xb108: /* Read Config Byte */
			byte = pci_read_config8(dev, reg);
			X86_ECX = byte;
			break;
		case 0xb109: /* Read Config Word */
			word = pci_read_config16(dev, reg);
			X86_ECX = word;
			break;
		case 0xb10a: /* Read Config Dword */
			dword = pci_read_config32(dev, reg);
			X86_ECX = dword;
			break;
		case 0xb10b: /* Write Config Byte */
			byte = X86_ECX;
			pci_write_config8(dev, reg, byte);
			break;
		case 0xb10c: /* Write Config Word */
			word = X86_ECX;
			pci_write_config16(dev, reg, word);
			break;
		case 0xb10d: /* Write Config Dword */
			dword = X86_ECX;
			pci_write_config32(dev, reg, dword);
			break;
		}

#if CONFIG_REALMODE_DEBUG
		printk(BIOS_DEBUG, "0x%x: bus %d devfn 0x%x reg 0x%x val 0x%x\n",
			     func, bus, devfn, reg, X86_ECX);
#endif
		X86_EAX &= 0xffff00ff; /* Clear AH */
		X86_EAX |= PCIBIOS_SUCCESSFUL;
		retval = 1;
		break;
	default:
		printk(BIOS_ERR, "UNSUPPORTED PCIBIOS FUNCTION 0x%x\n", func);
		X86_EAX &= 0xffff00ff; /* Clear AH */
		X86_EAX |= PCIBIOS_UNSUPPORTED;
		retval = 0;
		break;
	}

	return retval;
}

