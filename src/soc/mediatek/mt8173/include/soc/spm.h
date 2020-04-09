/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8173_SPM_H__
#define __SOC_MEDIATEK_MT8173_SPM_H__

#include <soc/addressmap.h>
#include <stddef.h>

enum {
	SPM_PROJECT_CODE = 0xb16
};

enum {
	DISP_PWR_STA_MASK	= 0x1 << 3,
	DISP_SRAM_PDN_MASK	= 0xf << 8,
	DISP_SRAM_ACK_MASK	= 0x1 << 12,
	AUDIO_PWR_STA_MASK	= 0x1 << 24,
	AUDIO_SRAM_PDN_MASK	= 0xf << 8,
	AUDIO_SRAM_ACK_MASK	= 0xf << 12,
};

struct mtk_spm_regs {
	u32 poweron_config_set;
	u32 reserved1[3];
	u32 power_on_val0;		/* 0x010 */
	u32 power_on_val1;
	u32 reserved2[58];
	u32 clk_settle;			/* 0x100 */
	u32 reserved3[63];
	u32 ca7_cpu0_pwr_con;		/* 0x200 */
	u32 ca7_dbg_pwr_con;
	u32 ca7_cputop_pwr_con;
	u32 reserved4;
	u32 vde_pwr_con;		/* 0x210 */
	u32 mfg_pwr_con;
	u32 ca7_cpu1_pwr_con;		/* 0x218 */
	u32 ca7_cpu2_pwr_con;
	u32 ca7_cpu3_pwr_con;		/* 0x220 */
	u32 reserved5[3];
	u32 ven_pwr_con;		/* 0x230 */
	u32 ifr_pwr_con;
	u32 isp_pwr_con;
	u32 dis_pwr_con;
	u32 dpy_pwr_con;		/* 0x240 */
	u32 ca7_cputop_l2_pdn;		/* 0x244 */
	u32 ca7_cputop_l2_sleep;
	u32 reserved6[4];
	struct {			/* 0x25c */
		u32 l1_pdn;
		u32 reserved;
	} ca7_cpu[4];
	u32 gcpu_sram_con;		/* 0x27c */
	u32 dpy2_pwr_con;		/* 0x280 */
	u32 md_pwr_con;
	u32 reserved11[2];
	u32 mcu_pwr_con;		/* 0x290 */
	u32 ifr_sramrom_con;
	u32 mjc_pwr_con;
	u32 audio_pwr_con;
	u32 ca15_cpu_pwr_con[4];	/* 0x2a0 */
	u32 ca15_cputop_pwr_con;	/* 0x2b0 */
	u32 ca15_l1_pwr_con;		/* 0x2b4 */
	u32 ca15_l2_pwr_con;		/* 0x2b8 */
	u32 reserved12;
	u32 mfg_2d_pwr_con;		/* 0x2c0 */
	u32 mfg_async_pwr_con;
	u32 vpu_sram_con;
	u32 reserved13[17];
	u32 pcm_con0;			/* 0x310 */
	u32 pcm_con1;
	u32 pcm_im_ptr;
	u32 pcm_im_len;
	u32 pcm_reg_data_ini;		/* 0x320 */
	u32 reserved14[7];
	u32 pcm_event_vector0;		/* 0x340 */
	u32 pcm_event_vector1;
	u32 pcm_event_vector2;
	u32 pcm_event_vector3;
	u32 reserved15;
	u32 pcm_mas_pause_mask;		/* 0x354 */
	u32 pcm_pwr_io_en;
	u32 pcm_timer_val;
	u32 pcm_timer_out;
	u32 reserved16[7];
	u32 pcm_reg_data[16];		/* 0x380 */
	u32 pcm_event_reg_sta;
	u32 pcm_fsm_sta;
	u32 pcm_im_host_rw_ptr;
	u32 pcm_im_host_rw_dat;
	u32 pcm_event_vector4;
	u32 pcm_event_vector5;
	u32 pcm_event_vector6;
	u32 pcm_event_vector7;
	u32 pcm_sw_int_set;
	u32 pcm_sw_int_clear;
	u32 reserved17[6];
	u32 clk_con;			/* 0x400 */
	u32 sleep_dual_vcore_pwr_con;	/* 0x404 */
	u32 sleep_ptpod2_con;
	u32 reserved18[125];
	u32 apmcu_pwrctl;		/* 0x600 */
	u32 ap_dvfs_con_set;
	u32 ap_stanby_con;
	u32 pwr_status;			/* 0x60c */
	u32 pwr_status_2nd;		/* 0x610 */
	u32 ap_bsi_req;
	u8 reserved19[0x720 - 0x618];
	u32 sleep_timer_sta;		/* 0x720 */
	u32 reserved20[15];
	u32 sleep_twam_con;		/* 0x760 */
	u32 sleep_twam_status0;
	u32 sleep_twam_status1;
	u32 sleep_twam_status2;
	u32 sleep_twam_status3;		/* 0x770 */
	u32 reserved21[39];
	u32 sleep_wakeup_event_mask;	/* 0x810 */
	u32 sleep_cpu_wakeup_event;
	u32 sleep_md32_wakeup_event_mask;
	u32 reserved22[2];
	u32 pcm_wdt_timer_val;		/* 0x824 */
	u32 pcm_wdt_timer_out;
	u32 reserved23;
	u32 pcm_md32_mailbox;		/* 0x830 */
	u32 pcm_md32_irq;
	u32 reserved24[50];
	u32 sleep_isr_mask;		/* 0x900 */
	u32 sleep_isr_status;
	u32 reserved25[2];
	u32 sleep_isr_raw_sta;		/* 0x910 */
	u32 sleep_md32_isr_raw_sta;
	u32 sleep_wakeup_misc;
	u32 sleep_bus_protect_rdy;
	u32 sleep_subsys_idle_sta;	/* 0x920 */
	u8 reserved26[0xb00 - 0x924];
	u32 pcm_reserve;		/* 0xb00 */
	u32 pcm_reserve2;
	u32 pcm_flags;
	u32 pcm_src_req;
	u32 reserved27[4];
	u32 pcm_debug_con;		/* 0xb20 */
	u32 reserved28[3];
	u32 ca7_cpu_irq_mask[4];	/* 0xb30 */
	u32 ca15_cpu_irq_mask[4];
	u32 reserved29[4];
	u32 pcm_pasr_dpd[4];		/* 0xb60 */
	u8 reserved30[0xf00 - 0xb70];
	u32 sleep_ca7_wfi_en[4];	/* 0xf00 */
	u32 sleep_ca15_wfi_en[4];
};

check_member(mtk_spm_regs, sleep_ca15_wfi_en[3], 0xf1c);

static struct mtk_spm_regs *const mtk_spm = (void *)SPM_BASE;

#endif  /* __SOC_MEDIATEK_MT8173_SPM_H__ */
