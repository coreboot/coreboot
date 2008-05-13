/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2007, 2008 Carl-Daniel Hailfinger
 * Copyright (C) 2008 Ronald Hoogenboom <ronald@zonnet.nl>
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

/*
 * Contains the generic SPI framework
 */

#include <stdio.h>
#include <pci/pci.h>
#include <stdint.h>
#include <string.h>
#include "flash.h"
#include "spi.h"

#define ITE_SUPERIO_PORT1	0x2e
#define ITE_SUPERIO_PORT2	0x4e


uint16_t it8716f_flashport = 0;
/* use fast 33MHz SPI (<>0) or slow 16MHz (0) */
int fast_spi = 1;

void spi_prettyprint_status_register(struct flashchip *flash);
void spi_disable_blockprotect(void);

/* Generic Super I/O helper functions */
uint8_t regval(uint16_t port, uint8_t reg)
{
	outb(reg, port);
	return inb(port + 1);
}

void regwrite(uint16_t port, uint8_t reg, uint8_t val)
{
	outb(reg, port);
	outb(val, port + 1);
}

/* Helper functions for most recent ITE IT87xx Super I/O chips */
#define CHIP_ID_BYTE1_REG	0x20
#define CHIP_ID_BYTE2_REG	0x21
static void enter_conf_mode_ite(uint16_t port)
{
	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	if (port == ITE_SUPERIO_PORT1)
		outb(0x55, port);
	else
		outb(0xaa, port);
}

static void exit_conf_mode_ite(uint16_t port)
{
	regwrite(port, 0x02, 0x02);
}

static uint16_t find_ite_spi_flash_port(uint16_t port)
{
	uint8_t tmp = 0;
	uint16_t id, flashport = 0;

	enter_conf_mode_ite(port);

	id = regval(port, CHIP_ID_BYTE1_REG) << 8;
	id |= regval(port, CHIP_ID_BYTE2_REG);

	/* TODO: Handle more IT87xx if they support flash translation */
	if (id == 0x8716) {
		/* NOLDN, reg 0x24, mask out lowest bit (suspend) */
		tmp = regval(port, 0x24) & 0xFE;
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0xFFFE0000, 0xFFFFFFFF, (tmp & 1 << 1) ? "en" : "dis");
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0x000E0000, 0x000FFFFF, (tmp & 1 << 1) ? "en" : "dis");
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0xFFEE0000, 0xFFEFFFFF, (tmp & 1 << 2) ? "en" : "dis");
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0xFFF80000, 0xFFFEFFFF, (tmp & 1 << 3) ? "en" : "dis");
		printf("LPC write to serial flash %sabled\n",
			(tmp & 1 << 4) ? "en" : "dis");
		printf("serial flash pin %i\n", (tmp & 1 << 5) ? 87 : 29);
		/* LDN 0x7, reg 0x64/0x65 */
		regwrite(port, 0x07, 0x7);
		flashport = regval(port, 0x64) << 8;
		flashport |= regval(port, 0x65);
	}
	exit_conf_mode_ite(port);
	return flashport;
}

int it87xx_probe_spi_flash(const char *name)
{
	it8716f_flashport = find_ite_spi_flash_port(ITE_SUPERIO_PORT1);
	if (!it8716f_flashport)
		it8716f_flashport = find_ite_spi_flash_port(ITE_SUPERIO_PORT2);
	return (!it8716f_flashport);
}

/* The IT8716F only supports commands with length 1,2,4,5 bytes including
   command byte and can not read more than 3 bytes from the device.
   This function expects writearr[0] to be the first byte sent to the device,
   whereas the IT8716F splits commands internally into address and non-address
   commands with the address in inverse wire order. That's why the register
   ordering in case 4 and 5 may seem strange. */
static int it8716f_spi_command(unsigned int writecnt, unsigned int readcnt, const unsigned char *writearr, unsigned char *readarr)
{
	uint8_t busy, writeenc;
	int i;

	do {
		busy = inb(it8716f_flashport) & 0x80;
	} while (busy);
	if (readcnt > 3) {
		printf("%s called with unsupported readcnt %i.\n",
			__FUNCTION__, readcnt);
		return 1;
	}
	switch (writecnt) {
	case 1:
		outb(writearr[0], it8716f_flashport + 1);
		writeenc = 0x0;
		break;
	case 2:
		outb(writearr[0], it8716f_flashport + 1);
		outb(writearr[1], it8716f_flashport + 7);
		writeenc = 0x1;
		break;
	case 4:
		outb(writearr[0], it8716f_flashport + 1);
		outb(writearr[1], it8716f_flashport + 4);
		outb(writearr[2], it8716f_flashport + 3);
		outb(writearr[3], it8716f_flashport + 2);
		writeenc = 0x2;
		break;
	case 5:
		outb(writearr[0], it8716f_flashport + 1);
		outb(writearr[1], it8716f_flashport + 4);
		outb(writearr[2], it8716f_flashport + 3);
		outb(writearr[3], it8716f_flashport + 2);
		outb(writearr[4], it8716f_flashport + 7);
		writeenc = 0x3;
		break;
	default:
		printf("%s called with unsupported writecnt %i.\n",
			__FUNCTION__, writecnt);
		return 1;
	}
	/* Start IO, 33 or 16 MHz, readcnt input bytes, writecnt output bytes.
	 * Note:
	 * We can't use writecnt directly, but have to use a strange encoding.
	 */ 
	outb(((0x4 + (fast_spi ? 1 : 0)) << 4) | ((readcnt & 0x3) << 2) | (writeenc), it8716f_flashport);

	if (readcnt > 0) {
		do {
			busy = inb(it8716f_flashport) & 0x80;
		} while (busy);

		for (i = 0; i < readcnt; i++) {
			readarr[i] = inb(it8716f_flashport + 5 + i);
		}
	}

	return 0;
}

int spi_command(unsigned int writecnt, unsigned int readcnt, const unsigned char *writearr, unsigned char *readarr)
{
	if (it8716f_flashport)
		return it8716f_spi_command(writecnt, readcnt, writearr, readarr);
	printf_debug("%s called, but no SPI chipset detected\n", __FUNCTION__);
	return 1;
}

static int spi_rdid(unsigned char *readarr)
{
	const unsigned char cmd[JEDEC_RDID_OUTSIZE] = {JEDEC_RDID};

	if (spi_command(JEDEC_RDID_OUTSIZE, JEDEC_RDID_INSIZE, cmd, readarr))
		return 1;
	printf_debug("RDID returned %02x %02x %02x.\n", readarr[0], readarr[1], readarr[2]);
	return 0;
}

void spi_write_enable()
{
	const unsigned char cmd[JEDEC_WREN_OUTSIZE] = {JEDEC_WREN};

	/* Send WREN (Write Enable) */
	spi_command(JEDEC_WREN_OUTSIZE, JEDEC_WREN_INSIZE, cmd, NULL);
}

void spi_write_disable()
{
	const unsigned char cmd[JEDEC_WRDI_OUTSIZE] = {JEDEC_WRDI};

	/* Send WRDI (Write Disable) */
	spi_command(JEDEC_WRDI_OUTSIZE, JEDEC_WRDI_INSIZE, cmd, NULL);
}

int probe_spi(struct flashchip *flash)
{
	unsigned char readarr[3];
	uint32_t manuf_id;
	uint32_t model_id;
	if (!spi_rdid(readarr)) {
		/* Check if this is a continuation vendor ID */
		if (readarr[0] == 0x7f) {
			manuf_id = (readarr[0] << 8) | readarr[1];
			model_id = readarr[2];
		} else {
			manuf_id = readarr[0];
			model_id = (readarr[1] << 8) | readarr[2];
		}
		printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, manuf_id, model_id);
		if (manuf_id == flash->manufacture_id &&
		    model_id == flash->model_id) {
			/* Print the status register to tell the
			 * user about possible write protection.
			 */
			spi_prettyprint_status_register(flash);

			return 1;
		}
		/* Test if this is a pure vendor match. */
		if (manuf_id == flash->manufacture_id &&
		    GENERIC_DEVICE_ID == flash->model_id)
			return 1;
	}

	return 0;
}

uint8_t spi_read_status_register()
{
	const unsigned char cmd[JEDEC_RDSR_OUTSIZE] = {JEDEC_RDSR};
	unsigned char readarr[1];

	/* Read Status Register */
	spi_command(JEDEC_RDSR_OUTSIZE, JEDEC_RDSR_INSIZE, cmd, readarr);
	return readarr[0];
}

/* Prettyprint the status register. Common definitions.
 */
void spi_prettyprint_status_register_common(uint8_t status)
{
	printf_debug("Chip status register: Bit 5 / Block Protect 3 (BP3) is "
		"%sset\n", (status & (1 << 5)) ? "" : "not ");
	printf_debug("Chip status register: Bit 4 / Block Protect 2 (BP2) is "
		"%sset\n", (status & (1 << 4)) ? "" : "not ");
	printf_debug("Chip status register: Bit 3 / Block Protect 1 (BP1) is "
		"%sset\n", (status & (1 << 3)) ? "" : "not ");
	printf_debug("Chip status register: Bit 2 / Block Protect 0 (BP0) is "
		"%sset\n", (status & (1 << 2)) ? "" : "not ");
	printf_debug("Chip status register: Write Enable Latch (WEL) is "
		"%sset\n", (status & (1 << 1)) ? "" : "not ");
	printf_debug("Chip status register: Write In Progress (WIP/BUSY) is "
		"%sset\n", (status & (1 << 0)) ? "" : "not ");
}

/* Prettyprint the status register. Works for
 * ST M25P series
 * MX MX25L series
 */
void spi_prettyprint_status_register_st_m25p(uint8_t status)
{
	printf_debug("Chip status register: Status Register Write Disable "
		"(SRWD) is %sset\n", (status & (1 << 7)) ? "" : "not ");
	printf_debug("Chip status register: Bit 6 is "
		"%sset\n", (status & (1 << 6)) ? "" : "not ");
	spi_prettyprint_status_register_common(status);
}

/* Prettyprint the status register. Works for
 * SST 25VF016
 */
void spi_prettyprint_status_register_sst25vf016(uint8_t status)
{
	const char *bpt[] = {
		"none",
		"1F0000H-1FFFFFH",
		"1E0000H-1FFFFFH",
		"1C0000H-1FFFFFH",
		"180000H-1FFFFFH",
		"100000H-1FFFFFH",
		"all", "all"
	};
	printf_debug("Chip status register: Block Protect Write Disable "
		"(BPL) is %sset\n", (status & (1 << 7)) ? "" : "not ");
	printf_debug("Chip status register: Auto Address Increment Programming "
		"(AAI) is %sset\n", (status & (1 << 6)) ? "" : "not ");
	spi_prettyprint_status_register_common(status);
	printf_debug("Resulting block protection : %s\n",
		bpt[(status & 0x1c) >> 2]);
}

void spi_prettyprint_status_register(struct flashchip *flash)
{
	uint8_t status;

	status = spi_read_status_register();
	printf_debug("Chip status register is %02x\n", status);
	switch (flash->manufacture_id) {
	case ST_ID:
	case MX_ID:
		if ((flash->model_id & 0xff00) == 0x2000)
			spi_prettyprint_status_register_st_m25p(status);
		break;
	case SST_ID:
		if (flash->model_id == SST_25VF016B)
			spi_prettyprint_status_register_sst25vf016(status);
		break;
	}
}
	
int spi_chip_erase_c7(struct flashchip *flash)
{
	const unsigned char cmd[JEDEC_CE_C7_OUTSIZE] = {JEDEC_CE_C7};
	
	spi_disable_blockprotect();
	spi_write_enable();
	/* Send CE (Chip Erase) */
	spi_command(JEDEC_CE_C7_OUTSIZE, JEDEC_CE_C7_INSIZE, cmd, NULL);
	/* Wait until the Write-In-Progress bit is cleared.
	 * This usually takes 1-85 s, so wait in 1 s steps.
	 */
	while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
		sleep(1);
	return 0;
}

/* Block size is usually
 * 64k for Macronix
 * 32k for SST
 * 4-32k non-uniform for EON
 */
int spi_block_erase_d8(const struct flashchip *flash, unsigned long addr)
{
	unsigned char cmd[JEDEC_BE_D8_OUTSIZE] = {JEDEC_BE_D8};

	cmd[1] = (addr & 0x00ff0000) >> 16;
	cmd[2] = (addr & 0x0000ff00) >> 8;
	cmd[3] = (addr & 0x000000ff);
	spi_write_enable();
	/* Send BE (Block Erase) */
	spi_command(JEDEC_BE_D8_OUTSIZE, JEDEC_BE_D8_INSIZE, cmd, NULL);
	/* Wait until the Write-In-Progress bit is cleared.
	 * This usually takes 100-4000 ms, so wait in 100 ms steps.
	 */
	while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
		usleep(100 * 1000);
	return 0;
}

/* Sector size is usually 4k, though Macronix eliteflash has 64k */
int spi_sector_erase(const struct flashchip *flash, unsigned long addr)
{
	unsigned char cmd[JEDEC_SE_OUTSIZE] = {JEDEC_SE};
	cmd[1] = (addr & 0x00ff0000) >> 16;
	cmd[2] = (addr & 0x0000ff00) >> 8;
	cmd[3] = (addr & 0x000000ff);

	spi_write_enable();
	/* Send SE (Sector Erase) */
	spi_command(JEDEC_SE_OUTSIZE, JEDEC_SE_INSIZE, cmd, NULL);
	/* Wait until the Write-In-Progress bit is cleared.
	 * This usually takes 15-800 ms, so wait in 10 ms steps.
	 */
	while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
		usleep(10 * 1000);
	return 0;
}

/* Page size is usually 256 bytes */
void it8716f_spi_page_program(int block, uint8_t *buf, uint8_t *bios) {
	int i;

	spi_write_enable();
	outb(0x06 , it8716f_flashport + 1);
	outb(((2 + (fast_spi ? 1 : 0)) << 4), it8716f_flashport);
	for (i = 0; i < 256; i++) {
		bios[256 * block + i] = buf[256 * block + i];
	}
	outb(0, it8716f_flashport);
	/* Wait until the Write-In-Progress bit is cleared.
	 * This usually takes 1-10 ms, so wait in 1 ms steps.
	 */
	while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
		usleep(1000);
}

void spi_page_program(int block, uint8_t *buf, uint8_t *bios)
{
	if (it8716f_flashport)
		it8716f_spi_page_program(block, buf, bios);
}

/*
 * This is according the SST25VF016 datasheet, who knows it is more
 * generic that this...
 */
void spi_write_status_register(int status)
{
	const unsigned char cmd[JEDEC_WRSR_OUTSIZE] = {JEDEC_WRSR, (unsigned char)status};

	/* Send WRSR (Write Status Register) */
	spi_command(JEDEC_WRSR_OUTSIZE, JEDEC_WRSR_INSIZE, cmd, NULL);
}

void spi_byte_program(int address, uint8_t byte)
{
	const unsigned char cmd[JEDEC_BYTE_PROGRAM_OUTSIZE] = {JEDEC_BYTE_PROGRAM,
		(address>>16)&0xff,
		(address>>8)&0xff,
		(address>>0)&0xff,
		byte
	};

	/* Send Byte-Program */
	spi_command(JEDEC_BYTE_PROGRAM_OUTSIZE, JEDEC_BYTE_PROGRAM_INSIZE, cmd, NULL);
}

void spi_disable_blockprotect(void)
{
	uint8_t status;

	status = spi_read_status_register();
	/* If there is block protection in effect, unprotect it first. */
	if ((status & 0x3c) != 0) {
		printf_debug("Some block protection in effect, disabling\n");
		spi_write_enable();
		spi_write_status_register(status & ~0x3c);
	}
}

/*
 * IT8716F only allows maximum of 512 kb SPI mapped to LPC memory cycles
 * Program chip using firmware cycle byte programming. (SLOW!)
 */
int it8716f_over512k_spi_chip_write(struct flashchip *flash, uint8_t *buf)
{
	int total_size = 1024 * flash->total_size;
	int i;
	fast_spi = 0;

	spi_disable_blockprotect();
	for (i = 0; i < total_size; i++) {
		spi_write_enable();
		spi_byte_program(i, buf[i]);
		while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
			myusec_delay(10);
	}
	/* resume normal ops... */
	outb(0x20, it8716f_flashport);
	return 0;
}

void spi_3byte_read(int address, uint8_t *bytes, int len)
{
	const unsigned char cmd[JEDEC_READ_OUTSIZE] = {JEDEC_READ,
		(address >> 16) & 0xff,
		(address >> 8) & 0xff,
		(address >> 0) & 0xff,
	};

	/* Send Read */
	spi_command(JEDEC_READ_OUTSIZE, len, cmd, bytes);
}

/*
 * IT8716F only allows maximum of 512 kb SPI mapped to LPC memory cycles
 * Need to read this big flash using firmware cycles 3 byte at a time.
 */
int spi_chip_read(struct flashchip *flash, uint8_t *buf)
{
	int total_size = 1024 * flash->total_size;
	int i;
	fast_spi = 0;

	if (total_size > 512 * 1024) {
		for (i = 0; i < total_size; i += 3) {
			int toread = 3;
			if (total_size - i < toread)
				toread = total_size - i;
			spi_3byte_read(i, buf + i, toread);
		}
	} else {
		memcpy(buf, (const char *)flash->virtual_memory, total_size);
	}
	return 0;
}

int spi_chip_write(struct flashchip *flash, uint8_t *buf) {
	int total_size = 1024 * flash->total_size;
	int i;
	if (total_size > 512 * 1024) {
		it8716f_over512k_spi_chip_write(flash, buf);
	} else {
		for (i = 0; i < total_size / 256; i++) {
			spi_page_program(i, buf, (uint8_t *)flash->virtual_memory);
		}
	}
	return 0;
}

