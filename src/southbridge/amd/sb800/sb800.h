/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SB800_H
#define SB800_H

#include <device/pci_ids.h>
#include "chip.h"

/* Power management index/data registers */
#define PM_INDEX	0xcd6
#define PM_DATA		0xcd7
#define PM2_INDEX	0xcd0
#define PM2_DATA	0xcd1

#define SB800_ACPI_IO_BASE 0x800

#define ACPI_PM_EVT_BLK		(SB800_ACPI_IO_BASE + 0x00) /* 4 bytes */
#define ACPI_PM1_CNT_BLK	(SB800_ACPI_IO_BASE + 0x04) /* 2 bytes */
#define ACPI_PMA_CNT_BLK	(SB800_ACPI_IO_BASE + 0x0F) /* 1 byte */
#define ACPI_PM_TMR_BLK		(SB800_ACPI_IO_BASE + 0x18) /* 4 bytes */
#define ACPI_GPE0_BLK		(SB800_ACPI_IO_BASE + 0x10) /* 8 bytes */
#define ACPI_CPU_CONTROL	(SB800_ACPI_IO_BASE + 0x08) /* 6 bytes */

extern void pm_iowrite(u8 reg, u8 value);
extern u8 pm_ioread(u8 reg);
extern void pm2_iowrite(u8 reg, u8 value);
extern u8 pm2_ioread(u8 reg);
extern void set_sm_enable_bits(device_t sm_dev, u32 reg_pos, u32 mask, u32 val);

#define REV_SB800_A11	0x11
#define REV_SB800_A12	0x12

/* This shouldn't be called before set_sb800_revision() is called.
 * Once set_sb800_revision() is called, we use get_sb800_revision(),
 * the simpler one, to get the sb800 revision ID.
 * The id is 0x39 if A11, 0x3A if A12, 0x3C if A14, 0x3D if A15.
 * The differentiate is 0x28, isn't it? */
//#define get_sb800_revision(sm_dev)	(pci_read_config8((sm_dev), 0x08) - 0x28)

void sb800_enable(device_t dev);
void sb800_enable_usbdebug(unsigned int port);

#endif /* SB800_H */
