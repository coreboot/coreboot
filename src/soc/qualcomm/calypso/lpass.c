/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/cmd_db.h>
#include <soc/lpass.h>
#include <soc/rpmh_bcm.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>
#include <timer.h>

static enum cb_err lpass_setup_core_infrastructure(void)
{
	if (clock_enable(&gcc->gcc_lpass_cfg_noc_sway_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable CFG NOC SWAY clock\n");
		return CB_ERR;
	}

	clock_configure_hw_ctl(&lpass_core_gdsc->lpass_top_cc_lpass_core_sway_ahb_ls_cbcr, true);

	write32(&lpass_aon_cc->lpass_hm_collapse_vote_for_q6, LPASS_CORE_HM_VOTE_POWER_ON);

	if (!wait_us(GDSC_PWR_ON_DELAY, !(read32(&lpass_core_gdsc->core_hm_gdscr) & GDSC_PWR_ON))) {
		printk(BIOS_ERR, "LPASS: Core HM GDSC PWR_ON timeout after vote\n");
		return CB_ERR;
	}
	if (!wait_us(LPASS_CORE_HM_READY_DELAY, read32(&lpass_core_gdsc->lpass_core_gds_hm_ready) & LPASS_CORE_HM_READY)) {
		printk(BIOS_ERR, "LPASS: Core HM ready timeout\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

/* Array mapping enum clk_lpass to actual CBCR registers */
static u32 *lpass_cbcr[LPASS_CLK_COUNT] = {
	[LPASS_CODEC_MEM_CBCR] = &lpass_audio_cc->codec_mem_cbcr,
	[LPASS_CODEC_MEM0_CBCR] = &lpass_audio_cc->codec_mem0_cbcr,
	[LPASS_CODEC_MEM1_CBCR] = &lpass_audio_cc->codec_mem1_cbcr,
	[LPASS_CODEC_MEM2_CBCR] = &lpass_audio_cc->codec_mem2_cbcr,
	[LPASS_CODEC_MEM3_CBCR] = &lpass_audio_cc->codec_mem3_cbcr,
	[LPASS_EXT_MCLK0_CBCR] = &lpass_audio_cc->codec_ext_mclk0_cbcr,
	[LPASS_EXT_MCLK1_CBCR] = &lpass_audio_cc->codec_ext_mclk1_cbcr,
	[LPASS_TX_MCLK_CBCR] = &lpass_aon_cc->tx_mclk_cbcr,
	[LPASS_TX_MCLK_2X_WSA_CBCR] = &lpass_audio_cc->tx_mclk_2x_wsa_cbcr,
	[LPASS_TX_MCLK_WSA_CBCR] = &lpass_audio_cc->tx_mclk_wsa_cbcr,
	[LPASS_WSA_MCLK_2X_CBCR] = &lpass_audio_cc->wsa_mclk_2x_cbcr,
	[LPASS_WSA_MCLK_CBCR] = &lpass_audio_cc->wsa_mclk_cbcr,
	[LPASS_TX_MCLK_2X_WSA2_CBCR] = &lpass_audio_cc->tx_mclk_2x_wsa2_cbcr,
	[LPASS_TX_MCLK_WSA2_CBCR] = &lpass_audio_cc->tx_mclk_wsa2_cbcr,
	[LPASS_WSA2_MCLK_2X_CBCR] = &lpass_audio_cc->wsa2_mclk_2x_cbcr,
	[LPASS_WSA2_MCLK_CBCR] = &lpass_audio_cc->wsa2_mclk_cbcr,
};

static enum cb_err lpass_audio_clocks_enable(void)
{
	for (size_t i = 0; i < LPASS_CLK_COUNT; i++) {
		if (clock_enable(lpass_cbcr[i]) != CB_SUCCESS) {
			printk(BIOS_ERR, "LPASS: Failed to enable audio clock\n");
			return CB_ERR;
		}
	}

	return CB_SUCCESS;
}

static enum cb_err lpass_rpmh_bcm_vote(void)
{
	enum cb_err ret;
	int rc;
	volatile u32 *boot_cookie = (volatile u32 *)AOP_BOOT_COOKIE_ADDR;

	if (!wait_us(AOP_BOOT_TIMEOUT_US, *boot_cookie == AOP_BOOT_COOKIE)) {
		printk(BIOS_ERR,
		       "AOP not booted after, addr[%p], %dus (cookie: 0x%x, expected: 0x%x)\n",
		       (u32 *)AOP_BOOT_COOKIE_ADDR, AOP_BOOT_TIMEOUT_US, *boot_cookie, AOP_BOOT_COOKIE);
		return CB_ERR;
	}

	ret = cmd_db_ready();
	if (ret != CB_SUCCESS) {
		ret = cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE);
		if (ret != CB_SUCCESS) {
			printk(BIOS_ERR, "CMD_DB: init failed\n");
			return CB_ERR;
		}
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "RPMH_RSC: init failed\n");
		return CB_ERR;
	}

	rc = rpmh_bcm_vote("LP0", BCM_LP0_VOTE_VALUE);
	if (rc) {
		printk(BIOS_ERR, "LPASS: Failed to send BCM vote for LPASS bus clock manager LP0\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}


enum cb_err lpass_init(void)
{
	if (lpass_rpmh_bcm_vote() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to initialize RPMH BCM vote\n");
		return CB_ERR;
	}

	if (enable_and_poll_gdsc_status(&lpass_aon_cc->lpass_audio_hm_gdscr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable Core HM GDSC\n");
		return CB_ERR;
	}

	if (lpass_setup_core_infrastructure() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to setup core infrastructure\n");
		return CB_ERR;
	}

	if (lpass_audio_clocks_enable() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable audio clocks\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "LPASS: BCM vote for LP0 and LPASS Init completed successfully\n");
	return CB_SUCCESS;
}

enum cb_err lpass_bring_up(void)
{
	if (lpass_init() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: lpass_init() failed\n");
		return CB_ERR;
	}
	return CB_SUCCESS;
}
