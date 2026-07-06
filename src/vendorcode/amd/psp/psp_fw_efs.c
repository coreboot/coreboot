
/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/psp/psp_fw_parser.h>

#include <commonlib/region.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "internal.h"

static const struct {
	uint8_t offset;
	enum psp_dir_type type;
} efs_offset_types[] = {
	{offsetof(struct embedded_firmware, psp_directory), PSP_DIR_PSP},
	{offsetof(struct embedded_firmware, psp_directory), PSP_DIR_COMBO},
	{offsetof(struct embedded_firmware, new_psp_directory), PSP_DIR_PSP},
	{offsetof(struct embedded_firmware, new_psp_directory), PSP_DIR_COMBO},
	{offsetof(struct embedded_firmware, psp_bak_directory), PSP_DIR_PSP},
	{offsetof(struct embedded_firmware, bios0_entry), PSP_DIR_BIOS},
	{offsetof(struct embedded_firmware, bios1_entry), PSP_DIR_BIOS},
	{offsetof(struct embedded_firmware, bios2_entry), PSP_DIR_BIOS},
	{offsetof(struct embedded_firmware, bios3_entry), PSP_DIR_BIOS},
	{offsetof(struct embedded_firmware, bios3_entry), PSP_DIR_COMBO},
};

static const uint8_t efs_offsets[] = {
	offsetof(struct embedded_firmware, psp_directory),
	offsetof(struct embedded_firmware, new_psp_directory),
	offsetof(struct embedded_firmware, psp_bak_directory),
	offsetof(struct embedded_firmware, bios0_entry),
	offsetof(struct embedded_firmware, bios1_entry),
	offsetof(struct embedded_firmware, bios2_entry),
	offsetof(struct embedded_firmware, bios3_entry),
};

/* Returns the number of entries in the PSP directory region.
 * Must be of type PSP_DIR_EFS.
 * Since 'efs_offset_types' contains duplicate offsets, the number of entries is
 * counted by unique offsets.
 *
 * @param dir_rdev	PSP directory region
 * @return the number of entries in the PSP directory region, or 0 if dir_rdev is NULL or could not be read.
 */
uint32_t psp_fw_efs_get_entry_count(const struct psp_dir_region *dir_rdev)
{
	size_t cnt = 0;

	assert (dir_rdev && dir_rdev->type == PSP_DIR_EFS);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_EFS)
		return 0;

	for (size_t i = 0; i < ARRAY_SIZE(efs_offsets); i++) {
		uint32_t dir_offset_data;
		ssize_t len = sizeof(dir_offset_data);
		if (rdev_readat(&dir_rdev->rdev, &dir_offset_data, efs_offsets[i], len) != len)
			return 0;

		/* Skip entries not filled */
		if (dir_offset_data == 0 || dir_offset_data == 0xFFFFFFFF)
			continue;

		cnt++;
	}
	return cnt;
}

/* Returns the PSP directory entry at the given index in the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @param index		Index of the entry to retrieve
 * @param entry		Pointer to a union psp_dir_entry to store the entry
 * @return CB_SUCCESS if the entry was retrieved successfully, otherwise an error code.
 */
enum cb_err psp_fw_efs_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					  const size_t index,
					  union psp_dir_entry *entry)
{
	size_t cnt = 0;
	uint32_t dir_offset_data = 0;

	/* Locate the entry by index. Unset fields are skipped. */
	for (size_t i = 0; i < ARRAY_SIZE(efs_offsets); i++) {
		ssize_t len = sizeof(dir_offset_data);
		if (rdev_readat(&dir_rdev->rdev, &dir_offset_data, efs_offsets[i], len) != len)
			return CB_ERR;

		/* Skip entries not filled */
		if (dir_offset_data == 0 || dir_offset_data == 0xFFFFFFFF)
			continue;

		if (cnt == index)
			break;

		cnt++;
	}

	if (dir_offset_data == 0 || dir_offset_data == 0xFFFFFFFF)
		return CB_ERR;	/* Not found */


	const struct region_device *root = psp_dir_get_root(dir_rdev);

	entry->efs.type = PSP_DIR_NONE;
	entry->efs.offset = dir_offset_data;

	/*
	 * Only check the cookie. This will also check if the offset is within the rdev.
	 * Can all be false when running inside unit-tests, since the EFS table would
	 * be the only table present, which should never be the case in real firmware.
	 */
	for (size_t i = 0; i < ARRAY_SIZE(efs_offset_types); i++) {
		switch (efs_offset_types[i].type) {
		case PSP_DIR_PSP:
			if (psp_fw_test_header(root, dir_offset_data, PSP_DIR_PSP) != 0) {
				entry->efs.type = PSP_DIR_PSP;
				break;
			}
			break;
		case PSP_DIR_BIOS:
			if (psp_fw_test_header(root, dir_offset_data, PSP_DIR_BIOS) != 0) {
				entry->efs.type = PSP_DIR_BIOS;
				break;
			}
			break;
		case PSP_DIR_COMBO:
			if (psp_fw_test_header(root, dir_offset_data, PSP_DIR_COMBO) != 0) {
				entry->efs.type = PSP_DIR_COMBO;
				break;
			}
			break;
		default:
			break;
		}

		if (entry->efs.type != PSP_DIR_NONE)
			break;
	}

	return CB_SUCCESS;
}

/* Validate the EFS table and returns a pointer to a psp_dir_region covering the table.
 *
 * The table is not copied, but a region_device is created that points to the
 * table in the original region_device.
 *
 * @param rdev			Region device to read from
 * @param offset		Offset of the table in rdev
 * @param rdev_hdr		Region device to create with the table
 * @return CB_SUCCESS if the table is valid, otherwise an error code.
 */
enum cb_err psp_fw_efs_validate_table(const struct region_device *rdev,
				      const uint32_t offset,
				      struct psp_dir_region *rdev_hdr)
{
	struct embedded_firmware efs;
	struct psp_dir_region ret = {0};

	if (!rdev || !rdev_hdr)
		return CB_ERR_ARG;

	ssize_t len = sizeof(struct embedded_firmware);
	if (rdev_readat(rdev, &efs, offset, len) != len)
		return CB_ERR;
	if (efs.signature != EMBEDDED_FW_SIGNATURE)
		return CB_ERR;
	if (rdev_chain(&ret.rdev, rdev, offset, len))
		return CB_ERR;
	ret.level = 0;
	ret.type = PSP_DIR_EFS;

	if (efs.efs_gen.gen == EFS_SECOND_GEN || efs.multi_gen_efs != 0xffffffff)
		ret.mode = AMD_ADDR_REL_BIOS;
	else
		ret.mode = AMD_ADDR_PHYSICAL;

	memcpy(rdev_hdr, &ret, sizeof(struct psp_dir_region));
	return CB_SUCCESS;
}
