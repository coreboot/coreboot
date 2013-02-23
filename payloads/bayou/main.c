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

#include "bayou.h"

static void print_banner(void)
{
	printf("\e[H\e[JBayou Payload Chooser v0.3\n");
}

int main(void)
{
	extern unsigned long _binary_builtin_lar_start;
	struct LAR *lar;

	print_banner();

	lar = openlar((void *)&_binary_builtin_lar_start);

	if (lar == NULL) {
		printf("[CHOOSER]: Unable to scan the attached LAR file\n");
		return -1;
	}

	get_configuration(lar);

	if (bayoucfg.n_entries == 0) {
		printf("[CHOOSER]:  No payloads were found in the LAR\n");
		return -1;
	}

	/*
	 * If timeout == 0xFF, then show the menu immediately.
	 * If timeout is zero, then find and run the default item immediately.
	 * If there is no default item, then show the menu.
	 * If timeout is anything else, then show a message and wait for a
	 * keystroke. If there is no keystroke in time then run the default.
	 * If there is no default then show the menu.
	 */
	if (bayoucfg.timeout != 0xFF) {
		struct payload *d = payload_get_default();

		if (d != NULL) {
			if (bayoucfg.timeout == 0)
				run_payload(d);
			else
				run_payload_timeout(d, bayoucfg.timeout);
		}
	}

	menu();

	return 0;
}
