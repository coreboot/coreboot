/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_STONEYRIDGE_MSR_H
#define AMD_STONEYRIDGE_MSR_H

/* MSRC001_00[6B:64] P-state [7:0] bit definitions */
union pstate_msr {
	struct {
		uint64_t cpu_fid_0_5	:  6; /* [ 0.. 5] */
		uint64_t cpu_dfs_id	:  3; /* [ 6.. 8] */
		uint64_t cpu_vid_0_7	:  8; /* [ 9..16] */
		uint64_t		:  5; /* [17..21] */
		uint64_t nb_pstate	:  1; /* [22..22] */
		uint64_t		:  9; /* [23..31] */
		uint64_t idd_value	:  8; /* [32..39] */
		uint64_t idd_div	:  2; /* [40..41] */
		uint64_t		: 21; /* [42..62] */
		uint64_t pstate_en	:  1; /* [63..63] */
	};
	uint64_t raw;
};

#endif /* AMD_STONEYRIDGE_MSR_H */
