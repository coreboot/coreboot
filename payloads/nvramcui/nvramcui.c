/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#include <stdlib.h>
#include <string.h>

#include <libpayload.h>
#include <coreboot_tables.h>

#include <curses.h>
#include <menu.h>
#include <form.h>

#ifndef HOSTED
#define HOSTED 0
#endif

static int min(int x, int y)
{
	if (x < y)
		return x;
	return y;
}

static int max(int x, int y)
{
	if (x > y)
		return x;
	return y;
}

void render_form(FORM *form)
{
	int y, x, line;
	WINDOW *w = form_win(form);
	WINDOW *inner_w = form_sub(form);
	int numlines = getmaxy(w)-2;
	getyx(inner_w, y, x);
	line = y - (y % numlines);
	WINDOW *der = derwin(w, getmaxy(w)-2, getmaxx(w)-2, 1, 1);
	wclear(der);
	wrefresh(der);
	delwin(der);
	copywin(inner_w, w, line, 0, 1, 1, min(numlines, getmaxy(inner_w)-line), 68, 0);
	wmove(w, y + 1 - line, x + 1);
	wrefresh(w);
}

int main()
{
	int ch, done;
	ITEM *cur;
	
	/* coreboot data structures */
	lib_get_sysinfo();

	struct cb_cmos_option_table *opttbl = get_system_option_table();

	if (opttbl == NULL) {
		printf("Could not find coreboot option table\n");
		halt();
	}

	/* display initialization */
	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	start_color();
	leaveok(stdscr, TRUE);
	curs_set(1);

	erase();
	box(stdscr, 0, 0);
	mvaddstr(0, 2, "coreboot configuration utility");

	/* prep CMOS layout into libcurses data structures */
	
	/* determine number of options, and maximum option name length */
	int numopts=0;
	int maxlength=0;
	struct cb_cmos_entries *option = first_cmos_entry(opttbl);
	while (option) {
		if ((option->config != 'r') && (strcmp("check_sum", option->name) != 0)) {
			maxlength = max(maxlength, strlen(option->name));
			numopts++;
		}
		option = next_cmos_entry(option);
	}
	if (numopts == 0) {
		printf("NO CMOS OPTIONS FOUND. EXITING!!!");
		return 1;
	}
	FIELD **fields = malloc(sizeof(FIELD*)*(2*numopts+1));
	int i;

	/* walk over options, fetch details */
	option = first_cmos_entry(opttbl);
	for (i=0;i<numopts;i++) {
		while ((option->config == 'r') || (strcmp("check_sum", option->name) == 0)) {
			option = next_cmos_entry(option);
		}
		fields[2*i] = new_field(1, strlen(option->name), i*2, 1, 0, 0);
		set_field_buffer(fields[2*i], 0, option->name);
		field_opts_off(fields[2*i], O_ACTIVE);

		fields[2*i+1] = new_field(1, 40, i*2, maxlength+2, 0, 0);
		char *buf = NULL;
		int fail = get_option_as_string(use_nvram, opttbl, &buf, option->name);
		switch (option->config) {
		case 'h': {
			set_field_type(fields[2*i+1], TYPE_INTEGER, 0, 0, (1<<option->length)-1);
			field_opts_on(fields[2*i+1], O_BLANK);
			break;
			  }
		case 's': {
			set_max_field(fields[2*i+1], option->length/8);
			field_opts_off(fields[2*i+1], O_STATIC);
			break;
			  }
		case 'e': {
			int numvals = 0;
			struct cb_cmos_enums *cmos_enum = first_cmos_enum_of_id(opttbl, option->config_id);

			/* if invalid data in CMOS, set buf to first enum */
			if (fail && cmos_enum) {
				buf = cmos_enum->text;
			}

			while (cmos_enum) {
				numvals++;
				cmos_enum = next_cmos_enum_of_id(cmos_enum, option->config_id);
			}

			char **values = malloc(sizeof(char*)*numvals + 1);
			int cnt = 0;

			cmos_enum = first_cmos_enum_of_id(opttbl, option->config_id);
			while (cmos_enum) {
				values[cnt] = cmos_enum->text;
				cnt++;
				cmos_enum = next_cmos_enum_of_id(cmos_enum, option->config_id);
			}
			values[cnt] = NULL;
			field_opts_off(fields[2*i+1], O_EDIT);
			set_field_type(fields[2*i+1], TYPE_ENUM, values, 1, 1);
			free(values); // copied by set_field_type
			break;
			  }
		default:
			  break;
		}
		if (buf) set_field_buffer(fields[2*i+1], 0, buf);
#if HOSTED
// underline is non-trivial on VGA text
		set_field_back(fields[2*i+1], A_UNDERLINE);
#endif
		field_opts_off(fields[2*i+1], O_BLANK | O_AUTOSKIP | O_NULLOK);

		option = next_cmos_entry(option);
	}
	fields[2*numopts]=NULL;

	FORM *form = new_form(fields);
	int numlines = min(numopts*2, 16);
	WINDOW *w = newwin(numlines+2, 70, 2, 1);
	WINDOW *inner_w = newpad(numopts*2, 68);
	box(w, 0, 0);
	mvwaddstr(w, 0, 2, "Press F1 when done");
	set_form_win(form, w);
	set_form_sub(form, inner_w);
	post_form(form);

	done = 0;
	while(!done) {
		ch=getch();
		if (ch == ERR) continue;
		switch (ch) {
		case KEY_DOWN:
			form_driver(form, REQ_NEXT_FIELD);
			break;
		case KEY_UP:
			form_driver(form, REQ_PREV_FIELD);
			break;
		case KEY_LEFT:
			if (field_type(current_field(form)) == TYPE_ENUM) {
				form_driver(form, REQ_PREV_CHOICE);
			} else {
				form_driver(form, REQ_LEFT_CHAR);
			}
			break;
		case KEY_RIGHT:
			if (field_type(current_field(form)) == TYPE_ENUM) {
				form_driver(form, REQ_NEXT_CHOICE);
			} else {
				form_driver(form, REQ_RIGHT_CHAR);
			}
			break;
		case KEY_BACKSPACE:
		case '\b':
			form_driver(form, REQ_DEL_PREV);
			break;
		case KEY_DC:
			form_driver(form, REQ_DEL_CHAR);
			break;
		case KEY_F(1):
			done=1;
			break;
		default:
			form_driver(form, ch);
			break;
		}
		render_form(form);
	}

	for (i = 0; i < numopts; i++) {
		char *name = field_buffer(fields[2*i], 0);
		char *value = field_buffer(fields[2*i+1], 0);
		set_option_from_string(use_nvram, opttbl, value, name);
	}

	unpost_form(form);
	free_form(form);
	touchwin(stdscr);
	refresh();

	endwin();
	/* TODO: reboot */
	halt();
}

