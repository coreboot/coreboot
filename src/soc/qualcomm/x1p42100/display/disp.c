/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <edid.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cmd_db.h>
#include <soc/display/disp.h>
#include <soc/display/dsc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/panel_ops.h>
#include <soc/rpmh.h>
#include <soc/rpmh_bcm.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>
#include <soc/symbols_common.h>
#include <string.h>
#include <timer.h>

static struct clock_freq_config disp_cc_mdss_ahb_cfg[] = {
	{
		.hz = SRC_XO_HZ,        /* 19.2MHz */
		.src = SRC_XO_19_2MHZ_AHB,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = CLK_37_5MHZ,
		.src = SRC_DISP_CC_PLL1_MAIN_AHB,
		.div = QCOM_CLOCK_DIV(16),
	},
	{
		.hz = CLK_75MHZ,
		.src = SRC_DISP_CC_PLL1_MAIN_AHB,
		.div = QCOM_CLOCK_DIV(8),
	},
};

static struct clock_freq_config disp_cc_mdss_mdp_cfg[] = {
	{
		.hz = CLK_575MHZ,
		.src = SRC_DISP_CC_PLL0_MAIN_MDP,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = CLK_400MHZ,
		.src = SRC_DISP_CC_PLL1_MAIN_MDP,
		.div = QCOM_CLOCK_DIV(1.5),
	},
};

static void mdss_ubwc_config(void)
{
	/*
	 * MDSS HW UBWC config - platform constants for x1p42100.
	 * Source: UEFI hal_sde_10_0_0.c HAL_MDP_10_0_0_UBWC_DEFAULT_*
	 */
	write32(&mdss_hw->ubwc_static_ctrl,     MDSS_UBWC_STATIC_CTRL_VAL);
	write32(&mdss_hw->ubwc_ctrl_2,          MDSS_UBWC_VERSION);
	write32(&mdss_hw->ubwc_prediction_mode, MDSS_UBWC_PREDICTION_MODE);
}

void enable_mdss_clk(void)
{
	mdss_clock_enable(GCC_DISP_AHB_CBCR);
	/*
	 * gcc_disp_xo_clk must be enabled before GDSC - keeps DISP_CC bus alive.
	 * Source: UEFI RsccClocks_92x[] in MDSSPlatform_9xx.h
	 */
	mdss_clock_enable(GCC_DISP_XO_CBCR);
	/*
	 * disp_cc_mdss_rscc_ahb_clk and disp_cc_mdss_rscc_vsync_clk must be
	 * enabled before GDSC - these gate access to DISP_CC register space.
	 * Source: UEFI RsccClocks_92x[] in MDSSPlatform_9xx.h
	 */
	mdss_clock_enable(DISP_CC_MDSS_RSCC_AHB_CBCR);
	mdss_clock_enable(DISP_CC_MDSS_RSCC_VSYNC_CBCR);
	/*
	 * Step 1: Confirm DISP_CC bus is accessible - read a known CBCR register
	 * (mdss_pclk0_cbcr at disp_cc+0x4). If this hangs, AHB/XO clocks are wrong.
	 */
	printk(BIOS_INFO, "disp_cc pclk0_cbcr: 0x%08x\n",
		read32(&disp_cc->mdss_pclk0_cbcr));

	/*
	 * Step 2: Read GDSCR before SW enable attempt.
	 * bit[31] = PWR_ON      (1 = already powered on)
	 * bit[ 1] = HW_CONTROL  (1 = HW owns GDSC, SW_COLLAPSE writes ignored)
	 * bit[ 0] = SW_COLLAPSE (1 = collapsed/off, 0 = enabled)
	 * If HW_CONTROL=1 we cannot enable via SW - need RSCC/HW path.
	 */
	{
		u32 gdscr = read32(&disp_cc->mdss_core_gdscr);
		printk(BIOS_INFO, "GDSCR=0x%08x PWR_ON=%u HW_CTL=%u SW_COLLAPSE=%u\n",
			gdscr,
			(gdscr >> 31) & 1,
			(gdscr >> 1) & 1,
			(gdscr >> 0) & 1);
	}
	clock_enable_disp_gdsc(DISP_CC_CORE_GDSC);
	mdss_clock_enable(GCC_DISP_HF_AXI_CBCR);
	disp_pll_init_and_set(apss_disp_pll0, L_VAL_1725MHz, DISP_PLL0_ALPHA_VAL);
	disp_pll_init_and_set(apss_disp_pll1, L_VAL_600MHz, DISP_PLL1_ALPHA_VAL);
	clock_configure(&disp_cc->mdss_ahb_rcg,
		disp_cc_mdss_ahb_cfg, SRC_XO_HZ, ARRAY_SIZE(disp_cc_mdss_ahb_cfg));
	mdss_clock_enable(DISP_CC_MDSS_AHB_CBCR);
	clock_configure(&disp_cc->mdss_mdp_rcg,
		disp_cc_mdss_mdp_cfg, CLK_575MHZ, ARRAY_SIZE(disp_cc_mdss_mdp_cfg));
	/*
	 * Set HW_CLK_CONTROL (bit[20]) in MDP_CFG_RCGR.
	 * Bit[20]=HW_CLK_CONTROL=1 enables hardware clock control for MDP RCG.
	 * Without this, MDP clock may not run at full speed and cause underrun.
	 */
	setbits32(&disp_cc->mdss_mdp_rcg.rcg_cfg, BIT(20));
	mdss_clock_enable(DISP_CC_MDSS_MDP_CBCR);
	mdss_clock_enable(DISP_CC_MDSS_VSYNC_CBCR);
	enable_disp_clock_tcsr();
	/* Configure UBWC hardware after clocks are enabled */
	mdss_ubwc_config();
}

static void disable_mdss_dsi0_clk(void)
{
	mdss_clock_disable(DISP_CC_MDSS_ESC0_CBCR);
	mdss_clock_disable(DISP_CC_MDSS_BYTE0_CBCR);
	mdss_clock_disable(DISP_CC_MDSS_BYTE0_INTF_CBCR);
	mdss_clock_disable(DISP_CC_MDSS_PCLK0_CBCR);
}

static void disable_mdss_dsi1_clk(void)
{
	mdss_clock_disable(DISP_CC_MDSS_ESC1_CBCR);
	mdss_clock_disable(DISP_CC_MDSS_BYTE1_CBCR);
	mdss_clock_disable(DISP_CC_MDSS_BYTE1_INTF_CBCR);
	mdss_clock_disable(DISP_CC_MDSS_PCLK1_CBCR);
}

void disable_mdss_clk(void)
{
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();

	if (panel && panel->config && panel->config->dsi_controller == 1)
		disable_mdss_dsi1_clk();
	else
		disable_mdss_dsi0_clk();
	/* Disable MDP clocks */
	clock_disable(&disp_cc->mdss_mdp_cbcr);
	clock_disable(&disp_cc->mdss_vsync_cbcr);
	clock_disable(&disp_cc->mdss_ahb_cbcr);
	/* Disable GDSC */
	clock_disable_disp_gdsc(DISP_CC_CORE_GDSC);
	/* Disable GCC clocks */
	clock_disable(&gcc->gcc_disp_hf_axi_cbcr);
	clock_disable(&gcc->gcc_disp_ahb_cbcr);
}

enum cb_err display_rpmh_init(void)
{
	enum cb_err ret;
	struct rpmh_vreg arc_reg;
	int rc;
	volatile u32 *boot_cookie = (volatile u32 *)AOP_BOOT_COOKIE_ADDR;

	if (!wait_us(AOP_BOOT_TIMEOUT_US, *boot_cookie == AOP_BOOT_COOKIE)) {
		printk(BIOS_ERR, "AOP not booted after %dus (cookie: 0x%x, expected: 0x%x)\n",
			AOP_BOOT_TIMEOUT_US, *boot_cookie, AOP_BOOT_COOKIE);
		return CB_ERR;
	}
	printk(BIOS_INFO, "AOP boot detected (cookie: 0x%x)\n", *boot_cookie);

	ret = cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE);
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "Failed to initialize Command DB\n");
		return CB_ERR;
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "Failed to initialize RPMh RSC\n");
		return CB_ERR;
	}

	rc = rpmh_regulator_init(&arc_reg, "mmcx.lvl", RPMH_REGULATOR_TYPE_ARC);
	if (rc) {
		printk(BIOS_ERR, "Failed to initialize mmcx ARC regulator\n");
		return CB_ERR;
	}

	rc = rpmh_regulator_arc_set_level(&arc_reg, RPMH_REGULATOR_LEVEL_NOM_L1, true, false);
	if (rc) {
		printk(BIOS_ERR, "Failed to set ARC level\n");
		return CB_ERR;
	}

	/*
	 * Wait for mmcx voltage to ramp up before enabling DISP_CC GDSC.
	 * RPMh TCS completion only means the request was accepted - the
	 * actual voltage ramp is asynchronous. GDSC enable will hang if
	 * mmcx has not reached NOM_L1 yet. 5ms is conservative.
	 */
	mdelay(5);

	rc = rpmh_bcm_vote("MM0", BCM_MM0_VOTE_VALUE);
	if (rc) {
		printk(BIOS_ERR, "Failed to send BCM vote for MM0\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "RPMh display init complete\n");
	return CB_SUCCESS;
}

/* Returns the active panel ops based on Kconfig selection */
const struct mdss_panel_ops *mdss_get_panel_ops(void)
{
	return mdss_get_active_panel_ops();
}

/*
 * mdss_dump_display_regs() - raw address-range register dump for purwa.
 *
 * Dumps all registers in each HW block by looping over the address range.
 * Format: "DISP_DUMP: 0xADDR = 0xVALUE" - grep-friendly for parsing.
 *
 * Purwa (x1p42100) base addresses (MDSS_BASE = 0xAE00000):
 *   CTL_0       MDSS+0x16000   SSPP_VIG0   MDSS+0x05000
 *   SSPP_VIG1   MDSS+0x07000   DSC0        MDSS+0x81000
 *   DSC1        MDSS+0x82000   INTF_1      MDSS+0x36000
 *   INTF_2      MDSS+0x37000   DSI0_CTL    MDSS+0x94000
 *   DSI0_PHY    MDSS+0x95000   DSI0_PLL    MDSS+0x95500
 *   DSI1_CTL    MDSS+0x96000   DSI1_PHY    MDSS+0x97000
 *   DSI1_PLL    MDSS+0x97500
 */
static void __attribute__((unused)) mdss_dump_display_regs(void)
{
	uint32_t i, addr, val;

	struct {
		uint32_t base;
		uint32_t size;
		const char *name;
	} blocks[] = {
		/* CTL_0: MDSS+0x16000 */
		{ 0x0AE16000, 0x200,  "CTL_0"    },
		/* SSPP VIG0: MDSS+0x05000 */
		{ 0x0AE05000, 0x200,  "SSPP_VIG0"},
		/* SSPP VIG1: MDSS+0x07000 */
		{ 0x0AE07000, 0x200,  "SSPP_VIG1"},
		/* DSC0 CMN+ENC0+ENC1: MDSS+0x81000 */
		{ 0x0AE81000, 0x300,  "DSC0"     },
		/* DSC1 CMN+ENC0+ENC1: MDSS+0x82000 */
		{ 0x0AE82000, 0x300,  "DSC1"     },
		/* INTF_1: MDSS+0x36000 */
		{ 0x0AE36000, 0x300,  "INTF_1"   },
		/* INTF_2: MDSS+0x37000 */
		{ 0x0AE37000, 0x300,  "INTF_2"   },
		/* DSI0 controller: MDSS+0x94000 */
		{ 0x0AE94000, 0x300,  "DSI0_CTL" },
		/* DSI0 PHY CMN+lanes: MDSS+0x95000 */
		{ 0x0AE95000, 0x500,  "DSI0_PHY" },
		/* DSI0 PLL: MDSS+0x95500 */
		{ 0x0AE95500, 0x270,  "DSI0_PLL" },
		/* DSI1 controller: MDSS+0x96000 */
		{ 0x0AE96000, 0x300,  "DSI1_CTL" },
		/* DSI1 PHY CMN+lanes: MDSS+0x97000 */
		{ 0x0AE97000, 0x500,  "DSI1_PHY" },
		/* DSI1 PLL: MDSS+0x97500 */
		{ 0x0AE97500, 0x270,  "DSI1_PLL" },
	};

	for (i = 0; i < ARRAY_SIZE(blocks); i++) {
		printk(BIOS_DEBUG, "\n===== DISP_DUMP: %s base=0x%08X size=0x%X =====\n",
		       blocks[i].name, blocks[i].base, blocks[i].size);
		for (addr = 0; addr < blocks[i].size; addr += 4) {
			val = read32((void *)(uintptr_t)(blocks[i].base + addr));
			printk(BIOS_DEBUG, "DISP_DUMP: 0x%08X = 0x%08X\n",
			       blocks[i].base + addr, val);
		}
	}
	printk(BIOS_DEBUG, "\n===== DISP_DUMP COMPLETE =====\n");
}

/*
 * mdss_dsi_display_init() - Prepare the DSI + DSC display pipeline.
 *
 * Call sequence:
 *   1. DSI PHY 2.7.0 init (PHY power-up, PLL, clocks, timings) + host controller init
 *   2. DSC engine config (2-2-1 or 2-2-2 topology)
 *   3. MDP SSPP + Layer Mixer + VBIF setup
 *   4. Board panel power-on (GPIOs, I2C bias setup, settling delays, reset)
 *   5. Panel init commands (Sleep Out + Display On), then DSC PPS
 *   6. DSI video compression mode registers
 *   7. DSI video mode timing registers
 *   8. INTF timing generator setup + CTL flush
 *   9. Defer timing engine start until display startup has rendered the logo
 */
enum cb_err mdss_dsi_display_init(struct edid *edid_out, enum cb_err (*power_on)(void))
{
	struct edid edid = {0};
	u32 bpp = 24;
	u32 lanes;

	if (!edid_out)
		return CB_ERR;

	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
	if (!panel) {
		printk(BIOS_ERR, "Display: mdss_get_panel_ops() returned NULL\n");
		return CB_ERR;
	}
	if (!panel->edid_mode) {
		printk(BIOS_ERR, "Display: panel->edid_mode is NULL\n");
		return CB_ERR;
	}
	if (!panel->config) {
		printk(BIOS_ERR, "Display: panel->config is NULL\n");
		return CB_ERR;
	}

	const struct mdss_panel_config *cfg = panel->config;
	lanes = cfg->num_lanes ? cfg->num_lanes : 4;

	edid.mode = *panel->edid_mode;
	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	printk(BIOS_INFO, "Display: DSI%u, %u %s lanes, %s mode\n",
	       cfg->dsi_controller, lanes,
	       cfg->cphy_mode ? "C-PHY" : "D-PHY",
	       cfg->command_mode ? "command" : "video");
	printk(BIOS_INFO, "Display: edid ha=%u va=%u hbl=%u vbl=%u pclk=%u\n",
	       edid.mode.ha, edid.mode.va, edid.mode.hbl, edid.mode.vbl,
	       edid.mode.pixel_clock);
	printk(BIOS_INFO, "Display: cfg pipe_count=%u dsc_ratio_x1000=%u intf_num=%u dsi_count=%u\n",
	       cfg->pipe_count, cfg->dsc_ratio_x1000, cfg->intf_num, cfg->dsi_count);

	/*
	 * Step 1: DSI PHY 2.7.0 init + host controller init.
	 * Initialize the PHY and host before panel power-on and reset.
	 * Video mode remains disabled during power-on and panel init commands.
	 */
	if (cfg->dsi_count > 1 || !cfg->dsi_controller) {
		if (mdss_dsi_config(&edid, lanes, bpp) != CB_SUCCESS) {
			printk(BIOS_ERR, "Display: DSI0 config failed\n");
			return CB_ERR;
		}
	}
	if (cfg->dsi_count > 1 || cfg->dsi_controller) {
		if (mdss_dsi1_config(&edid, lanes, bpp) != CB_SUCCESS) {
			printk(BIOS_ERR, "Display: DSI1 config failed\n");
			return CB_ERR;
		}
	}

	/* Step 2: DSC engine */
	mdss_dsc_config(panel->dsc_params);
	if (cfg->command_mode)
		mdss_dsi_command_mode_config(&edid);

	/* Step 3: MDP SSPP, Layer Mixer, VBIF */
	configure_vbif_qos();
	uintptr_t fb_addr = (REGION_SIZE(framebuffer)) ? (uintptr_t)_framebuffer : 0;
	/*
	 * Pre-fill framebuffer with black to avoid corruption during boot.
	 * Without this, zeroed framebuffer shows garbage until OS fills it.
	 */
	if (fb_addr)
		memset((void *)fb_addr, 0,
			(size_t)edid.mode.ha * edid.mode.va * 4);
	mdss_source_pipe_config(&edid, fb_addr, cfg);
	mdss_layer_mixer_setup(&edid, cfg);

	/*
	 * merge_3d - DSC_CMN_ENC_MAIN_CONF=0x83 (SPLIT_PANEL|MULTIPLEX_MODE)
	 * handles the split internally without CTL merge_3d.
	 */
	write32(&mdp_ctl_0->ctl_merge_3d_active, 0x0);
	write32(&mdp_ctl_0->merge_3d_flush, 0x0);

	/*
	 * Step 4: Complete board power-on and reset immediately before panel commands.
	 * The host is configured for LP command transfers, with video still disabled.
	 */
	if (power_on && power_on() != CB_SUCCESS) {
		printk(BIOS_ERR, "Display: panel power-on failed\n");
		return CB_ERR;
	}

	/*
	 * Step 5: Panel init commands MUST be sent BEFORE video mode starts.
	 * DSI ctrl must NOT have VIDEO_EN=1 when sending CMD mode DMA.
	 * panel_initialize() temporarily enables CMD mode (ctrl|0x04),
	 * sends commands, then restores ctrl.
	 */
	if (mdss_dsi_panel_initialize(panel->get_init_cmds()) != CB_SUCCESS) {
		printk(BIOS_ERR, "Display: panel init commands failed\n");
		return CB_ERR;
	}
	if (panel->dsc_cfg &&
	    mdss_dsi_send_pps(panel->dsc_cfg, &edid.mode, panel->dsc_params) != CB_SUCCESS) {
		printk(BIOS_ERR, "Display: DSC PPS send failed\n");
		return CB_ERR;
	}

	/*
	 * Step 6: DSI video compression mode registers.
	 * WC[31:16]=bytes_per_pkt, DATATYPE[13:8]=0x0B (compressed image),
	 * PKT_PER_LINE[7:6], EOL_BYTE_NUM[5:4], EN[0]=1.
	 */
	if (!cfg->command_mode) {
		struct dsi_regs *target = cfg->dsi_controller ? dsi1 : dsi0;
		write32(&target->video_compression_mode_ctrl, panel->video_compression_ctrl);
		write32(&target->video_compression_mode_ctrl2, panel->video_compression_ctrl2);
		if (cfg->dsi_count > 1) {
			write32(&dsi0->video_compression_mode_ctrl, panel->video_compression_ctrl);
			write32(&dsi0->video_compression_mode_ctrl2, panel->video_compression_ctrl2);
			write32(&dsi1->video_compression_mode_ctrl, panel->video_compression_ctrl);
			write32(&dsi1->video_compression_mode_ctrl2, panel->video_compression_ctrl2);
		}
	}

	/* Step 7: DSI video mode timing registers */
	if (!cfg->command_mode) {
		if (cfg->dsi_controller)
			mdss_dsi1_video_mode_config(&edid, bpp);
		else
			mdss_dsi_video_mode_config(&edid, bpp);
		if (cfg->dsi_count > 1)
			mdss_dsi1_video_mode_config(&edid, bpp);
	}

	if (!cfg->command_mode) {
		/* Step 8: INTF timing generator setup */
		intf_tg_setup(&edid, cfg);
		intf_fetch_start_config(&edid, cfg);
	}
	merge_3d_active(&edid, cfg);
	if (cfg->command_mode) {
		/* Dedicated TE/vsync setup from the UEFI MDP command-mode profile. */
		write32(&mdp_intf2->intf_mux, 0xF0000); /* PP0 -> INTF_2 */
		write32(&mdp_intf2_tear->mdp_vsync_sel, 1);
		write32(&mdp_intf2_tear->tear_check_en, BIT(0) | BIT(3));
		write32(&mdp_intf2_tear->sync_config_vsync,
			BIT(20) | BIT(19) | (19200000U /
				((edid.mode.va + edid.mode.vbl) * edid.mode.refresh)));
		write32(&mdp_intf2_tear->sync_config_height, 0xfffffff0);
		write32(&mdp_intf2_tear->rd_ptr_irq, edid.mode.va + 1);
		write32(&mdp_intf2_tear->wr_ptr_irq, 1);
		write32(&mdp_intf2_tear->vsync_init_val, edid.mode.va);
		write32(&mdp_intf2_tear->sync_thresh, (4 << 16) | 4);
		write32(&mdp_intf2_tear->start_pos, edid.mode.va);
		write32(&mdp_intf2_tear->sync_wrcount, edid.mode.va + 5);
	}

	/*
	 * Step 8b: CTL INTF_ACTIVE + INTF_FLUSH before timing engine.
	 * Sequence: CTL_FLUSH -> TIMING_ENGINE_EN=1.
	 * CTL_START is NOT used for video mode (only cmd/writeback).
	 */
	write32(&mdp_ctl_0->ctl_intf_master, MDSS_INTF_MASTER(cfg));
	write32(&mdp_ctl_0->ctl_intf_active,  MDSS_INTF_ACTIVE(cfg));
	write32(&mdp_ctl_0->ctl_intf_flush,   MDSS_INTF_FLUSH(cfg));

	/* Return edid for framebuffer registration and rendering before scanout starts. */
	*edid_out = edid;
	return CB_SUCCESS;
}

void mdss_dsi_display_stop(void)
{
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
	const struct mdss_panel_config *cfg = panel ? panel->config : NULL;
	if (cfg && cfg->command_mode)
		write32(&mdp_intf2_tear->autorefresh_config, 0);
	else
		write32(cfg && cfg->dsi_controller ? &mdp_intf2->timing_eng_enable :
			&mdp_intf->timing_eng_enable, 0x0);
	mdelay(20);

	if (panel && panel->off_cmds) {
		/* With the timing engine stopped, send LP commands without video scheduling. */
		clrbits32(cfg->dsi_controller ? &dsi1->ctrl : &dsi0->ctrl, BIT(1));
		if (cfg->dsi_count > 1)
			clrbits32(&dsi1->ctrl, BIT(1));
		if (mdss_dsi_panel_initialize(panel->off_cmds) != CB_SUCCESS)
			printk(BIOS_ERR, "Display: panel off commands failed\n");
	}

	/* Disable DSI clocks */
	disable_mdss_clk();
}

void mdss_dsi_command_mode_start(void)
{
	/* Start one framebuffer update and then enable panel TE auto-refresh. */
	write32(&mdp_ctl_0->ctl_start, 1);
	dsb();
	write32(&mdp_intf2_tear->autorefresh_config, BIT(31) | 1);
}
