/*
 * This file is part of the LinuxBIOS project.
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

void dump_superio(const char *vendor, const struct superio_registers reg_table[],
		  uint16_t port, uint16_t id)
{
	int i, j, k, nodump;
	int *idx;

	if (!dump)
		return;

	for (i = 0; /* Nothing */; i++) {
		if (reg_table[i].superio_id == EOT)
			break;

		if ((uint16_t)reg_table[i].superio_id != id)
			continue;

		nodump = 1;

		for (j = 0; /* Nothing */; j++) {
			if (reg_table[i].ldn[j].ldn == EOT)
				break;

			nodump = 0;

			if (reg_table[i].ldn[j].ldn != NOLDN) {
				printf("Switching to LDN 0x%02x\n",
				       reg_table[i].ldn[j].ldn);
				regwrite(port, 0x07, reg_table[i].ldn[j].ldn);
			}

			idx = reg_table[i].ldn[j].idx;

			printf("idx ");
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

		if (nodump)
			printf("No dump for %s %s\n", vendor, reg_table[i].name);
	}
}

void no_superio_found(uint16_t port) {
	if (!verbose)
		return;

	if (inb(port) == 0xff)
		printf("No Super I/O chip found at 0x%04x\n", port);
	else
		printf("Probing 0x%04x, failed (0x%02x), data returns 0x%02x\n", port, inb(port), inb(port + 1));
}

int main(int argc, char *argv[])
{
	int i, j, opt, option_index;

	const static struct option long_options[] = {
		{"dump",	no_argument, NULL, 'd'},
		{"verbose",	no_argument, NULL, 'V'},
		{"version",	no_argument, NULL, 'v'},
		{"help",	no_argument, NULL, 'h'},
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
			printf("superiotool %s\n", SUPERIOTOOL_VERSION);
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

	for (i = 0; i < ARRAY_SIZE(superio_ports_table); i++) {
		for (j = 0; superio_ports_table[i].ports[j] != EOT; j++)
			superio_ports_table[i].probe_idregs(
				superio_ports_table[i].ports[j]);
	}

	return 0;
}
