/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#ifndef SB800_H
#define SB800_H

#include <device/pci_ids.h>
#include "chip.h"

/* Power management index/data registers */
#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

#define SB800_ACPI_IO_BASE 0x800

#define ACPI_PM_EVT_BLK		(SB800_ACPI_IO_BASE + 0x00) /* 4 bytes */
#define ACPI_PM1_CNT_BLK	(SB800_ACPI_IO_BASE + 0x04) /* 2 bytes */
#define ACPI_PMA_CNT_BLK	(SB800_ACPI_IO_BASE + 0x17) /* 1 byte */
#define ACPI_PM_TMR_BLK		(SB800_ACPI_IO_BASE + 0x20) /* 4 bytes */
#define ACPI_GPE0_BLK		(SB800_ACPI_IO_BASE + 0x18) /* 8 bytes */
#define ACPI_CPU_CONTROL	(SB800_ACPI_IO_BASE + 0x08) /* 6 bytes */
#define ACPI_CPU_P_LVL2		(ACPI_CPU_CONTROL + 0x4)    /* 1 byte */

void pm_iowrite(u8 reg, u8 value);
u8 pm_ioread(u8 reg);
void pm2_iowrite(u8 reg, u8 value);
u8 pm2_ioread(u8 reg);

#ifndef __SIMPLE_DEVICE__
void set_sm_enable_bits(struct device *sm_dev, u32 reg_pos, u32 mask, u32 val);
#endif

#define REV_SB800_A11	0x11
#define REV_SB800_A12	0x12


#ifdef __PRE_RAM__
void sb800_lpc_port80(void);
void sb800_pci_port80(void);
void sb800_clk_output_48Mhz(void);

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos);
int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos);

#else
void sb800_enable(struct device *dev);
#endif

#endif /* SB800_H */
