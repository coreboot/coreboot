/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

#ifndef NORTHBRIDGE_H
#define NORTHBRIDGE_H

#define	NB			PCI_DEV(0, 0, 0)
#define	NB_REG_SPI_BASE		0x40
#define	NB_REG_CLK_OUT_CTL	0x48
#define	NB_REG_PCI_CLK_CTL	0x4b
#define	NB_REG_STRAP		0x60
#define	NB_REG_STRAP2		0x64
#define	NB_REG_MBR		0x6c
#define	NB_REG_DDR3_CFG		0x74
#define	NB_REG_DDR3_MTR1	0x78
#define	NB_REG_DDR3_MTR2	0x7c
#define	NB_REG_SMM		0x83
#define	NB_REG_MAR		0x84
#define	NB_REG_CID		0x90
#define	NB_REG_S1R		0x94
#define	NB_REG_S2R		0x98
#define	NB_REG_S3R		0x9c
#define	NB_REG_HOST_CTL		0xa0
#define	NB_REG_CPU_MBCR		0xc4
#define	NB_REG_CDR		0xd0
#define	NB_REG_PACR		0xf0
#define	NB_REG_PMCR		0xf4
#define	NB_REG_PCI_TARGET	0xf8
#define	NB_REG_PCSCR		0xfc

/* Additional "virtual" device, just extension of NB */
#define NB1			PCI_DEV(0, 0, 1)
#define NB1_REG_FJZ_PHY_CTL1	0x80
#define NB1_REG_FJZ_PHY_CTL2	0x84
#define NB1_REG_FJZ_PHY_CTL3	0x88
#define NB1_REG_FJZ_DRAM_CTL1	0x90
#define NB1_REG_FJZ_DRAM_CTL2	0x94
#define NB1_REG_FJZ_DRAM_CTL3	0x98
#define NB1_REG_FJZ_DRAM_CTL4	0x9c
#define NB1_REG_PLL_TEST_CTL	0xa8
#define NB1_REG_DDR3_PWR_SAV	0xbc
#define NB1_REG_DDR3_CTL_OPT1	0xc0
#define NB1_REG_DDR3_CTL_OPT3	0xc8
#define NB1_REG_DDR3_CTL_OPT4	0xcc
#define NB1_REG_DDR3_CTL_OPT5	0xce
#define NB1_REG_PLL_TEST_MODE	0xd0
#define NB1_REG_L2_CACHE_CTL	0xe8
#define NB1_REG_SSCR		0xec
#define NB1_REG_NB_CTL_OPT1	0xf4
#define NB1_REG_UPDATE_PHY_IO	0xf8
#define NB1_REG_RESET_DRAMC_PHY	0xfa

#endif				/* NORTHBRIDGE_H */
