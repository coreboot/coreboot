/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <fsp/fsp_debug_event.h>
#include <fsp/util.h>

static const uint8_t fsp_string_type_guid[16] = {
	0x80, 0x10, 0xd1, 0x92, 0x6f, 0x49, 0x95, 0x4d,
	0xbe, 0x7e, 0x03, 0x74, 0x88, 0x38, 0x2b, 0x0a
};

static efi_return_status_t print_fsp_string_data(const efi_status_code_data_t *data)
{
	printk(BIOS_SPEW, "%s", ((efi_status_code_string_data *) data)->String.Ascii);

	return FSP_SUCCESS;
}

efi_return_status_t fsp_debug_event_handler(efi_status_code_type_t ignored1,
	efi_status_code_value_t ignored2, efi_uint32_t ignored3, efi_guid_t *ignored4,
	efi_status_code_data_t *data)
{
	if (!fsp_guid_compare((uint8_t *)&(data->Type), fsp_string_type_guid))
		return FSP_NOT_FOUND;

	return print_fsp_string_data(data);
}
