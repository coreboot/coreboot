/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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

#define ITE_SUPERIO_PORT1	0x2e
#define ITE_SUPERIO_PORT2	0x4e

#define JEDEC_RDID	{0x9f}
#define JEDEC_RDID_OUTSIZE	0x01
#define JEDEC_RDID_INSIZE	0x03

static uint16_t it8716f_flashport = 0;

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
		printf("serial flash pin %i\n",	(tmp & 1 << 5) ? 87 : 29);
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
static int it8716f_spi_command(uint16_t port, unsigned char writecnt, unsigned char readcnt, const unsigned char *writearr, unsigned char *readarr)
{
	uint8_t busy, writeenc;
	do {
		busy = inb(port) & 0x80;
	} while (busy);
	if (readcnt > 3) {
		printf("%s called with unsupported readcnt %i.\n",
			__FUNCTION__, readcnt);
		return 1;
	}
	switch (writecnt) {
	case 1:
		outb(writearr[0], port + 1);
		writeenc = 0x0;
		break;
	case 2:
		outb(writearr[0], port + 1);
		outb(writearr[1], port + 7);
		writeenc = 0x1;
		break;
	case 4:
		outb(writearr[0], port + 1);
		outb(writearr[1], port + 4);
		outb(writearr[2], port + 3);
		outb(writearr[3], port + 2);
		writeenc = 0x2;
		break;
	case 5:
		outb(writearr[0], port + 1);
		outb(writearr[1], port + 4);
		outb(writearr[2], port + 3);
		outb(writearr[3], port + 2);
		outb(writearr[4], port + 7);
		writeenc = 0x3;
		break;
	default:
		printf("%s called with unsupported writecnt %i.\n",
			__FUNCTION__, writecnt);
		return 1;
	}
	/* Start IO, 33MHz, readcnt input bytes, writecnt output bytes. Note:
	 * We can't use writecnt directly, but have to use a strange encoding 
	 */ 
	outb((0x5 << 4) | ((readcnt & 0x3) << 2) | (writeenc), port);
	do {
		busy = inb(port) & 0x80;
	} while (busy);
	readarr[0] = inb(port + 5);
	readarr[1] = inb(port + 6);
	readarr[2] = inb(port + 7);
	return 0;
}

static int generic_spi_command(unsigned char writecnt, unsigned char readcnt, const unsigned char *writearr, unsigned char *readarr)
{
	if (it8716f_flashport)
		return it8716f_spi_command(it8716f_flashport, writecnt, readcnt, writearr, readarr);
	printf("%s called, but no SPI chipset detected\n", __FUNCTION__);
	return 1;
}

static int generic_spi_rdid(unsigned char *readarr)
{
	const unsigned char cmd[] = JEDEC_RDID;

	if (generic_spi_command(JEDEC_RDID_OUTSIZE, JEDEC_RDID_INSIZE, cmd, readarr))
		return 1;
	printf("RDID returned %02x %02x %02x.\n", readarr[0], readarr[1], readarr[2]);
	return 0;
}

int probe_spi(struct flashchip *flash)
{
	unsigned char readarr[3];
	uint8_t manuf_id;
	uint16_t model_id;
	if (!generic_spi_rdid(readarr)) {
		manuf_id = readarr[0];
		model_id = (readarr[1] << 8) | readarr[2];
		printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, manuf_id, model_id);
		if (manuf_id == flash->manufacture_id && model_id == flash->model_id)
			return 1;
	}

	return 0;
}

