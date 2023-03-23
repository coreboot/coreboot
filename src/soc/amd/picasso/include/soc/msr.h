/* SPDX-License-Identifier: GPL-2.0-only */

/* This file applies to AMD64 products.
 * The definitions come from the device's PPR.
 */

#ifndef AMD_PICASSO_MSR_H
#define AMD_PICASSO_MSR_H

/* MSRC001_00[6B:64] P-state [7:0] bit definitions */
union pstate_msr {
	struct {
		uint64_t cpu_fid_0_7	:  8; /* [ 0.. 7] */
		uint64_t cpu_dfs_id	:  6; /* [ 8..13] */
		uint64_t cpu_vid_0_7	:  8; /* [14..21] */
		uint64_t idd_value	:  8; /* [22..29] */
		uint64_t idd_div	:  2; /* [30..31] */
		uint64_t		: 31; /* [32..62] */
		uint64_t pstate_en	:  1; /* [63..63] */
	};
	uint64_t raw;
};

#define PSTATE_DEF_FREQ_DIV_MIN		0x8
#define PSTATE_DEF_EIGHTH_STEP_MAX	0x1A
#define PSTATE_DEF_FREQ_DIV_MAX		0x3E
#define PSTATE_DEF_CORE_FREQ_BASE	25

/* Value defined in Serial VID Interface 2.0 spec (#48022, NDA only) */
#define  SERIAL_VID_2_DECODE_MICROVOLTS	6250
#define  SERIAL_VID_2_MAX_MICROVOLTS	1550000L

#endif /* AMD_PICASSO_MSR_H */
