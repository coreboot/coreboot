/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"

#include <bootsplash.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <delay.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <halt.h>
#include <soc/pcie.h>
#include <soc/qupv3_config_common.h>
#include <soc/qupv3_i2c_common.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/variant.h>

bool mainboard_needs_pcie_init(void)
{
	return true;
}

/*
 * This function calls the underlying PMIC/EC function only once during the
 * first execution and caches the result for all subsequent calls.
 */
static enum boot_mode_t get_boot_mode(void)
{
	static bool initialized = false;
	static enum boot_mode_t boot_mode = LB_BOOT_MODE_NORMAL;
	if (!initialized) {
		enum boot_mode_t *boot_mode_ptr = cbmem_find(CBMEM_ID_BOOT_MODE);
		if (boot_mode_ptr)
			boot_mode = *boot_mode_ptr;
		printk(BIOS_INFO, "Boot mode is %d\n", boot_mode);
		initialized = true;
	}
	return boot_mode;
}

static void trigger_critical_battery_shutdown(void)
{
	printk(BIOS_WARNING, "Critical battery level detected without charger! Shutting down.\n");

	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	platform_handle_emergency_low_battery();

	google_chromeec_ap_poweroff();
}

static void load_qc_se_firmware_early(void)
{
	/* ADSP I2C (Charger/Fuel gauge) */
	qupv3_se_fw_load_and_init(QUPV3_2_SE4, SE_PROTOCOL_I2C, MIXED);

	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);
	gpi_firmware_load(QUP_2_GSI_BASE);
	gpi_firmware_load(QUP_3_GSI_BASE);
}

static bool is_low_power_boot_with_charger(void)
{
	bool ret = false;
	enum boot_mode_t boot_mode = get_boot_mode();
	if ((boot_mode == LB_BOOT_MODE_LOW_BATTERY_CHARGING) ||
	    (boot_mode == LB_BOOT_MODE_OFFMODE_CHARGING) ||
	    (boot_mode == LB_BOOT_MODE_RTC_WAKE))
		ret = true;

	return ret;
}

#if CONFIG(PLATFORM_HAS_OFF_MODE_CHARGING_INDICATOR)
bool platform_is_off_mode_charging_active(void)
{
	return (get_boot_mode() == LB_BOOT_MODE_OFFMODE_CHARGING);
}
#endif

static bool board_support_dead_battery_charging(void)
{
	uint32_t capacity;

	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return false;

	if (google_chromeec_read_batt_remaining_capacity(&capacity) < 0) {
		printk(BIOS_WARNING, "Failed to get battery capacity; defaulting to slow charging\n");
		return true;
	}

	/*
	 * If the remaining battery capacity is less than or equal to the
	 * threshold, set dead battery charging mode.
	 */
	return capacity <= DEAD_BATT_CHG_THRESHOLD_MAH;
}

/*
 * Handle charging and UI states for low-power or off-mode boot scenarios.
 * This function handles the transitions needed when the device is powered
 * solely to show a charging status rather than a full OS boot.
 */
static void handle_low_power_charging_boot(void)
{
	if (!pll_init_and_set(apss_ncc0, L_VAL_710P4MHz))
		printk(BIOS_DEBUG, "CPU Frequency set to 710MHz\n");

	if (board_support_dead_battery_charging())
		configure_dead_battery_boot();

	/* FIXME: Add fast charging support */
	enable_slow_battery_charging();

	/*
	 * Disable the lightbar for Low-Battery or Off-Mode charging sequences.
	 * This maintains visual consistency between the built-in display
	 * indicators and the external lightbar.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		google_chromeec_lightbar_off();

	/* Placeholder for display stop before launching charging applet */

	/* Boot to charging applet; if this fails, the applet should trigger a reset */
	launch_charger_applet();
}

static void mainboard_init(void *chip_info)
{
	enum boot_mode_t boot_mode = get_boot_mode();

	/* Do early display init for low/off-mode charging */
	if ((boot_mode == LB_BOOT_MODE_LOW_BATTERY) ||
			 (boot_mode == LB_BOOT_MODE_LOW_BATTERY_CHARGING) ||
			 (boot_mode == LB_BOOT_MODE_OFFMODE_CHARGING)) {
		/*
		 * Manual delay for panel readiness; required because standard SOC IP
		 * initialization is bypassed to prioritize fast-charging boot speeds.
		 */
		mdelay(250);
		/* Placeholder for display init */
	}

	/*
	 * Low-battery boot indicator is done. Therefore, power off if battery
	 * is critical and not charging
	 */
	if (get_boot_mode() == LB_BOOT_MODE_LOW_BATTERY)
		trigger_critical_battery_shutdown();

	load_qc_se_firmware_early();

	/* Skip mainboard initialization if boot mode is "low-battery" or "off-mode charging" */
	if (is_low_power_boot_with_charger()) {
		handle_low_power_charging_boot();
		halt();
	}
}

static void setup_audio(void)
{
	/* Placeholder for audio init */
}

static void setup_usb(void)
{
	/* Skip USB initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot_with_charger())
		return;

	/* Placeholder for late USB init */
}

static void setup_usb_late(void)
{
	/* Skip USB initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot_with_charger())
		return;

	/* Placeholder for late USB init */
}

void mainboard_soc_init(void)
{
	/* Setup USB related initial config */
	setup_usb();

	/* Placeholder for display init in LB_BOOT_MODE_NORMAL */

	/* Setup audio related initial config */
	setup_audio();

	/* Setup USB related late config */
	setup_usb_late();
}

static void load_qc_se_firmware_late(void)
{
	/*
	 * Load console UART QUP firmware.
	 * This is required even if coreboot's serial output is disabled.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_2_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_1_SE0, SE_PROTOCOL_I2C, MIXED); /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE6, SE_PROTOCOL_UART, FIFO); /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED); /* Trackpad I2C */

	qupv3_se_fw_load_and_init(QUPV3_0_SE3, SE_PROTOCOL_I2C, MIXED); /* USB-C0 Re-Timer I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_I2C, MIXED); /* USB-C1 Re-Timer I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED); /* USB-C2 Re-Timer I2C */

	qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_I2C, MIXED); /* eUSB repeater */
}

static void mainboard_late_init(struct device *dev)
{
	load_qc_se_firmware_late();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_late_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.init = mainboard_init,
};
