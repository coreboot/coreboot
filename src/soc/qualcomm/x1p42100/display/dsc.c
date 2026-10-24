/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSC (Display Stream Compression) engine configuration for purwa (x1p42100).
 * Topology: 2-2-1 (2 Layer Mixers, 2 DSC encoders, 1 DSI interface).
 *
 * Panel: r66451 AMOLED, 1080x2340, DSC 1.1, profile V1_1_RGB444_8BPC_8BPP
 * DSC params: slice 540x20, 8bpc, 8bpp, block_pred=true, 2 slices/packet
 *
 * All RC parameters are pre-calculated constants for this fixed panel config.
 * Source: UEFI hal_mdp_dsc.h profile table index 19 (V1_1_RGB444_8BPC_8BPP)
 *         + HAL_MDP_DSC_ENC_Commit() register field layout from mdssreg_1000.h
 *
 * Register field positions verified against:
 *   HWIO_MDP_DSC_0_DSC_0_ENC_* in mdssreg_1000.h
 *   HWIO_MDP_DSC_0_DSC_CMN_* in mdssreg_1000.h
 */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/display/dsc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

/* Program one DSC encoder (ENC0 or ENC1 within DSC core 0) */
static void dsc_program_encoder(struct dsc_enc_regs *enc,
				const struct mdss_dsc_params *p)
{
	write32(&enc->enc_df_ctrl,                p->enc_df_ctrl);
	write32(&enc->dsc_main_conf,              p->dsc_main_conf);
	write32(&enc->dsc_picture_size,           p->dsc_picture_size);
	write32(&enc->dsc_slice_size,             p->dsc_slice_size);
	write32(&enc->dsc_misc_size,              p->dsc_misc_size);
	write32(&enc->dsc_hrd_delays,             p->dsc_hrd_delays);
	write32(&enc->dsc_rc_scale,               p->dsc_rc_scale);
	write32(&enc->dsc_rc_scale_inc_dec,       p->dsc_rc_scale_inc_dec);
	write32(&enc->dsc_rc_offsets_1,           p->dsc_rc_offsets_1);
	write32(&enc->dsc_rc_offsets_2,           p->dsc_rc_offsets_2);
	write32(&enc->dsc_rc_offsets_3,           p->dsc_rc_offsets_3);
	write32(&enc->dsc_flatness_qp,            p->dsc_flatness_qp);
	write32(&enc->dsc_rc_model_size,          p->dsc_rc_model_size);
	write32(&enc->dsc_rc_config,              p->dsc_rc_config);
	write32(&enc->dsc_rc_buf_thresh_0,        p->dsc_rc_buf_thresh_0);
	write32(&enc->dsc_rc_buf_thresh_1,        p->dsc_rc_buf_thresh_1);
	write32(&enc->dsc_rc_buf_thresh_2,        p->dsc_rc_buf_thresh_2);
	write32(&enc->dsc_rc_buf_thresh_3,        p->dsc_rc_buf_thresh_3);
	write32(&enc->dsc_rc_min_qp_0,            p->dsc_rc_min_qp_0);
	write32(&enc->dsc_rc_min_qp_1,            p->dsc_rc_min_qp_1);
	write32(&enc->dsc_rc_min_qp_2,            p->dsc_rc_min_qp_2);
	write32(&enc->dsc_rc_max_qp_0,            p->dsc_rc_max_qp_0);
	write32(&enc->dsc_rc_max_qp_1,            p->dsc_rc_max_qp_1);
	write32(&enc->dsc_rc_max_qp_2,            p->dsc_rc_max_qp_2);
	write32(&enc->dsc_rc_range_bpg_offsets_0, p->dsc_rc_range_bpg_offsets_0);
	write32(&enc->dsc_rc_range_bpg_offsets_1, p->dsc_rc_range_bpg_offsets_1);
	write32(&enc->dsc_rc_range_bpg_offsets_2, p->dsc_rc_range_bpg_offsets_2);
}

/*
 * Configure DSC for 2-2-1 or 2-2-2 topology.
 *
 * 2-2-1: 2 Layer Mixers -> merge_3d -> 2 DSC encoders (ENC0+ENC1 in DSC core 0)
 *        -> single DSI interface (INTF5)
 *
 * For single-DSI topology, DSC core 0 is used and both ENC0 and ENC1 within
 * DSC core 0 are programmed identically.
 * ENC_MAIN_CONF: SPLIT_PANEL=1 (two halves), MULTIPLEX_MODE=1 (merge output).
 */
void mdss_dsc_config(const struct mdss_dsc_params *p)
{
	bool is_dual_dsi = !(p->enc_main_conf & BIT(1)); /* BIT(1)=MULTIPLEX_MODE: 0=2-2-2 */

	/* Step 1: Program DSC common block */
	write32(&dsc0_cmn->enc_main_conf, p->enc_main_conf);

	/* Step 2: Program both encoders in DSC core 0 */
	dsc_program_encoder(dsc0_enc0, p);
	dsc_program_encoder(dsc0_enc1, p);

	/*
	 * Step 2b: For 2-2-2, also program DSC core 1 (drives DSI1).
	 * Dump confirmed DSC1 enc_main_conf=0x80 (reset) when not programmed.
	 */
	if (is_dual_dsi) {
		write32(&dsc1_cmn->enc_main_conf, p->enc_main_conf);
		dsc_program_encoder(dsc1_enc0, p);
		dsc_program_encoder(dsc1_enc1, p);
	}

	/*
	 * Step 3: Program DSC wrapper CTL/CFG/DATA_IN_SWAP/CLK_CTRL
	 * DSC_CFG SPLIT_PANEL bit[13]:
	 *   2-2-1 (multiplex_mode=1): SPLIT_PANEL=1 -> 0x3801 (encoders merge to 1 INTF)
	 *   2-2-2 (multiplex_mode=0): SPLIT_PANEL=0 -> 0x1801 (each encoder to own INTF)
	 * Confirmed from HLOS golden dumps.
	 */
	u32 dsc_cfg = is_dual_dsi ? 0x1801 : 0x3801;
	write32(&dsc0_ctl0->dsc_ctl,          0x0);
	write32(&dsc0_ctl0->dsc_cfg,          dsc_cfg);
	write32(&dsc0_ctl0->dsc_data_in_swap, 0x2C688);
	/* dsc_clk_ctrl: 0x0 - customer confirmed 0xC0000000 causes corruption */
	write32(&dsc0_ctl0->dsc_clk_ctrl,     0x0);
	write32(&dsc0_ctl1->dsc_ctl,          0x1);
	write32(&dsc0_ctl1->dsc_cfg,          dsc_cfg);
	write32(&dsc0_ctl1->dsc_data_in_swap, 0x2C688);
	write32(&dsc0_ctl1->dsc_clk_ctrl,     0x0);
	if (is_dual_dsi) {
		write32(&dsc1_ctl0->dsc_ctl,          0x0);
		write32(&dsc1_ctl0->dsc_cfg,          dsc_cfg);
		write32(&dsc1_ctl0->dsc_data_in_swap, 0x2C688);
		write32(&dsc1_ctl0->dsc_clk_ctrl,     0x0);
		write32(&dsc1_ctl1->dsc_ctl,          0x1);
		write32(&dsc1_ctl1->dsc_cfg,          dsc_cfg);
		write32(&dsc1_ctl1->dsc_data_in_swap, 0x2C688);
		write32(&dsc1_ctl1->dsc_clk_ctrl,     0x0);
	}

	/*
	 * Step 4: Enable DSC in CTL and flush
	 * dsc_active: bit 0 = DSC_0, bit 1 = DSC_1 (within core 0, both encoders)
	 * dsc_flush:  same bits
	 */
	write32(&mdp_ctl_0->dsc_active, 0x3);
	write32(&mdp_ctl_0->dsc_flush,  0x3);

	printk(BIOS_INFO, "DSC: configuration complete (topology: %s)\n",
	       is_dual_dsi ? "2-2-2" : "2-2-1");
}
