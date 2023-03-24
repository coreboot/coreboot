/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_MENDOCINO_MSR_H
#define AMD_MENDOCINO_MSR_H

/* MSRC001_00[6B:64] P-state [7:0] bit definitions */
union pstate_msr {
	struct {
		uint64_t cpu_fid_0_7	:  8; /* [ 0.. 7] */
		uint64_t cpu_dfs_id	:  6; /* [ 8..13] */
		uint64_t cpu_vid_0_7	:  8; /* [14..21] */
		uint64_t idd_value	:  8; /* [22..29] */
		uint64_t idd_div	:  2; /* [30..31] */
		uint64_t cpu_vid_8	:  1; /* [32..32] */
		uint64_t		: 30; /* [33..62] */
		uint64_t pstate_en	:  1; /* [63..63] */
	};
	uint64_t raw;
};

#define MSR_CPPC_CAPABILITY_1				0xc00102b0
#define SHIFT_CPPC_CAPABILITY_1_HIGHEST_PERF		24
#define SHIFT_CPPC_CAPABILITY_1_NOMINAL_PERF		16
#define SHIFT_CPPC_CAPABILITY_1_LOW_NON_LIN_PERF	8
#define SHIFT_CPPC_CAPABILITY_1_LOWEST_PERF		0

#define MSR_CPPC_ENABLE				0xc00102b1
#define MSR_CPPC_REQUEST			0xc00102b3
#define SHIFT_CPPC_REQUEST_ENERGY_PERF_PREF	24
#define SHIFT_CPPC_REQUEST_DES_PERF		16
#define SHIFT_CPPC_REQUEST_MIN_PERF		8
#define SHIFT_CPPC_REQUEST_MAX_PERF		0

#define MSR_CPPC_STATUS		0xc00102b4

#define MSR_MAX_PERFORMANCE_FREQUENCY_CLOCK_COUNT	0xe7
#define MSR_ACTUAL_PERFORMANCE_FREQUENCY_CLOCK_COUNT	0xe8

#endif /* AMD_MENDOCINO_MSR_H */
