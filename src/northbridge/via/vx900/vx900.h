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
 */
#ifndef __VX900_H
#define __VX900_H

#define VX900_ACPI_IO_BASE		0x0400

#define SMBUS_IO_BASE	0x500

#define VX900_MMCONFIG_MBAR		0xbc

/* The maximum number of DIMM slots that the VX900 supports */
#define VX900_MAX_DIMM_SLOTS 2
#define VX900_MAX_MEM_RANKS 4

#include <device/pci_ops.h>
#include <device/pci.h>

u32 vx900_get_tolm(void);
void vx900_set_chrome9hd_fb_size(u32 size_mb);
u8 vx900_get_chrome9hd_fb_pow(void);
u32 vx900_get_chrome9hd_fb_size(void);
u8 vx900_int15_get_5f18_bl(void);
uint64_t get_uma_memory_base(void);

/* We use these throughout the code. They really belong in a generic part of
 * coreboot, but until bureaucracy gets them there, we still need them  */

void dump_pci_device(pci_devfn_t dev);

#endif				/* __VX900_H */
