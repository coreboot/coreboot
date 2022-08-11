/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __MT8188_SOC_PMIF_H__
#define __MT8188_SOC_PMIF_H__

#include <device/mmio.h>
#include <soc/pmif_common.h>
#include <types.h>

/* indicate which number SW channel start, by project */
#define PMIF_SPMI_SW_CHAN	BIT(6)
#define PMIF_SPMI_INF		0x5E7

struct mtk_pmif_regs {
	u32 init_done;
	u32 reserved1[5];
	u32 inf_busy_sta;
	u32 other_busy_sta_0;
	u32 other_busy_sta_1;
	u32 inf_en;
	u32 other_inf_en;
	u32 inf_cmd_per_0;
	u32 inf_cmd_per_1;
	u32 inf_cmd_per_2;
	u32 inf_cmd_per_3;
	u32 inf_max_bytecnt_per_0;
	u32 inf_max_bytecnt_per_1;
	u32 inf_max_bytecnt_per_2;
	u32 inf_max_bytecnt_per_3;
	u32 staupd_ctrl;
	u32 reserved2[48];
	u32 int_gps_auxadc_cmd_addr;
	u32 int_gps_auxadc_cmd;
	u32 int_gps_auxadc_rdata_addr;
	u32 reserved3[13];
	u32 arb_en;
	u32 reserved4[34];
	u32 lat_cnter_ctrl;
	u32 lat_cnter_en;
	u32 lat_limit_loading;
	u32 lat_limit_0;
	u32 lat_limit_1;
	u32 lat_limit_2;
	u32 lat_limit_3;
	u32 lat_limit_4;
	u32 lat_limit_5;
	u32 lat_limit_6;
	u32 lat_limit_7;
	u32 lat_limit_8;
	u32 lat_limit_9;
	u32 reserved5[99];
	u32 crc_ctrl;
	u32 crc_sta;
	u32 sig_mode;
	u32 pmic_sig_addr;
	u32 pmic_sig_val;
	u32 reserved6[2];
	u32 cmdissue_en;
	u32 reserved7[10];
	u32 timer_ctrl;
	u32 timer_sta;
	u32 sleep_protection_ctrl;
	u32 reserved8[6];
	u32 spi_mode_ctrl;
	u32 reserved9[2];
	u32 pmic_eint_sta_addr;
	u32 reserved10[2];
	u32 irq_event_en_0;
	u32 irq_flag_raw_0;
	u32 irq_flag_0;
	u32 irq_clr_0;
	u32 reserved11[244];
	u32 swinf_0_acc;
	u32 swinf_0_wdata_31_0;
	u32 swinf_0_wdata_63_32;
	u32 reserved12[2];
	u32 swinf_0_rdata_31_0;
	u32 swinf_0_rdata_63_32;
	u32 reserved13[2];
	u32 swinf_0_vld_clr;
	u32 swinf_0_sta;
	u32 reserved14[5];
	u32 swinf_1_acc;
	u32 swinf_1_wdata_31_0;
	u32 swinf_1_wdata_63_32;
	u32 reserved15[2];
	u32 swinf_1_rdata_31_0;
	u32 swinf_1_rdata_63_32;
	u32 reserved16[2];
	u32 swinf_1_vld_clr;
	u32 swinf_1_sta;
	u32 reserved17[5];
	u32 swinf_2_acc;
	u32 swinf_2_wdata_31_0;
	u32 swinf_2_wdata_63_32;
	u32 reserved18[2];
	u32 swinf_2_rdata_31_0;
	u32 swinf_2_rdata_63_32;
	u32 reserved19[2];
	u32 swinf_2_vld_clr;
	u32 swinf_2_sta;
	u32 reserved20[5];
	u32 swinf_3_acc;
	u32 swinf_3_wdata_31_0;
	u32 swinf_3_wdata_63_32;
	u32 reserved21[2];
	u32 swinf_3_rdata_31_0;
	u32 swinf_3_rdata_63_32;
	u32 reserved22[2];
	u32 swinf_3_vld_clr;
	u32 swinf_3_sta;
	u32 reserved23[133];
};

check_member(mtk_pmif_regs, inf_busy_sta, 0x18);
check_member(mtk_pmif_regs, int_gps_auxadc_cmd_addr, 0x110);
check_member(mtk_pmif_regs, arb_en, 0x0150);
check_member(mtk_pmif_regs, lat_cnter_en, 0x1E0);
check_member(mtk_pmif_regs, crc_ctrl, 0x39C);
check_member(mtk_pmif_regs, cmdissue_en, 0x3B8);
check_member(mtk_pmif_regs, timer_ctrl, 0x3E4);
check_member(mtk_pmif_regs, spi_mode_ctrl, 0x408);
check_member(mtk_pmif_regs, pmic_eint_sta_addr, 0x414);
check_member(mtk_pmif_regs, irq_event_en_0, 0x420);
check_member(mtk_pmif_regs, swinf_0_acc, 0x800);

#define PMIF_SPMI_AP_CHAN	(PMIF_SPMI_BASE + 0x880)
#define PMIF_SPI_AP_CHAN	(PMIF_SPI_BASE + 0x880)

struct mtk_clk_monitor_regs {
	u32 clk_monitor_ctrl;
};

#define mtk_clk_monitor		((struct mtk_clk_monitor_regs *)EFUSE_BASE + 0x45C)

enum {
	FREQ_260MHZ = 260,
};

#define FREQ_METER_ABIST_AD_OSC_CK	42

#endif /*__MT8188_SOC_PMIF_H__*/
