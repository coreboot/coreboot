/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <ec/google/chromeec/ec.h>
#include <edid.h>
#include <fw_config.h>
#include <gpio.h>
#include <halt.h>
#include <soc/cdt.h>
#include <soc/clock.h>
#include <soc/pcie.h>
#include <soc/platform_info.h>
#include <soc/qupv3_config_common.h>
#include <soc/qupv3_i2c_common.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/symbols_common.h>
#include <soc/usb/usb.h>
#include <soc/variant.h>
#include <soc/rpmh_config.h>

#include "board.h"
#include "display.h"

#define C0_RETIMER_I2C_BUS	0x03
#define C1_RETIMER_I2C_BUS	0x07
#define PS8820_SLAVE_ADDR	0x08

#define PS8820_USB_PORT_CONN_STATUS_REG	0x00
#define USB3_MODE_NORMAL_VAL	0x21
#define USB3_MODE_FLIP_VAL	0x23

void mainboard_usb_typec_configure(uint8_t port_num, bool inverse_polarity)
{
	if (!CONFIG(MAINBOARD_HAS_PS8820_RETIMER))
		return;

	/* There are only two ports (0 and 1) */
	if (port_num > 1) {
		printk(BIOS_WARNING, "Invalid USB Type-C port number (%d)!\n", port_num);
		return;
	}

	uint8_t bus = port_num ? C1_RETIMER_I2C_BUS : C0_RETIMER_I2C_BUS;
	uint8_t mode_value = inverse_polarity ? USB3_MODE_FLIP_VAL : USB3_MODE_NORMAL_VAL;
	i2c_writeb(bus, PS8820_SLAVE_ADDR, PS8820_USB_PORT_CONN_STATUS_REG, mode_value);
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

static void enable_usb_camera(void)
{
	if (CONFIG(MAINBOARD_HAS_CAMERA_VIA_USB)) {
		gpio_output(GPIO_USB_CAM_RESET_L, 1);
		gpio_output(GPIO_USB_CAM_ENABLE, 1);
	} else {
		gpio_output(GPIO_USB_CAM_ENABLE, 0);
	}
}

static void setup_usb_typec(void)
{
	gpio_output(GPIO_USB_C1_EN_PP3300, 1);
	mdelay(1);
	gpio_output(GPIO_USB_C1_EN_PP1800, 1);
	mdelay(1);
	gpio_output(GPIO_USB_C1_EN_PP0900, 1);
	mdelay(1);
	gpio_output(GPIO_USB_C1_RETIMER_RESET_L, 1);
}

static void setup_audio_gpios(void)
{
	gpio_output(GPIO_SNDW_AMP_0_ENABLE, 0);
	gpio_output(GPIO_SNDW_AMP_1_ENABLE, 0);
	gpio_configure_no_egpio(GPIO_SNDW_0_SCL, GPIO_FUN_SNDW_0_SCL,
		GPIO_NO_PULL, GPIO_16MA, GPIO_OUTPUT);
	gpio_configure_no_egpio(GPIO_SNDW_0_SDA, GPIO_FUN_SNDW_0_SDA,
		GPIO_NO_PULL, GPIO_16MA, GPIO_OUTPUT);
}

static void setup_audio(void)
{
	setup_audio_gpios();
}

static void setup_usb(void)
{
	setup_usb_typec();
	enable_usb_camera();
}

static void setup_usb_late(void)
{
	/* Skip USB initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot_with_charger())
		return;

	setup_usb_host0();
}

void lb_add_boot_mode(struct lb_header *header)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	struct lb_boot_mode *mode = (struct lb_boot_mode *)lb_new_record(header);
	memset(mode, 0, sizeof(*mode));

	mode->tag = LB_TAG_BOOT_MODE;
	mode->size = sizeof(*mode);
	mode->boot_mode = get_boot_mode();
}

bool mainboard_needs_pcie_init(void)
{
	if (CONFIG(MAINBOARD_HAS_UFS))
		return false;

	/* Skip PCIe initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot_with_charger())
		return false;

	return true;
}

#if CONFIG(PLATFORM_HAS_OFF_MODE_CHARGING_INDICATOR)
bool platform_is_off_mode_charging_active(void)
{
	return (get_boot_mode() == LB_BOOT_MODE_OFFMODE_CHARGING);
}
#endif

static void trigger_critical_battery_shutdown(void)
{
	printk(BIOS_WARNING, "Critical battery level detected without charger! Shutting down.\n");

	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	platform_handle_emergency_low_battery();

	/*
	 * Disable lightbar for visual consistency between the built-in
	 * display indicators and the external LED status.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		google_chromeec_lightbar_off();

	google_chromeec_ap_poweroff();
}

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

	display_stop();

	if (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())
		mdelay(CONFIG_PLATFORM_POST_RENDER_DELAY_SEC * 1000);

	if (CONFIG(EC_GOOGLE_CHROMEEC) && detect_ac_unplug_event(false)) {
		google_chromeec_offmode_heartbeat();
		google_chromeec_ap_poweroff();
	}

	/*
	 * Disable the lightbar for Low-Battery or Off-Mode charging sequences.
	 * This maintains visual consistency between the built-in display
	 * indicators and the external lightbar.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		google_chromeec_lightbar_off();

	enable_fast_battery_charging();

	/* Boot to charging applet; if this fails, the applet should trigger a reset */
	launch_charger_applet();
}

static void load_qc_se_firmware_early(void)
{
	/* ADSP I2C (Charger/Fuel gauge) */
	qupv3_se_fw_load_and_init(QUPV3_2_SE4, SE_PROTOCOL_I2C, MIXED);

	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);
	gpi_firmware_load(QUP_2_GSI_BASE);
}

static void mainboard_init(void *chip_info)
{
	configure_parallel_charging();
	configure_debug_access_port();

	enum boot_mode_t boot_mode = get_boot_mode();

	/* Do early display init for low/off-mode charging */
	if ((boot_mode == LB_BOOT_MODE_LOW_BATTERY) ||
			 (boot_mode == LB_BOOT_MODE_LOW_BATTERY_CHARGING) ||
			 (boot_mode == LB_BOOT_MODE_OFFMODE_CHARGING)) {
		/* Clear pending events before entering low-power boot */
		clear_pending_ec_events();

		/*
		 * Manual delay for panel readiness; required because standard SOC IP
		 * initialization is bypassed to prioritize fast-charging boot speeds.
		 */
		mdelay(250);
		display_startup();
	}

	/*
	 * Low-battery boot indicator is done. Therefore, power off if battery
	 * is critical and not charging
	 */
	if (boot_mode == LB_BOOT_MODE_LOW_BATTERY)
		trigger_critical_battery_shutdown();

	load_qc_se_firmware_early();

	configure_parallel_charging_late();

	/* Skip mainboard initialization if boot mode is "low-battery" or "off-mode charging" */
	if (is_low_power_boot_with_charger()) {
		handle_low_power_charging_boot();
		halt();
	}
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

	if (CONFIG(MAINBOARD_HAS_PS8820_RETIMER)) {
		i2c_init(QUPV3_0_SE3, I2C_SPEED_FAST); /* USB-C0 Re-Timer I2C */
		i2c_init(QUPV3_0_SE7, I2C_SPEED_FAST); /* USB-C1 Re-Timer I2C */
	} else {
		qupv3_se_fw_load_and_init(QUPV3_0_SE3, SE_PROTOCOL_I2C, MIXED); /* USB-C0 Re-Timer I2C */
		qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_I2C, MIXED); /* USB-C1 Re-Timer I2C */
	}

	if (!CONFIG(MAINBOARD_NO_USB_A_PORT))
		qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_I2C, MIXED); /* USB-A retimer */

	qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_I2C, MIXED); /* eUSB repeater */

	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		qupv3_se_fw_load_and_init(QUPV3_2_SE2, SE_PROTOCOL_SPI, MIXED); /* Fingerprint SPI */
}

static void mainboard_late_init(struct device *dev)
{
	load_qc_se_firmware_late();
}

void mainboard_soc_init(void)
{
	/* Setup USB related initial config */
	setup_usb();

	enum boot_mode_t boot_mode = get_boot_mode();

	if (boot_mode == LB_BOOT_MODE_NORMAL || boot_mode == LB_BOOT_MODE_NO_BATTERY)
		display_startup();

	if (CONFIG(MAINBOARD_HAS_UFS))
		ufs_rpmh_init();

	/* Enable touchpad power */
	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHPAD_POWER)
		gpio_output(GPIO_TP_POWER_EN, 1);

	/* Deassert FPMCU reset. Power applied in romstage has now stabilized. */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT))
		gpio_output(GPIO_FP_RST_L, 1);

	/* Setup audio related initial config */
	setup_audio();

	/* Setup USB related late config */
	setup_usb_late();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_late_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.init = mainboard_init,
};

void fw_config_get_mainboard_override(uint64_t *fw_config)
{
	if (!CONFIG(SOC_QUALCOMM_CDT))
		return;

	uint16_t soc_id;
	switch (platform_get_soc_id()) {
	case SOC_ID_HAMOA:
		soc_id = HAMOA_ID_SCP;
		break;
	case SOC_ID_X1P42100:
		soc_id = X1P42100_ID_SCP;
		break;
	default:
		printk(BIOS_WARNING, "CDT: Unknown SoC ID, skipping fw_config override\n");
		return;
	}

	uint8_t platform_type = cdt_get_platform_id();
	uint32_t soc_platform_id = ((uint32_t)soc_id << 16) | platform_type;

	printk(BIOS_INFO, "CDT: soc_platform_id=0x%08x (soc_id=0x%04x, platform_type=0x%02x)\n",
	       soc_platform_id, soc_id, platform_type);

	*fw_config = soc_platform_id;
}
