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
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <pci/pci.h>
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
#include "mx29f002.h"

struct flashchip flashchips[] = {
    {"Am29F040B",   AMD_ID,     AM_29F040B,   NULL, 512, 64*1024,
     probe_29f040b, erase_29f040b, write_29f040b, NULL},
    {"At29C040A",   ATMEL_ID,   AT_29C040A,   NULL, 512, 256,
     probe_jedec,   erase_jedec,   write_jedec, NULL},
    {"Mx29f002",    MX_ID,      MX_29F002,    NULL, 256, 64*1024,
     probe_29f002,  erase_29f002,  write_29f002, NULL},
    {"SST29EE020A", SST_ID,     SST_29EE020A, NULL, 256, 128,
     probe_jedec,   erase_jedec,   write_jedec, NULL},
    {"SST28SF040A", SST_ID,     SST_28SF040,  NULL, 512, 256,
     probe_28sf040, erase_28sf040, write_28sf040, NULL},
    {"SST39SF020A", SST_ID,     SST_39SF020,  NULL, 256, 4096,
     probe_39sf020, erase_39sf020, write_39sf020, NULL},
    {"SST39VF020",  SST_ID,     SST_39VF020,  NULL, 256, 4096,
     probe_39sf020, erase_39sf020, write_39sf020, NULL},
    {"W29C011",    WINBOND_ID, W_29C011,    NULL, 128, 128,
     probe_jedec,   erase_jedec,   write_jedec, NULL},
    {"W29C020C",    WINBOND_ID, W_29C020C,    NULL, 256, 128,
     probe_jedec,   erase_jedec,   write_jedec, NULL},
    {"W49F002U",    WINBOND_ID, W_49F002U,    NULL, 256, 128,
     probe_49f002,   erase_49f002,   write_49f002, NULL},
    {"M29F400BT",   ST_ID, ST_M29F400BT ,    NULL, 512, 64*1024,
     probe_m29f400bt,   erase_m29f400bt,   write_linuxbios_m29f400bt, NULL},
    {"82802ab",   137, 173 ,    NULL, 512, 64*1024,
     probe_82802ab,   erase_82802ab,   write_82802ab, NULL},
    {"82802ac",   137, 172 ,    NULL, 1024, 64*1024,
     probe_82802ab,   erase_82802ab,   write_82802ab, NULL},
    {"MD-2802 (M-Systems DiskOnChip Millennium Module)",
     MSYSTEMS_ID, MSYSTEMS_MD2802,
     NULL, 8, 8*1024,
     probe_md2802, erase_md2802, write_md2802, read_md2802},
    {NULL,}
};

char *chip_to_probe = NULL;

int enable_flash_sis630 (struct pci_dev *dev, char *name)
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

int
enable_flash_e7500(struct pci_dev *dev, char *name) {
  /* register 4e.b gets or'ed with one */
  unsigned char old, new;
  /* if it fails, it fails. There are so many variations of broken mobos
   * that it is hard to argue that we should quit at this point. 
   */
  
  old = pci_read_byte(dev, 0x4e);

  new = old | 1;

  if (new == old)
      return 0;

  pci_write_byte(dev, 0x4e, new);

  if (pci_read_byte(dev, 0x4e) != new) {
    printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 
	   0x4e, new, name);
    return -1;
  }
  return 0;
}

int
enable_flash_vt8235(struct pci_dev *dev, char *name) {
  unsigned char old, new, val;
  unsigned int base;
  int ok;
  
  /* get io privilege access PCI configuration space */
  if (iopl(3) != 0) {
      perror("Can not set io priviliage");
      exit(1);
  }

  old = pci_read_byte(dev, 0x40);

  new = old | 0x10;

  if (new == old)
      return 0;

  ok = pci_write_byte(dev, 0x40, new);
  if (ok != 0) {
    printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 
	   old, new, name);
  }

  /* enable GPIO15 which is connected to write protect. */
  base = ((pci_read_byte(dev, 0x88) & 0x80) | pci_read_byte(dev, 0x89) << 8);
  val = inb(base + 0x4d);
  val |= 0x80;
  outb(val, base + 0x4d);

  if (ok != 0) {
    return -1;
  } else {
    return 0;
  }
}

int
enable_flash_vt8231(struct pci_dev *dev, char *name) {
  unsigned char val;
  
  val = pci_read_byte(dev, 0x40);
  val |= 0x10;
  pci_write_byte(dev, 0x40, val);

  if (pci_read_byte(dev, 0x40) != val) {
    printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 
	   0x40, val, name);
    return -1;
  }
  return 0;
}

int
enable_flash_cs5530(struct pci_dev *dev, char *name) {
  unsigned char new;
  
  pci_write_byte(dev, 0x52, 0xee);

  new = pci_read_byte(dev, 0x52);

  if (new != 0xee) {
    printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 
	   0x52, new, name);
    return -1;
  }
  return 0;
}

int
enable_flash_sc1100(struct pci_dev *dev, char *name) {
  unsigned char new;
  
  pci_write_byte(dev, 0x52, 0xee);

  new = pci_read_byte(dev, 0x52);

  if (new != 0xee) {
    printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 
	   0x52, new, name);
    return -1;
  }
  return 0;
}

int
enable_flash_sis5595(struct pci_dev *dev, char *name) {
  unsigned char new, newer;
  
  new = pci_read_byte(dev, 0x45);

  /* clear bit 5 */
  new &= (~ 0x20);
  /* set bit 2 */
  new |= 0x4;

  pci_write_byte(dev, 0x45, new);

  newer = pci_read_byte(dev, 0x45);
  if (newer != new) {
    printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 
	   0x45, new, name);
    printf("Stuck at 0x%x\n", newer);
    return -1;
  }
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
	if (chip_to_probe && strcmp(flash->name, chip_to_probe) != 0) {
	    flash++;
	    continue;
	}
	printf("Trying %s, %d KB\n", flash->name, flash->total_size);
	size = flash->total_size * 1024;
/* BUG? what happens if getpagesize() > size!?
   -> ``Error MMAP /dev/mem: Invalid argument'' NIKI */
	if(getpagesize() > size)
	{
		size = getpagesize();
		printf("%s: warning: size: %d -> %ld\n", __FUNCTION__,
	 		flash->total_size * 1024, (unsigned long)size);
	}
	bios = mmap (0, size, PROT_WRITE | PROT_READ, MAP_SHARED,
		     fd_mem, (off_t) (0 - size));
	if (bios == MAP_FAILED) {
	    perror("Error MMAP /dev/mem");
	    exit(1);
	}
	flash->virt_addr = bios;
        flash->fd_mem = fd_mem;

	if (flash->probe(flash) == 1) {
	    printf ("%s found at physical address: 0x%lx\n",
		    flash->name, (0 - size));
	    return flash;
	}
	munmap ((void *) bios, size);
	flash++;
    }
    return NULL;
}

int verify_flash (struct flashchip * flash, char * buf, int verbose)
{
    int i = 0;
    int total_size = flash->total_size *1024;
    volatile char * bios = flash->virt_addr;

    printf("Verifying address: ");
    while (i++ < total_size) {
	if (verbose) 
		printf("0x%08x", i);
	if (*(bios+i) != *(buf+i)) {
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

// count to a billion. Time it. If it's < 1 sec, count to 10B, etc.

unsigned long micro = 1;

void 
myusec_calibrate_delay()
{
        int count = 1000;
	unsigned long timeusec;
	struct timeval start, end;
	int ok = 0;
	void myusec_delay(int time);

	printf("Setting up microsecond timing loop\n");
	while (! ok) {
		//fprintf(stderr, "Try %d\n", count);
		gettimeofday(&start, 0);
                myusec_delay(count);
		gettimeofday(&end, 0);
		timeusec = 1000000 * (end.tv_sec - start.tv_sec ) + 
				(end.tv_usec - start.tv_usec);
		//fprintf(stderr, "timeusec is %d\n", timeusec);
		count *= 2;
		if (timeusec < 1000000/4)
			continue;
		ok = 1;
	}

	// compute one microsecond. That will be count / time
	micro = count / timeusec;

	fprintf(stderr, "%ldM loops per second\n", (unsigned long)micro);


}

void
myusec_delay(int time)
{
  volatile unsigned long i;
  for(i = 0; i < time * micro; i++)
	;
}

typedef struct penable {
  unsigned short vendor, device; 
  char *name;
  int (*doit)(struct pci_dev *dev, char *name);
} FLASH_ENABLE;

FLASH_ENABLE enables[] = {

  {0x1, 0x1, "sis630 -- what's the ID?", enable_flash_sis630},
  {0x8086, 0x2480, "E7500", enable_flash_e7500},
  {0x1106, 0x8231, "VT8231", enable_flash_vt8231},
  {0x1106, 0x3177, "VT8235", enable_flash_vt8235},
  {0x1078, 0x0100, "CS5530", enable_flash_cs5530},
  {0x100b, 0x0510, "SC1100", enable_flash_sc1100},
  {0x1039, 0x8, "SIS5595", enable_flash_sis5595},
};
  
int
enable_flash_write() {
  int i;
  struct pci_access *pacc;
  struct pci_dev *dev = 0;
  FLASH_ENABLE *enable = 0;

  pacc = pci_alloc();           /* Get the pci_access structure */
  /* Set all options you want -- here we stick with the defaults */
  pci_init(pacc);               /* Initialize the PCI library */
  pci_scan_bus(pacc);           /* We want to get the list of devices */

  /* now let's try to find the chipset we have ... */
  for(i = 0; i < sizeof(enables)/sizeof(enables[0]) && (! dev); i++) {
    struct pci_filter f;
    struct pci_dev *z;
    /* the first param is unused. */
    pci_filter_init((struct pci_access *) 0, &f);
    f.vendor = enables[i].vendor;
    f.device = enables[i].device;
    for(z=pacc->devices; z; z=z->next)
      if (pci_filter_match(&f, z)) {
	enable = &enables[i];
	dev = z;
      }
  }

  /* now do the deed. */
  if (enable) {
      printf("Enabling flash write on %s...", enable->name);
      if (enable->doit(dev, enable->name) == 0)
          printf("OK\n");
  }
  return 0;
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

int
main (int argc, char * argv[])
{
    char * buf;
    unsigned long size;
    FILE * image;
    struct flashchip * flash;
    int opt;
    int read_it = 0, write_it = 0, verify_it = 0;
    char *filename = NULL;

    setbuf(stdout, NULL);

    while ((opt = getopt(argc, argv, "rwvc:")) != EOF) {
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
    
    if ((flash = probe_flash (flashchips)) == NULL) {
	printf("EEPROM not found\n");
	exit(1);
    }

    printf("Part is %s\n", flash->name);
    if (!filename){
	printf("OK, only ENABLING flash write, but NOT FLASHING\n");
	return 0;
    }
    size = flash->total_size * 1024;
    buf = (char *) calloc (size, sizeof(char));

    if (read_it) {
        if ((image = fopen (filename, "w")) == NULL) {
            perror(filename);
            exit(1);
        }
        printf("Reading Flash...");
	if(flash->read == NULL)
		memcpy(buf, (const char *) flash->virt_addr, size);
	else
		flash->read (flash, buf);
        fwrite(buf, sizeof(char), size, image);
        fclose(image);
        printf("done\n");
    } else {
        if ((image = fopen (filename, "r")) == NULL) {
            perror(filename);
            exit(1);
        }
        fread (buf, sizeof(char), size, image);
        fclose(image);
    }

    if (write_it || (!read_it && !verify_it))
        flash->write (flash, buf);
    if (verify_it)
        verify_flash (flash, buf, /* verbose = */ 0);
    return 0;
}
