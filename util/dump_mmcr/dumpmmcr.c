/*
 * dump mmcr of Elan520 uController (incomplete, see 22005b pg23+).
 *
 * Copyright 2006 coresystems GmbH 
 *      Stefan Reinauer <stepan@coresystems.de> 
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
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

int dump_mmcr(void)
{
	int fd_mem;
	volatile uint8_t *mmcr;
	unsigned long size=4096;

	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(1);
	}

	if (getpagesize() > size) {
		size = getpagesize();
	}
	
	mmcr = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) (0xFFFEF000));
	
	if (mmcr == MAP_FAILED) {
		perror("Error MMAP /dev/mem");
		exit(1);
	}
	
	printf("ElanSC520 uC Rev. ID  : %04x\n",*(uint16_t *)mmcr);
	printf("Am5x86 CPU Control    : %04x\n",*(uint16_t *)(mmcr+2));
	printf("\n");
	printf("SDRAM Control         : %04x\n",*(uint16_t *)(mmcr+0x10));
	printf("SDRAM Timing Control  : %04x\n",*(uint16_t *)(mmcr+0x12));
	printf("SDRAM Bank Config     : %04x\n",*(uint16_t *)(mmcr+0x14));
	printf("SDRAM Bank 0-3 Ending : %04x\n",*(uint16_t *)(mmcr+0x18));
	printf("ECC Control           : %04x\n",*(uint16_t *)(mmcr+0x20));
	printf("ECC Status            : %04x\n",*(uint16_t *)(mmcr+0x21));
	printf("ECC Check Bit Position: %04x\n",*(uint16_t *)(mmcr+0x22));
	printf("ECC Check Code Test   : %04x\n",*(uint16_t *)(mmcr+0x23));
	printf("ECC Single Bit ErrAddr: %04x\n",*(uint16_t *)(mmcr+0x24));
	printf("ECC Multi Bit ErrAddr : %04x\n",*(uint16_t *)(mmcr+0x28));
	printf("\n");
	printf("SDRAM Buffer Control  : %04x\n",*(uint16_t *)(mmcr+0x40));
	printf("\n");
	printf("BOOTCS Control        : %04x\n",*(uint16_t *)(mmcr+0x50));
	printf("BOOTCS1 Control       : %04x\n",*(uint16_t *)(mmcr+0x54));
	printf("BOOTCS2 Control       : %04x\n",*(uint16_t *)(mmcr+0x56));
	printf("\n");

	printf("Adr Decode Control    : %02x\n",*(uint8_t *)(mmcr+0x80));
	printf("WrProt Violation Stat.: %04x\n",*(uint16_t *)(mmcr+0x82));
	printf("PAR 0                 : %08x\n",*(uint32_t *)(mmcr+0x88));
	printf("PAR 1                 : %08x\n",*(uint32_t *)(mmcr+0x8C));
	printf("PAR 2                 : %08x\n",*(uint32_t *)(mmcr+0x90));
	printf("PAR 3                 : %08x\n",*(uint32_t *)(mmcr+0x94));
	printf("PAR 4                 : %08x\n",*(uint32_t *)(mmcr+0x98));
	printf("PAR 5                 : %08x\n",*(uint32_t *)(mmcr+0x9C));
	printf("PAR 6                 : %08x\n",*(uint32_t *)(mmcr+0xA0));
	printf("PAR 7                 : %08x\n",*(uint32_t *)(mmcr+0xA4));
	printf("PAR 8                 : %08x\n",*(uint32_t *)(mmcr+0xA8));
	printf("PAR 9                 : %08x\n",*(uint32_t *)(mmcr+0xAC));
	printf("PAR 10                : %08x\n",*(uint32_t *)(mmcr+0xB0));
	printf("PAR 11                : %08x\n",*(uint32_t *)(mmcr+0xB4));
	printf("PAR 12                : %08x\n",*(uint32_t *)(mmcr+0xB8));
	printf("PAR 13                : %08x\n",*(uint32_t *)(mmcr+0xBC));
	printf("PAR 14                : %08x\n",*(uint32_t *)(mmcr+0xC0));
	printf("PAR 15                : %08x\n",*(uint32_t *)(mmcr+0xC4));
	
	munmap((void *) mmcr, size);
	return 0;
}

int main(int argc, char *argv[])
{
	dump_mmcr();
	return 0;
}
