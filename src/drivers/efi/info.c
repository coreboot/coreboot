/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <stdint.h>
#include <string.h>
#include <uuid.h>

void lb_efi_fw_info(struct lb_header *header)
{
	uint8_t guid[16];
	struct lb_efi_fw_info *fw_info;

	if (parse_uuid(guid, CONFIG_DRIVERS_EFI_MAIN_FW_GUID)) {
		printk(BIOS_ERR, "%s(): failed to parse firmware's GUID: '%s'\n", __func__,
		       CONFIG_DRIVERS_EFI_MAIN_FW_GUID);
		return;
	}

	fw_info = (struct lb_efi_fw_info *)lb_new_record(header);
	fw_info->tag = LB_TAG_EFI_FW_INFO;
	fw_info->size = sizeof(*fw_info);

	memcpy(fw_info->guid, guid, sizeof(guid));
	fw_info->version = CONFIG_DRIVERS_EFI_MAIN_FW_VERSION;
	fw_info->lowest_supported_version = CONFIG_DRIVERS_EFI_MAIN_FW_LSV;
	fw_info->fw_size = CONFIG_ROM_SIZE;
}
