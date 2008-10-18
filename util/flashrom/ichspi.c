/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2008 Stefan Wildemann <stefan.wildemann@kontron.com>
 * Copyright (C) 2008 Claus Gindhart <claus.gindhart@kontron.com>
 * Copyright (C) 2008 Dominik Geyer <dominik.geyer@kontron.com>
 * Copyright (C) 2008 coresystems GmbH <info@coresystems.de>
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
 *
 */

/*
 * This module is designed for supporting the devices
 * ST M25P40
 * ST M25P80
 * ST M25P16
 * ST M25P32 already tested
 * ST M25P64
 * AT 25DF321 already tested
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <pci/pci.h>
#include "flash.h"
#include "spi.h"

/* ICH9 controller register definition */
#define ICH9_REG_FADDR         0x08	/* 32 Bits */
#define ICH9_REG_FDATA0                0x10	/* 64 Bytes */

#define ICH9_REG_SSFS          0x90	/* 08 Bits */
#define SSFS_SCIP		0x00000001
#define SSFS_CDS		0x00000004
#define SSFS_FCERR		0x00000008
#define SSFS_AEL		0x00000010

#define ICH9_REG_SSFC          0x91	/* 24 Bits */
#define SSFC_SCGO		0x00000200
#define SSFC_ACS		0x00000400
#define SSFC_SPOP		0x00000800
#define SSFC_COP		0x00001000
#define SSFC_DBC		0x00010000
#define SSFC_DS			0x00400000
#define SSFC_SME		0x00800000
#define SSFC_SCF		0x01000000
#define SSFC_SCF_20MHZ 0x00000000
#define SSFC_SCF_33MHZ 0x01000000

#define ICH9_REG_PREOP         0x94	/* 16 Bits */
#define ICH9_REG_OPTYPE                0x96	/* 16 Bits */
#define ICH9_REG_OPMENU                0x98	/* 64 Bits */

// ICH9R SPI commands
#define SPI_OPCODE_TYPE_READ_NO_ADDRESS     0
#define SPI_OPCODE_TYPE_WRITE_NO_ADDRESS    1
#define SPI_OPCODE_TYPE_READ_WITH_ADDRESS   2
#define SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS  3

// ICH7 registers
#define ICH7_REG_SPIS          0x00	/* 16 Bits */
#define SPIS_SCIP              0x00000001
#define SPIS_CDS               0x00000004
#define SPIS_FCERR             0x00000008

/* VIA SPI is compatible with ICH7, but maxdata
   to transfer is 16 bytes.

   DATA byte count on ICH7 is 8:13, on VIA 8:11

   bit 12 is port select CS0 CS1
   bit 13 is FAST READ enable
   bit 7  is used with fast read and one shot controls CS de-assert?
*/

#define ICH7_REG_SPIC          0x02	/* 16 Bits */
#define SPIC_SCGO              0x0002
#define SPIC_ACS               0x0004
#define SPIC_SPOP              0x0008
#define SPIC_DS                0x4000

#define ICH7_REG_SPIA          0x04	/* 32 Bits */
#define ICH7_REG_SPID0         0x08	/* 64 Bytes */
#define ICH7_REG_PREOP         0x54	/* 16 Bits */
#define ICH7_REG_OPTYPE                0x56	/* 16 Bits */
#define ICH7_REG_OPMENU                0x58	/* 64 Bits */

typedef struct _OPCODE {
	uint8_t opcode;		//This commands spi opcode
	uint8_t spi_type;	//This commands spi type
	uint8_t atomic;		//Use preop: (0: none, 1: preop0, 2: preop1
} OPCODE;

/* Opcode definition:
 * Preop 1: Write Enable
 * Preop 2: Write Status register enable
 *
 * OP 0: Write address
 * OP 1: Read Address
 * OP 2: ERASE block
 * OP 3: Read Status register
 * OP 4: Read ID
 * OP 5: Write Status register
 * OP 6: chip private (read JDEC id)
 * OP 7: Chip erase
 */
typedef struct _OPCODES {
	uint8_t preop[2];
	OPCODE opcode[8];
} OPCODES;

static OPCODES *curopcodes = NULL;

/* HW access functions */
static inline uint32_t REGREAD32(int X)
{
	volatile uint32_t regval;
	regval = *(volatile uint32_t *)((uint8_t *) spibar + X);
	return regval;
}

static inline uint16_t REGREAD16(int X)
{
	volatile uint16_t regval;
	regval = *(volatile uint16_t *)((uint8_t *) spibar + X);
	return regval;
}

#define REGWRITE32(X,Y) (*(uint32_t *)((uint8_t *)spibar+X)=Y)
#define REGWRITE16(X,Y) (*(uint16_t *)((uint8_t *)spibar+X)=Y)
#define REGWRITE8(X,Y)  (*(uint8_t *)((uint8_t *)spibar+X)=Y)

/* Common SPI functions */
static int program_opcodes(OPCODES * op);
static int run_opcode(uint8_t nr, OPCODE op, uint32_t offset,
		      uint8_t datalength, uint8_t * data);
static int ich_spi_read_page(struct flashchip *flash, uint8_t * buf,
			     int offset, int maxdata);
static int ich_spi_write_page(struct flashchip *flash, uint8_t * bytes,
			      int offset, int maxdata);
static int ich_spi_erase_block(struct flashchip *flash, int offset);

OPCODES O_ST_M25P = {
	{
	 JEDEC_WREN,
	 0},
	{
	 {JEDEC_BYTE_PROGRAM, SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS, 1},	// Write Byte
	 {JEDEC_READ, SPI_OPCODE_TYPE_READ_WITH_ADDRESS, 0},	// Read Data
	 {JEDEC_BE_D8, SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS, 1},	// Erase Sector
	 {JEDEC_RDSR, SPI_OPCODE_TYPE_READ_NO_ADDRESS, 0},	// Read Device Status Reg
	 {JEDEC_RES, SPI_OPCODE_TYPE_READ_WITH_ADDRESS, 0},	// Resume Deep Power-Down
	 {JEDEC_WRSR, SPI_OPCODE_TYPE_WRITE_NO_ADDRESS, 1},	// Write Status Register
	 {JEDEC_RDID, SPI_OPCODE_TYPE_READ_NO_ADDRESS, 0},	// Read JDEC ID
	 {JEDEC_CE_C7, SPI_OPCODE_TYPE_WRITE_NO_ADDRESS, 1},	// Bulk erase
	 }
};

int program_opcodes(OPCODES * op)
{
	uint8_t a;
	uint16_t preop, optype;
	uint32_t opmenu[2];

	/* Program Prefix Opcodes */
	preop = 0;
	/* 0:7 Prefix Opcode 1 */
	preop = (op->preop[0]);
	/* 8:16 Prefix Opcode 2 */
	preop |= ((uint16_t) op->preop[1]) << 8;

	/* Program Opcode Types 0 - 7 */
	optype = 0;
	for (a = 0; a < 8; a++) {
		optype |= ((uint16_t) op->opcode[a].spi_type) << (a * 2);
	}

	/* Program Allowable Opcodes 0 - 3 */
	opmenu[0] = 0;
	for (a = 0; a < 4; a++) {
		opmenu[0] |= ((uint32_t) op->opcode[a].opcode) << (a * 8);
	}

	/*Program Allowable Opcodes 4 - 7 */
	opmenu[1] = 0;
	for (a = 4; a < 8; a++) {
		opmenu[1] |= ((uint32_t) op->opcode[a].opcode) << ((a - 4) * 8);
	}

	switch (flashbus) {
	case BUS_TYPE_ICH7_SPI:
	case BUS_TYPE_VIA_SPI:
		REGWRITE16(ICH7_REG_PREOP, preop);
		REGWRITE16(ICH7_REG_OPTYPE, optype);
		REGWRITE32(ICH7_REG_OPMENU, opmenu[0]);
		REGWRITE32(ICH7_REG_OPMENU + 4, opmenu[1]);
		break;
	case BUS_TYPE_ICH9_SPI:
		REGWRITE16(ICH9_REG_PREOP, preop);
		REGWRITE16(ICH9_REG_OPTYPE, optype);
		REGWRITE32(ICH9_REG_OPMENU, opmenu[0]);
		REGWRITE32(ICH9_REG_OPMENU + 4, opmenu[1]);
		break;
	default:
		printf_debug("%s: unsupported chipset\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

static int ich7_run_opcode(uint8_t nr, OPCODE op, uint32_t offset,
			   uint8_t datalength, uint8_t * data, int maxdata)
{
	int write_cmd = 0;
	int timeout;
	uint32_t temp32 = 0;
	uint16_t temp16;
	uint32_t a;

	/* Is it a write command? */
	if ((op.spi_type == SPI_OPCODE_TYPE_WRITE_NO_ADDRESS)
	    || (op.spi_type == SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS)) {
		write_cmd = 1;
	}

	/* Programm Offset in Flash into FADDR */
	REGWRITE32(ICH7_REG_SPIA, (offset & 0x00FFFFFF));	/* SPI addresses are 24 BIT only */

	/* Program data into FDATA0 to N */
	if (write_cmd && (datalength != 0)) {
		temp32 = 0;
		for (a = 0; a < datalength; a++) {
			if ((a % 4) == 0) {
				temp32 = 0;
			}

			temp32 |= ((uint32_t) data[a]) << ((a % 4) * 8);

			if ((a % 4) == 3) {
				REGWRITE32(ICH7_REG_SPID0 + (a - (a % 4)),
					   temp32);
			}
		}
		if (((a - 1) % 4) != 3) {
			REGWRITE32(ICH7_REG_SPID0 +
				   ((a - 1) - ((a - 1) % 4)), temp32);
		}

	}

	/* Assemble SPIS */
	temp16 = 0;
	/* clear error status registers */
	temp16 |= (SPIS_CDS + SPIS_FCERR);
	REGWRITE16(ICH7_REG_SPIS, temp16);

	/* Assemble SPIC */
	temp16 = 0;

	if (datalength != 0) {
		temp16 |= SPIC_DS;
		temp16 |= ((uint32_t) ((datalength - 1) & (maxdata - 1))) << 8;
	}

	/* Select opcode */
	temp16 |= ((uint16_t) (nr & 0x07)) << 4;

	/* Handle Atomic */
	if (op.atomic != 0) {
		/* Select atomic command */
		temp16 |= SPIC_ACS;
		/* Selct prefix opcode */
		if ((op.atomic - 1) == 1) {
			/*Select prefix opcode 2 */
			temp16 |= SPIC_SPOP;
		}
	}

	/* Start */
	temp16 |= SPIC_SCGO;

	/* write it */
	REGWRITE16(ICH7_REG_SPIC, temp16);

	/* wait for cycle complete */
	timeout = 1000 * 60;	// 60s is a looong timeout.
	while (((REGREAD16(ICH7_REG_SPIS) & SPIS_CDS) == 0) && --timeout) {
		myusec_delay(1000);
	}
	if (!timeout) {
		printf_debug("timeout\n");
	}

	if ((REGREAD16(ICH7_REG_SPIS) & SPIS_FCERR) != 0) {
		printf_debug("Transaction error!\n");
		return 1;
	}

	if ((!write_cmd) && (datalength != 0)) {
		for (a = 0; a < datalength; a++) {
			if ((a % 4) == 0) {
				temp32 = REGREAD32(ICH7_REG_SPID0 + (a));
			}

			data[a] =
			    (temp32 & (((uint32_t) 0xff) << ((a % 4) * 8)))
			    >> ((a % 4) * 8);
		}
	}

	return 0;
}

static int ich9_run_opcode(uint8_t nr, OPCODE op, uint32_t offset,
			   uint8_t datalength, uint8_t * data)
{
	int write_cmd = 0;
	int timeout;
	uint32_t temp32;
	uint32_t a;

	/* Is it a write command? */
	if ((op.spi_type == SPI_OPCODE_TYPE_WRITE_NO_ADDRESS)
	    || (op.spi_type == SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS)) {
		write_cmd = 1;
	}

	/* Programm Offset in Flash into FADDR */
	REGWRITE32(ICH9_REG_FADDR, (offset & 0x00FFFFFF));	/* SPI addresses are 24 BIT only */

	/* Program data into FDATA0 to N */
	if (write_cmd && (datalength != 0)) {
		temp32 = 0;
		for (a = 0; a < datalength; a++) {
			if ((a % 4) == 0) {
				temp32 = 0;
			}

			temp32 |= ((uint32_t) data[a]) << ((a % 4) * 8);

			if ((a % 4) == 3) {
				REGWRITE32(ICH9_REG_FDATA0 + (a - (a % 4)),
					   temp32);
			}
		}
		if (((a - 1) % 4) != 3) {
			REGWRITE32(ICH9_REG_FDATA0 +
				   ((a - 1) - ((a - 1) % 4)), temp32);
		}

	}

	/* Assemble SSFS + SSFC */
	temp32 = 0;

	/* clear error status registers */
	temp32 |= (SSFS_CDS + SSFS_FCERR);
	/* USE 20 MhZ */
	temp32 |= SSFC_SCF_20MHZ;

	if (datalength != 0) {
		uint32_t datatemp;
		temp32 |= SSFC_DS;
		datatemp = ((uint32_t) ((datalength - 1) & 0x3f)) << (8 + 8);
		temp32 |= datatemp;
	}

	/* Select opcode */
	temp32 |= ((uint32_t) (nr & 0x07)) << (8 + 4);

	/* Handle Atomic */
	if (op.atomic != 0) {
		/* Select atomic command */
		temp32 |= SSFC_ACS;
		/* Selct prefix opcode */
		if ((op.atomic - 1) == 1) {
			/*Select prefix opcode 2 */
			temp32 |= SSFC_SPOP;
		}
	}

	/* Start */
	temp32 |= SSFC_SCGO;

	/* write it */
	REGWRITE32(ICH9_REG_SSFS, temp32);

	/*wait for cycle complete */
	timeout = 1000 * 60;	// 60s is a looong timeout.
	while (((REGREAD32(ICH9_REG_SSFS) & SSFS_CDS) == 0) && --timeout) {
		myusec_delay(1000);
	}
	if (!timeout) {
		printf_debug("timeout\n");
	}

	if ((REGREAD32(ICH9_REG_SSFS) & SSFS_FCERR) != 0) {
		printf_debug("Transaction error!\n");
		return 1;
	}

	if ((!write_cmd) && (datalength != 0)) {
		for (a = 0; a < datalength; a++) {
			if ((a % 4) == 0) {
				temp32 = REGREAD32(ICH9_REG_FDATA0 + (a));
			}

			data[a] =
			    (temp32 & (((uint32_t) 0xff) << ((a % 4) * 8)))
			    >> ((a % 4) * 8);
		}
	}

	return 0;
}

static int run_opcode(uint8_t nr, OPCODE op, uint32_t offset,
		      uint8_t datalength, uint8_t * data)
{
	switch (flashbus) {
	case BUS_TYPE_VIA_SPI:
		return ich7_run_opcode(nr, op, offset, datalength, data, 16);
	case BUS_TYPE_ICH7_SPI:
		return ich7_run_opcode(nr, op, offset, datalength, data, 64);
	case BUS_TYPE_ICH9_SPI:
		return ich9_run_opcode(nr, op, offset, datalength, data);
	default:
		printf_debug("%s: unsupported chipset\n", __FUNCTION__);
	}

	/* If we ever get here, something really weird happened */
	return -1;
}

static int ich_spi_erase_block(struct flashchip *flash, int offset)
{
	printf_debug("ich_spi_erase_block: offset=%d, sectors=%d\n", offset, 1);

	if (run_opcode(2, curopcodes->opcode[2], offset, 0, NULL) != 0) {
		printf_debug("Error erasing sector at 0x%x", offset);
		return -1;
	}

	printf("DONE BLOCK 0x%x\n", offset);

	return 0;
}

static int ich_spi_read_page(struct flashchip *flash, uint8_t * buf, int offset,
			     int maxdata)
{
	int page_size = flash->page_size;
	uint32_t remaining = flash->page_size;
	int a;

	printf_debug("ich_spi_read_page: offset=%d, number=%d, buf=%p\n",
		     offset, page_size, buf);

	for (a = 0; a < page_size; a += maxdata) {
		if (remaining < maxdata) {

			if (run_opcode
			    (1, curopcodes->opcode[1],
			     offset + (page_size - remaining), remaining,
			     &buf[page_size - remaining]) != 0) {
				printf_debug("Error reading");
				return 1;
			}
			remaining = 0;
		} else {
			if (run_opcode
			    (1, curopcodes->opcode[1],
			     offset + (page_size - remaining), maxdata,
			     &buf[page_size - remaining]) != 0) {
				printf_debug("Error reading");
				return 1;
			}
			remaining -= maxdata;
		}
	}

	return 0;
}

static int ich_spi_write_page(struct flashchip *flash, uint8_t * bytes,
			      int offset, int maxdata)
{
	int page_size = flash->page_size;
	uint32_t remaining = page_size;
	int a;

	printf_debug("ich_spi_write_page: offset=%d, number=%d, buf=%p\n",
		     offset, page_size, bytes);

	for (a = 0; a < page_size; a += maxdata) {
		if (remaining < maxdata) {
			if (run_opcode
			    (0, curopcodes->opcode[0],
			     offset + (page_size - remaining), remaining,
			     &bytes[page_size - remaining]) != 0) {
				printf_debug("Error writing");
				return 1;
			}
			remaining = 0;
		} else {
			if (run_opcode
			    (0, curopcodes->opcode[0],
			     offset + (page_size - remaining), maxdata,
			     &bytes[page_size - remaining]) != 0) {
				printf_debug("Error writing");
				return 1;
			}
			remaining -= maxdata;
		}
	}

	return 0;
}

int ich_spi_read(struct flashchip *flash, uint8_t * buf)
{
	int i, rc = 0;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	int maxdata = 64;

	if (flashbus == BUS_TYPE_VIA_SPI) {
		maxdata = 16;
	}

	for (i = 0; (i < total_size / page_size) && (rc == 0); i++) {
		rc = ich_spi_read_page(flash, (void *)(buf + i * page_size),
				       i * page_size, maxdata);
	}

	return rc;
}

int ich_spi_write(struct flashchip *flash, uint8_t * buf)
{
	int i, j, rc = 0;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	int erase_size = 64 * 1024;
	int maxdata = 64;

	spi_disable_blockprotect();

	printf("Programming page: \n");

	for (i = 0; i < total_size / erase_size; i++) {
		rc = ich_spi_erase_block(flash, i * erase_size);
		if (rc) {
			printf("Error erasing block at 0x%x\n", i);
			break;
		}

		if (flashbus == BUS_TYPE_VIA_SPI)
			maxdata = 16;

		for (j = 0; j < erase_size / page_size; j++) {
			ich_spi_write_page(flash,
			   (void *)(buf + (i * erase_size) + (j * page_size)),
			   (i * erase_size) + (j * page_size), maxdata);
		}
	}

	printf("\n");

	return rc;
}

int ich_spi_command(unsigned int writecnt, unsigned int readcnt,
		    const unsigned char *writearr, unsigned char *readarr)
{
	int a;
	int opcode_index = -1;
	const unsigned char cmd = *writearr;
	OPCODE *opcode;
	uint32_t addr = 0;
	uint8_t *data;
	int count;

	/* program opcodes if not already done */
	if (curopcodes == NULL) {
		printf_debug("Programming OPCODES... ");
		curopcodes = &O_ST_M25P;
		program_opcodes(curopcodes);
		printf_debug("done\n");
	}

	/* find cmd in opcodes-table */
	for (a = 0; a < 8; a++) {
		if ((curopcodes->opcode[a]).opcode == cmd) {
			opcode_index = a;
			break;
		}
	}

	/* unknown / not programmed command */
	if (opcode_index == -1) {
		printf_debug("Invalid OPCODE 0x%02x\n", cmd);
		return 1;
	}

	opcode = &(curopcodes->opcode[opcode_index]);

	/* if opcode-type requires an address */
	if (opcode->spi_type == SPI_OPCODE_TYPE_READ_WITH_ADDRESS ||
	    opcode->spi_type == SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS) {
		addr = (writearr[1] << 16) |
		    (writearr[2] << 8) | (writearr[3] << 0);
	}

	/* translate read/write array/count */
	if (opcode->spi_type == SPI_OPCODE_TYPE_WRITE_NO_ADDRESS) {
		data = (uint8_t *) (writearr + 1);
		count = writecnt - 1;
	} else if (opcode->spi_type == SPI_OPCODE_TYPE_WRITE_WITH_ADDRESS) {
		data = (uint8_t *) (writearr + 4);
		count = writecnt - 4;
	} else {
		data = (uint8_t *) readarr;
		count = readcnt;
	}

	if (run_opcode(opcode_index, *opcode, addr, count, data) != 0) {
		printf_debug("run OPCODE 0x%02x failed\n", opcode->opcode);
		return 1;
	}

	return 0;
}
