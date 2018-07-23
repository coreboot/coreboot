/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
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

#ifndef __SOC_CAVIUM_CN81XX_ADDRESSMAP_H__
#define __SOC_CAVIUM_CN81XX_ADDRESSMAP_H__

#define MAX_DRAM_ADDRESS	0x2000000000ULL	/* 128GB */

/* Physical addressed with bit 47 set indicate I/O memory space. */

/* ARM code entry vector */
#define BOOTROM_OFFSET		0x100000

/* Start of IO space */
#define IO_SPACE_START		0x800000000000ULL
#define IO_SPACE_SIZE		0x100000000000ULL

/* L2C */
#define L2C_PF_BAR0		0x87E080800000ULL
#define L2C_TAD0_PF_BAR0	(0x87E050000000ULL + 0x10000)
#define L2C_TAD0_INT_W1C	(0x87E050000000ULL + 0x40000)
#define L2C_CBC0_PF_BAR0	0x87E058000000ULL
#define L2C_MCI0_PF_BAR0	0x87E05C000000ULL

/* LMC */
#define LMC0_PF_BAR0		0x87E088000000ULL
#define LMC0_DDR_PLL_CTL0 	0x258

/* OCLA */

/* IOB */
#define IOBN0_PF_BAR0		0x87E0F0000000ULL
#define MRML_PF_BAR0		0x87E0FC000000ULL

/* SMMU */
#define SMMU_PF_BAR0		0x830000000000ULL

/* GTI */
#define GTI_PF_BAR0		0x844000000000ULL

/* PCC */
#define ECAM_PF_BAR2		0x848000000000ULL
#define ECAM0_DEVX_NSDIS	0x87e048070000ULL
#define ECAM0_DEVX_SDIS		0x87e048060000ULL
#define ECAM0_RSLX_NSDIS	0x87e048050000ULL
#define ECAM0_RSLX_SDIS		0x87e048040000ULL

/* CPT */
/* SLI */

/* RST */
#define RST_PF_BAR0		(0x87E006000000ULL + 0x1600)
#define RST_PP_AVAILABLE	(RST_PF_BAR0 + 0x138ULL)
#define RST_PP_RESET		(RST_PF_BAR0 + 0x140ULL)
#define RST_PP_PENDING		(RST_PF_BAR0 + 0x148ULL)

#define FUSF_PF_BAR0		0x87E004000000ULL
#define MIO_FUS_PF_BAR0		0x87E003000000ULL
#define MIO_BOOT_PF_BAR0	0x87E000000000ULL
#define MIO_BOOT_AP_JUMP	(MIO_BOOT_PF_BAR0 + 0xD0ULL)

/* PTP */
#define MIO_PTP_PF_BAR0		0x807000000000ULL

/* GIC */
/* NIC */
/* LBK */

#define GTI_PF_BAR0		0x844000000000ULL

/* DAP */
/* BCH */
/* KEY */
/* RNG */

#define GSER0_PF_BAR0		(0x87E090000000ULL + (0 << 24))
#define GSER1_PF_BAR0		(0x87E090000000ULL + (1 << 24))
#define GSER2_PF_BAR0		(0x87E090000000ULL + (2 << 24))
#define GSER3_PF_BAR0		(0x87E090000000ULL + (3 << 24))
#define GSERx_PF_BAR0(x) \
	((((x) == 0) || ((x) == 1) || ((x) == 2) || ((x) == 3)) ? \
	 (0x87E090000000ULL + ((x) << 24)) : 0)

/* PEM */
#define PEM_PEMX_PF_BAR0(x)	(0x87e0c0000000ULL + 0x1000000ULL * (x))

/* SATA */
/* USB */

/* UAA */
#define UAA0_PF_BAR0		(0x87E028000000ULL + (0 << 24))
#define UAA1_PF_BAR0		(0x87E028000000ULL + (1 << 24))
#define UAA2_PF_BAR0		(0x87E028000000ULL + (2 << 24))
#define UAA3_PF_BAR0		(0x87E028000000ULL + (3 << 24))
#define UAAx_PF_BAR0(x) \
	((((x) == 0) || ((x) == 1) || ((x) == 2) || ((x) == 3)) ? \
	 (0x87E028000000ULL + ((x) << 24)) : 0)

#define CAVM_GICD_SETSPI_NSR	0x801000000040ULL
#define CAVM_GICD_CLRSPI_NSR	0x801000000048ULL

/* TWSI */
#define MIO_TWS0_PF_BAR0	(0x87E0D0000000ULL + (0 << 24))
#define MIO_TWS1_PF_BAR0	(0x87E0D0000000ULL + (1 << 24))
#define MIO_TWSx_PF_BAR0(x) \
	((((x) == 0) || ((x) == 1)) ? (0x87E0D0000000ULL + ((x) << 24)) : 0)

/* GPIO */
#define GPIO_PF_BAR0		0x803000000000ULL

/* SGPIO */
#define SGP_PF_BAR0		0x803000000000ULL

/* SMI */

/* SPI */
#define MPI_PF_BAR0		(0x804000000000ULL + 0x1000)

/* PCM */
/* PBUS */
/* NDF */
/* EMM */

/* VRM */
/* VRM BARs are spaced apart by 0x1000000 */
#define VRM0_PF_BAR0		0x87E021000000ULL

#endif /* __SOC_CAVIUM_CN81XX_ADDRESSMAP_H__ */
