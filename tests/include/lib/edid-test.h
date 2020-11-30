/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <commonlib/bsd/helpers.h>


struct edid_raw {
	uint8_t header[8];

	/* Display product identification */
	uint16_t manufacturer_id;
	uint16_t product_code;
	uint32_t serial_number;
	uint8_t manufacture_week;
	uint8_t manufacture_year;

	/* EDID version information */
	uint8_t edid_version;
	uint8_t edid_revision;

	/* Basic display parameters */
	uint8_t video_input_type;
	uint8_t horizontal_size; /* [cm] */
	uint8_t vertical_size; /* [cm] */
	uint8_t display_gamma;
	uint8_t supported_features;

	/* Color space definition */
	uint8_t color_characteristics[10];

	/* Timing information */
	uint8_t established_supported_timings[2];
	uint8_t manufacturers_reserved_timing;
	uint8_t standard_timings_supported[16];
	uint8_t descriptor_block_1[18];
	uint8_t descriptor_block_2[18];
	uint8_t descriptor_block_3[18];
	uint8_t descriptor_block_4[18];

	/* Number of optional 128-byte extension blocks */
	uint8_t extension_flag;

	uint8_t checksum;
} __packed;

_Static_assert(sizeof(struct edid_raw) == 128,
	       "assert failed: edid_raw size mismatch");

#define EDID_HEADER_RAW		{ 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00 }
#define EDID_HEADER_INVALID_RAW	{ 0, 0, 0, 0, 0, 0, 0, 0 }

#define EDID_MANUFACTURER_ID		0xcb55
#define EDID_MANUFACTURER_NAME		"UNK"
#define EDID_PRODUCT_CODE		0x1234
#define EDID_SERIAL_NUMBER		0x56789ABC
#define EDID_MANUFACTURE_WEEK		23u
#define EDID_MANUFACTURE_NO_WEEK	0u
#define EDID_MANUFACTURE_YEAR		(2015u - 1990u)

/* Video Input Definition for Analog Video Signal Interface */
#define EDID_ANALOG_VSI			(0u << 7)
#define EDID_SIGNAL_LEVEL_0		0u
#define EDID_SIGNAL_LEVEL_1		(1u << 5)
#define EDID_SIGNAL_LEVEL_2		(2u << 5)
#define EDID_SIGNAL_LEVEL_3		(3u << 5)
#define EDID_VIDEO_SETUP_BLANK_EQ_BLACK 0u
#define EDID_VIDEO_SETUP_BLANK_TO_BLACK (1u << 4)
#define EDID_SEPARATE_SYNC_H_AND_V(v)	((v != 0 ? 0x1 : 0x0) << 3)
#define EDID_COMPOSITE_SYNC_H(v)	((v != 0 ? 0x1 : 0x0) << 2)
#define EDID_COMPOSITE_SYNC_ON_GREEN(v)	((v != 0 ? 0x1 : 0x0) << 1)
#define EDID_SERRATION_VSYNC(v)		(v != 0 ? 0x1 : 0x0)

/* Video Input Definition for Digital Video Signal Interface */
#define EDID_DIGITAL_VSI		(1u << 7)
#define EDID_COLOR_BIT_DEPTH_UNDEFINED	0u
#define EDID_COLOR_BIT_DEPTH_6B		(1u << 4)
#define EDID_COLOR_BIT_DEPTH_8B		(2u << 4)
#define EDID_COLOR_BIT_DEPTH_10B	(3u << 4)
#define EDID_COLOR_BIT_DEPTH_12B	(4u << 4)
#define EDID_COLOR_BIT_DEPTH_14B	(5u << 4)
#define EDID_COLOR_BIT_DEPTH_16B	(6u << 4)
#define EDID_INTERFACE_UNDEFINED	0u
#define EDID_INTERFACE_DVI		1u
#define EDID_INTERFACE_HDMI_A		2u
#define EDID_INTERFACE_HDMI_B		3u
#define EDID_INTERFACE_MDDI		4u
#define EDID_INTERFACE_DP		5u

/* BEGIN Supported features */
#define EDID_STANDBY_MODE(v)				((v != 0 ? 0x1 : 0x0) << 7)
#define EDID_SUSPEND_MODE(v)				((v != 0 ? 0x1 : 0x0) << 6)
#define EDID_ACTIVE_OFF(v)				((v != 0 ? 0x1 : 0x0) << 5)
/* For analog interface */
#define EDID_COLOR_TYPE_MONO				0u
#define EDID_COLOR_TYPE_RGB				(1u << 3)
#define EDID_COLOR_TYPE_NON_RGB				(2u << 3)
#define EDID_COLOR_TYPE_UNDEFINED			(3u << 3)
/* For digital interface */
#define EDID_COLOR_FORMAT_RGB444			0u
#define EDID_COLOR_FORMAT_RGB444_YCRCB444		(1u << 3)
#define EDID_COLOR_FORMAT_RGB444_YCRCB422		(2u << 3)
#define EDID_COLOR_FORMAT_RGB444_YCRCB422_YCRCB422	(3u << 3)

#define EDID_SRGB_SUPPORTED(v)				(((v) == 0 ? 0u : 1u) << 2)
#define EDID_PREFERRED_TIMING_EXTENDED_INFO		(1u << 1)
#define EDID_PREFERRED_TIMING_NO_EXTENDED_INFO		0u
#define EDID_DISPLAY_FREQUENCY_CONTINUOUS		1u
#define EDID_DISPLAY_FREQUENCY_NON_CONTINUOUS		0u
/* END Supported features */

/* Red X 0.640 */
#define EDID_COLOR_R_X	0x25
/* Red Y 0.330 */
#define EDID_COLOR_R_Y	0x152
/* Green X 0.300 */
#define EDID_COLOR_G_X	0x13a
/* Green Y 0.600 */
#define EDID_COLOR_G_Y	0x267
/* Blue X 0.150 */
#define EDID_COLOR_B_X	0x9a
/* Blue Y 0.060 */
#define EDID_COLOR_B_Y	0x3e
/* White X 0.3125 */
#define EDID_COLOR_W_X	0xa
/* White Y 0.3291 */
#define EDID_COLOR_W_Y	0x22a

/* 1 and 0 bits of each color */
#define EDID_COLOR_R_X10_Y10	(((EDID_COLOR_R_X & 0x3) << 2) | (EDID_COLOR_R_Y & 0x3))
#define EDID_COLOR_G_X10_Y10	(((EDID_COLOR_G_X & 0x3) << 2) | (EDID_COLOR_G_Y & 0x3))
#define EDID_COLOR_B_X10_Y10	(((EDID_COLOR_B_X & 0x3) << 2) | (EDID_COLOR_B_Y & 0x3))
#define EDID_COLOR_W_X10_Y10	(((EDID_COLOR_W_X & 0x3) << 2) | (EDID_COLOR_W_Y & 0x3))

/* Concatenated 0 and 1 bits of each color. To be put
 * as first and second byte of color characteristic. */
#define EDID_COLOR_RG_XY	((EDID_COLOR_R_X10_Y10 << 4) | EDID_COLOR_G_X10_Y10)
#define EDID_COLOR_BW_XY	((EDID_COLOR_B_X10_Y10 << 4) | EDID_COLOR_W_X10_Y10)

/* Bits 9 through 2 of each color */
#define EDID_COLOR_R_X92	(EDID_COLOR_R_X >> 2)
#define EDID_COLOR_R_Y92	(EDID_COLOR_R_Y >> 2)
#define EDID_COLOR_G_X92	(EDID_COLOR_G_X >> 2)
#define EDID_COLOR_G_Y92	(EDID_COLOR_G_Y >> 2)
#define EDID_COLOR_B_X92	(EDID_COLOR_B_X >> 2)
#define EDID_COLOR_B_Y92	(EDID_COLOR_B_Y >> 2)
#define EDID_COLOR_W_X92	(EDID_COLOR_W_X >> 2)
#define EDID_COLOR_W_Y92	(EDID_COLOR_W_Y >> 2)

#define EDID_ESTABLISHED_TIMINGS_1_800x600_60Hz		1u
#define EDID_ESTABLISHED_TIMINGS_1_800x600_56Hz		(1u << 1)
#define EDID_ESTABLISHED_TIMINGS_1_640x480_75Hz		(1u << 2)
#define EDID_ESTABLISHED_TIMINGS_1_640x480_72Hz		(1u << 3)
#define EDID_ESTABLISHED_TIMINGS_1_640x480_67Hz		(1u << 4)
#define EDID_ESTABLISHED_TIMINGS_1_640x480_60Hz		(1u << 5)
#define EDID_ESTABLISHED_TIMINGS_1_720x400_88Hz		(1u << 6)
#define EDID_ESTABLISHED_TIMINGS_1_720x400_70Hz		(1u << 7)

#define EDID_ESTABLISHED_TIMINGS_2_1280x1024_75Hz	1u
#define EDID_ESTABLISHED_TIMINGS_2_1024x768_75Hz	(1u << 1)
#define EDID_ESTABLISHED_TIMINGS_2_1024x768_70Hz	(1u << 2)
#define EDID_ESTABLISHED_TIMINGS_2_1024x768_60Hz	(1u << 3)
#define EDID_ESTABLISHED_TIMINGS_2_1024x768_80HzI	(1u << 4)
#define EDID_ESTABLISHED_TIMINGS_2_832x624_75Hz		(1u << 5)
#define EDID_ESTABLISHED_TIMINGS_2_800x600_75Hz		(1u << 6)
#define EDID_ESTABLISHED_TIMINGS_2_800x600_72Hz		(1u << 7)

#define EDID_MANUFACTURERS_TIMINGS_1152x870_75Hz	(1u << 7)

#define EDID_HORIZONTAL_ACCESSIBLE_PIXELS(px)	(((px) / 8 - 31) & 0xFF)
#define EDID_ASPECT_RATIO_16_10			0u
#define EDID_ASPECT_RATIO_4_3			(1u << 6)
#define EDID_ASPECT_RATIO_5_4			(2u << 6)
#define EDID_ASPECT_RATIO_16_9			(3u << 6)
#define EDID_FIELD_REFRESH_RATE(hz)		(((hz) - 60) & 0x1f)

#define EDID_PIXEL_CLOCK(v)			(((v) / 10000) & 0xFFFF)

#define EDID_RAW_DEFAULT_PARAMS .header = EDID_HEADER_RAW, \
				.edid_version = 1, \
				.edid_revision = 4, \
				.manufacturer_id = EDID_MANUFACTURER_ID, \
				.product_code = EDID_PRODUCT_CODE, \
				.serial_number = EDID_SERIAL_NUMBER, \
				.manufacture_week = EDID_MANUFACTURE_NO_WEEK, \
				.manufacture_year = EDID_MANUFACTURE_YEAR, \
				.color_characteristics = { \
					EDID_COLOR_RG_XY, \
					EDID_COLOR_BW_XY, \
					EDID_COLOR_R_X92, \
					EDID_COLOR_R_Y92, \
					EDID_COLOR_G_X92, \
					EDID_COLOR_G_Y92, \
					EDID_COLOR_B_X92, \
					EDID_COLOR_B_Y92, \
					EDID_COLOR_W_X92, \
					EDID_COLOR_W_Y92, \
				}
