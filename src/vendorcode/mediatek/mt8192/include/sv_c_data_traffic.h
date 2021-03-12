/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __SV_C_DATA_TRAFFIC_H
#define __SV_C_DATA_TRAFFIC_H

//#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define print_svarg(arg) \
({ \
	msg(TOSTRING(arg) "=0x%x\n", psra->arg); \
})

/*
 * channel type from sv's view
 */
enum {
	SV_CHN_A = 0,
	SV_CHN_B
};


/*
 * dram type from sv's view
 */
enum {
	SV_LPDDR = 0,
	SV_LPDDR2,
	SV_PCDDR3,
	SV_LPDDR3,
	SV_LPDDR4,
	SV_LPDDR5
};

/*
 * data rate from sv's view
 */
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

/*
 * cal_sv_rand_args is data traffic from sv to c.
 * sv randomizes these arguments for c to control
 * calibration.
 */
typedef struct cal_sv_rand_args {

/* >>>>>>>>>> common part begin>>>>>>>>>> */
	/*
	 * 0x4C503435
	 * "LP45"
	 */
	int magic;

	/*
	 * 0: channel-a
	 * 1: channel-b
	 */
	int calibration_channel;

	/*
	 * 0: rank0
	 * 1: rank1
	 */
	int calibration_rank;

	/*
	 * 0: LPDDR
	 * 1: LPDDR2
	 * 2: PCDDR3
	 * 3: LPDDR3
	 * 4: LPDDR4
	 * 5: LPDDR5
	 */
	int dram_type;

	/*
	 * 0: DDR4266
	 * 1: DDR3200
	 * 2: DDR1600
	 * 3: DDR3733
	 * 4: DDR2400
	 * 5: DDR1866
	 * 6: DDR1200
	 * 7: DDR1333
	 * 8: DDR800
	 * 9: DDR1066
	 * 10: DDR2667
	 * 11: DDR4800
	 * 12: DDR5500
	 * 13: DDR6000
	 * 14: DDR6400
	 * 15: DDR2750
	 * 16: DDR2133
	 */
	int datarate;

	/*
	 * Data Mask Disable
	 * 0: enable
	 * 1: disable
	 */
	int dmd;
	int mr2_value; /* for lp4-wirteleveling*/
	int mr3_value;
	int mr13_value;
	int mr12_value;
	int mr16_value;
	int mr18_value; /* lp5 writeleveling */
	int mr20_value; /* lp5 rddqc */
/* ============================= */


/* >>>>>>>>>> cbt part begin>>>>>>>>>> */
	/*
	 * 0: doesn't run cbt calibration
	 * 1: run cbt calibration
	 */
	int cbt;

	/*
	 * 0: rising phase
	 * 1: falling phase
	 */
	int cbt_phase;

	/*
	 * 0: training mode1
	 * 1: training mode2
	 */
	int cbt_training_mode;

	/*
	 * 0: normal mode
	 * 1: byte mode
	 */
	int rk0_cbt_mode;

	/*
	 * 0: normal mode
	 * 1: byte mode
	 */
	int rk1_cbt_mode;

	/*
	 * 0: cbt does NOT use autok
	 * 1: cbt use autok
	 */
	int cbt_autok;

	/*
	 * autok respi
	 * 0/1/2/3
	 */
	int cbt_atk_respi;

	/*
	 * 0: cbt does NOT use new cbt mode
	 * 1: cbt use new cbt mode
	 */
	int new_cbt_mode;

	/*
	 * cbt pat0~7v
	 */
	int pat_v[8];

	/*
	 * cbt pat0~7a
	 */
	int pat_a[8];

	/*
	 * cbt pat_dmv
	 */
	int pat_dmv;

	/*
	 * cbt pat_dma
	 */
	int pat_dma;

	/*
	 * cbt pat_cs
	 */
	int pat_cs;

	/*
	 * new cbt cagolden sel
	 */
	int cagolden_sel;

	/*
	 * new cbt invert num
	 */
	int invert_num;

/* ============================= */

/* >>>>>>>>>> wl part begin>>>>>>>>>> */
	/*
	 * 0: doesn't run wl calibration
	 * 1: run wl calibration
	 */
	int wl;

	/*
	 * 0: wl does NOT use autok
	 * 1: wl use autok
	 */
	int wl_autok;

	/*
	 * autok respi
	 * 0/1/2/3
	 */
	int wl_atk_respi;
/* ============================= */

/* >>>>>>>>>> Gating part begin >>>>>> */
	/*
	 * 0: does not run gating calibration
	 * 1: run gating calibration
	 */
	int gating;

	/*
	 * 0: SW mode calibration
	 * 1: HW AUTO calibration
	 */
	int gating_autok;

	int dqsien_autok_pi_offset;
	int dqsien_autok_early_break_en;
	int dqsien_autok_dbg_mode_en;
/* ============================= */

/* >>>>>>>>>> RDDQC part begin >>>>>> */
	/*
	 * 0: does not run rddq calibration
	 * 1: run rddq calibration
	 */
	int rddqc;

	int low_byte_invert_golden;
	int upper_byte_invert_golden;
	int mr_dq_a_golden;
	int mr_dq_b_golden;
	int lp5_mr20_6_golden;
	int lp5_mr20_7_golden;
/* ============================= */

/* >>>>>>>>>> TX perbit part begin >>>>>> */
	/*
	 * 0: does not run txperbit calibration
	 * 1: run txperbit  calibration
	 */
	int tx_perbit;

	/*
	 * 0: does not run txperbit auto calibration
	 * 1: run txperbit auto calibration
	 */
	int tx_auto_cal;

	int tx_atk_pass_pi_thrd;
	int tx_atk_early_break;
/* ============================= */

/* >>>>>>>>>> TX perbit part begin >>>>>> */
	/*
	 * 0: does not run rxperbit calibration
	 * 1: run rxperbit calibration
	 */
	int rx_perbit;

	/*
	 * 0: does not run rxperbit auto calibration
	 * 1: run rxperbit  auto calibration
	 */
	int rx_auto_cal;

	int rx_atk_cal_step;
	int rx_atk_cal_out_dbg_en;
	int rx_atk_cal_out_dbg_sel;
/* ============================= */
} cal_sv_rand_args_t;

void set_psra(cal_sv_rand_args_t *psra);
cal_sv_rand_args_t *get_psra(void);
void print_sv_args(cal_sv_rand_args_t *psra);
u8 valid_magic(cal_sv_rand_args_t *psra);
void set_type_freq_by_svargs(DRAMC_CTX_T *p,
	    cal_sv_rand_args_t *psra);

#endif /* __SV_C_DATA_TRAFFIC_H */
