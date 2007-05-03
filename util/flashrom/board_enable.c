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

static int board_iwill_dk8_htx(const char *name)
{
	/* Extended function index register, either 0x2e or 0x4e. */
#define EFIR 0x2e
	/* Extended function data register, one plus the index reg. */
#define EFDR EFIR + 1
	char b;

	/* Disable the flash write protect (which is connected to the
	 * Winbond W83627HF GPIOs).
	 */
	outb(0x87, EFIR);	/* Sequence to unlock extended functions */
	outb(0x87, EFIR);

	/* Activate logical device. */
	outb(0x7, EFIR);
	outb(8, EFDR);

	/* Set GPIO regs. */
	outb(0x2b, EFIR);	/* GPIO multiplexed pin reg. */
	b = inb(EFDR) | 0xd0;
	outb(0x2b, EFIR);
	outb(b, EFDR);

	outb(0x30, EFIR);	/* GPIO2 */
	b = inb(EFDR) | 0x01;
	outb(0x30, EFIR);
	outb(b, EFDR);

	outb(0xf0, EFIR);	/* IO sel */
	b = inb(EFDR) | 0xef;
	outb(0xf0, EFIR);
	outb(b, EFDR);

	outb(0xf1, EFIR);	/* GPIO data reg */
	b = inb(EFDR) | 0x16;
	outb(0xf1, EFIR);
	outb(b, EFDR);

	outb(0xf2, EFIR);	/* GPIO inversion reg */
	b = inb(EFDR) | 0x00;
	outb(0xf2, EFIR);
	outb(b, EFDR);

	/* Lock extended functions again. */
	outb(0xaa, EFIR);	/* Command to exit extended functions */

	return 0;
}

/*
 * Match on pci-ids, no report received, just data from the mainboard
 * specific code:
 *   main: 0x1022:0x746B, which is the SMBUS controller.
 *   card: 0x1022:0x36C0...
 */

static int board_agami_aruma(char *name)
{
	/* Extended function index register, either 0x2e or 0x4e    */
#define EFIR 0x2e
	/* Extended function data register, one plus the index reg. */
#define EFDR EFIR + 1
	char b;

        /*  Disable the flash write protect.  The flash write protect is
         *  connected to the WinBond w83627hf GPIO 24.
         */
	outb(0x87, EFIR); /* sequence to unlock extended functions */
	outb(0x87, EFIR);

	outb(0x20, EFIR); /* SIO device ID register */
	b = inb(EFDR);
	printf_debug("\nW83627HF device ID = 0x%x\n",b);

	if (b != 0x52) {
		fprintf(stderr, "\nIncorrect device ID, aborting write protect disable\n");
		return -1;
	}

	outb(0x2b, EFIR); /* GPIO multiplexed pin reg. */
	b = inb(EFDR) | 0x10;
	outb(0x2b, EFIR);
	outb(b, EFDR); /* select GPIO 24 instead of WDTO */

	outb(0x7, EFIR); /* logical device select */
        outb(0x8, EFDR); /* point to device 8, GPIO port 2 */

	outb(0x30, EFIR); /* logic device activation control */
	outb(0x1, EFDR); /* activate */

	outb(0xf0, EFIR); /* GPIO 20-27 I/O selection register */
	b = inb(EFDR) & ~0x10;
	outb(0xf0, EFIR);
	outb(b, EFDR); /* set GPIO 24 as an output */

	outb(0xf1, EFIR); /* GPIO 20-27 data register */
	b = inb(EFDR) | 0x10;
	outb(0xf1, EFIR);
	outb(b, EFDR); /* set GPIO 24 */

	outb(0xaa, EFIR); /* command to exit extended functions */

	return 0;
}

/*
 * Suited for VIAs EPIA M and MII, and maybe other CLE266 based EPIAs.
 *
 * We don't need to do this when using linuxbios, GPIO15 is never lowered there.
 */

static int board_via_epia_m(char *name)
{
        struct pci_dev *dev;
        unsigned int base;
        uint8_t val;

        dev = pci_dev_find(0x1106, 0x3177); /* VT8235 ISA bridge */
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
 * Winbond LPC super IO.
 *
 * Raises the ROM MEMW# line.
 */

static void w83697_rom_memw_enable(void)
{
        uint8_t val;

        outb(0x87, 0x2E); /* enable extended functions */
        outb(0x87, 0x2E);

        outb(0x24, 0x2E); /* rom bits live here */

        val = inb(0x2F);
        if (!(val & 0x02)) /* flash rom enabled? */
                outb(val | 0x08, 0x2F); /* enable MEMW# */

        outb(0xAA, 0x2E); /* disable extended functions */
}

/*
 * Suited for Asus A7V8X-MX SE and A7V400-MX.
 *
 */

static int board_asus_a7v8x_mx(char *name)
{
        struct pci_dev *dev;
        uint8_t val;

        dev = pci_dev_find(0x1106, 0x3177); /* VT8235 ISA bridge */
        if (!dev) {
                fprintf(stderr, "\nERROR: VT8235 ISA Bridge not found.\n");
                return -1;
        }

        /* This bit is marked reserved actually */
        val = pci_read_byte(dev, 0x59);
        val &= 0x7F;
        pci_write_byte(dev, 0x59, val);

        w83697_rom_memw_enable();

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
        uint16_t  first_vendor;
        uint16_t  first_device;
        uint16_t  first_card_vendor;
        uint16_t  first_card_device;

        /* Any device, but make it sensible, like 
	 * the host bridge. May be NULL
	 */
        uint16_t  second_vendor;
        uint16_t  second_device;
        uint16_t  second_card_vendor;
        uint16_t  second_card_device;

        /* From linuxbios table */
        char  *lb_vendor;
        char  *lb_part;

        char  *name;
        int  (*enable)(char *name);
};

struct board_pciid_enable board_pciid_enables[] = {
	{ 0x1022, 0x746B, 0x1022, 0x36C0,  0x0000, 0x0000, 0x0000, 0x0000,
		"AGAMI", "ARUMA", "agami Aruma", board_agami_aruma },
	{ 0x1106, 0x3177, 0x1106, 0xAA01,  0x1106, 0x3123, 0x1106, 0xAA01, 
		NULL, NULL, "VIA EPIA M/MII/...", board_via_epia_m },
	{ 0x1106, 0x3177, 0x1043, 0x80A1,  0x1106, 0x3205, 0x1043, 0x8118, 
		NULL, NULL, "ASUS A7V8-MX SE", board_asus_a7v8x_mx },
	{ 0x1022, 0x7468, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0,
		"iwill", "dk8_htx", "IWILL DK8-HTX", board_iwill_dk8_htx },

        { 0, 0, 0, 0,  0, 0, 0, 0, NULL, NULL } /* Keep this */
};

/*
 * Match boards on linuxbios table gathered vendor and part name.
 * Require main pci-ids to match too as extra safety.
 *
 */
static struct board_pciid_enable *
board_match_linuxbios_name(char *vendor, char *part)
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
