/*
 * flash.h: flash programming utility - central include file
 *
 * Copyright 2000 Silicon Integrated System Corporation
 * Copyright 2000 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright 2005-2007 coresystems GmbH <stepan@coresystems.de>
 * 
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef __FLASH_H__
#define __FLASH_H__ 1

#if defined(__GLIBC__)
#include <sys/io.h>
#endif

#include <unistd.h>
#include <stdint.h>

struct flashchip {
	char *name;
	int manufacture_id;
	int model_id;

	int total_size;
	int page_size;

	int (*probe) (struct flashchip *flash);
	int (*erase) (struct flashchip *flash);
	int (*write) (struct flashchip *flash, uint8_t *buf);
	int (*read) (struct flashchip *flash, uint8_t *buf);

	/* some flash devices have an additional
	 * register space
	 */
	volatile uint8_t *virtual_memory;
	volatile uint8_t *virtual_registers;
};

extern struct flashchip flashchips[];

/* Please keep this list sorted alphabetically by manufacturer. The first
 * entry of each section should be the manufacturer ID, followed by the
 * list of devices from that manufacturer (sorted by device IDs).
 */

#define AMD_ID			0x01	/* AMD */
#define AM_29F040B		0xA4
#define AM_29F016D		0xAD

#define ASD_ID			0x25	/* ASD */
#define ASD_AE49F2008		0x52

#define ATMEL_ID		0x1F	/* Atmel */
#define AT_29C040A		0xA4
#define AT_29C020		0xDA

#define MX_ID			0xC2	/* Macronix (MX) */
#define MX_29F002		0xB0

#define SHARP_ID		0xB0	/* Sharp */
#define SHARP_LHF00L04		0xCF

#define SST_ID			0xBF	/* SST */
#define SST_29EE020A		0x10
#define SST_28SF040		0x04
#define SST_39SF010		0xB5
#define SST_39SF020		0xB6
#define SST_39SF040		0xB7
#define SST_39VF020		0xD6
#define SST_49LF040B		0x50
#define SST_49LF040		0x51
#define SST_49LF020A		0x52
#define SST_49LF080A		0x5B
#define SST_49LF002A		0x57
#define SST_49LF003A		0x1B
#define SST_49LF004A		0x60
#define SST_49LF008A		0x5A
#define SST_49LF004C		0x54
#define SST_49LF008C		0x59
#define SST_49LF016C		0x5C
#define SST_49LF160C		0x4C

#define PMC_ID			0x9D	/* PMC */
#define PMC_49FL002		0x6D
#define PMC_49FL004		0x6E

#define WINBOND_ID		0xDA	/* Winbond */
#define W_29C011		0xC1
#define W_29C020C		0x45
#define W_39V040FA		0x34
#define W_39V040A		0x3D
#define W_39V040B		0x54
#define W_39V080A		0xD0
#define W_49F002U		0x0B
#define W_49V002A		0xB0
#define W_49V002FA		0x32

#define ST_ID			0x20	/* ST */
#define ST_M29F002B		0x34
#define ST_M29F002T		0xB0	/* M29F002T / M29F002NT */
#define ST_M29F400BT		0xD5
#define ST_M29F040B		0xE2

#define EMST_ID			0x8c	/* EMST / EFST */
#define EMST_F49B002UA		0x00

#define MSYSTEMS_ID		0x156f	/* M-Systems */
#define MSYSTEMS_MD2200		0xdb	/* ? */
#define MSYSTEMS_MD2800		0x30	/* hmm -- both 0x30 */
#define MSYSTEMS_MD2802		0x30	/* hmm -- both 0x30 */

#define SYNCMOS_ID		0x40	/* SyncMOS */
#define S29C51001T		0x01
#define S29C51002T		0x02
#define S29C51004T		0x03
#define S29C31004T		0x63

/* function prototypes from udelay.h */

void myusec_delay(int time);
void myusec_calibrate_delay();

/* pci handling for board/chipset_enable */
struct pci_access *pacc;	/* For board and chipset_enable */
struct pci_dev *pci_dev_find(uint16_t vendor, uint16_t device);
struct pci_dev *pci_card_find(uint16_t vendor, uint16_t device,
			      uint16_t card_vendor, uint16_t card_device);

int board_flash_enable(char *vendor, char *part);	/* board_enable.c */
int chipset_flash_enable(void);	/* chipset_enable.c */

/* physical memory mapping device */

#if defined (__sun) && (defined(__i386) || defined(__amd64))
#  define MEM_DEV "/dev/xsvc"
#else
#  define MEM_DEV "/dev/mem"
#endif

extern int fd_mem;

int map_flash_registers(struct flashchip *flash); /* flashrom.c */

#endif				/* !__FLASH_H__ */
