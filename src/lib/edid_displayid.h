/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __EDID_DISPLAYID_H__
#define __EDID_DISPLAYID_H__

/* DisplayID section structure, version 2.0 */
#define DISPLAY_ID_STRUCTURE_VERSION_20		0x20

/* Data block types */
#define DATA_BLOCK_V2_PRODUCT_ID			0x20
#define DATA_BLOCK_V2_DISPLAY_PARAMETERS		0x21
#define DATA_BLOCK_V2_TYPE_7_DETAILED_TIMING		0x22
#define DATA_BLOCK_V2_TYPE_8_ENUMERATED_TIMING		0x23
#define DATA_BLOCK_V2_TYPE_9_FORMULA_TIMING		0x24
#define DATA_BLOCK_V2_DYNAMIC_VIDEO_TIMING		0x25
#define DATA_BLOCK_V2_DISPLAY_INTERFACE_FEATURES	0x26
#define DATA_BLOCK_V2_STEREO_DISPLAY_INTERFACE		0x27
#define DATA_BLOCK_V2_TILED_DISPLAY_TOPOLOGY		0x28
#define DATA_BLOCK_V2_CONTAINER_ID			0x29
#define DATA_BLOCK_V2_VENDOR_SPECIFIC			0x7E
#define DATA_BLOCK_V2_CTA_DISPLAY_ID			0x81

struct displayid_section_header {
	u8 revision;
	u8 length;
	u8 product_type;
	u8 extension_count;
} __packed;

struct displayid_block_header {
	u8 tag;
	u8 revison;
	u8 length;
} ___packed;

struct displayid_type_7_detailed_timing_desc {
	u8 pixel_clock[3];
	u8 flags;
	u8 hactive[2];
	u8 hblank[2];
	u8 hfront_porch[2];
	u8 hsync_width[2];
	u8 vactive[2];
	u8 vblank[2];
	u8 vfront_porch[2];
	u8 vsync_width[2];
} __packed;
_Static_assert(sizeof(struct displayid_type_7_detailed_timing_desc) == 20,
	       "The size does not meet spec");

struct displayid_type_7_detailed_timing_block {
	struct displayid_block_header header;
	struct displayid_type_7_detailed_timing_desc descs[];
};

#endif /* __EDID_DISPLAYID_H__ */
