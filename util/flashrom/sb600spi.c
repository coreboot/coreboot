/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2008 Wang Qingpei <Qingpei.Wang@amd.com>
 * Copyright (C) 2008 Joe Bao <Zheng.Bao@amd.com>
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <pci/pci.h>
#include "flash.h"
#include "spi.h"

typedef struct _spi_controller {
	unsigned int spi_cntrl0;	/* 00h */
	unsigned int restrictedcmd1;	/* 04h */
	unsigned int restrictedcmd2;	/* 08h */
	unsigned int spi_cntrl1;	/* 0ch */
	unsigned int spi_cmdvalue0;	/* 10h */
	unsigned int spi_cmdvalue1;	/* 14h */
	unsigned int spi_cmdvalue2;	/* 18h */
	unsigned int spi_fakeid;	/* 1Ch */
} sb600_spi_controller;

sb600_spi_controller *spi_bar = NULL;
uint8_t volatile *sb600_spibar;

int sb600_spi_read(struct flashchip *flash, uint8_t *buf)
{
	int rc = 0, i;
	int total_size = flash->total_size * 1024;
	int page_size = 8;

	for (i = 0; i < total_size / page_size; i++)
		spi_nbyte_read(i * page_size, (void *)(buf + i * page_size),
			       page_size);
	return rc;
}

uint8_t sb600_read_status_register(void)
{
	const unsigned char cmd[0x02] = { JEDEC_RDSR, 0x00 };
	unsigned char readarr[JEDEC_RDSR_INSIZE];

	/* Read Status Register */
	spi_command(sizeof(cmd), sizeof(readarr), cmd, readarr);
	return readarr[0];
}

int sb600_spi_write(struct flashchip *flash, uint8_t *buf)
{
	int rc = 0, i;
	int total_size = flash->total_size * 1024;

	/* Erase first */
	printf("Erasing flash before programming... ");
	flash->erase(flash);
	printf("done.\n");

	printf("Programming flash");
	for (i = 0; i < total_size; i++, buf++) {
		spi_disable_blockprotect();
		spi_write_enable();
		spi_byte_program(i, *buf);
		/* wait program complete. */
		if (i % 0x8000 == 0)
			printf(".");
		while (spi_read_status_register() & JEDEC_RDSR_BIT_WIP)
			;
	}
	printf(" done.\n");
	return rc;
}

void reset_internal_fifo_pointer(void)
{
	sb600_spibar[2] |= 0x10;

	while (sb600_spibar[0xD] & 0x7)
		printf("reset\n");
}

void execute_command(void)
{
	sb600_spibar[2] |= 1;

	while (sb600_spibar[2] & 1)
		;
}

int sb600_spi_command(unsigned int writecnt, unsigned int readcnt,
		      const unsigned char *writearr, unsigned char *readarr)
{
	int count;
	/* First byte is cmd which can not being sent through FIFO. */
	unsigned char cmd = *writearr++;

	writecnt--;

	spi_bar = (sb600_spi_controller *) sb600_spibar;

	printf_debug("%s, cmd=%x, writecnt=%x, readcnt=%x\n",
		     __func__, cmd, writecnt, readcnt);

	if (readcnt > 8) {
		printf("%s, SB600 SPI controller can not receive %d bytes, "
		       "which is limited with 8 bytes\n", __func__, readcnt);
		return 1;
	}

	if (writecnt > 8) {
		printf("%s, SB600 SPI controller can not sent %d bytes, "
		       "which is limited with 8 bytes\n", __func__, writecnt);
		return 1;
	}

	sb600_spibar[0] = cmd;
	sb600_spibar[1] = readcnt << 4 | (writecnt);

	/* Before we use the FIFO, reset it first. */
	reset_internal_fifo_pointer();

	/* Send the write byte to FIFO. */
	for (count = 0; count < writecnt; count++, writearr++) {
		printf_debug(" [%x]", *writearr);
		sb600_spibar[0xC] = *writearr;
	}
	printf_debug("\n");

	/*
	 * We should send the data by sequence, which means we need to reset
	 * the FIFO pointer to the first byte we want to send.
	 */
	reset_internal_fifo_pointer();

	execute_command();

	/*
	 * After the command executed, we should find out the index of the
	 * received byte. Here we just reset the FIFO pointer, skip the
	 * writecnt, is there anyone who have anther method to replace it?
	 */
	reset_internal_fifo_pointer();

	for (count = 0; count < writecnt; count++) {
		cmd = sb600_spibar[0xC];	/* Skip the byte we send. */
		printf_debug("[ %2x]", cmd);
	}

	printf_debug("The FIFO pointer 6 is %d.\n", sb600_spibar[0xd] & 0x07);
	for (count = 0; count < readcnt; count++, readarr++) {
		*readarr = sb600_spibar[0xC];
		printf_debug("[%02x]", *readarr);
	}
	printf_debug("\n");

	return 0;
}
