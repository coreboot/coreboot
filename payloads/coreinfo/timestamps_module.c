/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"
#include <commonlib/timestamp_serialized.h>

#if CONFIG(MODULE_TIMESTAMPS)

#define LINES_SHOWN 19
#define TAB_WIDTH 2

/* Globals that are used for tracking screen state */
static char *g_buf;
static s32 g_line;
static s32 g_lines_count;
static s32 g_max_cursor_line;

static unsigned long tick_freq_mhz;

static const char *timestamp_name(uint32_t id)
{
	for (size_t i = 0; i < ARRAY_SIZE(timestamp_ids); i++) {
		if (timestamp_ids[i].id == id)
			return timestamp_ids[i].name;
	}

	return "<unknown>";
}

static void timestamp_set_tick_freq(unsigned long table_tick_freq_mhz)
{
	tick_freq_mhz = table_tick_freq_mhz;

	/* Honor table frequency. */
	if (tick_freq_mhz)
		return;

	tick_freq_mhz = lib_sysinfo.cpu_khz / 1000;

	if (!tick_freq_mhz) {
		fprintf(stderr, "Cannot determine timestamp tick frequency.\n");
		exit(1);
	}
}

static u64 arch_convert_raw_ts_entry(u64 ts)
{
	return ts / tick_freq_mhz;
}

static u32 char_width(char c, u32 cursor, u32 screen_width)
{
	if (c == '\n')
		return screen_width - (cursor % screen_width);
	else if (c == '\t')
		return TAB_WIDTH;
	else if (isprint(c))
		return 1;

	return 0;
}

static u32 calculate_chars_count(char *str, u32 str_len, u32 screen_width,
		u32 screen_height)
{
	u32 i, count = 0;

	for (i = 0; i < str_len; i++)
		count += char_width(str[i], count, screen_width);

	/* Ensure that 'count' can occupy at least the whole screen */
	if (count < screen_width * screen_height)
		count = screen_width * screen_height;

	/* Pad to line end */
	if (count % screen_width != 0)
		count += screen_width - (count % screen_width);

	return count;
}

/*
 * This method takes an input buffer and sanitizes it for display, which means:
 *  - '\n' is converted to spaces until end of line
 *  - Tabs are converted to spaces of size TAB_WIDTH
 *  - Only printable characters are preserved
 */
static int sanitize_buffer_for_display(char *str, u32 str_len, char *out,
		u32 out_len, u32 screen_width)
{
	u32 cursor = 0;
	u32 i;

	for (i = 0; i < str_len && cursor < out_len; i++) {
		u32 width = char_width(str[i], cursor, screen_width);

		if (width == 1)
			out[cursor++] = str[i];
		else if (width > 1)
			while (width-- && cursor < out_len)
				out[cursor++] = ' ';
	}

	/* Fill the rest of the out buffer with spaces */
	while (cursor < out_len)
		out[cursor++] = ' ';

	return 0;
}

static uint64_t timestamp_print_entry(char *buffer, size_t size, uint32_t *cur,
		uint32_t id, uint64_t stamp, uint64_t prev_stamp)
{
	const char *name;
	uint64_t step_time;

	name = timestamp_name(id);
	step_time = arch_convert_raw_ts_entry(stamp - prev_stamp);

	*cur += snprintf(buffer + *cur, size, "%4d: %-45s", id, name);
	*cur += snprintf(buffer + *cur, size, "%llu",
			arch_convert_raw_ts_entry(stamp));
	if (prev_stamp) {
		*cur += snprintf(buffer + *cur, size, " (");
		*cur += snprintf(buffer + *cur, size, "%llu", step_time);
		*cur += snprintf(buffer + *cur, size, ")");
	}
	*cur += snprintf(buffer + *cur, size, "\n");

	return step_time;
}

static int timestamps_module_init(void)
{
	/* Make sure that lib_sysinfo is initialized */
	int ret = lib_get_sysinfo();

	if (ret)
		return -1;

	struct timestamp_table *timestamps = phys_to_virt(lib_sysinfo.tstamp_table);

	if (timestamps == NULL)
		return -1;

	/* Extract timestamps information */
	u64 base_time = timestamps->base_time;
	u16 max_entries = timestamps->max_entries;
	u32 n_entries = timestamps->num_entries;

	timestamp_set_tick_freq(timestamps->tick_freq_mhz);

	char *buffer;
	u32 buff_cur = 0;
	uint64_t prev_stamp;
	uint64_t total_time;

	/* Allocate a buffer big enough to contain all of the possible
	 * entries plus the other information (number entries, total time). */
	buffer = malloc((max_entries + 4) * SCREEN_X * sizeof(char));

	if (buffer == NULL)
		return -3;

	/* Write the content */
	buff_cur += snprintf(buffer, SCREEN_X, "%d entries total:\n\n",
			n_entries);

	prev_stamp = 0;
	timestamp_print_entry(buffer, SCREEN_X, &buff_cur, 0, base_time,
			prev_stamp);
	prev_stamp = base_time;

	total_time = 0;
	for (u32 i = 0; i < n_entries; i++) {
		uint64_t stamp;
		const struct timestamp_entry *tse = &timestamps->entries[i];

		stamp = tse->entry_stamp + base_time;
		total_time += timestamp_print_entry(buffer, SCREEN_X,
				&buff_cur, tse->entry_id, stamp, prev_stamp);
		prev_stamp = stamp;
	}

	buff_cur += snprintf(buffer + buff_cur, SCREEN_X, "\nTotal Time: ");
	buff_cur += snprintf(buffer + buff_cur, SCREEN_X, "%llu", total_time);
	buff_cur += snprintf(buffer + buff_cur, SCREEN_X, "\n");

	/* Calculate how many characters will be displayed on screen */
	u32 chars_count = calculate_chars_count(buffer, buff_cur + 1,
			SCREEN_X, LINES_SHOWN);

	/* Sanity check, chars_count must be padded to full line */
	if (chars_count % SCREEN_X != 0) {
		free(buffer);
		return -2;
	}

	g_lines_count = chars_count / SCREEN_X;
	g_max_cursor_line = MAX(g_lines_count - 1 - LINES_SHOWN, 0);

	g_buf = malloc(chars_count);
	if (!g_buf) {
		free(buffer);
		return -3;
	}

	if (sanitize_buffer_for_display(buffer, buff_cur + 1, g_buf,
				chars_count, SCREEN_X) < 0) {
		free(buffer);
		free(g_buf);
		g_buf = NULL;
		return -4;
	}

	free(buffer);

	return 0;
}

static int timestamps_module_redraw(WINDOW *win)
{
	print_module_title(win, "coreboot Timestamps");

	if (!g_buf)
		return -1;

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

static int timestamps_module_handle(int key)
{
	if (!g_buf)
		return 0;

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

struct coreinfo_module timestamps_module = {
	.name = "Timestamps",
	.init = timestamps_module_init,
	.redraw = timestamps_module_redraw,
	.handle = timestamps_module_handle,
};

#else

struct coreinfo_module timestamps_module = {
};

#endif
