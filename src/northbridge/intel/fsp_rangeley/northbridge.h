/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __NORTHBRIDGE_INTEL_RANGELEY_NORTHBRIDGE_H__
#define __NORTHBRIDGE_INTEL_RANGELEY_NORTHBRIDGE_H__

#define DEFAULT_ECBASE CONFIG_MMCONF_BASE_ADDRESS

/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

#include <rules.h>
#include <device/device.h>

/* Device 0:0.0 PCI configuration space (Host Bridge) */

/* SideBand B-UNIT */
#define B_UNIT 3
 #define BNOCACHE    0x23
 #define BNOCACHECTL 0x24
 #define BMBOUND     0x25
 #define BMBOUND_HI  0x26
 #define BECREG      0x27
 #define BMISC       0x28
 #define BSMMRRL     0x2E
 #define BSMMRRH     0x2F
 #define BIMR0L      0x80
 #define BIMR0H      0x81
 #define BIMR0RAC    0x82
 #define BIMR0WAC    0x83

/* SideBand C-UNIT */
#define C_UNIT 8

/* SideBand D-UNIT */
#define D_UNIT 1

/* SideBand P-UNIT */
#define P_UNIT 4

#ifndef __ASSEMBLER__
static inline void barrier(void) { asm("" ::: "memory"); }

#define PCI_DEVICE_ID_RG_MIN 0x1F00
#define PCI_DEVICE_ID_RG_MAX 0x1F0F
#define SKPAD 0xFC

int bridge_silicon_revision(void);
void rangeley_late_initialization(void);
u32 sideband_read(int port, int reg);
void sideband_write(int port, int reg, long data);

/* debugging functions */
void print_pci_devices(void);
void dump_pci_device(unsigned dev);
void dump_pci_devices(void);
void dump_spd_registers(void);
void dump_mem(unsigned start, unsigned end);
void report_platform_info(void);

#ifndef __SIMPLE_DEVICE__
void northbridge_acpi_fill_ssdt_generator(struct device *device);
#endif

#endif /* #ifndef __ASSEMBLER__ */
#endif /* #ifndef __ACPI__ */
#endif /* __NORTHBRIDGE_INTEL_RANGELEY_NORTHBRIDGE_H__ */
