/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _FSP2_0_INFO_HEADER_H_
#define _FSP2_0_INFO_HEADER_H_

#include <rules.h>
#include <stdint.h>
#include <stdlib.h>
#include <types.h>

#define FSP_HDR_OFFSET			0x94
#define FSP_HDR_LEN			0x48
#define FSP_HDR_SIGNATURE		"FSPH"
#define FSP_HDR_ATTRIB_FSPT		0b0001
#define FSP_HDR_ATTRIB_FSPM		0b0010
#define FSP_HDR_ATTRIB_FSPS		0b0011

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

#if ENV_RAMSTAGE
/*
 * This is a FSP_INFO_HEADER that came from fsps.bin blob. It contains
 * both SiliconInit and Notify APIs. When SiliconInit is loaded the
 * header is saved so that when Notify is called we do not have to start
 * header parsing again.
 */
extern struct fsp_header fsps_hdr;
#endif

#endif /* _FSP2_0_INFO_HEADER_H_ */
