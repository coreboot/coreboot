/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#ifndef SOUTHBRIDGE_INTEL_I82801AX_I82801AX_H
#define SOUTHBRIDGE_INTEL_I82801AX_I82801AX_H

#if !defined(__PRE_RAM__)
#include "chip.h"
void i82801ax_enable(device_t dev);
#else
void enable_smbus(void);
int smbus_read_byte(u8 device, u8 address);
#endif

#define SMBUS_IO_BASE		0x0f00
#define PMBASE_ADDR		0x0400

#define PCI_DMA_CFG		0x90
#define SERIRQ_CNTL		0x64
#define GEN_CNTL		0xd0
#define GEN_STA			0xd4
#define RTC_CONF		0xd8
#define GEN_PMCON_3		0xa4

#define PMBASE			0x40
#define ACPI_CNTL		0x44
#define ACPI_EN			(1 << 4)
#define BIOS_CNTL		0x4E
#define GPIO_BASE		0x58 /* GPIO Base Address Register */
#define GPIO_CNTL		0x5C /* GPIO Control Register */
#define GPIO_EN			(1 << 4)

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63

#define FUNC_DIS		0xF2

#define COM_DEC			0xE0 /* LPC I/F Comm. Port Decode Ranges */
#define LPC_EN			0xE6 /* LPC IF Enables */

// TODO: FDC_DEC etc.

#define SBUS_NUM		0x19
#define SUB_BUS_NUM		0x1A
#define SMLT			0x1B
#define IOBASE			0x1C
#define IOLIM			0x1D
#define MEMBASE			0x20
#define MEMLIM			0x22
#define CNF			0x50
#define MTT			0x70
#define PCI_MAST_STS		0x82

#define TCOBASE			0x60 /* TCO Base Address Register */
#define TCO1_CNT		0x08 /* TCO1 Control Register */

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

/* IDE Timing registers (IDE_TIM) */
#define IDE_TIM_PRI		0x40	/* IDE timings, primary */
#define IDE_TIM_SEC		0x42	/* IDE timings, secondary */

/* IDE_TIM bits */
#define IDE_DECODE_ENABLE	(1 << 15)

/* SMBus */
#define SMB_BASE		0x20
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* SMBus I/O registers. */
#define SMBHSTSTAT		0x0
#define SMBHSTCTL		0x2
#define SMBHSTCMD		0x3
#define SMBXMITADD		0x4
#define SMBHSTDAT0		0x5
#define SMBHSTDAT1		0x6
#define SMBBLKDAT		0x7

#define SMBUS_TIMEOUT		(10 * 1000 * 100)

#endif
