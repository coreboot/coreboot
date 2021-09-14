/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <mipi/panel.h>
#include <drivers/ti/sn65dsi86bridge/sn65dsi86bridge.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/mdssreg.h>
#include <soc/qupv3_config.h>
#include <soc/qupv3_i2c.h>
#include <soc/usb.h>
#include <types.h>

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

static void power_on_bridge(void)
{
	printk(BIOS_INFO, "%s: Bridge gpio init\n", __func__);

	/* Bridge Enable GPIO */
	gpio_output(GPIO_EDP_BRIDGE_ENABLE, 1);

	/* PP3300 EDP power supply */
	gpio_output(GPIO_EN_PP3300_DX_EDP, 1);
}

static void configure_mipi_panel(void)
{
	int panel_id = sku_id() >> 8;
	gpio_output(GPIO_MIPI_1V8_ENABLE, 1);
	mdelay(5);
	gpio_output(GPIO_AVDD_LCD_ENABLE, 1);
	mdelay(5);
	gpio_output(GPIO_AVEE_LCD_ENABLE, 1);
	mdelay(15);
	gpio_output(GPIO_VDD_RESET_1V8, 1);
	mdelay(15);
	/*
	 * In mrbland, BOE panel_id = 3, it needs 15ms delay and
	 * do reset again according to spec(See in b/197300876).
	 */
	if (CONFIG(BOARD_GOOGLE_MRBLAND) && (panel_id == 3)) {
		gpio_output(GPIO_VDD_RESET_1V8, 0);
		mdelay(5);
		gpio_output(GPIO_VDD_RESET_1V8, 1);
	}
}

static struct panel_serializable_data *get_mipi_panel(enum lb_fb_orientation *orientation)
{
	const char *cbfs_filename = NULL;
	int panel_id = sku_id() >> 8;

	if (CONFIG(BOARD_GOOGLE_MRBLAND)) {
		switch (panel_id) {
		case 3:
			cbfs_filename = "panel-BOE_TV101WUM_N53";
			*orientation = LB_FB_ORIENTATION_LEFT_UP;
			break;
		case 6:
			cbfs_filename = "panel-AUO_B101UAN08_3";
			*orientation = LB_FB_ORIENTATION_LEFT_UP;
			break;
		}
	}

	if (!cbfs_filename)
		return NULL;

	struct panel_serializable_data *panel = cbfs_map(cbfs_filename, NULL);
	if (!panel) {
		printk(BIOS_ERR, "Could not find panel data for %s!\n", cbfs_filename);
		return NULL;
	}

	return panel;
}

static enum cb_err display_init(struct panel_serializable_data *panel)
{
	uint32_t dsi_bpp = 24;
	uint32_t lanes = 4;

	if (mdss_dsi_config(&panel->edid, lanes, dsi_bpp))
		return CB_ERR;
	if (CONFIG(TROGDOR_HAS_MIPI_PANEL)) {
		if (mdss_dsi_panel_initialize(panel->init))
			return CB_ERR;
	} else {
		sn65dsi86_bridge_configure(BRIDGE_BUS, BRIDGE_CHIP, &panel->edid,
					   lanes, dsi_bpp);
		if (CONFIG(TROGDOR_HAS_BRIDGE_BACKLIGHT))
			sn65dsi86_backlight_enable(BRIDGE_BUS, BRIDGE_CHIP);
	}

	mdp_dsi_video_config(&panel->edid);
	mdss_dsi_video_mode_config(&panel->edid, dsi_bpp);
	mdp_dsi_video_on();

	return CB_SUCCESS;
}

static void display_startup(void)
{
	struct panel_serializable_data edp_panel = {0};
	struct panel_serializable_data *panel = &edp_panel;
	enum lb_fb_orientation orientation = LB_FB_ORIENTATION_NORMAL;

	if (!display_init_required()) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	if (CONFIG(TROGDOR_HAS_MIPI_PANEL)) {
		configure_mipi_panel();
		panel = get_mipi_panel(&orientation);
		if (!panel)
			return;
	} else {
		enum dp_pll_clk_src ref_clk = SN65_SEL_19MHZ;
		i2c_init(QUPV3_0_SE2, I2C_SPEED_FAST); /* EDP Bridge I2C */
		power_on_bridge();
		mdelay(250); /* Delay for the panel to be up */
		sn65dsi86_bridge_init(BRIDGE_BUS, BRIDGE_CHIP, ref_clk);
		if (sn65dsi86_bridge_read_edid(BRIDGE_BUS, BRIDGE_CHIP, &panel->edid) < 0)
			return;
	}

	printk(BIOS_INFO, "display init!\n");
	edid_set_framebuffer_bits_per_pixel(&panel->edid, 32, 0);
	if (display_init(panel) == CB_SUCCESS) {
		struct fb_info *fb = fb_new_framebuffer_info_from_edid(&panel->edid, 0);
		fb_set_orientation(fb, orientation);
	}
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
