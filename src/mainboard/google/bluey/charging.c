/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <types.h>

#define SMB1_SLAVE_ID 0x07
#define SMB2_SLAVE_ID 0x0A
#define SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG 0x2666
#define SMB1_CHGR_MAX_FCC_CFG ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SMB2_CHGR_MAX_FCC_CFG ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SCHG_CHGR_CHARGING_ENABLE_CMD 0x2642
#define SMB1_CHGR_CHRG_EN_CMD ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)
#define SMB2_CHGR_CHRG_EN_CMD ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)

#define FCC_1A_STEP_50MA 0x14
#define FCC_DISABLE 0x8c

enum charging_status {
	CHRG_DISABLE,
	CHRG_ENABLE,
};

#define PMC8380F_SLAVE_ID	0x05
#define GPIO07_MODE_CTL			0x8E40
#define GPIO07_DIG_OUT_SOURCE_CTL	0x8E44
#define GPIO07_EN_CTL			0x8E46
#define GPIO09_MODE_CTL			0x9040
#define GPIO09_DIG_OUT_SOURCE_CTL	0x9044
#define GPIO09_EN_CTL			0x9046

#define MODE_OUTPUT		0x01
#define OUTPUT_INVERT		0x80
#define PERPH_EN		0x80

/*
 * Enable PMC8380F GPIO07 & GPIO09 for parallel charging.
 */
void configure_parallel_charging(void)
{
	if (!CONFIG(MAINBOARD_SUPPORTS_PARALLEL_CHARGING))
		return;

	printk(BIOS_INFO, "Configure parallel charging support\n");
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO07_MODE_CTL), MODE_OUTPUT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO07_DIG_OUT_SOURCE_CTL), OUTPUT_INVERT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO07_EN_CTL), PERPH_EN);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO09_MODE_CTL), MODE_OUTPUT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO09_DIG_OUT_SOURCE_CTL), OUTPUT_INVERT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO09_EN_CTL), PERPH_EN);
}

/*
 * Enable charging w/ 1A Icurrent supply at max.
 */
void enable_slow_battery_charging(void)
{
	/* Configure FCC and enable charging */
	printk(BIOS_INFO, "Use slow charging without fast charge support\n");
	spmi_write8(SMB1_CHGR_MAX_FCC_CFG, FCC_1A_STEP_50MA);
	spmi_write8(SMB2_CHGR_MAX_FCC_CFG, FCC_1A_STEP_50MA);
	spmi_write8(SMB1_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
	spmi_write8(SMB2_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
}

/*
 * Disable charging.
 */
void disable_slow_battery_charging(void)
{
	printk(BIOS_INFO, "Disable slow charge support\n");
	spmi_write8(SMB1_CHGR_CHRG_EN_CMD, CHRG_DISABLE);
	spmi_write8(SMB2_CHGR_CHRG_EN_CMD, CHRG_DISABLE);
	spmi_write8(SMB1_CHGR_MAX_FCC_CFG, FCC_DISABLE);
	spmi_write8(SMB2_CHGR_MAX_FCC_CFG, FCC_DISABLE);
}
