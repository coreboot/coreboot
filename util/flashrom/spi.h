/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2007, 2008 Carl-Daniel Hailfinger
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef __SPI_H__
#define __SPI_H__ 1

/*
 * Contains the generic SPI headers
 */

/* Read Electronic ID */
#define JEDEC_RDID		0x9f
#define JEDEC_RDID_OUTSIZE	0x01
#define JEDEC_RDID_INSIZE	0x03

/* AT25F512A has bit 3 as don't care bit in commands */
#define AT25F512A_RDID		0x15
#define AT25F512A_RDID_OUTSIZE	0x01
#define AT25F512A_RDID_INSIZE	0x02

/* Read Electronic Manufacturer Signature */
#define JEDEC_REMS		0x90
#define JEDEC_REMS_OUTSIZE	0x04
#define JEDEC_REMS_INSIZE	0x02

/* Read Electronic Signature */
#define JEDEC_RES		0xab
#define JEDEC_RES_OUTSIZE	0x04
#define JEDEC_RES_INSIZE	0x01

/* Write Enable */
#define JEDEC_WREN		0x06
#define JEDEC_WREN_OUTSIZE	0x01
#define JEDEC_WREN_INSIZE	0x00

/* Write Disable */
#define JEDEC_WRDI		0x04
#define JEDEC_WRDI_OUTSIZE	0x01
#define JEDEC_WRDI_INSIZE	0x00

/* Chip Erase 0x60 is supported by Macronix/SST chips. */
#define JEDEC_CE_60		0x60
#define JEDEC_CE_60_OUTSIZE	0x01
#define JEDEC_CE_60_INSIZE	0x00

/* Chip Erase 0xc7 is supported by SST/ST/EON/Macronix chips. */
#define JEDEC_CE_C7		0xc7
#define JEDEC_CE_C7_OUTSIZE	0x01
#define JEDEC_CE_C7_INSIZE	0x00

/* Block Erase 0x52 is supported by SST and old Atmel chips. */
#define JEDEC_BE_52		0x52
#define JEDEC_BE_52_OUTSIZE	0x04
#define JEDEC_BE_52_INSIZE	0x00

/* Block Erase 0xd8 is supported by EON/Macronix chips. */
#define JEDEC_BE_D8		0xd8
#define JEDEC_BE_D8_OUTSIZE	0x04
#define JEDEC_BE_D8_INSIZE	0x00

/* Sector Erase 0x20 is supported by Macronix/SST chips. */
#define JEDEC_SE		0x20
#define JEDEC_SE_OUTSIZE	0x04
#define JEDEC_SE_INSIZE		0x00

/* Read Status Register */
#define JEDEC_RDSR		0x05
#define JEDEC_RDSR_OUTSIZE	0x01
#define JEDEC_RDSR_INSIZE	0x01
#define JEDEC_RDSR_BIT_WIP	(0x01 << 0)

/* Write Status Enable */
#define JEDEC_EWSR		0x50
#define JEDEC_EWSR_OUTSIZE	0x01
#define JEDEC_EWSR_INSIZE	0x00

/* Write Status Register */
#define JEDEC_WRSR		0x01
#define JEDEC_WRSR_OUTSIZE	0x02
#define JEDEC_WRSR_INSIZE	0x00

/* Read the memory */
#define JEDEC_READ		0x03
#define JEDEC_READ_OUTSIZE	0x04
/*      JEDEC_READ_INSIZE : any length */

/* Write memory byte */
#define JEDEC_BYTE_PROGRAM	0x02
#define JEDEC_BYTE_PROGRAM_OUTSIZE	0x05
#define JEDEC_BYTE_PROGRAM_INSIZE	0x00

#endif		/* !__SPI_H__ */
