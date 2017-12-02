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

/* CPT */
/* SLI */

/* RST */
#define RST_PF_BAR0		(0x87E006000000ULL + 0x1600)
#define FUSF_PF_BAR0		0x87E004000000ULL
#define MIO_FUS_PF_BAR0		0x87E003000000ULL
#define MIO_BOOT_PF_BAR0	0x87E000000000ULL

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
