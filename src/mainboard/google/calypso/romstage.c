/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <bootmode.h>
#include <cbmem.h>
#include <commonlib/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <soc/aop_common.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>

static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;

static bool platform_get_battery_soc_information(uint32_t *batt_pct)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return false;

	if (google_chromeec_read_batt_state_of_charge(batt_pct))
		return false;

	return true;
}

/*
 * is_off_mode - Check if the system is booting due to an off-mode power event.
 *
 * This function provides the board-level policy wrapper for detecting if the
 * system power-on was triggered by an external charging event (e.g., cable
 * insertion). This is typically used to enter LB_BOOT_MODE_OFFMODE_CHARGING.
 *
 * @return true if the system was triggered by a specific off-mode reason
 * (e.g., charging cable insertion).
 * @return false otherwise.
 */
static bool is_off_mode(void)
{
	/* placeholder */
	return false;
}

static enum boot_mode_t set_boot_mode(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return boot_mode;

	enum boot_mode_t boot_mode_new;

	if (google_chromeec_is_rtc_event()) {
		boot_mode_new = LB_BOOT_MODE_RTC_WAKE;
	} else if (is_off_mode() && google_chromeec_is_battery_present()) {
		boot_mode_new = LB_BOOT_MODE_OFFMODE_CHARGING;
	} else if (google_chromeec_is_below_critical_threshold()) {
		if (google_chromeec_is_charger_present())
			boot_mode_new = LB_BOOT_MODE_LOW_BATTERY_CHARGING;
		else
			boot_mode_new = LB_BOOT_MODE_LOW_BATTERY;
	} else {
		boot_mode_new = LB_BOOT_MODE_NORMAL;
	}

	boot_mode = boot_mode_new;
	return boot_mode_new;
}

static void platform_init_lightbar(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		return;

	/*
	 * Early initialization of the Chrome EC lightbar.
	 * Ensures visual continuity if the AP firmware disabled the lightbar
	 * in a previous boot without a subsequent EC reset.
	 */
	google_chromeec_lightbar_on();

	/*
	 * Only alert the user (set LED to red in color) if the lid is closed and the battery
	 * is critically low without AC power.
	 */
	if (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch() &&
	    google_chromeec_is_critically_low_on_battery())
		google_chromeec_set_lightbar_rgb(0xff, 0xff, 0x00, 0x00);
}

/* Perform romstage early hardware initialization */
static void mainboard_setup_peripherals_early(void)
{
	platform_init_lightbar();
}

/* Perform romstage late hardware initialization */
static void mainboard_setup_peripherals_late(int mode)
{
	/* Placeholder */
}

void platform_romstage_main(void)
{
	mainboard_setup_peripherals_early();

	if (CONFIG(EC_GOOGLE_CHROMEEC) && CONFIG(CONSOLE_SERIAL)) {
		uint32_t batt_pct;
		if (platform_get_battery_soc_information(&batt_pct))
			printk(BIOS_INFO, "Battery state-of-charge %d%%\n", batt_pct);
		else
			printk(BIOS_WARNING, "Failed to get battery level\n");
	}

	if (!qclib_check_dload_mode())
        shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	/* Underlying PMIC registers are accessible only at this point */
	set_boot_mode();

	if (!qclib_check_dload_mode())
		aop_fw_load_reset();

	mainboard_setup_peripherals_late(boot_mode);

	qclib_rerun();
}

void platform_romstage_postram(void)
{
	enum boot_mode_t *boot_mode_ptr = cbmem_add(CBMEM_ID_BOOT_MODE, sizeof(*boot_mode_ptr));
	if (boot_mode_ptr) {
		*boot_mode_ptr = boot_mode;
		printk(BIOS_INFO, "Boot mode is %d\n", *boot_mode_ptr);
	}
}
