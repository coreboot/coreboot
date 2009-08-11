/*
 * This software and ancillary information (herein called SOFTWARE )
 * called LinuxBIOS          is made available under the terms described
 * here.  The SOFTWARE has been approved for release with associated
 * LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
 * been authored by an employee or employees of the University of
 * California, operator of the Los Alamos National Laboratory under
 * Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
 * U.S. Government has rights to use, reproduce, and distribute this
 * SOFTWARE.  The public may copy, distribute, prepare derivative works
 * and publicly display this SOFTWARE without charge, provided that this
 * Notice and any statement of authorship are reproduced on all copies.
 * Neither the Government nor the University makes any warranty, express
 * or implied, or assumes any liability or responsibility for the use of
 * this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
 * such modified SOFTWARE should be clearly marked, so as not to confuse
 * it with the version available from LANL.
 */
 /*
 * This file is part of the coreboot project.
 *
 *  (c) Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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
#ifdef CONFIG_COREBOOT_V2
#include <console/console.h>
#else
#include <console.h>
#endif
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <x86emu/x86emu.h>

#include "pcibios.h"

int pcibios_handler(void)
{
	int ret = 0;
	struct device *dev = 0;

	switch (X86_AX) {
	case PCI_BIOS_PRESENT:
		X86_AH	= 0x00;		/* no config space/special cycle support */
		X86_AL	= 0x01;		/* config mechanism 1 */
		X86_EDX = 'P' | 'C' << 8 | 'I' << 16 | ' ' << 24;
		X86_EBX = 0x0210;	/* Version 2.10 */
		X86_ECX = 0xFF00;	/* FixME: Max bus number */
		X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
		ret = 1;
		break;
	case FIND_PCI_DEVICE:
		/* FixME: support SI != 0 */
#ifdef CONFIG_COREBOOT_V2
		dev = dev_find_device(X86_DX, X86_CX, dev);
#else
		dev = dev_find_pci_device(X86_DX, X86_CX, dev);
#endif
		if (dev != 0) {
			X86_BH = dev->bus->secondary;
			X86_BL = dev->path.pci.devfn;
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */
			ret = 0;
		}
		break;
	case FIND_PCI_CLASS_CODE:
		/* FixME: support SI != 0 */
		dev = dev_find_class(X86_ECX, dev);
		if (dev != 0) {
			X86_BH = dev->bus->secondary;
			X86_BL = dev->path.pci.devfn;
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */
			ret = 0;
		}
		break;
	case READ_CONFIG_BYTE:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_CL = pci_read_config8(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case READ_CONFIG_WORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_CX = pci_read_config16(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case READ_CONFIG_DWORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			X86_ECX = pci_read_config32(dev, X86_DI);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_BYTE:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config8(dev, X86_DI, X86_CL);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_WORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config16(dev, X86_DI, X86_CX);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	case WRITE_CONFIG_DWORD:
		dev = dev_find_slot(X86_BH, X86_BL);
		if (dev != 0) {
			pci_write_config16(dev, X86_DI, X86_ECX);
			X86_AH = SUCCESSFUL;
			X86_EFLAGS &= ~FB_CF;	/* clear carry flag */
			ret = 1;
		} else {
			X86_AH = DEVICE_NOT_FOUND;
			X86_EFLAGS |= FB_CF;	/* set carry flag */	
			ret = 0;
		}
		break;
	default:
		X86_AH = FUNC_NOT_SUPPORTED;
		X86_EFLAGS |= FB_CF; 
		break;
	}

	return ret;
}
