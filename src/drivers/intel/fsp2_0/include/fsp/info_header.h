/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_INFO_HEADER_H_
#define _FSP2_0_INFO_HEADER_H_

#include <stdint.h>
#include <types.h>

#define FSP_HDR_OFFSET			0x94
#define FSP_HDR_LEN			0x48
#define FSP_HDR_SIGNATURE		"FSPH"
#define FSP_HDR_ATTRIB_FSPT		1
#define FSP_HDR_ATTRIB_FSPM		2
#define FSP_HDR_ATTRIB_FSPS		3

struct fsp_header {
	uint32_t fsp_revision;
	size_t image_size;
	uintptr_t image_base;
	uint16_t image_attribute;
	uint8_t spec_version;
	uint16_t component_attribute;
	size_t cfg_region_offset;
	size_t cfg_region_size;
	size_t temp_ram_init_entry;
	size_t temp_ram_exit_entry;
	size_t notify_phase_entry_offset;
	size_t memory_init_entry_offset;
	size_t silicon_init_entry_offset;
	char image_id[sizeof(uint64_t) + 1];
	uint8_t revision;
};

enum cb_err fsp_identify(struct fsp_header *hdr, const void *fsp_blob);

/*
 * This is a FSP_INFO_HEADER that came from fsps.bin blob. It contains
 * both SiliconInit and Notify APIs. When SiliconInit is loaded the
 * header is saved so that when Notify is called we do not have to start
 * header parsing again.
 */
extern struct fsp_header fsps_hdr;

#endif /* _FSP2_0_INFO_HEADER_H_ */
