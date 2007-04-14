/*
 * flashrom.c: Flash programming utility
 *
 * Copyright 2000 Silicon Integrated System Corporation
 * Copyright 2004 Tyan Corp
 *	yhlu yhlu@tyan.com add exclude start and end option
 * Copyright 2005-2007 coresystems GmbH 
 *      Stefan Reinauer <stepan@coresystems.de> added rom layout
 *      support, and checking for suitable rom image, various fixes
 *      support for flashing the Technologic Systems 5300.
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
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <pci/pci.h>

/* for iopl */
#if defined (__sun) && (defined(__i386) || defined(__amd64))
#include <strings.h>
#include <sys/sysi86.h>
#include <sys/psw.h>
#include <asm/sunddi.h>
#endif

#include "flash.h"
#include "lbtable.h"
#include "layout.h"
#include "debug.h"

char *chip_to_probe = NULL;
struct pci_access *pacc; /* For board and chipset_enable */
int exclude_start_page, exclude_end_page;
int force=0, verbose=0;

int fd_mem;

/*
 *
 */
struct pci_dev *
pci_dev_find(uint16_t vendor, uint16_t device)
{
        struct pci_dev  *temp;
        struct pci_filter  filter;

        pci_filter_init(NULL, &filter);
        filter.vendor = vendor;
        filter.device = device;

        for (temp = pacc->devices; temp; temp = temp->next)
                if (pci_filter_match(&filter, temp))
                        return temp;

        return NULL;
}

/*
 *
 */
struct pci_dev *
pci_card_find(uint16_t vendor, uint16_t device,
              uint16_t card_vendor, uint16_t card_device)
{
        struct pci_dev  *temp;
        struct pci_filter  filter;

        pci_filter_init(NULL, &filter);
        filter.vendor = vendor;
        filter.device = device;

        for (temp = pacc->devices; temp; temp = temp->next)
                if (pci_filter_match(&filter, temp)) {
                        if ((card_vendor == pci_read_word(temp, 0x2C)) &&
                            (card_device == pci_read_word(temp, 0x2E)))
                                return temp;
                }

        return NULL;
}

struct flashchip *probe_flash(struct flashchip *flash)
{
	volatile uint8_t *bios;
	unsigned long flash_baseaddr, size;

	while (flash->name != NULL) {
		if (chip_to_probe && strcmp(flash->name, chip_to_probe) != 0) {
			flash++;
			continue;
		}
		printf_debug("Probing for %s, %d KB\n", 
				flash->name, flash->total_size);

		size = flash->total_size * 1024;

#ifdef TS5300
		// FIXME: Wrong place for this decision
		flash_baseaddr = 0x9400000;
#else
		flash_baseaddr = (0xffffffff - size + 1);
#endif

		/* If getpagesize() > size -> 
		 * `Error MMAP /dev/mem: Invalid argument' 
		 * This should never happen as we don't support any flash chips
		 * smaller than 4k or 8k yet.
		 */

		if (getpagesize() > size) {
			size = getpagesize();
			printf("WARNING: size: %d -> %ld (page size)\n",
			       flash->total_size * 1024, (unsigned long) size);
		}

		bios = mmap(0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
			    fd_mem, (off_t)flash_baseaddr );
		if (bios == MAP_FAILED) {
			perror("Error: Can't mmap " MEM_DEV ".");
			exit(1);
		}
		flash->virt_addr = bios;

		if (flash->probe(flash) == 1) {
			printf("%s found at physical address: 0x%lx\n",
			       flash->name, flash_baseaddr);
			return flash;
		}
		munmap((void *) bios, size);

		flash++;
	}
	return NULL;
}

int verify_flash(struct flashchip *flash, uint8_t *buf)
{
	int idx;
	int total_size = flash->total_size * 1024;
	volatile uint8_t *bios = flash->virt_addr;

	printf("Verifying flash ");
	
	if(verbose) printf("address: 0x00000000\b\b\b\b\b\b\b\b\b\b");
	
	for (idx = 0; idx < total_size; idx++) {
		if (verbose && ( (idx & 0xfff) == 0xfff ))
			printf("0x%08x", idx);

		if (*(bios + idx) != *(buf + idx)) {
			if (verbose) {
				printf("0x%08x ", idx);
			}
			printf("- FAILED\n");
			return 1;
		}
		
		if (verbose && ( (idx & 0xfff) == 0xfff ))
			printf("\b\b\b\b\b\b\b\b\b\b");
	}
	if (verbose) 
		printf("\b\b\b\b\b\b\b\b\b\b ");
	
	printf("- VERIFIED         \n");
	return 0;
}


void usage(const char *name)
{
	printf("usage: %s [-rwvEVfh] [-c chipname] [-s exclude_start]\n", name);
	printf("       [-e exclude_end] [-m vendor:part] [-l file.layout] [-i imagename] [file]\n");
	printf("   -r | --read:                    read flash and save into file\n"
	       "   -w | --write:                   write file into flash (default when\n"
	       "                                   file is specified)\n"
	       "   -v | --verify:                  verify flash against file\n"
	       "   -E | --erase:                   erase flash device\n"
	       "   -V | --verbose:                 more verbose output\n"
	       "   -c | --chip <chipname>:         probe only for specified flash chip\n"
	       "   -s | --estart <addr>:           exclude start position\n"
	       "   -e | --eend <addr>:             exclude end postion\n"
	       "   -m | --mainboard <vendor:part>: override mainboard settings\n"
	       "   -f | --force:                   force write without checking image\n"
	       "   -l | --layout <file.layout>:    read rom layout from file\n"
	       "   -i | --image <name>:            only flash image name from flash layout\n"
	       "\n"
	       " If no file is specified, then all that happens\n"
	       " is that flash info is dumped.\n\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	uint8_t *buf;
	unsigned long size;
	FILE *image;
	struct flashchip *flash;
	int opt;
	int option_index = 0;
	int read_it = 0, 
	    write_it = 0, 
	    erase_it = 0, 
	    verify_it = 0;
	int ret = 0;

	static struct option long_options[]= {
		{ "read", 0, 0, 'r' },
		{ "write", 0, 0, 'w' },
		{ "erase", 0, 0, 'E' },
		{ "verify", 0, 0, 'v' },
		{ "chip", 1, 0, 'c' },
		{ "estart", 1, 0, 's' },
		{ "eend", 1, 0, 'e' },
		{ "mainboard", 1, 0, 'm' },
		{ "verbose", 0, 0, 'V' },
		{ "force", 0, 0, 'f' },
		{ "layout", 1, 0, 'l' },
		{ "image", 1, 0, 'i' },
		{ "help", 0, 0, 'h' },
		{ 0, 0, 0, 0 }
	};
	
	char *filename = NULL;


        unsigned int exclude_start_position=0, exclude_end_position=0; // [x,y)
	char *tempstr=NULL, *tempstr2=NULL;

	if (argc > 1) {
		/* Yes, print them. */
		int i;
		printf_debug ("The arguments are:\n");
		for (i = 1; i < argc; ++i)
			printf_debug ("%s\n", argv[i]);
	}

	setbuf(stdout, NULL);
	while ((opt = getopt_long(argc, argv, "rwvVEfc:s:e:m:l:i:h", long_options,
					&option_index)) != EOF) {
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
		case 'E':
			erase_it = 1;
			break;
		case 's':
			tempstr = strdup(optarg);
			sscanf(tempstr,"%x",&exclude_start_position);
			break;
		case 'e':
			tempstr = strdup(optarg);
			sscanf(tempstr,"%x",&exclude_end_position);
			break;
		case 'm':
			tempstr = strdup(optarg);
			strtok(tempstr, ":");
			tempstr2=strtok(NULL, ":");
			if (tempstr2) {
				lb_vendor=tempstr;
				lb_part=tempstr2;
			} else {
				printf("warning: ignored wrong format of"
						" mainboard: %s\n", tempstr);
			}
			break;
		case 'f':
			force=1;
			break;
		case 'l':
			tempstr=strdup(optarg);
			if (read_romlayout(tempstr))
				exit(1);
			break;
		case 'i':
			tempstr=strdup(optarg);
			find_romentry(tempstr);
			break;
		case 'h':
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

        /* First get full io access */
#if defined (__sun) && (defined(__i386) || defined(__amd64))
	if (sysi86(SI86V86, V86SC_IOPL, PS_IOPL) != 0){
#else
	if (iopl(3) != 0) {
#endif
		fprintf(stderr, "ERROR: iopl failed: \"%s\"\n", strerror(errno));
		exit(1);
	}

        /* Initialize PCI access for flash enables */
	pacc = pci_alloc();	/* Get the pci_access structure */
	/* Set all options you want -- here we stick with the defaults */
	pci_init(pacc);		/* Initialize the PCI library */
	pci_scan_bus(pacc);	/* We want to get the list of devices */

	/* Open the memory device. A lot of functions need it */
	if ((fd_mem = open(MEM_DEV, O_RDWR)) < 0) {
		perror("Error: Can not access memory using " MEM_DEV ". You need to be root.");
		exit(1);
	}

	myusec_calibrate_delay();

	/* We look at the lbtable first to see if we need a
	 * mainboard specific flash enable sequence.
	 */
	linuxbios_init();

	/* try to enable it. Failure IS an option, since not all motherboards
	 * really need this to be done, etc., etc.
	 */
        ret = chipset_flash_enable();
        if (ret == -2)
                printf("WARNING: No chipset found. Flash detection "
                       "will most likely fail.\n");

        board_flash_enable(lb_vendor, lb_part);


	if ((flash = probe_flash(flashchips)) == NULL) {
		printf("No EEPROM/flash device found.\n");
		exit(1);
	}

	printf("Flash part is %s (%d KB)\n", flash->name, flash->total_size);

	if (!filename && !erase_it) {
		// FIXME: Do we really want this feature implicitly?
		printf("OK, only ENABLING flash write, but NOT FLASHING.\n");
		return 0;
	}

	size = flash->total_size * 1024;
	buf = (uint8_t *) calloc(size, sizeof(char));
	
	if (erase_it) {
		printf("Erasing flash chip\n");
		flash->erase(flash);
		exit(0);		
	} else if (read_it) {
		if ((image = fopen(filename, "w")) == NULL) {
			perror(filename);
			exit(1);
		}
		printf("Reading Flash...");
		if (flash->read == NULL)
			memcpy(buf, (const char *) flash->virt_addr, size);
		else
			flash->read(flash, buf);

		if (exclude_end_position - exclude_start_position > 0)
			memset(buf+exclude_start_position, 0,
			       exclude_end_position-exclude_start_position);

		fwrite(buf, sizeof(char), size, image);
		fclose(image);
		printf("done\n");
	} else {
		struct stat image_stat;

		if ((image = fopen(filename, "r")) == NULL) {
			perror(filename);
			exit(1);
		}
		if (fstat(fileno(image), &image_stat) != 0) {
			perror(filename);
			exit(1);
		}
		if(image_stat.st_size!=flash->total_size*1024) {
			fprintf(stderr, "Error: Image size doesnt match\n");
			exit(1);
		}

		fread(buf, sizeof(char), size, image);
		show_id(buf, size);
		fclose(image);
	}

	/* exclude range stuff. Nice idea, but at the moment it is only
	 * supported in hardware by the pm49fl004 chips. 
	 * Instead of implementing this for all chips I suggest advancing
	 * it to the rom layout feature below and drop exclude range
	 * completely once all flash chips can do rom layouts. stepan
	 */
	
	// ////////////////////////////////////////////////////////////
	if (exclude_end_position - exclude_start_position > 0)
		memcpy(buf+exclude_start_position,
		       (const char *) flash->virt_addr+exclude_start_position, 
		       exclude_end_position-exclude_start_position);

        exclude_start_page = exclude_start_position/flash->page_size;
	if ((exclude_start_position%flash->page_size) != 0) {
		exclude_start_page++;
	}
	exclude_end_page = exclude_end_position/flash->page_size;
	// ////////////////////////////////////////////////////////////

	// This should be moved into each flash part's code to do it 
	// cleanly. This does the job.
	handle_romentries(buf, (uint8_t *)flash->virt_addr);
	 
	// ////////////////////////////////////////////////////////////
	
	if (write_it)
		ret |= flash->write(flash, buf);

	if (verify_it)
		ret |= verify_flash(flash, buf);

	return ret;
}
