/*
 * This file is part of the libpayload project.
 *
 * Copyright (c) 2012 Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload.h>
#include <stdint.h>

struct cbmem_console {
	uint32_t size;
	uint32_t cursor;
	uint8_t body[0];
} __attribute__ ((__packed__));

#define CURSOR_MASK ((1 << 28) - 1)
#define OVERFLOW (1 << 31)

static struct cbmem_console *cbmem_console_p;

static struct console_output_driver cbmem_console_driver =
{
	.write = &cbmem_console_write,
};

static void do_write(const void *buffer, size_t count)
{
	memcpy(cbmem_console_p->body + (cbmem_console_p->cursor & CURSOR_MASK),
	       buffer, count);
	cbmem_console_p->cursor += count;
}

void cbmem_console_init(void)
{
	cbmem_console_p = lib_sysinfo.cbmem_cons;
	if (cbmem_console_p && cbmem_console_p->size)
		console_add_output_driver(&cbmem_console_driver);
}

void cbmem_console_write(const void *buffer, size_t count)
{
	while ((cbmem_console_p->cursor & CURSOR_MASK) + count >=
	       cbmem_console_p->size) {
		size_t still_fits = cbmem_console_p->size -
				    (cbmem_console_p->cursor & CURSOR_MASK);
		do_write(buffer, still_fits);
		cbmem_console_p->cursor &= ~CURSOR_MASK;
		cbmem_console_p->cursor |= OVERFLOW;
		buffer += still_fits;
		count -= still_fits;
	}

	do_write(buffer, count);
}
