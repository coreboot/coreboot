/*
 * viatool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 * Copyright (C) 2009 Carl-Daniel Hailfinger
 * Copyright (C) 2013 Alexandru Gagniuc
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdint.h>

#ifndef _VIATOOL_H
#define _VIATOOL_H

#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectHW is available here: http://www.coreboot.org/DirectHW */
#define __DARWIN__
#include <DirectHW/DirectHW.h>
#endif
#include <pci/pci.h>

/* This #include is needed for freebsd_{rd,wr}msr. */
#if defined(__FreeBSD__)
#include <machine/cpufunc.h>
#endif

#include <stdlib.h>

#define VIATOOL_VERSION "1.0"

/* Tested chipsets: */
#define PCI_VENDOR_ID_VIA			0x1106
#define PCI_DEVICE_ID_VIA_VX900			0x0410
#define PCI_DEVICE_ID_VIA_VX900_SATA		0x9001
#define PCI_DEVICE_ID_VIA_VX900_LPC		0x8410


#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

#if !defined(__DARWIN__) && !defined(__FreeBSD__)
typedef struct { uint32_t hi, lo; } msr_t;
#endif
#if defined (__FreeBSD__)
/* FreeBSD already has conflicting definitions for wrmsr/rdmsr. */
#undef rdmsr
#undef wrmsr
#define rdmsr freebsd_rdmsr
#define wrmsr freebsd_wrmsr
typedef struct { uint32_t hi, lo; } msr_t;
msr_t freebsd_rdmsr(int addr);
int freebsd_wrmsr(int addr, msr_t msr);
#endif
typedef struct { uint16_t addr; int size; char *name; } io_register_t;

void *map_physical(uint64_t phys_addr, size_t len);
void unmap_physical(void *virt_addr, size_t len);

unsigned int cpuid(unsigned int op);
int print_intel_core_msrs(void);
int print_quirks_north(struct pci_dev *nb, struct pci_access *pacc);
int print_quirks_south(struct pci_dev *sb, struct pci_access *pacc);

#endif /* _VIATOOL_H */
