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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "coreinfo.h"

#define SCREEN_Y 24
#define SCREEN_X 80

extern struct coreinfo_module cpuinfo_module;
extern struct coreinfo_module pci_module;
extern struct coreinfo_module coreboot_module;
extern struct coreinfo_module nvram_module;

struct coreinfo_module *modules[] = {
#ifdef CONFIG_MODULE_CPUINFO
	&cpuinfo_module,
#endif
#ifdef CONFIG_MODULE_PCI
	&pci_module,
#endif
#ifdef CONFIG_MODULE_COREBOOT
	&coreboot_module,
#endif
#ifdef CONFIG_MODULE_NVRAM
	&nvram_module,
#endif
};

static WINDOW *modwin;
static int curwin;

void print_module_title(WINDOW *win, const char *title)
{
	int i;

	wattrset(win, COLOR_PAIR(2));
	mvwprintw(win, 0, 1, title);

	wmove(win, 1, 1);

	for (i = 0; i < 78; i++)
		waddch(win, '\304');
}

static void print_menu(void)
{
	int i, j;
	char menu[80];
	char *ptr = menu;

	wmove(stdscr, 23, 0);

	for (j = 0; j < SCREEN_X; j++)
		waddch(stdscr, ' ');

	for (i = 0; i < ARRAY_SIZE(modules); i++)
		ptr += sprintf(ptr, "F%d: %s ", i + 1, modules[i]->name);

	mvprintw(23, 0, menu);

#ifdef CONFIG_SHOW_DATE_TIME
	mvprintw(23, 59, "%02d/%02d/20%02d - %02d:%02d:%02d",
		 bcd2dec(nvram_read(NVRAM_RTC_MONTH)),
		 bcd2dec(nvram_read(NVRAM_RTC_DAY)),
		 bcd2dec(nvram_read(NVRAM_RTC_YEAR)),
		 bcd2dec(nvram_read(NVRAM_RTC_HOURS)),
		 bcd2dec(nvram_read(NVRAM_RTC_MINUTES)),
		 bcd2dec(nvram_read(NVRAM_RTC_SECONDS)));
#endif
}

static void center(int row, const char *str)
{
	int len = strlen(str);
	int j;

	wmove(stdscr, row, 0);

	for (j = 0; j < SCREEN_X; j++)
		waddch(stdscr, ' ');

	mvprintw(row, (SCREEN_X - len) / 2, str);
}

/* FIXME: Currently unused. */
#if 0
static void header(int row, const char *str)
{
	char buf[SCREEN_X];
	char *ptr = buf;
	int i;
	int len = strlen(str) + 4;

	for (i = 0; i < (SCREEN_X - len) / 2; i++)
		ptr += sprintf(ptr, "=");

	ptr += sprintf(ptr, "[ %s ]", str);

	for (i = ((SCREEN_X - len) / 2) + len; i < SCREEN_X; i++)
		ptr += sprintf(ptr, "=");

	mvprintw(row, 0, buf);
}
#endif

static void redraw_module(void)
{
	wclear(modwin);
	modules[curwin]->redraw(modwin);
	refresh();
}

static void loop(void)
{
	int key;

	center(0, "coreinfo v0.1");

	print_menu();
	modules[curwin]->redraw(modwin);
	refresh();

	while (1) {
		key = getch();

		if (key == ERR)
			continue;

		if (key >= KEY_F(1) && key <= KEY_F(9)) {
			unsigned char ch = key - KEY_F(1);

			if (ch < ARRAY_SIZE(modules)) {
				curwin = ch;
				redraw_module();
				continue;
			}
		}

		if (modules[curwin]->handle)
			if (modules[curwin]->handle(key))
				redraw_module();
	}
}

int main(void)
{
	int i, j;

	curses_enable_serial(0);
	curses_enable_vga(1);

	initscr();

	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_WHITE, COLOR_WHITE);

	modwin = newwin(23, 80, 1, 0);

	wattrset(stdscr, COLOR_PAIR(1) | A_BOLD);
	wattrset(modwin, COLOR_PAIR(2));

	for (i = 0; i < 23; i++) {
		wmove(modwin, i - 1, 0);

		for (j = 0; j < SCREEN_X; j++)
			waddch(modwin, ' ');
	}

	refresh();

	for (i = 0; i < ARRAY_SIZE(modules); i++)
		modules[i]->init();

	loop();

	return 0;
}
