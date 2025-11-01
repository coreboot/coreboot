/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <ctype.h>
#include <smbios.h>
#include <string.h>

#define MAX_HWID_LENGTH 0x100

/*
 * Process the HWID string according to requirements:
 * - If a colon is found, delete everything before it
 * - Trim leading/trailing whitespace
 * - Drop anything after hyphen or space
 *
 * Examples:
 * - 'hardware_id: NOCTURNE D5B-A5F-B47-H6A-A5L' --> Nocturne
 * - 'BANSHEE-UZTQ B4B-D3A-F3F-D8A-A7O' --> Banshee
 */
static void process_hwid(char *hwid)
{
	char *src = hwid;
	char *dst = hwid;
	char *colon;
	char *end;

	if (!hwid || !*hwid)
		return;

	/* Look for a colon and skip everything before it */
	colon = strchr(hwid, ':');
	if (colon) {
		src = colon + 1;  /* Start after the colon */
	}

	/* Trim leading whitespace */
	while (*src && isspace((unsigned char)*src))
		src++;

	/* Copy characters until we hit a hyphen, space, or end of string */
	while (*src && *src != '-' && !isspace((unsigned char)*src)) {
		*dst++ = *src++;
	}

	/* Null terminate */
	*dst = '\0';

	/* Trim trailing whitespace (shouldn't be any based on the logic above, but just in case) */
	end = dst - 1;
	while (end >= hwid && isspace((unsigned char)*end)) {
		*end = '\0';
		end--;
	}

	/* Normalize casing: capitalize first character, lowercase the rest */
	if (*hwid) {
		char *p = hwid;

		*p = toupper((unsigned char)*p);
		for (p = hwid + 1; *p; p++)
			*p = tolower((unsigned char)*p);
	}
}

const char *smbios_system_product_name(void)
{
	static char hwid_str[MAX_HWID_LENGTH + 1] = {0};

	/* Return cached value if already loaded */
	if (hwid_str[0] != '\0')
		return hwid_str;

	/* Try to load HWID from CBFS */
	size_t hwid_len = cbfs_load("hwid", hwid_str, MAX_HWID_LENGTH);

	if (hwid_len > 0) {
		/* Ensure null termination */
		hwid_str[hwid_len] = '\0';

		/* Process the HWID string */
		process_hwid(hwid_str);

		/* Check if we have a valid string after processing */
		if (hwid_str[0] != '\0') {
			printk(BIOS_INFO, "HWID: Using '%s' as product name\n", hwid_str);
			return hwid_str;
		}
	}

	/* Fall back to default if file not found or processing resulted in empty string */
	printk(BIOS_WARNING, "HWID: File not found or invalid, using default product name\n");
	strncpy(hwid_str, CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME, MAX_HWID_LENGTH);
	hwid_str[MAX_HWID_LENGTH] = '\0';

	return hwid_str;
}
