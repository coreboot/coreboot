/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <cbmem.h>
#include "gm45.h"

/** Decodes used Graphics Mode Select (GMS) to kilobytes. */
u32 decode_igd_memory_size(const u32 gms)
{
	switch (gms) {
	case 1:
		return   1 << 10;
	case 2:
		return   4 << 10; /* guessed */
	case 3:
		return   8 << 10; /* guessed */
	case 4:
		return  16 << 10;
	case 5:
		return  32 << 10;
	case 7:
		return  64 << 10;
	case 8:
		return 128 << 10;
	case 9:
		return 256 << 10;
	case 10:
		return  96 << 10;
	case 11:
		return 160 << 10;
	case 12:
		return 224 << 10;
	case 13:
		return 352 << 10;
	default:
		die("Bad Graphics Mode Select (GMS) setting.\n");
		return 0;
	}
}

/** Decodes used Graphics Stolen Memory (GSM) to kilobytes. */
u32 decode_igd_gtt_size(const u32 gsm)
{
	switch (gsm) {
	case 0:
		return 0 << 10;
	case 1:
		return 1 << 10;
	case 3:
	case 9:
		return 2 << 10;
	case 10:
		return 3 << 10;
	case 11:
		return 4 << 10;
	default:
		die("Bad Graphics Stolen Memory (GSM) setting.\n");
		return 0;
	}
}

unsigned long get_top_of_ram(void)
{
	const pci_devfn_t dev = PCI_DEV(0, 0, 0);

	u32 tor;

	/* Top of Lower Usable DRAM */
	tor = (pci_read_config16(dev, D0F0_TOLUD) & 0xfff0) << 16;

	/* Graphics memory comes next */
	const u32 ggc = pci_read_config16(dev, D0F0_GGC);
	if (!(ggc & 2)) {
		/* Graphics memory */
		tor -= decode_igd_memory_size((ggc >> 4) & 0xf) << 10;
		/* GTT Graphics Stolen Memory Size (GGMS) */
		tor -= decode_igd_gtt_size((ggc >> 8) & 0xf) << 10;
	}
	return tor;
}
