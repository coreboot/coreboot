/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 * Copyright (C) 2000 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005-2007 coresystems GmbH <stepan@coresystems.de>
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
 */

#ifndef __FLASH_H__
#define __FLASH_H__ 1

#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

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
 * All LPC/FWH parts (parallel flash) have 8-bit device IDs.
 * All SPI parts have 16-bit device IDs.
 */

#define ALLIANCE_ID		0x52	/* Alliance */

#define AMD_ID			0x01	/* AMD */
#define AM_29F040B		0xA4
#define AM_29LV040B		0x4F
#define AM_29F016D		0xAD

#define AMIC_ID			0x37	/* AMIC */

#define ASD_ID			0x25	/* ASD */
#define ASD_AE49F2008		0x52

#define ATMEL_ID		0x1F	/* Atmel */
#define AT_29C040A		0xA4
#define AT_29C020		0xDA

#define CATALYST_ID		0x31	/* Catalyst */

#define EMST_ID			0x8C	/* EMST / EFST */
#define EMST_F49B002UA		0x00

#define EON_ID			0x1C	/* EON */
/* EN25 chips are SPI, first byte of device id is memory type,
 * second byte of device id is log(bitsize)-9. */
#define EN_25B05		0x2010	/* 2^19 kbit or 2^16 kByte */
#define EN_25B10		0x2011
#define EN_25B20		0x2012
#define EN_25B40		0x2013
#define EN_25B80		0x2014
#define EN_25B16		0x2015
#define EN_25B32		0x2016

#define FUJITSU_ID		0x04	/* Fujitsu */

#define HYUNDAI_ID		0xAD	/* Hyundai */

#define IMT_ID			0x7F	/* IMT */

#define INTEL_ID		0x89	/* Intel */

#define ISSI_ID			0xD5	/* ISSI */

#define MSYSTEMS_ID		0x156F	/* M-Systems */
#define MSYSTEMS_MD2200		0xDB	/* ? */
#define MSYSTEMS_MD2800		0x30	/* hmm -- both 0x30 */
#define MSYSTEMS_MD2802		0x30	/* hmm -- both 0x30 */

#define MX_ID			0xC2	/* Macronix (MX) */
/* MX25 chips are SPI, first byte of device id is memory type,
 * second byte of device id is log(bitsize)-9. */
#define MX_25L512		0x2010	/* 2^19 kbit or 2^16 kByte */
#define MX_25L1005		0x2011
#define MX_25L2005		0x2012
#define MX_25L4005		0x2013	/* MX25L4005{,A} */
#define MX_25L8005		0x2014
#define MX_25L1605		0x2015	/* MX25L1605{,A,D} */
#define MX_25L3205		0x2016	/* MX25L3205{,A} */
#define MX_25L6405		0x2017	/* MX25L3205{,D} */
#define MX_25L1635D		0x2415
#define MX_25L3235D		0x2416
#define MX_29F002		0xB0

#define PMC_ID			0x9D	/* PMC */
#define PMC_49FL002		0x6D
#define PMC_49FL004		0x6E

#define SHARP_ID		0xB0	/* Sharp */
#define SHARP_LHF00L04		0xCF

#define SST_ID			0xBF	/* SST */
/* SST25 chips are SPI, first byte of device id is memory type, second
 * byte of device id is related to log(bitsize) at least for some chips. */
#define SST_25WF512		0x2501
#define SST_25WF010		0x2502
#define SST_25WF020		0x2503
#define SST_25WF040		0x2504
#define SST_25VF016B		0x2541
#define SST_25VF032B		0x254A
#define SST_25VF040B		0x258D
#define SST_25VF080B		0x258E
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

#define ST_ID			0x20	/* ST */
#define ST_M50FLW040A		0x08
#define ST_M50FLW040B		0x28
#define ST_M50FLW080A		0x80
#define ST_M50FLW080B		0x81
#define ST_M50FW040		0x2C
#define ST_M50FW080		0x2D
#define ST_M50FW016		0x2E
#define ST_M50LPW116		0x30
#define ST_M29F002B		0x34
#define ST_M29F002T		0xB0	/* M29F002T / M29F002NT */
#define ST_M29F400BT		0xD5
#define ST_M29F040B		0xE2
#define ST_M29W010B		0x23
#define ST_M29W040B		0xE3

#define SYNCMOS_ID		0x40	/* SyncMOS and Mosel Vitelic */
#define S29C51001T		0x01
#define S29C51002T		0x02
#define S29C51004T		0x03
#define S29C31004T		0x63

#define TI_ID			0x97	/* Texas Instruments */

#define WINBOND_ID		0xDA	/* Winbond */
#define W_29C011		0xC1
#define W_29C020C		0x45
#define W_29C040P		0x46
#define W_29EE011		0xC1
#define W_39V040FA		0x34
#define W_39V040A		0x3D
#define W_39V040B		0x54
#define W_39V080A		0xD0
#define W_49F002U		0x0B
#define W_49V002A		0xB0
#define W_49V002FA		0x32

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

/* debug.c */
extern int verbose;
#define printf_debug(x...) { if (verbose) printf(x); }

/* flashrom.c */
int map_flash_registers(struct flashchip *flash);

/* layout.c */
int show_id(uint8_t *bios, int size);
int read_romlayout(char *name);
int find_romentry(char *name);
int handle_romentries(uint8_t *buffer, uint8_t *content);

/* lbtable.c */
int linuxbios_init(void);
extern char *lb_part, *lb_vendor;

/* spi.c */
int probe_spi(struct flashchip *flash);
int it87xx_probe_spi_flash(const char *name);
int generic_spi_command(unsigned int writecnt, unsigned int readcnt, const unsigned char *writearr, unsigned char *readarr);
void generic_spi_write_enable();
void generic_spi_write_disable();
int generic_spi_chip_erase(struct flashchip *flash);
int generic_spi_chip_write(struct flashchip *flash, uint8_t *buf);

/* 82802ab.c */
int probe_82802ab(struct flashchip *flash);
int erase_82802ab(struct flashchip *flash);
int write_82802ab(struct flashchip *flash, uint8_t *buf);

/* am29f040b.c */
int probe_29f040b(struct flashchip *flash);
int erase_29f040b(struct flashchip *flash);
int write_29f040b(struct flashchip *flash, uint8_t *buf);

/* jedec.c */
void toggle_ready_jedec(volatile uint8_t *dst);
void data_polling_jedec(volatile uint8_t *dst, uint8_t data);
void unprotect_jedec(volatile uint8_t *bios);
void protect_jedec(volatile uint8_t *bios);
int write_byte_program_jedec(volatile uint8_t *bios, uint8_t *src,
			     volatile uint8_t *dst);
int probe_jedec(struct flashchip *flash);
int erase_chip_jedec(struct flashchip *flash);
int write_jedec(struct flashchip *flash, uint8_t *buf);
int erase_sector_jedec(volatile uint8_t *bios, unsigned int page);
int erase_block_jedec(volatile uint8_t *bios, unsigned int page);
int write_sector_jedec(volatile uint8_t *bios, uint8_t *src,
		       volatile uint8_t *dst, unsigned int page_size);

/* m29f400bt.c */
int probe_m29f400bt(struct flashchip *flash);
int erase_m29f400bt(struct flashchip *flash);
int block_erase_m29f400bt(volatile uint8_t *bios,
				 volatile uint8_t *dst);
int write_m29f400bt(struct flashchip *flash, uint8_t *buf);
int write_linuxbios_m29f400bt(struct flashchip *flash, uint8_t *buf);
void toggle_ready_m29f400bt(volatile uint8_t *dst);
void data_polling_m29f400bt(volatile uint8_t *dst, uint8_t data);
void protect_m29f400bt(volatile uint8_t *bios);
void write_page_m29f400bt(volatile uint8_t *bios, uint8_t *src,
			  volatile uint8_t *dst, int page_size);

/* mx29f002.c */
int probe_29f002(struct flashchip *flash);
int erase_29f002(struct flashchip *flash);
int write_29f002(struct flashchip *flash, uint8_t *buf);

/* pm49fl004.c */
int probe_49fl004(struct flashchip *flash);
int erase_49fl004(struct flashchip *flash);
int write_49fl004(struct flashchip *flash, uint8_t *buf);

/* sharplhf00l04.c */
int probe_lhf00l04(struct flashchip *flash);
int erase_lhf00l04(struct flashchip *flash);
int write_lhf00l04(struct flashchip *flash, uint8_t *buf);
void toggle_ready_lhf00l04(volatile uint8_t *dst);
void data_polling_lhf00l04(volatile uint8_t *dst, uint8_t data);
void protect_lhf00l04(volatile uint8_t *bios);

/* sst28sf040.c */
int probe_28sf040(struct flashchip *flash);
int erase_28sf040(struct flashchip *flash);
int write_28sf040(struct flashchip *flash, uint8_t *buf);

/* sst39sf020.c */
int probe_39sf020(struct flashchip *flash);
int write_39sf020(struct flashchip *flash, uint8_t *buf);

/* sst49lf040.c */
int erase_49lf040(struct flashchip *flash);
int write_49lf040(struct flashchip *flash, uint8_t *buf);

/* sst49lfxxxc.c */
int probe_49lfxxxc(struct flashchip *flash);
int erase_49lfxxxc(struct flashchip *flash);
int write_49lfxxxc(struct flashchip *flash, uint8_t *buf);

/* sst_fwhub.c */
int probe_sst_fwhub(struct flashchip *flash);
int erase_sst_fwhub(struct flashchip *flash);
int write_sst_fwhub(struct flashchip *flash, uint8_t *buf);

/* w29ee011.c */
int probe_w29ee011(struct flashchip *flash);

/* w49f002u.c */
int write_49f002(struct flashchip *flash, uint8_t *buf);

#endif				/* !__FLASH_H__ */
