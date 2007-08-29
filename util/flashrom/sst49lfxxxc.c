/*
 * This file is part of the flashrom project.
 *
 * Copyright 2000 Silicon Integrated System Corporation
 * Copyright 2005-2007 coresystems GmbH
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

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "flash.h"

#define SECTOR_ERASE		0x30
#define BLOCK_ERASE		0x20
#define ERASE			0xD0
#define AUTO_PGRM		0x10
#define RESET			0xFF
#define READ_ID			0x90
#define READ_STATUS		0x70
#define CLEAR_STATUS		0x50

#define STATUS_BPS		(1 << 1)
#define	STATUS_ESS		(1 << 6)
#define	STATUS_WSMS		(1 << 7)

static __inline__ int write_lockbits_49lfxxxc(volatile uint8_t *bios, int size,
					      unsigned char bits)
{
	int i, left = size;
	unsigned long address;

	//printf("bios=0x%08lx\n", (unsigned long)bios);
	for (i = 0; left > 65536; i++, left -= 65536) {
		//printf("lockbits at address=0x%08lx is 0x%01x\n", (unsigned long)0xFFC00000 - size + (i * 65536) + 2, *(bios + (i * 65536) + 2) );
		*(bios + (i * 65536) + 2) = bits;
	}
	address = i * 65536;
	//printf("lockbits at address=0x%08lx is 0x%01x\n", (unsigned long)0xFFc00000 - size + address + 2, *(bios + address + 2) );
	*(bios + address + 2) = bits;
	address += 32768;
	//printf("lockbits at address=0x%08lx is 0x%01x\n", (unsigned long)0xFFc00000 - size + address + 2, *(bios + address + 2) );
	*(bios + address + 2) = bits;
	address += 8192;
	//printf("lockbits at address=0x%08lx is 0x%01x\n", (unsigned long)0xFFc00000 - size + address + 2, *(bios + address + 2) );
	*(bios + address + 2) = bits;
	address += 8192;
	//printf("lockbits at address=0x%08lx is 0x%01x\n", (unsigned long)0xFFc00000 - size + address + 2, *(bios + address + 2) );
	*(bios + address + 2) = bits;

	return 0;
}

static __inline__ int erase_sector_49lfxxxc(volatile uint8_t *bios,
					    unsigned long address)
{
	unsigned char status;

	*bios = SECTOR_ERASE;
	*(bios + address) = ERASE;

	do {
		status = *bios;
		if (status & (STATUS_ESS | STATUS_BPS)) {
			printf("sector erase FAILED at address=0x%08lx status=0x%01x\n", (unsigned long)bios + address, status);
			*bios = CLEAR_STATUS;
			return (-1);
		}
	} while (!(status & STATUS_WSMS));

	return 0;
}

static __inline__ int write_sector_49lfxxxc(volatile uint8_t *bios,
					    uint8_t *src,
					    volatile uint8_t *dst,
					    unsigned int page_size)
{
	int i;
	unsigned char status;

	*bios = CLEAR_STATUS;
	for (i = 0; i < page_size; i++) {
		/* transfer data from source to destination */
		if (*src == 0xFF) {
			dst++, src++;
			/* If the data is 0xFF, don't program it */
			continue;
		}
		/*issue AUTO PROGRAM command */
		*bios = AUTO_PGRM;
		*dst++ = *src++;

		do {
			status = *bios;
			if (status & (STATUS_ESS | STATUS_BPS)) {
				printf("sector write FAILED at address=0x%08lx status=0x%01x\n", (unsigned long)dst, status);
				*bios = CLEAR_STATUS;
				return (-1);
			}
		} while (!(status & STATUS_WSMS));
	}

	return 0;
}

int probe_49lfxxxc(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;

	uint8_t id1, id2;

	*bios = RESET;

	*bios = READ_ID;
	id1 = *(volatile uint8_t *)bios;
	id2 = *(volatile uint8_t *)(bios + 0x01);

	*bios = RESET;

	printf_debug("%s: id1 0x%x, id2 0x%x\n", __FUNCTION__, id1, id2);

	if (!(id1 == flash->manufacture_id && id2 == flash->model_id))
		return 0;

	map_flash_registers(flash);

	return 1;
}

int erase_49lfxxxc(struct flashchip *flash)
{
	volatile uint8_t *bios = flash->virtual_memory;
	volatile uint8_t *registers = flash->virtual_registers;
	int i;
	unsigned int total_size = flash->total_size * 1024;

	write_lockbits_49lfxxxc(registers, total_size, 0);
	for (i = 0; i < total_size; i += flash->page_size)
		if (erase_sector_49lfxxxc(bios, i) != 0)
			return (-1);

	*bios = RESET;

	return 0;
}

int write_49lfxxxc(struct flashchip *flash, uint8_t *buf)
{
	int i;
	int total_size = flash->total_size * 1024;
	int page_size = flash->page_size;
	volatile uint8_t *bios = flash->virtual_memory;

	write_lockbits_49lfxxxc(flash->virtual_registers, total_size, 0);
	printf("Programming Page: ");
	for (i = 0; i < total_size / page_size; i++) {
		/* erase the page before programming */
		erase_sector_49lfxxxc(bios, i * page_size);

		/* write to the sector */
		printf("%04d at address: 0x%08x", i, i * page_size);
		write_sector_49lfxxxc(bios, buf + i * page_size,
				      bios + i * page_size, page_size);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	printf("\n");

	*bios = RESET;

	return 0;
}
