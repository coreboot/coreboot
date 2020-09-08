/* SPDX-License-Identifier: GPL-2.0-only */

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

extern void docpuid(uint32_t idx, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx);

void print_module_title(WINDOW *win, const char *title);

#define SCREEN_Y 25
#define SCREEN_X 80

#endif
