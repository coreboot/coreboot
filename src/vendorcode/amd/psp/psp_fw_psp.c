
/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/psp/psp_fw_parser.h>

#include <commonlib/region.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "internal.h"

/* Returns the number of entries in the PSP directory region.
 * Must be of type PSP_DIR_PSP.
 *
 * @param dir_rdev	PSP directory region
 * @return the number of entries in the PSP directory region, or 0 if dir_rdev is NULL or could not be read.
 */
uint32_t psp_fw_psp_get_entry_count(const struct psp_dir_region *dir_rdev)
{
	struct psp_directory_header psp_hdr;

	assert (dir_rdev && dir_rdev->type == PSP_DIR_PSP);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_PSP)
		return 0;

	ssize_t len = sizeof(struct psp_directory_header);
	if (rdev_readat(&dir_rdev->rdev, &psp_hdr, 0, len) != len)
		return 0;

	return psp_hdr.num_entries;
}

/* Returns the PSP directory entry at the given index in the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @param index		Index of the entry to retrieve
 * @param entry		Pointer to a union psp_dir_entry to store the entry
 * @return CB_SUCCESS if the entry was retrieved successfully, otherwise an error code.
 */
enum cb_err psp_fw_psp_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					  const size_t index,
					  union psp_dir_entry *entry)
{
	ssize_t offset, len;
	struct psp_directory_entry psp;

	assert (dir_rdev && dir_rdev->type == PSP_DIR_PSP);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_PSP)
		return CB_ERR_ARG;
	if (index >= psp_fw_psp_get_entry_count(dir_rdev))
		return CB_ERR_ARG;

	offset = sizeof(struct psp_directory_header) + index * sizeof(struct psp_directory_entry);
	len = sizeof(struct psp_directory_entry);
	if (rdev_readat(&dir_rdev->rdev, &psp, offset, len) != len)
		return CB_ERR;

	entry->psp = psp;

	return CB_SUCCESS;
}

/* Validate the PSP table header and returns a pointer to a psp_dir_region covering the table.
 *
 * The table checksum must be valid, otherwise an error code is returned.
 * The table is not copied, but a region_device is created that points to the
 * table in the original region_device.
 *
 * @param rdev			Region device to read from
 * @param offset		Offset of the table in rdev
 * @param rdev_hdr		Region device to create with the table
 * @return CB_SUCCESS if the table is valid, otherwise an error code.
 */
enum cb_err psp_fw_psp_validate_table(const struct region_device *rdev,
				      const uint32_t offset,
				      struct psp_dir_region *rdev_hdr)
{
	struct psp_directory_header psp_hdr;
	struct psp_dir_region ret = {0};

	if (!rdev || !rdev_hdr)
		return CB_ERR_ARG;

	ssize_t len = sizeof(struct psp_directory_header);
	if (rdev_readat(rdev, &psp_hdr, offset, len) != len)
		return CB_ERR;

	if (psp_hdr.cookie != PSP_COOKIE && psp_hdr.cookie != PSPL2_COOKIE)
		return CB_ERR;
	if (!psp_hdr.num_entries)
		return CB_ERR;

	len += psp_hdr.num_entries * sizeof(struct psp_directory_entry);
	if (rdev_chain(&ret.rdev, rdev, offset, len))
		return CB_ERR;

	ret.level = psp_hdr.cookie == PSP_COOKIE ? 1 : 2;
	ret.type = PSP_DIR_PSP;
	ret.mode = AMD_ADDR_AUTO;

	/* Validate checksum */
	uint32_t checksum = psp_hdr.checksum;
	size_t cksm_offset = offsetof(struct psp_directory_header, checksum) + sizeof(uint32_t);

	/* Create rdev with no cookie and checksum field. */
	struct region_device rdev_checksum;
	if (rdev_chain(&rdev_checksum, &ret.rdev, cksm_offset, region_device_sz(&ret.rdev) - cksm_offset))
		return CB_ERR;

	if (psp_fw_fletcher32_rdev(&rdev_checksum) != checksum)
		return CB_ERR;

	/* Figure out address mode */
	if (psp_hdr.additional_info != 0 && psp_hdr.additional_info != 0xffffffff) {
		if (psp_hdr.additional_info_fields.version == 1) {
			if (psp_hdr.additional_info_fields_v1.address_mode > AMD_ADDR_PHYSICAL)
				ret.mode = psp_hdr.additional_info_fields_v1.address_mode;
		} else {
			if (psp_hdr.additional_info_fields.address_mode > AMD_ADDR_PHYSICAL)
				ret.mode = psp_hdr.additional_info_fields.address_mode;
		}
	}

	/* 0 in AIF could mean "not set" or PHYSICAL. Guess based on entries. */
	if (ret.mode == AMD_ADDR_AUTO) {
		ret.mode = AMD_ADDR_REL_BIOS;
		for (size_t i = 0; i < psp_hdr.num_entries; i++) {
			union psp_dir_entry entry;
			enum cb_err err = psp_fw_psp_get_entry_by_index(&ret, i, &entry);
			if (err != CB_SUCCESS)
				return err;
			if (entry.psp.addr < 0xffffffff && entry.psp.addr > 0xff000000) {
				ret.mode = AMD_ADDR_PHYSICAL;
				break;
			}
		}
	}

	memcpy(rdev_hdr, &ret, sizeof(struct psp_dir_region));
	return CB_SUCCESS;
}
