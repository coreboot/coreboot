/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2000 Silicon Integrated System Corporation
 * Copyright (C) 2005-2007 coresystems GmbH <stepan@coresystems.de>
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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
 * Contains the chipset specific flash enables.
 */

#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <pci/pci.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "flash.h"

static int enable_flash_ali_m1533(struct pci_dev *dev, const char *name)
{
	uint8_t tmp;

	/*
	 * ROM Write enable, 0xFFFC0000-0xFFFDFFFF and
	 * 0xFFFE0000-0xFFFFFFFF ROM select enable.
	 */
	tmp = pci_read_byte(dev, 0x47);
	tmp |= 0x46;
	pci_write_byte(dev, 0x47, tmp);

	return 0;
}

static int enable_flash_sis630(struct pci_dev *dev, const char *name)
{
	uint8_t b;

	/* Enable 0xFFF8000~0xFFFF0000 decoding on SiS 540/630. */
	b = pci_read_byte(dev, 0x40);
	pci_write_byte(dev, 0x40, b | 0xb);

	/* Flash write enable on SiS 540/630. */
	b = pci_read_byte(dev, 0x45);
	pci_write_byte(dev, 0x45, b | 0x40);

	/* The same thing on SiS 950 Super I/O side... */

	/* First probe for Super I/O on config port 0x2e. */
	outb(0x87, 0x2e);
	outb(0x01, 0x2e);
	outb(0x55, 0x2e);
	outb(0x55, 0x2e);

	if (inb(0x2f) != 0x87) {
		/* If that failed, try config port 0x4e. */
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
static int enable_flash_piix4(struct pci_dev *dev, const char *name)
{
	uint16_t old, new;
	uint16_t xbcs = 0x4e;	/* X-Bus Chip Select register. */

	old = pci_read_word(dev, xbcs);

	/* Set bit 9: 1-Meg Extended BIOS Enable (PCI master accesses to
	 *            FFF00000-FFF7FFFF are forwarded to ISA).
	 * Set bit 7: Extended BIOS Enable (PCI master accesses to
	 *            FFF80000-FFFDFFFF are forwarded to ISA).
	 * Set bit 6: Lower BIOS Enable (PCI master, or ISA master accesses to
	 *            the lower 64-Kbyte BIOS block (E0000-EFFFF) at the top
	 *            of 1 Mbyte, or the aliases at the top of 4 Gbyte
	 *            (FFFE0000-FFFEFFFF) result in the generation of BIOSCS#.
	 * Note: Accesses to FFFF0000-FFFFFFFF are always forwarded to ISA.
	 * Set bit 2: BIOSCS# Write Enable (1=enable, 0=disable).
	 */
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

/*
 * See ie. page 375 of "Intel ICH7 External Design Specification"
 * http://download.intel.com/design/chipsets/datashts/30701302.pdf
 */
static int enable_flash_ich(struct pci_dev *dev, const char *name,
			    int bios_cntl)
{
	uint8_t old, new;

	/*
	 * Note: the ICH0-ICH5 BIOS_CNTL register is actually 16 bit wide, but
	 * just treating it as 8 bit wide seems to work fine in practice.
	 */
	old = pci_read_byte(dev, bios_cntl);

	new = old | 1;

	if (new == old)
		return 0;

	pci_write_byte(dev, bios_cntl, new);

	if (pci_read_byte(dev, bios_cntl) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", bios_cntl, new, name);
		return -1;
	}

	return 0;
}

static int enable_flash_ich_4e(struct pci_dev *dev, const char *name)
{
	return enable_flash_ich(dev, name, 0x4e);
}

static int enable_flash_ich_dc(struct pci_dev *dev, const char *name)
{
	return enable_flash_ich(dev, name, 0xdc);
}

static int enable_flash_vt823x(struct pci_dev *dev, const char *name)
{
	uint8_t val;

	/* ROM write enable */
	val = pci_read_byte(dev, 0x40);
	val |= 0x10;
	pci_write_byte(dev, 0x40, val);

	if (pci_read_byte(dev, 0x40) != val) {
		printf("\nWARNING: Failed to enable ROM Write on \"%s\"\n",
		       name);
		return -1;
	}

	return 0;
}

static int enable_flash_cs5530(struct pci_dev *dev, const char *name)
{
	uint8_t reg8;

	#define DECODE_CONTROL_REG2		0x5b	/* F0 index 0x5b */
	#define ROM_AT_LOGIC_CONTROL_REG	0x52	/* F0 index 0x52 */

	#define LOWER_ROM_ADDRESS_RANGE		(1 << 0)
	#define ROM_WRITE_ENABLE		(1 << 1)
	#define UPPER_ROM_ADDRESS_RANGE		(1 << 2)
	#define BIOS_ROM_POSITIVE_DECODE	(1 << 5)

	/* Decode 0x000E0000-0x000FFFFF (128 KB), not just 64 KB, and
	 * decode 0xFF000000-0xFFFFFFFF (16 MB), not just 256 KB.
	 * Make the configured ROM areas writable.
	 */
	reg8 = pci_read_byte(dev, ROM_AT_LOGIC_CONTROL_REG);
	reg8 |= LOWER_ROM_ADDRESS_RANGE;
	reg8 |= UPPER_ROM_ADDRESS_RANGE;
	reg8 |= ROM_WRITE_ENABLE;
	pci_write_byte(dev, ROM_AT_LOGIC_CONTROL_REG, reg8);

	/* Set positive decode on ROM. */
	reg8 = pci_read_byte(dev, DECODE_CONTROL_REG2);
	reg8 |= BIOS_ROM_POSITIVE_DECODE;
	pci_write_byte(dev, DECODE_CONTROL_REG2, reg8);

	return 0;
}

static int enable_flash_cs5536(struct pci_dev *dev, const char *name)
{
	int fd_msr;
	unsigned char buf[8];
	unsigned int addr = 0x1808;

	/* Geode systems write protect the BIOS via RCONFs (cache
	 * settings similar to MTRRs). To unlock, change MSR 0x1808
	 * top byte to 0x22. Reading and writing to msr, however
	 * requires instructions rdmsr/wrmsr, which are ring0 privileged
	 * instructions so only the kernel can do the read/write.  This
	 * function, therefore, requires that the msr kernel module be
	 * loaded to access these instructions from user space using
	 * device /dev/cpu/0/msr.  This hard-coded driver location
	 * could have potential problems on SMP machines since it
	 * assumes cpu0, but it is safe on the Geode which is not SMP.
	 *
	 * This is probably not portable beyond Linux.
	 */

	fd_msr = open("/dev/cpu/0/msr", O_RDONLY);
	if (!fd_msr) {
		perror("open msr");
		return -1;
	}
	lseek64(fd_msr, (off64_t) addr, SEEK_SET);
	read(fd_msr, buf, 8);
	close(fd_msr);
	if (buf[7] != 0x22) {
		printf("Enabling Geode MSR to write to flash.\n");
		buf[7] &= 0xFB;
		fd_msr = open("/dev/cpu/0/msr", O_WRONLY);
		if (!fd_msr) {
			perror("open msr");
			return -1;
		}
		lseek64(fd_msr, (off64_t) addr, SEEK_SET);
		if (write(fd_msr, buf, 8) < 0) {
			perror("msr write");
			printf
			    ("Cannot write to MSR.  Make sure msr kernel is loaded: 'modprobe msr'\n");
			return -1;
		}
		close(fd_msr);
	}
	return 0;
}

static int enable_flash_sc1100(struct pci_dev *dev, const char *name)
{
	uint8_t new;

	pci_write_byte(dev, 0x52, 0xee);

	new = pci_read_byte(dev, 0x52);

	if (new != 0xee) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 0x52, new, name);
		return -1;
	}

	return 0;
}

static int enable_flash_sis5595(struct pci_dev *dev, const char *name)
{
	uint8_t new, newer;

	new = pci_read_byte(dev, 0x45);

	new &= (~0x20);		/* Clear bit 5. */
	new |= 0x4;		/* Set bit 2. */

	pci_write_byte(dev, 0x45, new);

	newer = pci_read_byte(dev, 0x45);
	if (newer != new) {
		printf("tried to set register 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 0x45, new, name);
		printf("Stuck at 0x%x\n", newer);
		return -1;
	}

	return 0;
}

static int enable_flash_amd8111(struct pci_dev *dev, const char *name)
{
	uint8_t old, new;

	/* Enable decoding at 0xffb00000 to 0xffffffff. */
	old = pci_read_byte(dev, 0x43);
	new = old | 0xC0;
	if (new != old) {
		pci_write_byte(dev, 0x43, new);
		if (pci_read_byte(dev, 0x43) != new) {
			printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 0x43, new, name);
		}
	}

	old = pci_read_byte(dev, 0x40);
	new = old | 0x01;
	if (new == old)
		return 0;
	pci_write_byte(dev, 0x40, new);

	if (pci_read_byte(dev, 0x40) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 0x40, new, name);
		return -1;
	}

	return 0;
}

static int enable_flash_ck804(struct pci_dev *dev, const char *name)
{
	uint8_t old, new;

	old = pci_read_byte(dev, 0x88);
	new = old | 0xc0;
	if (new != old) {
		pci_write_byte(dev, 0x88, new);
		if (pci_read_byte(dev, 0x88) != new) {
			printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 0x88, new, name);
		}
	}

	old = pci_read_byte(dev, 0x6d);
	new = old | 0x01;
	if (new == old)
		return 0;
	pci_write_byte(dev, 0x6d, new);

	if (pci_read_byte(dev, 0x6d) != new) {
		printf("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n", 0x6d, new, name);
		return -1;
	}

	return 0;
}

/* ATI Technologies Inc IXP SB400 PCI-ISA Bridge (rev 80) */
static int enable_flash_sb400(struct pci_dev *dev, const char *name)
{
	uint8_t tmp;
	struct pci_filter f;
	struct pci_dev *smbusdev;

	/* Look for the SMBus device. */
	pci_filter_init((struct pci_access *)0, &f);
	f.vendor = 0x1002;
	f.device = 0x4372;

	for (smbusdev = pacc->devices; smbusdev; smbusdev = smbusdev->next) {
		if (pci_filter_match(&f, smbusdev)) {
			break;
		}
	}

	if (!smbusdev) {
		fprintf(stderr, "ERROR: SMBus device not found. Aborting.\n");
		exit(1);
	}

	/* Enable some SMBus stuff. */
	tmp = pci_read_byte(smbusdev, 0x79);
	tmp |= 0x01;
	pci_write_byte(smbusdev, 0x79, tmp);

	/* Change southbridge. */
	tmp = pci_read_byte(dev, 0x48);
	tmp |= 0x21;
	pci_write_byte(dev, 0x48, tmp);

	/* Now become a bit silly. */
	tmp = inb(0xc6f);
	outb(tmp, 0xeb);
	outb(tmp, 0xeb);
	tmp |= 0x40;
	outb(tmp, 0xc6f);
	outb(tmp, 0xeb);
	outb(tmp, 0xeb);

	return 0;
}

static int enable_flash_mcp55(struct pci_dev *dev, const char *name)
{
	uint8_t old, new, byte;
	uint16_t word;

	/* Set the 0-16 MB enable bits. */
	byte = pci_read_byte(dev, 0x88);
	byte |= 0xff;		/* 256K */
	pci_write_byte(dev, 0x88, byte);
	byte = pci_read_byte(dev, 0x8c);
	byte |= 0xff;		/* 1M */
	pci_write_byte(dev, 0x8c, byte);
	word = pci_read_word(dev, 0x90);
	word |= 0x7fff;		/* 16M */
	pci_write_word(dev, 0x90, word);

	old = pci_read_byte(dev, 0x6d);
	new = old | 0x01;
	if (new == old)
		return 0;
	pci_write_byte(dev, 0x6d, new);

	if (pci_read_byte(dev, 0x6d) != new) {
		printf
		    ("tried to set 0x%x to 0x%x on %s failed (WARNING ONLY)\n",
		     0x6d, new, name);
		return -1;
	}

	return 0;
}

static int enable_flash_ht1000(struct pci_dev *dev, const char *name)
{
	uint8_t byte;

	/* Set the 4MB enable bit. */
	byte = pci_read_byte(dev, 0x41);
	byte |= 0x0e;
	pci_write_byte(dev, 0x41, byte);

	byte = pci_read_byte(dev, 0x43);
	byte |= (1 << 4);
	pci_write_byte(dev, 0x43, byte);

	return 0;
}

typedef struct penable {
	uint16_t vendor, device;
	const char *name;
	int (*doit) (struct pci_dev *dev, const char *name);
} FLASH_ENABLE;

static const FLASH_ENABLE enables[] = {
	{0x1039, 0x0630, "SIS630", enable_flash_sis630},
	{0x8086, 0x7110, "PIIX4/PIIX4E/PIIX4M", enable_flash_piix4},
	{0x8086, 0x7198, "Intel 440MX", enable_flash_piix4},
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
	{0x8086, 0x27b0, "ICH7DH", enable_flash_ich_dc},
	{0x8086, 0x27b8, "ICH7/ICH7R", enable_flash_ich_dc},
	{0x8086, 0x27b9, "ICH7M", enable_flash_ich_dc},
	{0x8086, 0x27bd, "ICH7MDH", enable_flash_ich_dc},
	{0x8086, 0x2810, "ICH8/ICH8R", enable_flash_ich_dc},
	{0x8086, 0x2812, "ICH8DH", enable_flash_ich_dc},
	{0x8086, 0x2814, "ICH8DO", enable_flash_ich_dc},
	{0x1106, 0x8231, "VT8231", enable_flash_vt823x},
	{0x1106, 0x3177, "VT8235", enable_flash_vt823x},
	{0x1106, 0x3227, "VT8237", enable_flash_vt823x},
	{0x1106, 0x8324, "CX700", enable_flash_vt823x},
	{0x1106, 0x0686, "VT82C686", enable_flash_amd8111},
	{0x1078, 0x0100, "CS5530/CS5530A", enable_flash_cs5530},
	{0x100b, 0x0510, "SC1100", enable_flash_sc1100},
	{0x1039, 0x0008, "SIS5595", enable_flash_sis5595},
	{0x1022, 0x2080, "AMD GEODE CS5536", enable_flash_cs5536},
	{0x1022, 0x7468, "AMD8111", enable_flash_amd8111},
	{0x10B9, 0x1533, "ALi M1533", enable_flash_ali_m1533},
	{0x10de, 0x0050, "NVIDIA CK804", enable_flash_ck804},	/* LPC */
	{0x10de, 0x0051, "NVIDIA CK804", enable_flash_ck804},	/* Pro */
	{0x10de, 0x00d3, "NVIDIA CK804", enable_flash_ck804},	/* Slave, should not be here, to fix known bug for A01. */
	{0x10de, 0x0260, "NVidia MCP51", enable_flash_ck804},
	{0x10de, 0x0261, "NVidia MCP51", enable_flash_ck804},
	{0x10de, 0x0262, "NVidia MCP51", enable_flash_ck804},
	{0x10de, 0x0263, "NVidia MCP51", enable_flash_ck804},
	{0x10de, 0x0360, "NVIDIA MCP55", enable_flash_mcp55},	/* M57SLI-S4 */
	{0x10de, 0x0361, "NVIDIA MCP55", enable_flash_mcp55},	/* LPC */
	{0x10de, 0x0362, "NVIDIA MCP55", enable_flash_mcp55},	/* LPC */
	{0x10de, 0x0363, "NVIDIA MCP55", enable_flash_mcp55},	/* LPC */
	{0x10de, 0x0364, "NVIDIA MCP55", enable_flash_mcp55},	/* LPC */
	{0x10de, 0x0365, "NVIDIA MCP55", enable_flash_mcp55},	/* LPC */
	{0x10de, 0x0366, "NVIDIA MCP55", enable_flash_mcp55},	/* LPC */
	{0x10de, 0x0367, "NVIDIA MCP55", enable_flash_mcp55},	/* Pro */
	{0x1002, 0x4377, "ATI SB400", enable_flash_sb400},
	{0x1166, 0x0205, "Broadcom HT-1000", enable_flash_ht1000},
};

int chipset_flash_enable(void)
{
	struct pci_dev *dev = 0;
	int ret = -2;		/* Nothing! */
	int i;

	/* Now let's try to find the chipset we have... */
	/* TODO: Use ARRAY_SIZE. */
	for (i = 0; i < sizeof(enables) / sizeof(enables[0]); i++) {
		dev = pci_dev_find(enables[i].vendor, enables[i].device);
		if (dev)
			break;
	}

	if (dev) {
		printf("Found chipset \"%s\", enabling flash write... ",
		       enables[i].name);

		ret = enables[i].doit(dev, enables[i].name);
		if (ret)
			printf("FAILED!\n");
		else
			printf("OK.\n");
	}

	return ret;
}
