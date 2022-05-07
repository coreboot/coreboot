/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef HASWELL_RAMINIT_NATIVE_H
#define HASWELL_RAMINIT_NATIVE_H

#include <assert.h>
#include <device/dram/ddr3.h>
#include <northbridge/intel/haswell/haswell.h>
#include <string.h>
#include <types.h>

#include "reg_structs.h"

/** TODO (Angel): Remove this after in-review patches are submitted **/
#define SPD_LEN SPD_SIZE_MAX_DDR3

/* Each channel has 4 ranks, spread across 2 slots */
#define NUM_SLOTRANKS		4

#define NUM_GROUPS		2

/* 8 data lanes + 1 ECC lane */
#define NUM_LANES		9
#define NUM_LANES_NO_ECC	8

#define COMP_INT		10

/* Always use 12 legs for emphasis (not trained) */
#define TXEQFULLDRV		(3 << 4)

enum command_training_iteration {
	CT_ITERATION_CLOCK = 0,
	CT_ITERATION_CMD_NORTH,
	CT_ITERATION_CMD_SOUTH,
	CT_ITERATION_CKE,
	CT_ITERATION_CTL,
	CT_ITERATION_CMD_VREF,
	MAX_CT_ITERATION,
};

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
	RAMINIT_STATUS_MPLL_INIT_FAILURE,
	RAMINIT_STATUS_POLL_TIMEOUT,
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
	bool lpddr_dram_odt;
	uint8_t lpddr_cke_rank_map[NUM_CHANNELS];
	uint8_t dq_byte_map[NUM_CHANNELS][MAX_CT_ITERATION][2];

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

	uint32_t tREFI;
	uint32_t tXP;

	uint8_t lanes;			/* 8 or 9 */
	uint8_t chanmap;
	uint8_t dpc[NUM_CHANNELS];	/* DIMMs per channel */
	uint8_t rankmap[NUM_CHANNELS];
	uint8_t rank_mirrored[NUM_CHANNELS];
	uint32_t channel_size_mb[NUM_CHANNELS];

	uint8_t base_freq;		/* Memory base frequency, either 100 or 133 MHz */
	uint32_t multiplier;
	uint32_t mem_clock_mhz;
	uint32_t mem_clock_fs;		/* Memory clock period in femtoseconds */
	uint32_t qclkps;		/* Quadrature clock period in picoseconds */

	uint16_t vdd_mv;

	union ddr_scram_misc_control_reg misc_control_0;

	union ddr_comp_ctl_0_reg comp_ctl_0;
	union ddr_comp_ctl_1_reg comp_ctl_1;

	union ddr_data_vref_adjust_reg dimm_vref;

	uint32_t data_offset_train[NUM_CHANNELS][NUM_LANES];
	uint32_t data_offset_comp[NUM_CHANNELS][NUM_LANES];

	uint32_t dq_control_0[NUM_CHANNELS];
	uint32_t dq_control_1[NUM_CHANNELS][NUM_LANES];
	uint32_t dq_control_2[NUM_CHANNELS][NUM_LANES];

	uint16_t tx_dq[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	uint16_t txdqs[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	uint8_t  tx_eq[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];

	uint16_t rcven[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	uint8_t  rx_eq[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	uint8_t rxdqsp[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	uint8_t rxdqsn[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];
	int8_t  rxvref[NUM_CHANNELS][NUM_SLOTRANKS][NUM_LANES];

	uint8_t clk_pi_code[NUM_CHANNELS][NUM_SLOTRANKS];
	uint8_t ctl_pi_code[NUM_CHANNELS][NUM_SLOTRANKS];
	uint8_t cke_pi_code[NUM_CHANNELS][NUM_SLOTRANKS];

	uint8_t cke_cmd_pi_code[NUM_CHANNELS][NUM_GROUPS];
	uint8_t cmd_north_pi_code[NUM_CHANNELS][NUM_GROUPS];
	uint8_t cmd_south_pi_code[NUM_CHANNELS][NUM_GROUPS];
};

static inline bool is_hsw_ult(void)
{
	return CONFIG(INTEL_LYNXPOINT_LP);
}

static inline bool rank_in_mask(uint8_t rank, uint8_t rankmask)
{
	assert(rank < NUM_SLOTRANKS);
	return !!(BIT(rank) & rankmask);
}

static inline bool does_ch_exist(const struct sysinfo *ctrl, uint8_t channel)
{
	return !!ctrl->dpc[channel];
}

static inline bool does_rank_exist(const struct sysinfo *ctrl, uint8_t rank)
{
	return rank_in_mask(rank, ctrl->rankmap[0] | ctrl->rankmap[1]);
}

static inline bool rank_in_ch(const struct sysinfo *ctrl, uint8_t rank, uint8_t channel)
{
	assert(channel < NUM_CHANNELS);
	return rank_in_mask(rank, ctrl->rankmap[channel]);
}

/** TODO: Handling of data_offset_train could be improved, also coupled with reg updates **/
static inline void clear_data_offset_train_all(struct sysinfo *ctrl)
{
	memset(ctrl->data_offset_train, 0, sizeof(ctrl->data_offset_train));
}

void raminit_main(enum raminit_boot_mode bootmode);

enum raminit_status collect_spd_info(struct sysinfo *ctrl);
enum raminit_status initialise_mpll(struct sysinfo *ctrl);
enum raminit_status convert_timings(struct sysinfo *ctrl);
enum raminit_status configure_mc(struct sysinfo *ctrl);

void configure_timings(struct sysinfo *ctrl);
void configure_refresh(struct sysinfo *ctrl);

enum raminit_status wait_for_first_rcomp(void);

uint8_t get_rx_bias(const struct sysinfo *ctrl);

uint8_t get_tCWL(uint32_t mem_clock_mhz);
uint32_t get_tREFI(uint32_t mem_clock_mhz);
uint32_t get_tXP(uint32_t mem_clock_mhz);

#endif
