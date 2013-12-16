/*
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, incorporated herein by reference.
 */

#include <libpayload.h>

void hexdump(void *memory, int length)
{
	int i;
	uint8_t *m;
	int all_zero = 0;

	m = (uint8_t *) memory;

	for (i = 0; i < length; i += 16) {
		int j;

		all_zero++;
		for (j = 0; j < 16; j++) {
			if (m[i + j] != 0) {
				all_zero = 0;
				break;
			}
		}

		if (all_zero < 2) {
			printf("%08lx:", memory + i);
			for (j = 0; j < 16; j++)
				printf(" %02x", m[i + j]);
			printf("  ");
			for (j = 0; j < 16; j++)
				printf("%c",
				       isprint(m[i + j]) ? m[i + j] : '.');
			printf("\n");
		} else if (all_zero == 2) {
			printf("...\n");
		}
	}
}
