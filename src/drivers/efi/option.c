/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <smmstore.h>

#include <Uefi/UefiBaseType.h>

#include "efivars.h"
#include "option.h"

static const EFI_GUID EficorebootNvDataGuid = {
	0xceae4c1d, 0x335b, 0x4685, { 0xa4, 0xa0, 0xfc, 0x4a, 0x94, 0xee, 0xa0, 0x85 } };

enum cb_err efi_option_initialize_store(void)
{
	struct region_device rdev;

	if (smmstore_lookup_region(&rdev))
		return CB_CMOS_OTABLE_DISABLED;

	return efi_fv_initialize(&rdev, SMM_BLOCK_SIZE);
}

enum cb_err efi_option_get_uint(const char *name, uint32_t *value)
{
	struct region_device rdev;
	enum cb_err ret;
	uint32_t size;

	if (!name || !value)
		return CB_ERR_ARG;

	if (smmstore_lookup_region(&rdev))
		return CB_CMOS_OTABLE_DISABLED;

	size = sizeof(*value);
	ret = efi_fv_get_option(&rdev, &EficorebootNvDataGuid, name, value, &size);
	if (ret != CB_SUCCESS)
		return ret;
	if (size != sizeof(*value))
		return CB_EFI_ACCESS_ERROR;

	return CB_SUCCESS;
}

enum cb_err efi_option_set_uint(const char *name, uint32_t value)
{
	struct region_device rdev;
	uint32_t var = value;

	if (!name)
		return CB_ERR_ARG;

	if (smmstore_lookup_region(&rdev))
		return CB_CMOS_OTABLE_DISABLED;

	return efi_fv_set_option(&rdev, &EficorebootNvDataGuid, name, &var, sizeof(var));
}

unsigned int get_uint_option(const char *name, const unsigned int fallback)
{
	uint32_t value;

	if (efi_option_get_uint(name, &value) != CB_SUCCESS)
		return fallback;

	return value;
}

enum cb_err set_uint_option(const char *name, unsigned int value)
{
	return efi_option_set_uint(name, value);
}
