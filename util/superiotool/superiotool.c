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

unsigned char regval(unsigned short port, unsigned char reg)
{
	outb(reg, port);
	return inb(port + 1);
}

void regwrite(unsigned short port, unsigned char reg, unsigned char val)
{
	outb(reg, port);
	outb(val, port + 1);
}

void dump_superio(const char *name, const struct superio_registers reg_table[],
		  unsigned short port, unsigned short id)
{
	int i, j, k;
	signed short *idx;

	printf("%s ", name);

	for (i = 0; /* Nothing */; i++) {
		if (reg_table[i].superio_id == EOT)
			break;

		if ((unsigned short)reg_table[i].superio_id != id)
			continue;

		printf("%s\n", reg_table[i].name);

		for (j = 0;; j++) {
			if (reg_table[i].ldn[j].ldn == EOT)
				break;

			if (reg_table[i].ldn[j].ldn != NOLDN) {
				printf("Switching to LDN 0x%01x\n",
				       reg_table[i].ldn[j].ldn);
				regwrite(port, 0x07,
					 reg_table[i].ldn[j].ldn);
			}

			idx = reg_table[i].ldn[j].idx;

			printf("idx ");
			for (k = 0;; k++) {
				if (idx[k] == EOT)
					break;
				printf("%02x ", idx[k]);
			}

			printf("\nval ");
			for (k = 0;; k++) {
				if (idx[k] == EOT)
					break;
				printf("%02x ", regval(port, idx[k]));
			}

			printf("\ndef ");
			idx = reg_table[i].ldn[j].def;
			for (k = 0;; k++) {
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
