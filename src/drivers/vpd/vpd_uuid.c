/* SPDX-License-Identifier: BSD-3-Clause */

#include <smbios.h>
#include <string.h>

#include "vpd.h"

#define VPD_KEY_SYSTEM_UUID "system_uuid"
#define UUID_SIZE 16

void smbios_system_set_uuid(u8 *uuid)
{
	int uuid_size;
	const void *p = vpd_find(VPD_KEY_SYSTEM_UUID, &uuid_size, VPD_RO);

	/* If no valid UUID was found, return an all-zero UUID */
	if (!p || uuid_size != UUID_SIZE)
		memset(uuid, 0, UUID_SIZE);
	else
		memcpy(uuid, p, UUID_SIZE);
}
