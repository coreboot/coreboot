/*
 * This file is part of the coreboot project.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cbmem.h>
#include "i945.h"

static uintptr_t smm_region_start(void)
{
	uintptr_t tom;

	if (pci_read_config8(PCI_DEV(0, 0x0, 0), DEVEN) & (DEVEN_D2F0 | DEVEN_D2F1)) {
		/* IGD enabled, get top of Memory from BSM register */
		tom = pci_read_config32(PCI_DEV(0,2,0), BSM);
	} else {
		tom = (pci_read_config8(PCI_DEV(0,0,0), TOLUD) & 0xf7) << 24;
	}

	/* if TSEG enabled subtract size */
	switch(pci_read_config8(PCI_DEV(0, 0, 0), ESMRAM) & 0x07) {
	case 0x01:
		/* 1MB TSEG */
		tom -= 0x100000;
		break;
	case 0x03:
		/* 2MB TSEG */
		tom -= 0x200000;
		break;
	case 0x05:
		/* 8MB TSEG */
		tom -= 0x800000;
		break;
	default:
		/* TSEG either disabled or invalid */
		break;
	}
	return tom;
}

void *cbmem_top(void)
{
	return (void *) smm_region_start();
}
