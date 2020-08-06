/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __CPU_TI_AM335X_DDR_INIT_H__
#define __CPU_TI_AM335X_DDR_INIT_H__

#include <types.h>

struct ctrl_ioregs {
	uint32_t cm0ioctl;
	uint32_t cm1ioctl;
	uint32_t cm2ioctl;
	uint32_t dt0ioctl;
	uint32_t dt1ioctl;
	uint32_t dt2ioctrl;
	uint32_t dt3ioctrl;
	uint32_t emif_sdram_config_ext;
};

/**
 * Encapsulates DDR DATA registers.
 */
struct ddr_data {
	uint32_t datardsratio0;
	uint32_t datawdsratio0;
	uint32_t datawiratio0;
	uint32_t datagiratio0;
	uint32_t datafwsratio0;
	uint32_t datawrsratio0;
};

/**
 * Encapsulates DDR CMD control registers.
 */
struct cmd_control {
	uint32_t cmd0csratio;
	uint32_t cmd0csforce;
	uint32_t cmd0csdelay;
	uint32_t cmd0iclkout;
	uint32_t cmd1csratio;
	uint32_t cmd1csforce;
	uint32_t cmd1csdelay;
	uint32_t cmd1iclkout;
	uint32_t cmd2csratio;
	uint32_t cmd2csforce;
	uint32_t cmd2csdelay;
	uint32_t cmd2iclkout;
};


/*
 * Structure containing shadow of important registers in EMIF
 * The calculation function fills in this structure to be later used for
 * initialization and DVFS
 */
struct emif_regs {
	uint32_t freq;
	uint32_t sdram_config_init;
	uint32_t sdram_config;
	uint32_t sdram_config2;
	uint32_t ref_ctrl;
	uint32_t ref_ctrl_final;
	uint32_t sdram_tim1;
	uint32_t sdram_tim2;
	uint32_t sdram_tim3;
	uint32_t ocp_config;
	uint32_t read_idle_ctrl;
	uint32_t zq_config;
	uint32_t temp_alert_config;
	uint32_t emif_ddr_phy_ctlr_1_init;
	uint32_t emif_ddr_phy_ctlr_1;
	uint32_t emif_ddr_ext_phy_ctrl_1;
	uint32_t emif_ddr_ext_phy_ctrl_2;
	uint32_t emif_ddr_ext_phy_ctrl_3;
	uint32_t emif_ddr_ext_phy_ctrl_4;
	uint32_t emif_ddr_ext_phy_ctrl_5;
	uint32_t emif_rd_wr_lvl_rmp_win;
	uint32_t emif_rd_wr_lvl_rmp_ctl;
	uint32_t emif_rd_wr_lvl_ctl;
	uint32_t emif_rd_wr_exec_thresh;
	uint32_t emif_prio_class_serv_map;
	uint32_t emif_connect_id_serv_1_map;
	uint32_t emif_connect_id_serv_2_map;
	uint32_t emif_cos_config;
	uint32_t emif_ecc_ctrl_reg;
	uint32_t emif_ecc_address_range_1;
	uint32_t emif_ecc_address_range_2;
};

/* VTP Registers */
struct vtp_reg {
	uint32_t vtp0ctrlreg;
};


/* Reg mapping structure */
struct emif_reg_struct {
	uint32_t emif_mod_id_rev;
	uint32_t emif_status;
	uint32_t emif_sdram_config;
	uint32_t emif_lpddr2_nvm_config;
	uint32_t emif_sdram_ref_ctrl;
	uint32_t emif_sdram_ref_ctrl_shdw;
	uint32_t emif_sdram_tim_1;
	uint32_t emif_sdram_tim_1_shdw;
	uint32_t emif_sdram_tim_2;
	uint32_t emif_sdram_tim_2_shdw;
	uint32_t emif_sdram_tim_3;
	uint32_t emif_sdram_tim_3_shdw;
	uint32_t emif_lpddr2_nvm_tim;
	uint32_t emif_lpddr2_nvm_tim_shdw;
	uint32_t emif_pwr_mgmt_ctrl;
	uint32_t emif_pwr_mgmt_ctrl_shdw;
	uint32_t emif_lpddr2_mode_reg_data;
	uint32_t padding1[1];
	uint32_t emif_lpddr2_mode_reg_data_es2;
	uint32_t padding11[1];
	uint32_t emif_lpddr2_mode_reg_cfg;
	uint32_t emif_l3_config;
	uint32_t emif_l3_cfg_val_1;
	uint32_t emif_l3_cfg_val_2;
	uint32_t emif_iodft_tlgc;
	uint32_t padding2[7];
	uint32_t emif_perf_cnt_1;
	uint32_t emif_perf_cnt_2;
	uint32_t emif_perf_cnt_cfg;
	uint32_t emif_perf_cnt_sel;
	uint32_t emif_perf_cnt_tim;
	uint32_t padding3;
	uint32_t emif_read_idlectrl;
	uint32_t emif_read_idlectrl_shdw;
	uint32_t padding4;
	uint32_t emif_irqstatus_raw_sys;
	uint32_t emif_irqstatus_raw_ll;
	uint32_t emif_irqstatus_sys;
	uint32_t emif_irqstatus_ll;
	uint32_t emif_irqenable_set_sys;
	uint32_t emif_irqenable_set_ll;
	uint32_t emif_irqenable_clr_sys;
	uint32_t emif_irqenable_clr_ll;
	uint32_t padding5;
	uint32_t emif_zq_config;
	uint32_t emif_temp_alert_config;
	uint32_t emif_l3_err_log;
	uint32_t emif_rd_wr_lvl_rmp_win;
	uint32_t emif_rd_wr_lvl_rmp_ctl;
	uint32_t emif_rd_wr_lvl_ctl;
	uint32_t padding6[1];
	uint32_t emif_ddr_phy_ctrl_1;
	uint32_t emif_ddr_phy_ctrl_1_shdw;
	uint32_t emif_ddr_phy_ctrl_2;
	uint32_t padding7[4];
	uint32_t emif_prio_class_serv_map;
	uint32_t emif_connect_id_serv_1_map;
	uint32_t emif_connect_id_serv_2_map;
	uint32_t padding8;
	uint32_t emif_ecc_ctrl_reg;
	uint32_t emif_ecc_address_range_1;
	uint32_t emif_ecc_address_range_2;
	uint32_t padding8_1;
	uint32_t emif_rd_wr_exec_thresh;
	uint32_t emif_cos_config;
	uint32_t padding9[6];
	uint32_t emif_ddr_phy_status[28];
	uint32_t padding10[20];
	uint32_t emif_ddr_ext_phy_ctrl_1;
	uint32_t emif_ddr_ext_phy_ctrl_1_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_2;
	uint32_t emif_ddr_ext_phy_ctrl_2_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_3;
	uint32_t emif_ddr_ext_phy_ctrl_3_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_4;
	uint32_t emif_ddr_ext_phy_ctrl_4_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_5;
	uint32_t emif_ddr_ext_phy_ctrl_5_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_6;
	uint32_t emif_ddr_ext_phy_ctrl_6_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_7;
	uint32_t emif_ddr_ext_phy_ctrl_7_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_8;
	uint32_t emif_ddr_ext_phy_ctrl_8_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_9;
	uint32_t emif_ddr_ext_phy_ctrl_9_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_10;
	uint32_t emif_ddr_ext_phy_ctrl_10_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_11;
	uint32_t emif_ddr_ext_phy_ctrl_11_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_12;
	uint32_t emif_ddr_ext_phy_ctrl_12_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_13;
	uint32_t emif_ddr_ext_phy_ctrl_13_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_14;
	uint32_t emif_ddr_ext_phy_ctrl_14_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_15;
	uint32_t emif_ddr_ext_phy_ctrl_15_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_16;
	uint32_t emif_ddr_ext_phy_ctrl_16_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_17;
	uint32_t emif_ddr_ext_phy_ctrl_17_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_18;
	uint32_t emif_ddr_ext_phy_ctrl_18_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_19;
	uint32_t emif_ddr_ext_phy_ctrl_19_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_20;
	uint32_t emif_ddr_ext_phy_ctrl_20_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_21;
	uint32_t emif_ddr_ext_phy_ctrl_21_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_22;
	uint32_t emif_ddr_ext_phy_ctrl_22_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_23;
	uint32_t emif_ddr_ext_phy_ctrl_23_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_24;
	uint32_t emif_ddr_ext_phy_ctrl_24_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_25;
	uint32_t emif_ddr_ext_phy_ctrl_25_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_26;
	uint32_t emif_ddr_ext_phy_ctrl_26_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_27;
	uint32_t emif_ddr_ext_phy_ctrl_27_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_28;
	uint32_t emif_ddr_ext_phy_ctrl_28_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_29;
	uint32_t emif_ddr_ext_phy_ctrl_29_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_30;
	uint32_t emif_ddr_ext_phy_ctrl_30_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_31;
	uint32_t emif_ddr_ext_phy_ctrl_31_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_32;
	uint32_t emif_ddr_ext_phy_ctrl_32_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_33;
	uint32_t emif_ddr_ext_phy_ctrl_33_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_34;
	uint32_t emif_ddr_ext_phy_ctrl_34_shdw;
	uint32_t emif_ddr_ext_phy_ctrl_35;
	uint32_t emif_ddr_ext_phy_ctrl_35_shdw;
	union {
		uint32_t emif_ddr_ext_phy_ctrl_36;
		uint32_t emif_ddr_fifo_misaligned_clear_1;
	};
	union {
		uint32_t emif_ddr_ext_phy_ctrl_36_shdw;
		uint32_t emif_ddr_fifo_misaligned_clear_2;
	};
};

struct ddr_cmd_regs {
	uint32_t resv0[7];
	uint32_t cm0csratio; /* offset 0x01C */
	uint32_t resv1[3];
	uint32_t cm0iclkout; /* offset 0x02C */
	uint32_t resv2[8];
	uint32_t cm1csratio; /* offset 0x050 */
	uint32_t resv3[3];
	uint32_t cm1iclkout; /* offset 0x060 */
	uint32_t resv4[8];
	uint32_t cm2csratio; /* offset 0x084 */
	uint32_t resv5[3];
	uint32_t cm2iclkout; /* offset 0x094 */
	uint32_t resv6[3];
};

struct ddr_data_regs {
	uint32_t dt0rdsratio0; /* offset 0x0C8 */
	uint32_t resv1[4];
	uint32_t dt0wdsratio0; /* offset 0x0DC */
	uint32_t resv2[4];
	uint32_t dt0wiratio0; /* offset 0x0F0 */
	uint32_t resv3;
	uint32_t dt0wimode0;  /* offset 0x0F8 */
	uint32_t dt0giratio0; /* offset 0x0FC */
	uint32_t resv4;
	uint32_t dt0gimode0;   /* offset 0x104 */
	uint32_t dt0fwsratio0; /* offset 0x108 */
	uint32_t resv5[4];
	uint32_t dt0dqoffset;  /* offset 0x11C */
	uint32_t dt0wrsratio0; /* offset 0x120 */
	uint32_t resv6[4];
	uint32_t dt0rdelays0; /* offset 0x134 */
	uint32_t dt0dldiff0;  /* offset 0x138 */
	uint32_t resv7[12];
};

/* Control Status Register */
struct ctrl_stat {
	uint32_t resv1[16];
	uint32_t statusreg; /* ofset 0x40 */
	uint32_t resv2[51];
	uint32_t secure_emif_sdram_config; /* offset 0x0110 */
	uint32_t resv3[319];
	uint32_t dev_attr;
};

/**
 * This structure represents the DDR io control on AM33XX devices.
 */
struct ddr_cmdtctrl {
	uint32_t cm0ioctl;
	uint32_t cm1ioctl;
	uint32_t cm2ioctl;
	uint32_t resv2[12];
	uint32_t dt0ioctl;
	uint32_t dt1ioctl;
	uint32_t dt2ioctrl;
	uint32_t dt3ioctrl;
	uint32_t resv3[4];
	uint32_t emif_sdram_config_ext;
};

struct ddr_ctrl {
	uint32_t ddrioctrl;
	uint32_t resv1[325];
	uint32_t ddrckectrl;
};

/* AM335X EMIF Register values */
#define VTP_CTRL_READY (0x1 << 5)
#define VTP_CTRL_ENABLE (0x1 << 6)
#define VTP_CTRL_START_EN (0x1)

#define DDR_CKE_CTRL_NORMAL 0x1

#define PHY_EN_DYN_PWRDN (0x1 << 20)

/* VTP Base address */
#define VTP0_CTRL_ADDR 0x44E10E0C
#define VTP1_CTRL_ADDR 0x48140E10

/* EMIF Base address */
#define EMIF4_0_CFG_BASE 0x4C000000
#define EMIF4_1_CFG_BASE 0x4D000000

/* DDR Base address */
#define DDR_PHY_CMD_ADDR 0x44E12000
#define DDR_PHY_DATA_ADDR 0x44E120C8
#define DDR_PHY_CMD_ADDR2 0x47C0C800
#define DDR_PHY_DATA_ADDR2 0x47C0C8C8
#define DDR_DATA_REGS_NR 2

/* DDR Base address */
#define DDR_CTRL_ADDR 0x44E10E04
#define DDR_CONTROL_BASE_ADDR 0x44E11404

/* Control Module Base Address */
#define CTRL_BASE 0x44E10000

#define EMIF_REG_MAJOR_REVISION_SHIFT 8
#define EMIF_REG_MAJOR_REVISION_MASK (0x7 << 8)

#define EMIF_REG_SDRAM_TYPE_SHIFT 29
#define EMIF_REG_SDRAM_TYPE_MASK (0x7 << 29)

#define EMIF_EXT_PHY_CTRL_TIMING_REG 0x5

#define EMIF_REG_INITREF_DIS_MASK (1 << 31)
#define EMIF_4D5 0x5

/* SDRAM TYPE */
#define EMIF_SDRAM_TYPE_DDR2 0x2
#define EMIF_SDRAM_TYPE_DDR3 0x3
#define EMIF_SDRAM_TYPE_LPDDR2 0x4

#define PLL_BYPASS_MODE 0x4
#define ST_MN_BYPASS 0x00000100
#define ST_DPLL_CLK 0x00000001
#define CLK_SEL_MASK 0x7ffff
#define CLK_DIV_MASK 0x1f
#define CLK_DIV2_MASK 0x7f
#define CLK_SEL_SHIFT 0x8
#define CLK_MODE_SEL 0x7
#define CLK_MODE_MASK 0xfffffff8
#define CLK_DIV_SEL 0xFFFFFFE0
#define CPGMAC0_IDLE 0x30000
#define DPLL_CLKDCOLDO_GATE_CTRL 0x300

#define V_OSCK 24000000 /* Clock output from T2 */
#define OSC (V_OSCK / 1000000)

#define DDRPLL_M 266
#define DDRPLL_N (OSC - 1)
#define DDRPLL_M2 1

void config_ddr(uint32_t pll, const struct ctrl_ioregs *ioregs, const struct ddr_data *data,
		const struct cmd_control *ctrl, const struct emif_regs *regs, int nr);

#endif
