/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include "board.h"
#include <bootmode.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <security/vboot/vboot_common.h>
#include <soc/aop_common.h>
#include <soc/pmic.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/watchdog.h>

static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;

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
bool is_off_mode(void)
{
	return is_pon_on_ac();
}

static enum boot_mode_t set_boot_mode(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return boot_mode;

	enum boot_mode_t boot_mode_new;

	if (is_off_mode() && google_chromeec_is_battery_present()) {
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

static bool is_pd_sync_required(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return false;

	const uint64_t manual_pwron_event_mask =
		(EC_HOST_EVENT_MASK(EC_HOST_EVENT_POWER_BUTTON) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_LID_OPEN));
	uint64_t ec_events = google_chromeec_get_events_b();

	if (!(ec_events & EC_HOST_EVENT_MASK(EC_HOST_EVENT_AC_CONNECTED)))
		return false;

	if (!(ec_events & manual_pwron_event_mask) ||
		google_chromeec_is_below_critical_threshold() || !google_chromeec_is_battery_present())
		return true;

	return false;
}

/* Check if it is okay to enable PD sync. */
static bool vboot_can_enable_pd_sync(void)
{
	if (!CONFIG(VBOOT))
		return false;

	/* Always enable if in developer or recovery mode */
	if (vboot_developer_mode_enabled() || vboot_recovery_mode_enabled() ||
			 vboot_check_recovery_request())
		return true;

	/* Otherwise disable */
	return false;
}

int qclib_mainboard_override(struct qclib_cb_if_table *table)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return 0;

	if (is_pd_sync_required() || vboot_can_enable_pd_sync())
		table->global_attributes |= QCLIB_GA_ENABLE_PD_NEGOTIATION;
	else
		table->global_attributes &= ~QCLIB_GA_ENABLE_PD_NEGOTIATION;
	return 0;
}

static void platform_dump_battery_soc_information(void)
{
	uint32_t batt_pct;
	if (google_chromeec_read_batt_state_of_charge(&batt_pct))
		printk(BIOS_WARNING, "Failed to get battery level\n");
	else
		printk(BIOS_INFO, "Battery state-of-charge %d%%\n", batt_pct);
}

static void early_setup_usb_typec(void)
{
	gpio_output(GPIO_USB_C1_RETIMER_RESET_L, 0);
	gpio_output(GPIO_USB_C1_EN_PP3300, 0);
	gpio_output(GPIO_USB_C1_EN_PP1800, 0);
	gpio_output(GPIO_USB_C1_EN_PP0900, 0);
}

static void early_setup_usb(void)
{
	early_setup_usb_typec();
}

void platform_romstage_main(void)
{
	/*
	 * Early initialization of the Chrome EC lightbar.
	 * Ensures visual continuity if the AP firmware disabled the lightbar
	 * in a previous boot without a subsequent EC reset.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		google_chromeec_lightbar_on();

	/*
	 * Only alert the user (set LED to red in color) if the lid is closed and the battery
	 * is critically low without AC power.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC) && CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch() &&
			 google_chromeec_is_critically_low_on_battery())
		google_chromeec_set_lightbar_rgb(0xff, 0xff, 0x00, 0x00);

	/* Setup early USB related config */
	early_setup_usb();

	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();

	if (CONFIG(EC_GOOGLE_CHROMEEC) && CONFIG(CONSOLE_SERIAL))
		platform_dump_battery_soc_information();

	if (!qclib_check_dload_mode())
		shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	/* Underlying PMIC registers are accessible only at this point */
	set_boot_mode();

	aop_fw_load_reset();

	qclib_rerun();

	/*
	 * Enable this power rail now for FPMCU stability prior to
	 * its reset being deasserted in ramstage. This applies
	 * when MAINBOARD_HAS_FINGERPRINT_VIA_SPI Kconfig is enabled.
	 * Requires >=200ms delay after its pin was driven low in bootblock.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		gpio_output(GPIO_EN_FP_RAILS, 1);
}

void platform_romstage_postram(void)
{
	enum boot_mode_t *boot_mode_ptr = cbmem_add(CBMEM_ID_BOOT_MODE, sizeof(*boot_mode_ptr));
	if (boot_mode_ptr) {
		*boot_mode_ptr = boot_mode;
		printk(BIOS_INFO, "Boot mode is %d\n", *boot_mode_ptr);
	}
}
