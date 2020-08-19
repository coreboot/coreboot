/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"

#if CONFIG(MODULE_BOOTLOG)

#define LINES_SHOWN 19
#define TAB_WIDTH 2

/* Globals that are used for tracking screen state */
static char *g_buf = NULL;
static s32 g_line = 0;
static s32 g_lines_count = 0;
static s32 g_max_cursor_line = 0;

/* Copied from libpayload/drivers/cbmem_console.c */
struct cbmem_console {
	u32 size;
	u32 cursor;
	u8 body[0];
} __packed;

#define CURSOR_MASK ((1 << 28) - 1)
#define OVERFLOW (1 << 31)

static u32 char_width(char c, u32 cursor, u32 screen_width)
{
	if (c == '\n') {
		return screen_width - (cursor % screen_width);
	} else if (c == '\t') {
		return TAB_WIDTH;
	} else if (isprint(c)) {
		return 1;
	}

	return 0;
}

static u32 calculate_chars_count(char *str, u32 str_len, u32 screen_width, u32 screen_height)
{
	u32 i, count = 0;

	for (i = 0; i < str_len; i++) {
		count += char_width(str[i], count, screen_width);
	}

	/* Ensure that 'count' can occupy at least the whole screen */
	if (count < screen_width * screen_height) {
		count = screen_width * screen_height;
	}

	/* Pad to line end */
	if (count % screen_width != 0) {
		count += screen_width - (count % screen_width);
	}

	return count;
}

/*
 * This method takes an input buffer and sanitizes it for display, which means:
 *  - '\n' is converted to spaces until end of line
 *  - Tabs are converted to spaces of size TAB_WIDTH
 *  - Only printable characters are preserved
 */
static int sanitize_buffer_for_display(char *str, u32 str_len, char *out, u32 out_len, u32 screen_width)
{
	u32 cursor = 0;
	u32 i;

	for (i = 0; i < str_len && cursor < out_len; i++) {
		u32 width = char_width(str[i], cursor, screen_width);
		if (width == 1) {
			out[cursor++] = str[i];
		} else if (width > 1) {
			while (width-- && cursor < out_len) {
				out[cursor++] = ' ';
			}
		}
	}

	/* Fill the rest of the out buffer with spaces */
	while (cursor < out_len) {
		out[cursor++] = ' ';
	}

	return 0;
}

static int bootlog_module_init(void)
{
	/* Make sure that lib_sysinfo is initialized */
	int ret = lib_get_sysinfo();
	if (ret) {
		return -1;
	}

	struct cbmem_console *console = phys_to_virt(lib_sysinfo.cbmem_cons);
	if (console == NULL) {
		return -1;
	}
	/* Extract console information */
	char *buffer = (char *)(&(console->body));
	u32 size = console->size;
	u32 cursor = console->cursor & CURSOR_MASK;

	/* The cursor may be bigger than buffer size with older console code */
	if (cursor >= size) {
		cursor = size - 1;
	}

	/* Calculate how much characters will be displayed on screen */
	u32 chars_count = calculate_chars_count(buffer, cursor, SCREEN_X, LINES_SHOWN);
	u32 overflow_chars_count = 0;
	if (console->cursor & OVERFLOW) {
		overflow_chars_count = calculate_chars_count(buffer + cursor,
			size - cursor, SCREEN_X, LINES_SHOWN);
	}

	/* Sanity check, chars_count must be padded to full line */
	if (chars_count % SCREEN_X || overflow_chars_count % SCREEN_X) {
		return -2;
	}

	g_lines_count = (chars_count + overflow_chars_count) / SCREEN_X;
	g_max_cursor_line = MAX(g_lines_count - 1 - LINES_SHOWN, 0);

	g_buf = malloc(chars_count);
	if (!g_buf) {
		return -3;
	}

	if (console->cursor & OVERFLOW) {
		if (sanitize_buffer_for_display(buffer + cursor, size - cursor,
				g_buf, overflow_chars_count, SCREEN_X) < 0) {
			goto err_free;
		}
	}
	if (sanitize_buffer_for_display(buffer, cursor,
					g_buf + overflow_chars_count,
					chars_count, SCREEN_X) < 0) {
		goto err_free;
	}

	/* TODO: Maybe a _cleanup hook where we call free()? */

	return 0;

err_free:
	free(g_buf);
	g_buf = NULL;
	return -4;
}

static int bootlog_module_redraw(WINDOW *win)
{
	print_module_title(win, "coreboot Bootlog");

	if (!g_buf) {
		return -1;
	}

	int x = 0, y = 0;
	char *tmp = g_buf + g_line * SCREEN_X;

	for (y = 0; y < LINES_SHOWN; y++) {
		for (x = 0; x < SCREEN_X; x++) {
			mvwaddch(win, y + 2, x, *tmp);
			tmp++;
		}

	}

	return 0;
}

static int bootlog_module_handle(int key)
{
	if (!g_buf) {
		return 0;
	}

	switch (key) {
	case KEY_DOWN:
		g_line++;
		break;
	case KEY_UP:
		g_line--;
		break;
	case KEY_NPAGE: /* Page up */
		g_line -= LINES_SHOWN;
		break;
	case KEY_PPAGE: /* Page down */
		g_line += LINES_SHOWN;
		break;
	}

	if (g_line < 0)
		g_line = 0;

	if (g_line > g_max_cursor_line)
		g_line = g_max_cursor_line;

	return 1;
}

struct coreinfo_module bootlog_module = {
	.name = "Bootlog",
	.init = bootlog_module_init,
	.redraw = bootlog_module_redraw,
	.handle = bootlog_module_handle,
};

#else

struct coreinfo_module bootlog_module = {
};

#endif
