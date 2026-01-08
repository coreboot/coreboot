/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <cbmem.h>
#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/coreboot_tables.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/pcie.h>
#include <soc/qupv3_config_common.h>
#include <soc/qup_se_handlers_common.h>
#include "board.h"
#include <soc/rpmh_config.h>
#include <soc/usb/usb.h>

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

static bool is_low_power_boot(void)
{
	enum boot_mode_t boot_mode = get_boot_mode();
	if ((boot_mode == LB_BOOT_MODE_LOW_BATTERY) ||
	    (boot_mode == LB_BOOT_MODE_OFFMODE_CHARGING))
		return true;
	return false;
}

static void enable_usb_camera(void)
{
	gpio_output(GPIO_USB_CAM_RESET_L, 1);
	gpio_output(GPIO_USB_CAM_ENABLE, 1);
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

static void setup_usb(void)
{
	setup_usb_typec();

	enable_usb_camera();
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

	/* Enable charging only during off-mode or low-battery mode and charger present */
	if (is_low_power_boot() && google_chromeec_is_charger_present())
		enable_slow_battery_charging();
}

bool mainboard_needs_pcie_init(void)
{
	/* Skip PCIe initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot())
		return false;

	return true;
}

static void display_startup(void)
{
	if (!display_init_required()) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	/* Initialize RPMh subsystem and display power rails */
	if (display_rpmh_init() != CB_SUCCESS)
		return;

	enable_mdss_clk();
}

static void mainboard_init(struct device *dev)
{
	configure_parallel_charging();

	/* Skip mainboard initialization if boot mode is "low-battery" or "off-mode charging"*/
	if (is_low_power_boot())
		return;

	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);
	gpi_firmware_load(QUP_2_GSI_BASE);

	/*
	 * Load console UART QUP firmware.
	 * This is required even if coreboot's serial output is disabled.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_2_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_1_SE0, SE_PROTOCOL_I2C, MIXED); /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE6, SE_PROTOCOL_UART, FIFO); /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED); /* Trackpad I2C */

	/* ADSP I2C (Charger/Fuel gauge) */
	qupv3_se_fw_load_and_init(QUPV3_2_SE4, SE_PROTOCOL_I2C, GSI);

	/* USB-C0 Re-Timer I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE3, SE_PROTOCOL_I2C, MIXED);

	/* USB-C1 Re-Timer I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_I2C, MIXED);

	if (!CONFIG(MAINBOARD_NO_USB_A_PORT))
		qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_I2C, MIXED); /* USB-A retimer */
	qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_I2C, MIXED); /* eUSB repeater */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT_VIA_SPI))
		qupv3_se_fw_load_and_init(QUPV3_2_SE2, SE_PROTOCOL_SPI, MIXED); /* Fingerprint SPI */

	/* Enable touchpad power */
	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHPAD_POWER)
		gpio_output(GPIO_TP_POWER_EN, 1);

	/*
	 * Deassert FPMCU reset. Power applied in romstage
	 * has now stabilized.
	 */
	if (CONFIG(MAINBOARD_HAS_FINGERPRINT))
		gpio_output(GPIO_FP_RST_L, 1);

	/* Setup USB related initial config */
	setup_usb();

	display_startup();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
