/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __SV_C_DATA_TRAFFIC_H
#define __SV_C_DATA_TRAFFIC_H

#if FOR_DV_SIMULATION_USED
#ifndef STRINGIFY(x)
#define STRINGIFY(x) #x
#endif
#endif
#define TOSTRING(x) STRINGIFY(x)

#define print_svarg(arg) \
({ \
	mcSHOW_DBG_MSG((TOSTRING(arg) "=0x%x\n", psra->arg)); \
})


enum {
	SV_CHN_A = 0,
	SV_CHN_B
};



enum {
	SV_LPDDR = 0,
	SV_LPDDR2,
	SV_PCDDR3,
	SV_LPDDR3,
	SV_LPDDR4,
	SV_LPDDR5
};


enum {
	SV_DDR4266 = 0,
	SV_DDR3200,
	SV_DDR1600,
	SV_DDR3733,
	SV_DDR2400,
	SV_DDR1866,
	SV_DDR1200,
	SV_DDR1333,
	SV_DDR800,
	SV_DDR1066,
	SV_DDR2667,
	SV_DDR4800,
	SV_DDR5500,
	SV_DDR6000,
	SV_DDR6400,
	SV_DDR2750,
	SV_DDR2133
};


typedef struct cal_sv_rand_args {


	int magic;


	int calibration_channel;


	int calibration_rank;


	int dram_type;


	int datarate;


	int dmd;
	int mr2_value;
	int mr3_value;
	int mr13_value;
	int mr12_value;
	int mr16_value;
	int mr18_value;
	int mr20_value;

	int cbt;


	int cbt_phase;


	int cbt_training_mode;


	int rk0_cbt_mode;


	int rk1_cbt_mode;


	int cbt_autok;


	int cbt_atk_respi;


	int new_cbt_mode;


	int pat_v[8];


	int pat_a[8];


	int pat_dmv;


	int pat_dma;


	int pat_cs;


	int cagolden_sel;


	int invert_num;


	int wl;


	int wl_autok;


	int wl_atk_respi;

	int gating;


	int gating_autok;

	int dqsien_autok_pi_offset;
	int dqsien_autok_early_break_en;
	int dqsien_autok_dbg_mode_en;

	int rddqc;

	int low_byte_invert_golden;
	int upper_byte_invert_golden;
	int mr_dq_a_golden;
	int mr_dq_b_golden;
	int lp5_mr20_6_golden;
	int lp5_mr20_7_golden;

	int tx_perbit;


	int tx_auto_cal;

	int tx_atk_pass_pi_thrd;
	int tx_atk_early_break;

	int rx_perbit;


	int rx_auto_cal;

	int rx_atk_cal_step;
	int rx_atk_cal_out_dbg_en;
	int rx_atk_cal_out_dbg_sel;

} cal_sv_rand_args_t;

void set_psra(cal_sv_rand_args_t *psra);
cal_sv_rand_args_t *get_psra(void);
void print_sv_args(cal_sv_rand_args_t *psra);
u8 valid_magic(cal_sv_rand_args_t *psra);
void set_type_freq_by_svargs(DRAMC_CTX_T *p,
	    cal_sv_rand_args_t *psra);

#endif /* __SV_C_DATA_TRAFFIC_H */
