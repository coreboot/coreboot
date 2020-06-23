/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8192_DRAMC_PI_API_H__
#define __SOC_MEDIATEK_MT8192_DRAMC_PI_API_H__

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <stdint.h>
#include <types.h>

#include <soc/addressmap.h>
#include <soc/dramc_common_mt8192.h>
#include <soc/dramc_register_bits_def.h>
#include <soc/emi.h>

#define dramc_err(_x_...) printk(BIOS_ERR, _x_)
#define dramc_info(_x_...) printk(BIOS_INFO, _x_)
#if CONFIG(DEBUG_DRAM)
#define dramc_dbg(_x_...) printk(BIOS_INFO, _x_)
#else
#define dramc_dbg(_x_...)
#endif

#define DRAMC_BROADCAST_ON	0x7f
#define DRAMC_BROADCAST_OFF	0x0

#define TX_DQ_UI_TO_PI_TAP		64
#define TX_PHASE_DQ_UI_TO_PI_TAP	32
#define LP4_TX_VREF_DATA_NUM		50
#define LP4_TX_VREF_PASS_CONDITION	0
#define TX_PASS_WIN_CRITERIA		7
#define LP4_TX_VREF_BOUNDARY_NOT_READY 0xff
#define REG_SHU_OFFSET_WIDTH		0x700
#define SHU_OFFSET			(REG_SHU_OFFSET_WIDTH / 4)

typedef enum {
	DDRFREQ_400,
	DDRFREQ_600,
	DDRFREQ_800,
	DDRFREQ_933,
	DDRFREQ_1200,
	DDRFREQ_1600,
	DDRFREQ_2133,
	DDRFREQ_MAX,
} dram_freq_grp;

typedef enum {
	CALI_SEQ0 = 0,
	CALI_SEQ1,
	CALI_SEQ2,
	CALI_SEQ3,
	CALI_SEQ4,
	CALI_SEQ5,
	CALI_SEQ6,
	CALI_SEQ_MAX
} dram_cali_seq;

typedef enum {
	DIV8_MODE = 0,
	DIV4_MODE,
	UNKNOWN_MODE,
} dram_div_mode;

typedef enum {
	VREF_CALI_OFF = 0,
	VREF_CALI_ON,
} vref_cali_mode;

typedef enum {
	DRVP = 0,
	DRVN,
	ODTP,
	ODTN,
	IMP_DRV_MAX
} imp_drv_type;

typedef enum {
	RX_WIN_RD_DQC = 0,
	RX_WIN_TEST_ENG,
} rx_cali_type;

typedef enum TX_CAL_TYPE {
	TX_DQ_DQS_MOVE_DQ_ONLY,
	TX_DQ_DQS_MOVE_DQM_ONLY,
	TX_DQ_DQS_MOVE_DQ_DQM,
} tx_cali_type;

typedef enum {
	DCM_OFF = 0,
	DCM_ON,
} dcm_state;

typedef enum {
	CBT_LOW_FREQ = 0,
	CBT_HIGH_FREQ,
	CBT_UNKNOWN_FREQ = 0xff,
} cbt_freq;

typedef enum {
	IN_CBT,
	OUT_CBT,
} cbt_state;

enum {
	PHYPLL_MODE = 0,
	CLRPLL_MODE,
};

enum {
	DUTYSCAN_K_DQ,
	DUTYSCAN_K_DQM,
};

typedef enum {
	O1_OFF,
	O1_ON,
} o1_state;

typedef enum {
	SINGLE_RANK_DDR = 1,
	DUAL_RANK_DDR
} ddr_rank_num;

enum {
	DQS_8PH_DEGREE_0 = 0,
	DQS_8PH_DEGREE_180,
	DQS_8PH_DEGREE_45,

	DQS_8PH_DEGREE_MAX,
};

struct dram_impedance {
	u32 result[ODT_MAX][IMP_DRV_MAX];
};

struct mr_values {
	u8 mr01[FSP_MAX];
	u8 mr02[FSP_MAX];
	u8 mr03[FSP_MAX];
	u8 mr04[RANK_MAX];
	u8 mr11[FSP_MAX];
	u8 mr12[CHANNEL_MAX][RANK_MAX][FSP_MAX];
	u8 mr13[RANK_MAX];
	u8 mr14[CHANNEL_MAX][RANK_MAX][FSP_MAX];
	u8 mr18[CHANNEL_MAX][RANK_MAX];
	u8 mr19[CHANNEL_MAX][RANK_MAX];
	u8 mr20[FSP_MAX];
	u8 mr21[FSP_MAX];
	u8 mr22[FSP_MAX];
	u8 mr23[CHANNEL_MAX][RANK_MAX];
	u8 mr26[RANK_MAX];
	u8 mr30[RANK_MAX];
	u8 mr51[FSP_MAX];
};

struct ddr_cali {
	u8 chn;
	u8 rank;
	/*
	 * frequency set point:
	 *	0 means lower,un-terminated freq;
	 *	1 means higher,terminated freq
	 */
	u8 fsp;
	u8 density;
	u8 *pll_mode;
	u32 frequency;
	u32 vcore_voltage;
	dram_dfs_shu shu;
	ddr_rank_num support_ranks;
	dbi_mode w_dbi[FSP_MAX];
	vref_cali_mode vref_cali;
	dram_odt_state odt_onoff;
	dram_freq_grp freq_group;
	dram_div_mode div_mode;
	dram_pinmux_type pinmux_type;
	dram_cbt_mode cbt_mode[RANK_MAX];
	struct dram_impedance impedance;
	struct mr_values *mr_value;
	const struct emi_mdl *emi_config;
	const struct sdram_params *params;
};

struct reg_bak {
	u32 *addr;
	u32 value;
};

typedef struct _ana_top_config {
	u8 dll_async_en;
	u8 all_slave_en;
	u8 rank_mode;
	u8 dll_idle_mode;
	u8 aphy_comb_en;
	u8 tx_odt_dis;
	u8 new_8x_mode;
} ana_top_config;

typedef struct ana_dvfs_core_config {
	u8 ckr;
	u8 dq_p2s_ratio;
	u8 ca_p2s_ratio;
	u8 dq_ca_open;
	u8 dq_semi_open;
	u8 ca_semi_open;
	u8 ca_full_rate;
	u8 dq_ckdiv4_en;
	u8 ca_ckdiv4_en;
	u8 ca_prediv_en;
	u8 ph8_dly;
	u8 semi_open_ca_pick_mck_ratio;
	u8 dq_aamck_div;
	u8 ca_admck_div;
	u8 dq_track_ca_en;
	u32 pll_freq;
} ana_dvfs_core;

typedef struct lp4_dram_config {
	u8 ex_row_en[RANK_MAX];
	u8 mr_wl;
	u8 dbi_wr;
	u8 dbi_rd;
	u8 lp4y_en;
	u8 work_fsp;
} dram_config;

typedef struct _dvfs_group_config {
	u32 data_rate;
	u8 dqsien_mode;
	u8 dq_p2s_ratio;
	u8 ckr;
} dvfs_group_config;

struct gating_config {
	u8 gat_track_en;
	u8 rx_gating_mode;
	u8 rx_gating_track_mode;
	u8 valid_lat_value;
};

typedef struct _dramc_subsys_config {
	dram_freq_grp freq_group;
	ana_top_config *a_cfg;
	ana_dvfs_core *dvfs_core;
	dram_config *lp4_init;
	dvfs_group_config *dfs_gp;
	struct gating_config *gat_c;
} dramc_subsys_config;

typedef struct _reg_transfer {
	u32 *addr;
	u8 offset;
} reg_transfer;

void emi_init2(void);
u32 get_column_num(void);
u32 get_row_width_from_emi(u32 rank);
u8 dramc_mode_reg_read(u8 chn, u8 mr_idx);
u8 dramc_mode_reg_read_by_rank(u8 chn, u8 rank, u8 mr_idx);
void dramc_mode_reg_write_by_rank(const struct ddr_cali *cali,
	u8 chn, u8 rank, u8 mr_idx, u8 value);
void after_calib(const struct ddr_cali *cali);
void init_dram(const struct dramc_data *dparam);
void global_option_init(struct ddr_cali *cali);
u32 dramc_get_broadcast(void);
void dramc_set_broadcast(u32 onoff);
void dramc_sw_impedance_cal(dram_odt_state odt, struct dram_impedance *imp);
void dramc_sw_impedance_save_register(const struct ddr_cali *cali);
void dfs_init_for_calibration(const struct ddr_cali *cali);
void dramc_auto_refresh_switch(u8 chn, bool flag);
void dramc_runtime_config(const struct ddr_cali *cali);
void emi_mdl_init(const struct emi_mdl *emi_con);
void cke_fix_onoff(const struct ddr_cali *cali, u8 chn, u8 rank, int option);
void enable_phy_dcm_shuffle(dcm_state enable, u8 shuffle_save);
void enable_phy_dcm_non_shuffle(dcm_state enable);
void dramc_8_phase_cal(const struct ddr_cali *cali);
void dramc_duty_calibration(const struct sdram_params *params);
void dramc_write_leveling(const struct ddr_cali *cali,
			  u8 dqs_final_delay[RANK_MAX][DQS_NUMBER]);
void dramc_rx_dqs_gating_cal(const struct ddr_cali *cali, u8 *txdly_min, u8 *txdly_max);
void dramc_rx_dqs_gating_post_process(const struct ddr_cali *cali,
	u8 txdly_min, u8 txdly_max);
void dramc_rx_datlat_cal(const struct ddr_cali *cali);
void dramc_dual_rank_rx_datlat_cal(const struct ddr_cali *cali);
void dramc_cmd_bus_training(const struct ddr_cali *cali);
void dramc_rx_window_perbit_cal(const struct ddr_cali *cali, rx_cali_type type);
void dramc_tx_window_perbit_cal(const struct ddr_cali *cali, tx_cali_type cal_type,
	const u8 dqs_final_delay[RANK_MAX][DQS_NUMBER], bool vref_scan_enable);
void dramc_tx_oe_calibration(const struct ddr_cali *cali);
dram_freq_grp get_freq_group(const struct ddr_cali *cali);
dram_odt_state get_odt_state(const struct ddr_cali *cali);
u8 get_fsp(const struct ddr_cali *cali);
dram_dfs_shu get_shu(const struct ddr_cali *cali);
dram_freq_grp get_highest_freq_group(void);
dram_cbt_mode get_cbt_mode(const struct ddr_cali *cali);
u32 get_frequency(const struct ddr_cali *cali);
vref_cali_mode get_vref_cali(const struct ddr_cali *cali);
dram_div_mode get_div_mode(const struct ddr_cali *cali);
dbi_mode get_write_dbi(const struct ddr_cali *cali);
dram_dfs_shu get_shu_save_by_k_shu(dram_cali_seq k_seq);
dram_freq_grp get_freq_group_by_shu_save(dram_dfs_shu shu);
dram_pinmux_type get_pinmux_type(const struct ddr_cali *cali);
u32 get_frequency_by_shu(dram_dfs_shu shu);
u32 get_vcore_value(const struct ddr_cali *cali);
void set_cali_datas(struct ddr_cali *cali,
	const struct dramc_data *dparam, dram_cali_seq k_seq);
u8 get_mck2ui_div_shift(const struct ddr_cali *cali);
void tx_picg_setting(const struct ddr_cali *cali);
void xrtrtr_shu_setting(const struct ddr_cali *cali);
void cbt_switch_freq(const struct ddr_cali *cali, cbt_freq freq);
void enable_dfs_hw_mode_clk(void);
void dramc_dfs_direct_jump_rg_mode(const struct ddr_cali *cali, u8 shu_level);
void dramc_dfs_direct_jump_sram_shu_rg_mode(const struct ddr_cali *cali,
	dram_dfs_shu shu_level);
void dramc_save_result_to_shuffle(dram_dfs_shu src, dram_dfs_shu dst);
void dramc_load_shuffle_to_dramc(dram_dfs_shu src, dram_dfs_shu dst);
void dvfs_settings(const struct ddr_cali *cali);
void dramc_dqs_precalculation_preset(const struct ddr_cali *cali);
void freq_jump_ratio_calculation(const struct ddr_cali *cali);
void dramc_hmr4_presetting(const struct ddr_cali *cali);
void dramc_enable_perbank_refresh(bool en);
void dramc_modified_refresh_mode(void);
void dramc_cke_debounce(const struct ddr_cali *cali);
void dramc_hw_dqsosc(const struct ddr_cali *cali, u8 chn);
void xrtwtw_shu_setting(const struct ddr_cali *cali);
void enable_write_DBI_after_calibration(const struct ddr_cali *cali);
void dramc_set_mr13_vrcg_to_normal(const struct ddr_cali *cali);
void ana_init(const struct ddr_cali *cali, dramc_subsys_config *subsys);
void dig_static_setting(const struct ddr_cali *cali, dramc_subsys_config *subsys);
void dig_config_shuf(const struct ddr_cali *cali, dramc_subsys_config *subsys);
void resetb_pull_dn(void);
void dramc_subsys_pre_config(dram_freq_grp freq_group, dramc_subsys_config *subsys);
void single_end_dramc_post_config(u8 lp4y_en);
void dram_configure(dram_freq_grp freq_group, dram_config *tr);
void ana_clk_div_config(ana_dvfs_core *tr, dvfs_group_config *dfs);
void apply_write_dbi_power_improve(bool onoff);
void dramc_write_dbi_onoff(u8 onoff);
void cbt_delay_ca_clk(u8 chn, u8 rank, s32 iDelay);
void dramc_cmd_ui_delay_setting(u8 chn, u8 value);
void dramc_dqsosc_set_mr18_mr19(const struct ddr_cali *cali,
	u16 *osc_thrd_inc, u16 *osc_thrd_dec);
void dqsosc_shu_settings(const struct ddr_cali *cali,
	u16 *osc_thrd_inc, u16 *osc_thrd_dec);
void shift_dq_ui(const struct ddr_cali *cali, u8 rk, s8 shift_ui);
void shuffle_dfs_to_fsp1(const struct ddr_cali *cali);
u8 get_cbt_vref_pinmux_value(const struct ddr_cali *cali, u8 range, u8 vref_lev);
void o1_path_on_off(const struct ddr_cali *cali, o1_state o1);

#endif /* __SOC_MEDIATEK_MT8192_DRAMC_PI_API_H__ */
