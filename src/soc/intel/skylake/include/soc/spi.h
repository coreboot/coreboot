/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#ifndef _SOC_SPI_H_
#define _SOC_SPI_H_

/*
 * SPI Opcode Menu setup for SPIBAR lockdown
 * should support most common flash chips.
 */
#define SPIDVID_OFFSET	0x0

/* Reigsters within the SPIBAR */
#define SPIBAR_SSFC		0xA1

#define SPIBAR_PREOP		0xA4
#define SPIBAR_OPTYPE		0xA6
#define SPIBAR_OPMENU_LOWER	0xA8
#define SPIBAR_OPMENU_UPPER	0xAc
/* STRAP LOCK Register */
#define SPIBAR_RESET_LOCK 0xF0
#define SPIBAR_RESET_LOCK_DISABLE	0
#define SPIBAR_RESET_LOCK_ENABLE	1
/* STRAP MSG Control Register*/
#define SPIBAR_RESET_CTRL 0xF4
#define SPIBAR_RESET_CTRL_SSMC	1
/* STRAP Data Register*/
#define SPIBAR_RESET_DATA 0xF8

#define SPI_OPMENU_0 0x01 /* WRSR: Write Status Register */
#define SPI_OPTYPE_0 0x01 /* Write, no address */

#define SPI_OPMENU_1 0x02 /* BYPR: Byte Program */
#define SPI_OPTYPE_1 0x03 /* Write, address required */

#define SPI_OPMENU_2 0x03 /* READ: Read Data */
#define SPI_OPTYPE_2 0x02 /* Read, address required */

#define SPI_OPMENU_3 0x05 /* RDSR: Read Status Register */
#define SPI_OPTYPE_3 0x00 /* Read, no address */

#define SPI_OPMENU_4 0x20 /* SE20: Sector Erase 0x20 */
#define SPI_OPTYPE_4 0x03 /* Write, address required */

#define SPI_OPMENU_5 0x9f /* RDID: Read ID */
#define SPI_OPTYPE_5 0x00 /* Read, no address */

#define SPI_OPMENU_6 0xd8 /* BED8: Block Erase 0xd8 */
#define SPI_OPTYPE_6 0x03 /* Write, address required */

#define SPI_OPMENU_7 0x0b /* FAST: Fast Read */
#define SPI_OPTYPE_7 0x02 /* Read, address required */

#define SPI_OPMENU_UPPER ((SPI_OPMENU_7 << 24) | (SPI_OPMENU_6 << 16) | \
			  (SPI_OPMENU_5 << 8) | SPI_OPMENU_4)
#define SPI_OPMENU_LOWER ((SPI_OPMENU_3 << 24) | (SPI_OPMENU_2 << 16) | \
			  (SPI_OPMENU_1 << 8) | SPI_OPMENU_0)

#define SPI_OPTYPE ((SPI_OPTYPE_7 << 14) | (SPI_OPTYPE_6 << 12) | \
		    (SPI_OPTYPE_5 << 10) | (SPI_OPTYPE_4 << 8)  | \
		    (SPI_OPTYPE_3 << 6) | (SPI_OPTYPE_2 << 4)   | \
		    (SPI_OPTYPE_1 << 2) | (SPI_OPTYPE_0))

#define SPI_OPPREFIX ((0x50 << 8) | 0x06) /* EWSR and WREN */

#define SPIBAR_HSFS		0x04	 /* SPI hardware sequence status */
#define  SPIBAR_HSFS_FLOCKDN	(1 << 15)/* Flash Configuration Lock-Down */
#define  SPIBAR_HSFS_SCIP	(1 << 5) /* SPI Cycle In Progress */
#define  SPIBAR_HSFS_AEL	(1 << 2) /* SPI Access Error Log */
#define  SPIBAR_HSFS_FCERR	(1 << 1) /* SPI Flash Cycle Error */
#define  SPIBAR_HSFS_FDONE	(1 << 0) /* SPI Flash Cycle Done */
#define  SPIBAR_HSFS_BERASE_MASK	3 /* Block/Sector Erase MASK */
#define  SPIBAR_HSFS_BERASE_OFFSET	3 /* Block/Sector Erase OFFSET */
#define SPIBAR_HSFC		0x06	 /* SPI hardware sequence control */
#define  SPIBAR_HSFC_BYTE_COUNT(c)	(((c - 1) & 0x3f) << 8)
#define  SPIBAR_HSFC_CYCLE_READ		(0 << 1) /* Read cycle */
#define  SPIBAR_HSFC_CYCLE_WRITE	(2 << 1) /* Write cycle */
#define  SPIBAR_HSFC_CYCLE_ERASE	(3 << 1) /* Erase cycle */
#define  SPIBAR_HSFC_GO		(1 << 0) /* GO: start SPI transaction */
#define SPIBAR_FADDR		0x08	 /* SPI flash address */
#define  SPIBAR_FADDR_MASK	0x7FFFFFF

#define SPIBAR_FDATA(n)		(0x10 + (4 * n)) /* SPI flash data */
#define SPIBAR_FPR(n)		(0x84 + (4 * n)) /* SPI flash protected range */
#define SPIBAR_FPR_WPE		(1 << 31) /* Flash Write protected */
#define SPIBAR_SSFS		0xA0
#define  SPIBAR_SSFS_ERROR	(1 << 3)
#define  SPIBAR_SSFS_DONE	(1 << 2)
#define SPIBAR_SSFC		0xA1
#define  SPIBAR_SSFC_DATA	(1 << 14)
#define  SPIBAR_SSFC_GO		(1 << 1)

#define SPIBAR_FDOC		0xB4
#define  SPIBAR_FDOC_COMPONENT	(1 << 12)
#define  SPIBAR_FDOC_FDSI_1	(1 << 2)

#define SPIBAR_FDOD		0xB8
#define  FLCOMP_C0DEN_MASK	0xF
#define  FLCOMP_C0DEN_8MB	4
#define  FLCOMP_C0DEN_16MB	5
#define  FLCOMP_C0DEN_32MB	6

#define SPIBAR_BIOS_CNTL	0xDC
#define  SPIBAR_BC_WPD		(1 << 0)
#define  SPIBAR_BC_EISS		(1 << 5)

void *get_spi_bar(void);
#endif
