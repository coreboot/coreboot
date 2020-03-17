/*
 * This file is part of the coreboot project.
 *
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

#ifndef VBOOT_CHECK_H
#define VBOOT_CHECK_H

#include <cbfs.h>
#include <device/device.h>
#include <device/pci.h>
#include <lib.h>
#include CONFIG_VENDORCODE_ELTAN_VBOOT_MANIFEST
#include <console/console.h>
#include <vb2_sha.h>
#include <string.h>
#include <program_loading.h>
#include <mboot.h>

#define VERIFIED_BOOT_COPY_BLOCK	0x80000000
/* These method verifies the SHA256 hash over the 'named' CBFS component.
 * 'type' denotes the type of CBFS component i.e. stage, payload or fsp.
 */
void verified_boot_bootblock_check(void);
void verified_boot_early_check(void);

int verified_boot_check_manifest(void);

void verified_boot_check_cbfsfile(const char *name, uint32_t type,
	uint32_t hash_index, void **buffer, uint32_t *filesize, int32_t pcr);

typedef enum {
	VERIFY_TERMINATOR = 0,
	VERIFY_FILE,
	VERIFY_BLOCK,
	VERIFY_OPROM

} verify_type;

typedef struct {
	verify_type type;
	const char *name;
	union {
		struct {
			const void *related_items;
			uint32_t cbfs_type;
		} file;
		struct {
			const void *start;
			uint32_t size;
		} block;
		struct {
			const void *related_items;
			uint32_t viddev;
		} oprom;
	} data;
	uint32_t hash_index;
	int32_t pcr;
} verify_item_t;

void process_verify_list(const verify_item_t list[]);

extern const verify_item_t bootblock_verify_list[];
extern const verify_item_t romstage_verify_list[];
extern const verify_item_t postcar_verify_list[];
extern const verify_item_t ramstage_verify_list[];
extern const verify_item_t payload_verify_list[];
extern const verify_item_t oprom_verify_list[];

#endif //VBOOT_CHECK_H
