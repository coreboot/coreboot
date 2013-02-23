/*
 * This file is part of the coreinfo project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "coreinfo.h"

#ifdef CONFIG_MODULE_LAR

static struct LAR *lar;
static int lcount, selected;
static char **lnames;
static const char *compression_table[4] = {"none", "LZMA", "NRV2B", "zeroes"};

static int lar_module_init(void)
{
	int index = 0;
	struct larent *larent;

	lar = openlar(NULL);

	if (lar == NULL)
		return 0;

	while ((larent = readlar(lar)))
		lcount++;

	lnames = malloc(lcount * sizeof(char *));

	if (lnames == NULL)
		return 0;

	rewindlar(lar);

	while ((larent = readlar(lar)))
		lnames[index++] = strdup((const char *) larent->name);

	return 0;
}

static int lar_module_redraw(WINDOW *win)
{
	int i, row = 2;
	struct larstat stat;

	print_module_title(win, "LAR Listing");

	if (lar == 0) {
		mvwprintw(win, 11, 61 / 2, "Bad or missing LAR");
		return 0;
	}

	/* Draw a line down the middle. */
	for (i = 2; i < 21; i++)
		mvwaddch(win, i, 30, ACS_VLINE);

	/* Draw the names down the left side. */
	for (i = 0; i < lcount; i++) {
		if (i == selected)
			wattrset(win, COLOR_PAIR(3) | A_BOLD);
		else
			wattrset(win, COLOR_PAIR(2));

		mvwprintw(win, 2 + i, 1, "%.25s", lnames[i]);
	}

	/* Get the information for the LAR. */
	if (larstat(lar, lnames[selected], &stat)) {
		printf("larstat failed\n");
		return 0;
	}

	wattrset(win, COLOR_PAIR(2));

	mvwprintw(win, row++, 32, "Offset: 0x%x", stat.offset);

	if (stat.compression) {
		mvwprintw(win, row++, 32, "Compression: %s",
			  compression_table[stat.compression]);
		mvwprintw(win, row++, 32, "Compressed length: %d", stat.len);
		mvwprintw(win, row++, 32, "Compressed checksum: 0x%x",
			  stat.compchecksum);
	}

	mvwprintw(win, row++, 32, "Length: %d", stat.reallen);
	mvwprintw(win, row++, 32, "Checksum: 0x%x", stat.checksum);
	mvwprintw(win, row++, 32, "Load address: 0x%llx", stat.loadaddress);
	mvwprintw(win, row++, 32, "Entry point: 0x%llx", stat.entry);

	return 0;
}

static int lar_module_handle(int key)
{
	int ret = 0;

	if (lar == NULL)
		return 0;

	switch (key) {
	case KEY_DOWN:
		if (selected + 1 < lcount) {
			selected++;
			ret = 1;
		}
		break;
	case KEY_UP:
		if (selected > 0) {
			selected--;
			ret = 1;
		}
		break;
	}

	return ret;
}

struct coreinfo_module lar_module = {
	.name = "LAR",
	.init = lar_module_init,
	.redraw = lar_module_redraw,
	.handle = lar_module_handle
};

#else

struct coreinfo_module lar_module = {
};

#endif
