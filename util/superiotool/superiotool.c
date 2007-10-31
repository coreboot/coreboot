/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2006 Ronald Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Carl-Daniel Hailfinger
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

/* Command line options. */
int dump = 0, verbose = 0;

/* Global flag which indicates whether a chip was detected at all. */
int chip_found = 0;

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

void enter_conf_mode_winbond_fintek_ite_8787(uint16_t port)
{
	outb(0x87, port);
	outb(0x87, port);
}

void exit_conf_mode_winbond_fintek_ite_8787(uint16_t port)
{
	outb(0xaa, port);		/* Fintek, Winbond */
	regwrite(port, 0x02, 0x02);	/* ITE */
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
		      int i, int j, uint16_t port)
{
	int k;
	const int *idx;

	if (reg_table[i].ldn[j].ldn != NOLDN) {
		printf("LDN 0x%02x ", reg_table[i].ldn[j].ldn);
		if (reg_table[i].ldn[j].name != NULL)
			printf("(%s)", reg_table[i].ldn[j].name);
		regwrite(port, 0x07, reg_table[i].ldn[j].ldn);
	} else {
		printf("Register dump:");
	}

	idx = reg_table[i].ldn[j].idx;

	printf("\nidx ");
	for (k = 0; /* Nothing */; k++) {
		if (idx[k] == EOT)
			break;
		printf("%02x ", idx[k]);
	}

	printf("\nval ");
	for (k = 0; /* Nothing */; k++) {
		if (idx[k] == EOT)
			break;
		printf("%02x ", regval(port, idx[k]));
	}

	printf("\ndef ");
	idx = reg_table[i].ldn[j].def;
	for (k = 0; /* Nothing */; k++) {
		if (idx[k] == EOT)
			break;
		else if (idx[k] == NANA)
			printf("NA ");
		else if (idx[k] == RSVD)
			printf("RR ");
		else if (idx[k] == MISC)	/* TODO */
			printf("MM ");
		else
			printf("%02x ", idx[k]);
	}
	printf("\n");
}

void dump_superio(const char *vendor,
		  const struct superio_registers reg_table[],
		  uint16_t port, uint16_t id)
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
			dump_regs(reg_table, i, j, port);
		}

		if (no_dump_available)
			printf("No dump available for this Super I/O\n");
	}
}

void probing_for(const char *vendor, const char *info, uint16_t port)
{
	if (!verbose)
		return;

	/* Yes, there's no space between '%s' and 'at'! */
	printf("Probing for %s Super I/O %sat 0x%x...\n",
	       vendor, info, port);
}

static void print_version(void)
{
	printf("superiotool r%s\n", SUPERIOTOOL_VERSION);
}

int main(int argc, char *argv[])
{
	int i, j, opt, option_index;

	static const struct option long_options[] = {
		{"dump",		no_argument, NULL, 'd'},
		{"verbose",		no_argument, NULL, 'V'},
		{"version",		no_argument, NULL, 'v'},
		{"help",		no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "dVvh",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'd':
			dump = 1;
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
			exit(0);
			break;
		default:
			/* Unknown option. */
			exit(1);
			break;
		}
	}

	if (iopl(3) < 0) {
		perror("iopl");
		printf("Superiotool must be run as root.\n");
		exit(1);
	}

	print_version();

	for (i = 0; i < ARRAY_SIZE(superio_ports_table); i++) {
		for (j = 0; superio_ports_table[i].ports[j] != EOT; j++)
			superio_ports_table[i].probe_idregs(
				superio_ports_table[i].ports[j]);
	}

	if (!chip_found)
		printf("No Super I/O found\n");

	return 0;
}
