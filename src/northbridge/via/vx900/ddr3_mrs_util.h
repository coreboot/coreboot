#ifndef REDUNDANT_H
#define REDUNDANT_H

#include <stdint.h>

typedef u32 mrs_cmd_t;

enum ddr3_mr0_precharge {
	DDR3_MR0_PRECHARGE_SLOW		= 0,
	DDR3_MR0_PRECHARGE_FAST		= 1,
};
enum ddr3_mr0_mode {
	DDR3_MR0_MODE_NORMAL		= 0,
	DDR3_MR0_MODE_TEST		= 1,
};
enum ddr3_mr0_dll_reset {
	DDR3_MR0_DLL_RESET_NO		= 0,
	DDR3_MR0_DLL_RESET_YES		= 1,
};
enum ddr3_mr0_burst_type {
	DDR3_MR0_BURST_TYPE_SEQUENTIAL	= 0,
	DDR3_MR0_BURST_TYPE_INTERLEAVED	= 1,
};
enum ddr3_mr0_burst_length {
	DDR3_MR0_BURST_LENGTH_8		= 0,
	DDR3_MR0_BURST_LENGTH_CHOP	= 1,
	DDR3_MR0_BURST_LENGTH_4		= 2,
};
/**
 * \brief Get command address for a DDR3 MR0 command
 */
mrs_cmd_t ddr3_get_mr0(enum ddr3_mr0_precharge precharge_pd,
		       u8 write_recovery,
		       enum ddr3_mr0_dll_reset dll_reset,
		       enum ddr3_mr0_mode mode,
		       u8 cas,
		       enum ddr3_mr0_burst_type interleaved_burst,
		       enum ddr3_mr0_burst_length burst_lenght
);

enum ddr3_mr1_qoff {
	DDR3_MR1_QOFF_ENABLE		= 0,
	DDR3_MR1_QOFF_DISABLE		= 1,
};
enum ddr3_mr1_tqds {
	DDR3_MR1_TQDS_DISABLE		= 0,
	DDR3_MR1_TQDS_ENABLE		= 1,
};
enum ddr3_mr1_write_leveling {
	DDR3_MR1_WRLVL_DISABLE		= 0,
	DDR3_MR1_WRLVL_ENABLE		= 1,
};
enum ddr3_mr1_rtt_nom {
	DDR3_MR1_RTT_NOM_OFF		= 0,
	DDR3_MR1_RTT_NOM_RZQ4		= 1,
	DDR3_MR1_RTT_NOM_RZQ2		= 2,
	DDR3_MR1_RTT_NOM_RZQ6		= 3,
	DDR3_MR1_RTT_NOM_RZQ12		= 4,
	DDR3_MR1_RTT_NOM_RZQ8		= 5,
};
enum ddr3_mr1_additive_latency {
	DDR3_MR1_AL_DISABLE		= 0,
	DDR3_MR1_AL_CL_MINUS_1		= 1,
	DDR3_MR1_AL_CL_MINUS_2		= 2,
};
enum ddr3_mr1_ods {
	DDR3_MR1_ODS_RZQ6		= 0,
	DDR3_MR1_ODS_RZQ7		= 1,
};
enum ddr3_mr1_dll {
	DDR3_MR1_DLL_ENABLE		= 0,
	DDR3_MR1_DLL_DISABLE		= 1,
};
/**
 * \brief Get command address for a DDR3 MR1 command
 */
mrs_cmd_t ddr3_get_mr1(enum ddr3_mr1_qoff qoff,
		       enum ddr3_mr1_tqds tqds,
		       enum ddr3_mr1_rtt_nom rtt_nom,
		       enum ddr3_mr1_write_leveling write_leveling,
		       enum ddr3_mr1_ods output_drive_strenght,
		       enum ddr3_mr1_additive_latency additive_latency,
		       enum ddr3_mr1_dll dll_disable
);

enum ddr3_mr2_rttwr {
	DDR3_MR2_RTTWR_OFF		= 0,
	DDR3_MR2_RTTWR_RZQ4		= 1,
	DDR3_MR2_RTTWR_RZQ2		= 2,
};
enum ddr3_mr2_srt_range {
	DDR3_MR2_SRT_NORMAL		= 0,
	DDR3_MR2_SRT_EXTENDED		= 1,
};
enum ddr3_mr2_asr {
	DDR3_MR2_ASR_MANUAL		= 0,
	DDR3_MR2_ASR_AUTO		= 1,
};
/**
 * \brief Get command address for a DDR3 MR2 command
 */
mrs_cmd_t ddr3_get_mr2(enum ddr3_mr2_rttwr rtt_wr,
		       enum ddr3_mr2_srt_range extended_temp,
		       enum ddr3_mr2_asr self_refresh,
		       u8 cas_cwl
);

/**
 * \brief Get command address for a DDR3 MR3 command
 */
mrs_cmd_t ddr3_get_mr3(char dataflow_from_mpr);

#endif /* REDUNDANT_H */