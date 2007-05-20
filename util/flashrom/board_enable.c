/*
 *   flash rom utility: enable flash writes (board specific)
 *
 *   Copyright (C) 2005-2007 coresystems GmbH <stepan@coresystems.de>
 *   Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 *   Copyright (C) 2007 Luc Verhaegen <libv@skynet.be>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   version 2
 *
 */

/*
 * Contains the board specific flash enables.
 */

#include <stdio.h>
#include <pci/pci.h>
#include <stdint.h>
#include <string.h>

#include "flash.h"
#include "debug.h"

/*
 * Helper functions for many Winbond superIOs of the w836xx range.
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

/* General functions for read/writing WB SuperIOs */
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

static void
wbsio_mask(unsigned char index, unsigned char data, unsigned char mask)
{
	unsigned char tmp;

	outb(index, W836_INDEX);
	tmp = inb(W836_DATA) & ~mask;
	outb(tmp | (data & mask), W836_DATA);
}

/*
 * WinBond w83627hf: raise GPIO24.
 *
 * Suited for:
 *      * Agami Aruma
 *      * IWILL DK8-HTX
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

/*
 * Suited for VIAs EPIA M and MII, and maybe other CLE266 based EPIAs.
 *
 * We don't need to do this when using linuxbios, GPIO15 is never lowered there.
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

/*
 * Suited for ASUS A7V8X-MX SE and A7V400-MX.
 *
 */

static int board_asus_a7v8x_mx(const char *name)
{
	struct pci_dev *dev;
	uint8_t val;

	dev = pci_dev_find(0x1106, 0x3177);	/* VT8235 ISA bridge */
	if (!dev) {
		fprintf(stderr, "\nERROR: VT8235 ISA Bridge not found.\n");
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

/*
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

/*
 * We use 2 sets of ids here, you're free to choose which is which. This
 * to provide a very high degree of certainty when matching a board on
 * the basis of Subsystem/card ids. As not every vendor handles
 * subsystem/card ids in a sane manner.
 *
 * Keep the second set nulled if it should be ignored.
 *
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
	{0x1022, 0x7468, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	 "iwill", "dk8_htx", "IWILL DK8-HTX", w83627hf_gpio24_raise},
	{0x1022, 0x746B, 0x1022, 0x36C0, 0x0000, 0x0000, 0x0000, 0x0000,
	 "AGAMI", "ARUMA", "agami Aruma", w83627hf_gpio24_raise},
	{0x1106, 0x3177, 0x1106, 0xAA01, 0x1106, 0x3123, 0x1106, 0xAA01,
	 NULL, NULL, "VIA EPIA M/MII/...", board_via_epia_m},
	{0x1106, 0x3177, 0x1043, 0x80A1, 0x1106, 0x3205, 0x1043, 0x8118,
	 NULL, NULL, "ASUS A7V8-MX SE", board_asus_a7v8x_mx},
	{0x10B9, 0x1541, 0x0000, 0x0000, 0x10B9, 0x1533, 0x0000, 0x0000,
	 "asus", "p5a", "ASUS P5A", board_asus_p5a},
	{0, 0, 0, 0, 0, 0, 0, 0, NULL, NULL}	/* Keep this */
};

/*
 * Match boards on linuxbios table gathered vendor and part name.
 * Require main pci-ids to match too as extra safety.
 *
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

/*
 * Match boards on pci ids and subsystem ids.
 * Second set of ids can be main only or missing completely.
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

/*
 *
 */
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
