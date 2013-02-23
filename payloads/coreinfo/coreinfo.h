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

#ifndef COREINFO_H_
#define COREINFO_H_

#include <libpayload.h>
#include <config.h>
#include <curses.h>

struct coreinfo_module {
	char name[15];
	int (*init) (void);
	int (*redraw) (WINDOW *);
	int (*handle) (int);
};

extern void docpuid(int, unsigned long *, unsigned long *, unsigned long *,
		    unsigned long *);

void print_module_title(WINDOW *win, const char *title);

#endif
