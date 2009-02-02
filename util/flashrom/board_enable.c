/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2005-2007 coresystems GmbH <stepan@coresystems.de>
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007-2008 Luc Verhaegen <libv@skynet.be>
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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
 * Contains the board specific flash enables.
 */

#include <stdio.h>
#include <pci/pci.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include "flash.h"

/*
 * Helper functions for many Winbond Super I/Os of the W836xx range.
 */
/* Enter extended functions */
void w836xx_ext_enter(uint16_t port)
{
	OUTB(0x87, port);
	OUTB(0x87, port);
}

/* Leave extended functions */
void w836xx_ext_leave(uint16_t port)
{
	OUTB(0xAA, port);
}

/* General functions for reading/writing Winbond Super I/Os. */
unsigned char wbsio_read(uint16_t index, uint8_t reg)
{
	OUTB(reg, index);
	return INB(index + 1);
}

void wbsio_write(uint16_t index, uint8_t reg, uint8_t data)
{
	OUTB(reg, index);
	OUTB(data, index + 1);
}

void wbsio_mask(uint16_t index, uint8_t reg, uint8_t data, uint8_t mask)
{
	uint8_t tmp;

	OUTB(reg, index);
	tmp = INB(index + 1) & ~mask;
	OUTB(tmp | (data & mask), index + 1);
}

/**
 * Winbond W83627HF: Raise GPIO24.
 *
 * Suited for:
 *  - Agami Aruma
 *  - IWILL DK8-HTX
 */
static int w83627hf_gpio24_raise(uint16_t index, const char *name)
{
	w836xx_ext_enter(index);

	/* Is this the W83627HF? */
	if (wbsio_read(index, 0x20) != 0x52) {	/* Super I/O device ID reg. */
		fprintf(stderr, "\nERROR: %s: W83627HF: Wrong ID: 0x%02X.\n",
			name, wbsio_read(index, 0x20));
		w836xx_ext_leave(index);
		return -1;
	}

	/* PIN89S: WDTO/GP24 multiplex -> GPIO24 */
	wbsio_mask(index, 0x2B, 0x10, 0x10);

	/* Select logical device 8: GPIO port 2 */
	wbsio_write(index, 0x07, 0x08);

	wbsio_mask(index, 0x30, 0x01, 0x01);	/* Activate logical device. */
	wbsio_mask(index, 0xF0, 0x00, 0x10);	/* GPIO24 -> output */
	wbsio_mask(index, 0xF2, 0x00, 0x10);	/* Clear GPIO24 inversion */
	wbsio_mask(index, 0xF1, 0x10, 0x10);	/* Raise GPIO24 */

	w836xx_ext_leave(index);

	return 0;
}

static int w83627hf_gpio24_raise_2e(const char *name)
{
	/* TODO: Typo? Shouldn't this be 0x2e? */
	return w83627hf_gpio24_raise(0x2d, name);
}

/**
 * Winbond W83627THF: GPIO 4, bit 4
 *
 * Suited for:
 *  - MSI K8T Neo2-F
 *  - MSI K8N-NEO3
 */
static int w83627thf_gpio4_4_raise(uint16_t index, const char *name)
{
	w836xx_ext_enter(index);

	/* Is this the W83627THF? */
	if (wbsio_read(index, 0x20) != 0x82) {	/* Super I/O device ID reg. */
		fprintf(stderr, "\nERROR: %s: W83627THF: Wrong ID: 0x%02X.\n",
			name, wbsio_read(index, 0x20));
		w836xx_ext_leave(index);
		return -1;
	}

	/* PINxxxxS: GPIO4/bit 4 multiplex -> GPIOXXX */

	wbsio_write(index, 0x07, 0x09);      /* Select LDN 9: GPIO port 4 */
	wbsio_mask(index, 0x30, 0x02, 0x02); /* Activate logical device. */
	wbsio_mask(index, 0xF4, 0x00, 0x10); /* GPIO4 bit 4 -> output */
	wbsio_mask(index, 0xF6, 0x00, 0x10); /* Clear GPIO4 bit 4 inversion */
	wbsio_mask(index, 0xF5, 0x10, 0x10); /* Raise GPIO4 bit 4 */

	w836xx_ext_leave(index);

	return 0;
}

static int w83627thf_gpio4_4_raise_2e(const char *name)
{
	return w83627thf_gpio4_4_raise(0x2e, name);
}

static int w83627thf_gpio4_4_raise_4e(const char *name)
{
	return w83627thf_gpio4_4_raise(0x4e, name);
}

/**
 * Suited for VIAs EPIA M and MII, and maybe other CLE266 based EPIAs.
 *
 * We don't need to do this when using coreboot, GPIO15 is never lowered there.
 */
static int board_via_epia_m(const char *name)
{
	struct pci_dev *dev;
	uint16_t base;
	uint8_t val;

	dev = pci_dev_find(0x1106, 0x3177);	/* VT8235 ISA bridge */
	if (!dev) {
		fprintf(stderr, "\nERROR: VT8235 ISA bridge not found.\n");
		return -1;
	}

	/* GPIO12-15 -> output */
	val = pci_read_byte(dev, 0xE4);
	val |= 0x10;
	pci_write_byte(dev, 0xE4, val);

	/* Get Power Management IO address. */
	base = pci_read_word(dev, 0x88) & 0xFF80;

	/* Enable GPIO15 which is connected to write protect. */
	val = INB(base + 0x4D);
	val |= 0x80;
	OUTB(val, base + 0x4D);

	return 0;
}

/**
 * Suited for:
 *   - ASUS A7V8X-MX SE and A7V400-MX: AMD K7 + VIA KM400A + VT8235
 *   - Tyan Tomcat K7M: AMD Geode NX + VIA KM400 + VT8237.
 */
static int board_asus_a7v8x_mx(const char *name)
{
	struct pci_dev *dev;
	uint8_t val;

	dev = pci_dev_find(0x1106, 0x3177);	/* VT8235 ISA bridge */
	if (!dev)
		dev = pci_dev_find(0x1106, 0x3227);	/* VT8237 ISA bridge */
	if (!dev) {
		fprintf(stderr, "\nERROR: VT823x ISA bridge not found.\n");
		return -1;
	}

	/* This bit is marked reserved actually. */
	val = pci_read_byte(dev, 0x59);
	val &= 0x7F;
	pci_write_byte(dev, 0x59, val);

	/* Raise ROM MEMW# line on Winbond W83697 Super I/O. */
	w836xx_ext_enter(0x2E);

	if (!(wbsio_read(0x2E, 0x24) & 0x02))	/* Flash ROM enabled? */
		wbsio_mask(0x2E, 0x24, 0x08, 0x08);	/* Enable MEMW#. */

	w836xx_ext_leave(0x2E);

	return 0;
}

/**
 * Suited for VIAs EPIA SP.
 */
static int board_via_epia_sp(const char *name)
{
	struct pci_dev *dev;
	uint8_t val;

	dev = pci_dev_find(0x1106, 0x3227);	/* VT8237R ISA bridge */
	if (!dev) {
		fprintf(stderr, "\nERROR: VT8237R ISA bridge not found.\n");
		return -1;
	}

	/* All memory cycles, not just ROM ones, go to LPC */
	val = pci_read_byte(dev, 0x59);
	val &= ~0x80;
	pci_write_byte(dev, 0x59, val);

	return 0;
}

/**
 * Suited for ASUS P5A.
 *
 * This is rather nasty code, but there's no way to do this cleanly.
 * We're basically talking to some unknown device on SMBus, my guess
 * is that it is the Winbond W83781D that lives near the DIP BIOS.
 */
static int board_asus_p5a(const char *name)
{
	uint8_t tmp;
	int i;

#define ASUSP5A_LOOP 5000

	OUTB(0x00, 0xE807);
	OUTB(0xEF, 0xE803);

	OUTB(0xFF, 0xE800);

	for (i = 0; i < ASUSP5A_LOOP; i++) {
		OUTB(0xE1, 0xFF);
		if (INB(0xE800) & 0x04)
			break;
	}

	if (i == ASUSP5A_LOOP) {
		printf("%s: Unable to contact device.\n", name);
		return -1;
	}

	OUTB(0x20, 0xE801);
	OUTB(0x20, 0xE1);

	OUTB(0xFF, 0xE802);

	for (i = 0; i < ASUSP5A_LOOP; i++) {
		tmp = INB(0xE800);
		if (tmp & 0x70)
			break;
	}

	if ((i == ASUSP5A_LOOP) || !(tmp & 0x10)) {
		printf("%s: failed to read device.\n", name);
		return -1;
	}

	tmp = INB(0xE804);
	tmp &= ~0x02;

	OUTB(0x00, 0xE807);
	OUTB(0xEE, 0xE803);

	OUTB(tmp, 0xE804);

	OUTB(0xFF, 0xE800);
	OUTB(0xE1, 0xFF);

	OUTB(0x20, 0xE801);
	OUTB(0x20, 0xE1);

	OUTB(0xFF, 0xE802);

	for (i = 0; i < ASUSP5A_LOOP; i++) {
		tmp = INB(0xE800);
		if (tmp & 0x70)
			break;
	}

	if ((i == ASUSP5A_LOOP) || !(tmp & 0x10)) {
		printf("%s: failed to write to device.\n", name);
		return -1;
	}

	return 0;
}

static int board_ibm_x3455(const char *name)
{
	uint8_t byte;

	/* Set GPIO lines in the Broadcom HT-1000 southbridge. */
	OUTB(0x45, 0xcd6);
	byte = INB(0xcd7);
	OUTB(byte | 0x20, 0xcd7);

	return 0;
}

/**
 * Suited for EPoX EP-BX3, and maybe some other Intel 440BX based boards.
 */
static int board_epox_ep_bx3(const char *name)
{
	uint8_t tmp;

	/* Raise GPIO22. */
	tmp = INB(0x4036);
	OUTB(tmp, 0xEB);

	tmp |= 0x40;

	OUTB(tmp, 0x4036);
	OUTB(tmp, 0xEB);

	return 0;
}

/**
 * Suited for Acorp 6A815EPD.
 */
static int board_acorp_6a815epd(const char *name)
{
	struct pci_dev *dev;
	uint16_t port;
	uint8_t val;

	dev = pci_dev_find(0x8086, 0x2440);	/* Intel ICH2 LPC */
	if (!dev) {
		fprintf(stderr, "\nERROR: ICH2 LPC bridge not found.\n");
		return -1;
	}

	/* Use GPIOBASE register to find where the GPIO is mapped. */
	port = (pci_read_word(dev, 0x58) & 0xFFC0) + 0xE;

	val = INB(port);
	val |= 0x80;		/* Top Block Lock -- pin 8 of PLCC32 */
	val |= 0x40;		/* Lower Blocks Lock -- pin 7 of PLCC32 */
	OUTB(val, port);

	return 0;
}

/**
 * Suited for Artec Group DBE61 and DBE62.
 */
static int board_artecgroup_dbe6x(const char *name)
{
#define DBE6x_MSR_DIVIL_BALL_OPTS	0x51400015
#define DBE6x_PRI_BOOT_LOC_SHIFT	(2)
#define DBE6x_BOOT_OP_LATCHED_SHIFT	(8)
#define DBE6x_SEC_BOOT_LOC_SHIFT	(10)
#define DBE6x_PRI_BOOT_LOC		(3 << DBE6x_PRI_BOOT_LOC_SHIFT)
#define DBE6x_BOOT_OP_LATCHED		(3 << DBE6x_BOOT_OP_LATCHED_SHIFT)
#define DBE6x_SEC_BOOT_LOC		(3 << DBE6x_SEC_BOOT_LOC_SHIFT)
#define DBE6x_BOOT_LOC_FLASH		(2)
#define DBE6x_BOOT_LOC_FWHUB		(3)

	unsigned long msr[2];
	int msr_fd;
	unsigned long boot_loc;

	msr_fd = open("/dev/cpu/0/msr", O_RDWR);
	if (msr_fd == -1) {
		perror("open /dev/cpu/0/msr");
		return -1;
	}

	if (lseek(msr_fd, DBE6x_MSR_DIVIL_BALL_OPTS, SEEK_SET) == -1) {
		perror("lseek");
		close(msr_fd);
		return -1;
	}

	if (read(msr_fd, (void *)msr, 8) != 8) {
		perror("read");
		close(msr_fd);
		return -1;
	}

	if ((msr[0] & (DBE6x_BOOT_OP_LATCHED)) ==
	    (DBE6x_BOOT_LOC_FWHUB << DBE6x_BOOT_OP_LATCHED_SHIFT))
		boot_loc = DBE6x_BOOT_LOC_FWHUB;
	else
		boot_loc = DBE6x_BOOT_LOC_FLASH;

	msr[0] &= ~(DBE6x_PRI_BOOT_LOC | DBE6x_SEC_BOOT_LOC);
	msr[0] |= ((boot_loc << DBE6x_PRI_BOOT_LOC_SHIFT) |
		   (boot_loc << DBE6x_SEC_BOOT_LOC_SHIFT));

	if (lseek(msr_fd, DBE6x_MSR_DIVIL_BALL_OPTS, SEEK_SET) == -1) {
		perror("lseek");
		close(msr_fd);
		return -1;
	}

	if (write(msr_fd, (void *)msr, 8) != 8) {
		perror("write");
		close(msr_fd);
		return -1;
	}

	close(msr_fd);
	return 0;
}

/**
 * Set the specified GPIO on the specified ICHx southbridge to high.
 *
 * @param name The name of this board.
 * @param ich_vendor PCI vendor ID of the specified ICHx southbridge.
 * @param ich_device PCI device ID of the specified ICHx southbridge.
 * @param gpiobase_reg GPIOBASE register offset in the LPC bridge.
 * @param gp_lvl Offset of GP_LVL register in I/O space, relative to GPIOBASE.
 * @param gp_lvl_bitmask GP_LVL bitmask (set GPIO bits to 1, all others to 0).
 * @param gpio_bit The bit (GPIO) which shall be set to high.
 * @return If the write-enable was successful return 0, otherwise return -1.
 */
static int ich_gpio_raise(const char *name, uint16_t ich_vendor,
			  uint16_t ich_device, uint8_t gpiobase_reg,
			  uint8_t gp_lvl, uint32_t gp_lvl_bitmask,
			  unsigned int gpio_bit)
{
	struct pci_dev *dev;
	uint16_t gpiobar;
	uint32_t reg32;

	dev = pci_dev_find(ich_vendor, ich_device);	/* Intel ICHx LPC */
	if (!dev) {
		fprintf(stderr, "\nERROR: ICHx LPC dev %4x:%4x not found.\n",
			ich_vendor, ich_device);
		return -1;
	}

	/* Use GPIOBASE register to find the I/O space for GPIO. */
	gpiobar = pci_read_word(dev, gpiobase_reg) & gp_lvl_bitmask;

	/* Set specified GPIO to high. */
	reg32 = INL(gpiobar + gp_lvl);
	reg32 |= (1 << gpio_bit);
	OUTL(reg32, gpiobar + gp_lvl);

	return 0;
}

/**
 * Suited for ASUS P4B266.
 */
static int ich2_gpio22_raise(const char *name)
{
	return ich_gpio_raise(name, 0x8086, 0x2440, 0x58, 0x0c, 0xffc0, 22);
}

/**
 * Suited for MSI MS-7046.
 */
static int ich6_gpio19_raise(const char *name)
{
	return ich_gpio_raise(name, 0x8086, 0x2640, 0x48, 0x0c, 0xffc0, 19);
}

static int board_kontron_986lcd_m(const char *name)
{
	struct pci_dev *dev;
	uint16_t gpiobar;
	uint32_t val;

#define ICH7_GPIO_LVL2 0x38

	dev = pci_dev_find(0x8086, 0x27b8);	/* Intel ICH7 LPC */
	if (!dev) {
		// This will never happen on this board
		fprintf(stderr, "\nERROR: ICH7 LPC bridge not found.\n");
		return -1;
	}

	/* Use GPIOBASE register to find where the GPIO is mapped. */
	gpiobar = pci_read_word(dev, 0x48) & 0xfffc;

	val = INL(gpiobar + ICH7_GPIO_LVL2);	/* GP_LVL2 */
	printf_debug("\nGPIOBAR=0x%04x GP_LVL: 0x%08x\n", gpiobar, val);

	/* bit 2 (0x04) = 0 #TBL --> bootblock locking = 1
	 * bit 2 (0x04) = 1 #TBL --> bootblock locking = 0
	 * bit 3 (0x08) = 0 #WP --> block locking = 1
	 * bit 3 (0x08) = 1 #WP --> block locking = 0
	 *
	 * To enable full block locking, you would do:
	 *     val &= ~ ((1 << 2) | (1 << 3));
	 */
	val |= (1 << 2) | (1 << 3);

	OUTL(val, gpiobar + ICH7_GPIO_LVL2);

	return 0;
}

/**
 * Suited for:
 *   - BioStar P4M80-M4: Intel P4 + VIA P4M800 + VT8237
 *   - GIGABYTE GA-7VT600: AMD K7 + VIA KT600 + VT8237
 */
static int board_biostar_p4m80_m4(const char *name)
{
	/* enter IT87xx conf mode */
	OUTB(0x87, 0x2e);
	OUTB(0x01, 0x2e);
	OUTB(0x55, 0x2e);
	OUTB(0x55, 0x2e);

	/* select right flash chip */
	wbsio_mask(0x2e, 0x22, 0x80, 0x80);

	/* bit 3: flash chip write enable
	 * bit 7: map flash chip at 1MB-128K (why though? ignoring this.)
	 */
	wbsio_mask(0x2e, 0x24, 0x04, 0x04);

	/* exit IT87xx conf mode */
	wbsio_write(0x2, 0x2e, 0x2);

	return 0;
}

/**
 * Winbond W83697HF Super I/O + VIA VT8235 southbridge
 *
 * Suited for:
 *   - MSI KT4V and KT4V-L: AMD K7 + VIA KT400 + VT8235
 *   - MSI KT3 Ultra2: AMD K7 + VIA KT333 + VT8235
 */
static int board_msi_kt4v(const char *name)
{
	struct pci_dev *dev;
	uint8_t val;
	uint32_t val2;
	uint16_t port;

	dev = pci_dev_find(0x1106, 0x3177);	/* VT8235 ISA bridge */
	if (!dev) {
		fprintf(stderr, "\nERROR: VT823x ISA bridge not found.\n");
		return -1;
	}

	val = pci_read_byte(dev, 0x59);
	val &= 0x0c;
	pci_write_byte(dev, 0x59, val);

	/* We need the I/O Base Address for this board's flash enable. */
	port = pci_read_word(dev, 0x88) & 0xff80;

	/* Starting at 'I/O Base + 0x4c' is the GPO Port Output Value.
	 * We must assert GPO12 for our enable, which is in 0x4d.
	 */
	val2 = INB(port + 0x4d);
	val2 |= 0x10;
	OUTB(val2, port + 0x4d);

	/* Raise ROM MEMW# line on Winbond W83697 Super I/O. */
	w836xx_ext_enter(0x2e);
	if (!(wbsio_read(0x2e, 0x24) & 0x02)) {	/* Flash ROM enabled? */
		/* Enable MEMW# and set ROM size select to max. (4M). */
		wbsio_mask(0x2e, 0x24, 0x28, 0x28);
	}
	w836xx_ext_leave(0x2e);

	return 0;
}

/**
 * We use 2 sets of IDs here, you're free to choose which is which. This
 * is to provide a very high degree of certainty when matching a board on
 * the basis of subsystem/card IDs. As not every vendor handles
 * subsystem/card IDs in a sane manner.
 *
 * Keep the second set NULLed if it should be ignored.
 *
 * Keep the subsystem IDs NULLed if they don't identify the board fully.
 */
struct board_pciid_enable {
	/* Any device, but make it sensible, like the ISA bridge. */
	uint16_t first_vendor;
	uint16_t first_device;
	uint16_t first_card_vendor;
	uint16_t first_card_device;

	/* Any device, but make it sensible, like 
	 * the host bridge. May be NULL.
	 */
	uint16_t second_vendor;
	uint16_t second_device;
	uint16_t second_card_vendor;
	uint16_t second_card_device;

	/* The vendor / part name from the coreboot table. */
	const char *lb_vendor;
	const char *lb_part;

	const char *name;
	int (*enable) (const char *name);
};

struct board_pciid_enable board_pciid_enables[] = {
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x0571,
		.first_card_vendor	= 0x1462,
		.first_card_device	= 0x7120,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "msi",
		.lb_part		= "kt4v",
		.name			= "MSI KT4V",
		.enable			= board_msi_kt4v,
	},
	{
		.first_vendor		= 0x8086,
		.first_device		= 0x1a30,
		.first_card_vendor	= 0x1043,
		.first_card_device	= 0x8070,
		.second_vendor		= 0x8086,
		.second_device		= 0x244b,
		.second_card_vendor	= 0x1043,
		.second_card_device	= 0x8028,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "ASUS P4B266",
		.enable			= ich2_gpio22_raise,
	},
	{
		.first_vendor		= 0x10de,
		.first_device		= 0x0360,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "gigabyte",
		.lb_part		= "m57sli",
		.name			= "GIGABYTE GA-M57SLI-S4",
		.enable			= it87xx_probe_spi_flash,
	},
	{
		.first_vendor		= 0x10de,
		.first_device		= 0x03e0,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "gigabyte",
		.lb_part		= "m61p",
		.name			= "GIGABYTE GA-M61P-S3",
		.enable			= it87xx_probe_spi_flash,
	},
	{
		.first_vendor		= 0x1002,
		.first_device		= 0x4398,
		.first_card_vendor	= 0x1458,
		.first_card_device	= 0x5004,
		.second_vendor		= 0x1002,
		.second_device		= 0x4385,
		.second_card_vendor	= 0x1458,
		.second_card_device	= 0x4385,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "GIGABYTE GA-MA78G-DS3H",
		.enable			= it87xx_probe_spi_flash,
	},
	{
		.first_vendor		= 0x1039,
		.first_device		= 0x0761,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "gigabyte",
		.lb_part		= "2761gxdk",
		.name			= "GIGABYTE GA-2761GXDK",
		.enable			= it87xx_probe_spi_flash,
	},
	{
		.first_vendor		= 0x1022,
		.first_device		= 0x7468,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "iwill",
		.lb_part		= "dk8_htx",
		.name			= "IWILL DK8-HTX",
		.enable			= w83627hf_gpio24_raise_2e,
	},
	{
		.first_vendor		= 0x10de,
		.first_device		= 0x005e,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "msi",
		.lb_part		= "k8n-neo3",
		.name			= "MSI K8N Neo3",
		.enable			= w83627thf_gpio4_4_raise_4e,
	},
	{
		.first_vendor		= 0x1022,
		.first_device		= 0x746B,
		.first_card_vendor	= 0x1022,
		.first_card_device	= 0x36C0,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "AGAMI",
		.lb_part		= "ARUMA",
		.name			= "agami Aruma",
		.enable			= w83627hf_gpio24_raise_2e,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x3177,
		.first_card_vendor	= 0x1106,
		.first_card_device	= 0xAA01,
		.second_vendor		= 0x1106,
		.second_device		= 0x3123,
		.second_card_vendor	= 0x1106,
		.second_card_device	= 0xAA01,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "VIA EPIA M/MII/...",
		.enable			= board_via_epia_m,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x3177,
		.first_card_vendor	= 0x1043,
		.first_card_device	= 0x80A1,
		.second_vendor		= 0x1106,
		.second_device		= 0x3205,
		.second_card_vendor	= 0x1043,
		.second_card_device	= 0x8118,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "ASUS A7V8-MX SE",
		.enable			= board_asus_a7v8x_mx,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x3227,
		.first_card_vendor	= 0x1106,
		.first_card_device	= 0xAA01,
		.second_vendor		= 0x1106,
		.second_device		= 0x0259,
		.second_card_vendor	= 0x1106,
		.second_card_device	= 0xAA01,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "VIA EPIA SP",
		.enable			= board_via_epia_sp,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x0314,
		.first_card_vendor	= 0x1106,
		.first_card_device	= 0xaa08,
		.second_vendor		= 0x1106,
		.second_device		= 0x3227,
		.second_card_vendor	= 0x1106,
		.second_card_device	= 0xAA08,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "VIA EPIA-CN",
		.enable			= board_via_epia_sp,
	},
	{
		.first_vendor		= 0x8086,
		.first_device		= 0x1076,
		.first_card_vendor	= 0x8086,
		.first_card_device	= 0x1176,
		.second_vendor		= 0x1106,
		.second_device		= 0x3059,
		.second_card_vendor	= 0x10f1,
		.second_card_device	= 0x2498,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "Tyan Tomcat K7M",
		.enable			= board_asus_a7v8x_mx,
	},
	{
		.first_vendor		= 0x10B9,
		.first_device		= 0x1541,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x10B9,
		.second_device		= 0x1533,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "asus",
		.lb_part		= "p5a",
		.name			= "ASUS P5A",
		.enable			= board_asus_p5a,
	},
	{
		.first_vendor		= 0x1166,
		.first_device		= 0x0205,
		.first_card_vendor	= 0x1014,
		.first_card_device	= 0x0347,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "ibm",
		.lb_part		= "x3455",
		.name			= "IBM x3455",
		.enable			= board_ibm_x3455,
	},
	{
		.first_vendor		= 0x8086,
		.first_device		= 0x7110,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x8086,
		.second_device		= 0x7190,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "epox",
		.lb_part		= "ep-bx3",
		.name			= "EPoX EP-BX3",
		.enable			= board_epox_ep_bx3,
	},
	{
		.first_vendor		= 0x8086,
		.first_device		= 0x1130,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x105a,
		.second_device		= 0x0d30,
		.second_card_vendor	= 0x105a,
		.second_card_device	= 0x4d33,
		.lb_vendor		= "acorp",
		.lb_part		= "6a815epd",
		.name			= "Acorp 6A815EPD",
		.enable			= board_acorp_6a815epd,
	},
	{
		.first_vendor		= 0x1022,
		.first_device		= 0x2090,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x1022,
		.second_device		= 0x2080,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "artecgroup",
		.lb_part		= "dbe61",
		.name			= "Artec Group DBE61",
		.enable			= board_artecgroup_dbe6x,
	},
	{
		.first_vendor		= 0x1022,
		.first_device		= 0x2090,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x1022,
		.second_device		= 0x2080,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "artecgroup",
		.lb_part		= "dbe62",
		.name			= "Artec Group DBE62",
		.enable			= board_artecgroup_dbe6x,
	},
	/* Note: There are >= 2 version of the Kontron 986LCD-M/mITX! */
	{
		.first_vendor		= 0x8086,
		.first_device		= 0x27b8,
		.first_card_vendor	= 0x0000,
		.first_card_device	= 0x0000,
		.second_vendor		= 0x0000,
		.second_device		= 0x0000,
		.second_card_vendor	= 0x0000,
		.second_card_device	= 0x0000,
		.lb_vendor		= "kontron",
		.lb_part		= "986lcd-m",
		.name			= "Kontron 986LCD-M",
		.enable			= board_kontron_986lcd_m,
	},
	{
		.first_vendor		= 0x10ec,
		.first_device		= 0x8168,
		.first_card_vendor	= 0x10ec,
		.first_card_device	= 0x8168,
		.second_vendor		= 0x104c,
		.second_device		= 0x8023,
		.second_card_vendor	= 0x104c,
		.second_card_device	= 0x8019,
		.lb_vendor		= "kontron",
		.lb_part		= "986lcd-m",
		.name			= "Kontron 986LCD-M",
		.enable			= board_kontron_986lcd_m,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x3149,
		.first_card_vendor	= 0x1565,
		.first_card_device	= 0x3206,
		.second_vendor		= 0x1106,
		.second_device		= 0x3344,
		.second_card_vendor	= 0x1565,
		.second_card_device	= 0x1202,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "BioStar P4M80-M4",
		.enable			= board_biostar_p4m80_m4,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x3227,
		.first_card_vendor	= 0x1458,
		.first_card_device	= 0x5001,
		.second_vendor		= 0x10ec,
		.second_device		= 0x8139,
		.second_card_vendor	= 0x1458,
		.second_card_device	= 0xe000,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "GIGABYTE GA-7VT600",
		.enable			= board_biostar_p4m80_m4,
	},
	{
		.first_vendor		= 0x1106,
		.first_device		= 0x3149,
		.first_card_vendor	= 0x1462,
		.first_card_device	= 0x7094,
		.second_vendor		= 0x10ec,
		.second_device		= 0x8167,
		.second_card_vendor	= 0x1462,
		.second_card_device	= 0x094c,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "MSI K8T Neo2",
		.enable			= w83627thf_gpio4_4_raise_2e,
	},
	{
		.first_vendor		= 0x1039,
		.first_device		= 0x5513,
		.first_card_vendor	= 0x8086,
		.first_card_device	= 0xd61f,
		.second_vendor		= 0x1039,
		.second_device		= 0x6330,
		.second_card_vendor	= 0x8086,
		.second_card_device	= 0xd61f,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "Intel Desktop Board D201GLY",
		.enable			= wbsio_check_for_spi,
	},
	{
		.first_vendor		= 0x8086,
		.first_device		= 0x2658,
		.first_card_vendor	= 0x1462,
		.first_card_device	= 0x7046,
		.second_vendor		= 0x1106,
		.second_device		= 0x3044,
		.second_card_vendor	= 0x1462,
		.second_card_device	= 0x046d,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
		.name			= "MSI MS-7046",
		.enable			= ich6_gpio19_raise,
	},
	{
		.first_vendor		= 0,
		.first_device		= 0,
		.first_card_vendor	= 0,
		.first_card_device	= 0,
		.second_vendor		= 0,
		.second_device		= 0,
		.second_card_vendor	= 0,
		.second_card_device	= 0,
		.lb_vendor		= NULL,
		.lb_part		= NULL,
	}	/* Keep this */
};

void print_supported_boards(void)
{
	int i;

	printf("\nSupported mainboards (this list is not exhaustive!):\n\n");

	for (i = 0; board_pciid_enables[i].name != NULL; i++) {
		if (board_pciid_enables[i].lb_vendor != NULL) {
			printf("%s (-m %s:%s)\n", board_pciid_enables[i].name,
			       board_pciid_enables[i].lb_vendor,
			       board_pciid_enables[i].lb_part);
		} else {
			printf("%s (autodetected)\n",
			       board_pciid_enables[i].name);
		}
	}

	printf("\nSee also: http://coreboot.org/Flashrom\n");
}

/**
 * Match boards on coreboot table gathered vendor and part name.
 * Require main PCI IDs to match too as extra safety.
 */
static struct board_pciid_enable *board_match_coreboot_name(const char *vendor,
							    const char *part)
{
	struct board_pciid_enable *board = board_pciid_enables;
	struct board_pciid_enable *partmatch = NULL;

	for (; board->name; board++) {
		if (vendor && (!board->lb_vendor
			       || strcasecmp(board->lb_vendor, vendor)))
			continue;

		if (!board->lb_part || strcasecmp(board->lb_part, part))
			continue;

		if (!pci_dev_find(board->first_vendor, board->first_device))
			continue;

		if (board->second_vendor &&
		    !pci_dev_find(board->second_vendor, board->second_device))
			continue;

		if (vendor)
			return board;

		if (partmatch) {
			/* a second entry has a matching part name */
			printf("AMBIGUOUS BOARD NAME: %s\n", part);
			printf("At least vendors '%s' and '%s' match.\n",
			       partmatch->lb_vendor, board->lb_vendor);
			printf("Please use the full -m vendor:part syntax.\n");
			return NULL;
		}
		partmatch = board;
	}

	if (partmatch)
		return partmatch;

	printf("\nUnknown vendor:board from coreboot table or -m option: %s:%s\n\n", vendor, part);
	return NULL;
}

/**
 * Match boards on PCI IDs and subsystem IDs.
 * Second set of IDs can be main only or missing completely.
 */
static struct board_pciid_enable *board_match_pci_card_ids(void)
{
	struct board_pciid_enable *board = board_pciid_enables;

	for (; board->name; board++) {
		if (!board->first_card_vendor || !board->first_card_device)
			continue;

		if (!pci_card_find(board->first_vendor, board->first_device,
				   board->first_card_vendor,
				   board->first_card_device))
			continue;

		if (board->second_vendor) {
			if (board->second_card_vendor) {
				if (!pci_card_find(board->second_vendor,
						   board->second_device,
						   board->second_card_vendor,
						   board->second_card_device))
					continue;
			} else {
				if (!pci_dev_find(board->second_vendor,
						  board->second_device))
					continue;
			}
		}

		return board;
	}

	return NULL;
}

int board_flash_enable(const char *vendor, const char *part)
{
	struct board_pciid_enable *board = NULL;
	int ret = 0;

	if (part)
		board = board_match_coreboot_name(vendor, part);

	if (!board)
		board = board_match_pci_card_ids();

	if (board) {
		printf("Found board \"%s\", enabling flash write... ",
		       board->name);

		ret = board->enable(board->name);
		if (ret)
			printf("FAILED!\n");
		else
			printf("OK.\n");
	}

	return ret;
}
