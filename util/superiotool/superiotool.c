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

void dump_superio(const char *vendor, const struct superio_registers reg_table[],
		  uint16_t port, uint16_t id)
{
	int i, j, k, nodump;
	int *idx;

	for (i = 0; /* Nothing */; i++) {
		if (reg_table[i].superio_id == EOT)
			break;

		if ((uint16_t)reg_table[i].superio_id != id)
			continue;

		nodump = 1;

		for (j = 0; /* Nothing */; j++) {
			if (reg_table[i].ldn[j].ldn == EOT)
				break;

			printf("%s %s\n", vendor, reg_table[i].name);
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
				else
					printf("%02x ", idx[k]);
			}
			printf("\n");
		}

		if (nodump)
			printf("No dump for %s %s\n", vendor, reg_table[i].name);
	}
}

int main(int argc, char *argv[])
{
	int i, j;

	if (iopl(3) < 0) {
		perror("iopl");
		exit(1);
	}

	for (i = 0; i < ARRAY_SIZE(superio_ports_table); i++) {
		for (j = 0; superio_ports_table[i].ports[j] != EOT; j++)
			superio_ports_table[i].probe_idregs(
				superio_ports_table[i].ports[j]);
	}

	return 0;
}
