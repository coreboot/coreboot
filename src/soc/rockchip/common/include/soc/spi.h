/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#ifndef __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_SPI_H
#define __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_SPI_H

/* This driver serves as a CBFS media source. */
#include <spi-generic.h>
#include <stddef.h>

struct rockchip_spi {
	u32  ctrlr0;
	u32  ctrlr1;
	u32  spienr;
	u32  ser;
	u32  baudr;
	u32  txftlr;
	u32  rxftlr;
	u32  txflr;
	u32  rxflr;
	u32  sr;
	u32  ipr;
	u32  imr;
	u32  isr;
	u32  risr;
	u32  icr;
	u32  dmacr;
	u32  damtdlr;
	u32  damrdlr;
	u32  reserved[(0x400-0x48)/4];
	u32  txdr[0x100];
	u32  rxdr[0x100];
};
check_member(rockchip_spi, rxdr, 0x800);


#define SF_READ_DATA_CMD	0x3

/* --------Bit fields in CTRLR0--------begin */

#define SPI_DFS_OFFSET	0	/* Data Frame Size */
#define SPI_DFS_MASK	0x3
#define SPI_DFS_4BIT	0x00
#define SPI_DFS_8BIT	0x01
#define SPI_DFS_16BIT	0x02
#define SPI_DFS_RESV	0x03

/* Control Frame Size */
#define SPI_CFS_OFFSET	2
#define SPI_CFS_MASK	0xF

/* Serial Clock Phase */
#define SPI_SCPH_OFFSET	6
#define SPI_SCPH_MASK	0x1

/* Serial clock toggles in middle of first data bit */
#define SPI_SCPH_TOGMID	0

/* Serial clock toggles at start of first data bit */
#define SPI_SCPH_TOGSTA	1

/* Serial Clock Polarity */
#define SPI_SCOL_OFFSET	7
#define SPI_SCOL_MASK	0x1

/* Inactive state of clock serial clock is low */
#define SPI_SCOL_LOW	0

/* Inactive state of clock serial clock is high */
#define SPI_SCOL_HIGH	1

/* Chip Select Mode */
#define SPI_CSM_OFFSET	8
#define SPI_CSM_MASK	0x3

/* ss_n keep low after every frame data is transferred */
#define SPI_CSM_KEEP	0x00

/*
 * ss_n be high for half sclk_out cycles after
 * every frame data is transferred
 */
#define SPI_CSM_HALF	0x01

/* ss_n be high for one sclk_out cycle after every frame data is transferred */
#define SPI_CSM_ONE	0x02
#define SPI_CSM_RESV	0x03

/* SSN to Sclk_out delay */
#define SPI_SSN_DELAY_OFFSET	10
#define SPI_SSN_DELAY_MASK	0x1
/* the peroid between ss_n active and sclk_out active is half sclk_out cycles */
#define SPI_SSN_DELAY_HALF	0x00
/* the peroid between ss_n active and sclk_out active is one sclk_out cycle */
#define SPI_SSN_DELAY_ONE	0x01

/* Serial Endian Mode */
#define SPI_SEM_OFFSET	11
#define SPI_SEM_MASK	0x1
/* little endian */
#define SPI_SEM_LITTLE	0x00
/* big endian */
#define SPI_SEM_BIG	0x01

/* First Bit Mode */
#define SPI_FBM_OFFSET	12
#define SPI_FBM_MASK	0x1
/* first bit in MSB */
#define SPI_FBM_MSB	0x00
/* first bit in LSB */
#define SPI_FBM_LSB	0x01

/* Byte and Halfword Transform */
#define SPI_HALF_WORLD_TX_OFFSET 13
#define SPI_HALF_WORLD_MASK	0x1
/* apb 16bit write/read, spi 8bit write/read */
#define SPI_APB_16BIT	0x00
/* apb 8bit write/read, spi 8bit write/read */
#define SPI_APB_8BIT	0x01

/* Rxd Sample Delay */
#define SPI_RXDSD_OFFSET	14
#define SPI_RXDSD_MASK	0x3

/* Frame Format */
#define SPI_FRF_OFFSET	16
#define SPI_FRF_MASK	0x3
/* motorola spi */
#define SPI_FRF_SPI	0x00
/* Texas Instruments SSP*/
#define SPI_FRF_SSP	0x01
/*  National Semiconductors Microwire */
#define SPI_FRF_MICROWIRE	0x02
#define SPI_FRF_RESV	0x03

/* Transfer Mode */
#define SPI_TMOD_OFFSET	18
#define SPI_TMOD_MASK	0x3
/* xmit & recv */
#define	SPI_TMOD_TR	0x00
/* xmit only */
#define SPI_TMOD_TO	0x01
/* recv only */
#define SPI_TMOD_RO	0x02
#define SPI_TMOD_RESV	0x03

/* Operation Mode */
#define SPI_OMOD_OFFSET	20
#define SPI_OMOD_MASK	0x1
/* Master Mode */
#define	SPI_OMOD_MASTER	0x00
/* Slave Mode */
#define SPI_OMOD_SLAVE	0x01

/* --------Bit fields in CTRLR0--------end */
/* Bit fields in SR, 7 bits */
#define SR_MASK	0x7f
#define SR_BUSY	(1 << 0)
#define SR_TF_FULL	(1 << 1)
#define SR_TF_EMPT	(1 << 2)
#define SR_RF_EMPT	(1 << 3)
#define SR_RF_FULL	(1 << 4)

/* Bit fields in ISR, IMR, RISR, 7 bits */
#define SPI_INT_TXEI	(1 << 0)
#define SPI_INT_TXOI	(1 << 1)
#define SPI_INT_RXUI	(1 << 2)
#define SPI_INT_RXOI	(1 << 3)
#define SPI_INT_RXFI	(1 << 4)

/* Bit fields in DMACR */
#define SPI_DMACR_TX_ENABLE	(1 << 1)
#define SPI_DMACR_RX_ENABLE	(1 << 0)

/* Bit fields in ICR */
#define SPI_CLEAR_INT_ALL	(1 << 0)
#define SPI_CLEAR_INT_RXUI	(1 << 1)
#define SPI_CLEAR_INT_RXOI	(1 << 2)
#define SPI_CLEAR_INT_TXOI	(1 << 3)

void rockchip_spi_init(unsigned int bus, unsigned int speed_hz);

#endif  /* ! __COREBOOT_SRC_SOC_ROCKCHIP_COMMON_INCLUDE_SOC_SPI_H */
