/*
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <libpayload.h>
#include <stdint.h>

struct cbmem_console {
	uint32_t size;
	uint32_t cursor;
	uint8_t body[0];
} __attribute__ ((__packed__));

static struct cbmem_console *cbmem_console_p;

static struct console_output_driver cbmem_console_driver =
{
	.putchar = &cbmem_console_putc
};

void cbmem_console_init(void)
{
	cbmem_console_p = lib_sysinfo.cbmem_cons;
	if (cbmem_console_p)
		console_add_output_driver(&cbmem_console_driver);
}

void cbmem_console_putc(unsigned int data)
{
	// Bail out if the buffer is full.
	if (cbmem_console_p->cursor >= cbmem_console_p->size)
		return;

	cbmem_console_p->body[cbmem_console_p->cursor++] = data;
}
