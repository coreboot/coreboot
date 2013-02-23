/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#ifndef VX800_H
#define  VX800_H 1

#ifndef __PRE_RAM__
#include <device/device.h>
static inline void vx800_noop(device_t dev)
{
}
#endif

#ifdef __PRE_RAM__
/* vx800_early_smbus.c */
struct mem_controller;
void enable_smbus(void);
void smbus_fixup(const struct mem_controller *mem_ctrl);

/* vx800_early_serial.c */
void enable_vx800_serial(void);
#endif

//#define REV_B0 0x10
#define REV_B1 0x11
//#define REV_B2 0x12
#define REV_B3 0x13
#define REV_B4 0x14
//#define REV_B2 0xB4
#define REV_B0 0x00
#define REV_B2 0x01

/* VGA stuff */
#define SR_INDEX	0x3c4
#define SR_DATA		0x3c5
#define CRTM_INDEX	0x3b4
#define CRTM_DATA	0x3b5
#define CRTC_INDEX	0x3d4
#define CRTC_DATA	0x3d5

/* Memory Controller Registers */
#define RANK0_END		0x40
#define RANK1_END		0x41
#define RANK2_END		0x42
#define RANK3_END		0x43
#define RANK0_START		0x48
#define RANK1_START		0x49
#define RANK2_START		0x4a
#define RANK3_START		0x4b
#define DDR_PAGE_CTL		0x69
#define DRAM_REFRESH_COUNTER	0x6a
#define DRAM_MISC_CTL		0x6b
#define CH_A_DQS_OUTPUT_DELAY	0x70
#define CH_A_MD_OUTPUT_DELAY	0x71

/* RAM Init Commands */
#define RAM_COMMAND_NORMAL	0x0
#define RAM_COMMAND_NOP		0x1
#define RAM_COMMAND_PRECHARGE	0x2
#define RAM_COMMAND_MRS		0x3
#define RAM_COMMAND_CBR		0x4

/* IDE specific bits */
#define IDE_MODE_REG		0x09
#define IDE0_NATIVE_MODE	(1 << 0)
#define IDE1_NATIVE_MODE	(1 << 2)

/* These are default addresses according to Via */
#define IDE0_DATA_ADDR		0x1f0
#define IDE0_CONTROL_ADDR	0x3f4
#define IDE1_DATA_ADDR		0x170
#define IDE1_CONTROL_ADDR	0x370

/* By Award default, Via default is 0xCC0 */
#define BUS_MASTER_ADDR		0xfe00

#define CHANNEL_ENABLE_REG	0x40
#define ENABLE_IDE0		(1 << 0)
#define ENABLE_IDE1		(1 << 1)

#define VX800_ACPI_IO_BASE	0x0400

#define NB_APIC_REG 0,0,5,
#define NB_PXPTRF_REG  NB_APIC_REG
#define NB_MSGC_REG NB_APIC_REG
#define NB_HOST_REG 0,0,2,
#define NB_P6IF_REG NB_HOST_REG

#define NB_DRAMC_REG 0,0,3,
#define NB_PMU_REG 0,0,4,
#define NB_VLINK_REG 0,0,7,
#define NB_PEG_BRIDGE_REG 0,2, 0,
#define NB_D3F0_REG 0,3, 0,
#define NB_D3F1_REG 0,3, 1,

#define SB_LPC_REG 0,0x11,0,
#define SB_VLINK_REG 0,0x11,7,
#define SB_SATA_REG 0,0xf, 0,
#define SB_IDEC_REG 0,0xf, 0,
#define SB_P2PB_REG 0,0x13, 0,
#define SB_USB0_REG 0,0x10, 0,
#define SB_USB1_REG 0,0x10, 1,
#define SB_USB2_REG 0,0x10, 2,
#define SB_EHCI_REG 0,0x10, 4,

#define VX800SB_APIC_ID			0x4
#define VX800SB_APIC_DATA_OFFSET             0x10
#define VX800SB_APIC_ENTRY_NUMBER 0x40

#define VX800_D0F5_MMCONFIG_MBAR	0x61

#endif
