/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _MT8173_SOC_DDP_H_
#define _MT8173_SOC_DDP_H_

#include <soc/addressmap.h>
#include <soc/ddp_common.h>
#include <types.h>

struct mmsys_cfg_regs {
	u32 mmsys_inten;
	u32 mmsys_intsta;
	u32 mjc_apb_tx_con;
	u32 pwm_apb_err_addr;
	u8 reserved0[12];
	u32 isp_mout_en;
	u32 mdp_rdma0_mout_en;
	u32 mdp_prz0_mout_en;
	u32 mdp_prz1_mout_en;
	u32 mdp_prz2_mout_en;
	u32 mdp_tdshp0_mout_en;
	u32 mdp_tdshp1_mout_en;
	u32 mdp0_mout_en;
	u32 mdp1_mout_en;
	u32 disp_ovl0_mout_en;
	u32 disp_ovl1_mout_en;
	u32 disp_od_mout_en;
	u32 disp_gamma_mout_en;
	u32 disp_ufoe_mout_en;
	u32 mmsys_mout_rst;
	u32 mdp_prz0_sel_in;
	u32 mdp_prz1_sel_in;
	u32 mdp_prz2_sel_in;
	u32 mdp_tdshp0_sel_in;
	u32 mdp_tdshp1_sel_in;
	u32 mdp0_sel_in;
	u32 mdp1_sel_in;
	u32 mdp_crop_sel_in;
	u32 mdp_wdma_sel_in;
	u32 mdp_wrot0_sel_in;
	u32 mdp_wrot1_sel_in;
	u32 disp_color0_sel_in;
	u32 disp_color1_sel_in;
	u32 disp_aal_sel_in;
	u32 disp_path0_sel_in;
	u32 disp_path1_sel_in;
	u32 disp_wdma0_sel_in;
	u32 disp_wdma1_sel_in;
	u32 disp_ufoe_sel_in;
	u32 dsi0_sel_in;
	u32 dsi1_sel_in;
	u32 dpi_sel_in;
	u32 disp_rdma0_sout_sel_in;
	u32 disp_rdma1_sout_sel_in;
	u32 disp_rdma2_sout_sel_in;
	u32 disp_color0_sout_sel_in;
	u32 disp_color1_sout_sel_in;
	u32 disp_path0_sout_sel_in;
	u32 disp_path1_sout_sel_in;
	u8 reserved1[36];
	u32 mmsys_misc;
	u8 reserved2[12];
	u32 mmsys_cg_con0;
	u32 mmsys_cg_set0;
	u32 mmsys_cg_clr0;
	u8 reserved3[4];
	u32 mmsys_cg_con1;
	u32 mmsys_cg_set1;
	u32 mmsys_cg_clr1;
	u8 reserved4[4];
	u32 mmsys_hw_dcm_dis0;
	u32 mmsys_hw_dcm_dis_set0;
	u32 mmsys_hw_dcm_dis_clr0;
	u8 reserved5[4];
	u32 mmsys_hw_dcm_dis1;
	u32 mmsys_hw_dcm_dis_set1;
	u32 mmsys_hw_dcm_dis_clr1;
	u8 reserved6[4];
	u32 mmsys_sw0_rst_b;
	u32 mmsys_sw1_rst_b;
	u8 reserved7[8];
	u32 mmsys_lcm_rst_b;
	u8 reserved8[20];
	u32 smi_n21mux_cfg_wr;
	u32 smi_n21mux_cfg_rd;
	u32 ela2gmc_base_addr;
	u32 ela2gmc_base_addr_end;
	u32 ela2gmc_final_addr;
	u32 ela2gmc_status;
	u8 reserved9[128];
	u32 disp_fake_eng_en;
	u32 disp_fake_eng_rst;
	u32 disp_fake_eng_con0;
	u32 disp_fake_eng_con1;
	u32 disp_fake_eng_rd_addr;
	u32 disp_fake_eng_wr_addr;
	u32 disp_fake_eng_state;
	u8 reserved10[1508];
	u32 mmsys_mbist_con;
	u32 mmsys_mbist_done;
	u32 mmsys_mbist_holdb;
	u32 mmsys_mbist_mode;
	u32 mmsys_mbist_fail0;
	u32 mmsys_mbist_fail1;
	u32 mmsys_mbist_fail2;
	u8 reserved11[4];
	u32 mmsys_mbist_bsel[4];
	u32 mmsys_mem_delsel[6];
	u8 reserved12[56];
	u32 mmsys_debug_out_sel;
	u8 reserved13[12];
	u32 mmsys_dummy;
	u8 reserved14[12];
	u32 mmsys_mbist_rp_rst_b;
	u32 mmsys_mbist_rp_fail;
	u32 mmsys_mbist_rp_ok;
	u8 reserved15[4];
	u32 disp_dl_valid_0;
	u32 disp_dl_valid_1;
	u32 disp_dl_ready_0;
	u32 disp_dl_ready_1;
	u32 mdp_dl_valid_0;
	u32 mdp_dl_valid_1;
	u32 mdp_dl_ready_0;
	u32 mdp_dl_ready_1;
	u32 smi_larb0_greq;
	u8 reserved16[48];
	u32 hdmi_en;
};

check_member(mmsys_cfg_regs, mmsys_sw1_rst_b, 0x144);
check_member(mmsys_cfg_regs, hdmi_en, 0x904);
static struct mmsys_cfg_regs *const mmsys_cfg = (void *)MMSYS_BASE;

/* DISP_REG_CONFIG_MMSYS_CG_CON0
   Configures free-run clock gating 0
	0: Enable clock
	1: Clock gating  */
enum {
	CG_CON0_SMI_COMMON  = BIT(0),
	CG_CON0_SMI_LARB0   = BIT(1),
	CG_CON0_CAM_MDP     = BIT(2),
	CG_CON0_MDP_RDMA0   = BIT(3),
	CG_CON0_MDP_RDMA1   = BIT(4),
	CG_CON0_MDP_RSZ0    = BIT(5),
	CG_CON0_MDP_RSZ1    = BIT(6),
	CG_CON0_MDP_RSZ2    = BIT(7),
	CG_CON0_MDP_TDSHP0  = BIT(8),
	CG_CON0_MDP_TDSHP1  = BIT(9),
	CG_CON0_MDP_CROP    = BIT(10),
	CG_CON0_MDP_WDMA    = BIT(11),
	CG_CON0_MDP_WROT0   = BIT(12),
	CG_CON0_MDP_WROT1   = BIT(13),
	CG_CON0_FAKE_ENG    = BIT(14),
	CG_CON0_MUTEX_32K   = BIT(15),
	CG_CON0_DISP_OVL0   = BIT(16),
	CG_CON0_DISP_OVL1   = BIT(17),
	CG_CON0_DISP_RDMA0  = BIT(18),
	CG_CON0_DISP_RDMA1  = BIT(19),
	CG_CON0_DISP_RDMA2  = BIT(20),
	CG_CON0_DISP_WDMA0  = BIT(21),
	CG_CON0_DISP_WDMA1  = BIT(22),
	CG_CON0_DISP_COLOR0 = BIT(23),
	CG_CON0_DISP_COLOR1 = BIT(24),
	CG_CON0_DISP_AAL    = BIT(25),
	CG_CON0_DISP_GAMMA  = BIT(26),
	CG_CON0_DISP_UFOE   = BIT(27),
	CG_CON0_DISP_SPLIT0 = BIT(28),
	CG_CON0_DISP_SPLIT1 = BIT(29),
	CG_CON0_DISP_MERGE  = BIT(30),
	CG_CON0_DISP_OD     = BIT(31),
	CG_CON0_ALL	    = 0xffffffff
};

/* DISP_REG_CONFIG_MMSYS_CG_CON1
   Configures free-run clock gating 1
	0: Enable clock
	1: Clock gating */
enum {
	CG_CON1_DISP_PWM0_MM  = BIT(0),
	CG_CON1_DISP_PWM0_26M = BIT(1),
	CG_CON1_DISP_PWM1_MM  = BIT(2),
	CG_CON1_DISP_PWM1_26M = BIT(3),
	CG_CON1_DSI0_ENGINE   = BIT(4),
	CG_CON1_DSI0_DIGITAL  = BIT(5),
	CG_CON1_DSI1_ENGINE   = BIT(6),
	CG_CON1_DSI1_DIGITAL  = BIT(7),
	CG_CON1_DPI_PIXEL     = BIT(8),
	CG_CON1_DPI_ENGINE    = BIT(9),

	CG_CON1_ALL	      = 0xffffffff
};

enum {
	OVL0_MOUT_EN_COLOR0 = BIT(0),
	OD_MOUT_EN_RDMA0    = BIT(0),
	UFOE_MOUT_EN_DSI0   = BIT(0),
	UFOE_MOUT_EN_SPLIT1 = BIT(1),
};

enum {
	COLOR0_SEL_IN_OVL0  = 1,
	DSI0_SEL_IN_UFOE    = 0,
	DSI0_SEL_IN_SPLIT1  = 1,
	DSI1_SEL_IN_SPLIT1  = 0,
};

/* MMSYS_SW1_RST_B */
enum {
	MMSYS_SW1_RST_DSI0_B = BIT(2),
	MMSYS_SW1_RST_DSI1_B = BIT(3),
};

struct disp_mutex_regs {
	u32 inten;
	u32 intsta;
	u8 reserved0[24];
	struct {
		u32 en;
		u32 dummy;
		u32 rst;
		u32 mod;
		u32 sof;
		u32 reserved[3];
	} mutex[6];
	u8 reserved1[32];
	u32 debug_out_sel;
};

check_member(disp_mutex_regs, debug_out_sel, 0x100);
static struct disp_mutex_regs *const disp_mutex = (void *)DISP_MUTEX_BASE;

enum {
	MUTEX_MOD_DISP_OVL0	= BIT(11),
	MUTEX_MOD_DISP_RDMA0	= BIT(13),
	MUTEX_MOD_DISP_COLOR0	= BIT(18),
	MUTEX_MOD_DISP_AAL	= BIT(20),
	MUTEX_MOD_DISP_UFOE	= BIT(22),
	MUTEX_MOD_DISP_OD	= BIT(25),
	MUTEX_MOD_MAIN_PATH	= MUTEX_MOD_DISP_OVL0 | MUTEX_MOD_DISP_RDMA0 |
				  MUTEX_MOD_DISP_COLOR0 | MUTEX_MOD_DISP_AAL |
				  MUTEX_MOD_DISP_UFOE | MUTEX_MOD_DISP_OD,
};

struct disp_od_regs {
	u32 en;
	u32 reset;
	u32 inten;
	u32 ints;
	u32 status;
	u8 reserved0[12];
	u32 cfg;
	u32 input_count;
	u32 output_count;
	u32 chks_um;
	u32 size;
	u8 reserved1[12];
	u32 hsync_width;
	u32 vsync_width;
	u32 misc;
};

check_member(disp_od_regs, misc, 0x48);
static struct disp_od_regs *const disp_od = (void *)DISP_OD_BASE;

enum {
	OD_RELAY_MODE = BIT(0),
};

struct disp_ufoe_regs {
	u32 start;
	u32 inten;
	u32 intsta;
	u32 dbuf;
	u8 reserved0[4];
	u32 crc;
	u32 sw_scratch;
	u8 reserved1[4];
	u32 cr0p6_pad;
	u8 reserved2[4];
	u32 ck_on;
	u8 reserved3[36];
	u32 frame_width;
	u32 frame_height;
	u32 outen;
	u8 reserved4[148];
	u32 r0_crc;
	u8 reserved5[12];
	u32 cfg[5];
	u8 reserved6[12];
	u32 ro[5];
	u8 reserved7[12];
	u32 dbg[8];
};

check_member(disp_ufoe_regs, dbg[7], 0x15C);
static struct disp_ufoe_regs *const disp_ufoe = (void *)DISP_UFOE_BASE;

enum {
	UFO_BYPASS = BIT(2),
	UFO_LR = BIT(3) | BIT(0),
};

void mtk_ddp_init(void);
void mtk_ddp_mode_set(const struct edid *edid);

#endif
