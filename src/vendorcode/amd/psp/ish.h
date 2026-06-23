/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _AMD_PSP_PSP_IMAGE_SLOT_HEADER_H_
#define  _AMD_PSP_PSP_IMAGE_SLOT_HEADER_H_

#include <commonlib/bsd/compiler.h>
#include <stdint.h>

struct ish_directory_table {
	uint32_t checksum;
	uint32_t boot_priority;
	uint32_t update_retry_count;
	uint8_t  glitch_retry_count;
	uint8_t  glitch_higherbits_reserved[3];
	uint32_t pl2_location;
	uint32_t psp_id;
	uint32_t slot_max_size;
	uint32_t reserved;
} __packed;

#endif  /* _AMD_PSP_PSP_IMAGE_SLOT_HEADER_H_ */
