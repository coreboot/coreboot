/*
 *  linux/include/linux/mtd/nand.h
 *
 *  Copyright (c) 2000 David Woodhouse <dwmw2@mvhi.com>
 *                     Steven J. Hill <sjhill@cotw.com>
 *
 * $Id$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Info:
 *   Contains standard defines and IDs for NAND flash devices
 *
 *  Changelog:
 *   01-31-2000 DMW     Created
 *   09-18-2000 SJH     Moved structure out of the Disk-On-Chip drivers
 *			so it can be used by other NAND flash device
 *			drivers. I also changed the copyright since none
 *			of the original contents of this file are specific
 *			to DoC devices. David can whack me with a baseball
 *			bat later if I did something naughty.
 *   10-11-2000 SJH     Added private NAND flash structure for driver
 */
#ifndef __LINUX_MTD_NAND_H
#define __LINUX_MTD_NAND_H


/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_RESET		0xff

/*
 * Enumeration for NAND flash chip state
 */
typedef enum {
	FL_READY,
	FL_READING,
	FL_WRITING,
	FL_ERASING,
	FL_SYNCING
} nand_state_t;

/*
 * NAND Private Flash Chip Data
 *
 * Structure overview:
 *
 *  IO_ADDR - address to access the 8 I/O lines to the flash device
 *
 *  CTRL_ADDR - address where ALE, CLE and CE control bits are accessed
 *
 *  CLE - location in control word for Command Latch Enable bit
 *
 *  ALE - location in control word for Address Latch Enable bit
 *
 *  NCE - location in control word for nChip Enable bit
 *
 *  chip_lock - spinlock used to protect access to this structure
 *
 *  wq - wait queue to sleep on if a NAND operation is in progress
 *
 *  state - give the current state of the NAND device
 *
 *  page_shift - number of address bits in a page (column address bits)
 *
 *  data_buf - data buffer passed to/from MTD user modules
 *
 *  ecc_code_buf - used only for holding calculated or read ECCs for
 *                 a page read or written when ECC is in use
 *
 *  reserved - padding to make structure fall on word boundary if
 *             when ECC is in use
 */

/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec

/*
 * NAND Flash Device ID Structure
 *
 * Structure overview:
 *
 *  name - Complete name of device
 *
 *  manufacture_id - manufacturer ID code of device.
 *
 *  model_id - model ID code of device.
 *
 *  chipshift - total number of address bits for the device which
 *              is used to calculate address offsets and the total
 *              number of bytes the device is capable of.
 *
 *  page256 - denotes if flash device has 256 byte pages or not.
 *
 *  pageadrlen - number of bytes minus one needed to hold the
 *               complete address into the flash array. Keep in
 *               mind that when a read or write is done to a
 *               specific address, the address is input serially
 *               8 bits at a time. This structure member is used
 *               by the read/write routines as a loop index for
 *               shifting the address out 8 bits at a time.
 *
 *  erasesize - size of an erase block in the flash device.
 */
struct nand_flash_dev {
	char * name;
	int manufacture_id;
	int model_id;
	int chipshift;
	char page256;
	char pageadrlen;
	unsigned long erasesize;
};

#endif /* __LINUX_MTD_NAND_H */
