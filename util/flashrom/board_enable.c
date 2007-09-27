/*
 * This file is part of the flashrom project.
 *
 * Copyright (C) 2005-2007 coresystems GmbH <stepan@coresystems.de>
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Luc Verhaegen <libv@skynet.be>
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
#include "flash.h"

/* Generic Super I/O helper functions */
uint8_t regval(uint16_t port, uint8_t reg)
{
	outb(reg, port);
	return inb(port + 1);
}

void regwrite(uint16_t port, uint8_t reg, uint8_t val)
{
	outb(reg, port);
	outb(val, port + 1);
}

/* Helper functions for most recent ITE IT87xx Super I/O chips */
#define CHIP_ID_BYTE1_REG	0x20
#define CHIP_ID_BYTE2_REG	0x21
static void enter_conf_mode_ite(uint16_t port)
{
	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	if (port == 0x2e)
		outb(0x55, port);
	else
		outb(0xaa, port);
}

static void exit_conf_mode_ite(uint16_t port)
{
	regwrite(port, 0x02, 0x02);
}

static uint16_t find_ite_serial_flash_port(uint16_t port)
{
	uint8_t tmp = 0;
	uint16_t id, flashport = 0;

	enter_conf_mode_ite(port);

	id = regval(port, CHIP_ID_BYTE1_REG) << 8;
	id |= regval(port, CHIP_ID_BYTE2_REG);

	/* TODO: Handle more IT87xx if they support flash translation */
	if (id == 0x8716) {
		/* NOLDN, reg 0x24, mask out lowest bit (suspend) */
		tmp = regval(port, 0x24) & 0xFE;
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0xFFFE0000, 0xFFFFFFFF, (tmp & 1 << 1) ? "en" : "dis");
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0x000E0000, 0x000FFFFF, (tmp & 1 << 1) ? "en" : "dis");
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0xFFEE0000, 0xFFEFFFFF, (tmp & 1 << 2) ? "en" : "dis");
		printf("Serial flash segment 0x%08x-0x%08x %sabled\n",
			0xFFF80000, 0xFFFEFFFF, (tmp & 1 << 3) ? "en" : "dis");
		printf("LPC write to serial flash %sabled\n",
			(tmp & 1 << 4) ? "en" : "dis");
		printf("serial flash pin %i\n",	(tmp & 1 << 5) ? 87 : 29);
		/* LDN 0x7, reg 0x64/0x65 */
		regwrite(port, 0x07, 0x7);
		flashport = regval(port, 0x64) << 8;
		flashport |= regval(port, 0x65);
	}
	exit_conf_mode_ite(port);
	return flashport;
}

static void it8716_serial_rdid(uint16_t port)
{
	uint8_t busy, data0, data1, data2;
	do {
		busy = inb(port) & 0x80;
	} while (busy);
	/* RDID */
	outb(0x9f, port + 1);
	/* Start IO, 33MHz, 3 input bytes, 0 output bytes*/
	outb((0x5<<4)|(0x3<<2)|(0x0), port);
	do {
		busy = inb(port) & 0x80;
	} while (busy);
	data0 = inb(port + 5);
	data1 = inb(port + 6);
	data2 = inb(port + 7);
	printf("RDID returned %02x %02x %02x\n", data0, data1, data2);
	return;
}

static int it87xx_probe_serial_flash(const char *name)
{
	uint16_t flashport;
	flashport = find_ite_serial_flash_port(0x2e);
	if (flashport)
		it8716_serial_rdid(flashport);
	flashport = find_ite_serial_flash_port(0x4e);
	if (flashport)
		it8716_serial_rdid(flashport);
	return 0;
}

/*
 * Helper functions for many Winbond Super I/Os of the W836xx range.
 */
#define W836_INDEX 0x2E
#define W836_DATA  0x2F

/* Enter extended functions */
static void w836xx_ext_enter(void)
{
	outb(0x87, W836_INDEX);
	outb(0x87, W836_INDEX);
}

/* Leave extended functions */
static void w836xx_ext_leave(void)
{
	outb(0xAA, W836_INDEX);
}

/* General functions for reading/writing Winbond Super I/Os. */
static unsigned char wbsio_read(unsigned char index)
{
	outb(index, W836_INDEX);
	return inb(W836_DATA);
}

static void wbsio_write(unsigned char index, unsigned char data)
{
	outb(index, W836_INDEX);
	outb(data, W836_DATA);
}

static void wbsio_mask(unsigned char index, unsigned char data,
		       unsigned char mask)
{
	unsigned char tmp;

	outb(index, W836_INDEX);
	tmp = inb(W836_DATA) & ~mask;
	outb(tmp | (data & mask), W836_DATA);
}

/**
 * Winbond W83627HF: Raise GPIO24.
 *
 * Suited for:
 *  - Agami Aruma
 *  - IWILL DK8-HTX
 */
static int w83627hf_gpio24_raise(const char *name)
{
	w836xx_ext_enter();

	/* Is this the w83627hf? */
	if (wbsio_read(0x20) != 0x52) {	/* SIO device ID register */
		fprintf(stderr, "\nERROR: %s: W83627HF: Wrong ID: 0x%02X.\n",
			name, wbsio_read(0x20));
		w836xx_ext_leave();
		return -1;
	}

	/* PIN89S: WDTO/GP24 multiplex -> GPIO24 */
	wbsio_mask(0x2B, 0x10, 0x10);

	wbsio_write(0x07, 0x08);	/* Select logical device 8: GPIO port 2 */

	wbsio_mask(0x30, 0x01, 0x01);	/* Activate logical device. */

	wbsio_mask(0xF0, 0x00, 0x10);	/* GPIO24 -> output */

	wbsio_mask(0xF2, 0x00, 0x10);	/* Clear GPIO24 inversion */

	wbsio_mask(0xF1, 0x10, 0x10);	/* Raise GPIO24 */

	w836xx_ext_leave();

	return 0;
}

/**
 * Suited for VIAs EPIA M and MII, and maybe other CLE266 based EPIAs.
 *
 * We don't need to do this when using LinuxBIOS, GPIO15 is never lowered there.
 */
static int board_via_epia_m(const char *name)
{
	struct pci_dev *dev;
	unsigned int base;
	uint8_t val;

	dev = pci_dev_find(0x1106, 0x3177);	/* VT8235 ISA bridge */
	if (!dev) {
		fprintf(stderr, "\nERROR: VT8235 ISA Bridge not found.\n");
		return -1;
	}

	/* GPIO12-15 -> output */
	val = pci_read_byte(dev, 0xE4);
	val |= 0x10;
	pci_write_byte(dev, 0xE4, val);

	/* Get Power Management IO address. */
	base = pci_read_word(dev, 0x88) & 0xFF80;

	/* enable GPIO15 which is connected to write protect. */
	val = inb(base + 0x4D);
	val |= 0x80;
	outb(val, base + 0x4D);

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

	/* This bit is marked reserved actually */
	val = pci_read_byte(dev, 0x59);
	val &= 0x7F;
	pci_write_byte(dev, 0x59, val);

	/* Raise ROM MEMW# line on Winbond w83697 SuperIO */
	w836xx_ext_enter();

	if (!(wbsio_read(0x24) & 0x02))	/* flash rom enabled? */
		wbsio_mask(0x24, 0x08, 0x08);	/* enable MEMW# */

	w836xx_ext_leave();

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

	outb(0x00, 0xE807);
	outb(0xEF, 0xE803);

	outb(0xFF, 0xE800);

	for (i = 0; i < ASUSP5A_LOOP; i++) {
		outb(0xE1, 0xFF);
		if (inb(0xE800) & 0x04)
			break;
	}

	if (i == ASUSP5A_LOOP) {
		printf("%s: Unable to contact device.\n", name);
		return -1;
	}

	outb(0x20, 0xE801);
	outb(0x20, 0xE1);

	outb(0xFF, 0xE802);

	for (i = 0; i < ASUSP5A_LOOP; i++) {
		tmp = inb(0xE800);
		if (tmp & 0x70)
			break;
	}

	if ((i == ASUSP5A_LOOP) || !(tmp & 0x10)) {
		printf("%s: failed to read device.\n", name);
		return -1;
	}

	tmp = inb(0xE804);
	tmp &= ~0x02;

	outb(0x00, 0xE807);
	outb(0xEE, 0xE803);

	outb(tmp, 0xE804);

	outb(0xFF, 0xE800);
	outb(0xE1, 0xFF);

	outb(0x20, 0xE801);
	outb(0x20, 0xE1);

	outb(0xFF, 0xE802);

	for (i = 0; i < ASUSP5A_LOOP; i++) {
		tmp = inb(0xE800);
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
	outb(0x45, 0xcd6);
	byte = inb(0xcd7);
	outb(byte | 0x20, 0xcd7);

	return 0;
}

/**
 * Suited for EPoX EP-BX3, and maybe some other Intel 440BX based boards.
 */
static int board_epox_ep_bx3(const char *name)
{
	uint8_t tmp;

	/* Raise GPIO22. */
	tmp = inb(0x4036);
	outb(tmp, 0xEB);

	tmp |= 0x40;

	outb(tmp, 0x4036);
	outb(tmp, 0xEB);

	return 0;
}

/**
 * We use 2 sets of IDs here, you're free to choose which is which. This
 * is to provide a very high degree of certainty when matching a board on
 * the basis of subsystem/card IDs. As not every vendor handles
 * subsystem/card IDs in a sane manner.
 *
 * Keep the second set NULLed if it should be ignored.
 */
struct board_pciid_enable {
	/* Any device, but make it sensible, like the isa bridge. */
	uint16_t first_vendor;
	uint16_t first_device;
	uint16_t first_card_vendor;
	uint16_t first_card_device;

	/* Any device, but make it sensible, like 
	 * the host bridge. May be NULL
	 */
	uint16_t second_vendor;
	uint16_t second_device;
	uint16_t second_card_vendor;
	uint16_t second_card_device;

	/* From linuxbios table */
	char *lb_vendor;
	char *lb_part;

	char *name;
	int (*enable) (const char *name);
};

struct board_pciid_enable board_pciid_enables[] = {
	{0x10de, 0x0360, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	 "gigabyte", "m57sli", "GIGABYTE GA-M57SLI", it87xx_probe_serial_flash},
	{0x1022, 0x7468, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	 "iwill", "dk8_htx", "IWILL DK8-HTX", w83627hf_gpio24_raise},
	{0x1022, 0x746B, 0x1022, 0x36C0, 0x0000, 0x0000, 0x0000, 0x0000,
	 "AGAMI", "ARUMA", "agami Aruma", w83627hf_gpio24_raise},
	{0x1106, 0x3177, 0x1106, 0xAA01, 0x1106, 0x3123, 0x1106, 0xAA01,
	 NULL, NULL, "VIA EPIA M/MII/...", board_via_epia_m},
	{0x1106, 0x3177, 0x1043, 0x80A1, 0x1106, 0x3205, 0x1043, 0x8118,
	 NULL, NULL, "ASUS A7V8-MX SE", board_asus_a7v8x_mx},
	{0x8086, 0x1076, 0x8086, 0x1176, 0x1106, 0x3059, 0x10f1, 0x2498,
	 NULL, NULL, "Tyan Tomcat K7M", board_asus_a7v8x_mx},
	{0x10B9, 0x1541, 0x0000, 0x0000, 0x10B9, 0x1533, 0x0000, 0x0000,
	 "asus", "p5a", "ASUS P5A", board_asus_p5a},
	{0x1166, 0x0205, 0x1014, 0x0347, 0x0000, 0x0000, 0x0000, 0x0000,
	 "ibm", "x3455", "IBM x3455", board_ibm_x3455},
	{0x8086, 0x7110, 0x0000, 0x0000, 0x8086, 0x7190, 0x0000, 0x0000,
	 "epox", "ep-bx3", "EPoX EP-BX3", board_epox_ep_bx3},
	{0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL}	/* Keep this */
};

/**
 * Match boards on LinuxBIOS table gathered vendor and part name.
 * Require main PCI IDs to match too as extra safety.
 */
static struct board_pciid_enable *board_match_linuxbios_name(char *vendor,
							     char *part)
{
	struct board_pciid_enable *board = board_pciid_enables;

	for (; board->name; board++) {
		if (!board->lb_vendor || strcmp(board->lb_vendor, vendor))
			continue;

		if (!board->lb_part || strcmp(board->lb_part, part))
			continue;

		if (!pci_dev_find(board->first_vendor, board->first_device))
			continue;

		if (board->second_vendor &&
		    !pci_dev_find(board->second_vendor, board->second_device))
			continue;
		return board;
	}
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

int board_flash_enable(char *vendor, char *part)
{
	struct board_pciid_enable *board = NULL;
	int ret = 0;

	if (vendor && part)
		board = board_match_linuxbios_name(vendor, part);

	if (!board)
		board = board_match_pci_card_ids();

	if (board) {
		printf("Found board \"%s\": Enabling flash write... ",
		       board->name);

		ret = board->enable(board->name);
		if (ret)
			printf("Failed!\n");
		else
			printf("OK.\n");
	}

	return ret;
}
