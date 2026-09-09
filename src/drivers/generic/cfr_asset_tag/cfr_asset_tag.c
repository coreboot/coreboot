/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <device/device.h>
#include <option.h>
#include <smbios.h>
#include <smmstore.h>
#include <drivers/option/cfr_frontend.h>
#include <drivers/efi/efivars.h>
#include <Uefi/UefiBaseType.h>

#define OPTION_NAME "asset_tag"

/* The length of sm_object::default_value (minus one) seems to define the maximum length allowed.
 * This is a limitation in UefiPayloadPkg/CfrSetupMenuDxe.
 */
#define DEFAULT_VALUE "[no asset tag                                  ]"

static const EFI_GUID EficorebootNvDataGuid = {
	0xceae4c1d, 0x335b, 0x4685, { 0xa4, 0xa0, 0xfc, 0x4a, 0x94, 0xee, 0xa0, 0x85 } };

const struct sm_object cfr_asset_tag = SM_DECLARE_VARCHAR({
	.opt_name	= OPTION_NAME,
	.ui_name	= "Asset tag",
	.ui_helptext	= "Freely defineable asset tag, e.g. for managing inventory in your organisation.",
	.default_value	= DEFAULT_VALUE,
});

#define MAX_VALUE_SIZE 128

static void ucs2_to_ascii_inplace(char *buffer, size_t size)
{
	for (size_t i = 0; i < size; i += 2) {
		char lo = buffer[i];
		char hi = buffer[i + 1];

		buffer[i / 2] = hi ? '?' : lo;
	}
	buffer[size / 2] = 0;
}

static const char *get_varchar_option(const char *name, const char *fallback)
{
	static char buffer[MAX_VALUE_SIZE];

	struct region_device rdev;
	if (smmstore_lookup_region(&rdev))
		return fallback;

	uint32_t size = sizeof(buffer) - 1;
	enum cb_err ret = efi_fv_get_option(&rdev, &EficorebootNvDataGuid, name, buffer, &size);
	if (ret != CB_SUCCESS)
		return fallback;

	ucs2_to_ascii_inplace(buffer, size);

	return buffer;
}

const char *smbios_mainboard_asset_tag(void)
{
	return get_varchar_option(OPTION_NAME, DEFAULT_VALUE);
}
