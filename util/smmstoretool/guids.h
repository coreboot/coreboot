/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SMMSTORETOOL__GUIDS_H__
#define SMMSTORETOOL__GUIDS_H__

#include <stdbool.h>

#include "udk2017.h"

#define GUID_LEN 35

struct guid_alias_t {
	const char *alias;
	EFI_GUID guid;
};

extern const struct guid_alias_t known_guids[];

extern const int known_guid_count;

char *format_guid(const EFI_GUID *guid, bool use_alias);

bool parse_guid(const char str[], EFI_GUID *guid);

#endif // SMMSTORETOOL__GUIDS_H__
