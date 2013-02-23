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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef HUDSON_H
#define HUDSON_H

#include <device/pci_ids.h>
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

void pm_iowrite(u8 reg, u8 value);
u8 pm_ioread(u8 reg);
void pm2_iowrite(u8 reg, u8 value);
u8 pm2_ioread(u8 reg);
void set_sm_enable_bits(device_t sm_dev, u32 reg_pos, u32 mask, u32 val);

#define REV_HUDSON_A11	0x11
#define REV_HUDSON_A12	0x12

#define PM1_EVT_BLK_ADDRESS                     0x800                           //      AcpiPm1EvtBlkAddr;
#define PM1_CNT_BLK_ADDRESS                     0x804                           //      AcpiPm1CntBlkAddr;
#define PM1_TMR_BLK_ADDRESS                     0x808                           //      AcpiPmTmrBlkAddr;
#define CPU_CNT_BLK_ADDRESS                     0x810                           //      CpuControlBlkAddr;
#define GPE0_BLK_ADDRESS                        0x820                           //  AcpiGpe0BlkAddr;
#define SMI_CMD_PORT                            0xB0                            //      SmiCmdPortAddr;
#define SPIROM_BASE_ADDRESS_REGISTER    0xA0

#ifdef __PRE_RAM__
void hudson_lpc_port80(void);
void hudson_pci_port80(void);
void hudson_clk_output_48Mhz(void);

int s3_save_nvram_early(u32 dword, int size, int  nvram_pos);
int s3_load_nvram_early(int size, u32 *old_dword, int nvram_pos);

int acpi_is_wakeup_early(void);

#else
void hudson_enable(device_t dev);
void __attribute__((weak)) hudson_setup_sata_phys(struct device *dev);
#endif

#endif /* HUDSON_H */
