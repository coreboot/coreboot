/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <drivers/ti/sn65dsi86bridge/sn65dsi86bridge.h>
#include <framebuffer_info.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/mdssreg.h>
#include <soc/qupv3_config.h>
#include <soc/qupv3_i2c.h>
#include <soc/usb.h>

#include "board.h"

#define BRIDGE_BUS 0x2
#define BRIDGE_CHIP 0x2d

static struct usb_board_data usb0_board_data = {
	.pll_bias_control_2 = 0x22,
	.imp_ctrl1 = 0x08,
	.port_tune1 = 0xc5,
};

static void setup_usb(void)
{
	/* Assert EN_PP3300_HUB for those board variants that use it. */
	gpio_output(GPIO(84), 1);

	setup_usb_host0(&usb0_board_data);
}

static void qi2s_configure_gpios(void)
{
	gpio_configure(GPIO(49), GPIO49_FUNC_MI2S_1_SCK,
				GPIO_NO_PULL, GPIO_8MA, GPIO_OUTPUT);

	gpio_configure(GPIO(50), GPIO50_FUNC_MI2S_1_WS,
				GPIO_NO_PULL, GPIO_8MA, GPIO_OUTPUT);

	gpio_configure(GPIO(51), GPIO51_FUNC_MI2S_1_DATA0,
				GPIO_NO_PULL, GPIO_8MA, GPIO_OUTPUT);
}

static void load_qup_fw(void)
{
	qupv3_se_fw_load_and_init(QUPV3_0_SE1, SE_PROTOCOL_SPI, MIXED);  /* ESIM SPI */
	qupv3_se_fw_load_and_init(QUPV3_0_SE3, SE_PROTOCOL_UART, FIFO);  /* BT UART */
	qupv3_se_fw_load_and_init(QUPV3_0_SE4, SE_PROTOCOL_I2C, MIXED);  /* Pen Detect I2C */
	qupv3_se_fw_load_and_init(QUPV3_0_SE5, SE_PROTOCOL_I2C, MIXED);  /* SAR I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE1, SE_PROTOCOL_I2C, MIXED);  /* Trackpad I2C */
	/*
	 * When coreboot firmware disables serial output,
	 * we still need to load console UART QUP FW for OS.
	 */
	if (!CONFIG(CONSOLE_SERIAL))
		qupv3_se_fw_load_and_init(QUPV3_1_SE2, SE_PROTOCOL_UART, FIFO);

	qupv3_se_fw_load_and_init(QUPV3_1_SE3, SE_PROTOCOL_I2C, MIXED);  /* Speaker Amps I2C */
	qupv3_se_fw_load_and_init(QUPV3_1_SE4, SE_PROTOCOL_SPI, MIXED);  /* Fingerprint SPI */
	qupv3_se_fw_load_and_init(QUPV3_1_SE5, SE_PROTOCOL_I2C, MIXED);  /* Codec I2C */
}

static void configure_display(void)
{
	printk(BIOS_INFO, "%s: Bridge gpio init\n", __func__);

	/* Bridge Enable GPIO */
	gpio_output(GPIO_EDP_BRIDGE_ENABLE, 1);

	/* PP3300 EDP power supply */
	gpio_output(GPIO_EN_PP3300_DX_EDP, 1);
}

static void display_init(struct edid *edid)
{
	uint32_t dsi_bpp = 24;
	uint32_t lanes = 4;

	if (mdss_dsi_config(edid, lanes, dsi_bpp))
		return;

	sn65dsi86_bridge_configure(BRIDGE_BUS, BRIDGE_CHIP, edid, lanes, dsi_bpp);
	mdp_dsi_video_config(edid);
	mdss_dsi_video_mode_config(edid, dsi_bpp);
	mdp_dsi_video_on();
}

static void display_startup(void)
{
	static struct edid ed;
	enum dp_pll_clk_src ref_clk = SN65_SEL_19MHZ;

	i2c_init(QUPV3_0_SE2, I2C_SPEED_FAST); /* EDP Bridge I2C */
	if (display_init_required()) {
		configure_display();
		mdelay(250); /* Delay for the panel to be up */
		sn65dsi86_bridge_init(BRIDGE_BUS, BRIDGE_CHIP, ref_clk);
		if (sn65dsi86_bridge_read_edid(BRIDGE_BUS, BRIDGE_CHIP, &ed) < 0)
			return;

		printk(BIOS_INFO, "display init!\n");
		display_init(&ed);
		fb_new_framebuffer_info_from_edid(&ed, (uintptr_t)0);
	} else
		printk(BIOS_INFO, "Skipping display init.\n");
}

static void mainboard_init(struct device *dev)
{
	/* Take FPMCU out of reset. Power was already applied
	   in romstage and should have stabilized by now. */
	if (CONFIG(TROGDOR_HAS_FINGERPRINT))
		gpio_output(GPIO_FP_RST_L, 1);

	setup_usb();
	qi2s_configure_gpios();
	load_qup_fw();
	display_startup();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
