/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2008 Peter Stuge <peter@stuge.se>
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

#include <stdio.h>
#include <pci/pci.h>
#include <stdint.h>
#include <string.h>
#include "flash.h"
#include "spi.h"

#define WBSIO_PORT1	0x2e
#define WBSIO_PORT2	0x4e

static uint16_t wbsio_spibase = 0;

static uint16_t wbsio_get_spibase(uint16_t port)
{
	uint8_t id;
	uint16_t flashport = 0;

	w836xx_ext_enter(port);
	id = wbsio_read(port, 0x20);
	if (id != 0xa0) {
		fprintf(stderr, "\nW83627 not found at 0x%x, id=0x%02x want=0xa0.\n", port, id);
		goto done;
	}

	if (0 == (wbsio_read(port, 0x24) & 2)) {
		fprintf(stderr, "\nW83627 found at 0x%x, but SPI pins are not enabled. (CR[0x24] bit 1=0)\n", port);
		goto done;
	}

	wbsio_write(port, 0x07, 0x06);
	if (0 == (wbsio_read(port, 0x30) & 1)) {
		fprintf(stderr, "\nW83627 found at 0x%x, but SPI is not enabled. (LDN6[0x30] bit 0=0)\n", port);
		goto done;
	}

	flashport = (wbsio_read(port, 0x62) << 8) | wbsio_read(port, 0x63);

done:
	w836xx_ext_leave(port);
	return flashport;
}

int wbsio_check_for_spi(const char *name)
{
	if (0 == (wbsio_spibase = wbsio_get_spibase(WBSIO_PORT1)))
		if (0 == (wbsio_spibase = wbsio_get_spibase(WBSIO_PORT2)))
			return 1;

	printf_debug("\nwbsio_spibase = 0x%x\n", wbsio_spibase);
	flashbus = BUS_TYPE_WBSIO_SPI;
	return 0;
}

/* W83627DHG has 11 command modes:
 * 1=1 command only
 * 2=1 command+1 data write
 * 3=1 command+2 data read
 * 4=1 command+3 address
 * 5=1 command+3 address+1 data write
 * 6=1 command+3 address+4 data write
 * 7=1 command+3 address+1 dummy address inserted by wbsio+4 data read
 * 8=1 command+3 address+1 data read
 * 9=1 command+3 address+2 data read
 * a=1 command+3 address+3 data read
 * b=1 command+3 address+4 data read
 *
 * mode[7:4] holds the command mode
 * mode[3:0] holds SPI address bits [19:16]
 *
 * The Winbond SPI master only supports 20 bit addresses on the SPI bus. :\
 * Would one more byte of RAM in the chip (to get all 24 bits) really make
 * such a big difference?
 */
int wbsio_spi_command(unsigned int writecnt, unsigned int readcnt,
		      const unsigned char *writearr, unsigned char *readarr)
{
	int i;
	uint8_t mode = 0;

	printf_debug("%s:", __func__);

	if (1 == writecnt && 0 == readcnt) {
		mode = 0x10;
	} else if (2 == writecnt && 0 == readcnt) {
		OUTB(writearr[1], wbsio_spibase + 4);
		printf_debug(" data=0x%02x", writearr[1]);
		mode = 0x20;
	} else if (1 == writecnt && 2 == readcnt) {
		mode = 0x30;
	} else if (4 == writecnt && 0 == readcnt) {
		printf_debug(" addr=0x%02x", (writearr[1] & 0x0f));
		for (i = 2; i < writecnt; i++) {
			OUTB(writearr[i], wbsio_spibase + i);
			printf_debug("%02x", writearr[i]);
		}
		mode = 0x40 | (writearr[1] & 0x0f);
	} else if (5 == writecnt && 0 == readcnt) {
		printf_debug(" addr=0x%02x", (writearr[1] & 0x0f));
		for (i = 2; i < 4; i++) {
			OUTB(writearr[i], wbsio_spibase + i);
			printf_debug("%02x", writearr[i]);
		}
		OUTB(writearr[i], wbsio_spibase + i);
		printf_debug(" data=0x%02x", writearr[i]);
		mode = 0x50 | (writearr[1] & 0x0f);
	} else if (8 == writecnt && 0 == readcnt) {
		printf_debug(" addr=0x%02x", (writearr[1] & 0x0f));
		for (i = 2; i < 4; i++) {
			OUTB(writearr[i], wbsio_spibase + i);
			printf_debug("%02x", writearr[i]);
		}
		printf_debug(" data=0x");
		for (; i < writecnt; i++) {
			OUTB(writearr[i], wbsio_spibase + i);
			printf_debug("%02x", writearr[i]);
		}
		mode = 0x60 | (writearr[1] & 0x0f);
	} else if (5 == writecnt && 4 == readcnt) {
		/* XXX: TODO not supported by flashrom infrastructure!
		 * This mode, 7, discards the fifth byte in writecnt,
		 * but since we can not express that in flashrom, fail
		 * the operation for now.
		 */
		;
	} else if (4 == writecnt && readcnt >= 1 && readcnt <= 4) {
		printf_debug(" addr=0x%02x", (writearr[1] & 0x0f));
		for (i = 2; i < writecnt; i++) {
			OUTB(writearr[i], wbsio_spibase + i);
			printf_debug("%02x", writearr[i]);
		}
		mode = ((7 + readcnt) << 4) | (writearr[1] & 0x0f);
	}
	printf_debug(" cmd=%02x mode=%02x\n", writearr[0], mode);

	if (!mode) {
		fprintf(stderr, "%s: unsupported command type wr=%d rd=%d\n",
			__func__, writecnt, readcnt);
		return 1;
	}

	OUTB(writearr[0], wbsio_spibase);
	OUTB(mode, wbsio_spibase + 1);
	myusec_delay(10);

	if (!readcnt)
		return 0;

	printf_debug("%s: returning data =", __func__);
	for (i = 0; i < readcnt; i++) {
		readarr[i] = INB(wbsio_spibase + 4 + i);
		printf_debug(" 0x%02x", readarr[i]);
	}
	printf_debug("\n");
	return 0;
}

int wbsio_spi_read(struct flashchip *flash, uint8_t *buf)
{
	int size = flash->total_size * 1024;

	if (flash->total_size > 1024) {
		fprintf(stderr, "%s: Winbond saved on 4 register bits so max chip size is 1024 KB!\n", __func__);
		return 1;
	}

	memcpy(buf, (const char *)flash->virtual_memory, size);
	return 0;
}

int wbsio_spi_write(struct flashchip *flash, uint8_t *buf)
{
	int pos, size = flash->total_size * 1024;

	if (flash->total_size > 1024) {
		fprintf(stderr, "%s: Winbond saved on 4 register bits so max chip size is 1024 KB!\n", __func__);
		return 1;
	}

	flash->erase(flash);
	spi_write_enable();
	for (pos = 0; pos < size; pos++) {
		spi_byte_program(pos, buf[pos]);
		while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
			myusec_delay(10);
	}
	spi_write_disable();
	return 0;
}
