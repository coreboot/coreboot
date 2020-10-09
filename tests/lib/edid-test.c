/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdlib.h>
#include <tests/test.h>
#include <edid.h>
#include <assert.h>
#include <string.h>
#include <lib/edid-test.h>

struct test_state {
	int data_size;
	void *data;
};

static uint8_t get_raw_edid_checksum(const unsigned char *x)
{
	unsigned char sum = 0;
	int i;
	for (i = 0; i < 127; ++i)
		sum += x[i];

	return 256 - sum;
}

static void edid_raw_calc_checksum(struct edid_raw *raw)
{
	raw->checksum = get_raw_edid_checksum((const unsigned char *)raw);
}

static void test_decode_edid_no_edid(void **state)
{
	assert_int_equal(EDID_ABSENT, decode_edid(NULL, 0, NULL));
}

static void test_decode_edid_invalid_header(void **state)
{
	struct edid_raw raw = {
		.header = EDID_HEADER_INVALID_RAW
	};
	raw.checksum = get_raw_edid_checksum((const unsigned char *)&raw);

	assert_int_equal(EDID_ABSENT, decode_edid((unsigned char *)&raw, sizeof(raw), NULL));
}

/* Frame is modified example of an LCD Desktop IT display
 * from VESA E-EDID Standard Release A2.
 */
static int setup_decode_edid_basic_frame(void **state)
{
	struct edid_raw raw = {
		EDID_RAW_DEFAULT_PARAMS,
		.video_input_type = EDID_ANALOG_VSI
			| EDID_SIGNAL_LEVEL_0
			| EDID_VIDEO_SETUP_BLANK_EQ_BLACK
			| EDID_SEPARATE_SYNC_H_AND_V(1)
			| EDID_COMPOSITE_SYNC_H(1)
			| EDID_COMPOSITE_SYNC_ON_GREEN(1)
			| EDID_SERRATION_VSYNC(1),
		.horizontal_size = 43, /* [cm] */
		.vertical_size = 32, /* [cm] */
		.display_gamma = 120, /* 220% */
		.supported_features = EDID_STANDBY_MODE(0)
			| EDID_SUSPEND_MODE(0)
			| EDID_ACTIVE_OFF(1)
			| EDID_COLOR_FORMAT_RGB444
			| EDID_SRGB_SUPPORTED(0)
			| EDID_PREFERRED_TIMING_EXTENDED_INFO
			| EDID_DISPLAY_FREQUENCY_CONTINUOUS,
		.established_supported_timings = {
			[0] = EDID_ESTABLISHED_TIMINGS_1_720x400_70Hz
				| EDID_ESTABLISHED_TIMINGS_1_720x400_88Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_60Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_67Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_72Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_75Hz
				| EDID_ESTABLISHED_TIMINGS_1_800x600_56Hz
				| EDID_ESTABLISHED_TIMINGS_1_800x600_60Hz,
			[1] = EDID_ESTABLISHED_TIMINGS_2_800x600_72Hz
				| EDID_ESTABLISHED_TIMINGS_2_800x600_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_832x624_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_80HzI
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_60Hz
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_70Hz
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_1280x1024_75Hz,
		},
		.manufacturers_reserved_timing = EDID_MANUFACTURERS_TIMINGS_1152x870_75Hz,
		.standard_timings_supported = {
			[0] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1600),
			[1] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(85),

			[2] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1600),
			[3] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(75),

			[4] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1600),
			[5] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(70),

			[6] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1600),
			[7] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(65),

			[8] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1280),
			[9] = EDID_ASPECT_RATIO_5_4 | EDID_FIELD_REFRESH_RATE(85),

			[10] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1280),
			[11] = EDID_ASPECT_RATIO_5_4 | EDID_FIELD_REFRESH_RATE(60),

			[12] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1024),
			[13] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(85),

			[14] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(800),
			[15] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(85),
		},
		.descriptor_block_1 = {
			[0] = EDID_PIXEL_CLOCK(162000000u) & 0xFF,
			[1] = (EDID_PIXEL_CLOCK(162000000u) >> 8) & 0xFF,

			/*
			 * Horizontal Addressable Video is 1600px
			 * Horizontal Blanking is 560px
			 */
			[2] = 0x40, [3] = 0x30, [4] = 0x62,

			/*
			 * Vertical Addressable Video is 1200 lines
			 * Vertical Blanking is 50 lines
			 */
			[5] = 0xB0, [6] = 0x32, [7] = 0x40,

			[8] = 64u, /* Horizontal Front Porch in pixels */
			[9] = 192u, /* Horizontal Pulse Sync Width in pixels */
			[10] = 0x13, /* Vertical Front Porch is 1 line */
			[11] = 0x00, /* Vertical Sync Pulse Width is 3 lines */

			/*
			 * Horizontal Addressable Image Size is 427mm
			 * Vertical Addressable Image Size is 320mm
			 */
			[12] = 0xAB, [13] = 0x40, [14] = 0x11,

			[15] = 0x00, /* Horizontal border size is 0px*/
			[16] = 0x00, /* Vertical Border Size is 0px */

			/*
			 * Timing is Non-Interlaced Video,
			 * Stereo Video is not supported,
			 * Digital separate syncs are requires.
			 * */
			[17] = 0x1E,
		},
		.descriptor_block_2 = {
			/* Display Range Limits Block Tag */
			[0] = 0, [1] = 0, [2] = 0, [3] = 0xFD,

			[4] = 0, /* Horizontal and Vertical Rate Offsets are zero */
			[5] = 50u, /* Minimum Vertical Freq is 50Hz */
			[6] = 90u, /* Maximum Vertical Freq is 90Hz */

			[7] = 30u, /* Minimum Horizontal Freq is 30kHz */
			[8] = 110u, /* Maximum Horizontal Freq is 110kHz */
			[9] = 23u, /* Maximum Pixel Clock Freq i 230MHz */
			[10] = 0x4, /* Begin CVT Support Info */
			[11] = 0x11, /* Compatible with CVT Version 1.1 */
			[12] = 0, /* Maimum Pixel Clock Freq remains at 230MHz */
			[13] = 200, /* Maximum Active Pixels per Pile is 1600 */
			[14] = 0x90, /* Supported aspect ratios: 4:3, 5:4 */

			/* Preferred Aspect Ratio is 4:3, Standard CVT Blanking is supported */
			[15] = 0,
			[16] = 0x50, /* H. & V. Stretch are supported and Shrinks are not */
			[17] = 60u, /* Preferred Refresh Rate is 60Hz */
		},
		.descriptor_block_3 = {
			/* Established Timings III Block Tag */
			[0] = 0, [1] = 0, [2] = 0, [3] = 0xF7, [4] = 0,

			[5] = 10u, /* VESA DMT Standard Version #10 */
			/*
			 * 640x350@85Hz,
			 * 640x400@85Hz,
			 * 720x400@85Hz,
			 * 640x480@85Hz,
			 * 800x600@85Hz,
			 * 1024x768@85Hz,
			 * 1152x864@75Hz
			 */
			[6] = 0x7F,

			/*
			 * 1280x960@60Hz,
			 * 1280x960@85Hz,
			 * 1280x1024@60Hz,
			 * 1280x1024@85Hz
			 */
			[7] = 0x0F,

			/*
			 * 1400x1050@60Hz (Normal Blanking),
			 * 1400x1050@75Hz are supported.
			 */
			[8] = 0x03,

			/*
			 * 1400x1050@85Hz,
			 * 1600x1200@60Hz,
			 * 1600x1200@65Hz,
			 * 1600x1200@70Hz are supported.
			 */
			[9] = 0x87,

			/*
			 * 1600x1200@75Hz,
			 * 1600x1200@85Hz are supported.
			 */
			[10] = 0xC0,

			/* 1920 timings not supported */
			[11] = 0x0,

			[12 ... 17] = 0,
		},
		.descriptor_block_4 = {
			/* Display Product Name Block Tag */
			[0] = 0, [1] = 0, [2] = 0, [3] = 0xFC, [4] = 0,

			/* Product name */
			[5] = 'A',
			[6] = 'B',
			[7] = 'C',
			[8] = ' ',
			[9] = 'L',
			[10] = 'C',
			[11] = 'D',
			[12] = '2',
			[13] = '1',
			[14] = '\n',
			[15] = ' ',
			[16] = ' ',
			[17] = ' ',
		},
		.extension_flag = 0x0, /* No extensions */
	};

	edid_raw_calc_checksum(&raw);

	*state = malloc(sizeof(struct test_state));

	struct test_state ts = {
		.data_size = sizeof(struct edid_raw),
		.data = malloc(sizeof(struct edid_raw))
	};

	memcpy(ts.data, &raw, sizeof(raw));
	memcpy(*state, &ts, sizeof(ts));

	return 0;
}

/* Test decoding of EDID frame without extensions.
 */
static void test_decode_edid_basic_frame(void **state)
{
	struct edid out;
	struct test_state *ts = *state;

	/* In real-life situations frames often are not 100% conformant,
	 * but are at least correct when it comes to key data fields.
	 */
	assert_int_equal(EDID_CONFORMANT,
			decode_edid((unsigned char *)ts->data, ts->data_size, &out));

	assert_int_equal(32, out.framebuffer_bits_per_pixel);
	assert_int_equal(0, out.panel_bits_per_color);
	assert_int_equal(0, out.panel_bits_per_pixel);
	assert_int_equal(0, out.link_clock);
	assert_int_equal(1600, out.x_resolution);
	assert_int_equal(1200, out.y_resolution);
	assert_int_equal(6400, out.bytes_per_line);
	assert_int_equal(0, out.hdmi_monitor_detected);
	assert_int_equal(0, strnlen(out.ascii_string, ARRAY_SIZE(out.ascii_string)));
	assert_string_equal(out.manufacturer_name, EDID_MANUFACTURER_NAME);

	/* Mode */
	assert_null(out.mode.name);
	assert_int_equal(162000, out.mode.pixel_clock);
	assert_int_equal(1, out.mode.lvds_dual_channel);
	assert_int_equal(0, out.mode.refresh);
	assert_int_equal(1600, out.mode.ha);
	assert_int_equal(560, out.mode.hbl);
	assert_int_equal(64, out.mode.hso);
	assert_int_equal(192, out.mode.hspw);
	assert_int_equal(0, out.mode.hborder);
	assert_int_equal(1200, out.mode.va);
	assert_int_equal(50, out.mode.vbl);
	assert_int_equal(1, out.mode.vso);
	assert_int_equal(0, out.mode.vborder);
	assert_int_equal(43, out.mode.phsync);
	assert_int_equal(43, out.mode.pvsync);
	assert_int_equal(0, out.mode.x_mm);
	assert_int_equal(0, out.mode.y_mm);

	assert_int_equal(1, out.mode_is_supported[EDID_MODE_640x480_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_720x480_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_1280x720_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_1920x1080_60Hz]);
}

/* Frame is modified example of base EDID frame with CEA861 extension
 * for DTV Display from VESA E-EDID Standard Release A2.
 */
static int setup_decode_edid_dtv_frame_with_extension(void **state)
{
	struct edid_raw raw = {
		EDID_RAW_DEFAULT_PARAMS,
		.video_input_type = EDID_DIGITAL_VSI
			| EDID_INTERFACE_HDMI_A
			| EDID_COLOR_BIT_DEPTH_8B,
		.horizontal_size = 16, /* Aspect ratio 16:9 in landscape */
		.vertical_size = 0, /* Landscape flag */
		.display_gamma = 120, /* 220% */
		.supported_features = EDID_STANDBY_MODE(0)
			| EDID_SUSPEND_MODE(0)
			| EDID_ACTIVE_OFF(0)
			| EDID_COLOR_FORMAT_RGB444_YCRCB422_YCRCB422
			| EDID_SRGB_SUPPORTED(1)
			| EDID_PREFERRED_TIMING_EXTENDED_INFO
			| EDID_DISPLAY_FREQUENCY_NON_CONTINUOUS,

		.established_supported_timings = {
			[0] = EDID_ESTABLISHED_TIMINGS_1_640x480_60Hz,
			[1] = 0,
		},
		.manufacturers_reserved_timing = 0,
		.standard_timings_supported = { [0 ... 15] = 0, },
		.descriptor_block_1 = {
			[0] = EDID_PIXEL_CLOCK(148500000u) & 0xFF,
			[1] = (EDID_PIXEL_CLOCK(148500000u) >> 8) & 0xFF,

			/* Horizontal Addressable Video is 1920px
			 * Horizontal Blanking is 280px
			 */
			[2] = 0x80, [3] = 0x18, [4] = 0x71,

			/* Vertical Addressable Video is 1080 lines
			 * Vertical Blanking is 45 lines
			 */
			[5] = 0x38, [6] = 0x2D, [7] = 0x40,

			[8] = 88u, /* Horizontal Front Porch in pixels */
			[9] = 44u, /* Horizontal Pulse Sync Width in pixels */
			[10] = 4u, /* Vertical Front Porch is 4 lines */
			[11] = 5u, /* Vertical Sync Pulse Width is 5 lines */

			/* Horizontal Addressable Image Size is 1039mm
			 * Vertical Addressable Image Size is 584mm
			 */
			[12] = 0x0F, [13] = 0x48, [14] = 0x42,

			[15] = 0x00, /* Horizontal border size is 0px*/
			[16] = 0x00, /* Vertical Border Size is 0px */

			/* Timing is Non-Interlaced Video,
			 * Stereo Video is not supported,
			 * Digital separate and syncs are requires.
			 */
			[17] = 0x1E,
		},
		.descriptor_block_2 = {
			[0] = EDID_PIXEL_CLOCK(74250000u) & 0xFF,
			[1] = (EDID_PIXEL_CLOCK(74250000u) >> 8) & 0xFF,

			/* Horizontal Addressable Video is 1920px
			 * Horizontal Blanking is 280px
			 */
			[2] = 0x80, [3] = 0x18, [4] = 0x71,

			/* Vertical Addressable Video is 540 lines
			 * Vertical Blanking is 22 lines
			 */
			[5] = 0x1C, [6] = 0x16, [7] = 0x20,

			[8] = 88u, /* Horizontal Front Porch in pixels */
			[9] = 44u, /* Horizontal Pulse Sync Width in pixels */
			[10] = 0x25, /* Vertical Front Porch is 2 lines */
			[11] = 0x00, /* Vertical Sync Pulse Width is 5 lines */

			/* Horizontal Addressable Image Size is 1039mm
			 * Vertical Addressable Image Size is 584mm
			 */
			[12] = 0x0F, [13] = 0x48, [14] = 0x42,

			[15] = 0x00, /* Horizontal border size is 0px*/
			[16] = 0x00, /* Vertical Border Size is 0px */

			/* Timing is Interlaced Video,
			 * Stereo Video is not supported,
			 * Digital separate and syncs are requires.
			 */
			[17] = 0x9E,
		},
		.descriptor_block_3 = {
			[0] = EDID_PIXEL_CLOCK(74250000u) & 0xFF,
			[1] = (EDID_PIXEL_CLOCK(74250000u) >> 8) & 0xFF,

			/* Horizontal Addressable Video is 1280px
			 * Horizontal Blanking is 370px
			 */
			[2] = 0x00, [3] = 0x72, [4] = 0x51,

			/* Vertical Addressable Video is 720 lines
			 * Vertical Blanking is 30 lines
			 */
			[5] = 0xD0, [6] = 0x1E, [7] = 0x20,

			[8] = 110u, /* Horizontal Front Porch in pixels */
			[9] = 40u, /* Horizontal Pulse Sync Width in pixels */
			[10] = 0x55u, /* Vertical Front Porch is 5 lines */
			[11] = 0x00, /* Vertical Sync Pulse Width is 5 lines */

			/* Horizontal Addressable Image Size is 1039mm
			 * Vertical Addressable Image Size is 584mm
			 */
			[12] = 0x0F, [13] = 0x48, [14] = 0x42,

			[15] = 0x00, /* Horizontal border size is 0px*/
			[16] = 0x00, /* Vertical Border Size is 0px */

			/* Timing is Non-Interlaced Video,
			 * Stereo Video is not supported,
			 * Digital separate syncs are requires.
			 */
			[17] = 0x1E,
		},
		.descriptor_block_4 = {
			/* Display Product Name Block Tag */
			[0] = 0, [1] = 0, [2] = 0, [3] = 0xFC, [4] = 0,

			/* Product name */
			[5] = 'A',
			[6] = 'B',
			[7] = 'C',
			[8] = ' ',
			[9] = 'L',
			[10] = 'C',
			[11] = 'D',
			[12] = '4',
			[13] = '7',
			[14] = 'w',
			[15] = '\n',
			[16] = ' ',
			[17] = ' ',
		},
		.extension_flag = 0x0, /* No extensions */
	};

	edid_raw_calc_checksum(&raw);

	unsigned char ext[128] = {
		[0] = 0x02, /* CEA 861 Extension Block Tag Code */
		[1] = 0x03, /* CEA 861 Block Version */

		[2] = 0x18, /* Detail Timing Descriptors start 0x18 bytes from here */

		/* Underscan is not supported
		 * Basic Audio is supported
		 * YCbCr 4:4:4 & YCbCr 4:2:2 are supported
		 * Number of native formats: 2
		 */
		[3] = 0x72,

		/* Video Data Block Tag Code is 2
		 * Number of Short Video Descriptor Bytes i 7
		 */
		[4] = 0x47,

		/* 1920x1080p 59.94/60 Hz 16 : 9 AR (CEA Format #16)
		 * is a supported Native Format. */
		[5] = 0x90,

		/* 1920x1080i 59.94/60 Hz 16 : 9 AR (CEA Format #5)
		 * is a supported Native Format. */
		[6] = 0x85,

		/* 1280x720p 59.94/60 Hz 16 : 9 AR (CEA Format #4) is a supported format. */
		[7] = 0x04,

		/* 720x480p 59.94/60 Hz 16 : 9 AR (CEA Format #3) is a supported format. */
		[8] = 0x03,

		/* 720x480p 59.94/60 Hz 4 : 3 AR (CEA Format #2) is a supported format. */
		[9] = 0x02,

		/* 720x480i 59.94/60 Hz 16 : 9 AR (CEA Format #7) is a supported format. */
		[10] = 0x07,

		/* 720x480i 59.94/60 Hz 4 : 3 AR (CEA Format #6) is a supported format. */
		[11] = 0x06,

		/* Audio Data Block Tag Code is 1.
		 * Number of Short Audio Descriptor Bytes is 3.
		 */
		[12] = 0x23,

		/* Audio Format Tag Code is 1 --- LPCM is supported.
		 * Maximum number of audio channels is 2
		 */
		[13] = 0x09,

		/* Supported Sampling Frequencies include: 48kHz; 44.1kHz & 32kHz. */
		[14] = 0x07,

		/* Supported Sampling Bit Rates include: 24 bit; 20 bit & 16 bit. */
		[15] = 0x07,

		/* Speaker Allocation Block Tag Code is 4.
		 * Number of Speaker Allocation
		 * Descriptor Bytes is 3.
		 */
		[16] = 0x83,

		/* Speaker Allocation is Front-Left & Front-Right */
		[17] = 0x01,

		/* Reserved */
		[18 ... 19] = 0,

		/* Vendor Specific Data Block Tag Code is 3.
		 * Number of Vendor Specific Data Bytes is 5.
		 */
		[20] = 0x65,

		/* 24bit IEEE registration Identifier is 0x000C03 */
		[21] = 0x03, [22] = 0x0C, [23] = 0x00,

		/* Vendor Specific Data is 0x10000 */
		[24] = 0x01, [25] = 0x00,

		/* Descriptor Block 5 [18 Bytes] */

		[26] = EDID_PIXEL_CLOCK(27027000u) & 0xFF,
		[27] = (EDID_PIXEL_CLOCK(27027000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 720px.
		 * Horizontal Blanking is 138 px.
		 */
		[28] = 0xD0, [29] = 0x8A, [30] = 0x20,

		/* Vertical Addressable Video is 480 lines.
		 * Vertical Blanking is 45 lines.
		 */
		[31] = 0xE0, [32] = 0x2D, [33] = 0x10,

		[34] = 16u, /* Horizontal Front Porch in pixels */
		[35] = 62u, /* Horizontal Sync Pulse Width in pixels */
		[36] = 0x96, /* Vertical Front Porch is 9 lines */
		[37] = 0x00, /* Vertical Sync Pulse Width is 6 lines */

		/* Displayed Image Aspect Ratio is 16:9 */
		[38] = 16u, [39] = 9u, [40] = 0u,

		/* Horizontal and Vertical Border Size is 0 px */
		[41] = 0u, [42] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[43] = 0x18,

		/* Descriptor Block 6 [18 Bytes] */

		[44] = EDID_PIXEL_CLOCK(27027000u) & 0xFF,
		[45] = (EDID_PIXEL_CLOCK(27027000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 720px.
		 * Horizontal Blanking is 138 px.
		 */
		[46] = 0xD0, [47] = 0x8A, [48] = 0x20,

		/* Vertical Addressable Video is 480 lines.
		 * Vertical Blanking is 45 lines.
		 */
		[49] = 0xE0, [50] = 0x2D, [51] = 0x10,

		[52] = 16u, /* Horizontal Front Porch in pixels */
		[53] = 62u, /* Horizontal Sync Pulse Width in pixels */
		[54] = 0x96, /* Vertical Front Porch is 9 lines */
		[55] = 0x00, /* Vertical Sync Pulse Width is 6 lines */

		/* Displayed Image Aspect Ratio is 4:3 */
		[56] = 4u, [57] = 3u, [58] = 0u,

		/* Horizontal and Vertical Border Size is 0 px */
		[59] = 0u, [60] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[61] = 0x18,

		/* Descriptor Block 7 [18 Bytes] */

		[62] = EDID_PIXEL_CLOCK(27027000u) & 0xFF,
		[63] = (EDID_PIXEL_CLOCK(27027000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 1440px.
		 * Horizontal Blanking is 276 px.
		 */
		[64] = 0xA0, [65] = 0x14, [66] = 0x51,

		/* Vertical Addressable Video is 240 lines.
		 * Vertical Blanking is 23 lines.
		 */
		[67] = 0xF0, [68] = 0x16, [69] = 0x00,

		[70] = 38u, /* Horizontal Front Porch in pixels */
		[71] = 124u, /* Horizontal Sync Pulse Width in pixels */
		[72] = 0x43, /* Vertical Front Porch is 9 lines */
		[73] = 0x00, /* Vertical Sync Pulse Width is 6 lines */

		/* Displayed Image Aspect Ratio is 16:9 */
		[74] = 16u, [75] = 9u, [76] = 0u,

		/* Horizontal and Vertical Border Size is 0 px */
		[77] = 0u, [78] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[79] = 0x98,

		/* Descriptor Block 8 [18 Bytes] */

		[80] = EDID_PIXEL_CLOCK(27027000u) & 0xFF,
		[81] = (EDID_PIXEL_CLOCK(27027000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 1440px.
		 * Horizontal Blanking is 276 px.
		 */
		[82] = 0xA0, [83] = 0x14, [84] = 0x51,

		/* Vertical Addressable Video is 240 lines.
		 * Vertical Blanking is 23 lines.
		 */
		[85] = 0xF0, [86] = 0x16, [87] = 0x00,

		[88] = 38u, /* Horizontal Front Porch in pixels */
		[89] = 124u, /* Horizontal Sync Pulse Width in pixels */
		[90] = 0x43, /* Vertical Front Porch is 9 lines */
		[91] = 0x00, /* Vertical Sync Pulse Width is 6 lines */

		/* Displayed Image Aspect Ratio is 4:3 */
		[92] = 4u, [93] = 3u, [94] = 0u,

		/* Horizontal and Vertical Border Size is 0 px */
		[95] = 0u, [96] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[97] = 0x98,

		[99 ... 126] = 0
	};

	ext[127] = get_raw_edid_checksum(ext);

	*state = malloc(sizeof(struct test_state));

	struct test_state ts = {
		.data_size = sizeof(raw) + sizeof(ext),
		.data = malloc(sizeof(raw) + sizeof(ext))
	};

	memcpy(ts.data, &raw, sizeof(raw));
	memcpy(ts.data + sizeof(raw), &ext[0], sizeof(ext));

	memcpy(*state, &ts, sizeof(ts));

	return 0;
}

/* Test decoding of EDID frame with one extension.
 */
static void test_decode_edid_dtv_frame_with_extension(void **state)
{
	struct edid out;
	struct test_state *ts = *state;

	/* In real-life situations frames often are not 100% conformant,
	 * but are at least correct when it comes to key data fields.
	 */
	assert_int_equal(EDID_CONFORMANT,
			decode_edid((unsigned char *)ts->data, ts->data_size, &out));

	assert_int_equal(32, out.framebuffer_bits_per_pixel);
	assert_int_equal(8, out.panel_bits_per_color);
	assert_int_equal(24, out.panel_bits_per_pixel);
	assert_int_equal(0, out.link_clock);
	assert_int_equal(1920, out.x_resolution);
	assert_int_equal(1080, out.y_resolution);
	assert_int_equal(7680, out.bytes_per_line);
	assert_int_equal(1, out.hdmi_monitor_detected);
	assert_int_equal(0, strnlen(out.ascii_string, ARRAY_SIZE(out.ascii_string)));
	assert_string_equal(out.manufacturer_name, EDID_MANUFACTURER_NAME);

	/* Mode */
	assert_null(out.mode.name);
	assert_int_equal(148500, out.mode.pixel_clock);
	assert_int_equal(1, out.mode.lvds_dual_channel);
	assert_int_equal(0, out.mode.refresh);
	assert_int_equal(1920, out.mode.ha);
	assert_int_equal(280, out.mode.hbl);
	assert_int_equal(88, out.mode.hso);
	assert_int_equal(44, out.mode.hspw);
	assert_int_equal(0, out.mode.hborder);
	assert_int_equal(1080, out.mode.va);
	assert_int_equal(45, out.mode.vbl);
	assert_int_equal(16, out.mode.vso);
	assert_int_equal(0, out.mode.vborder);
	assert_int_equal(43, out.mode.phsync);
	assert_int_equal(43, out.mode.pvsync);
	assert_int_equal(0, out.mode.x_mm);
	assert_int_equal(0, out.mode.y_mm);

	assert_int_equal(1, out.mode_is_supported[EDID_MODE_640x480_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_720x480_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_1280x720_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_1920x1080_60Hz]);
}


/* Test decoding of EDID frame with one extension. Tested frame is modified
 * example of base EDID frame with CEA861 extension for IT/DTV Display from
 * VESA E-EDID Standard Release A2.
 */
static int setup_decode_edid_it_dtv_frame_with_extension(void **state)
{
	struct edid_raw raw = {
		EDID_RAW_DEFAULT_PARAMS,
		.video_input_type = EDID_DIGITAL_VSI
			| EDID_INTERFACE_HDMI_A
			| EDID_COLOR_BIT_DEPTH_8B,
		.horizontal_size = 121, /* Aspect ratio 16:9 in landscape */
		.vertical_size = 68, /* Landscape flag */
		.display_gamma = 120, /* 220% */
		.supported_features = EDID_STANDBY_MODE(0)
			| EDID_SUSPEND_MODE(0)
			| EDID_ACTIVE_OFF(0)
			| EDID_COLOR_FORMAT_RGB444_YCRCB422_YCRCB422
			| EDID_SRGB_SUPPORTED(1)
			| EDID_PREFERRED_TIMING_EXTENDED_INFO
			| EDID_DISPLAY_FREQUENCY_NON_CONTINUOUS,
		.established_supported_timings = {
			[0] = EDID_ESTABLISHED_TIMINGS_1_800x600_60Hz
				| EDID_ESTABLISHED_TIMINGS_1_800x600_56Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_75Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_72Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_67Hz
				| EDID_ESTABLISHED_TIMINGS_1_640x480_60Hz
				| EDID_ESTABLISHED_TIMINGS_1_720x400_88Hz
				| EDID_ESTABLISHED_TIMINGS_1_720x400_70Hz,
			[1] = EDID_ESTABLISHED_TIMINGS_2_1280x1024_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_70Hz
				| EDID_ESTABLISHED_TIMINGS_2_1024x768_60Hz
				| EDID_ESTABLISHED_TIMINGS_2_832x624_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_800x600_75Hz
				| EDID_ESTABLISHED_TIMINGS_2_800x600_72Hz,
		},
		.manufacturers_reserved_timing = EDID_MANUFACTURERS_TIMINGS_1152x870_75Hz,
		.standard_timings_supported = {
			[0] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1280),
			[1] = EDID_ASPECT_RATIO_5_4 | EDID_FIELD_REFRESH_RATE(85),

			[2] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1280),
			[3] = EDID_ASPECT_RATIO_5_4 | EDID_FIELD_REFRESH_RATE(60),

			[4] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1280),
			[5] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(85),

			[6] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1280),
			[7] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(60),

			[8] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(1024),
			[9] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(85),

			[10] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(848),
			[11] = EDID_ASPECT_RATIO_16_9 | EDID_FIELD_REFRESH_RATE(60),

			[12] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(800),
			[13] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(60),

			[14] = EDID_HORIZONTAL_ACCESSIBLE_PIXELS(640),
			[15] = EDID_ASPECT_RATIO_4_3 | EDID_FIELD_REFRESH_RATE(60),
		},
		.descriptor_block_1 = {
			[0] = EDID_PIXEL_CLOCK(85500000u) & 0xFF,
			[1] = (EDID_PIXEL_CLOCK(85500000u) >> 8) & 0xFF,

			/* Horizontal Addressable Video is 1360px
			 * Horizontal Blanking is 432px
			 */
			[2] = 0x50, [3] = 0xB0, [4] = 0x51,

			/* Vertical Addressable Video is 768 lines
			 * Vertical Blanking is 27 lines
			 */
			[5] = 0x00, [6] = 0x1B, [7] = 0x30,

			[8] = 64u, /* Horizontal Front Porch in pixels */
			[9] = 112u, /* Horizontal Pulse Sync Width in pixels */
			[10] = 0x36, /* Vertical Front Porch is 3 lines */
			[11] = 0u, /* Vertical Sync Pulse Width is 6 lines */

			/* Horizontal Addressable Image Size is 1214mm
			 * Vertical Addressable Image Size is 683mm
			 */
			[12] = 0xBE, [13] = 0xAB, [14] = 0x42,

			[15] = 0x00, /* Horizontal border size is 0px*/
			[16] = 0x00, /* Vertical Border Size is 0px */

			/* Timing is Non-Interlaced Video,
			 * Stereo Video is not supported,
			 * Digital separate and syncs are requires.
			 */
			[17] = 0x1E,
		},
		.descriptor_block_2 = {
			[0] = EDID_PIXEL_CLOCK(74250000u) & 0xFF,
			[1] = (EDID_PIXEL_CLOCK(74250000u) >> 8) & 0xFF,

			/* Horizontal Addressable Video is 1280px
			 * Horizontal Blanking is 370px
			 */
			[2] = 0x00, [3] = 0x72, [4] = 0x51,

			/* Vertical Addressable Video is 720 lines
			 * Vertical Blanking is 30 lines
			 */
			[5] = 0xD0, [6] = 0x1E, [7] = 0x20,

			[8] = 110u, /* Horizontal Front Porch in pixels */
			[9] = 40u, /* Horizontal Pulse Sync Width in pixels */
			[10] = 0x55, /* Vertical Front Porch is 5 lines */
			[11] = 0x00, /* Vertical Sync Pulse Width is 5 lines */

			/* Horizontal Addressable Image Size is 1214mm
			 * Vertical Addressable Image Size is 683mm
			 */
			[12] = 0xBE, [13] = 0xAB, [14] = 0x42,

			[15] = 0x00, /* Horizontal border size is 0px*/
			[16] = 0x00, /* Vertical Border Size is 0px */

			/* Timing is Non-Interlaced Video,
			 * Stereo Video is not supported,
			 * Digital separate and syncs are requires.
			 */
			[17] = 0x1E,
		},
		.descriptor_block_3 = {
			/* Established timings III Block Tag */
			[0 ... 2] = 0u, [3] = 0xF7, [4] = 0u,

			/*
			 * VESA DMT Standard Version #10
			 */
			[5] = 10u,

			/* 640x350@85Hz,
			 * 640x400@85Hz,
			 * 720x400@85Hz,
			 * 640x480@85Hz,
			 * 800x600@85Hz,
			 * 1024x768@85Hz,
			 * 1152x864@75Hz are supported.
			 */
			[6] = 0x7F,

			/* 1280x960@60Hz,
			 * 1280x960@85Hz,
			 * 1280x1024@60Hz,
			 * 1280x1024@85Hz
			 */
			[7] = 0x0F,

			/* 1400x1050@60Hz (Normal Blanking),
			 * 1400x1050@75Hz are supported.
			 */
			[8] = 0x03,

			/* 1400x1050@85Hz,
			 * 1600x1200@60Hz,
			 * 1600x1200@65Hz,
			 * 1600x1200@70Hz are supported.
			 */
			[9] = 0x87,

			/* 1600x1200@75Hz,
			 * 1600x1200@85Hz are supported.
			 */
			[10] = 0xC0,

			/* 1920 PC Timings are not supported. */
			[11] = 0u,

			/* Reserved */
			[12 ... 17] = 0,
		},
		.descriptor_block_4 = {
			/* Display Product Name Block Tag */
			[0] = 0, [1] = 0, [2] = 0, [3] = 0xFC, [4] = 0,

			/* Product name */
			[5] = 'A',
			[6] = 'B',
			[7] = 'C',
			[8] = ' ',
			[9] = 'P',
			[10] = 'L',
			[11] = 'A',
			[12] = '5',
			[13] = '5',
			[14] = '\n',
			[15] = ' ',
			[16] = ' ',
			[17] = ' ',
		},
		.extension_flag = 0x0, /* No extensions */
	};

	edid_raw_calc_checksum(&raw);

	unsigned char ext[128] = {
		[0] = 0x02, /* CEA 861 Extension Block Tag Code */
		[1] = 0x03, /* CEA 861 Block Version */
		[2] = 0x17, /* Detail Timing Descriptors start 0x17 bytesfrom here */

		/* Underscan is supported
		 * Basic Audio is supported
		 * YCbCr 4:4:4 & YCbCr 4:2:2 are supported
		 * Number of native formats: 0
		 */
		[3] = 0xF0,

		/* Video Data Block Tag Code is 2
		 * Number of Short Video Descriptor Bytes i 6
		 */
		[4] = 0x46,

		/* 1920x1080i 59.94/60 Hz 16 : 9 AR (CEA Format #5) is a supported format. */
		[5] = 0x05,

		/* 1280x720p 59.94/60 Hz 16 : 9 AR (CEA Format #4) is a supported format. */
		[6] = 0x04,

		/* 720x480p 59.94/60 Hz 16 : 9 AR (CEA Format #3) is a supported format. */
		[7] = 0x03,

		/* 720x480p 59.94/60 Hz 4 : 3 AR (CEA Format #2) is a supported format. */
		[8] = 0x02,

		/* 720x480i 59.94/60 Hz 16 : 9 AR (CEA Format #7) is a supported format. */
		[9] = 0x07,

		/* 720x480i 59.94/60 Hz 4 : 3 AR (CEA Format #6) is a supported format. */
		[10] = 0x06,

		/* Audio Data Block Tag Code is 1.
		 * Number of Short Audio Descriptor Bytes is 3.
		 */
		[11] = 0x23,

		/* Audio Format Tag Code is 1 --- LPCM is supported.
		 * Maximum number of audio channels is 2
		 */
		[12] = 0x09,

		/* Supported Sampling Frequencies include: 48kHz; 44.1kHz & 32kHz. */
		[13] = 0x07,

		/* Supported Sampling Bit Rates include: 24 bit; 20 bit & 16 bit. */
		[14] = 0x07,

		/* Speaker Allocation Block Tag Code is 4.
		 * Number of Speaker Allocation
		 * Descriptor Bytes is 3.
		 */
		[15] = 0x83,

		/* Speaker Allocation is Front-Left & Front-Right */
		[16] = 0x01,

		/* Reserved */
		[17 ... 18] = 0,

		/* Vendor Specific Data Block Tag Code is 3.
		 * Number of Vendor Specific Data Bytes is 5.
		 */
		[19] = 0x65,

		/* 24bit IEEE registration Identifier is 0x000C03 */
		[20] = 0x03, [21] = 0x0C, [22] = 0x00,

		/* Vendor Specific Data is 0x10000 */
		[23] = 0x01, [24] = 0x00,

		/* Descriptor Block 5 [18 Bytes] */

		[25] = EDID_PIXEL_CLOCK(74250000u) & 0xFF,
		[26] = (EDID_PIXEL_CLOCK(74250000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 1920px.
		 * Horizontal Blanking is 280px.
		 */
		[27] = 0x80, [28] = 0x18, [29] = 0x71,

		/* Vertical Addressable Video is 540 lines.
		 * Vertical Blanking is 22 lines.
		 */
		[30] = 0x1C, [31] = 0x16, [32] = 0x20,

		[33] = 88u, /* Horizontal Front Porch in pixels */
		[34] = 44u, /* Horizontal Sync Pulse Width in pixels */
		[35] = 0x25, /* Vertical Front Porch is 2 lines */
		[36] = 0x00, /* Vertical Sync Pulse Width is 5 lines */

		/* Image size: 1039mm x 584mm */
		[37] = 0x0F, [38] = 0x48, [39] = 0x42,

		/* Horizontal and Vertical Border Size is 0 px */
		[40] = 0u, [41] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[42] = 0x9E,

		/* Descriptor Block 6 [18 Bytes] */

		[43] = EDID_PIXEL_CLOCK(74250000u) & 0xFF,
		[44] = (EDID_PIXEL_CLOCK(74250000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 1280px.
		 * Horizontal Blanking is 370 px.
		 */
		[45] = 0x00, [46] = 0x72, [47] = 0x51,

		/* Vertical Addressable Video is 720 lines.
		 * Vertical Blanking is 30 lines.
		 */
		[48] = 0xD0, [49] = 0x1E, [50] = 0x20,

		[51] = 110u, /* Horizontal Front Porch in pixels */
		[52] = 40u, /* Horizontal Sync Pulse Width in pixels */
		[53] = 0x55, /* Vertical Front Porch is 5 lines */
		[54] = 0x00, /* Vertical Sync Pulse Width is 5 lines */

		/* Image size: 1039mm x 584mm */
		[55] = 0x0F, [56] = 0x48, [57] = 0x42,

		/* Horizontal and Vertical Border Size is 0 px */
		[58] = 0u, [59] = 0u,

		/* Timing is Non-Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[60] = 0x1E,

		/* Descriptor Block 7 [18 Bytes] */

		[61] = EDID_PIXEL_CLOCK(27000000u) & 0xFF,
		[62] = (EDID_PIXEL_CLOCK(27000000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 1440px.
		 * Horizontal Blanking is 276 px.
		 */
		[63] = 0xA0, [64] = 0x14, [65] = 0x51,

		/* Vertical Addressable Video is 240 lines.
		 * Vertical Blanking is 23 lines.
		 */
		[66] = 0xF0, [67] = 0x16, [68] = 0x00,

		[69] = 38u, /* Horizontal Front Porch in pixels */
		[70] = 124u, /* Horizontal Sync Pulse Width in pixels */
		[71] = 0x43, /* Vertical Front Porch is 4 lines */
		[72] = 0x00, /* Vertical Sync Pulse Width is 3 lines */

		/* Image size: 1039mm x 584mm */
		[73] = 0x0F, [74] = 0x48, [75] = 0x42,

		/* Horizontal and Vertical Border Size is 0 px */
		[76] = 0u, [77] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[78] = 0x18,

		/* Descriptor Block 8 [18 Bytes] */

		[79] = EDID_PIXEL_CLOCK(27027000u) & 0xFF,
		[80] = (EDID_PIXEL_CLOCK(27027000u) >> 8) & 0xFF,

		/* Horizontal Addressable Video is 1440px.
		 * Horizontal Blanking is 276 px.
		 */
		[81] = 0xA0, [82] = 0x14, [83] = 0x51,

		/* Vertical Addressable Video is 240 lines.
		 * Vertical Blanking is 23 lines.
		 */
		[84] = 0xF0, [85] = 0x16, [86] = 0x00,

		[87] = 38u, /* Horizontal Front Porch in pixels */
		[88] = 124u, /* Horizontal Sync Pulse Width in pixels */
		[89] = 0x43, /* Vertical Front Porch is 4 lines */
		[90] = 0x00, /* Vertical Sync Pulse Width is 3 lines */

		/* Image size: 1039mm x 584mm */
		[91] = 0x0F, [92] = 0x48, [93] = 0x42,

		/* Horizontal and Vertical Border Size is 0 px */
		[94] = 0u, [95] = 0u,

		/* Timing is Interlaced Video
		 * Stereo Video is not supported
		 * Digital Separate Syncs are required
		 */
		[96] = 0x98,

		[97 ... 126] = 0,
	};

	ext[127] = get_raw_edid_checksum(ext);

	*state = malloc(sizeof(struct test_state));

	struct test_state ts = {
		.data_size = sizeof(raw) + sizeof(ext),
		.data = malloc(sizeof(raw) + sizeof(ext))
	};

	memcpy(ts.data, &raw, sizeof(raw));
	memcpy(ts.data + sizeof(raw), &ext[0], sizeof(ext));

	memcpy(*state, &ts, sizeof(ts));

	return 0;
}

static void test_decode_edid_it_dtv_frame_with_extension(void **state)
{
	struct edid out;
	struct test_state *ts = *state;

	/* In real-life situations frames often are not 100% conformant,
	 * but are at least correct when it comes to key data fields.
	 */
	assert_int_equal(EDID_CONFORMANT,
			decode_edid((unsigned char *)ts->data, ts->data_size, &out));

	assert_int_equal(32, out.framebuffer_bits_per_pixel);
	assert_int_equal(8, out.panel_bits_per_color);
	assert_int_equal(24, out.panel_bits_per_pixel);
	assert_int_equal(0, out.link_clock);
	assert_int_equal(1360, out.x_resolution);
	assert_int_equal(768, out.y_resolution);
	assert_int_equal(5440, out.bytes_per_line);
	assert_int_equal(1, out.hdmi_monitor_detected);
	assert_int_equal(0, strnlen(out.ascii_string, ARRAY_SIZE(out.ascii_string)));
	assert_string_equal(out.manufacturer_name, EDID_MANUFACTURER_NAME);

	/* Mode */
	assert_null(out.mode.name);
	assert_int_equal(85500, out.mode.pixel_clock);
	assert_int_equal(0, out.mode.lvds_dual_channel);
	assert_int_equal(0, out.mode.refresh);
	assert_int_equal(1360, out.mode.ha);
	assert_int_equal(432, out.mode.hbl);
	assert_int_equal(64, out.mode.hso);
	assert_int_equal(112, out.mode.hspw);
	assert_int_equal(0, out.mode.hborder);
	assert_int_equal(768, out.mode.va);
	assert_int_equal(27, out.mode.vbl);
	assert_int_equal(3, out.mode.vso);
	assert_int_equal(0, out.mode.vborder);
	assert_int_equal(43, out.mode.phsync);
	assert_int_equal(43, out.mode.pvsync);
	assert_int_equal(0, out.mode.x_mm);
	assert_int_equal(0, out.mode.y_mm);

	assert_int_equal(1, out.mode_is_supported[EDID_MODE_640x480_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_720x480_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_1280x720_60Hz]);
	assert_int_equal(0, out.mode_is_supported[EDID_MODE_1920x1080_60Hz]);
}

static int teardown_edid_test(void **state)
{
	struct test_state *ts;

	if (*state == NULL)
		return 0;

	ts = (struct test_state *)*state;

	free(ts->data);
	free(ts);

	return 0;
}

static void test_edid_set_framebuffer_bits_per_pixel(void **state)
{
	struct edid out;
	struct test_state *ts = *state;

	decode_edid((unsigned char *)ts->data, ts->data_size, &out);

	edid_set_framebuffer_bits_per_pixel(&out, 16, 2);

	assert_int_equal(16, out.framebuffer_bits_per_pixel);
	assert_int_equal(out.mode.ha * 2, out.bytes_per_line);
	assert_int_equal(out.bytes_per_line / (16 / 8), out.x_resolution);
	assert_int_equal(out.mode.va, out.y_resolution);

	edid_set_framebuffer_bits_per_pixel(&out, 24, 4);

	assert_int_equal(24, out.framebuffer_bits_per_pixel);
	assert_int_equal(out.mode.ha * 3, out.bytes_per_line);
	assert_int_equal(out.bytes_per_line / (24 / 8), out.x_resolution);
	assert_int_equal(out.mode.va, out.y_resolution);

	edid_set_framebuffer_bits_per_pixel(&out, 32, 4);

	assert_int_equal(32, out.framebuffer_bits_per_pixel);
	assert_int_equal(out.mode.ha * 4, out.bytes_per_line);
	assert_int_equal(out.bytes_per_line / (32 / 8), out.x_resolution);
	assert_int_equal(out.mode.va, out.y_resolution);
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_decode_edid_no_edid),
		cmocka_unit_test(test_decode_edid_invalid_header),
		cmocka_unit_test_setup_teardown(test_decode_edid_basic_frame,
						setup_decode_edid_basic_frame,
						teardown_edid_test),
		cmocka_unit_test_setup_teardown(test_decode_edid_dtv_frame_with_extension,
						setup_decode_edid_dtv_frame_with_extension,
						teardown_edid_test),
		cmocka_unit_test_setup_teardown(test_decode_edid_it_dtv_frame_with_extension,
						setup_decode_edid_it_dtv_frame_with_extension,
						teardown_edid_test),
		cmocka_unit_test_setup_teardown(test_edid_set_framebuffer_bits_per_pixel,
						setup_decode_edid_basic_frame,
						teardown_edid_test),
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}

