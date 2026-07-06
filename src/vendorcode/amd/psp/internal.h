/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PSP_FW_INTERNAL_H_
#define _PSP_FW_INTERNAL_H_

#include <commonlib/region.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <vendorcode/amd/psp/psp_fw_parser.h>

uint32_t psp_fw_fletcher32_rdev(const struct region_device *rdev);

/* BIOS */
uint32_t psp_fw_bios_get_entry_count(const struct psp_dir_region *dir_rdev);
enum cb_err psp_fw_bios_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					   const size_t index,
					   union psp_dir_entry *entry);
enum cb_err psp_fw_bios_validate_table(const struct region_device *rdev,
				       const uint32_t offset,
				       struct psp_dir_region *rdev_hdr);

/* COMBO */
uint32_t psp_fw_combo_get_entry_count(const struct psp_dir_region *dir_rdev);
enum cb_err psp_fw_combo_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					    const size_t index,
					    union psp_dir_entry *entry);
enum cb_err psp_fw_combo_validate_table(const struct region_device *rdev,
					const uint32_t offset,
					struct psp_dir_region *rdev_hdr);
uint32_t psp_fw_combo_cookie(const struct psp_dir_region *dir_rdev);

/* ISH */
uint32_t psp_fw_ish_get_entry_count(const struct psp_dir_region *dir_rdev);
enum cb_err psp_fw_ish_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					  const size_t index,
					  union psp_dir_entry *entry);
enum cb_err psp_fw_ish_validate_table(const struct region_device *rdev,
			       const uint32_t offset,
			       struct psp_dir_region *rdev_hdr);

/* PSP */
uint32_t psp_fw_psp_get_entry_count(const struct psp_dir_region *dir_rdev);
enum cb_err psp_fw_psp_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					  const size_t index,
					  union psp_dir_entry *entry);
enum cb_err psp_fw_psp_validate_table(const struct region_device *rdev,
				      const uint32_t offset,
				      struct psp_dir_region *rdev_hdr);

/* EFS */
uint32_t psp_fw_efs_get_entry_count(const struct psp_dir_region *dir_rdev);
enum cb_err psp_fw_efs_get_entry_by_index(const struct psp_dir_region *dir_rdev,
					  const size_t index,
					  union psp_dir_entry *entry);
enum cb_err psp_fw_efs_validate_table(const struct region_device *rdev,
				      const uint32_t offset,
				      struct psp_dir_region *rdev_hdr);

#endif /* _PSP_FW_INTERNAL_H_ */
