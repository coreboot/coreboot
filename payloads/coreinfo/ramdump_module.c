/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"

#if CONFIG(MODULE_RAMDUMP)

static s64 cursor = 0;
static s64 cursor_max = (1 * 1024 * 1024 * 1024); /* Max. 1 GB RAM for now. */

static int ramdump_module_init(void)
{
	return 0;
}

static void dump_ram(WINDOW *win, uint32_t addr, int row, int col)
{
	int i, x = 0, y = 0, count = 0;
	volatile uint8_t *ptr = (void *)(addr);

	mvwprintw(win, 0, col + 54, "RAM address: 0x%08x", addr);

	/* Dump 256 bytes of RAM. */
	for (i = 1; i < 257; i++) {
		if (x == 0) {
			mvwprintw(win, row + y, col - 1, "%08x", addr + 16 * y);
			mvwaddch(win, row + y, col + 59, '|');
			mvwaddch(win, row + y, col + 76, '|');
		}
		mvwprintw(win, row + y, col + x + 9, "%02x", ptr[i - 1]);
		mvwprintw(win, row + y, 62 + count++, "%c",
			  isprint(ptr[i - 1]) ? ptr[i - 1] : ' ');
		x += 3;
		if (x == 24)	/* One more space after column/byte 8. */
			x++;
		if (i % 16 == 0) {
			y++;	/* Start a newline after 16 bytes. */
			x = count = 0;
		}
	}
}

static int ramdump_module_redraw(WINDOW *win)
{
	print_module_title(win, "RAM Dump");
	dump_ram(win, cursor * 256, 2, 2);

	return 0;
}

static int ramdump_module_handle(int key)
{
	switch (key) {
	case KEY_DOWN:
		cursor++;
		break;
	case KEY_UP:
		cursor--;
		break;
	case KEY_RIGHT:
		cursor += 256;
		break;
	case KEY_LEFT:
		cursor -= 256;
		break;
	case KEY_PPAGE:
		cursor += 4096; /* Jump in 1MB steps. */
		break;
	case KEY_NPAGE:
		cursor -= 4096; /* Jump in 1MB steps. */
		break;
	}

	if (cursor > cursor_max)
		cursor = cursor_max;

	if (cursor < 0)
		cursor = 0;

	return 1;
}

struct coreinfo_module ramdump_module = {
	.name = "RAM Dump",
	.init = ramdump_module_init,
	.redraw = ramdump_module_redraw,
	.handle = ramdump_module_handle,
};

#else

struct coreinfo_module ramdump_module = {
};

#endif
