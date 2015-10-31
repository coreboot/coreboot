/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Siemens AG.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "modhwinfo.h"
#include "lcd_panel.h"
#include <cbfs.h>
#include <string.h>

/** \brief This function will find the first linked info block.
 * @param  *filename    Filename in cbfs
 * @param  *file_offset Pointer to the offset of the cbfs file contents
 * @return u8*          Pointer to the found block
 */
u8* get_first_linked_block(char *filename, u8 **file_offset)
{
	u8* block_ptr = NULL;

	block_ptr = cbfs_boot_map_with_leak(filename, 0x50, NULL);
	if (!block_ptr)
		return NULL;
	if (!strncmp((char*)block_ptr, "H1W2M3I4", LEN_MAGIC_NUM)) {
		if ((*((u16*)(block_ptr + HWI_LEN_OFFSET)) == LEN_MAIN_HWINFO) &&
		    (*((s32*)(block_ptr + NEXT_OFFSET_HWINFO)) != 0x00)) {
			*file_offset = block_ptr;
			return *((s32*)(block_ptr + NEXT_OFFSET_HWINFO)) + block_ptr;
		} else
			return NULL;
	} else if (!strncmp((char*)block_ptr, "H1W2M3I5", LEN_MAGIC_NUM)) {
		*file_offset = block_ptr;
		return block_ptr;
	} else
		return NULL;
}

/** \brief This function will find the main info block
 * @param  *filename Filename in cbfs
 * @return *hwinfo   Pointer to the data of the main info block
 */
struct hwinfo* get_hwinfo(char *filename)
{
	struct hwinfo* main_hwinfo;

	main_hwinfo = cbfs_boot_map_with_leak(filename, 0x50, NULL);
	if ((main_hwinfo) &&
		(!strncmp(main_hwinfo->magicNumber, "H1W2M3I4", LEN_MAGIC_NUM)) &&
		(main_hwinfo->length == LEN_MAIN_HWINFO))
		  return main_hwinfo;
	else
		return NULL;
}

/** \brief This function will find the short info block
 * @param  *filename  Filename in cbfs
 * @return *shortinfo Pointer to the data of the short info block
 */
struct shortinfo* get_shortinfo(char *filename)
{
	u8 *block_ptr = NULL;
	u8 *file_offset = NULL;

	block_ptr = get_first_linked_block(filename, &file_offset);
	if ((block_ptr == NULL) ||
	    (strncmp((char*)block_ptr, "H1W2M3I5", LEN_MAGIC_NUM)))
		return NULL;

	if ((*((u16*)(block_ptr + HWI_LEN_OFFSET))) == LEN_SHORT_INFO)
		return (struct shortinfo *)block_ptr;

	block_ptr = (file_offset + *((s32*)(block_ptr + NEXT_OFFSET_EDID)));
	if ((*((u16*)(block_ptr + HWI_LEN_OFFSET))) == LEN_SHORT_INFO)
		return (struct shortinfo *)block_ptr;
	else
		return NULL;
}

/** \brief This function will find the edid info block
 * @param  *filename  Filename in cbfs
 * @return *edidinfo  Pointer to the data of the edid info block
 */
struct edidinfo* get_edidinfo(char *filename)
{
	u8 *block_ptr = NULL;
	u8 *file_offset = NULL;

	block_ptr = get_first_linked_block(filename, &file_offset);
	if ((block_ptr == NULL) ||
	    (strncmp((char*)block_ptr, "H1W2M3I5", LEN_MAGIC_NUM)))
		return NULL;

	if ((*((u16*)(block_ptr + HWI_LEN_OFFSET))) == LEN_EDID_INFO)
		return (struct edidinfo *)block_ptr;

	block_ptr = (file_offset + *((s32*)(block_ptr + NEXT_OFFSET_SIB)));
	if ((*((u16*)(block_ptr + HWI_LEN_OFFSET))) == LEN_EDID_INFO)
		return (struct edidinfo *)block_ptr;
	else
		return NULL;
}

/** \brief This function will search for a MAC address which can be assigned
 *         to a MACPHY.
 * @param  pci_bdf Bus, device and function of the given PCI-device
 * @param  mac     buffer where to store the MAC address
 * @return cb_err  CB_ERR or CB_SUCCESS
 */
enum cb_err mainboard_get_mac_address(u16 bus, u8 devfn, u8 mac[6])
{
	struct hwinfo* main_hwinfo;
	u32 i;

	main_hwinfo = get_hwinfo((char*)"hwinfo.hex");
	if (!main_hwinfo)
		return CB_ERR;
	/* Ensure the first MAC-Address is not completely 0x00 or 0xff */
	for (i = 0; i < 6; i++) {
		if (main_hwinfo->macAddress1[i] != 0xFF)
			break;
	}
	if (i == 6){
		return CB_ERR;
	}
	for (i = 0; i < 6; i++) {
		if (main_hwinfo->macAddress1[i] != 0x00)
			break;
	}
	if (i == 6){
		return CB_ERR;
	} else {
		memcpy(mac, main_hwinfo->macAddress1, 6);
		return CB_SUCCESS;
	}
}
