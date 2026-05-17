/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/starlabs/merlin/ec.h>
#include <option.h>
#include <smbios.h>
#include <stdio.h>
#include <types.h>

#define SERIAL_NUMBER_OPTION		"serial_number"
#define SERIAL_NUMBER_MAX_LENGTH	64

/* Get the Embedded Controller firmware version */
void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision)
{
	u16 ec_version = ec_get_version();

	*ec_major_revision = ec_version >> 8;
	*ec_minor_revision = ec_version & 0xff;
}

const char *smbios_system_sku(void)
{
	return CONFIG_MAINBOARD_FAMILY;
}

static size_t copy_serial_number_prefix(char *dest, size_t dest_size)
{
	size_t i;
	size_t j = 0;

	if (!dest || !dest_size)
		return 0;

	for (i = 0; CONFIG_MAINBOARD_FAMILY[i] && j + 1 < dest_size; i++) {
		if (CONFIG_MAINBOARD_FAMILY[i] != '-')
			dest[j++] = CONFIG_MAINBOARD_FAMILY[i];
	}

	dest[j] = '\0';
	return j;
}

static bool serial_number_prefix_matches(const char *prefix, size_t prefix_len)
{
	size_t i;

	if (prefix_len > sizeof(CONFIG_MAINBOARD_SERIAL_NUMBER) - 1)
		return false;

	for (i = 0; i < prefix_len; i++) {
		if (CONFIG_MAINBOARD_SERIAL_NUMBER[i] != prefix[i])
			return false;
	}

	return true;
}

static unsigned int serial_number_fallback(void)
{
	char prefix[SERIAL_NUMBER_MAX_LENGTH];
	const char *serial_number = CONFIG_MAINBOARD_SERIAL_NUMBER;
	const size_t prefix_len = copy_serial_number_prefix(prefix, sizeof(prefix));
	unsigned int value = 0;
	size_t i;

	if (!serial_number_prefix_matches(prefix, prefix_len))
		return 0;

	for (i = prefix_len; serial_number[i]; i++) {
		if (serial_number[i] < '0' || serial_number[i] > '9')
			return 0;
		value = value * 10 + serial_number[i] - '0';
	}

	return value;
}

static int serial_number_suffix_width(const char *prefix, size_t prefix_len)
{
	const size_t fallback_len = sizeof(CONFIG_MAINBOARD_SERIAL_NUMBER) - 1;

	if (!serial_number_prefix_matches(prefix, prefix_len) || fallback_len <= prefix_len)
		return 5;

	return fallback_len - prefix_len;
}

const char *smbios_mainboard_serial_number(void)
{
	static char serial_number[SERIAL_NUMBER_MAX_LENGTH];
	static bool serial_number_loaded;

	if (!serial_number_loaded) {
		char prefix[SERIAL_NUMBER_MAX_LENGTH];
		const size_t prefix_len = copy_serial_number_prefix(prefix, sizeof(prefix));
		const int suffix_width = serial_number_suffix_width(prefix, prefix_len);
		const unsigned int suffix = get_uint_option(SERIAL_NUMBER_OPTION,
							    serial_number_fallback());

		snprintf(serial_number, sizeof(serial_number), "%s%0*u", prefix, suffix_width,
			 suffix);
		serial_number_loaded = true;
	}

	return serial_number;
}

u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAGS_HOSTING_BOARD | SMBIOS_FEATURE_FLAGS_REPLACEABLE;
}

const char *smbios_chassis_version(void)
{
	return smbios_mainboard_version();
}

const char *smbios_chassis_serial_number(void)
{
	return smbios_mainboard_serial_number();
}
