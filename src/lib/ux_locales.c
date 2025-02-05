/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <security/vboot/misc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ux_locales.h>
#include <vb2_api.h>

#define LANG_ID_MAX 100
#define LANG_ID_LEN 3

#define PRERAM_LOCALES_VERSION_BYTE 0x01
#define PRERAM_LOCALES_NAME "preram_locales"

/* We need different delimiters to deal with the case where 'string_name' is the same as
   'localized_string'. */
#define DELIM_STR 0x00
#define DELIM_NAME 0x01

/* Mapping of different default UX local message based on message ID */
static const struct {
	const char *ux_locale_name;
	const char *ux_locale_fallback_text;
} ux_locale_msg_list[] = {
	[UX_LOCALE_MSG_MEMORY_TRAINING] = {
		"memory_training_desc",
		"Your device is finishing an update. This may take 1-2 minutes.\n"
		"Please do not turn off your device."
	},
};

/*
 * Devices which support early vga have the capability to show localized text in
 * Code Page 437 encoding. (see src/drivers/pc80/vga/vga_font_8x16.c)
 *
 * preram_locales located in CBFS is an uncompressed file located in either RO
 * or RW CBFS. It contains the localization information in the following format:
 *
 * [PRERAM_LOCALES_VERSION_BYTE]
 * [string_name_1] [\x00]
 * [language_id_1] [\x00] [localized_string_1] [\x00]
 * [language_id_2] [\x00] [localized_string_2] [\x00] ...
 * [\x01]
 * [string_name_2] [\x00] ...
 *
 * This file contains tools to locate the file and search for localized strings
 * with specific language ID.
 */

/* Cached state for map (locales_get_map) and unmap (ux_locales_unmap). */
struct preram_locales_state {
	void *data;
	size_t size;
	bool initialized;
};

static struct preram_locales_state cached_state;

void ux_locales_unmap(void)
{
	if (cached_state.initialized) {
		if (cached_state.data)
			cbfs_unmap(cached_state.data);
		cached_state.initialized = false;
		cached_state.size = 0;
		cached_state.data = NULL;
	}
}

/* Get the map address of preram_locales. */
static void *locales_get_map(size_t *size_out, bool unmap)
{
	if (cached_state.initialized) {
		*size_out = cached_state.size;
		return cached_state.data;
	}
	cached_state.initialized = true;
	cached_state.data = cbfs_ro_map(PRERAM_LOCALES_NAME,
					&cached_state.size);
	*size_out = cached_state.size;
	return cached_state.data;
}

/* Move to the next string in the data. Strings are separated by delim. */
static size_t move_next(const char *data, size_t offset, size_t size, char delim)
{
	while (offset < size && data[offset] != delim)
		offset++;
	/* If we found delim, move to the start of the next string. */
	if (offset < size)
		offset++;
	return offset;
}

/* Find the next occurrence of the specific string. Strings are separated by delim. */
static size_t search_for(const char *data, size_t offset, size_t size,
			 const char *str, char delim)
{
	while (offset < size) {
		if (!strncmp(data + offset, str, size - offset))
			return offset;
		offset = move_next(data, offset, size, delim);
	}
	return size;
}

/* Find the next occurrence of the string_name, which should always follow a DELIM_NAME. */
static inline size_t search_for_name(const char *data, size_t offset, size_t size,
				     const char *name)
{
	return search_for(data, offset, size, name, DELIM_NAME);
}

/* Find the next occurrence of the integer ID, where ID is less than 100. */
static size_t search_for_id(const char *data, size_t offset, size_t size,
			    int id)
{
	if (id >= LANG_ID_MAX)
		return offset;
	char int_to_str[LANG_ID_LEN] = {};
	snprintf(int_to_str, LANG_ID_LEN, "%d", id);
	return search_for(data, offset, size, int_to_str, DELIM_STR);
}

const char *ux_locales_get_text(enum ux_locale_msg msg_id)
{
	const char *data;
	size_t size, offset, name_offset, next_name_offset, next;
	uint32_t lang_id = 0; /* default language English (0) */
	unsigned char version;
	const char *name;
	const char *fallback_text;

	if (msg_id >= UX_LOCALE_MSG_NUM) {
		printk(BIOS_ERR, "%s: Unknown message id = %d.\n", __func__, msg_id);
		return "Trying to display an unknown message?";
	}

	name = ux_locale_msg_list[msg_id].ux_locale_name;
	fallback_text = ux_locale_msg_list[msg_id].ux_locale_fallback_text;

	data = locales_get_map(&size, false);
	if (!data || size == 0) {
		printk(BIOS_ERR, "%s: %s not found.\n", __func__,
		       PRERAM_LOCALES_NAME);
		return fallback_text;
	}

	if (CONFIG(VBOOT)) {
		/* Get the language ID from vboot API. */
		lang_id = vb2api_get_locale_id(vboot_get_context());
		/* Validity check: Language ID should smaller than LANG_ID_MAX. */
		if (lang_id >= LANG_ID_MAX) {
			printk(BIOS_WARNING, "%s: ID %d too big; fallback to 0.\n",
			       __func__, lang_id);
			lang_id = 0;
		}
	}

	printk(BIOS_INFO, "%s: Search for %s with language ID: %u\n",
	       __func__, name, lang_id);

	/* Check if the version byte is the expected version. */
	version = (unsigned char)data[0];
	if (version != PRERAM_LOCALES_VERSION_BYTE) {
		printk(BIOS_ERR, "%s: The version %u is not the expected one %u\n",
		       __func__, version, PRERAM_LOCALES_VERSION_BYTE);
		return fallback_text;
	}

	/* Search for name. Skip the version byte. */
	offset = search_for_name(data, 1, size, name);
	if (offset >= size) {
		printk(BIOS_ERR, "%s: Name %s not found.\n", __func__, name);
		return fallback_text;
	}
	name_offset = offset;

	/* Search for language ID. We should not search beyond the range of the current
	   string_name. */
	next_name_offset = move_next(data, offset, size, DELIM_NAME);
	assert(next_name_offset <= size);
	offset = search_for_id(data,  name_offset, next_name_offset, lang_id);
	/* Language ID not supported; fallback to English if the current language is not
	   English (0). */
	if (offset >= next_name_offset) {
		/* Since we only support a limited charset, it is very normal that a language
		   is not supported and we fallback here silently. */
		if (lang_id != 0)
			offset = search_for_id(data, name_offset, next_name_offset, 0);
		if (offset >= next_name_offset) {
			printk(BIOS_ERR, "%s: Neither %d nor 0 found.\n", __func__, lang_id);
			return fallback_text;
		}
	}

	/* Move to the corresponding localized_string. */
	offset = move_next(data, offset, next_name_offset, DELIM_STR);
	if (offset >= next_name_offset)
		return fallback_text;

	/* Validity check that the returned string must be NULL terminated. */
	next = move_next(data, offset, next_name_offset, DELIM_STR) - 1;
	if (next >= next_name_offset || data[next] != '\0') {
		printk(BIOS_ERR, "%s: %s is not NULL terminated.\n",
		       __func__, PRERAM_LOCALES_NAME);
		return fallback_text;
	}

	return data + offset;
}
