/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"

#if CONFIG(MODULE_NVRAM)

/**
 * Dump 256 bytes of NVRAM.
 */
static void dump_nvram(WINDOW *win, int row, int col)
{
	int i, x = 0, y = 0;

	/* Print vertical and horizontal index numbers. */
	for (i = 0; i < 16; i++) {
		mvwprintw(win, ((i < 8) ? 4 : 5) + i, 1, "%2.2X ", i * 0x10);
		mvwprintw(win, 2, 4 + (i * 3), "%2.2X ", i);
	}

	/* Print vertical and horizontal line. */
	for (i = 0; i < 18; i++)
		mvwaddch(win, 3 + i, 3, ACS_VLINE);
	for (i = 0; i < 48; i++)
		mvwaddch(win, 3, 3 + i, (i == 0) ? ACS_ULCORNER : ACS_HLINE);

	/* Dump NVRAM contents. */
	for (i = 1; i < 257; i++) {
		mvwprintw(win, row + y, col + x, "%02x ", nvram_read(i - 1));
		x += 3;
		if (i % 16 == 0) {
			y++;	/* Start a newline after 16 bytes. */
			x = 0;
		}
		if (i % 128 == 0) {
			y++;	/* Add an empty line after 128 bytes. */
			x = 0;
		}
	}
}

static int nvram_module_redraw(WINDOW *win)
{
	print_module_title(win, "NVRAM Dump");
	dump_nvram(win, 4, 4);
	return 0;
}

static int nvram_module_init(void)
{
	return 0;
}

struct coreinfo_module nvram_module = {
	.name = "NVRAM",
	.init = nvram_module_init,
	.redraw = nvram_module_redraw,
};

#else

struct coreinfo_module nvram_module = {
};

#endif
