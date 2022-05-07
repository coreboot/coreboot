/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <types.h>
#include <timer.h>

#include "raminit_native.h"

static void assert_reset(const bool do_reset)
{
	if (is_hsw_ult()) {
		uint32_t pm_cfg2 = RCBA32(PM_CFG2);
		if (do_reset)
			pm_cfg2 &= ~PM_CFG2_DRAM_RESET_CTL;
		else
			pm_cfg2 |= PM_CFG2_DRAM_RESET_CTL;
		RCBA32(PM_CFG2) = pm_cfg2;
	} else {
		union mc_init_state_g_reg mc_init_state_g = {
			.raw = mchbar_read32(MC_INIT_STATE_G),
		};
		mc_init_state_g.ddr_not_reset = !do_reset;
		mchbar_write32(MC_INIT_STATE_G, mc_init_state_g.raw);
	}
}

/*
 * Perform JEDEC reset.
 *
 * If RTT_NOM is to be enabled in MR1, the ODT input signal must be
 * statically held low in our system since RTT_NOM is always enabled.
 */
static void jedec_reset(struct sysinfo *ctrl)
{
	if (is_hsw_ult())
		assert_reset(false);

	union mc_init_state_g_reg mc_init_state_g = {
		.ddr_not_reset     = 1,
		.safe_self_refresh = 1,
	};
	mchbar_write32(MC_INIT_STATE_G, mc_init_state_g.raw);

	union reut_misc_cke_ctrl_reg reut_misc_cke_ctrl = {
		.cke_override = 0xf,
		.cke_on       = 0,
	};
	mchbar_write32(REUT_MISC_CKE_CTRL, reut_misc_cke_ctrl.raw);

	assert_reset(true);

	/** TODO: check and switch DDR3 voltage here (mainboard-specific) **/

	udelay(200);

	assert_reset(false);

	udelay(500);

	mc_init_state_g.dclk_enable = 1;
	mchbar_write32(MC_INIT_STATE_G, mc_init_state_g.raw);

	/* Delay at least 20 nanoseconds for tCKSRX */
	tick_delay(1);

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		reut_misc_cke_ctrl.cke_on = ctrl->rankmap[channel];
		mchbar_write32(REUT_ch_MISC_CKE_CTRL(channel), reut_misc_cke_ctrl.raw);
	}

	/*
	 * Wait minimum of reset CKE exit time, tXPR.
	 * Spec says MAX(tXS, 5 tCK). 5 tCK is 10 ns.
	 */
	tick_delay(1);
}

enum raminit_status do_jedec_init(struct sysinfo *ctrl)
{
	/* Never do a JEDEC reset in S3 resume */
	if (ctrl->bootmode == BOOTMODE_S3)
		return RAMINIT_STATUS_SUCCESS;

	enum raminit_status status = io_reset();
	if (status)
		return status;

	status = wait_for_first_rcomp();
	if (status)
		return status;

	/* Force ODT low (JEDEC spec) */
	const union reut_misc_odt_ctrl_reg reut_misc_odt_ctrl = {
		.odt_override = 0xf,
		.odt_on       = 0,
	};
	mchbar_write32(REUT_MISC_ODT_CTRL, reut_misc_odt_ctrl.raw);

	/*
	 * Note: Haswell MRC does not clear ODT override for LPDDR3. However,
	 * Broadwell MRC does. Hell suspects this difference is important, as
	 * there is an erratum in the specification update for Broadwell:
	 *
	 * Erratum BDM74: LPDDR3 Memory Training May Cause Platform Boot Failure
	 */
	if (ctrl->lpddr)
		die("%s: LPDDR-specific JEDEC init not implemented\n", __func__);

	jedec_reset(ctrl);
	status = ddr3_jedec_init(ctrl);
	if (!status)
		ctrl->restore_mrs = true;

	/* Release ODT override */
	mchbar_write32(REUT_MISC_ODT_CTRL, 0);
	return status;
}
