/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#ifndef SB600_H
#define SB600_H

#ifndef __ACPI__
#include <device/pci_ids.h>
#include "chip.h"
#endif

/* Power management index/data registers */
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

#define HPET_BASE_ADDRESS 0xfed00000

#ifndef __ACPI__
extern void pm_iowrite(u8 reg, u8 value);
extern u8 pm_ioread(u8 reg);
extern void pm2_iowrite(u8 reg, u8 value);
extern u8 pm2_ioread(u8 reg);

#ifndef __SIMPLE_DEVICE__
extern void set_sm_enable_bits(struct device *sm_dev, u32 reg_pos, u32 mask,
			       u32 val);
void sb600_enable(struct device *dev);
#endif

void sb600_lpc_port80(void);
void sb600_pci_port80(void);

#endif /* __ACPI__ */
#endif /* SB600_H */
