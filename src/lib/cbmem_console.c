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
 * Foundation, Inc.
 */

#include <console/console.h>
#include <console/cbmem_console.h>
#include <console/uart.h>
#include <cbmem.h>
#include <arch/early_variables.h>
#include <symbols.h>
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

static void copy_console_buffer(struct cbmem_console *old_cons_p,
	struct cbmem_console *new_cons_p);

#ifdef __PRE_RAM__
/*
 * While running from ROM, before DRAM is initialized, some area in cache as
 * ram space is used for the console buffer storage. The size and location of
 * the area are defined by the linker script with _(e)preram_cbmem_console.
 */

#else

/*
 * When running from RAM, a lot of console output is generated before CBMEM is
 * reinitialized. This static buffer is used to store that output temporarily,
 * to be concatenated with the CBMEM console buffer contents accumulated
 * during the ROM stage, once CBMEM becomes available at RAM stage.
 */

#if IS_ENABLED(CONFIG_EARLY_CBMEM_INIT)
#define STATIC_CONSOLE_SIZE 1024
#else
#define STATIC_CONSOLE_SIZE CONFIG_CONSOLE_CBMEM_BUFFER_SIZE
#endif
static u8 static_console[STATIC_CONSOLE_SIZE];
#endif

/* flags for init */
#define CBMEMC_RESET	(1<<0)
#define CBMEMC_APPEND	(1<<1)

static inline struct cbmem_console *current_console(void)
{
	return car_sync_var(cbmem_console_p);
}

static inline void current_console_set(struct cbmem_console *new_console_p)
{
	car_set_var(cbmem_console_p, new_console_p);
}

static inline void init_console_ptr(void *storage, u32 total_space, int flags)
{
	struct cbmem_console *cbm_cons_p = storage;

	if (!cbm_cons_p || total_space == 0) {
		current_console_set(NULL);
		return;
	}

	if (flags & CBMEMC_RESET) {
		cbm_cons_p->buffer_size = total_space - sizeof(struct cbmem_console);
		cbm_cons_p->buffer_cursor = 0;
	}
	if (flags & CBMEMC_APPEND) {
		struct cbmem_console *tmp_cons_p = current_console();
		if (tmp_cons_p)
			copy_console_buffer(tmp_cons_p, cbm_cons_p);
	}

	current_console_set(cbm_cons_p);
}

void cbmemc_init(void)
{
#ifdef __PRE_RAM__
	int flags = 0;

	/* If in bootblock always initialize the console first. */
	if (ENV_BOOTBLOCK)
		flags = CBMEMC_RESET;
	else if (ENV_ROMSTAGE) {
		/* Initialize console for the first time in romstage when
		 * there's no prior stage that initialized it first. */
		if (!IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK) &&
		    !IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
			flags = CBMEMC_RESET;
	} else if (ENV_VERSTAGE) {
		/*
		 * Initialize console for the first time in verstage when
		 * there is no console in bootblock. Otherwise honor the
		 * bootblock console when verstage comes right after
		 * bootblock.
		 */
		if (IS_ENABLED(CONFIG_VBOOT_STARTS_IN_BOOTBLOCK) &&
		    !IS_ENABLED(CONFIG_BOOTBLOCK_CONSOLE))
			flags = CBMEMC_RESET;
	}

	init_console_ptr(_preram_cbmem_console,
			_preram_cbmem_console_size, flags);
#else
	/*
	 * Initializing before CBMEM is available, use static buffer to store
	 * the log.
	 */
	init_console_ptr(static_console, sizeof(static_console), CBMEMC_RESET);
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
static void copy_console_buffer(struct cbmem_console *old_cons_p,
	struct cbmem_console *new_cons_p)
{
	u32 copy_size, dropped_chars;
	u32 cursor = new_cons_p->buffer_cursor;

	if (old_cons_p->buffer_cursor < old_cons_p->buffer_size)
		copy_size = old_cons_p->buffer_cursor;
	else
		copy_size = old_cons_p->buffer_size;

	if (cursor > new_cons_p->buffer_size)
		copy_size = 0;
	else if (cursor + copy_size > new_cons_p->buffer_size)
		copy_size = new_cons_p->buffer_size - cursor;

	dropped_chars = old_cons_p->buffer_cursor - copy_size;
	if (dropped_chars) {
		/* Reserve 80 chars to report overflow, if possible. */
		if (copy_size < 80)
			return;
		copy_size -= 80;
		dropped_chars += 80;
	}

	memcpy(new_cons_p->buffer_body + cursor, old_cons_p->buffer_body,
	       copy_size);

	cursor += copy_size;

	if (dropped_chars) {
		const char loss_str1[] = "\n\n*** Log truncated, ";
		const char loss_str2[] = " characters dropped. ***\n\n";

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

static void cbmemc_reinit(int is_recovery)
{
	struct cbmem_console *cbm_cons_p;
	const size_t size = CONFIG_CONSOLE_CBMEM_BUFFER_SIZE;
	int flags = CBMEMC_APPEND;

	/* No appending when no preram console available and adding for
	 * the first time. */
	if (!ENV_RAMSTAGE && _preram_cbmem_console_size == 0)
		flags = CBMEMC_RESET;

	/* Need to reset the newly added cbmem console in romstage. */
	if (ENV_ROMSTAGE)
		flags |= CBMEMC_RESET;

	/* Need to reset the newly added cbmem console in ramstage
	 * when there was no console in preram environment. */
	if (ENV_RAMSTAGE && IS_ENABLED(CONFIG_LATE_CBMEM_INIT))
		flags |= CBMEMC_RESET;

	/* If CBMEM entry already existed, old contents is not altered. */
	cbm_cons_p = cbmem_add(CBMEM_ID_CONSOLE, size);

	init_console_ptr(cbm_cons_p, size, flags);
}
ROMSTAGE_CBMEM_INIT_HOOK(cbmemc_reinit)
RAMSTAGE_CBMEM_INIT_HOOK(cbmemc_reinit)

#if IS_ENABLED(CONFIG_CONSOLE_CBMEM_DUMP_TO_UART)
void cbmem_dump_console(void)
{
	struct cbmem_console *cbm_cons_p;
	int cursor;

	cbm_cons_p = current_console();
	if (!cbm_cons_p)
		return;

	uart_init(0);
	for (cursor = 0; cursor < cbm_cons_p->buffer_cursor; cursor++)
		uart_tx_byte(0, cbm_cons_p->buffer_body[cursor]);
}
#endif
