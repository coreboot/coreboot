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
#define printk(x...) do_printk(x)

enum {
	PCIBIOS_CHECK = 0xb101,
	PCIBIOS_FINDDEV = 0xb102,
	PCIBIOS_READCONFBYTE = 0xb108,
	PCIBIOS_READCONFWORD = 0xb109,
	PCIBIOS_READCONFDWORD = 0xb10a,
	PCIBIOS_WRITECONFBYTE = 0xb10b,
	PCIBIOS_WRITECONFWORD = 0xb10c,
	PCIBIOS_WRITECONFDWORD = 0xb10d
};

// errors go in AH. Just set these up so that word assigns
// will work. KISS.
enum {
	PCIBIOS_SUCCESSFUL = 0x0000,
	PCIBIOS_UNSUPPORTED = 0x8100,
	PCIBIOS_BADVENDOR = 0x8300,
	PCIBIOS_NODEV = 0x8600,
	PCIBIOS_BADREG = 0x8700
};

int int12_handler(struct eregs *regs);
int int1a_handler(struct eregs *regs);
int int15_handler(struct eregs *regs);

int int12_handler(struct eregs *regs)
{
	regs->eax = 64 * 1024;
	return 0;
}

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
	case PCIBIOS_CHECK:
		regs->edx = 0x20494350;	/* ' ICP' */
		regs->edi = 0x00000000;	/* protected mode entry */
		retval = 0;
		break;
	case PCIBIOS_FINDDEV:
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
			regs->eax = 0;
			// busnum is an unsigned char;
			// devfn is an int, so we mask it off.
			busdevfn = (dev->bus->secondary << 8)
			    | (dev->path.pci.devfn & 0xff);
			printk(BIOS_DEBUG, "0x%x: return 0x%x\n", func, busdevfn);
			regs->ebx = busdevfn;
			retval = 0;
		} else {
			regs->eax = PCIBIOS_NODEV;
			retval = -1;
		}
		break;
	case PCIBIOS_READCONFDWORD:
	case PCIBIOS_READCONFWORD:
	case PCIBIOS_READCONFBYTE:
	case PCIBIOS_WRITECONFDWORD:
	case PCIBIOS_WRITECONFWORD:
	case PCIBIOS_WRITECONFBYTE:
		devfn = regs->ebx & 0xff;
		bus = regs->ebx >> 8;
		reg = regs->edi;
		dev = dev_find_slot(bus, devfn);
		if (!dev) {
			printk(BIOS_DEBUG, "0x%x: BAD DEVICE bus %d devfn 0x%x\n", func, bus, devfn);
			// idiots. the pcibios guys assumed you'd never pass a bad bus/devfn!
			regs->eax = PCIBIOS_BADREG;
			retval = -1;
			return retval;
		}
		switch (func) {
		case PCIBIOS_READCONFBYTE:
			byte = pci_read_config8(dev, reg);
			regs->ecx = byte;
			break;
		case PCIBIOS_READCONFWORD:
			word = pci_read_config16(dev, reg);
			regs->ecx = word;
			break;
		case PCIBIOS_READCONFDWORD:
			dword = pci_read_config32(dev, reg);
			regs->ecx = dword;
			break;
		case PCIBIOS_WRITECONFBYTE:
			byte = regs->ecx;
			pci_write_config8(dev, reg, byte);
			break;
		case PCIBIOS_WRITECONFWORD:
			word = regs->ecx;
			pci_write_config16(dev, reg, word);
			break;
		case PCIBIOS_WRITECONFDWORD:
			dword = regs->ecx;
			pci_write_config32(dev, reg, dword);
			break;
		}

		printk(BIOS_DEBUG, "0x%x: bus %d devfn 0x%x reg 0x%x val 0x%x\n",
			     func, bus, devfn, reg, regs->ecx);
		regs->eax = 0;
		retval = 0;
		break;
	default:
		printk(BIOS_ERR, "UNSUPPORTED PCIBIOS FUNCTION 0x%x\n", func);
		retval = -1;
		break;
	}

	return retval;
}

int int15_handler(struct eregs *regs)
{
	int res = -1;

	/* This int15 handler is VIA Tech. specific. Other chipsets need other
	 * handlers. The right way to do this is to move this handler code into
	 * the mainboard or northbridge code.
	 */
	switch (regs->eax & 0xffff) {
	case 0x5f19:
		break;
	case 0x5f18:
		regs->eax = 0x5f;
		// MCLK = 133, 32M frame buffer, 256 M main memory
		regs->ebx = 0x545;
		regs->ecx = 0x060;
		res = 0;
		break;
	case 0x5f00:
		regs->eax = 0x8600;
		break;
	case 0x5f01:
		regs->eax = 0x5f;
		regs->ecx = (regs->ecx & 0xffffff00 ) | 2; // panel type =  2 = 1024 * 768
		res = 0;
		break;
	case 0x5f02:
		regs->eax = 0x5f;
		regs->ebx = (regs->ebx & 0xffff0000) | 2;
		regs->ecx = (regs->ecx & 0xffff0000) | 0x401;  // PAL + crt only
		regs->edx = (regs->edx & 0xffff0000) | 0;  // TV Layout - default
		res = 0;
		break;
	case 0x5f0f:
		regs->eax = 0x860f;
		break;
	/* And now Intel IGD code */
#define BOOT_DISPLAY_DEFAULT    0
#define BOOT_DISPLAY_CRT        (1 << 0)
#define BOOT_DISPLAY_TV         (1 << 1)
#define BOOT_DISPLAY_EFP        (1 << 2)
#define BOOT_DISPLAY_LCD        (1 << 3)
#define BOOT_DISPLAY_CRT2       (1 << 4)
#define BOOT_DISPLAY_TV2        (1 << 5)
#define BOOT_DISPLAY_EFP2       (1 << 6)
#define BOOT_DISPLAY_LCD2       (1 << 7)

	case 0x5f35:
		regs->eax = 0x5f;
		regs->ecx = BOOT_DISPLAY_DEFAULT;
		res = 0;
		break;
	case 0x5f40:
		regs->eax = 0x5f;
		regs->ecx = 3; // This is mainboard specific
		printk(BIOS_DEBUG, "DISPLAY=%x\n", regs->ecx);
		res = 0;
		break;
	default:
		printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", 
				regs->eax & 0xffff);
	}

	return res;
}

