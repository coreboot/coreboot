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

// errors go in AH. Just set these up so that word assigns
// will work. KISS.
enum {
	PCIBIOS_SUCCESSFUL = 0x0000,
	PCIBIOS_UNSUPPORTED = 0x8100,
	PCIBIOS_BADVENDOR = 0x8300,
	PCIBIOS_NODEV = 0x8600,
	PCIBIOS_BADREG = 0x8700
};

int int10_handler(struct eregs *regs)
{
	int res=-1;
	static u8 cursor_row=0, cursor_col=0;
	switch((regs->eax & 0xff00)>>8) {
	case 0x01: // Set cursor shape
		res = 0;
		break;
	case 0x02: // Set cursor position
		if (cursor_row != ((regs->edx >> 8) & 0xff) ||
		    cursor_col >= (regs->edx & 0xff)) {
			printk(BIOS_INFO, "\n");
		}
		cursor_row = (regs->edx >> 8) & 0xff;
		cursor_col = regs->edx & 0xff;
		res = 0;
		break;
	case 0x03: // Get cursor position
		regs->eax &= 0x00ff;
		regs->ecx = 0x0607;
		regs->edx = (cursor_row << 8) | cursor_col;
		res = 0;
		break;
	case 0x06: // Scroll up
		printk(BIOS_INFO, "\n");
		res = 0;
		break;
	case 0x08: // Get Character and Mode at Cursor Position
		regs->eax = 0x0f00 | 'A'; // White on black 'A'
		res = 0;
		break;
	case 0x09: // Write Character and attribute
	case 0x0e: // Write Character
		printk(BIOS_INFO, "%c", regs->eax & 0xff);
		res = 0;
		break;
	case 0x0f: // Get video mode
		regs->eax = 0x5002; //80x25
		regs->ebx &= 0x00ff;
		res = 0;
		break;
        default:
		printk(BIOS_WARNING, "Unknown INT10 function %04x!\n",
				regs->eax & 0xffff);
		break;
	}
	return res;
}

int int12_handler(struct eregs *regs)
{
	regs->eax = 64 * 1024;
	return 0;
}

int int16_handler(struct eregs *regs)
{
	int res=-1;
	switch((regs->eax & 0xff00)>>8) {
	case 0x00: // Check for Keystroke
		regs->eax = 0x6120; // Space Bar, Space
		res = 0;
		break;
	case 0x01: // Check for Keystroke
		regs->eflags |= 1<<6; // Zero Flag set (no key available)
		res = 0;
		break;
        default:
		printk(BIOS_WARNING, "Unknown INT16 function %04x!\n",
				regs->eax & 0xffff);
		break;
	}
	return res;
}

#define PCI_CONFIG_SPACE_TYPE1	(1 << 0)
#define PCI_SPECIAL_CYCLE_TYPE1	(1 << 4)

int int1a_handler(struct eregs *regs)
{
	unsigned short func = (unsigned short)regs->eax;
	int retval = 0;
	unsigned short devid, vendorid, devfn;
	/* Use short to get rid of gabage in upper half of 32-bit register */
	short devindex;
	unsigned char bus;
	struct device *dev;
	u32 dword;
	u16 word;
	u8 byte, reg;

	switch (func) {
	case 0xb101: /* PCIBIOS Check */
		regs->edx = 0x20494350;	/* ' ICP' */
		regs->eax &= 0xffff0000; /* Clear AH / AL */
		regs->eax |= PCI_CONFIG_SPACE_TYPE1 | PCI_SPECIAL_CYCLE_TYPE1;
		// last bus in the system. Hard code to 255 for now.
		// dev_enumerate() does not seem to tell us (publically)
		regs->ecx = 0xff;
		regs->edi = 0x00000000;	/* protected mode entry */
		retval = 0;
		break;
	case 0xb102: /* Find Device */
		devid = regs->ecx;
		vendorid = regs->edx;
		devindex = regs->esi;
		dev = 0;
		while ((dev = dev_find_device(vendorid, devid, dev))) {
			if (devindex <= 0)
				break;
			devindex--;
		}
		if (dev) {
			unsigned short busdevfn;
			regs->eax &= 0xffff00ff; /* Clear AH */
			regs->eax |= PCIBIOS_SUCCESSFUL;
			// busnum is an unsigned char;
			// devfn is an int, so we mask it off.
			busdevfn = (dev->bus->secondary << 8)
			    | (dev->path.pci.devfn & 0xff);
			printk(BIOS_DEBUG, "0x%x: return 0x%x\n", func, busdevfn);
			regs->ebx = busdevfn;
			retval = 0;
		} else {
			regs->eax &= 0xffff00ff; /* Clear AH */
			regs->eax |= PCIBIOS_NODEV;
			retval = -1;
		}
		break;
	case 0xb10a: /* Read Config Dword */
	case 0xb109: /* Read Config Word */
	case 0xb108: /* Read Config Byte */
	case 0xb10d: /* Write Config Dword */
	case 0xb10c: /* Write Config Word */
	case 0xb10b: /* Write Config Byte */
		devfn = regs->ebx & 0xff;
		bus = regs->ebx >> 8;
		reg = regs->edi;
		dev = dev_find_slot(bus, devfn);
		if (!dev) {
			printk(BIOS_DEBUG, "0x%x: BAD DEVICE bus %d devfn 0x%x\n", func, bus, devfn);
			// Or are we supposed to return PCIBIOS_NODEV?
			regs->eax &= 0xffff00ff; /* Clear AH */
			regs->eax |= PCIBIOS_BADREG;
			retval = -1;
			return retval;
		}
		switch (func) {
		case 0xb108: /* Read Config Byte */
			byte = pci_read_config8(dev, reg);
			regs->ecx = byte;
			break;
		case 0xb109: /* Read Config Word */
			word = pci_read_config16(dev, reg);
			regs->ecx = word;
			break;
		case 0xb10a: /* Read Config Dword */
			dword = pci_read_config32(dev, reg);
			regs->ecx = dword;
			break;
		case 0xb10b: /* Write Config Byte */
			byte = regs->ecx;
			pci_write_config8(dev, reg, byte);
			break;
		case 0xb10c: /* Write Config Word */
			word = regs->ecx;
			pci_write_config16(dev, reg, word);
			break;
		case 0xb10d: /* Write Config Dword */
			dword = regs->ecx;
			pci_write_config32(dev, reg, dword);
			break;
		}

#if CONFIG_REALMODE_DEBUG
		printk(BIOS_DEBUG, "0x%x: bus %d devfn 0x%x reg 0x%x val 0x%x\n",
			     func, bus, devfn, reg, regs->ecx);
#endif
		regs->eax &= 0xffff00ff; /* Clear AH */
		regs->eax |= PCIBIOS_SUCCESSFUL;
		retval = 0;
		break;
	default:
		printk(BIOS_ERR, "UNSUPPORTED PCIBIOS FUNCTION 0x%x\n", func);
		regs->eax &= 0xffff00ff; /* Clear AH */
		regs->eax |= PCIBIOS_UNSUPPORTED;
		retval = -1;
		break;
	}

	return retval;
}

