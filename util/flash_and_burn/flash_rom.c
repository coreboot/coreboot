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
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "flash.h"
#include "jedec.h"
#include "m29f400bt.h"
#include "82802ab.h"
#include "msys_doc.h"
#include "am29f040b.h"
#include "sst28sf040.h"
#include "w49f002u.h"
#include "sst39sf020.h"
#include "sst49lf040.h"
#include "pm49fl004.h"
#include "mx29f002.h"

struct flashchip flashchips[] = {
	{"Am29F040B",	AMD_ID, 	AM_29F040B,	NULL, 512, 64 * 1024,
	 probe_29f040b, erase_29f040b,	write_29f040b,	NULL},
	{"At29C040A",	ATMEL_ID,	AT_29C040A,	NULL, 512, 256,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"Mx29f002",	MX_ID,		MX_29F002,	NULL, 256, 64 * 1024,
	 probe_29f002,	erase_29f002, 	write_29f002,	NULL},
	{"SST29EE020A", SST_ID,		SST_29EE020A,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"SST28SF040A", SST_ID,		SST_28SF040,	NULL, 512, 256,
	 probe_28sf040, erase_28sf040, write_28sf040,	NULL},
	{"SST39SF020A", SST_ID,		SST_39SF020,	NULL, 256, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020,NULL},
	{"SST39VF020",	SST_ID,		SST_39VF020,	NULL, 256, 4096,
	 probe_jedec,	erase_chip_jedec, write_39sf020,NULL},
	{"SST49LF040",	SST_ID,		SST_49LF040, 	NULL, 512, 4096,
	 probe_jedec, 	erase_chip_jedec, write_49lf040,NULL},
	{"SST49LF080A",	SST_ID,		SST_49LF080A,	NULL, 1024, 4096,
	 probe_jedec,	erase_chip_jedec, write_49lf040,NULL},
	{"SST49LF002A",	SST_ID,		SST_49LF002A,	NULL, 256, 4096,
	 probe_jedec,	erase_chip_jedec, write_49lf040,NULL},
	{"SST49LF003A", SST_ID,		SST_49LF003A,	NULL, 384, 4096,
	 probe_jedec,	erase_chip_jedec, write_49lf040,NULL},
	{"SST49LF004A", SST_ID,		SST_49LF004A,	NULL, 512, 4096,
	 probe_jedec,	erase_chip_jedec, write_49lf040,NULL},
	{"SST49LF008A", SST_ID,		SST_49LF008A, 	NULL, 1024, 4096,
	 probe_jedec, 	erase_chip_jedec, write_49lf040,NULL},
	{"Pm49FL004",	PMC_ID,		PMC_49FL004,	NULL, 512, 64 * 1024,
	 probe_jedec,	erase_chip_jedec, write_49fl004,NULL},
	{"W29C011",	WINBOND_ID,	W_29C011,	NULL, 128, 128,
	 probe_jedec,	erase_chip_jedec, write_jedec,	NULL},
	{"W29C020C", 	WINBOND_ID, 	W_29C020C,	NULL, 256, 128,
	 probe_jedec, 	erase_chip_jedec, write_jedec,	NULL},
	{"W49F002U", 	WINBOND_ID, 	W_49F002U,	NULL, 256, 128,
	 probe_jedec,	erase_chip_jedec, write_49f002, NULL},
	{"M29F400BT",	ST_ID,		ST_M29F400BT,	NULL, 512, 64 * 1024,
	 probe_m29f400bt, erase_m29f400bt, write_linuxbios_m29f400bt, NULL},
	{"82802ab",	137,		173,		NULL, 512, 64 * 1024,
	 probe_82802ab, erase_82802ab,	write_82802ab,	NULL},
	{"82802ac",	137,		172,		NULL, 1024, 64 * 1024,
	 probe_82802ab, erase_82802ab,	write_82802ab,	NULL},
	{"MD-2802 (M-Systems DiskOnChip Millennium Module)",
	 MSYSTEMS_ID, MSYSTEMS_MD2802,
	 NULL, 8, 8 * 1024,
	 probe_md2802, erase_md2802, write_md2802, read_md2802},
	{NULL,}
};

char *chip_to_probe = NULL;

struct flashchip *probe_flash(struct flashchip *flash)
{
	int fd_mem;
	volatile char *bios;
	unsigned long size;

	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(1);
	}

	while (flash->name != NULL) {
		if (chip_to_probe && strcmp(flash->name, chip_to_probe) != 0) {
			flash++;
			continue;
		}
		printf("Trying %s, %d KB\n", flash->name, flash->total_size);
		size = flash->total_size * 1024;
		/* BUG? what happens if getpagesize() > size!?
		   -> ``Error MMAP /dev/mem: Invalid argument'' NIKI */
		if (getpagesize() > size) {
			size = getpagesize();
			printf("%s: warning: size: %d -> %ld\n",
			       __FUNCTION__, flash->total_size * 1024,
			       (unsigned long) size);
		}
		bios = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
			    fd_mem, (off_t) (0xffffffff - size + 1));
		if (bios == MAP_FAILED) {
			perror("Error MMAP /dev/mem");
			exit(1);
		}
		flash->virt_addr = bios;
		flash->fd_mem = fd_mem;

		if (flash->probe(flash) == 1) {
			printf("%s found at physical address: 0x%lx\n",
			       flash->name, (0xffffffff - size + 1));
			return flash;
		}
		munmap((void *) bios, size);
		flash++;
	}
	return NULL;
}

int verify_flash(struct flashchip *flash, char *buf, int verbose)
{
	int i;
	int total_size = flash->total_size * 1024;
	volatile char *bios = flash->virt_addr;

	printf("Verifying address: ");
	for (i = 0; i < total_size; i++) {
		if (verbose)
			printf("0x%08x", i);
		if (*(bios + i) != *(buf + i)) {
			printf("FAILED\n");
			return 0;
		}
		if (verbose)
			printf("\b\b\b\b\b\b\b\b\b\b");
	}
	if (verbose)
		printf("\n");
	else
		printf("VERIFIED\n");
	return 1;
}


void usage(const char *name)
{
	printf("usage: %s [-rwv] [-c chipname][file]\n", name);
	printf("-r: read flash and save into file\n"
	       "-w: write file into flash (default when file is specified)\n"
	       "-v: verify flash against file\n"
	       "-c: probe only for specified flash chip\n"
	       " If no file is specified, then all that happens\n"
	       " is that flash info is dumped\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	char *buf;
	unsigned long size;
	FILE *image;
	struct flashchip *flash;
	int opt;
	int read_it = 0, write_it = 0, verify_it = 0, verbose = 0;
	char *filename = NULL;

	setbuf(stdout, NULL);

	while ((opt = getopt(argc, argv, "rwvVc:")) != EOF) {
		switch (opt) {
		case 'r':
			read_it = 1;
			break;
		case 'w':
			write_it = 1;
			break;
		case 'v':
			verify_it = 1;
			break;
		case 'c':
			chip_to_probe = strdup(optarg);
			break;
		case 'V':
			verbose = 1;
			break;
		default:
			usage(argv[0]);
			break;
		}
	}
	if (read_it && write_it) {
		printf("-r and -w are mutually exclusive\n");
		usage(argv[0]);
	}

	if (optind < argc)
		filename = argv[optind++];

	printf("Calibrating timer since microsleep sucks ... takes a second\n");
	myusec_calibrate_delay();
	printf("OK, calibrated, now do the deed\n");

	/* try to enable it. Failure IS an option, since not all motherboards
	 * really need this to be done, etc., etc. It sucks.
	 */
	(void) enable_flash_write();

	if ((flash = probe_flash(flashchips)) == NULL) {
		printf("EEPROM not found\n");
		exit(1);
	}

	printf("Part is %s\n", flash->name);
	if (!filename) {
		printf
		    ("OK, only ENABLING flash write, but NOT FLASHING\n");
		return 0;
	}
	size = flash->total_size * 1024;
	buf = (char *) calloc(size, sizeof(char));

	if (read_it) {
		if ((image = fopen(filename, "w")) == NULL) {
			perror(filename);
			exit(1);
		}
		printf("Reading Flash...");
		if (flash->read == NULL)
			memcpy(buf, (const char *) flash->virt_addr, size);
		else
			flash->read(flash, buf);
		fwrite(buf, sizeof(char), size, image);
		fclose(image);
		printf("done\n");
	} else {
		if ((image = fopen(filename, "r")) == NULL) {
			perror(filename);
			exit(1);
		}
		fread(buf, sizeof(char), size, image);
		fclose(image);
	}

	if (write_it || (!read_it && !verify_it))
		flash->write(flash, buf);
	if (verify_it)
		verify_flash(flash, buf, verbose);
	return 0;
}
