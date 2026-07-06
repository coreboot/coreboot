
/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/psp/psp_fw_parser.h>

#include <commonlib/region.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "internal.h"

/* Returns the number of entries in the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @return the number of entries in the PSP directory region
 */
uint32_t psp_fw_ish_get_entry_count(const struct psp_dir_region *dir_rdev)
{
	if (!dir_rdev || dir_rdev->type != PSP_DIR_ISH)
		return 0;

	(void)dir_rdev;

	return 1;	/* Only PL2 location */
}

/* Returns the ISH directory entry at the given index in the ISH directory region.
 *
 * @param dir_rdev	PSP directory region
 * @param index		Index of the entry to retrieve
 * @param entry		Pointer to a union psp_dir_entry to store the entry
 * @return CB_SUCCESS if the entry was retrieved successfully, otherwise an error code.
 */
enum cb_err psp_fw_ish_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					  const size_t index,
					  union psp_dir_entry *entry)
{
	struct ish_directory_table ish;
	ssize_t len;

	assert (dir_rdev->type == PSP_DIR_ISH);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_ISH)
		return CB_ERR_ARG;

	if (index >= psp_fw_ish_get_entry_count(dir_rdev))
		return CB_ERR_ARG;

	len = sizeof(ish);
	if (rdev_readat(&dir_rdev->rdev, &ish, 0, len) != len)
		return CB_ERR;

	entry->ish.pl2_location = ish.pl2_location;
	entry->ish.psp_id = ish.psp_id;

	return CB_SUCCESS;
}

/* Validate the ISH table and returns a pointer to a psp_dir_region covering the table.
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
enum cb_err psp_fw_ish_validate_table(const struct region_device *rdev,
				      const uint32_t offset,
				      struct psp_dir_region *rdev_hdr)
{
	struct ish_directory_table ish;
	struct psp_dir_region ret = {0};

	if (!rdev || !rdev_hdr)
		return CB_ERR_ARG;

	ssize_t len = sizeof(struct ish_directory_table);
	if (rdev_readat(rdev, &ish, offset, len) != len)
		return CB_ERR;
	if (ish.pl2_location == 0xFFFFFFFF)
		return CB_ERR;
	if (ish.psp_id == 0xFFFFFFFF)
		return CB_ERR;
	if (rdev_chain(&ret.rdev, rdev, offset, len))
		return CB_ERR;

	/* Validate checksum */
	uint32_t checksum = ish.checksum;
	size_t cksm_offset = offsetof(struct ish_directory_table, checksum) + sizeof(uint32_t);

	/* Create rdev with no cookie and checksum field. */
	struct region_device rdev_checksum;
	if (rdev_chain(&rdev_checksum, &ret.rdev, cksm_offset, region_device_sz(&ret.rdev) - cksm_offset))
		return CB_ERR;

	ret.level = 0;
	ret.type = PSP_DIR_ISH;

	if (psp_fw_fletcher32_rdev(&rdev_checksum) != checksum)
		return CB_ERR;

	memcpy(rdev_hdr, &ret, sizeof(struct psp_dir_region));
	return CB_SUCCESS;
}
