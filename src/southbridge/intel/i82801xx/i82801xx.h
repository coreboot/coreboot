/*
 * This file is part of the LinuxBIOS project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef I82801XX_H
#define I82801XX_H

#ifndef __ROMCC__
#include "chip.h"
extern void i82801xx_enable(device_t dev);
#endif

#define PCI_DMA_CFG     0x90
#define SERIRQ_CNTL     0x64
#define GEN_CNTL        0xd0
#define GEN_STS         0xd4
#define RTC_CONF        0xd8
#define GEN_PMCON_3     0xa4

#define PCICMD          0x04
#define PMBASE          0x40
#define PM_BASE_ADDR	0x1100
#define ACPI_CNTL       0x44
#define BIOS_CNTL       0x4E
#define GPIO_BASE	0x58
#define GPIO_BASE_ADDR	0x1180
#define GPIO_CNTL       0x5C
#define PIRQA_ROUT      0x60
#define PIRQE_ROUT      0x68
#define COM_DEC         0xE0
#define LPC_EN          0xE6
#define FUNC_DIS        0xF2

#define CMD		0x04
#define SBUS_NUM	0x19
#define SUB_BUS_NUM	0x1A
#define SMLT		0x1B
#define IOBASE		0x1C
#define IOLIM		0x1D
#define MEMBASE		0x20
#define MEMLIM		0x22
#define CNF		0x50
#define MTT		0x70
#define PCI_MAST_STS	0x82

// GEN_PMCON_3 bits
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

// PCI Configuration Space (D31:F1)
#define IDE_TIM_PRI		0x40	// IDE timings, primary
#define IDE_TIM_SEC		0x42	// IDE timings, secondary

// IDE_TIM bits
#define IDE_DECODE_ENABLE	(1 << 15)

// PCI Configuration Space (D31:F3)
#define SMB_BASE	0x20
#define HOSTC		0x40

// HOSTC bits
#define I2C_EN		(1 << 2)
#define SMB_SMI_EN	(1 << 1)
#define HST_EN		(1 << 0)

// SMBus IO bits
/* TODO: Does it matter where we put the SMBus IO base, as long as we keep 
   consistent and don't interfere with anything else? */
//#define SMBUS_IO_BASE 0x1000
#define SMBUS_IO_BASE	0x0f00

#define SMBHSTSTAT	0x0
#define SMBHSTCTL	0x2
#define SMBHSTCMD	0x3
#define SMBXMITADD	0x4
#define SMBHSTDAT0	0x5
#define SMBHSTDAT1	0x6
#define SMBBLKDAT	0x7
#define SMBTRNSADD	0x9
#define SMBSLVDATA	0xa
#define SMLINK_PIN_CTL	0xe
#define SMBUS_PIN_CTL	0xf 

#define SMBUS_TIMEOUT	(10 * 1000 * 100)

//HPET, if present
#define HPET_ADDR		0xfed0000

#endif /* I82801XX_H */
