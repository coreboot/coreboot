/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008 by coresystems GmbH 
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
#include <pci/pci.h>

#define INTELTOOL_VERSION "1.0"

/* Tested chipsets: */
#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_DEVICE_ID_INTEL_ICH		0x2410
#define PCI_DEVICE_ID_INTEL_ICH0	0x2420
#define PCI_DEVICE_ID_INTEL_ICH2	0x2440
#define PCI_DEVICE_ID_INTEL_ICH4	0x24c0
#define PCI_DEVICE_ID_INTEL_ICH4M	0x24cc
#define PCI_DEVICE_ID_INTEL_ICH7DH	0x27b0
#define PCI_DEVICE_ID_INTEL_ICH7	0x27b8
#define PCI_DEVICE_ID_INTEL_ICH7M	0x27b9
#define PCI_DEVICE_ID_INTEL_ICH7MDH	0x27bd

#define PCI_DEVICE_ID_INTEL_82845	0x1a30
#define PCI_DEVICE_ID_INTEL_82945GM	0x27a0

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

typedef struct { uint32_t hi, lo; } msr_t;
typedef struct { uint16_t addr; int size; char *name; } io_register_t;

extern int fd_mem;

unsigned int cpuid(unsigned int op);
int print_intel_core_msrs(void);
int print_mchbar(struct pci_dev *nb);
int print_pmbase(struct pci_dev *sb);
int print_rcba(struct pci_dev *sb);
int print_gpios(struct pci_dev *sb);
int print_epbar(struct pci_dev *nb);
int print_dmibar(struct pci_dev *nb);
int print_pciexbar(struct pci_dev *nb);

