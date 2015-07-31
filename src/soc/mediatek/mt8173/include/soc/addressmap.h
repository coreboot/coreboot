/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#ifndef __SOC_MEDIATEK_MT8173_INCLUDE_SOC_ADDRESS_MAP_H__
#define __SOC_MEDIATEK_MT8173_INCLUDE_SOC_ADDRESS_MAP_H__

#include <stddef.h>
#include <stdint.h>

enum {
	VER_BASE	= 0x08000000,
	IO_PHYS		= 0x10000000,
	DDR_BASE	= 0x40000000
};

enum {
	CKSYS_BASE		= IO_PHYS,
	INFRACFG_AO_BASE	= IO_PHYS + 0x1000,
	PERI_CON_BASE		= IO_PHYS + 0x3000,
	CHA_DRAMCAO_BASE	= IO_PHYS + 0x4000,
	GPIO_BASE		= IO_PHYS + 0x5000,
	SPM_BASE		= IO_PHYS + 0x6000,
	RGU_BASE		= IO_PHYS + 0x7000,
	GPT_BASE		= IO_PHYS + 0x8000,
	PMIC_WRAP_BASE		= IO_PHYS + 0xD000,
	CHA_DDRPHY_BASE		= IO_PHYS + 0xF000,
	CHB_DRAMCAO_BASE	= IO_PHYS + 0x11000,
	CHB_DDRPHY_BASE		= IO_PHYS + 0x12000,
	MCUCFG_BASE		= IO_PHYS + 0x200000,
	EMI_BASE		= IO_PHYS + 0x203000,
	EFUSE_BASE		= IO_PHYS + 0x206000,
	APMIXED_BASE		= IO_PHYS + 0x209000,
	CHA_DRAMCNAO_BASE	= IO_PHYS + 0x20E000,
	CHB_DRAMCNAO_BASE	= IO_PHYS + 0x213000,
	ANA_MIPI_CS1_BASE	= IO_PHYS + 0x218000,
	UART0_BASE		= IO_PHYS + 0x1002000,
	SPI_BASE		= IO_PHYS + 0x100A000,
	I2C_BASE		= IO_PHYS + 0x1007000,
	I2C_DMA_BASE		= IO_PHYS + 0x1000080,
	SFLASH_REG_BASE		= IO_PHYS + 0x100D000,
	SSUSB_MAC_BASE		= IO_PHYS + 0x1270000,
	SSUSB_IPPC_BASE		= IO_PHYS + 0x1280700,
	SSUSB_SIF_BASE		= IO_PHYS + 0x1290800,
};

#endif /* __SOC_MEDIATEK_MT8173_INCLUDE_SOC_ADDRESS_MAP_H___ */
