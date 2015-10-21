/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc
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

#ifndef __PCI_USERSPACE_H__
#define __PCI_USERSPACE_H__

struct device {
	int busno;
	int slot;
	int func;
};

int pci_initialize(void);
int pci_exit(void);

u8 pci_read_config8(struct device *dev, unsigned int where);
u16 pci_read_config16(struct device *dev, unsigned int where);
u32 pci_read_config32(struct device *dev, unsigned int where);
void pci_write_config8(struct device *dev, unsigned int where, u8 val);
void pci_write_config16(struct device *dev, unsigned int where, u16 val);
void pci_write_config32(struct device *dev, unsigned int where, u32 val);

#endif
