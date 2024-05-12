/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef HASWELL_RAMINIT_NATIVE_H
#define HASWELL_RAMINIT_NATIVE_H

#include <device/dram/ddr3.h>
#include <northbridge/intel/haswell/haswell.h>

/** TODO (Angel): Remove this after in-review patches are submitted **/
#define SPD_LEN SPD_SIZE_MAX_DDR3

/* 8 data lanes + 1 ECC lane */
#define NUM_LANES		9
#define NUM_LANES_NO_ECC	8

enum raminit_boot_mode {
	BOOTMODE_COLD,
	BOOTMODE_WARM,
	BOOTMODE_S3,
	BOOTMODE_FAST,
};

enum raminit_status {
	RAMINIT_STATUS_SUCCESS = 0,
	RAMINIT_STATUS_NO_MEMORY_INSTALLED,
	RAMINIT_STATUS_UNSUPPORTED_MEMORY,
	RAMINIT_STATUS_UNSPECIFIED_ERROR, /** TODO: Deprecated in favor of specific values **/
};

enum generic_stepping {
	STEPPING_A0 = 1,
	STEPPING_B0 = 2,
	STEPPING_C0 = 3,
};

struct raminit_dimm_info {
	spd_ddr3_raw_data raw_spd;
	struct dimm_attr_ddr3_st data;
	uint8_t spd_addr;
	bool valid;
};

struct sysinfo {
	enum raminit_boot_mode bootmode;
	enum generic_stepping stepping;
	uint32_t cpu;		/* CPUID value */

	bool dq_pins_interleaved;

	/** TODO: ECC support untested **/
	bool is_ecc;

	/**
	 * FIXME: LPDDR support is incomplete. The largest chunks are missing,
	 * but some LPDDR-specific variations in algorithms have been handled.
	 * LPDDR-specific functions have stubs which will halt upon execution.
	 */
	bool lpddr;

	struct raminit_dimm_info dimms[NUM_CHANNELS][NUM_SLOTS];
	union dimm_flags_ddr3_st flags;
	uint16_t cas_supported;

	/* Except for tCK, everything is eventually stored in DCLKs */
	uint32_t tCK;
	uint32_t tAA;			/* Also known as tCL */
	uint32_t tWR;
	uint32_t tRCD;
	uint32_t tRRD;
	uint32_t tRP;
	uint32_t tRAS;
	uint32_t tRC;
	uint32_t tRFC;
	uint32_t tWTR;
	uint32_t tRTP;
	uint32_t tFAW;
	uint32_t tCWL;
	uint32_t tCMD;

	uint8_t lanes;			/* 8 or 9 */
	uint8_t chanmap;
	uint8_t dpc[NUM_CHANNELS];	/* DIMMs per channel */
	uint8_t rankmap[NUM_CHANNELS];
	uint8_t rank_mirrored[NUM_CHANNELS];
	uint32_t channel_size_mb[NUM_CHANNELS];
};

void raminit_main(enum raminit_boot_mode bootmode);

enum raminit_status collect_spd_info(struct sysinfo *ctrl);

#endif
