/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/car.h>
#include <string.h>

/*
 * Structure describing console buffer. It is overlaid on a flat memory area,
 * whith buffer_body covering the extent of the memory. Once the buffer is
 * full, the cursor keeps going but the data is dropped on the floor. This
 * allows to tell how much data was lost in the process.
 */
struct cbmem_console {
	u32 buffer_size;
	u32 buffer_cursor;
	u8  buffer_body[0];
}  __attribute__ ((__packed__));

#ifdef __PRE_RAM__
/*
 * While running from ROM, before DRAM is initialized, some area in cache as
 * ram space is used for the console buffer storage. The size and location of
 * the area are defined in the config.
 */

static struct cbmem_console car_cbmem_console CAR_CBMEM;
#define cbmem_console_p (&car_cbmem_console)

/*
 * Once DRAM is initialized and the cache as ram mode is disabled, while still
 * running from ROM, the console buffer in the cache as RAM area becomes
 * unavailable.
 *
 * By this time the console log buffer is already available in
 * CBMEM. The location at 0x600 is used as the redirect pointer allowing to
 * find out where the actual console log buffer is.
 */
#define CBMEM_CONSOLE_REDIRECT (*((struct cbmem_console **)0x600))
#else

/*
 * When running from RAM, a lot of console output is generated before CBMEM is
 * reinitialized. This static buffer is used to store that output temporarily,
 * to be concatenated with the CBMEM console buffer contents accumulated
 * during the ROM stage, once CBMEM becomes available at RAM stage.
 */
static u8 static_console[40000];
static struct cbmem_console *cbmem_console_p;
#endif

void cbmemc_init(void)
{
#ifdef __PRE_RAM__
	cbmem_console_p->buffer_size = CONFIG_CONSOLE_CAR_BUFFER_SIZE -
		sizeof(struct cbmem_console);
#else
	/*
	 * Initializing before CBMEM is available, use static buffer to store
	 * the log.
	 */
	cbmem_console_p = (struct cbmem_console *) static_console;
	cbmem_console_p->buffer_size = sizeof(static_console) -
		sizeof(struct cbmem_console);
#endif
	cbmem_console_p->buffer_cursor = 0;
}

void cbmemc_tx_byte(unsigned char data)
{
	struct cbmem_console *cbm_cons_p = cbmem_console_p;
	u32 cursor;
#ifdef __PRE_RAM__
	/*
	 * This check allows to tell if the cache as RAM mode has been exited
	 * or not. If it has been exited, the real memory is being used
	 * (resulting in the variable on the stack located below
	 * DCACHE_RAM_BASE), use the redirect pointer to find out where the
	 * actual console buffer is.
	 */
	if ((uintptr_t)&cursor < (uintptr_t)&car_cbmem_console)
		cbm_cons_p = CBMEM_CONSOLE_REDIRECT;
#endif
	if (!cbm_cons_p)
		return;

	cursor = cbm_cons_p->buffer_cursor++;
	if (cursor < cbm_cons_p->buffer_size)
		cbm_cons_p->buffer_body[cursor] = data;
}

/*
 * Copy the current console buffer (either from the cache as RAM area, or from
 * the static buffer, pointed at by cbmem_console_p) into the CBMEM console
 * buffer space (pointed at by new_cons_p), concatenating the copied data with
 * the CBMEM console buffer contents.
 *
 * If there is overflow - add to the destination area a string, reporting the
 * overflow and the number of dropped charactes.
 */
static void copy_console_buffer(struct cbmem_console *new_cons_p)
{
	u32 copy_size;
	u32 cursor = new_cons_p->buffer_cursor;
	int overflow = cbmem_console_p->buffer_cursor >
		cbmem_console_p->buffer_size;

	copy_size = overflow ?
		cbmem_console_p->buffer_size : cbmem_console_p->buffer_cursor;

	memcpy(new_cons_p->buffer_body + cursor,
	       cbmem_console_p->buffer_body,
	       copy_size);

	cursor += copy_size;

	if (overflow) {
		const char loss_str1[] = "\n\n*** Log truncated, ";
		const char loss_str2[] = " characters dropped. ***\n\n";
		u32 dropped_chars = cbmem_console_p->buffer_cursor - copy_size;

		/*
		 * When running from ROM sprintf is not available, a simple
		 * itoa implementation is used instead.
		 */
		int got_first_digit = 0;

		/* Way more than possible number of dropped characters. */
		u32 mult = 100000;

		strcpy((char *)new_cons_p->buffer_body + cursor, loss_str1);
		cursor += sizeof(loss_str1) - 1;

		while (mult) {
			int digit = dropped_chars / mult;
			if (got_first_digit || digit) {
				new_cons_p->buffer_body[cursor++] = digit + '0';
				dropped_chars %= mult;
				/* Excessive, but keeps it simple */
				got_first_digit = 1;
			}
			mult /= 10;
		}

		strcpy((char *)new_cons_p->buffer_body + cursor, loss_str2);
		cursor += sizeof(loss_str2) - 1;
	}
	new_cons_p->buffer_cursor = cursor;
}

void cbmemc_reinit(void)
{
	struct cbmem_console *cbm_cons_p;

#ifdef __PRE_RAM__
	cbm_cons_p = cbmem_add(CBMEM_ID_CONSOLE,
			       CONFIG_CONSOLE_CBMEM_BUFFER_SIZE);
	if (!cbm_cons_p) {
		CBMEM_CONSOLE_REDIRECT = NULL;
		return;
	}

	cbm_cons_p->buffer_size = CONFIG_CONSOLE_CBMEM_BUFFER_SIZE -
		sizeof(struct cbmem_console);

	cbm_cons_p->buffer_cursor = 0;

	copy_console_buffer(cbm_cons_p);

	CBMEM_CONSOLE_REDIRECT = cbm_cons_p;
#else
	cbm_cons_p = cbmem_find(CBMEM_ID_CONSOLE);

	if (!cbm_cons_p)
		return;

	copy_console_buffer(cbm_cons_p);

	cbmem_console_p = cbm_cons_p;
#endif
}
