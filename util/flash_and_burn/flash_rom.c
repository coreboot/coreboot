/*
 * flash_rom.c: Flash programming utility for SiS 630/950 M/Bs
 *
 *
 * Copyright 2000 Silicon Integrated System Corporation
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
 *
 * Reference:
 *	1. SiS 630 Specification
 *	2. SiS 950 Specification
 *
 * $Id$
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/io.h>
#include <unistd.h>
#include <stdio.h>

#include "flash.h"
#include "jedec.h"
#include "m29f400bt.h"

struct flashchip flashchips[] = {
    {"Am29F040B",   AMD_ID,     AM_29F040B,   NULL, 512, 64*1024,
     probe_29f040b, erase_29f040b, write_29f040b},
    {"At29C040A",   ATMEL_ID,   AT_29C040A,   NULL, 512, 256,
     probe_jedec,   erase_jedec,   write_jedec},
    {"Mx29f002",    MX_ID,      MX_29F002,    NULL, 256, 64*1024,
     probe_29f002,  erase_29f002,  write_29f002},
    {"SST29EE020A", SST_ID,     SST_29EE020A, NULL, 256, 128,
     probe_jedec,   erase_jedec,   write_jedec},
    {"SST28SF040A", SST_ID,     SST_28SF040,  NULL, 512, 256,
     probe_28sf040, erase_28sf040, write_28sf040},
    {"SST39SF020A", SST_ID,     SST_39SF020,  NULL, 256, 4096,
     probe_39sf020, erase_39sf020, write_39sf020},
    {"SST39VF020",  SST_ID,     SST_39VF020,  NULL, 256, 4096,
     probe_39sf020, erase_39sf020, write_39sf020},
    {"W29C020C",    WINBOND_ID, W_29C020C,    NULL, 256, 128,
     probe_jedec,   erase_jedec,   write_jedec},
    {"M29F400BT",   ST_ID, ST_M29F400BT ,    NULL, 512, 64*1024,
     probe_m29f400bt,   erase_m29f400bt,   write_linuxbios_m29f400bt},
    {NULL,}
};

int enable_flash_sis630 (void)
{
    char b;

    /* get io privilege access PCI configuration space */
    if (iopl(3) != 0) {
    	perror("Can not set io priviliage");
        exit(1);
    }

    /* Enable 0xFFF8000~0xFFFF0000 decoding on SiS 540/630 */
    outl(0x80000840, 0x0cf8);
    b = inb(0x0cfc) | 0x0b;
    outb(b, 0xcfc);
    /* Flash write enable on SiS 540/630 */
    outl(0x80000845, 0x0cf8);
    b = inb(0x0cfd) | 0x40;
    outb(b, 0xcfd);

    /* The same thing on SiS 950 SuperIO side */
    outb(0x87, 0x2e);
    outb(0x01, 0x2e);
    outb(0x55, 0x2e);
    outb(0x55, 0x2e);

    if (inb(0x2f) != 0x87) {
	outb(0x87, 0x4e);
    	outb(0x01, 0x4e);
    	outb(0x55, 0x4e);
    	outb(0xaa, 0x4e);
	if (inb(0x4f) != 0x87) {
	    printf("Can not access SiS 950\n");
	    return -1;
	}
	outb(0x24, 0x4e);
	b = inb(0x4f) | 0xfc;
	outb(0x24, 0x4e);
	outb(b, 0x4f);
	outb(0x02, 0x4e);
	outb(0x02, 0x4f);	
    }

    outb(0x24, 0x2e);
    printf("2f is %#x\n", inb(0x2f));
    b = inb(0x2f) | 0xfc;
    outb(0x24, 0x2e);
    outb(b, 0x2f);

    outb(0x02, 0x2e);
    outb(0x02, 0x2f);

    return 0;
}

struct flashchip * probe_flash(struct flashchip * flash)
{
    int fd_mem;
    volatile char * bios;
    unsigned long size;

    if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
	perror("Can not open /dev/mem");
	exit(1);
    }

    while (flash->name != NULL) {
	printf("Trying %s, %d KB\n", flash->name, flash->total_size);
	size = flash->total_size * 1024;
	bios = mmap (0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		     fd_mem, (off_t) (0 - size));
	if (bios == MAP_FAILED) {
	    perror("Error MMAP /dev/mem");
	    exit(1);
	}
	flash->virt_addr = bios;

	if (flash->probe(flash) == 1) {
	    printf ("%s found at physical address: 0x%lx\n",
		    flash->name, (0 - size), bios);
	    return flash;
	}
	munmap ((void *) bios, size);
	flash++;
    }
    return NULL;
}

int verify_flash (struct flashchip * flash, char * buf)
{
    int i = 0;
    int total_size = flash->total_size *1024;
    volatile char * bios = flash->virt_addr;

    printf("Verifying address: ");
    while (i++ < total_size) {
	printf("0x%08x", i);
	if (*(bios+i) != *(buf+i)) {
	    return 0;
	}
	printf("\b\b\b\b\b\b\b\b\b\b");
    }
    printf("\n");
    return 1;
}

// count to a billion. Time it. If it's < 1 sec, count to 10B, etc.

unsigned long micro = 0;

void 
myusec_calibrate_delay()
{
	unsigned long count = 2 *  1024 * 1024;
	volatile unsigned long i;
	unsigned long timeusec;
	struct timeval start, end;
	int ok = 0;

	fprintf(stderr, "Setting up microsecond timing loop\n");
	while (! ok) {
		fprintf(stderr, "Try %d\n", count);
		gettimeofday(&start, 0);
		for( i = count; i; i--)
			;
		gettimeofday(&end, 0);
		timeusec = 1000000 * (end.tv_sec - start.tv_sec ) + 
				(end.tv_usec - start.tv_usec);
		fprintf(stderr, "timeusec is %d\n", timeusec);
		count *= 10;
		if (timeusec < 1000000)
			continue;
		ok = 1;
	}

	// compute one microsecond. That will be count / time
	micro = count / timeusec;

	fprintf(stderr, "one us is %d count\n", micro);


}

void
myusec_delay(time)
{
  volatile unsigned long i;
  for(i = 0; i < time * micro; i++)
	;

}

main (int argc, char * argv[])
{
    char * buf;
    unsigned long size;
    FILE * image;
    struct flashchip * flash;

    myusec_calibrate_delay();

    if (argc > 2){
	printf("usage: %s [romimage]\n", argv[0]);
	printf(" If no romimage is specified, then all that happens\n");
	printf(" is that flash info is dumped\n");
    }

    if ((flash = probe_flash (flashchips)) == NULL) {
	printf("EEPROM not found\n");
	exit(1);
    }

    printf("Part is %s\n", flash->name);
    if (argc < 2){
	printf("OK, only ENABLING flash write, but NOT FLASHING\n");
        exit(0);
    }
    size = flash->total_size * 1024;

    if ((image = fopen (argv[1], "r")) == NULL) {
	perror("Error opening image file");
	exit(1);
    }

    buf = (char *) calloc (size, sizeof(char));
    fread (buf, sizeof(char), size, image);

    flash->write (flash, buf);
    verify_flash (flash, buf);
}
