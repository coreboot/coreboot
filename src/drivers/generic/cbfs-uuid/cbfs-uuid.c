/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <device/device.h>
#include <smbios.h>
#include <string.h>
#include <uuid.h>

void smbios_system_set_uuid(u8 *uuid)
{
	/* Add 3 more bytes: 2 for possible CRLF and third for NULL char */
	char uuid_str[UUID_STRLEN + 3] = {0};
	uint8_t system_uuid[UUID_LEN];

	size_t uuid_len = cbfs_load("system_uuid", uuid_str, UUID_STRLEN);

	if (uuid_len >= UUID_STRLEN && uuid_len <= UUID_STRLEN + 3) {
		/* Cut off any trailing whitespace like CR or LF */
		uuid_str[UUID_STRLEN] = '\0';
		if (!parse_uuid(system_uuid, uuid_str))
			memcpy(uuid, system_uuid, UUID_LEN);
	}
}
