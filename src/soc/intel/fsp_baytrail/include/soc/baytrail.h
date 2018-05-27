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

#ifndef __SOC_INTEL_FSP_BAYTRAIL_BAYTRAIL_H__
#define __SOC_INTEL_FSP_BAYTRAIL_BAYTRAIL_H__

#define DEFAULT_ECBASE CONFIG_MMCONF_BASE_ADDRESS
#define CPU_MICROCODE_CBFS_LEN 0x26000

/* Southbridge internal device IO BARs (Set to match FSP settings) */
#define SMBUS_IO_BASE		0xefa0
#define SMBUS_SLAVE_ADDR	0x24
#define DEFAULT_GPIOBASE	0x0500
#define DEFAULT_ABASE		0x0400

/* Southbridge internal device MEM BARs (Set to match FSP settings) */
#define DEFAULT_IBASE		0xfed08000
#define DEFAULT_PBASE		0xfed03000
#ifndef __ACPI__
#define DEFAULT_RCBA		((u8 *)0xfed1c000)
#else
#define DEFAULT_RCBA		0xfed1c000
#endif
/* Everything below this line is ignored in the DSDT */
#ifndef __ACPI__

/* Device 0:0.0 PCI configuration space (Host Bridge) */

/* SOC types */
#define SOC_TYPE_BAYTRAIL	0x0F1C

#ifndef __ASSEMBLER__
static inline void barrier(void) { asm("" ::: "memory"); }

#define SKPAD 0xFC

int bridge_silicon_revision(void);
void rangeley_early_initialization(void);

#ifndef __PRE_RAM__
/* soc.c */
int soc_silicon_revision(void);
int soc_silicon_type(void);
int soc_silicon_supported(int type, int rev);
void soc_enable(struct device *dev);

/* debugging functions */
void print_pci_devices(void);
void dump_pci_device(unsigned dev);
void dump_pci_devices(void);
void dump_spd_registers(void);
void dump_mem(unsigned start, unsigned end);
void report_platform_info(void);

#endif	/* __PRE_RAM__ */
#endif	/* __ASSEMBLER__ */

#endif	/* __ACPI__ */
#endif
