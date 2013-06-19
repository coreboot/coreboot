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
#include <arch/early_variables.h>
#include <string.h>

/*
 * Structure describing console buffer. It is overlaid on a flat memory area,
 * with buffer_body covering the extent of the memory. Once the buffer is
 * full, the cursor keeps going but the data is dropped on the floor. This
 * allows to tell how much data was lost in the process.
 */
struct cbmem_console {
	u32 buffer_size;
	u32 buffer_cursor;
	u8  buffer_body[0];
}  __attribute__ ((__packed__));

static struct cbmem_console *cbmem_console_p CAR_GLOBAL;

#ifdef __PRE_RAM__
/*
 * While running from ROM, before DRAM is initialized, some area in cache as
 * ram space is used for the console buffer storage. The size and location of
 * the area are defined in the config.
 */

static struct cbmem_console car_cbmem_console CAR_CBMEM;

#else

/*
 * When running from RAM, a lot of console output is generated before CBMEM is
 * reinitialized. This static buffer is used to store that output temporarily,
 * to be concatenated with the CBMEM console buffer contents accumulated
 * during the ROM stage, once CBMEM becomes available at RAM stage.
 */
static u8 static_console[40000];
#endif

static inline struct cbmem_console *current_console(void)
{
	return car_get_var(cbmem_console_p);
}

static inline void current_console_set(struct cbmem_console *new_console_p)
{
	car_set_var(cbmem_console_p, new_console_p);
}

static inline void init_console_ptr(void *storage, u32 total_space)
{
	struct cbmem_console *cbm_cons_p = storage;

	/* Initialize the cache-as-ram pointer and underlying structure. */
	car_set_var(cbmem_console_p, cbm_cons_p);
	cbm_cons_p->buffer_size = total_space - sizeof(struct cbmem_console);
	cbm_cons_p->buffer_cursor = 0;
}

void cbmemc_init(void)
{
#ifdef __PRE_RAM__
	init_console_ptr(&car_cbmem_console, CONFIG_CONSOLE_CAR_BUFFER_SIZE);
#else
	/*
	 * Initializing before CBMEM is available, use static buffer to store
	 * the log.
	 */
	init_console_ptr(static_console, sizeof(static_console));
#endif
}

void cbmemc_tx_byte(unsigned char data)
{
	struct cbmem_console *cbm_cons_p = current_console();
	u32 cursor;

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
 * overflow and the number of dropped characters.
 */
static void copy_console_buffer(struct cbmem_console *new_cons_p)
{
	u32 copy_size;
	u32 cursor = new_cons_p->buffer_cursor;
	struct cbmem_console *old_cons_p;
	int overflow;

	old_cons_p = current_console();

	overflow = old_cons_p->buffer_cursor > old_cons_p->buffer_size;

	copy_size = overflow ?
		old_cons_p->buffer_size : old_cons_p->buffer_cursor;

	memcpy(new_cons_p->buffer_body + cursor, old_cons_p->buffer_body,
	       copy_size);

	cursor += copy_size;

	if (overflow) {
		const char loss_str1[] = "\n\n*** Log truncated, ";
		const char loss_str2[] = " characters dropped. ***\n\n";
		u32 dropped_chars = old_cons_p->buffer_cursor - copy_size;

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
	struct cbmem_console *cbm_cons_p = NULL;

#ifndef __PRE_RAM__
	cbm_cons_p = cbmem_find(CBMEM_ID_CONSOLE);
#endif

	if (!cbm_cons_p) {
		cbm_cons_p = cbmem_add(CBMEM_ID_CONSOLE,
							CONFIG_CONSOLE_CBMEM_BUFFER_SIZE);

		if (!cbm_cons_p) {
			current_console_set(NULL);
			return;
		}

		cbm_cons_p->buffer_size = CONFIG_CONSOLE_CBMEM_BUFFER_SIZE -
			sizeof(struct cbmem_console);

		cbm_cons_p->buffer_cursor = 0;
	}

	copy_console_buffer(cbm_cons_p);

	current_console_set(cbm_cons_p);
}

/* Call cbmemc_reinit() at CAR migration time. */
CAR_MIGRATE(cbmemc_reinit)
