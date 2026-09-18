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
#include <security/vboot/vboot_common.h>
#include <soc/aop_common.h>
#include <soc/pcie.h>
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <soc/qclib_common.h>
#include <soc/qupv3_config_common.h>
#include <soc/qupv3_i2c_common.h>
#include <soc/qupv3_spi_common.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>
#include <timer.h>

#define DELAY_FOR_SHIP_MODE 11000 /* 11sec */
/* Up to 300 seconds for ship/trickle recovery */
#define DELAY_FOR_BATT_RECOVERY_MODE (300 * 1000)
#define DELAY_FOR_BATT_AT_FULL_ON_MODE (60 * 1000)
#define BATT_RECOVERY_MODE_POLL_INTERVAL_MS 500 /* Poll every 500 ms */

static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;
static bool battery_present = true;
static bool battery_below_threshold = false;
static int32_t battery_cfet_status = 1; /* Active C-FET */
static int32_t battery_dfet_status = 1; /* Active D-FET */
static bool battery_is_cutoff = false;
static bool battery_needs_recovery = false;
static bool chipset_dload_mode_active = false; /* Mode for crashlog */
static bool below_trickle_battery_voltage = false;

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
	return is_pon_on_ac();
}

static bool ap_running_rw(void)
{
	if (!CONFIG(VBOOT))
		return false;

	/* In coreboot, all non-recovery boot runs from RW */
	return !vboot_recovery_mode_enabled();
}

static enum boot_mode_t init_boot_mode(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return boot_mode;

	enum boot_mode_t boot_mode_new;

	if (!battery_present) {
		boot_mode_new = LB_BOOT_MODE_NO_BATTERY;
	} else if (google_chromeec_is_rtc_event()) {
		boot_mode_new = LB_BOOT_MODE_RTC_WAKE;
	} else if (battery_below_threshold) {
		if (google_chromeec_is_charger_present())
			boot_mode_new = LB_BOOT_MODE_LOW_BATTERY_CHARGING;
		else
			boot_mode_new = LB_BOOT_MODE_LOW_BATTERY;
	} else if (is_off_mode() && ap_running_rw() && !google_ec_running_ro()) {
		boot_mode_new = LB_BOOT_MODE_OFFMODE_CHARGING;
	} else {
		boot_mode_new = LB_BOOT_MODE_NORMAL;
	}

	return boot_mode_new;
}

/*
 * Check if the current battery voltage is at or below the trickle-charge
 * threshold (CONFIG_BATTERY_TRICKLE_VOLTAGE_MV).
 *
 * Return: true if battery voltage <= threshold (or if EC read fails/BMS asleep),
 *         false if battery voltage is above the trickle threshold.
 */
static bool is_battery_below_trickle_threshold(void)
{
	uint32_t battery_voltage = 0;
	/*
	 * If EC fails to read battery voltage (e.g., BMS is unpowered in ship mode),
	 * treat it as being in the trickle-charge region.
	 */
	if (google_chromeec_read_batt_voltage(&battery_voltage) != 0) {
		printk(BIOS_WARNING, "Failed to read battery voltage; assuming trickle state\n");
		return true;
	}

	printk(BIOS_DEBUG, "Battery voltage: %u mV (trickle threshold: %d mV)\n",
	       battery_voltage, CONFIG_BATTERY_TRICKLE_VOLTAGE_MV);

	return (battery_voltage <= CONFIG_BATTERY_TRICKLE_VOLTAGE_MV);
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

	/*
	 * TRICKLE BATTERY MODE DETECTION:
	 * Determine if the battery is deeply depleted or has an unpowered BMS
	 * (voltage <= CONFIG_BATTERY_TRICKLE_VOLTAGE_MV) requiring trickle recovery.
	 */
	below_trickle_battery_voltage = is_battery_below_trickle_threshold();
}

/* Perform romstage early hardware initialization */
static void mainboard_setup_peripherals_early(void)
{
	configure_gpios_defaults();

	update_battery_status();

	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();
}

/* Perform romstage late hardware initialization */
static void mainboard_setup_peripherals_late(int mode)
{
	bool nvme_present = mainboard_nvme_present();
	/*
	 * Power on NVMe early so that the DDR init and other operations
	 * that follow provide an organic >50ms delay before PCIe PERST
	 * de-assertion in platform_romstage_postram(), satisfying the
	 * NVMe spec requirement without a static mdelay().
	 */
	if (nvme_present)
		gcom_pcie_power_on_ep();

	if (!chipset_dload_mode_active && nvme_present) {
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

/*
 * Poll until the battery transitions from trickle/pre-charge to fast-charge mode,
 * or until the timeout expires.
 */
static void wait_for_fast_charge_ready(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC) || !google_chromeec_is_charger_present())
		return;

	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, DELAY_FOR_BATT_RECOVERY_MODE);

	/* Poll until the battery exits trickle/pre-charge or timeout occurs */
	while (!stopwatch_expired(&sw)) {
		if (is_fast_charge_ready()) {
			printk(BIOS_INFO, "\nBattery recovered to fast-charge stage after %lld ms\n",
			       stopwatch_duration_msecs(&sw));
			/* Waiting before existing battery recovery mode */
			mdelay(DELAY_FOR_BATT_AT_FULL_ON_MODE);
			return;
		}
		mdelay(BATT_RECOVERY_MODE_POLL_INTERVAL_MS);
		/* Print heartbeat to keep user informed */
		printk(BIOS_INFO, ".");
	}

	printk(BIOS_WARNING, "\nBattery failed to reach fast-charge threshold after %d ms.\n",
		DELAY_FOR_BATT_RECOVERY_MODE);

	return;
}

static void handle_battery_shipping_recovery(bool board_reset, bool need_trickle_charge)
{
	printk(BIOS_INFO, "Boot mode is %d\n", boot_mode);

	printk(BIOS_INFO, "==================================================\n");
	printk(BIOS_INFO, "Device has entered into shipping recovery mode.\n");
	printk(BIOS_INFO, "Please wait ...\n");
	printk(BIOS_INFO, "==================================================\n");

	enable_slow_battery_charging();

	/*
	 * For deeply depleted battery recovery, dynamically poll until the charger exits
	 * trickle/pre-charge into fast-charge mode.
	 *
	 * Note: skip trickle charging during no-battery boot (aka LB_BOOT_MODE_NO_BATTERY)
	 *
	 * For standard factory ship-mode exit (where cells hold nominal charge),
	 * a fixed delay is sufficient to bias and wake the BMS protection circuit.
	 */
	if (need_trickle_charge && boot_mode != LB_BOOT_MODE_NO_BATTERY) {
		/*
		 * Override board_reset after trickle charging if the battery
		 * voltage has recovered above the trickle-charge threshold.
		 */
		board_reset = true;
		wait_for_fast_charge_ready();
	} else {
		mdelay(DELAY_FOR_SHIP_MODE);
	}

	if (board_reset) {
		printk(BIOS_INFO, "Issuing board reset\n");
		do_board_reset();
	}

	/* Disable charging where `board_reset` is not allowed */
	disable_slow_battery_charging();

}

static void check_first_boot_and_reset(enum boot_mode_t mode)
{
	if ((mode == LB_BOOT_MODE_RTC_WAKE) && (boot_count_read() == 1)) {
		printk(BIOS_INFO, "First boot detected in non-normal mode; triggering reset.\n");
		do_board_reset();
	}
}

/*
 * check_invalid_recovery_request - Check if recovery mode was entered via warm reset
 *
 * If the AP is running in RO firmware (!ap_running_rw(), typical for recovery mode)
 * and the boot was initiated by a warm reset, stale memory contents and registers
 * from the previous boot cycle (e.g., RW firmware or OS) could still reside in
 * memory. A board reset is required to guarantee a clean slate.
 *
 * Return: true if running in RO after a warm reset, false otherwise.
 */
static bool check_invalid_recovery_request(void)
{
	return !ap_running_rw() && is_reset_type_warm();
}

void platform_romstage_main(void)
{
	mainboard_setup_peripherals_early();
	chipset_dload_mode_active = qclib_check_dload_mode();

	if (!CONFIG(QC_RAMDUMP_ENABLE) && chipset_dload_mode_active) {
		printk(BIOS_INFO,
		       "Ramdump mode detected even without QC_RAMDUMP_ENABLE "
		       "Kconfig being set. Issuing board reset to avoid hung state.\n");

		do_board_reset();
	}

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

	/* Underlying PMIC registers are accessible only at this point */
	boot_mode = init_boot_mode();

	/* Recovery from battery shipping mode */
	if (battery_needs_recovery || battery_is_cutoff || below_trickle_battery_voltage)
		handle_battery_shipping_recovery(battery_needs_recovery, below_trickle_battery_voltage);

	init_sdam_config();

	if (check_invalid_recovery_request()) {
		printk(BIOS_INFO,
			"Issuing board reset to wipe out stale memory context before recovery request\n");
		do_board_reset();
	}

	mainboard_setup_peripherals_late(boot_mode);

	check_first_boot_and_reset(boot_mode);
}

static void mainboard_peripherals_reinit(void)
{
	if (CONFIG(I2C_TPM))
		i2c_init(CONFIG_DRIVER_TPM_I2C_BUS, I2C_SPEED_FAST); /* H1/TPM I2C */

	if (CONFIG(EC_GOOGLE_CHROMEEC))
		qup_spi_init(CONFIG_EC_GOOGLE_CHROMEEC_SPI_BUS, 3200 * KHz); /* EC SPI */
}

void platform_romstage_postram(void)
{
	set_boot_mode(boot_mode);

	if (!chipset_dload_mode_active)
		aop_fw_load_reset();

	if (is_low_power_boot_with_charger()) {
		if (board_support_dead_battery_charging())
			configure_dead_battery_boot();

		/* FIXME: Add fast charging support */
		enable_slow_battery_charging();
	}

	qclib_rerun();

	if (CONFIG(MAINBOARD_REINIT_QUP_SE))
		mainboard_peripherals_reinit();
}
