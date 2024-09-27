/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <soc/intel/common/reset.h>
#include <stdint.h>

static const uint8_t fsp_reset_guid[16] = {
	0xff, 0x97, 0x05, 0xea, 0x58, 0x88, 0xca, 0x41,
	0xbb, 0xc1, 0xfe, 0x18, 0xfc, 0xd2, 0x8e, 0x22
};

static const uint8_t fsp_global_reset_guid[16] = {
	0x4c, 0x1b, 0xb3, 0x9d, 0xef, 0xf5, 0xbb, 0x48,
	0x94, 0x2b, 0x18, 0x1f, 0x7e, 0x3a, 0x3e, 0x40
};

/* Platform Reset String as per Intel FSP is "PCH RESET" in unicode */
#define PLATFORM_RESET_STRING_LENGTH  20

struct pch_reset_data {
	char reserved[PLATFORM_RESET_STRING_LENGTH];
	efi_guid_t global_reset_uid;
};

/* This structure is used to provide information about PCH Reset */
struct fsp_reset_hob {
	EFI_RESET_TYPE   reset_type;
	struct pch_reset_data   reset_data;
};

void chipset_handle_reset(efi_return_status_t status)
{
	if (status == CONFIG_FSP_STATUS_GLOBAL_RESET) {
		printk(BIOS_DEBUG, "GLOBAL RESET!\n");
		global_reset();
	}

	fsp_printk(status, BIOS_ERR, "unhandled reset type");
	die("unknown reset type");
}

static efi_return_status_t fsp_reset_type_to_status(EFI_RESET_TYPE reset_type)
{
	efi_return_status_t status;

	switch (reset_type) {
	case EfiResetCold:
		status = FSP_STATUS_RESET_REQUIRED_COLD;
		break;
	case EfiResetWarm:
		status = FSP_STATUS_RESET_REQUIRED_WARM;
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", reset_type);
		die("unknown reset type");
	}

	return status;
}

/*
 * Return PCH Reset Status
 * The return status can be between EfiResetCold, EfiResetWarm, EfiResetShutdown
 * or EfiResetPlatformSpecific.
 *
 * If reset type is `EfiResetPlatformSpecific` then relying on pch_reset_data structure
 * to know if the reset type is a global reset.
 */
efi_return_status_t fsp_get_pch_reset_status(void)
{
	size_t size;
	const struct fsp_reset_hob *hob = fsp_find_extension_hob_by_guid(fsp_reset_guid, &size);
	if (!hob)
		return 0;

	if ((hob->reset_type == EfiResetPlatformSpecific) &&
		 fsp_guid_compare((void *)&(hob->reset_data.global_reset_uid),
			 fsp_global_reset_guid))
		return CONFIG_FSP_STATUS_GLOBAL_RESET;

	return fsp_reset_type_to_status(hob->reset_type);
}
