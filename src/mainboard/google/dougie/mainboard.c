/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <console/uart.h>
#include <delay.h>
#include <device/device.h>
#include <device/mmio.h>
#include <edid.h>
#include <fw_config.h>
#include <soc/clock.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/mdssreg.h>
#include <soc/pcie.h>
#include <soc/pmic_gpio.h>
#include <soc/qcom_qup_se.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_config_common.h>
#include <soc/sdhci.h>
#include <soc/usb/snps_usb_phy.h>
#include <soc/usb/usb_common.h>
#include <string.h>
#include <symbols.h>

static struct usb_board_data usb0_board_data = {
	.parameter_override_x0 = 0xe6,
	.parameter_override_x1 = 0x8b,
	.parameter_override_x2 = 0x16,
	.parameter_override_x3 = 0x03,
};

static void setup_usb(void)
{
	setup_usb_host0(&usb0_board_data);
}

static void configure_sdhci(void)
{
	/* Program eMMC drive strength to 16/10/10 mA */
	write32p(SDC1_TLMM_CFG_ADDR, 0x9FE4);
}

static bool mainboard_has_emmc(void)
{
	if (!CONFIG(MAINBOARD_HAS_EMMC))
		return false;

	/* Without UFSC storage info, assume eMMC may be present. */
	if (!fw_config_is_provisioned() ||
	    fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UNKNOWN)))
		return true;

	return fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_EMMC));
}

static void edp_configure_gpios(void)
{
	/* Panel power on GPIO enable */
	gpio_output(GPIO_PANEL_POWER_ON, 1);

	/* Panel HPD GPIO enable */
	gpio_input(GPIO_PANEL_HPD);
}

static void display_startup(void)
{
	struct edid ed;
	uintptr_t fb_addr = (uintptr_t)_framebuffer;

	if (!display_init_required() || (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	enable_mdss_clk();
	edp_configure_gpios();
	mdelay(250); /* Delay for the panel to be up */
	if (edp_ctrl_init(&ed) != CB_SUCCESS)
		return;

	edid_set_framebuffer_bits_per_pixel(&ed, 32, 0);
	if (!fb_new_framebuffer_info_from_edid(&ed, fb_addr))
		return;
	memset((void *)fb_addr, 0, ed.bytes_per_line * ed.y_resolution);
	if (CONFIG(BMP_LOGO)) {
		struct logo_config config = {
			.panel_orientation = LB_FB_ORIENTATION_NORMAL,
			.halignment = FW_SPLASH_HALIGNMENT_CENTER,
			.valignment = FW_SPLASH_VALIGNMENT_CENTER,
			.logo_bottom_margin = SPLASH_LOGO_BOTTOM_MARGIN,
		};
		render_logo_to_framebuffer(&config);
	}

	mdp_dsi_video_config(&ed);
	write32(&mdp_sspp->sspp_src0, fb_addr);
	mdp_dsi_video_on();
	/*
	 * The eDP output does not recover once the MDP stream stops, so start
	 * it here and leave it running for the payload, which keeps using this
	 * framebuffer.
	 */
	write32(&mdp_intf->timing_eng_enable, 1);

	pmic_gpio_output(BACKLIGHT_PMIC_SID, BACKLIGHT_PMIC_REG_EN, true);
	pmic_gpio_output(BACKLIGHT_PMIC_SID, BACKLIGHT_PMIC_EN, true);
	pmic_gpio_output(BACKLIGHT_PMIC_SID, BACKLIGHT_PMIC_PWM, true);
}

bool mainboard_needs_pcie_init(void)
{
	return CONFIG(MAINBOARD_HAS_NVME);
}

static void mainboard_init(struct device *dev)
{
	if (mainboard_has_emmc()) {
		/* Configure clock and early CMD1 for eMMC */
		clock_configure_sdcc1(384 * MHz);
		qc_emmc_early_init();
		configure_sdhci();
	}

	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);

	/*
	 * When coreboot firmware disables serial output,
	 * we still need to load console UART QUP FW for OS.
	 */
	if (!console_serial_enabled())
		qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED);	 /* Trackpad I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED);	 /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_UART, FIFO);	 /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_1_SE4, SE_PROTOCOL_I2C, MIXED);	 /* HDMI bridge I2C */
	if (CONFIG(MAINBOARD_HAS_KEYBOARD_VIA_I2C))
		qupv3_se_fw_load_and_init(QUPV3_1_SE1, SE_PROTOCOL_I2C, MIXED); /* KBD I2C */
	if (CONFIG(MAINBOARD_HAS_AMP_VIA_I2C))
		qupv3_se_fw_load_and_init(QUPV3_0_SE6, SE_PROTOCOL_I2C, MIXED); /* Amp I2C */

	setup_usb();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

/* Runs at BS_DEV_INIT_CHIPS, so the splash shows up before the rest of ramstage. */
static void mainboard_chip_init(void *chip_info)
{
	display_startup();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.init = mainboard_chip_init,
};
