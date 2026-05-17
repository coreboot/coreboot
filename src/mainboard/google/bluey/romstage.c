/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <arch/stages.h>
#include "board.h"
#include <bootmode.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <delay.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <reset.h>
#include <security/vboot/vboot_common.h>
#include <soc/aop_common.h>
#include <soc/pcie.h>
#include <soc/pmic.h>
#include <soc/qcom_spmi.h>
#include <soc/qclib_common.h>
#include <soc/shrm.h>
#include <soc/symbols_common.h>
#include <soc/watchdog.h>

#define DELAY_FOR_SHIP_MODE 11000 /* 11sec */
#define IMEM_COOKIE_TYPE_OFFSET 108

static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;
static bool battery_present = true;
static bool battery_below_threshold = false;
static int32_t battery_cfet_status = 1; /* Active C-FET */
static int32_t battery_dfet_status = 1; /* Active D-FET */
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
bool is_off_mode(void)
{
	return is_pon_on_ac();
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

	if (!(ec_events & manual_pwron_event_mask) || battery_below_threshold ||
			!battery_present || !battery_cfet_status)
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

	uint32_t capacity;
	if (google_chromeec_read_batt_remaining_capacity(&capacity) < 0) {
		printk(BIOS_WARNING, "Failed to get battery capacity\n");
		return 0;
	}

	if (capacity <= REMAINING_BATTERY_THRESHOLD_FOR_SLOW_CHARGING)
		table->global_attributes &= ~QCLIB_GA_ENABLE_PD_NEGOTIATION;

	return 0;
}

static void early_setup_usb_typec(void)
{
	gpio_output(GPIO_USB_C1_RETIMER_RESET_L, 0);
	gpio_output(GPIO_USB_C1_EN_PP3300, 0);
	gpio_output(GPIO_USB_C1_EN_PP1800, 0);
	gpio_output(GPIO_USB_C1_EN_PP0900, 0);
	/*
	 * Only disable Type-C if the battery is present and not
	 * at a critical level, to prevent abrupt power-off.
	 */
	bool battery_power_ready = battery_present && (battery_cfet_status || battery_dfet_status);
	if (!battery_below_threshold && battery_power_ready) {
		gpio_output(GPIO_USB_C0_EN_L, 1);
		gpio_output(GPIO_USB_C1_EN_L, 1);
	}
}

static void early_setup_usb(void)
{
	early_setup_usb_typec();
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

static void edp_configure_gpios(void)
{
	/* Ensure enabling power for Touchscreen if available */
	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_POWER)
		gpio_output(GPIO_TS_POWER_EN, 1);

	/* Panel power on GPIO enable */
	gpio_output(GPIO_PANEL_POWER_ON, 1);

	/* Panel HPD GPIO enable */
	gpio_input(GPIO_PANEL_HPD);
}

/**
 * Update and cache battery status from the EC.
 * This should be called once, early in the boot process,
 * after the EC is reachable.
 */
static void update_battery_status(void)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	struct ec_response_battery_get_misc_info misc_info;

	battery_present = google_chromeec_is_battery_present();
	battery_below_threshold = google_chromeec_is_below_critical_threshold();

	if (!google_chromeec_get_battery_misc_info(&misc_info)) {
		battery_cfet_status = misc_info.cfet_status;
		battery_dfet_status = misc_info.dfet_status;
	}

	/*
	 * SHIP MODE RECOVERY HANDLER:
	 * Triggered when the battery is present, CFET is disabled, and the
	 * battery status indicates it is waiting in an uninitialized charging state.
	 */
	bool is_bms_locked = (battery_cfet_status <= 0) && (battery_dfet_status <= 0);
	battery_needs_recovery = battery_present && is_bms_locked;
}

/* Perform romstage early hardware initialization */
static void mainboard_setup_peripherals_early(void)
{
	platform_init_lightbar();

	update_battery_status();

	/*
	 * Power on NVMe early so that the DDR init and other operations
	 * that follow provide an organic >50ms delay before PCIe PERST
	 * de-assertion in platform_romstage_postram(), satisfying the
	 * NVMe spec requirement without a static mdelay().
	 */
	gcom_pcie_power_on_ep();

	edp_configure_gpios();

	/* This GPIO has external pullup hence disable default PD */
	gpio_input(GPIO_LID_OPEN_S3);

	/* Setup early USB related config */
	early_setup_usb();

	/* Watchdog must be checked first to avoid erasing watchdog info later. */
	check_wdog();
}

static void late_setup_usb_typec(void)
{
	bool battery_power_ready = battery_present && (battery_cfet_status || battery_dfet_status);

	if (battery_below_threshold || !battery_power_ready)
		return;

	gpio_output(GPIO_USB_C0_EN_L, 0);
	gpio_output(GPIO_USB_C1_EN_L, 0);
}

/*
 * Perform romstage late hardware initialization based on boot mode.
 * Handles PCIe host setup and fingerprint sensor power rails.
 */
static void mainboard_setup_peripherals_late(int mode)
{
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
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI)) {
		if (mode == LB_BOOT_MODE_NORMAL || mode == LB_BOOT_MODE_NO_BATTERY)
			gpio_output(GPIO_EN_FP_RAILS, 1);
	}
}

static void handle_battery_shipping_recovery(void)
{
	printk(BIOS_INFO, "==================================================\n");
	printk(BIOS_INFO, "Device has entered into shipping recovery mode.\n");
	printk(BIOS_INFO, "Please wait ...\n");
	printk(BIOS_INFO, "==================================================\n");

	enable_slow_battery_charging();
	mdelay(DELAY_FOR_SHIP_MODE);

	printk(BIOS_INFO, "Issuing board reset\n");
	do_board_reset();
}

static bool check_ramdump_mode_is_set(void)
{
	if (!CONFIG(QC_RAMDUMP_ENABLE))
		return false;

	uint32_t rawdump_val = read32(_shared_imem + IMEM_COOKIE_TYPE_OFFSET);

	if (rawdump_val == 0x1) {
		printk(BIOS_DEBUG, "Ramdump mode detected: 0x%x\n", rawdump_val);
		return true;
	}

	printk(BIOS_DEBUG, "Ramdump mode not enabled (value: 0x%x)\n", rawdump_val);

	return false;
}

void platform_romstage_main(void)
{
	static bool ramdump_mode = false;

	if (check_ramdump_mode_is_set())
		ramdump_mode = true;

	mainboard_setup_peripherals_early();

	chipset_dload_mode_active = qclib_check_dload_mode();

	if (!chipset_dload_mode_active)
		shrm_fw_load_reset();

	/* QCLib: DDR init & train */
	qclib_load_and_run();

	/* Recovery from battery shipping mode */
	if (battery_needs_recovery)
		handle_battery_shipping_recovery();

	init_sdam_config();

	/* Underlying PMIC registers are accessible only at this point */
	set_boot_mode();

	if (!chipset_dload_mode_active)
		aop_fw_load_reset();

	mainboard_setup_peripherals_late(boot_mode);

	qclib_rerun();

	if (ramdump_mode) {
		printk(BIOS_INFO, "Issuing board reset to come out of Ramdump mode\n");
		do_board_reset();
	}

	late_setup_usb_typec();
}

void platform_romstage_postram(void)
{
	enum boot_mode_t *boot_mode_ptr = cbmem_add(CBMEM_ID_BOOT_MODE, sizeof(*boot_mode_ptr));
	if (boot_mode_ptr) {
		*boot_mode_ptr = boot_mode;
		printk(BIOS_INFO, "Boot mode is %d\n", *boot_mode_ptr);
	}
}
