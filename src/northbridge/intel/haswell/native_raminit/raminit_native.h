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

/* DDR3 mode register bits */
#define MR0_DLL_RESET		BIT(8)

#define MR1_WL_ENABLE		BIT(7)
#define MR1_QOFF_ENABLE		BIT(12) /* If set, output buffers disabled */

#define RTTNOM_MASK		(BIT(9) | BIT(6) | BIT(2))

/* ZQ calibration types */
enum {
	ZQ_INIT,	/* DDR3: ZQCL with tZQinit, LPDDR3: ZQ Init  with tZQinit  */
	ZQ_LONG,	/* DDR3: ZQCL with tZQoper, LPDDR3: ZQ Long  with tZQCL    */
	ZQ_SHORT,	/* DDR3: ZQCS with tZQCS,   LPDDR3: ZQ Short with tZQCS    */
	ZQ_RESET,	/* DDR3: not used,          LPDDR3: ZQ Reset with tZQreset */
};

/* REUT initialisation modes */
enum {
	REUT_MODE_IDLE = 0,
	REUT_MODE_TEST = 1,
	REUT_MODE_MRS  = 2,
	REUT_MODE_NOP  = 3, /* Normal operation mode */
};

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
	RAMINIT_STATUS_REUT_ERROR,
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
	bool restore_mrs;

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

	union tc_bank_reg tc_bank[NUM_CHANNELS];
	union tc_bank_rank_a_reg tc_bankrank_a[NUM_CHANNELS];
	union tc_bank_rank_b_reg tc_bankrank_b[NUM_CHANNELS];
	union tc_bank_rank_c_reg tc_bankrank_c[NUM_CHANNELS];
	union tc_bank_rank_d_reg tc_bankrank_d[NUM_CHANNELS];

	uint16_t mr0[NUM_CHANNELS][NUM_SLOTS];
	uint16_t mr1[NUM_CHANNELS][NUM_SLOTS];
	uint16_t mr2[NUM_CHANNELS][NUM_SLOTS];
	uint16_t mr3[NUM_CHANNELS][NUM_SLOTS];
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

/* Number of ticks to wait in units of 69.841279 ns (citation needed) */
static inline void tick_delay(const uint32_t delay)
{
	/* Just perform reads to a random register */
	for (uint32_t start = 0; start <= delay; start++)
		mchbar_read32(REUT_ERR_DATA_STATUS);
}

/*
 * 64-bit MCHBAR registers need to be accessed atomically. If one uses
 * two 32-bit ops instead, there will be problems with the REUT's CADB
 * (Command Address Data Buffer): hardware automatically advances the
 * pointer into the register file after a write to the input register.
 */
static inline uint64_t mchbar_read64(const uintptr_t x)
{
	const uint64_t *offset = (uint64_t *)(CONFIG_FIXED_MCHBAR_MMIO_BASE + x);
	uint64_t mmxsave, v;
	asm volatile (
		"\n\t movq %%mm0, %0"
		"\n\t movq %2, %%mm0"
		"\n\t movq %%mm0, %1"
		"\n\t movq %3, %%mm0"
		"\n\t emms"
		: "=m"(mmxsave),
		  "=m"(v)
		: "m"(offset[0]),
		  "m"(mmxsave));
	return v;
}

static inline void mchbar_write64(const uintptr_t x, const uint64_t v)
{
	const uint64_t *offset = (uint64_t *)(CONFIG_FIXED_MCHBAR_MMIO_BASE + x);
	uint64_t mmxsave;
	asm volatile (
		"\n\t movq %%mm0, %0"
		"\n\t movq %2, %%mm0"
		"\n\t movq %%mm0, %1"
		"\n\t movq %3, %%mm0"
		"\n\t emms"
		: "=m"(mmxsave)
		: "m"(offset[0]),
		  "m"(v),
		  "m"(mmxsave));
}

void raminit_main(enum raminit_boot_mode bootmode);

enum raminit_status collect_spd_info(struct sysinfo *ctrl);
enum raminit_status initialise_mpll(struct sysinfo *ctrl);
enum raminit_status convert_timings(struct sysinfo *ctrl);
enum raminit_status configure_mc(struct sysinfo *ctrl);
enum raminit_status configure_memory_map(struct sysinfo *ctrl);
enum raminit_status do_jedec_init(struct sysinfo *ctrl);

void configure_timings(struct sysinfo *ctrl);
void configure_refresh(struct sysinfo *ctrl);

uint32_t get_tCKE(uint32_t mem_clock_mhz, bool lpddr);
uint32_t get_tXPDLL(uint32_t mem_clock_mhz);
uint32_t get_tAONPD(uint32_t mem_clock_mhz);
uint32_t get_tMOD(uint32_t mem_clock_mhz);
uint32_t get_tXS_offset(uint32_t mem_clock_mhz);
uint32_t get_tZQOPER(uint32_t mem_clock_mhz, bool lpddr);
uint32_t get_tZQCS(uint32_t mem_clock_mhz, bool lpddr);

enum raminit_status io_reset(void);
enum raminit_status wait_for_first_rcomp(void);

uint16_t encode_ddr3_rttnom(uint32_t rttnom);
void ddr3_program_mr1(struct sysinfo *ctrl, uint8_t wl_mode, uint8_t q_off);
enum raminit_status ddr3_jedec_init(struct sysinfo *ctrl);

void reut_issue_mrs(
	struct sysinfo *ctrl,
	uint8_t channel,
	uint8_t rankmask,
	uint8_t mr,
	uint16_t val);

void reut_issue_mrs_all(
	struct sysinfo *ctrl,
	uint8_t channel,
	uint8_t mr,
	const uint16_t val[NUM_SLOTS]);

enum raminit_status reut_issue_zq(struct sysinfo *ctrl, uint8_t chanmask, uint8_t zq_type);

uint8_t get_rx_bias(const struct sysinfo *ctrl);

uint8_t get_tCWL(uint32_t mem_clock_mhz);
uint32_t get_tREFI(uint32_t mem_clock_mhz);
uint32_t get_tXP(uint32_t mem_clock_mhz);

#endif
