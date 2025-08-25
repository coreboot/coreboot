/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include "board.h"
#include <gpio.h>
#include <soc/aop_common.h>
#include <soc/qclib_common.h>
#include <soc/qcom_spmi.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>

#define SMB1_SLAVE_ID 0x07
#define SMB2_SLAVE_ID 0x0A
#define SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG 0x2666
#define SMB1_CHGR_MAX_FCC_CFG ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SMB2_CHGR_MAX_FCC_CFG ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SCHG_CHGR_CHARGING_ENABLE_CMD 0x2642
#define SMB1_CHGR_CHRG_EN_CMD ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)
#define SMB2_CHGR_CHRG_EN_CMD ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)

#define FCC_1A_STEP_50MA 0x14
#define CHRG_ENABLE 0x01

static void enable_battery_charging(void)
{
	/* Configure FCC and enable charging */
	printk(BIOS_INFO, "Use slow charging without fast charge support\n");
	spmi_write8(SMB1_CHGR_MAX_FCC_CFG, FCC_1A_STEP_50MA);
	spmi_write8(SMB2_CHGR_MAX_FCC_CFG, FCC_1A_STEP_50MA);
	spmi_write8(SMB1_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
	spmi_write8(SMB2_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
}

void platform_romstage_main(void)
{
	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();

	shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	aop_fw_load_reset();

	qclib_rerun();

	enable_battery_charging();

	/*
	 * Enable this power rail now for FPMCU stability prior to
	 * its reset being deasserted in ramstage. This applies
	 * when MAINBOARD_HAS_FINGERPRINT_VIA_SPI Kconfig is enabled.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		gpio_output(GPIO_EN_FP_RAILS, 1);
}
