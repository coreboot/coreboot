/* SPDX-License-Identifier: GPL-2.0-only */

#include "board.h"
#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/mdssreg.h>
#include <soc/pcie.h>
#include <soc/qcom_qup_se.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_config_common.h>
#include <soc/usb/snps_usb_phy.h>
#include <soc/usb/usb_common.h>

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

	if (!display_init_required() || (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	enable_mdss_clk();
	edp_configure_gpios();
	mdelay(250); /* Delay for the panel to be up */
	if (edp_ctrl_init(&ed) == CB_SUCCESS) {
		mdp_dsi_video_config(&ed);
		mdp_dsi_video_on();
		edid_set_framebuffer_bits_per_pixel(&ed, 32, 0);
		fb_new_framebuffer_info_from_edid(&ed, (uintptr_t)0);
	}
}

bool mainboard_needs_pcie_init(void)
{
	return true;
}

static void mainboard_init(struct device *dev)
{
	gpi_firmware_load(QUP_0_GSI_BASE);
	gpi_firmware_load(QUP_1_GSI_BASE);

	/*
	 * When coreboot firmware disables serial output,
	 * we still need to load console UART QUP FW for OS.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_0_SE0, SE_PROTOCOL_I2C, MIXED);	 /* Trackpad I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED);	 /* Touch I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE7, SE_PROTOCOL_UART, FIFO);	 /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_1_SE4, SE_PROTOCOL_I2C, MIXED);	 /* HDMI bridge I2C */

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
