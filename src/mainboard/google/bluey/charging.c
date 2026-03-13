/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"

#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <reset.h>
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <timer.h>
#include <types.h>

#define SMB1_SLAVE_ID 0x07
#define SMB2_SLAVE_ID 0x0A
#define SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG 0x2666
#define SMB1_CHGR_MAX_FCC_CFG ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SMB2_CHGR_MAX_FCC_CFG ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SCHG_CHGR_CHARGING_ENABLE_CMD 0x2642
#define SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SNK_CFG 0x2B4A
#define SMB1_CHGR_CHRG_EN_CMD ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)
#define SMB2_CHGR_CHRG_EN_CMD ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)
#define SMB1_SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SNK_CFG \
((SMB1_SLAVE_ID << 16) | SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SNK_CFG)
#define SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SRC_CFG 0x2B4C
#define SMB1_SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SRC_CFG \
((SMB1_SLAVE_ID << 16) | SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SRC_CFG)
#define SCHG_CHGR_CHARGING_FCC 0x260A
#define SMB1_CHGR_CHARGING_FCC ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_FCC)
#define SMB2_CHGR_CHARGING_FCC ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_FCC)

#define FCC_1A_STEP_50MA 0x14
#define FCC_DISABLE 0x8c
#define EN_DEBUG_ACCESS_SNK 0x1B
#define EN_DEBUG_ACCESS_SRC 0x01

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

#define DELAY_CHARGING_APPLET_MS 2000 /* 2sec */
#define CHARGING_RAIL_STABILIZATION_DELAY_MS 3000 /* 3sec */
#define DELAY_CHARGING_ACTIVE_LB_MS 4000 /* 4sec */

enum charging_status {
	CHRG_DISABLE,
	CHRG_ENABLE,
};

static int get_battery_icurr_ma(void)
{
	/* Read battery i-current value */
	int icurr = spmi_read8(SMB1_CHGR_CHARGING_FCC);
	if (icurr <= 0)
		icurr = spmi_read8(SMB2_CHGR_CHARGING_FCC);
	if (icurr < 0)
		icurr = 0;

	icurr *= 50;
	return icurr;
}

static void clear_ec_manual_poweron_event(void)
{
	const uint64_t manual_pwron_event_mask =
		(EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN));
	google_chromeec_clear_events_b(manual_pwron_event_mask);
}

static int detect_ec_manual_poweron_event(void)
{
	const uint64_t manual_pwron_event_mask =
		(EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN));
	uint64_t events = google_chromeec_get_events_b();

	if (!!(events & manual_pwron_event_mask))
		return 1;

	return 0;
}

static void clear_ac_unplug_event(void)
{
	const uint64_t ac_unplug_event =
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED);
	google_chromeec_clear_events_b(ac_unplug_event);
}

static int detect_ac_unplug_event(void)
{
	const uint64_t ac_unplug_event_mask =
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_DISCONNECTED);
	uint64_t events = google_chromeec_get_events_b();

	if (!!(events & ac_unplug_event_mask))
		return 1;

	return 0;
}

/*
 * Provides visual feedback via the LEDs and clears the AC unplug
 * event to acknowledge the transition into a charging state.
 */
static void indicate_charging_status(void)
{
	/* Turn on LEDs to alert user of power state change */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL)) {
		google_chromeec_lightbar_on();
		mdelay(DELAY_CHARGING_ACTIVE_LB_MS);
	}

	/* Clear the event to prevent re-triggering in the next iteration */
	clear_ac_unplug_event();
}

void launch_charger_applet(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	static const long charging_enable_timeout_ms = CHARGING_RAIL_STABILIZATION_DELAY_MS;
	struct stopwatch sw;

	printk(BIOS_INFO, "Inside %s. Initiating charging\n", __func__);

	/* Reset AC-unplug detection state and lightbar status before entering loop */
	clear_ac_unplug_event();
	/* clear any pending power button press and lid open event */
	clear_ec_manual_poweron_event();

	stopwatch_init_msecs_expire(&sw, charging_enable_timeout_ms);
	while (!get_battery_icurr_ma()) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_WARNING, "Charging not enabled %ld ms. Abort.\n",
					charging_enable_timeout_ms);
			/*
			 * If firmware-based charging fails to enable within the timeout,
			 * do not simply return, as this leaves IPs uninitialized and
			 * causes a boot hang. Instead, issue a shutdown if not charging.
			 */
			printk(BIOS_INFO, "Issuing power-off.\n");
			if (detect_ac_unplug_event())
				indicate_charging_status();
			google_chromeec_offmode_heartbeat();
			google_chromeec_ap_poweroff();
		}
		mdelay(200);
	}
	printk(BIOS_INFO, "Charging ready after %lld ms\n", stopwatch_duration_msecs(&sw));

	do {
		/* Add static delay before reading the charging applet pre-requisites */
		mdelay(DELAY_CHARGING_APPLET_MS);

		/*
		 * Issue a shutdown if not charging.
		 */
		if (!get_battery_icurr_ma()) {
			printk(BIOS_INFO, "Issuing power-off due to change in charging state.\n");
			if (detect_ac_unplug_event())
				indicate_charging_status();
			google_chromeec_offmode_heartbeat();
			google_chromeec_ap_poweroff();
		}

		/*
		 * Exit the charging loop in the event of lid open or power
		 * button press.
		 *
		 * Reset the device to ensure a fresh boot to OS.
		 * This is required to avoid any kind of tear-down due to ADSP-lite
		 * being loaded and need some clean up before loading ADSP firmware by
		 * linux kernel.
		 */
		if (detect_ec_manual_poweron_event()) {
			printk(BIOS_INFO, "Exiting charging applet to boot to OS\n");
			do_board_reset();
		}

		/* TODO: add Tsense support and issue a shutdown in the event of temperature trip */
	} while (true);
}

/*
 * Enable PMC8380F GPIO07 & GPIO09 for parallel charging.
 */
void configure_parallel_charging(void)
{
	if (!CONFIG(MAINBOARD_SUPPORTS_PARALLEL_CHARGING))
		return;

	printk(BIOS_INFO, "Configure parallel charging support\n");
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO07_DIG_OUT_SOURCE_CTL), OUTPUT_INVERT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO07_EN_CTL), PERPH_EN);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO07_MODE_CTL), MODE_OUTPUT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO09_DIG_OUT_SOURCE_CTL), OUTPUT_INVERT);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO09_EN_CTL), PERPH_EN);
	spmi_write8(SPMI_ADDR(PMC8380F_SLAVE_ID, GPIO09_MODE_CTL), MODE_OUTPUT);
}

/*
 * Configure debug access port to support source and sink modes.
 */
void configure_debug_access_port(void)
{
	if (!CONFIG(HAVE_DEBUG_ACCESS_PORT_SOURCE_SINK))
		return;

	printk(BIOS_INFO, "Enable support of source and sink modes for debug access port\n");
	spmi_write8(SMB1_SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SRC_CFG, EN_DEBUG_ACCESS_SRC);
	spmi_write8(SMB1_SCHG_TYPE_C_TYPE_C_DEBUG_ACCESS_SNK_CFG, EN_DEBUG_ACCESS_SNK);
}

/*
 * Late configuration for parallel charging.
 */
void configure_parallel_charging_late(void)
{
	if (!CONFIG(MAINBOARD_SUPPORTS_PARALLEL_CHARGING))
		return;

	gpio_output(GPIO_PARALLEL_CHARGING_CFG, 1);
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
