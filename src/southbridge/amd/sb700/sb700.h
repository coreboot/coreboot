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

#ifndef SB700_H
#define SB700_H

#include <device/pci_ids.h>
#include "chip.h"

/* Power management index/data registers */
#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

#define SB700_ACPI_IO_BASE 0x800

#define ACPI_PM_EVT_BLK		(SB700_ACPI_IO_BASE + 0x00) /* 4 bytes */
#define ACPI_PM1_CNT_BLK	(SB700_ACPI_IO_BASE + 0x04) /* 2 bytes */
#define ACPI_PMA_CNT_BLK	(SB700_ACPI_IO_BASE + 0x16) /* 1 byte */
#define ACPI_PM_TMR_BLK		(SB700_ACPI_IO_BASE + 0x20) /* 4 bytes */
#define ACPI_GPE0_BLK		(SB700_ACPI_IO_BASE + 0x18) /* 8 bytes */
#define ACPI_CPU_CONTROL	(SB700_ACPI_IO_BASE + 0x08) /* 6 bytes */
#define ACPI_CPU_P_LVL2		(ACPI_CPU_CONTROL + 0x4)    /* 1 byte */

extern void pm_iowrite(u8 reg, u8 value);
extern u8 pm_ioread(u8 reg);
extern void pm2_iowrite(u8 reg, u8 value);
extern u8 pm2_ioread(u8 reg);
#ifndef __SIMPLE_DEVICE__
extern void set_sm_enable_bits(struct device *sm_dev, u32 reg_pos, u32 mask,
			       u32 val);
#endif

#define REV_SB700_A11	0x11
#define REV_SB700_A12	0x12
#define REV_SB700_A14	0x14
#define REV_SB700_A15	0x15

/* This shouldn't be called before set_sb700_revision() is called.
 * Once set_sb700_revision() is called, we use get_sb700_revision(),
 * the simpler one, to get the sb700 revision ID.
 * The id is 0x39 if A11, 0x3A if A12, 0x3C if A14, 0x3D if A15.
 * The differentiate is 0x28, isn't it? */
#define get_sb700_revision(sm_dev)	(pci_read_config8((sm_dev), 0x08) - 0x28)

#ifndef __SIMPLE_DEVICE__
void sb7xx_51xx_enable(struct device *dev);
#endif

#ifdef __PRE_RAM__
void sb7xx_51xx_lpc_port80(void);
void sb7xx_51xx_pci_port80(void);
void sb7xx_51xx_lpc_init(void);
void sb7xx_51xx_enable_wideio(u8 wio_index, u16 base);
void sb7xx_51xx_disable_wideio(u8 wio_index);
void sb7xx_51xx_early_setup(void);
void sb7xx_51xx_before_pci_init(void);
uint16_t sb7xx_51xx_decode_last_reset(void);
#else
#include <device/pci.h>
/* allow override in mainboard.c */
void sb7xx_51xx_setup_sata_phys(struct device *dev);
void sb7xx_51xx_setup_sata_port_indication(void *sata_bar5);

#endif

void set_lpc_sticky_ctl(bool enable);

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos);
int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos);

void enable_fid_change_on_sb(u32 sbbusn, u32 sbdn);
#endif /* SB700_H */
