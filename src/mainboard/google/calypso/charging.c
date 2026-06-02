/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"

#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <reset.h>
#include <soc/pmic_gpio.h>
#include <soc/qcom_spmi.h>
#include <timer.h>
#include <types.h>

#define SMB1_SLAVE_ID 0x09
#define SMB2_SLAVE_ID 0x0A
#define SMB3_SLAVE_ID 0x0B

#define SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG 0x2666
#define SMB1_CHGR_MAX_FCC_CFG ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SMB2_CHGR_MAX_FCC_CFG ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)
#define SMB3_CHGR_MAX_FCC_CFG ((SMB3_SLAVE_ID << 16) | SCHG_CHGR_MAX_FAST_CHARGE_CURRENT_CFG)

#define SCHG_CHGR_CHARGING_ENABLE_CMD 0x2642
#define SMB1_CHGR_CHRG_EN_CMD ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)
#define SMB2_CHGR_CHRG_EN_CMD ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)
#define SMB3_CHGR_CHRG_EN_CMD ((SMB3_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_ENABLE_CMD)

#define SCHG_CHGR_CHARGING_FCC 0x260A
#define SMB1_CHGR_CHARGING_FCC ((SMB1_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_FCC)
#define SMB2_CHGR_CHARGING_FCC ((SMB2_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_FCC)
#define SMB3_CHGR_CHARGING_FCC ((SMB3_SLAVE_ID << 16) | SCHG_CHGR_CHARGING_FCC)

#define FCC_3A_STEP_50MA 0x3C
#define FCC_DISABLE 0x8c

/*
 * SDAM15_MEM_061 (SPMI address 0x7E7D) - SetMaxPwrReq_BattSts register
 * Bit 6 - DEAD_BATT_STS
 */
#define SDAM15_MEM_061_ADDR	0x7E7D
#define DEAD_BATT_STS	BIT(6)

#define DELAY_CHARGING_APPLET_MS 2000 /* 2sec */
#define CHARGING_RAIL_STABILIZATION_DELAY_MS 5000 /* 5sec */
#define DEAD_BATTERY_CHARGING_LOOP_EXIT_MS (10 * 60 * 1000) /* 10min */
#define DELAY_CHARGING_ACTIVE_LB_MS 4000 /* 4sec */
#define SMB_FCC_MULTIPLIER_MA 50
#define SMB_READ_DELAY_MS 5

enum charging_status {
	CHRG_DISABLE,
	CHRG_ENABLE,
};

static int get_battery_icurr_ma(void)
{
	const uint32_t smb_regs[] = {
		SMB1_CHGR_CHARGING_FCC,
		SMB2_CHGR_CHARGING_FCC,
		SMB3_CHGR_CHARGING_FCC,
	};

	int icurr = 0;
	for (size_t i = 0; i < ARRAY_SIZE(smb_regs); i++) {
		mdelay(SMB_READ_DELAY_MS);
		icurr = spmi_read8_safe(smb_regs[i]);

		if (icurr > 0)
			return icurr * SMB_FCC_MULTIPLIER_MA;
	}

	printk(BIOS_ERR, "Critical: All SMB registers failed to read.\n");
	return 0;
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
	bool has_entered_dead_battery_mode = false;

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

	static const long dead_battery_charging_timeout_ms = DEAD_BATTERY_CHARGING_LOOP_EXIT_MS;
	uint32_t capacity;
	if (google_chromeec_read_batt_remaining_capacity(&capacity) < 0) {
		printk(BIOS_WARNING, "Failed to get battery capacity\n");
		return;
	}
	/*
	 * If the remaining battery is less than
	 * DEAD_BATT_CHG_THRESHOLD_MAH threshold, enter low-battery
	 * charging mode and start a timeout timer to come out from dead battery charging
	 * mode.
	 */
	if (capacity <= DEAD_BATT_CHG_THRESHOLD_MAH) {
		has_entered_dead_battery_mode = true;
		stopwatch_init_msecs_expire(&sw, dead_battery_charging_timeout_ms);
	}

	do {
		/* Add static delay before reading the charging applet pre-requisites */
		mdelay(DELAY_CHARGING_APPLET_MS);

		if (has_entered_dead_battery_mode) {
			if (stopwatch_expired(&sw)) {
				printk(BIOS_INFO, "Issuing power-off to come out from"
						" dead battery charging mode.\n");
				google_chromeec_ap_poweroff();
			}
		}

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
	} while (true);
}

/*
 * Enable charging w/ 1A Icurrent supply at max.
 */
void enable_slow_battery_charging(void)
{
	/* Configure FCC and enable charging */
	printk(BIOS_INFO, "Use slow charging without fast charge support\n");
	spmi_write8(SMB1_CHGR_MAX_FCC_CFG, FCC_3A_STEP_50MA);
	spmi_write8(SMB2_CHGR_MAX_FCC_CFG, FCC_3A_STEP_50MA);
	spmi_write8(SMB3_CHGR_MAX_FCC_CFG, FCC_3A_STEP_50MA);
	spmi_write8(SMB1_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
	spmi_write8(SMB2_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
	spmi_write8(SMB3_CHGR_CHRG_EN_CMD, CHRG_ENABLE);
}

/*
 * Disable charging.
 */
void disable_slow_battery_charging(void)
{
	printk(BIOS_INFO, "Disable slow charge support\n");
	spmi_write8(SMB1_CHGR_CHRG_EN_CMD, CHRG_DISABLE);
	spmi_write8(SMB2_CHGR_CHRG_EN_CMD, CHRG_DISABLE);
	spmi_write8(SMB3_CHGR_CHRG_EN_CMD, CHRG_DISABLE);
	spmi_write8(SMB1_CHGR_MAX_FCC_CFG, FCC_DISABLE);
	spmi_write8(SMB2_CHGR_MAX_FCC_CFG, FCC_DISABLE);
	spmi_write8(SMB3_CHGR_MAX_FCC_CFG, FCC_DISABLE);
}

/*
 * Enable fast battery charging with ADSP support.
 *
 * This function loads SoCCP firmware and configures fast charging.
 */
void enable_fast_battery_charging(void)
{
	/* TODO */
}

bool platform_get_battery_soc_information(uint32_t *batt_pct)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return false;

	if (google_chromeec_read_batt_state_of_charge(batt_pct))
		return false;

	return true;
}

void configure_dead_battery_boot(void)
{
	spmi_rmw8(SDAM15_MEM_061_ADDR, DEAD_BATT_STS, DEAD_BATT_STS);
}
