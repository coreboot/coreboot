/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <ctype.h>
#include <lib.h>

void hexdump_loglevel(int msg_level, const void *memory, size_t length)
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
			printk(msg_level, "%p:", memory + i);
			for (j = 0; j < num_bytes; j++)
				printk(msg_level, " %02x", line[j]);
			for (; j < 16; j++)
				printk(msg_level, "   ");
			printk(msg_level, "  ");
			for (j = 0; j < num_bytes; j++)
				printk(msg_level, "%c",
				       isprint(line[j]) ? line[j] : '.');
			printk(msg_level, "\n");
		} else if ((all_zero == 2) || (all_one == 2)) {
			printk(msg_level, "...\n");
		}
	}
}

void hexdump(const void *memory, size_t length)
{
	hexdump_loglevel(BIOS_DEBUG, memory, length);
}
