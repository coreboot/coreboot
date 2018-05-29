/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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

#ifndef HUDSON_H
#define HUDSON_H

#include <device/pci_ids.h>
#include <device/device.h>
#include "chip.h"

/* Power management index/data registers */
#define BIOSRAM_INDEX	0xcd4
#define BIOSRAM_DATA	0xcd5
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

#define HUDSON_ACPI_IO_BASE 0x800

#define ACPI_PM_EVT_BLK		(HUDSON_ACPI_IO_BASE + 0x00) /* 4 bytes */
#define ACPI_PM1_CNT_BLK	(HUDSON_ACPI_IO_BASE + 0x04) /* 2 bytes */
#define ACPI_PM_TMR_BLK		(HUDSON_ACPI_IO_BASE + 0x18) /* 4 bytes */
#define ACPI_GPE0_BLK		(HUDSON_ACPI_IO_BASE + 0x10) /* 8 bytes */
#define ACPI_CPU_CONTROL	(HUDSON_ACPI_IO_BASE + 0x08) /* 6 bytes */

#define ACPI_SMI_CTL_PORT		0xb2
#define ACPI_SMI_CMD_CST_CONTROL	0xde
#define ACPI_SMI_CMD_PST_CONTROL	0xad
#define ACPI_SMI_CMD_DISABLE		0xbe
#define ACPI_SMI_CMD_ENABLE		0xef
#define ACPI_SMI_CMD_S4_REQ		0xc0

#define REV_HUDSON_A11	0x11
#define REV_HUDSON_A12	0x12

#define SPIROM_BASE_ADDRESS_REGISTER  0xA0
#define SPI_ROM_ENABLE                0x02
#define SPI_BASE_ADDRESS              0xFEC10000

static inline int hudson_sata_enable(void)
{
	/* True if IDE or AHCI. */
	return (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 2);
}

static inline int hudson_ide_enable(void)
{
	/* True if IDE or LEGACY IDE. */
	return (CONFIG_HUDSON_SATA_MODE == 0) || (CONFIG_HUDSON_SATA_MODE == 3);
}

#ifndef __SMM__

void pm_write8(u8 reg, u8 value);
u8 pm_read8(u8 reg);
void pm_write16(u8 reg, u16 value);
u16 pm_read16(u16 reg);

#ifdef __SIMPLE_DEVICE__
void hudson_lpc_port80(void);
void hudson_pci_port80(void);
void hudson_clk_output_48Mhz(void);

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos);
int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos);

#else
void hudson_enable(struct device *dev);

#endif /* __PRE_RAM__ */
#endif /* __SMM__ */

#endif /* HUDSON_H */
