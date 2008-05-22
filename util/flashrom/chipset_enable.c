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
#include <sys/mman.h>
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
	OUTB(0x87, 0x2e);
	OUTB(0x01, 0x2e);
	OUTB(0x55, 0x2e);
	OUTB(0x55, 0x2e);

	if (INB(0x2f) != 0x87) {
		/* If that failed, try config port 0x4e. */
		OUTB(0x87, 0x4e);
		OUTB(0x01, 0x4e);
		OUTB(0x55, 0x4e);
		OUTB(0xaa, 0x4e);
		if (INB(0x4f) != 0x87) {
			printf("Can not access SiS 950\n");
			return -1;
		}
		OUTB(0x24, 0x4e);
		b = INB(0x4f) | 0xfc;
		OUTB(0x24, 0x4e);
		OUTB(b, 0x4f);
		OUTB(0x02, 0x4e);
		OUTB(0x02, 0x4f);
	}

	OUTB(0x24, 0x2e);
	printf("2f is %#x\n", INB(0x2f));
	b = INB(0x2f) | 0xfc;
	OUTB(0x24, 0x2e);
	OUTB(b, 0x2f);

	OUTB(0x02, 0x2e);
	OUTB(0x02, 0x2f);

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
 * See ie. page 375 of "Intel I/O Controller Hub 7 (ICH7) Family Datasheet"
 * http://download.intel.com/design/chipsets/datashts/30701303.pdf
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

	printf_debug("BIOS Lock Enable: %sabled, ",
		     (old & (1 << 1)) ? "en" : "dis");
	printf_debug("BIOS Write Enable: %sabled, ",
		     (old & (1 << 0)) ? "en" : "dis");
	printf_debug("BIOS_CNTL is 0x%x\n", old);

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

void *ich_spibar = NULL;

static int enable_flash_ich_dc_spi(struct pci_dev *dev, const char *name, unsigned long spibar)
{
	uint8_t old, new, bbs;
	uint32_t tmp, gcs;
	void *rcrb;

	/* Read the Root Complex Base Address Register (RCBA) */
	tmp = pci_read_long(dev, 0xf0);
	/* Calculate the Root Complex Register Block address */
	tmp &= 0xffffc000;
	printf_debug("Root Complex Register Block address = 0x%x\n", tmp);
	rcrb = mmap(0, 0x4000, PROT_READ | PROT_WRITE, MAP_SHARED, fd_mem, (off_t)tmp);
	if (rcrb == MAP_FAILED) {
		perror("Can't mmap memory using " MEM_DEV);
		exit(1);
	}
	printf_debug("GCS address = 0x%x\n", tmp + 0x3410);
	gcs = *(volatile uint32_t *)(rcrb + 0x3410);
	printf_debug("GCS = 0x%x: ", gcs);
	printf_debug("BIOS Interface Lock-Down: %sabled, ",
		     (gcs & 0x1) ? "en" : "dis");
	bbs = (gcs >> 10) & 0x3;
	printf_debug("BOOT BIOS Straps: 0x%x (%s)\n",	bbs,
		     (bbs == 0x3) ? "LPC" : ((bbs == 0x2) ? "PCI" : "SPI"));

	/* SPIBAR is at RCRB+0x3020 for ICH[78] and RCRB+0x3800 for ICH9. */
	printf_debug("SPIBAR = 0x%lx\n", tmp + spibar);
	/* TODO: Dump the SPI config regs */
	ich_spibar =  rcrb + spibar;

	old = pci_read_byte(dev, 0xdc);
	printf_debug("SPI Read Configuration: ");
	new = (old >> 2) & 0x3;
	switch (new) {
	case 0:
	case 1:
	case 2:
		printf_debug("prefetching %sabled, caching %sabled, ",
			(new & 0x2) ? "en" : "dis", (new & 0x1) ? "dis" : "en");
		break;
	default:
		printf_debug("invalid prefetching/caching settings, ");
		break;
	}
	return enable_flash_ich_dc(dev, name);
}

static int enable_flash_ich7(struct pci_dev *dev, const char *name)
{
	return enable_flash_ich_dc_spi(dev, name, 0x3020);
}

int ich9_detected = 0;

static int enable_flash_ich8(struct pci_dev *dev, const char *name)
{
	ich9_detected = 1;
	return enable_flash_ich_dc_spi(dev, name, 0x3020);
}

static int enable_flash_ich9(struct pci_dev *dev, const char *name)
{
	ich9_detected = 1;
	return enable_flash_ich_dc_spi(dev, name, 0x3800);
}

static int enable_flash_vt823x(struct pci_dev *dev, const char *name)
{
	uint8_t val;

	/* enable ROM decode range (1MB) FFC00000 - FFFFFFFF*/
	pci_write_byte(dev, 0x41, 0x7f);

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

/**
 * Geode systems write protect the BIOS via RCONFs (cache settings similar
 * to MTRRs). To unlock, change MSR 0x1808 top byte to 0x22. Reading and
 * writing to MSRs, however requires instructions rdmsr/wrmsr, which are
 * ring0 privileged instructions so only the kernel can do the read/write.
 * This function, therefore, requires that the msr kernel module be loaded
 * to access these instructions from user space using device /dev/cpu/0/msr.
 *
 * This hard-coded location could have potential problems on SMP machines
 * since it assumes cpu0, but it is safe on the Geode which is not SMP.
 *
 * Geode systems also write protect the NOR flash chip itself via MSR_NORF_CTL.
 * To enable write to NOR Boot flash for the benefit of systems that have such
 * a setup, raise MSR 0x51400018 WE_CS3 (write enable Boot Flash Chip Select).
 *
 * This is probably not portable beyond Linux.
 */
static int enable_flash_cs5536(struct pci_dev *dev, const char *name)
{
	#define MSR_RCONF_DEFAULT	0x1808
	#define MSR_NORF_CTL		0x51400018

	int fd_msr;
	unsigned char buf[8];

	fd_msr = open("/dev/cpu/0/msr", O_RDWR);
	if (!fd_msr) {
		perror("open msr");
		return -1;
	}

	if (lseek64(fd_msr, (off64_t) MSR_RCONF_DEFAULT, SEEK_SET) == -1) {
		perror("lseek64");
		printf("Cannot operate on MSR. Did you run 'modprobe msr'?\n");
		close(fd_msr);
		return -1;
	}

	if (read(fd_msr, buf, 8) != 8) {
		perror("read msr");
		close(fd_msr);
		return -1;
	}

	if (buf[7] != 0x22) {
		buf[7] &= 0xfb;
		if (lseek64(fd_msr, (off64_t) MSR_RCONF_DEFAULT, SEEK_SET) == -1) {
			perror("lseek64");
			close(fd_msr);
			return -1;
		}

		if (write(fd_msr, buf, 8) < 0) {
			perror("msr write");
			close(fd_msr);
			return -1;
		}
	}

	if (lseek64(fd_msr, (off64_t) MSR_NORF_CTL, SEEK_SET) == -1) {
		perror("lseek64");
		close(fd_msr);
		return -1;
	}

	if (read(fd_msr, buf, 8) != 8) {
		perror("read msr");
		close(fd_msr);
		return -1;
	}

	/* Raise WE_CS3 bit. */
	buf[0] |= 0x08;

	if (lseek64(fd_msr, (off64_t) MSR_NORF_CTL, SEEK_SET) == -1) {
		perror("lseek64");
		close(fd_msr);
		return -1;
	}
	if (write(fd_msr, buf, 8) < 0) {
		perror("msr write");
		close(fd_msr);
		return -1;
	}

	close(fd_msr);

	#undef MSR_RCONF_DEFAULT
	#undef MSR_NORF_CTL
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
	tmp = INB(0xc6f);
	OUTB(tmp, 0xeb);
	OUTB(tmp, 0xeb);
	tmp |= 0x40;
	OUTB(tmp, 0xc6f);
	OUTB(tmp, 0xeb);
	OUTB(tmp, 0xeb);

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
	{0x1039, 0x0630, "SiS630",		enable_flash_sis630},
	{0x8086, 0x7110, "Intel PIIX4/4E/4M",	enable_flash_piix4},
	{0x8086, 0x7198, "Intel 440MX",		enable_flash_piix4},
	{0x8086, 0x2410, "Intel ICH",		enable_flash_ich_4e},
	{0x8086, 0x2420, "Intel ICH0",		enable_flash_ich_4e},
	{0x8086, 0x2440, "Intel ICH2",		enable_flash_ich_4e},
	{0x8086, 0x244c, "Intel ICH2-M",	enable_flash_ich_4e},
	{0x8086, 0x2480, "Intel ICH3-S",	enable_flash_ich_4e},
	{0x8086, 0x248c, "Intel ICH3-M",	enable_flash_ich_4e},
	{0x8086, 0x24c0, "Intel ICH4/ICH4-L",	enable_flash_ich_4e},
	{0x8086, 0x24cc, "Intel ICH4-M",	enable_flash_ich_4e},
	{0x8086, 0x24d0, "Intel ICH5/ICH5R",	enable_flash_ich_4e},
	{0x8086, 0x25a1, "Intel 6300ESB",	enable_flash_ich_4e},
	{0x8086, 0x2640, "Intel ICH6/ICH6R",	enable_flash_ich_dc},
	{0x8086, 0x2641, "Intel ICH6-M",	enable_flash_ich_dc},
	{0x8086, 0x27b0, "Intel ICH7DH",	enable_flash_ich7},
	{0x8086, 0x27b8, "Intel ICH7/ICH7R",	enable_flash_ich7},
	{0x8086, 0x27b9, "Intel ICH7M",		enable_flash_ich7},
	{0x8086, 0x27bd, "Intel ICH7MDH",	enable_flash_ich7},
	{0x8086, 0x2810, "Intel ICH8/ICH8R",	enable_flash_ich8},
	{0x8086, 0x2811, "Intel ICH8M-E",	enable_flash_ich8},
	{0x8086, 0x2812, "Intel ICH8DH",	enable_flash_ich8},
	{0x8086, 0x2814, "Intel ICH8DO",	enable_flash_ich8},
	{0x8086, 0x2815, "Intel ICH8M",		enable_flash_ich8},
	{0x8086, 0x2912, "Intel ICH9DH",	enable_flash_ich9},
	{0x8086, 0x2914, "Intel ICH9DO",	enable_flash_ich9},
	{0x8086, 0x2916, "Intel ICH9R",		enable_flash_ich9},
	{0x8086, 0x2917, "Intel ICH9M-E",	enable_flash_ich9},
	{0x8086, 0x2918, "Intel ICH9",		enable_flash_ich9},
	{0x8086, 0x2919, "Intel ICH9M",		enable_flash_ich9},
	{0x1106, 0x8231, "VIA VT8231",		enable_flash_vt823x},
	{0x1106, 0x3177, "VIA VT8235",		enable_flash_vt823x},
	{0x1106, 0x3227, "VIA VT8237",		enable_flash_vt823x},
	{0x1106, 0x8324, "VIA CX700",		enable_flash_vt823x},
	{0x1106, 0x0686, "VIA VT82C686",	enable_flash_amd8111},
	{0x1078, 0x0100, "AMD CS5530(A)",	enable_flash_cs5530},
	{0x100b, 0x0510, "AMD SC1100",		enable_flash_sc1100},
	{0x1039, 0x0008, "SiS5595",		enable_flash_sis5595},
	{0x1022, 0x2080, "AMD CS5536",		enable_flash_cs5536},
	{0x1022, 0x7468, "AMD8111",		enable_flash_amd8111},
	{0x10B9, 0x1533, "ALi M1533",		enable_flash_ali_m1533},
	{0x10de, 0x0050, "NVIDIA CK804",	enable_flash_ck804}, /* LPC */
	{0x10de, 0x0051, "NVIDIA CK804",	enable_flash_ck804}, /* Pro */
	/* Slave, should not be here, to fix known bug for A01. */
	{0x10de, 0x00d3, "NVIDIA CK804",	enable_flash_ck804},
	{0x10de, 0x0260, "NVIDIA MCP51",	enable_flash_ck804},
	{0x10de, 0x0261, "NVIDIA MCP51",	enable_flash_ck804},
	{0x10de, 0x0262, "NVIDIA MCP51",	enable_flash_ck804},
	{0x10de, 0x0263, "NVIDIA MCP51",	enable_flash_ck804},
	{0x10de, 0x0360, "NVIDIA MCP55",	enable_flash_mcp55}, /* M57SLI*/
	{0x10de, 0x0361, "NVIDIA MCP55",	enable_flash_mcp55}, /* LPC */
	{0x10de, 0x0362, "NVIDIA MCP55",	enable_flash_mcp55}, /* LPC */
	{0x10de, 0x0363, "NVIDIA MCP55",	enable_flash_mcp55}, /* LPC */
	{0x10de, 0x0364, "NVIDIA MCP55",	enable_flash_mcp55}, /* LPC */
	{0x10de, 0x0365, "NVIDIA MCP55",	enable_flash_mcp55}, /* LPC */
	{0x10de, 0x0366, "NVIDIA MCP55",	enable_flash_mcp55}, /* LPC */
	{0x10de, 0x0367, "NVIDIA MCP55",	enable_flash_mcp55}, /* Pro */
	{0x1002, 0x4377, "ATI SB400",		enable_flash_sb400},
	{0x1166, 0x0205, "Broadcom HT-1000",	enable_flash_ht1000},
};

void print_supported_chipsets(void)
{
	int i;

	printf("\nSupported chipsets:\n\n");

	for (i = 0; i < ARRAY_SIZE(enables); i++)
		printf("%s (%04x:%04x)\n", enables[i].name,
		       enables[i].vendor, enables[i].device);
}

int chipset_flash_enable(void)
{
	struct pci_dev *dev = 0;
	int ret = -2;		/* Nothing! */
	int i;

	/* Now let's try to find the chipset we have... */
	for (i = 0; i < ARRAY_SIZE(enables); i++) {
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
