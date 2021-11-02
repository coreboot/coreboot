/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _FSP2_0_INFO_HEADER_H_
#define _FSP2_0_INFO_HEADER_H_

#include <types.h>

#define FSP_HDR_OFFSET			0x94
#define FSP_HDR_SIGNATURE		"FSPH"
#define FSP_HDR_ATTRIB_FSPT		1
#define FSP_HDR_ATTRIB_FSPM		2
#define FSP_HDR_ATTRIB_FSPS		3
#define FSP_IMAGE_ID_LENGTH		8

#if CONFIG(PLATFORM_USES_FSP2_X86_32)
struct fsp_header {
	uint32_t  signature; //FSPH
	uint32_t  header_length;
	uint8_t   res1[2];
	uint8_t   spec_version;
	uint8_t   header_revision;
	uint32_t  image_revision;
	char      image_id[FSP_IMAGE_ID_LENGTH]; // not zero terminated
	uint32_t  image_size;
	uint32_t  image_base;
	uint16_t  image_attribute;
	uint16_t  component_attribute;
	uint32_t  cfg_region_offset;
	uint32_t  cfg_region_size;
	uint32_t  res2;
	uint32_t  temp_ram_init_entry_offset; //initial stack
	uint32_t  res3;
	uint32_t  notify_phase_entry_offset;
	uint32_t  fsp_memory_init_entry_offset;
	uint32_t  temp_ram_exit_entry_offset;
	uint32_t  fsp_silicon_init_entry_offset;
	uint32_t  fsp_multi_phase_si_init_entry_offset;
	uint16_t  extended_image_revision;
	uint16_t  res4;
}  __packed;
#else
#error You need to implement this struct for x86_64 FSP
#endif


enum cb_err fsp_identify(struct fsp_header *hdr, const void *fsp_blob);

/*
 * This is a FSP_INFO_HEADER that came from fsps.bin blob. It contains
 * both SiliconInit and Notify APIs. When SiliconInit is loaded the
 * header is saved so that when Notify is called we do not have to start
 * header parsing again.
 */
extern struct fsp_header fsps_hdr;

#endif /* _FSP2_0_INFO_HEADER_H_ */
