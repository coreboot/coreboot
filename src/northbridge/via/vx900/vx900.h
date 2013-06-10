/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __VX900_H
#define __VX900_H

#define VX900_ACPI_IO_BASE		0x0400

#define VX900_NB_IOAPIC_ID		0x2
#define VX900_NB_IOAPIC_BASE		0xfecc000

#define VX900_SB_IOAPIC_ID		0x1
#define VX900_SB_IOAPIC_BASE		0xfec0000

#define SMBUS_IO_BASE	0x500

/* The maximum number of DIMM slots that the VX900 supports */
#define VX900_MAX_DIMM_SLOTS 2
#define VX900_MAX_MEM_RANKS 4

#include <arch/io.h>
#include <device/pci.h>

#include <console/console.h>

u32 chrome9hd_fb_size(void);
u8 vx900_int15_get_5f18_bl(void);

/* We use these throughout the code. They really belong in a generic part of
 * coreboot, but until bureaucracy gets them there, we still need them  */
void dump_pci_device(device_t dev);
void pci_mod_config8(device_t dev, unsigned int where,
		     uint8_t clr_mask, uint8_t set_mask);
void pci_mod_config16(device_t dev, unsigned int where,
		      uint16_t clr_mask, uint16_t set_mask);
void pci_mod_config32(device_t dev, unsigned int where,
		      uint32_t clr_mask, uint32_t set_mask);

#endif				/* __VX900_H */
