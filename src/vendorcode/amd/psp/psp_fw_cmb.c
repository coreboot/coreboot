
/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/psp/psp_fw_parser.h>

#include <commonlib/region.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "internal.h"
#include "psp_fw_parser.h"

/* Returns the number of entries in the PSP directory region.
 * Must be of type PSP_DIR_COMBO.
 *
 * @param dir_rdev	PSP directory region
 * @return the number of entries in the PSP directory region, or 0 if dir_rdev is NULL or could not be read.
 */
uint32_t psp_fw_combo_get_entry_count(const struct psp_dir_region *dir_rdev)
{
	struct psp_combo_header combo_hdr;

	assert (dir_rdev && dir_rdev->type == PSP_DIR_COMBO);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_COMBO)
		return 0;

	ssize_t len = sizeof(struct psp_combo_header);
	if (rdev_readat(&dir_rdev->rdev, &combo_hdr, 0, len) != len)
		return 0;

	return combo_hdr.num_entries;
}

/* Returns the PSP directory entry at the given index in the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @param index		Index of the entry to retrieve
 * @param entry		Pointer to a union psp_dir_entry to store the entry
 * @return CB_SUCCESS if the entry was retrieved successfully, otherwise an error code.
*/
enum cb_err psp_fw_combo_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					    const size_t index,
					    union psp_dir_entry *entry)
{
	ssize_t offset, len;
	struct psp_combo_entry combo;

	assert (dir_rdev && dir_rdev->type == PSP_DIR_COMBO);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_COMBO)
		return CB_ERR_ARG;
	if (index >= psp_fw_combo_get_entry_count(dir_rdev))
		return CB_ERR_ARG;

	offset = sizeof(struct psp_combo_header) + index * sizeof(struct psp_combo_entry);
	len = sizeof(struct psp_combo_entry);
	if (rdev_readat(&dir_rdev->rdev, &combo, offset, len) != len)
		return CB_ERR;

	entry->combo = combo;
	return CB_SUCCESS;
}

/* Validate the COMBO table header and returns a pointer to a psp_dir_region covering the table.
 *
 * The table checksum must be valid, otherwise CB_ERR is returned.
 * The table is not copied, but a region_device is created that points to the
 * table in the original region_device.
 *
 * @param rdev			Region device to read from
 * @param offset		Offset of the table in rdev
 * @param rdev_hdr		Region device to create with the table
 * @return CB_SUCCESS if the table is valid, otherwise an error code.
 */
enum cb_err psp_fw_combo_validate_table(const struct region_device *rdev,
					const uint32_t offset,
					struct psp_dir_region *rdev_hdr)
{
	struct psp_combo_header combo_hdr;
	struct psp_dir_region ret = {0};

	if (!rdev || !rdev_hdr)
		return CB_ERR_ARG;

	ssize_t len = sizeof(struct psp_combo_header);
	if (rdev_readat(rdev, &combo_hdr, offset, len) != len)
		return CB_ERR;

	if (combo_hdr.cookie != PSP2_COOKIE && combo_hdr.cookie != BHD2_COOKIE)
		return CB_ERR;
	if (!combo_hdr.num_entries)
		return CB_ERR;
	len += combo_hdr.num_entries * sizeof(struct psp_combo_entry);
	if (rdev_chain(&ret.rdev, rdev, offset, len))
		return CB_ERR;
	ret.level = 1;
	ret.type = PSP_DIR_COMBO;
	ret.mode = AMD_ADDR_PHYSICAL;

	/* Validate checksum */
	uint32_t checksum = combo_hdr.checksum;
	size_t cksm_offset = offsetof(struct psp_combo_header, checksum) + sizeof(uint32_t);

	/* Create rdev with no cookie and checksum field. */
	struct region_device rdev_checksum;
	if (rdev_chain(&rdev_checksum, &ret.rdev, cksm_offset, region_device_sz(&ret.rdev) - cksm_offset))
		return CB_ERR;

	if (psp_fw_fletcher32_rdev(&rdev_checksum) != checksum)
		return CB_ERR;

	memcpy(rdev_hdr, &ret, sizeof(struct psp_dir_region));
	return CB_SUCCESS;
}

/* Returns the cookie of the PSP directory region.
 * Must be of type PSP_DIR_COMBO.
 *
 * @param dir_rdev	PSP directory region
 * @return the cookie of the PSP directory region, or 0 if dir_rdev is NULL or could not be read.
 */
uint32_t psp_fw_combo_cookie(const struct psp_dir_region *dir_rdev)
{
	struct psp_combo_header combo_hdr;

	assert (dir_rdev && dir_rdev->type == PSP_DIR_COMBO);
	if (!dir_rdev || dir_rdev->type != PSP_DIR_COMBO)
		return 0;

	ssize_t len = sizeof(struct psp_combo_header);
	if (rdev_readat(&dir_rdev->rdev, &combo_hdr, 0, len) != len)
		return 0;

	return combo_hdr.cookie;
}
