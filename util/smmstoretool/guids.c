/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "guids.h"

#include <stdio.h>
#include <string.h>

#include <commonlib/bsd/helpers.h>

#include "udk2017.h"
#include "utils.h"

const struct guid_alias_t known_guids[] = {
	{
		"coreboot",
		{
			0xceae4c1d, 0x335b, 0x4685,
			{ 0xa4, 0xa0, 0xfc, 0x4a, 0x94, 0xee, 0xa0, 0x85 }
		},
	},
	{
		"dasharo",
		{
			0xd15b327e, 0xff2d, 0x4fc1,
			{ 0xab, 0xf6, 0xc1, 0x2b, 0xd0, 0x8c, 0x13, 0x59 }
		},
	},
	{
		"global",
		{
			0x8be4df61, 0x93ca, 0x11d2,
			{ 0xaa, 0x0d, 0x00, 0xe0, 0x98, 0x03, 0x2b, 0x8c }
		},
	},
	{
		"secureboot",
		{
			0xd719b2cb, 0x3d3a, 0x4596,
			{ 0xa3, 0xbc, 0xda, 0xd0, 0x0e, 0x67, 0x65, 0x6f }
		},
	},
	{
		"microsoft",
		{
			0x77fa9abd, 0x0359, 0x4d32,
			{ 0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b }
		},
	},
};

const int known_guid_count = ARRAY_SIZE(known_guids);

char *format_guid(const EFI_GUID *guid, bool use_alias)
{
	if (use_alias) {
		for (int i = 0; i < known_guid_count; ++i) {
			const struct guid_alias_t *known_guid = &known_guids[i];
			if (memcmp(&known_guid->guid, guid, sizeof(*guid)) == 0)
				return strdup(known_guid->alias);
		}
	}

	char *str = xmalloc(GUID_LEN + 1);
	snprintf(str, GUID_LEN + 1,
		 "%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x",
		 guid->Data1, guid->Data2, guid->Data3,
		 guid->Data4[0], guid->Data4[1],
		 guid->Data4[2], guid->Data4[3],
		 guid->Data4[4], guid->Data4[5],
		 guid->Data4[6], guid->Data4[7]);
	return str;
}

bool parse_guid(const char str[], EFI_GUID *guid)
{
	for (int i = 0; i < known_guid_count; ++i) {
		const struct guid_alias_t *known_guid = &known_guids[i];
		if (str_eq(known_guid->alias, str)) {
			*guid = known_guid->guid;
			return true;
		}
	}

	if (strlen(str) != GUID_LEN)
		return false;

	int n = sscanf(str,
		       "%08x-%04hx-%04hx-"
		       "%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
		       &guid->Data1, &guid->Data2, &guid->Data3,
		       &guid->Data4[0], &guid->Data4[1],
		       &guid->Data4[2], &guid->Data4[3],
		       &guid->Data4[4], &guid->Data4[5],
		       &guid->Data4[6], &guid->Data4[7]);
	return n == 11;
}
