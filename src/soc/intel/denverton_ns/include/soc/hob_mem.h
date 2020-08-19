/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_HOB_MEM_H
#define _DENVERTON_NS_HOB_MEM_H

#include <console/console.h>
#include <fsp/util.h>

void soc_display_fsp_smbios_memory_info_hob(
		const FSP_SMBIOS_MEMORY_INFO *memory_info_hob);

void soc_save_dimm_info(void);

#define FSP_SMBIOS_MEMORY_INFO_GUID	\
{	\
	0x8c, 0x10, 0xa1, 0x01, 0xee, 0x9d, 0x84, 0x49,	\
	0x88, 0xc3, 0xee, 0xe8, 0xc4, 0x9e, 0xfb, 0x89	\
}

static inline const FSP_SMBIOS_MEMORY_INFO *
soc_get_fsp_smbios_memory_info_hob(void)
{
	size_t hob_size;
	const FSP_SMBIOS_MEMORY_INFO *memory_info_hob;
	const uint8_t smbios_memory_info_guid[16] =
			FSP_SMBIOS_MEMORY_INFO_GUID;

	/* Locate the memory info HOB */
	memory_info_hob = fsp_find_extension_hob_by_guid(
				smbios_memory_info_guid,
				&hob_size);
	if (memory_info_hob == NULL || hob_size == 0) {
		printk(BIOS_ERR, "SMBIOS MEMORY_INFO_DATA_HOB not found\n");
		return NULL;
	}

	return memory_info_hob;
}

#endif // _DENVERTON_NS_HOB_MEM_H
