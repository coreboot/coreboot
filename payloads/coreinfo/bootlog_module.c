/*
 * This file is part of the coreinfo project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifdef CONFIG_MODULE_BOOTLOG

#define CONFIG_COREBOOT_PRINTK_BUFFER_ADDR 0x90000
#define CONFIG_COREBOOT_PRINTK_BUFFER_SIZE 65536

static char *buf;
static s32 cursor = 0;
static s32 cursor_max;

static int bootlog_module_init(void)
{
	int i;
	volatile unsigned long *ptr =
		(void *)(CONFIG_COREBOOT_PRINTK_BUFFER_ADDR + 16); /* FIXME */

	buf = malloc(CONFIG_COREBOOT_PRINTK_BUFFER_SIZE);
	if (!buf) {
		/* TODO */
	}

	memcpy(buf, (char *)ptr, CONFIG_COREBOOT_PRINTK_BUFFER_SIZE);

	cursor_max = CONFIG_COREBOOT_PRINTK_BUFFER_SIZE;
	for (i = 0; i < 20; i++) {
		do {
			cursor_max--;
		} while (*(buf + cursor_max) != '\n');
	}
	cursor_max++;	/* Stay _behind_ the newline. */

	/* TODO: Maybe a _cleanup hook where we call free()? */

	return 0;
}

static int bootlog_module_redraw(WINDOW *win)
{
	int x = 0, y = 0;
	char *tmp = buf + cursor;

	print_module_title(win, "Coreboot Bootlog");

	/* FIXME: Handle lines longer than 80 characters. */
	while (y <= 18) {
		mvwaddch(win, y + 2, x, isprint(*tmp) ? *tmp : ' ');
		x++;
		tmp++;
		if (*tmp == '\n') {
			y++;
			x = 0;
			tmp++;		/* Skip the newline. */
		}
	}

	return 0;
}

/* TODO: Simplify code. */
static int bootlog_module_handle(int key)
{
	int i;

	switch (key) {
	case KEY_DOWN:
		if (cursor == cursor_max)
			return 0;
		while (*(buf + cursor) != '\n')
			cursor++;
		cursor++;	/* Skip the newline. */
		break;
	case KEY_UP:
		if (cursor == 0)
			return 0;
		cursor--;	/* Skip the newline. */
		do {
			cursor--;
		} while (*(buf + cursor) != '\n');
		cursor++;	/* Stay _behind_ the newline. */
		break;
	case KEY_NPAGE:
		if (cursor == cursor_max)
			return 0;
		for (i = 0; i < 20; i++) {
			while (*(buf + cursor) != '\n')
				cursor++;
			cursor++;	/* Skip the newline. */
		}
		break;
	case KEY_PPAGE:
		if (cursor == 0)
			return 0;
		for (i = 0; i < 20; i++) {
			do {
				cursor--;
			} while (*(buf + cursor) != '\n');
		}
		cursor++;	/* Stay _behind_ the newline. */
		break;
	}

	if (cursor > cursor_max)
		cursor = cursor_max;

	if (cursor < 0)
		cursor = 0;

	return 1;
}

struct coreinfo_module bootlog_module = {
	.name = "Bootlog",
	.init = bootlog_module_init,
	.redraw = bootlog_module_redraw,
	.handle = bootlog_module_handle,
};

#else

struct coreinfo_module bootlog_module = {
};

#endif
