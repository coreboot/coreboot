/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <timer.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cmd_db.h>
#include <soc/rpmh.h>
#include <soc/rpmh_bcm.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>

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

void enable_mdss_clk(void)
{
	/*
	 * Display clock initialization sequence
	 * 1. Enable GCC clocks (AHB, AXI) - GCC clocks that are required for display
	 * 2. Enable GDSC (power domain) - powers up the display subsystem
	 * 3. Initialize display PLLs - required to use clock sources from disp_cc domain
	 * 4. Configure and enable disp_cc clocks - enable display clocks
	 */
	mdss_clock_enable(GCC_DISP_AHB_CBCR);
	clock_enable_disp_gdsc(DISP_CC_CORE_GDSC);
	mdss_clock_enable(GCC_DISP_HF_AXI_CBCR);
	disp_pll_init_and_set(apss_disp_pll0, L_VAL_1725MHz, DISP_PLL0_ALPHA_VAL);
	disp_pll_init_and_set(apss_disp_pll1, L_VAL_600MHz, DISP_PLL1_ALPHA_VAL);
	clock_configure(&disp_cc->mdss_ahb_rcg,
		disp_cc_mdss_ahb_cfg, CLK_75MHZ, ARRAY_SIZE(disp_cc_mdss_ahb_cfg));
	mdss_clock_enable(DISP_CC_MDSS_AHB_CBCR);
	clock_configure(&disp_cc->mdss_mdp_rcg,
		disp_cc_mdss_mdp_cfg, CLK_575MHZ, ARRAY_SIZE(disp_cc_mdss_mdp_cfg));
	mdss_clock_enable(DISP_CC_MDSS_MDP_CBCR);
	mdss_clock_enable(DISP_CC_MDSS_VSYNC_CBCR);
	enable_disp_clock_tcsr();
}

/**
 * display_rpmh_init() - Initialize RPMh for display power management
 *
 * Initializes the RPMh (Resource Power Manager-Hardened) subsystem for display:
 * - Waits for AOP boot
 * - Initializes Command DB and RPMh RSC (Resource State Coordinator)
 * - Configures MMCX ARC regulator for display power rail
 * - Votes for MM0 BCM (Bus Clock Manager) for display bus clocks
 *
 * Return: CB_SUCCESS on success, CB_ERR on failure
 */
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
		printk(BIOS_ERR, "Failed to initialize display power rail mmcx ARC regulator\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "ARC regulator initialized successfully\n");

	rc = rpmh_regulator_arc_set_level(&arc_reg, RPMH_REGULATOR_LEVEL_MIN_MM0, true, false);
	if (rc) {
		printk(BIOS_ERR, "Failed to set ARC level\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "ARC level was set successfully\n");

	rc = rpmh_bcm_vote("MM0", BCM_MM0_VOTE_VALUE);
	if (rc) {
		printk(BIOS_ERR, "Failed to send BCM vote for display bus clock manager MM0\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "BCM vote for MM0 sent successfully\n");

	return CB_SUCCESS;
}
