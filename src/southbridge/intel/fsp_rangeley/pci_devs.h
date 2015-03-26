/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
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
 * Foundation, Inc.
 */

#ifndef _RANGELEY_PCI_DEVS_H_
#define _RANGELEY_PCI_DEVS_H_

#define BUS0 0

/* All these devices live on bus 0 with the associated device and function */
#define DEV_FUNC(x,y) ((x<<3) | y)

/* Host Bridge */
#define SOC_DEV 0x0
#define SOC_FUNC 0
# define SOC_DEVID 0x1f08
# define SOC_DEV_FUNC DEV_FUNC(SOC_DEV,SOC_FUNC)

/* PCIE Port 1 */
#define PCIE_PORT1_DEV 0x1
#define PCIE_PORT1_FUNC 0
# define PCIE_PORT1_DEVID 0x1f10
# define PCIE_PORT1_DEV_FUNC DEV_FUNC(PCIE_PORT1_DEV,PCIE_PORT1_FUNC)

/* PCIE Port 2 */
#define PCIE_PORT2_DEV 0x2
#define PCIE_PORT2_FUNC 0
# define PCIE_PORT2_DEVID 0x1f11
# define PCIE_PORT2_DEV_FUNC DEV_FUNC(PCIE_PORT2_DEV,PCIE_PORT2_FUNC)

/* PCIE Port 3 */
#define PCIE_PORT3_DEV 0x3
#define PCIE_PORT3_FUNC 0
# define PCIE_PORT3_DEVID 0x1f12
# define PCIE_PORT3_DEV_FUNC DEV_FUNC(PCIE_PORT3_DEV,PCIE_PORT3_FUNC)

/* PCIE Port 4 */
#define PCIE_PORT4_DEV 0x4
#define PCIE_PORT4_FUNC 0
# define PCIE_PORT4_DEVID 0x1f13
# define PCIE_PORT4_DEV_FUNC DEV_FUNC(PCIE_PORT4_DEV,PCIE_PORT4_FUNC)

/* Host Bridge, Fabric, and RAS Registers */
#define HOST_BRIDGE_DEV 0xe
#define HOST_BRIDGE_FUNC 0
# define HOST_BRIDGE_DEVID 0x1f14
# define HOST_BRIDGE_DEV_FUNC DEV_FUNC(HOST_BRIDGE_DEV,HOST_BRIDGE_FUNC)

/* Root Complex Event Collector (RCEC) */
#define RCEC_DEV 0xf
#define RCEC_FUNC 0
# define RCEC_DEVID 0x1f16
# define RCEC_DEV_FUNC DEV_FUNC(RCEC_DEV,RCEC_FUNC)

/* SMBus 2.0 1 */
#define SMBUS1_DEV 0x13
#define SMBUS1_FUNC 0
# define SMBUS1_DEVID 0x1f15
# define SMBUS1_DEV_FUNC DEV_FUNC(SMBUS1_DEV,SMBUS1_FUNC)

/* Gigabit Ethernet (GbE) */
#define GBE_DEV 0x14
#define GBE_DEVID 0x1f41
#define GBE1_DEV GBE_DEV
#define GBE1_FUNC 0
# define GBE1_DEVID GBE_DEVID
# define GBE1_DEV_FUNC DEV_FUNC(GBE1_DEV,GBE1_FUNC)
#define GBE2_DEV GBE_DEV
#define GBE2_FUNC 1
# define GBE2_DEVID GBE_DEVID
# define GBE2_DEV_FUNC DEV_FUNC(GBE2_DEV,GBE2_FUNC)
#define GBE3_DEV GBE_DEV
#define GBE3_FUNC 2
# define GBE3_DEVID GBE_DEVID
# define GBE3_DEV_FUNC DEV_FUNC(GBE3_DEV,GBE3_FUNC)
#define GBE4_DEV GBE_DEV
#define GBE4_FUNC 3
# define GBE4_DEVID GBE_DEVID
# define GBE4_DEV_FUNC DEV_FUNC(GBE4_DEV,GBE4_FUNC)

/* USB 2.0 */
#define USB2_DEV 0x16
#define USB2_FUNC 0
# define USB2_DEVID 0x1f2c
# define USB2_DEV_FUNC DEV_FUNC(USB2_DEV,USB2_FUNC)

/* SATA Gen 2 */
#define SATA2_DEV 0x17
#define SATA2_FUNC 0
# define SATA2_DEVID 0x1f22
# define SATA2_DEV_FUNC DEV_FUNC(SATA2_DEV,SATA2_FUNC)

/* SATA Gen 3 */
#define SATA3_DEV 0x18
#define SATA3_FUNC 0
# define SATA3_DEVID 0x1f32
# define SATA3_DEV_FUNC DEV_FUNC(SATA3_DEV,SATA3_FUNC)

/* Platform Control Unit (PCU) */
#define PCU_DEV 0x1f

/* Low Pin Count (LPC/ISA) */
#define LPC_DEV PCU_DEV
#define LPC_FUNC 0
# define LPC_DEVID 0x1f38
# define LPC_DEV_FUNC DEV_FUNC(LPC_DEV,LPC_FUNC)
# define LPC_BDF PCI_DEV(BUS0, LPC_DEV, LPC_FUNC)

/* SMBus 2.0 0 */
#define SMBUS0_DEV PCU_DEV
#define SMBUS0_FUNC 3
# define SMBUS0_DEVID 0x1f3c
# define SMBUS0_DEV_FUNC DEV_FUNC(SMBUS0_DEV,SMBUS0_FUNC)

/* Intel QuickAssist Integrated Accelerator (IQIA) */
#define IQAT_DEV 0xb
#define IQAT_FUNC 0
# define IQAT_DEVID 0x1f18
# define IQAT_DEV_FUNC DEV_FUNC(IQAT_DEV,IQAT_FUNC)

#endif /* _RANGELEY_PCI_DEVS_H_ */
