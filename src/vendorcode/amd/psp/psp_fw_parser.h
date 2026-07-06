
/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PSP_FW_PARSER_H_
#define _PSP_FW_PARSER_H_

#include <commonlib/bsd/cb_err.h>
#include <commonlib/region.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>

#include <vendorcode/amd/psp/psp_directory.h>
#include <vendorcode/amd/psp/bios_directory.h>
#include <vendorcode/amd/psp/combo_directory.h>
#include <vendorcode/amd/psp/ish.h>
#include <vendorcode/amd/psp/psp_efs.h>
/*
 * The parent type of the PSP directory.
 * The EFS has no parent.
 */
enum psp_dir_type {
	PSP_DIR_NONE,
	PSP_DIR_EFS,
	PSP_DIR_PSP,
	PSP_DIR_BIOS,
	PSP_DIR_COMBO,
	PSP_DIR_ISH,
};

struct psp_dir_region {
	struct region_device rdev;
	const struct psp_dir_region *parent;	/* Parent region */
	size_t parent_index;	/* Index of the entry in the parent directory */
	enum psp_dir_type type;
	enum amd_addr_mode mode;
	size_t level;	/* 1 or 2 for BIOS and PSP */
};

/* Metadata for PSP table parsing */
struct efs_entry {
	enum psp_dir_type type;
	uint32_t offset;
};

struct psp_ish_entry {
	uint32_t pl2_location;
	uint32_t psp_id;
};

union psp_dir_entry {
	struct psp_directory_entry psp;
	struct bios_directory_entry bios;
	struct psp_combo_entry combo;
	struct psp_ish_entry ish;
	struct efs_entry efs;
};

/* Check if destination looks like the expected PSP directory header.
 *
 * Only checks the cookie, as SPI flash access might be slow.
 *
 * @returns the cookie if it looks like the requested PSP directory, otherwise 0.
 */
uint32_t psp_fw_test_header(const struct region_device *rdev,
			    const uint32_t offset,
			    const enum psp_dir_type type);

/* Validate the PSP table header and returns a pointer to a psp_dir_region covering the table.
 *
 * The table checksum must be valid, otherwise an error code is returned.
 * The table is not copied, but a region_device is created that points to the
 * table in the original region_device.
 *
 * @param rdev			Region device to read from
 * @param offset		Offset of the table in rdev
 * @param expected_cookie	Expected cookie of the table
 * @param rdev_hdr		Region device to create with the table
 * @return CB_SUCCESS if the table is valid, otherwise an error code.
 */
enum cb_err psp_fw_read_and_validate_table(const struct region_device *rdev,
					   const uint32_t offset,
					   const enum psp_dir_type type,
					   struct psp_dir_region *rdev_hdr);

/* Operations on struct psp_dir_region */
const struct region_device *psp_dir_get_rdev(const struct psp_dir_region *dir_rdev);
const struct region_device *psp_dir_get_root(const struct psp_dir_region *dir_rdev);
size_t psp_dir_relative_offset(const struct psp_dir_region *dir_rdev);
uint32_t psp_dir_count_entries(const struct psp_dir_region *dir_rdev);
enum cb_err psp_dir_get_entry_by_index(const struct psp_dir_region *dir_rdev,
				       const size_t index,
				       union psp_dir_entry *entry);
uint32_t psp_dir_entry_offset(const struct psp_dir_region *dir_rdev,
			      const union psp_dir_entry *entry);

#endif /* _PSP_FW_PARSER_H_ */
