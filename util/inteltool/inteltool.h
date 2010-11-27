/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 * Copyright (C) 2009 Carl-Daniel Hailfinger
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

#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#if (defined(__MACH__) && defined(__APPLE__))
/* DirectIO is available here: http://www.coresystems.de/en/directio */
#define __DARWIN__
#include <DirectIO/darwinio.h>
#endif
#include <pci/pci.h>

/* This #include is needed for freebsd_{rd,wr}msr. */
#if defined(__FreeBSD__)
#include <machine/cpufunc.h>
#endif

#define INTELTOOL_VERSION "1.0"

/* Tested chipsets: */
#define PCI_VENDOR_ID_INTEL			0x8086
#define PCI_DEVICE_ID_INTEL_ICH			0x2410
#define PCI_DEVICE_ID_INTEL_ICH0		0x2420
#define PCI_DEVICE_ID_INTEL_ICH2		0x2440
#define PCI_DEVICE_ID_INTEL_ICH4		0x24c0
#define PCI_DEVICE_ID_INTEL_ICH4M		0x24cc
#define PCI_DEVICE_ID_INTEL_ICH6		0x2640
#define PCI_DEVICE_ID_INTEL_ICH7DH		0x27b0
#define PCI_DEVICE_ID_INTEL_ICH7		0x27b8
#define PCI_DEVICE_ID_INTEL_ICH7M		0x27b9
#define PCI_DEVICE_ID_INTEL_ICH7MDH		0x27bd
#define PCI_DEVICE_ID_INTEL_NM10		0x27bc
#define PCI_DEVICE_ID_INTEL_ICH8		0x2810
#define PCI_DEVICE_ID_INTEL_ICH8M		0x2815
#define PCI_DEVICE_ID_INTEL_ICH9DH		0x2912
#define PCI_DEVICE_ID_INTEL_ICH9DO		0x2914
#define PCI_DEVICE_ID_INTEL_ICH9R		0x2916
#define PCI_DEVICE_ID_INTEL_ICH9		0x2918
#define PCI_DEVICE_ID_INTEL_ICH9M		0x2919
#define PCI_DEVICE_ID_INTEL_ICH9ME		0x2917
#define PCI_DEVICE_ID_INTEL_ICH10R		0x3a16
#define PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC	0x8119

#define PCI_DEVICE_ID_INTEL_82810		0x7120
#define PCI_DEVICE_ID_INTEL_82810DC		0x7122
#define PCI_DEVICE_ID_INTEL_82810E_MC		0x7124
#define PCI_DEVICE_ID_INTEL_82830M		0x3575
#define PCI_DEVICE_ID_INTEL_82845		0x1a30
#define PCI_DEVICE_ID_INTEL_82915		0x2580
#define PCI_DEVICE_ID_INTEL_82945P		0x2770
#define PCI_DEVICE_ID_INTEL_82945GM		0x27a0
#define PCI_DEVICE_ID_INTEL_82945GSE		0x27ac
#define PCI_DEVICE_ID_INTEL_PM965		0x2a00
#define PCI_DEVICE_ID_INTEL_Q965		0x2990
#define PCI_DEVICE_ID_INTEL_82975X		0x277c
#define PCI_DEVICE_ID_INTEL_82Q35		0x29b0
#define PCI_DEVICE_ID_INTEL_82G33		0x29c0
#define PCI_DEVICE_ID_INTEL_82Q33		0x29d0
#define PCI_DEVICE_ID_INTEL_GS45		0x2a40
#define PCI_DEVICE_ID_INTEL_X58			0x3405
#define PCI_DEVICE_ID_INTEL_SCH_POULSBO		0x8100
#define PCI_DEVICE_ID_INTEL_ATOM_DXXX		0xa000

/* untested, but almost identical to D-series */
#define PCI_DEVICE_ID_INTEL_ATOM_NXXX		0xa010

#define PCI_DEVICE_ID_INTEL_82443LX		0x7180
/* 82443BX has a different device ID if AGP is disabled (hardware-wise). */
#define PCI_DEVICE_ID_INTEL_82443BX		0x7190
#define PCI_DEVICE_ID_INTEL_82443BX_NO_AGP	0x7192

/* 82371AB/EB/MB use the same device ID value. */
#define PCI_DEVICE_ID_INTEL_82371XX		0x7110

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

void *map_physical(unsigned long phys_addr, size_t len);
void unmap_physical(void *virt_addr, size_t len);

unsigned int cpuid(unsigned int op);
int print_intel_core_msrs(void);
int print_mchbar(struct pci_dev *nb);
int print_pmbase(struct pci_dev *sb, struct pci_access *pacc);
int print_rcba(struct pci_dev *sb);
int print_gpios(struct pci_dev *sb);
int print_epbar(struct pci_dev *nb);
int print_dmibar(struct pci_dev *nb);
int print_pciexbar(struct pci_dev *nb);
