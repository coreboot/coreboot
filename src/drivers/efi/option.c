/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <string.h>
#include <option.h>
#include <smmstore.h>

#include <Uefi/UefiBaseType.h>

#include "efivars.h"

static const EFI_GUID EficorebootNvDataGuid = {
	0xceae4c1d, 0x335b, 0x4685, { 0xa4, 0xa0, 0xfc, 0x4a, 0x94, 0xee, 0xa0, 0x85 } };

unsigned int get_uint_option(const char *name, const unsigned int fallback)
{
	struct region_device rdev;
	enum cb_err ret;
	uint32_t var;
	uint32_t size;

	if (smmstore_lookup_region(&rdev))
		return fallback;

	var = 0;
	size = sizeof(var);
	ret = efi_fv_get_option(&rdev, &EficorebootNvDataGuid, name, &var, &size);
	if (ret != CB_SUCCESS)
		return fallback;

	return var;
}

enum cb_err set_uint_option(const char *name, unsigned int value)
{
	struct region_device rdev;
	uint32_t var = value;

	if (smmstore_lookup_region(&rdev))
		return CB_CMOS_OTABLE_DISABLED;

	return efi_fv_set_option(&rdev, &EficorebootNvDataGuid, name, &var, sizeof(var));
}
