/*
 * This file is part of the bayou project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <libpayload.h>
#include <curses.h>
#include "bayou.h"

#define SCREEN_X 80
#define SCREEN_Y 25

static int menu_width = 0;
static struct payload *mpayloads[BAYOU_MAX_ENTRIES];
static int m_entries = 0;
static unsigned int selected = 0;
static WINDOW *menuwin, *status;

void create_menu(void)
{
	int i;

	for (i = 0; i < bayoucfg.n_entries; i++) {
		struct payload *p = &(bayoucfg.entries[i]);
		char *name;

		if ((p->pentry.parent != 0) ||
		    (p->pentry.flags & BPT_FLAG_NOSHOW))
			continue;

		mpayloads[m_entries++] = p;

		name = payload_get_name(p);

		if (strlen(name) > menu_width)
			menu_width = strlen(name);
	}

	menu_width += 4;

	if (menu_width < 30)
		menu_width = 30;

	menuwin = newwin(m_entries + 3, menu_width,
			 (SCREEN_Y - (m_entries + 3)) / 2,
			 (SCREEN_X - menu_width) / 2);
}

void draw_menu(void)
{
	struct payload *s;
	int i;

	wattrset(menuwin, COLOR_PAIR(3));
	wclear(menuwin);
	wborder(menuwin, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE,
		ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

	wattrset(menuwin, COLOR_PAIR(4) | A_BOLD);
	mvwprintw(menuwin, 0, (menu_width - 17) / 2, " Payload Chooser ");

	wattrset(menuwin, COLOR_PAIR(3));

	for (i = 0; i < m_entries; i++) {
		char *name = payload_get_name(mpayloads[i]);
		int col = (menu_width - (2 + strlen(name))) / 2;

		if (i == selected)
			wattrset(menuwin, COLOR_PAIR(5) | A_BOLD);
		else
			wattrset(menuwin, COLOR_PAIR(3));

		mvwprintw(menuwin, 2 + i, col, name);
	}

	s = mpayloads[selected];

	wclear(status);

	if (s->params[BAYOU_PARAM_DESC] != NULL) {
		char buf[66];
		int len = strnlen(s->params[BAYOU_PARAM_DESC], 65);

		snprintf(buf, 65, s->params[BAYOU_PARAM_DESC]);
		buf[65] = 0;

		mvwprintw(status, 0, (80 - len) / 2, buf);
	}

	wrefresh(menuwin);
	wrefresh(status);
}

void loop(void)
{
	int key;

	while (1) {
		key = getch();

		if (key == ERR)
			continue;

		if (key == KEY_DOWN)
			selected = (selected + 1) % m_entries;
		else if (key == KEY_UP)
			selected = (selected - 1) % m_entries;
		else if (key == KEY_ENTER) {
			run_payload(mpayloads[selected]);
			clear();
			refresh();
		} else
			continue;

		draw_menu();
	}
}

void menu(void)
{
	initscr();

	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	init_pair(4, COLOR_CYAN, COLOR_WHITE);
	init_pair(5, COLOR_WHITE, COLOR_RED);

	wattrset(stdscr, COLOR_PAIR(1));
	wclear(stdscr);

	status = newwin(1, 80, 24, 0);
	wattrset(status, COLOR_PAIR(2));
	wclear(status);

	refresh();

	create_menu();
	draw_menu();

	loop();
}
