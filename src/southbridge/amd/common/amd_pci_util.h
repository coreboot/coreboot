/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#ifndef AMD_PCI_UTIL_H
#define AMD_PCI_UTIL_H

#include <stdint.h>
#include "amd_pci_int_defs.h"

/* FCH index/data registers */
#define PCI_INTR_INDEX	0xc00
#define PCI_INTR_DATA	0xc01

#ifndef __PRE_RAM__

struct pirq_struct {
	u8 devfn;
	u8 PIN[4];	/* PINA/B/C/D are index 0/1/2/3 */
};

extern const struct pirq_struct * pirq_data_ptr;
extern u32 pirq_data_size;
extern const u8 *intr_data_ptr;
extern const u8 *picr_data_ptr;

u8 read_pci_int_idx(u8 index, int mode);
void write_pci_int_idx(u8 index, int mode, u8 data);
void write_pci_cfg_irqs(void);
void write_pci_int_table (void);
#endif /* __PRE_RAM */

#endif /* AMD_PCI_UTIL_H */
