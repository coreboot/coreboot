/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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

#ifndef AMDFAM10_PCI_H
#define AMDFAM10_PCI_H

#include <inttypes.h>
#include <arch/io.h>
#include <device/pci_def.h>

u32 pci_read_config32_index(pci_devfn_t dev, u32 index_reg, u32 index);
u32 pci_read_config32_index_wait(pci_devfn_t dev, u32 index_reg, u32 index);

#endif
