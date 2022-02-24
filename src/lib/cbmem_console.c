/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/cbmem_console.h>
#include <console/console.h>
#include <console/uart.h>
#include <cbmem.h>
#include <symbols.h>
#include <types.h>

/*
 * Structure describing console buffer. It is overlaid on a flat memory area,
 * with body covering the extent of the memory. Once the buffer is full,
 * output will wrap back around to the start of the buffer. The high bit of the
 * cursor field gets set to indicate that this happened. If the underlying
 * storage allows this, the buffer will persist across multiple boots and append
 * to the previous log.
 *
 * NOTE: These are known implementations accessing this console that need to be
 * updated in case of structure/API changes:
 *
 * cbmem:	[coreboot]/src/util/cbmem/cbmem.c
 * libpayload:	[coreboot]/payloads/libpayload/drivers/cbmem_console.c
 * coreinfo:	[coreboot]/payloads/coreinfo/bootlog_module.c
 * Linux:	drivers/firmware/google/memconsole-coreboot.c
 * SeaBIOS:	src/firmware/coreboot.c
 * GRUB:	grub-core/term/i386/coreboot/cbmemc.c
 */
struct cbmem_console {
	u32 size;
	u32 cursor;
	u8  body[0];
}  __packed;

#define MAX_SIZE (1 << 28)	/* can't be changed without breaking readers! */
#define CURSOR_MASK (MAX_SIZE - 1)	/* bits 31-28 are reserved for flags */
#define OVERFLOW (1UL << 31)		/* set if in ring-buffer mode */
_Static_assert(CONFIG_CONSOLE_CBMEM_BUFFER_SIZE <= MAX_SIZE,
	"cbmem_console format cannot support buffers larger than 256MB!");

static struct cbmem_console *current_console;

static bool console_paused;

/*
 * While running from ROM, before DRAM is initialized, some area in cache as
 * RAM space is used for the console buffer storage. The size and location of
 * the area are defined by the linker script with _(e)preram_cbmem_console.
 *
 * When running from RAM, some console output is generated before CBMEM is
 * reinitialized. This static buffer is used to store that output temporarily,
 * to be concatenated with the CBMEM console buffer contents accumulated
 * during the ROM stage, once CBMEM becomes available at RAM stage.
 */

#define STATIC_CONSOLE_SIZE 1024
static u8 static_console[STATIC_CONSOLE_SIZE];

static int buffer_valid(struct cbmem_console *cbm_cons_p, u32 total_space)
{
	return (cbm_cons_p->cursor & CURSOR_MASK) < cbm_cons_p->size &&
	       cbm_cons_p->size <= MAX_SIZE &&
	       cbm_cons_p->size == total_space - sizeof(struct cbmem_console);
}

static void init_console_ptr(void *storage, u32 total_space)
{
	struct cbmem_console *cbm_cons_p = storage;

	if (!cbm_cons_p || total_space <= sizeof(struct cbmem_console)) {
		current_console = NULL;
		return;
	}

	if (!buffer_valid(cbm_cons_p, total_space)) {
		cbm_cons_p->size = total_space - sizeof(struct cbmem_console);
		cbm_cons_p->cursor = 0;
	}

	current_console = cbm_cons_p;
}

void cbmemc_init(void)
{
	if (ENV_ROMSTAGE_OR_BEFORE) {
		/* Pre-RAM environments use special buffer placed by linker script. */
		init_console_ptr(_preram_cbmem_console, REGION_SIZE(preram_cbmem_console));
	} else if (ENV_SMM) {
		void *cbmemc = NULL;
		size_t cbmemc_size = 0;

		smm_get_cbmemc_buffer(&cbmemc, &cbmemc_size);

		init_console_ptr(cbmemc, cbmemc_size);
	} else {
		/* Post-RAM uses static (BSS) buffer before CBMEM is reinitialized. */
		init_console_ptr(static_console, sizeof(static_console));
	}
}

void cbmemc_tx_byte(unsigned char data)
{
	if (!current_console || !current_console->size || console_paused)
		return;

	u32 flags = current_console->cursor & ~CURSOR_MASK;
	u32 cursor = current_console->cursor & CURSOR_MASK;

	current_console->body[cursor++] = data;
	if (cursor >= current_console->size) {
		cursor = 0;
		flags |= OVERFLOW;
	}

	current_console->cursor = flags | cursor;
}

/*
 * Copy the current console buffer (either from the cache as RAM area or from
 * the static buffer, pointed at by src_cons_p) into the newly initialized CBMEM
 * console. The use of cbmemc_tx_byte() ensures that all special cases for the
 * target console (e.g. overflow) will be handled. If there had been an
 * overflow in the source console, log a message to that effect.
 */
static void copy_console_buffer(struct cbmem_console *src_cons_p)
{
	u32 c;

	if (!src_cons_p)
		return;

	if (src_cons_p->cursor & OVERFLOW) {
		const char overflow_warning[] = "\n*** Pre-CBMEM " ENV_STRING
			" console overflowed, log truncated! ***\n";
		for (c = 0; c < sizeof(overflow_warning) - 1; c++)
			cbmemc_tx_byte(overflow_warning[c]);
		for (c = src_cons_p->cursor & CURSOR_MASK;
		     c < src_cons_p->size; c++)
			cbmemc_tx_byte(src_cons_p->body[c]);
	}

	for (c = 0; c < (src_cons_p->cursor & CURSOR_MASK); c++)
		cbmemc_tx_byte(src_cons_p->body[c]);

	/* Invalidate the source console, so it will be reinitialized on the
	   next reboot. Otherwise, we might copy the same bytes again. */
	src_cons_p->size = 0;
}

void cbmemc_copy_in(void *buffer, size_t size)
{
	struct cbmem_console *previous = (void *)buffer;

	if (!buffer_valid(previous, size))
		return;

	copy_console_buffer(previous);
}

static void cbmemc_reinit(int is_recovery)
{
	const size_t size = CONFIG_CONSOLE_CBMEM_BUFFER_SIZE;
	/* If CBMEM entry already existed, old contents are not altered. */
	struct cbmem_console *cbmem_cons_p = cbmem_add(CBMEM_ID_CONSOLE, size);
	struct cbmem_console *previous_cons_p = current_console;

	init_console_ptr(cbmem_cons_p, size);
	copy_console_buffer(previous_cons_p);
}

/* Run the romstage hook early so that the console region is one of the earliest created, and
   therefore more likely to stay in the same place even across different boot modes where some
   other regions may sometimes not get created (e.g. RW_MCACHE in vboot recovery mode). */
ROMSTAGE_CBMEM_INIT_HOOK_EARLY(cbmemc_reinit)
RAMSTAGE_CBMEM_INIT_HOOK(cbmemc_reinit)
POSTCAR_CBMEM_INIT_HOOK(cbmemc_reinit)

#if CONFIG(CONSOLE_CBMEM_DUMP_TO_UART)
void cbmem_dump_console_to_uart(void)
{
	u32 cursor;
	unsigned int console_index;

	if (!current_console)
		return;

	console_index = get_uart_for_console();

	uart_init(console_index);
	if (current_console->cursor & OVERFLOW) {
		for (cursor = current_console->cursor & CURSOR_MASK;
		     cursor < current_console->size; cursor++) {
			if (BIOS_LOG_IS_MARKER(current_console->body[cursor]))
				continue;
			if (current_console->body[cursor] == '\n')
				uart_tx_byte(console_index, '\r');
			uart_tx_byte(console_index, current_console->body[cursor]);
		}
	}
	for (cursor = 0; cursor < (current_console->cursor & CURSOR_MASK); cursor++) {
		if (BIOS_LOG_IS_MARKER(current_console->body[cursor]))
			continue;
		if (current_console->body[cursor] == '\n')
			uart_tx_byte(console_index, '\r');
		uart_tx_byte(console_index, current_console->body[cursor]);
	}
}
#endif

void cbmem_dump_console(void)
{
	u32 cursor;
	if (!current_console)
		return;

	console_paused = true;

	if (current_console->cursor & OVERFLOW)
		for (cursor = current_console->cursor & CURSOR_MASK;
		     cursor < current_console->size; cursor++)
			if (!BIOS_LOG_IS_MARKER(current_console->body[cursor]))
				do_putchar(current_console->body[cursor]);
	for (cursor = 0; cursor < (current_console->cursor & CURSOR_MASK); cursor++)
		if (!BIOS_LOG_IS_MARKER(current_console->body[cursor]))
			do_putchar(current_console->body[cursor]);

	console_paused = false;
}
