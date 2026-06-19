/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"

#include <arch/stages.h>
#include <bootmode.h>
#include <cbmem.h>
#include <commonlib/coreboot_tables.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <reset.h>
#include <soc/aop_common.h>
#include <soc/pcie.h>
#include <soc/qcom_spmi.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>

#define DELAY_FOR_SHIP_MODE 11000 /* 11sec */
static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;
static bool battery_present = true;
static bool battery_below_threshold = false;
static int32_t battery_cfet_status = 1; /* Active C-FET */
static int32_t battery_dfet_status = 1; /* Active D-FET */
static bool battery_is_cutoff = false;
static bool battery_needs_recovery = false;
static bool chipset_dload_mode_active = false; /* Mode for crashlog */

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

	if (!battery_present) {
		boot_mode_new = LB_BOOT_MODE_NO_BATTERY;
	} else if (google_chromeec_is_rtc_event()) {
		boot_mode_new = LB_BOOT_MODE_RTC_WAKE;
	} else if (is_off_mode()) {
		boot_mode_new = LB_BOOT_MODE_OFFMODE_CHARGING;
	} else if (battery_below_threshold) {
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
}
/*
 * Update and cache battery status from the EC.
 * This should be called once, early in the boot process,
 * after the EC is reachable.
 */
static void update_battery_status(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	/*
	 * Force a board reset if the EC reports invalid battery data and crashlog mode
	 * is not set to prevent downstream configuration issues with bad telemetry.
	 */
	if (!google_chromeec_is_battery_data_valid() && !chipset_dload_mode_active) {
		printk(BIOS_INFO, "Battery data invalid! doing board reset.\n");
		do_board_reset();
	}

	struct ec_response_battery_get_misc_info misc_info;
	bool misc_info_valid = false;

	battery_present = google_chromeec_is_battery_present();
	battery_below_threshold = google_chromeec_is_below_critical_threshold();

	if (battery_present && (google_chromeec_get_battery_misc_info(&misc_info) == 0)) {
		battery_cfet_status = misc_info.cfet_status;
		battery_dfet_status = misc_info.dfet_status;
		misc_info_valid = true;
	} else {
		printk(BIOS_WARNING, "Failed to get battery FET status from EC\n");
		battery_cfet_status = -1;
		battery_dfet_status = -1;
	}

	/*
	 * SHIP MODE RECOVERY HANDLER:
	 * Triggered ONLY when the battery info was successfully read,
	 * and BOTH FETs are explicitly 0 (indicating a locked BMS).
	 */
	battery_needs_recovery = misc_info_valid && (battery_cfet_status == 0)
			 && (battery_dfet_status == 0);

	/*
	 * BATTERY CUTOFF / DISCONNECT DETECTION:
	 * Triggered when the hardware reports no battery present AND
	 * the EC FET status read failed (returning -1).
	 */
	battery_is_cutoff = (battery_cfet_status == -1) && (battery_dfet_status == -1);

}

/* Perform romstage early hardware initialization */
static void mainboard_setup_peripherals_early(void)
{
	platform_init_lightbar();

	update_battery_status();

	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();
}

/* Perform romstage late hardware initialization */
static void mainboard_setup_peripherals_late(int mode)
{
	/*
	 * Power on NVMe early so that the DDR init and other operations
	 * that follow provide an organic >50ms delay before PCIe PERST
	 * de-assertion in platform_romstage_postram(), satisfying the
	 * NVMe spec requirement without a static mdelay().
	 */
	gcom_pcie_power_on_ep();

	if (!chipset_dload_mode_active) {
		/* Perform PCIe setup early in async mode if supported to save 100ms */
		if (mode == LB_BOOT_MODE_NORMAL || mode == LB_BOOT_MODE_NO_BATTERY)
			qcom_setup_pcie_host(NULL);
		else
			gcom_pcie_power_off_ep();
	}

	/*
	 * Enable fingerprint power rail early for stability prior to
	 * its reset being deasserted in ramstage.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT)) {
		if (mode == LB_BOOT_MODE_NORMAL || mode == LB_BOOT_MODE_NO_BATTERY)
			gpio_output(GPIO_EN_FP_RAILS, 1);
	}
}

static void handle_battery_shipping_recovery(bool board_reset)
{
	printk(BIOS_INFO, "==================================================\n");
	printk(BIOS_INFO, "Device has entered into shipping recovery mode.\n");
	printk(BIOS_INFO, "Please wait ...\n");
	printk(BIOS_INFO, "==================================================\n");

	enable_slow_battery_charging();
	mdelay(DELAY_FOR_SHIP_MODE);

	if (board_reset) {
		printk(BIOS_INFO, "Issuing board reset\n");
		do_board_reset();
	}

	/* Disable charging where `board_reset` is not allowed */
	disable_slow_battery_charging();

}

static void check_first_boot_and_reset(enum boot_mode_t mode)
{
	if ((mode != LB_BOOT_MODE_NORMAL) && (boot_count_read() == 1)) {
		printk(BIOS_INFO, "First boot detected in non-normal mode; triggering reset.\n");
		do_board_reset();
	}
}

void platform_romstage_main(void)
{
	mainboard_setup_peripherals_early();
	chipset_dload_mode_active = qclib_check_dload_mode();

	if (CONFIG(EC_GOOGLE_CHROMEEC) && CONFIG(CONSOLE_SERIAL)) {
		uint32_t batt_pct;
		if (platform_get_battery_soc_information(&batt_pct))
			printk(BIOS_INFO, "Battery state-of-charge %d%%\n", batt_pct);
		else
			printk(BIOS_WARNING, "Failed to get battery level\n");
	}

	if (!chipset_dload_mode_active)
		shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	/* Recovery from battery shipping mode */
	if (battery_needs_recovery || battery_is_cutoff)
		handle_battery_shipping_recovery(battery_needs_recovery);

	/* Underlying PMIC registers are accessible only at this point */
	set_boot_mode();

	if (!chipset_dload_mode_active)
		aop_fw_load_reset();

	mainboard_setup_peripherals_late(boot_mode);

	qclib_rerun();

	check_first_boot_and_reset(boot_mode);
}

void platform_romstage_postram(void)
{
	enum boot_mode_t *boot_mode_ptr = cbmem_add(CBMEM_ID_BOOT_MODE, sizeof(*boot_mode_ptr));
	if (boot_mode_ptr) {
		*boot_mode_ptr = boot_mode;
		printk(BIOS_INFO, "Boot mode is %d\n", *boot_mode_ptr);
	}
}
