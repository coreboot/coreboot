/*
 *   flash rom utility: enable flash writes
 *
 *   Copyright (C) 2000-2004 ???
 *   Copyright (C) 2005 coresystems GmbH <stepan@openbios.org>
 *   Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   version 2
 *
 */

#include <sys/io.h>
#include <stdio.h>
#include <pci/pci.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "lbtable.h"
#include "debug.h"

// We keep this for the others.
static struct pci_access *pacc;

static int enable_flash_sis630(struct pci_dev *dev, char *name)
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

/* Datasheet:
 *   - Name: 82371AB PCI-TO-ISA / IDE XCELERATOR (PIIX4)
 *   - URL: http://www.intel.com/design/intarch/datashts/290562.htm
 *   - PDF: http://www.intel.com/design/intarch/datashts/29056201.pdf
 *   - Order Number: 290562-001
 */
static int enable_flash_piix4(struct pci_dev *dev, char *name)
{
	uint16_t old, new;
	uint16_t xbcs = 0x4e; /* X-Bus Chip Select register. */

	old = pci_read_word(dev, xbcs);

	/* Set bit 9: 1-Meg Extended BIOS Enable (PCI master accesses to
                      FFF00000-FFF7FFFF are forwarded to ISA).
           Set bit 7: Extended BIOS Enable (PCI master accesses to
                      FFF80000-FFFDFFFF are forwarded to ISA).
           Set bit 6: Lower BIOS Enable (PCI master, or ISA master accesses to
                      the lower 64-Kbyte BIOS block (E0000-EFFFF) at the top
                      of 1 Mbyte, or the aliases at the top of 4 Gbyte
                      (FFFE0000-FFFEFFFF) result in the generation of BIOSCS#.
           Note: Accesses to FFFF0000-FFFFFFFF are always forwarded to ISA.
           Set bit 2: BIOSCS# Write Enable (1=enable, 0=disable). */
	new = old | 0x2c4;

	if (new == old)
		return 0;

	pci_write_word(dev, xbcs, new);

	if (pci_read_word(dev, xbcs) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", xbcs, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_ich(struct pci_dev *dev, char *name, int bios_cntl)
{
	/* register 4e.b gets or'ed with one */
	uint8_t old, new;

	/* if it fails, it fails. There are so many variations of broken mobos
	 * that it is hard to argue that we should quit at this point. 
	 */

	/* Note: the ICH0-ICH5 BIOS_CNTL register is actually 16 bit wide, but
         * just treating it as 8 bit wide seems to work fine in practice. 
	 */

	/* see ie. page 375 of "Intel ICH7 External Design Specification"
	 * http://download.intel.com/design/chipsets/datashts/30701302.pdf 
	 */

	old = pci_read_byte(dev, bios_cntl);

	new = old | 1;

	if (new == old)
		return 0;

	pci_write_byte(dev, bios_cntl, new);

	if (pci_read_byte(dev, bios_cntl) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       bios_cntl, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_ich_4e(struct pci_dev *dev, char *name)
{
	return enable_flash_ich(dev, name, 0x4e);
}

static int enable_flash_ich_dc(struct pci_dev *dev, char *name)
{
	return enable_flash_ich(dev, name, 0xdc);
}

static int enable_flash_vt8235(struct pci_dev *dev, char *name)
{
	uint8_t old, new, val;
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

static int enable_flash_vt8231(struct pci_dev *dev, char *name)
{
	uint8_t val;

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

static int enable_flash_cs5530(struct pci_dev *dev, char *name)
{
	uint8_t new;

	pci_write_byte(dev, 0x52, 0xee);

	new = pci_read_byte(dev, 0x52);

	if (new != 0xee) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x52, new, name);
		return -1;
	}
	
	new = pci_read_byte(dev, 0x5b) | 0x20;
	pci_write_byte(dev, 0x5b, new);
	
	return 0;
}


static int enable_flash_sc1100(struct pci_dev *dev, char *name)
{
	uint8_t new;

	pci_write_byte(dev, 0x52, 0xee);

	new = pci_read_byte(dev, 0x52);

	if (new != 0xee) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x52, new, name);
		return -1;
	}
	return 0;
}

static int enable_flash_sis5595(struct pci_dev *dev, char *name)
{
	uint8_t new, newer;

	new = pci_read_byte(dev, 0x45);

	/* clear bit 5 */
	new &= (~0x20);
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

static int enable_flash_amd8111(struct pci_dev *dev, char *name)
{
	/* register 4e.b gets or'ed with one */
	uint8_t old, new;
	/* if it fails, it fails. There are so many variations of broken mobos
	 * that it is hard to argue that we should quit at this point. 
	 */

	/* enable decoding at 0xffb00000 to 0xffffffff */
	old = pci_read_byte(dev, 0x43);
	new = old | 0xC0;
	if (new != old) {
		pci_write_byte(dev, 0x43, new);
		if (pci_read_byte(dev, 0x43) != new) {
			printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
			       0x43, new, name);
		}
	}

	old = pci_read_byte(dev, 0x40);
	new = old | 0x01;
	if (new == old)
		return 0;
	pci_write_byte(dev, 0x40, new);

	if (pci_read_byte(dev, 0x40) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		       0x40, new, name);
		return -1;
	}
	return 0;
}

//By yhlu
static int enable_flash_ck804(struct pci_dev *dev, char *name)
{
        /* register 4e.b gets or'ed with one */
        uint8_t old, new;
        /* if it fails, it fails. There are so many variations of broken mobos
         * that it is hard to argue that we should quit at this point. 
         */

        //dump_pci_device(dev); 
        
        old = pci_read_byte(dev, 0x88);
        new = old | 0xc0;
        if (new != old) {
                pci_write_byte(dev, 0x88, new);
                if (pci_read_byte(dev, 0x88) != new) {
                        printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
                               0x88, new, name);
                }
        }

        old = pci_read_byte(dev, 0x6d);
        new = old | 0x01;
        if (new == old)
                return 0;
        pci_write_byte(dev, 0x6d, new);

        if (pci_read_byte(dev, 0x6d) != new) {
                printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
                       0x6d, new, name);
                return -1;
        }
        return 0;
}

static int enable_flash_sb400(struct pci_dev *dev, char *name)
{
        uint8_t tmp;

	struct pci_filter f;
	struct pci_dev *smbusdev;

	/* get io privilege access */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}

	/* then look for the smbus device */
	pci_filter_init((struct pci_access *) 0, &f);
	f.vendor = 0x1002;
	f.device = 0x4372;
	
	for (smbusdev = pacc->devices; smbusdev; smbusdev = smbusdev->next) {
		if (pci_filter_match(&f, smbusdev)) {
			break;
		}
	}
	
	if(!smbusdev) {
		perror("smbus device not found. aborting\n");
		exit(1);
	}
	
	// enable some smbus stuff
	tmp=pci_read_byte(smbusdev, 0x79);
	tmp|=0x01;
	pci_write_byte(smbusdev, 0x79, tmp);

	// change southbridge
	tmp=pci_read_byte(dev, 0x48);
	tmp|=0x21;
	pci_write_byte(dev, 0x48, tmp);

	// now become a bit silly. 
	tmp=inb(0xc6f);
	outb(tmp,0xeb);
	outb(tmp, 0xeb);
	tmp|=0x40;
	outb(tmp, 0xc6f);
	outb(tmp, 0xeb);
	outb(tmp, 0xeb);

	return 0;
}

typedef struct penable {
	unsigned short vendor, device;
	char *name;
	int (*doit) (struct pci_dev * dev, char *name);
} FLASH_ENABLE;

static FLASH_ENABLE enables[] = {
	{0x1039, 0x0630, "SIS630", enable_flash_sis630},
	{0x8086, 0x7110, "PIIX4/PIIX4E/PIIX4M", enable_flash_piix4},
	{0x8086, 0x2410, "ICH", enable_flash_ich_4e},
	{0x8086, 0x2420, "ICH0", enable_flash_ich_4e},
	{0x8086, 0x2440, "ICH2", enable_flash_ich_4e},
	{0x8086, 0x244c, "ICH2-M", enable_flash_ich_4e},
	{0x8086, 0x2480, "ICH3-S", enable_flash_ich_4e},
	{0x8086, 0x248c, "ICH3-M", enable_flash_ich_4e},
	{0x8086, 0x24c0, "ICH4/ICH4-L", enable_flash_ich_4e},
	{0x8086, 0x24cc, "ICH4-M", enable_flash_ich_4e},
	{0x8086, 0x24d0, "ICH5/ICH5R", enable_flash_ich_4e},
	{0x8086, 0x2640, "ICH6/ICH6R", enable_flash_ich_dc},
	{0x8086, 0x2641, "ICH6-M", enable_flash_ich_dc},
	{0x8086, 0x27b8, "ICH7/ICH7R", enable_flash_ich_dc},
	{0x8086, 0x27b9, "ICH7M", enable_flash_ich_dc},
	{0x8086, 0x27bd, "ICH7MDH", enable_flash_ich_dc},
	{0x8086, 0x2810, "ICH8/ICH8R", enable_flash_ich_dc},
	{0x8086, 0x2812, "ICH8DH", enable_flash_ich_dc},
	{0x8086, 0x2814, "ICH8DO", enable_flash_ich_dc},
	{0x1106, 0x8231, "VT8231", enable_flash_vt8231},
	{0x1106, 0x3177, "VT8235", enable_flash_vt8235},
	{0x1106, 0x3227, "VT8237", enable_flash_vt8231},
	{0x1106, 0x0686, "VT82C686", enable_flash_amd8111},
	{0x1078, 0x0100, "CS5530", enable_flash_cs5530},
	{0x100b, 0x0510, "SC1100", enable_flash_sc1100},
	{0x1039, 0x0008, "SIS5595", enable_flash_sis5595},
	{0x1022, 0x7468, "AMD8111", enable_flash_amd8111},
	// this fallthrough looks broken.
        {0x10de, 0x0050, "NVIDIA CK804", enable_flash_ck804}, // LPC
        {0x10de, 0x0051, "NVIDIA CK804", enable_flash_ck804}, // Pro
        {0x10de, 0x00d3, "NVIDIA CK804", enable_flash_ck804}, // Slave, should not be here, to fix known bug for A01.

        {0x10de, 0x0260, "NVidia MCP51", enable_flash_ck804},
        {0x10de, 0x0261, "NVidia MCP51", enable_flash_ck804},
        {0x10de, 0x0262, "NVidia MCP51", enable_flash_ck804},
        {0x10de, 0x0263, "NVidia MCP51", enable_flash_ck804},

	{0x1002, 0x4377, "ATI SB400", enable_flash_sb400}, // ATI Technologies Inc IXP SB400 PCI-ISA Bridge (rev 80)
};

static int mbenable_island_aruma(void)
{
#define EFIR 0x2e      /* Extended function index register, either 0x2e or 0x4e    */
#define EFDR EFIR + 1  /* Extended function data register, one plus the index reg. */
	char b;

/*  Disable the flash write protect.  The flash write protect is 
 *  connected to the WinBond w83627hf GPIO 24.
 */

	/* get io privilege access winbond config space */
	if (iopl(3) != 0) {
		perror("Can not set io priviliage");
		exit(1);
	}
	
	printf("Disabling mainboard flash write protection.\n");

	outb(0x87, EFIR); // sequence to unlock extended functions
	outb(0x87, EFIR);

	outb(0x20, EFIR); // SIO device ID register
	b = inb(EFDR); 
	printf_debug("W83627HF device ID = 0x%x\n",b);

	if (b != 0x52) {
		perror("Incorrect device ID, aborting write protect disable\n");
		exit(1);
	}

	outb(0x2b, EFIR); // GPIO multiplexed pin reg.
	b = inb(EFDR) | 0x10; 
	outb(0x2b, EFIR); 
	outb(b, EFDR); // select GPIO 24 instead of WDTO

	outb(0x7, EFIR); // logical device select
	outb(0x8, EFDR); // point to device 8, GPIO port 2

	outb(0x30, EFIR); // logic device activation control
	outb(0x1, EFDR); // activate

	outb(0xf0, EFIR); // GPIO 20-27 I/O selection register
	b = inb(EFDR) & ~0x10; 
	outb(0xf0, EFIR); 
	outb(b, EFDR); // set GPIO 24 as an output

	outb(0xf1, EFIR); // GPIO 20-27 data register
	b = inb(EFDR) | 0x10; 
	outb(0xf1, EFIR); 
	outb(b, EFDR); // set GPIO 24

	outb(0xaa, EFIR); // command to exit extended functions

	return 0;
}

typedef struct mbenable {
	char *vendor, *part;
	int (*doit)(void);
} MAINBOARD_ENABLE;

static MAINBOARD_ENABLE mbenables[] = {
	{ "ISLAND", "ARUMA", mbenable_island_aruma },
};

int enable_flash_write()
{
	int i;
	struct pci_dev *dev = 0;
	FLASH_ENABLE *enable = 0;

	pacc = pci_alloc();	/* Get the pci_access structure */
	/* Set all options you want -- here we stick with the defaults */
	pci_init(pacc);		/* Initialize the PCI library */
	pci_scan_bus(pacc);	/* We want to get the list of devices */

	
	/* First look whether we have to do something for this
	 * motherboard.
	 */
	for (i = 0; i < sizeof(mbenables) / sizeof(mbenables[0]); i++) {
		if(lb_vendor && !strcmp(mbenables[i].vendor, lb_vendor) &&
		   lb_part && !strcmp(mbenables[i].part, lb_part)) {
			mbenables[i].doit();
			break;
		}
	}
	
	/* now let's try to find the chipset we have ... */
	for (i = 0; i < sizeof(enables) / sizeof(enables[0]) && (!dev);
	     i++) {
		struct pci_filter f;
		struct pci_dev *z;
		/* the first param is unused. */
		pci_filter_init((struct pci_access *) 0, &f);
		f.vendor = enables[i].vendor;
		f.device = enables[i].device;
		for (z = pacc->devices; z; z = z->next)
			if (pci_filter_match(&f, z)) {
				enable = &enables[i];
				dev = z;
			}
	}

	if (!enable) {
		printf("Warning: Unknown system. Flash detection "
			"will most likely fail.\n");
		return 1;
	}
	
	/* now do the deed. */
	printf("Enabling flash write on %s...", enable->name);
	if (enable->doit(dev, enable->name) == 0)
		printf("OK\n");
	return 0;
}
