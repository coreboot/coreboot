/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */



#include <bootblock_common.h>


#if 0
#include "../../lib/uart8250.c"
#include "lib/div.c"

struct uart8250 uart = {
	115200
}; 
#endif

void main(unsigned long bist)
{
#if 0
	init_uart8250(CONFIG_TTYS0_BASE, &uart);
	uart8250_tx_byte(CONFIG_TTYS0_BASE, '@');

	if (boot_cpu()) {
		bootblock_cpu_init();
		bootblock_northbridge_init();
		bootblock_southbridge_init();
	}
	const char* target1 = "fallback/romstage";
	unsigned long entry;
	entry = findstage(target1);
	if (entry) call(entry, bist);

	hlt();
#endif
}

