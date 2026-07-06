
/* SPDX-License-Identifier: GPL-2.0-only */

#include <vendorcode/amd/psp/psp_fw_parser.h>

#include <commonlib/region.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <vendorcode/amd/psp/internal.h>

/* Public API */

/*
 * Returns the region_device of the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @return the region_device of the PSP directory region, or NULL if dir_rdev is NULL.
 */
const struct region_device *psp_dir_get_rdev(const struct psp_dir_region *dir_rdev)
{
	return dir_rdev ? (const struct region_device *)&dir_rdev->rdev : NULL;
}

/*
 * Returns the root region_device of the PSP directory region.
 * If the PSP directory region is a root region, it returns itself.
 *
 * @param dir_rdev	PSP directory region
 * @return the region_device of the PSP directory region, or NULL if dir_rdev is NULL.
 */
const struct region_device *psp_dir_get_root(const struct psp_dir_region *dir_rdev)
{
	if (!dir_rdev)
		return NULL;
	if (!dir_rdev->rdev.root)
		return &dir_rdev->rdev;
	return dir_rdev->rdev.root;
}

/*
 * Returns the root region_device of the PSP directory region.
 * If the PSP directory region is a root region, it returns itself.
 *
 * @param dir_rdev	PSP directory region
 * @return the region_device of the PSP directory region, or NULL if dir_rdev is NULL.
 */
size_t psp_dir_relative_offset(const struct psp_dir_region *dir_rdev)
{
	if (!dir_rdev)
		return 0;
	if (!dir_rdev->rdev.root || dir_rdev->rdev.root == &dir_rdev->rdev)
		return 0;
	return rdev_relative_offset(dir_rdev->rdev.root, &dir_rdev->rdev);
}

/*
 * Returns the number of entries in the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @return the number of entries in the PSP directory region, or 0 if dir_rdev is NULL.
 */
uint32_t psp_dir_count_entries(const struct psp_dir_region *dir_rdev)
{
	if (!dir_rdev)
		return 0;
	switch (dir_rdev->type) {
	case PSP_DIR_PSP:
		return psp_fw_psp_get_entry_count(dir_rdev);
	case PSP_DIR_BIOS:
		return psp_fw_bios_get_entry_count(dir_rdev);
	case PSP_DIR_COMBO:
		return psp_fw_combo_get_entry_count(dir_rdev);
	case PSP_DIR_ISH:
		return psp_fw_ish_get_entry_count(dir_rdev);
	case PSP_DIR_EFS:
		return psp_fw_efs_get_entry_count(dir_rdev);
	default:
		break;
	}
	return 0;
}

/*
 * Returns the entry at the given index in the PSP directory region.
 *
 * @param dir_rdev	PSP directory region
 * @param index		Index of the entry to return
 * @param entry		Pointer to a union psp_dir_entry to store the entry
 * @return CB_SUCCESS if the entry was found, otherwise an error code.
 */
enum cb_err psp_dir_get_entry_by_index(const struct psp_dir_region *dir_rdev,
				       const size_t index,
				       union psp_dir_entry *entry)
{
	if (!dir_rdev || !entry)
		return CB_ERR_ARG;

	switch (dir_rdev->type) {
	case PSP_DIR_PSP:
		return psp_fw_psp_get_entry_by_index(dir_rdev, index, entry);
	case PSP_DIR_BIOS:
		return psp_fw_bios_get_entry_by_index(dir_rdev, index, entry);
	case PSP_DIR_COMBO:
		return psp_fw_combo_get_entry_by_index(dir_rdev, index, entry);
	case PSP_DIR_ISH:
		return psp_fw_ish_get_entry_by_index(dir_rdev, index, entry);
	case PSP_DIR_EFS:
		return psp_fw_efs_get_entry_by_index(dir_rdev, index, entry);
	default:
		break;
	}
	return CB_ERR_NOT_IMPLEMENTED;
}

#define SPI_ROM_BASE 0xff000000

/* Converts addresses to be relative to the start of the file */
static enum cb_err relative_offset(const struct psp_dir_region *dir_rdev,
				   const uint64_t addr,
				   const enum amd_addr_mode mode,
				   uint64_t *offset)
{
	const struct region_device *root = psp_dir_get_root(dir_rdev);

	switch (mode) {
	case AMD_ADDR_AUTO:
		if (addr < region_device_sz(root)) {
			return relative_offset(dir_rdev, addr, AMD_ADDR_REL_BIOS, offset);
		} else if (addr >= SPI_ROM_BASE && addr <= 0xffffffff) {
			return relative_offset(dir_rdev, addr, AMD_ADDR_PHYSICAL, offset);
		}
		return CB_ERR;
	/* Since this utility operates on the BIOS file, physical address is converted
	   relative to the start of the BIOS file. */
	case AMD_ADDR_PHYSICAL:
		if (addr < MAX(SPI_ROM_BASE, 0xffffffff - region_device_sz(root) + 1) ||
		    addr > 0xffffffff) {
			return CB_ERR;
		}
		/* Assumes region_device_sz(root) is a power of 2 */
		*offset = addr & (region_device_sz(root) - 1);
		return CB_SUCCESS;

	case AMD_ADDR_REL_BIOS:
		if (addr >= region_device_sz(root)) {
			return CB_ERR;
		}
		/* Assumes region_device_sz(root) is a power of 2 */
		*offset = addr & (region_device_sz(root) - 1);
		return CB_SUCCESS;

	case AMD_ADDR_REL_TAB:
		*offset = addr + psp_dir_relative_offset(dir_rdev);
		return CB_SUCCESS;

	case AMD_ADDR_REL_SLOT:
		if (dir_rdev->parent && dir_rdev->parent->type == PSP_DIR_ISH) {
			*offset = addr + psp_dir_relative_offset(dir_rdev->parent);
			return CB_SUCCESS;
		}
		return CB_ERR;
		break;
	default:
		break;
	}
	return CB_ERR_ARG;
}

/*
 * Returns the offset of the entry relative to the start of the flash.
 *
 * @param dir_rdev	PSP directory region
 * @param entry		Entry to get the offset of
 * @return the offset of the entry in the PSP directory region, or 0 if dir_rdev is NULL.
 */
uint32_t psp_dir_entry_offset(const struct psp_dir_region *dir_rdev, const union psp_dir_entry *entry)
{
	enum amd_addr_mode mode = AMD_ADDR_AUTO;
	uint64_t offset;
	uint64_t addr = 0;

	if (!dir_rdev)
		return 0;

	switch (dir_rdev->type) {
	case PSP_DIR_EFS:
		mode = dir_rdev->mode;
		addr = entry->efs.offset;
		break;
	case PSP_DIR_PSP:
		if (dir_rdev->mode < AMD_ADDR_REL_TAB)
			mode = dir_rdev->mode;
		else
			mode = entry->psp.address_mode;
		addr = entry->psp.addr;
		break;
	case PSP_DIR_BIOS:
		if (dir_rdev->mode < AMD_ADDR_REL_TAB)
			mode = dir_rdev->mode;
		else
			mode = entry->bios.address_mode;
		addr = entry->bios.source;
		break;
	case PSP_DIR_COMBO:
		addr = entry->combo.lvl2_addr;
		break;
	case PSP_DIR_ISH:
		addr = entry->ish.pl2_location;
		break;
	default:
		break;
	}

	enum cb_err err = relative_offset(dir_rdev, addr, mode, &offset);
	return err == CB_SUCCESS ? (uint32_t)offset : 0;
}

/* Check if destination looks like the expected PSP directory header.
 *
 * Only checks the cookie for PSP/BIOS/COMBO, as SPI flash access might be slow.
 *
 * @param rdev		Region device to read from
 * @param offset	Offset of the table in rdev
 * @param type		Expected type of the PSP directory
 *
 * @returns the cookie if it looks like the requested PSP directory, otherwise 0.
 */
uint32_t psp_fw_test_header(const struct region_device *rdev, const uint32_t offset,
			    const enum psp_dir_type type)
{
	struct region_device rdev_hdr;
	ssize_t hdr_size;
	ssize_t len = sizeof(uint32_t);
	uint32_t cookie;

	if (!rdev)
		return 0;

	/* SPI flash access might be slow, thus only check the cookie */
	if (rdev_readat(rdev, &cookie, offset, len) != len)
		return 0;

	switch (cookie) {
	case PSP_COOKIE:
	case PSPL2_COOKIE:
		if (type != PSP_DIR_PSP)
			return 0;
		hdr_size = sizeof(struct psp_directory_header);
		break;
	case BHD_COOKIE:
	case BHDL2_COOKIE:
		if (type != PSP_DIR_BIOS)
			return 0;
		hdr_size = sizeof(struct bios_directory_hdr);
		break;
	case PSP2_COOKIE:
	case BHD2_COOKIE :
		if (type != PSP_DIR_COMBO)
			return 0;
		hdr_size = sizeof(struct psp_combo_header);
		break;
	default:
		/* Unknown */
		return 0;
	}

	/* Check if rdev could hold complete header. Does not access SPI flash. */
	if (rdev_chain(&rdev_hdr, rdev, offset, hdr_size))
		return 0;

	return cookie;
}

/* Validate the PSP table header and returns a pointer to a psp_dir_region covering the table.
 *
 * The table checksum must be valid, otherwise an error code is returned.
 * The table is not copied, but a region_device is created that points to the
 * table in the original region_device.
 *
 * @param rdev			Region device to read from
 * @param offset		Offset of the table in rdev
 * @param type			Expected type of the PSP directory
 * @param rdev_hdr		Region device to create with the table
 * @return CB_SUCCESS if the table is valid, otherwise an error code.
 */
enum cb_err psp_fw_read_and_validate_table(const struct region_device *rdev, const uint32_t offset,
				    const enum psp_dir_type type, struct psp_dir_region *rdev_hdr)
{
	if (!rdev || !rdev_hdr)
		return CB_ERR_ARG;

	switch (type) {
	case PSP_DIR_PSP:
		return psp_fw_psp_validate_table(rdev, offset, rdev_hdr);
	case PSP_DIR_BIOS:
		return psp_fw_bios_validate_table(rdev, offset, rdev_hdr);
	case PSP_DIR_COMBO:
		return psp_fw_combo_validate_table(rdev, offset, rdev_hdr);
	case PSP_DIR_ISH:
		return psp_fw_ish_validate_table(rdev, offset, rdev_hdr);
	case PSP_DIR_EFS:
		return psp_fw_efs_validate_table(rdev, offset, rdev_hdr);
	default:
		break;
	}
	return CB_ERR_NOT_IMPLEMENTED;
}
