/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Carl-Daniel Hailfinger
 * Copyright (C) 2008 Robinson P. Tryon <bishop.robinson@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include "superiotool.h"

#if defined(__FreeBSD__)
#include <fcntl.h>
#include <unistd.h>
#endif

/* Command line options. */
int dump = 0, verbose = 0, extra_dump = 0;

/* Global flag which indicates whether a chip was detected at all. */
int chip_found = 0;

static void set_bank(uint16_t port, uint8_t bank)
{
	OUTB(0x4E, port);
	OUTB(bank, port + 1);
}

static uint8_t datareg(uint16_t port, uint8_t reg)
{
	OUTB(reg, port);
	return INB(port + 1);
}

uint8_t regval(uint16_t port, uint8_t reg)
{
	OUTB(reg, port);
	return INB(port + ((port == 0x3bd) ? 2 : 1)); /* 0x3bd is special. */
}

void regwrite(uint16_t port, uint8_t reg, uint8_t val)
{
	OUTB(reg, port);
	OUTB(val, port + 1);
}

void enter_conf_mode_winbond_fintek_ite_8787(uint16_t port)
{
	OUTB(0x87, port);
	OUTB(0x87, port);
}

void exit_conf_mode_winbond_fintek_ite_8787(uint16_t port)
{
	OUTB(0xaa, port);		/* Fintek, Winbond */
	regwrite(port, 0x02, 0x02);	/* ITE */
}

void enter_conf_mode_fintek_7777(uint16_t port)
{
	OUTB(0x77, port);
	OUTB(0x77, port);
}

void exit_conf_mode_fintek_7777(uint16_t port)
{
	OUTB(0xaa, port);		/* Fintek */
}

int superio_unknown(const struct superio_registers reg_table[], uint16_t id)
{
	return !strncmp(get_superio_name(reg_table, id), "<unknown>", 9);
}


const char *get_superio_name(const struct superio_registers reg_table[],
			     uint16_t id)
{
	int i;

	for (i = 0; /* Nothing */; i++) {
		if (reg_table[i].superio_id == EOT)
			break;

		if ((uint16_t)reg_table[i].superio_id != id)
			continue;

		return reg_table[i].name;
	}

	return "<unknown>";
}

static void dump_regs(const struct superio_registers reg_table[],
		      int i, int j, uint16_t port, uint8_t ldn_sel)
{
	int k;
	const int16_t *idx;

	if (reg_table[i].ldn[j].ldn != NOLDN) {
		printf("LDN 0x%02x", reg_table[i].ldn[j].ldn);
		if (reg_table[i].ldn[j].name != NULL)
			printf(" (%s)", reg_table[i].ldn[j].name);
		regwrite(port, ldn_sel, reg_table[i].ldn[j].ldn);
	} else {
		if (reg_table[i].ldn[j].name == NULL)
			printf("Register dump:");
		else
			printf("(%s)", reg_table[i].ldn[j].name);
	}

	idx = reg_table[i].ldn[j].idx;

	printf("\nidx");
	for (k = 0; idx[k] != EOT; k++) {
		if (k && !(k % 8))
			putchar(' ');
		printf(" %02x", idx[k]);
	}

	printf("\nval");
	for (k = 0; idx[k] != EOT; k++) {
		if (k && !(k % 8))
			putchar(' ');
		printf(" %02x", regval(port, idx[k]));
	}

	printf("\ndef");
	idx = reg_table[i].ldn[j].def;
	for (k = 0; idx[k] != EOT; k++) {
		if (k && !(k % 8))
			putchar(' ');
		if (idx[k] == NANA)
			printf(" NA");
		else if (idx[k] == RSVD)
			printf(" RR");
		else if (idx[k] == MISC)
			printf(" MM");
		else
			printf(" %02x", idx[k]);
	}
	printf("\n");
}

void dump_superio(const char *vendor,
		  const struct superio_registers reg_table[],
		  uint16_t port, uint16_t id, uint8_t ldn_sel)
{
	int i, j, no_dump_available = 1;

	if (!dump)
		return;

	for (i = 0; /* Nothing */; i++) {
		if (reg_table[i].superio_id == EOT)
			break;

		if ((uint16_t)reg_table[i].superio_id != id)
			continue;

		for (j = 0; /* Nothing */; j++) {
			if (reg_table[i].ldn[j].ldn == EOT)
				break;
			no_dump_available = 0;
			dump_regs(reg_table, i, j, port, ldn_sel);
		}

		if (no_dump_available)
			printf("No dump available for this Super I/O\n");
	}
}

void dump_io(uint16_t iobase, uint16_t length)
{
	uint16_t i;

	printf("Dumping %d I/O mapped registers at base 0x%04x:\n",
			length, iobase);
	for (i = 0; i < length; i++)
		printf("%02x ", i);
	printf("\n");
	for (i = 0; i < length; i++)
		printf("%02x ", INB(iobase + i));
	printf("\n");
}

void dump_data(uint16_t iobase, int bank)
{
	uint16_t i;

	printf("Bank %d:\n", bank);
	printf("    ");
	for (i = 0; i < 16; i++)
		printf("%02x ", i);
	set_bank(iobase, bank);
	for (i = 0; i < 256; i++) {
		if (i % 16 == 0)
			printf("\n%02x: ", i / 16);
		printf("%02x ", datareg(iobase, i));
	}
	printf("\n");
}

void probing_for(const char *vendor, const char *info, uint16_t port)
{
	if (!verbose)
		return;

	/* Yes, there's no space between '%s' and 'at'! */
	printf("Probing for %s Super I/O %sat 0x%x...\n", vendor, info, port);
}

/** Print a list of all supported chips from the given vendor. */
void print_vendor_chips(const char *vendor,
			const struct superio_registers reg_table[])
{
	int i;

	for (i = 0; reg_table[i].superio_id != EOT; i++) {
		printf("%s %s", vendor, reg_table[i].name);

		/* Unless the ldn is empty, assume this chip has a dump. */
		if (reg_table[i].ldn[0].ldn != EOT)
			printf(" (dump available)");

		printf("\n");
	}

	/* If we printed any chips for this vendor, put in a blank line. */
	if (i != 0)
		printf("\n");
}

/** Print a list of all chips supported by superiotool. */
void print_list_of_supported_chips(void)
{
	int i;

	printf("Supported Super I/O chips:\n\n");

	for (i = 0; i < ARRAY_SIZE(vendor_print_functions); i++)
		vendor_print_functions[i].print_list();

	printf("See <http://coreboot.org/Superiotool#Supported_devices> "
	       "for more information.\n");
}

static void print_version(void)
{
	printf("superiotool r%s\n", SUPERIOTOOL_VERSION);
}

int main(int argc, char *argv[])
{
	int i, j, opt, option_index;
#if defined(__FreeBSD__)
	int io_fd;
#endif

	static const struct option long_options[] = {
		{"dump",		no_argument, NULL, 'd'},
		{"extra-dump",		no_argument, NULL, 'e'},
		{"list-supported",	no_argument, NULL, 'l'},
		{"verbose",		no_argument, NULL, 'V'},
		{"version",		no_argument, NULL, 'v'},
		{"help",		no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "delVvh",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'd':
			dump = 1;
			break;
		case 'e':
			extra_dump = 1;
			break;
		case 'l':
			print_list_of_supported_chips();
			exit(0);
			break;
		case 'V':
			verbose = 1;
			break;
		case 'v':
			print_version();
			exit(0);
			break;
		case 'h':
			printf(USAGE);
			printf(USAGE_INFO);
			exit(0);
			break;
		default:
			/* Unknown option. */
			exit(1);
			break;
		}
	}

#if defined(__FreeBSD__)
	if ((io_fd = open("/dev/io", O_RDWR)) < 0) {
		perror("/dev/io");
#else
	if (iopl(3) < 0) {
		perror("iopl");
#endif
		printf("Superiotool must be run as root.\n");
		exit(1);
	}

	print_version();

#ifdef PCI_SUPPORT
	/* Do some basic libpci init. */
	pacc = pci_alloc();
	pci_init(pacc);
	pci_scan_bus(pacc);
#endif

	for (i = 0; i < ARRAY_SIZE(superio_ports_table); i++) {
		for (j = 0; superio_ports_table[i].ports[j] != EOT; j++)
			superio_ports_table[i].probe_idregs(
				superio_ports_table[i].ports[j]);
	}

	if (!chip_found)
		printf("No Super I/O found\n");

#if defined(__FreeBSD__)
	close(io_fd);
#endif
	return 0;
}
