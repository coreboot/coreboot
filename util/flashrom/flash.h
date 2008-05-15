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

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

struct flashchip {
	const char *vendor;
	const char *name;
	/* With 32bit manufacture_id and model_id we can cover IDs up to
	 * (including) the 4th bank of JEDEC JEP106W Standard Manufacturer's
	 * Identification code.
	 */
	uint32_t manufacture_id;
	uint32_t model_id;

	int total_size;
	int page_size;

	/* Indicate if flashrom has been tested with this flash chip and if
	 * everything worked correctly.
	 */
	uint32_t tested;

	int (*probe) (struct flashchip *flash);
	int (*erase) (struct flashchip *flash);
	int (*write) (struct flashchip *flash, uint8_t *buf);
	int (*read) (struct flashchip *flash, uint8_t *buf);

	/* Some flash devices have an additional register space. */
	volatile uint8_t *virtual_memory;
	volatile uint8_t *virtual_registers;
};

#define TEST_UNTESTED	0

#define TEST_OK_PROBE	(1<<0)
#define TEST_OK_READ	(1<<1)
#define TEST_OK_ERASE	(1<<2)
#define TEST_OK_WRITE	(1<<3)
#define TEST_OK_PREW	(TEST_OK_PROBE|TEST_OK_READ|TEST_OK_ERASE|TEST_OK_WRITE)
#define TEST_OK_MASK	0x0f

#define TEST_BAD_PROBE	(1<<4)
#define TEST_BAD_READ	(1<<5)
#define TEST_BAD_ERASE	(1<<6)
#define TEST_BAD_WRITE	(1<<7)
#define TEST_BAD_MASK	0xf0

extern struct flashchip flashchips[];

/*
 * Please keep this list sorted alphabetically by manufacturer. The first
 * entry of each section should be the manufacturer ID, followed by the
 * list of devices from that manufacturer (sorted by device IDs).
 *
 * All LPC/FWH parts (parallel flash) have 8-bit device IDs if there is no
 * continuation code.
 * All SPI parts have 16-bit device IDs.
 */

#define GENERIC_DEVICE_ID	0xffff	/* Only match the vendor ID */

#define ALLIANCE_ID		0x52	/* Alliance Semiconductor */

#define AMD_ID			0x01	/* AMD */
#define AM_29F040B		0xA4
#define AM_29LV040B		0x4F
#define AM_29F016D		0xAD

#define AMIC_ID			0x7F37	/* AMIC */
#define AMIC_ID_NOPREFIX	0x37	/* AMIC */

#define ASD_ID			0x25	/* ASD, not listed in JEP106W */
#define ASD_AE49F2008		0x52

#define ATMEL_ID		0x1F	/* Atmel */
#define AT_25DF021		0x4300
#define AT_25DF041A		0x4401
#define AT_25DF081		0x4502
#define AT_25DF161		0x4602
#define AT_25DF321		0x4700	/* also 26DF321 */
#define AT_25DF321A		0x4701
#define AT_25DF641		0x4800
#define AT_26DF041		0x4400
#define AT_26DF081		0x4500	/* guessed, no datasheet available */
#define AT_26DF081A		0x4501
#define AT_26DF161		0x4600
#define AT_26DF161A		0x4601
#define AT_29C040A		0xA4
#define AT_29C020		0xDA
#define AT_49F002N		0x07	/* for AT49F002(N)  */
#define AT_49F002NT		0x08	/* for AT49F002(N)T */

#define CATALYST_ID		0x31	/* Catalyst */

#define EMST_ID			0x8C	/* EMST / EFST Elite Flash Storage*/
#define EMST_F49B002UA		0x00

/*
 * EN25 chips are SPI, first byte of device ID is memory type,
 * second byte of device ID is log(bitsize)-9.
 * Vendor and device ID of EN29 series are both prefixed with 0x7F, which
 * is the continuation code for IDs in bank 2.
 * Vendor ID of EN25 series is NOT prefixed with 0x7F, this results in
 * a collision with Mitsubishi. Mitsubishi once manufactured flash chips.
 * Let's hope they are not manufacturing SPI flash chips as well.
 */
#define EON_ID			0x7F1C	/* EON Silicon Devices */
#define EON_ID_NOPREFIX		0x1C	/* EON, missing 0x7F prefix */
#define EN_25B05		0x2010	/* 2^19 kbit or 2^16 kByte */
#define EN_25B10		0x2011
#define EN_25B20		0x2012
#define EN_25B40		0x2013
#define EN_25B80		0x2014
#define EN_25B16		0x2015
#define EN_25B32		0x2016
#define EN_29F512		0x7F21
#define EN_29F010		0x7F20
#define EN_29F040A		0x7F04
#define EN_29LV010		0x7F6E
#define EN_29LV040A		0x7F4F	/* EN_29LV040(A) */
#define EN_29F002T		0x7F92
#define EN_29F002B		0x7F97

#define FUJITSU_ID		0x04	/* Fujitsu */
/* MBM29F400TC_STRANGE has a value not mentioned in the data sheet and we
 * try to read it from a location not mentioned in the data sheet.
 */
#define MBM29F400TC_STRANGE	0x23
#define MBM29F400BC		0x7B
#define MBM29F400TC		0x77

#define HYUNDAI_ID		0xAD	/* Hyundai */

#define IMT_ID			0x7F1F	/* Integrated Memory Technologies */
#define IM_29F004B		0xAE
#define IM_29F004T		0xAF

#define INTEL_ID		0x89	/* Intel */

#define ISSI_ID			0xD5	/* ISSI Integrated Silicon Solutions */

#define MSYSTEMS_ID		0x156F	/* M-Systems, not listed in JEP106W */
#define MSYSTEMS_MD2200		0xDB
#define MSYSTEMS_MD2800		0x30	/* hmm -- both 0x30 */
#define MSYSTEMS_MD2802		0x30	/* hmm -- both 0x30 */

/*
 * MX25 chips are SPI, first byte of device ID is memory type,
 * second byte of device ID is log(bitsize)-9.
 * Generalplus SPI chips seem to be compatible with Macronix
 * and use the same set of IDs.
 */
#define MX_ID			0xC2	/* Macronix (MX) */
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

/* Programmable Micro Corp is listed in JEP106W in bank 2, so it should have
 * a 0x7F continuation code prefix.
 */
#define PMC_ID			0x7F9D	/* PMC */
#define PMC_ID_NOPREFIX		0x9D	/* PMC, missing 0x7F prefix */
#define PMC_25LV512		0x7B
#define PMC_25LV010		0x7C
#define PMC_25LV020		0x7D
#define PMC_25LV040		0x7E
#define PMC_25LV080B		0x13
#define PMC_25LV016B		0x14
#define PMC_39LV512		0x1B
#define PMC_39F010		0x1C	/* also Pm39LV010 */
#define PMC_39LV020		0x3D
#define PMC_39LV040		0x3E
#define PMC_39F020		0x4D
#define PMC_39F040		0x4E
#define PMC_49FL002		0x6D
#define PMC_49FL004		0x6E

#define SHARP_ID		0xB0	/* Sharp */
#define SHARP_LHF00L04		0xCF

/*
 * Spansion was previously a joint venture of AMD and Fujitsu.
 * S25 chips are SPI. The first device ID byte is memory type and
 * the second device ID byte is memory capacity.
 */
#define SPANSION_ID		0x01	/* Spansion */
#define SPANSION_S25FL016A	0x0214

/*
 * SST25 chips are SPI, first byte of device ID is memory type, second
 * byte of device ID is related to log(bitsize) at least for some chips.
 */
#define SST_ID			0xBF	/* SST */
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
#define SST_39VF512		0xD4
#define SST_39VF010		0xD5
#define SST_39VF020		0xD6
#define SST_39VF040		0xD7
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

/*
 * ST25P chips are SPI, first byte of device ID is memory type, second
 * byte of device ID is related to log(bitsize) at least for some chips.
 */
#define ST_ID			0x20	/* ST / SGS/Thomson */
#define ST_M25P05A		0x2010
#define ST_M25P10A		0x2011
#define ST_M25P20		0x2012
#define ST_M25P40		0x2013
#define ST_M25P40_RES		0x12
#define ST_M25P80		0x2014
#define ST_M25P16		0x2015
#define ST_M25P32		0x2016
#define ST_M25P64		0x2017
#define ST_M25P128		0x2018
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

/*
 * W25X chips are SPI, first byte of device ID is memory type, second
 * byte of device ID is related to log(bitsize).
 */
#define WINBOND_ID		0xDA	/* Winbond */
#define WINBOND_NEX_ID		0xEF	/* Winbond (ex Nexcom) serial flash devices */
#define W_25X10			0x3011
#define W_25X20			0x3012
#define W_25X40			0x3013
#define W_25X80			0x3014
#define W_29C011		0xC1
#define W_29C020C		0x45
#define W_29C040P		0x46
#define W_29EE011		0xC1
#define W_39V040FA		0x34
#define W_39V040A		0x3D
#define W_39V040B		0x54
#define W_39V080A		0xD0
#define W_39V080FA		0xD3
#define W_39V080FA_DM		0x93
#define W_49F002U		0x0B
#define W_49V002A		0xB0
#define W_49V002FA		0x32

/* udelay.c */
void myusec_delay(int time);
void myusec_calibrate_delay();

/* PCI handling for board/chipset_enable */
struct pci_access *pacc;
struct pci_dev *pci_dev_find(uint16_t vendor, uint16_t device);
struct pci_dev *pci_card_find(uint16_t vendor, uint16_t device,
			      uint16_t card_vendor, uint16_t card_device);


/* board_enable.c */
int board_flash_enable(const char *vendor, const char *part);
void print_supported_boards(void);

/* chipset_enable.c */
int chipset_flash_enable(void);
void print_supported_chipsets(void);
extern int ich9_detected;
extern void *ich_spibar;

/* Physical memory mapping device */
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
int coreboot_init(void);
extern char *lb_part, *lb_vendor;

/* spi.c */
int probe_spi_rdid(struct flashchip *flash);
int probe_spi_res(struct flashchip *flash);
int it87xx_probe_spi_flash(const char *name);
int spi_command(unsigned int writecnt, unsigned int readcnt, const unsigned char *writearr, unsigned char *readarr);
void spi_write_enable();
void spi_write_disable();
int spi_chip_erase_c7(struct flashchip *flash);
int spi_chip_write(struct flashchip *flash, uint8_t *buf);
int spi_chip_read(struct flashchip *flash, uint8_t *buf);
uint8_t spi_read_status_register();
void spi_disable_blockprotect(void);
void spi_byte_program(int address, uint8_t byte);
void spi_page_program(int block, uint8_t *buf, uint8_t *bios);
void spi_nbyte_read(int address, uint8_t *bytes, int len);

/* 82802ab.c */
int probe_82802ab(struct flashchip *flash);
int erase_82802ab(struct flashchip *flash);
int write_82802ab(struct flashchip *flash, uint8_t *buf);

/* am29f040b.c */
int probe_29f040b(struct flashchip *flash);
int erase_29f040b(struct flashchip *flash);
int write_29f040b(struct flashchip *flash, uint8_t *buf);

/* it87spi.c */
extern uint16_t it8716f_flashport;
int it8716f_spi_command(unsigned int writecnt, unsigned int readcnt, const unsigned char *writearr, unsigned char *readarr);
int it8716f_spi_chip_read(struct flashchip *flash, uint8_t *buf);
int it8716f_spi_chip_write(struct flashchip *flash, uint8_t *buf);
void it8716f_spi_page_program(int block, uint8_t *buf, uint8_t *bios);

/* jedec.c */
uint8_t oddparity(uint8_t val);
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
int write_coreboot_m29f400bt(struct flashchip *flash, uint8_t *buf);
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

/* w39V080fa.c */
int probe_winbond_fwhub(struct flashchip *flash);
int erase_winbond_fwhub(struct flashchip *flash);
int write_winbond_fwhub(struct flashchip *flash, uint8_t *buf);

/* w29ee011.c */
int probe_w29ee011(struct flashchip *flash);

/* w49f002u.c */
int write_49f002(struct flashchip *flash, uint8_t *buf);

/* stm50flw0x0x.c */
int probe_stm50flw0x0x(struct flashchip *flash);
int erase_stm50flw0x0x(struct flashchip *flash);
int write_stm50flw0x0x(struct flashchip *flash, uint8_t *buf);
#endif				/* !__FLASH_H__ */
