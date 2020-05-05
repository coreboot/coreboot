/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <ctype.h>
#include <lib.h>

void hexdump(const void *memory, size_t length)
{
	size_t i, j;
	uint8_t *line;
	int all_zero = 0;
	int all_one = 0;
	size_t num_bytes;

	for (i = 0; i < length; i += 16) {
		num_bytes = MIN(length - i, 16);
		line = ((uint8_t *)memory) + i;

		all_zero++;
		all_one++;
		for (j = 0; j < num_bytes; j++) {
			if (line[j] != 0) {
				all_zero = 0;
				break;
			}
		}

		for (j = 0; j < num_bytes; j++) {
			if (line[j] != 0xff) {
				all_one = 0;
				break;
			}
		}

		if ((all_zero < 2) && (all_one < 2)) {
			printk(BIOS_DEBUG, "%p:", memory + i);
			for (j = 0; j < num_bytes; j++)
				printk(BIOS_DEBUG, " %02x", line[j]);
			for (; j < 16; j++)
				printk(BIOS_DEBUG, "   ");
			printk(BIOS_DEBUG, "  ");
			for (j = 0; j < num_bytes; j++)
				printk(BIOS_DEBUG, "%c",
				       isprint(line[j]) ? line[j] : '.');
			printk(BIOS_DEBUG, "\n");
		} else if ((all_zero == 2) || (all_one == 2)) {
			printk(BIOS_DEBUG, "...\n");
		}
	}
}

void hexdump32(char LEVEL, const void *d, size_t len)
{
	size_t count = 0;

	while (len > 0) {
		if (count % 8 == 0) {
			printk(LEVEL, "\n");
			printk(LEVEL, "%p:", d);
		}
		printk(LEVEL, " 0x%08lx", *(unsigned long *)d);
		count++;
		len--;
		d += 4;
	}

	printk(LEVEL, "\n\n");
}
