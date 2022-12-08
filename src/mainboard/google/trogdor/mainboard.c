/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <device/mmio.h>
#include <mipi/panel.h>
#include <drivers/ti/sn65dsi86bridge/sn65dsi86bridge.h>
#include <drivers/parade/ps8640/ps8640.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/mdssreg.h>
#include <soc/qupv3_config_common.h>
#include <soc/qup_se_handlers_common.h>
#include <soc/qupv3_i2c_common.h>
#include <soc/qcom_qup_se.h>
#include <soc/usb/usb_common.h>
#include <soc/usb/qusb_phy.h>
#include "board.h"
#include <soc/addressmap.h>

#define BRIDGE_BUS QUPV3_0_SE2
#define BRIDGE_SN65DSI86_CHIP	0x2d
#define BRIDGE_PS8640_CHIP	0x08

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

static bool is_ps8640_bridge(void)
{
	/*
	 * Because the board_id pins for the early Homestar builds were
	 * misstuffed, after we enable tri-state board_id pins, a -rev1
	 * board reports itself as -rev19, and a -rev2 board reports itself
	 * as -rev23. We need to account for those quirks here.
	 */
	return (CONFIG(BOARD_GOOGLE_HOMESTAR) && board_id() >= 4 &&
		board_id() != 19 && board_id() != 23) ||
	       (CONFIG(BOARD_GOOGLE_LAZOR) && board_id() >= 9) ||
	       (CONFIG(BOARD_GOOGLE_KINGOFTOWN) && board_id() >= 1) ||
	       (CONFIG(BOARD_GOOGLE_PAZQUEL) && (sku_id() & 0x4));
}

static void power_on_sn65dsi86_bridge(void)
{
	printk(BIOS_INFO, "%s: Bridge gpio init\n", __func__);

	/* Bridge Enable GPIO */
	gpio_output(GPIO_EDP_BRIDGE_ENABLE, 1);

	/* PP3300 EDP power supply */
	gpio_output(GPIO_EN_PP3300_DX_EDP, 1);
}

static void power_on_ps8640_bridge(void)
{
	printk(BIOS_INFO, "%s: Bridge gpio init\n", __func__);

	/* PP3300 EDP panel power supply */
	gpio_output(GPIO_EN_PP3300_DX_EDP, 1);

	gpio_output(GPIO_PS8640_EDP_BRIDGE_3V3_ENABLE, 1);

	/*
	 * According to ps8640 v1.4 spec, and the raise time of vdd33 is a bit
	 * long, so wait for 4ms after VDD33 goes high and then deassert PD.
	 */
	mdelay(4);

	gpio_output(GPIO_PS8640_EDP_BRIDGE_PD_L, 1);

	/*
	 * According to ps8640 app note v0.6, wait for 2ms after VDD33 goes
	 * high and then deassert RST.
	 */
	mdelay(2);

	gpio_output(GPIO_PS8640_EDP_BRIDGE_RST_L, 1);
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
	 * In mrbland, BOE panel_id = 3(EVT) or 4(DVT and after),
	 * it needs 15ms delay and do reset again according to spec
	 * (See in b/197300876).
	 */
	if (CONFIG(BOARD_GOOGLE_MRBLAND) && ((panel_id == 3) || (panel_id == 4))) {
		gpio_output(GPIO_VDD_RESET_1V8, 0);
		mdelay(5);
		gpio_output(GPIO_VDD_RESET_1V8, 1);
	}
	/*
	 * In mipi panel, TP_EN(GPIO 85) need pull up before
	 * GPIO_BACKLIGHT_ENABLE(GPIO12) up.
	 */
	if (CONFIG(TROGDOR_HAS_MIPI_PANEL))
		gpio_output(GPIO_TP_EN, 1);
}

static struct panel_serializable_data *get_mipi_panel(enum lb_fb_orientation *orientation)
{
	const char *cbfs_filename = NULL;
	int panel_id = sku_id() >> 8;

	if (CONFIG(BOARD_GOOGLE_MRBLAND)) {
		switch (panel_id) {
		case 3:
		case 4:
			cbfs_filename = "panel-BOE_TV101WUM_N53";
			*orientation = LB_FB_ORIENTATION_LEFT_UP;
			break;
		case 6:
			cbfs_filename = "panel-AUO_B101UAN08_3";
			*orientation = LB_FB_ORIENTATION_LEFT_UP;
			break;
		}
	}

	if (CONFIG(BOARD_GOOGLE_QUACKINGSTICK)) {
		switch (panel_id) {
		case 6:
			cbfs_filename = "panel-AUO_B101UAN08_3";
			*orientation = LB_FB_ORIENTATION_LEFT_UP;
			break;
		}
	}

	if (CONFIG(BOARD_GOOGLE_WORMDINGLER)) {
		switch (panel_id) {
		case 0:
			cbfs_filename = "panel-INX_P110ZZD_DF0";
			*orientation = LB_FB_ORIENTATION_LEFT_UP;
			break;
		case 4:
			cbfs_filename = "panel-BOE_TV110C9M_LL0";
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
	} else if (!is_ps8640_bridge()) {
		/*
		 * Parade ps8640 is auto-configured based on a pre-programmed
		 * SPI-ROM. Only TI sn65dsi86 needs to be configured here.
		 */
		sn65dsi86_bridge_configure(BRIDGE_BUS, BRIDGE_SN65DSI86_CHIP,
					   &panel->edid, lanes, dsi_bpp);
		if (CONFIG(TROGDOR_HAS_BRIDGE_BACKLIGHT))
			sn65dsi86_backlight_enable(BRIDGE_BUS, BRIDGE_SN65DSI86_CHIP);
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

	/* Always initialize this so QUP firmware is loaded for the kernel. */
	i2c_init(BRIDGE_BUS, I2C_SPEED_FAST);

	if (!display_init_required()) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	if (CONFIG(TROGDOR_HAS_MIPI_PANEL)) {
		configure_mipi_panel();
		panel = get_mipi_panel(&orientation);
		if (!panel)
			return;
	} else if (is_ps8640_bridge()) {
		power_on_ps8640_bridge();
		ps8640_init(BRIDGE_BUS, BRIDGE_PS8640_CHIP);
		if (ps8640_get_edid(BRIDGE_BUS, BRIDGE_PS8640_CHIP, &panel->edid) < 0)
			return;
	} else {
		enum dp_pll_clk_src ref_clk = SN65_SEL_19MHZ;
		power_on_sn65dsi86_bridge();
		mdelay(250); /* Delay for the panel to be up */
		sn65dsi86_bridge_init(BRIDGE_BUS, BRIDGE_SN65DSI86_CHIP, ref_clk);
		if (sn65dsi86_bridge_read_edid(BRIDGE_BUS, BRIDGE_SN65DSI86_CHIP,
					       &panel->edid) < 0)
			return;
	}

	printk(BIOS_INFO, "display init!\n");
	edid_set_framebuffer_bits_per_pixel(&panel->edid, 32, 0);
	if (display_init(panel) == CB_SUCCESS) {
		struct fb_info *fb = fb_new_framebuffer_info_from_edid(&panel->edid, 0);
		fb_set_orientation(fb, orientation);
	}
}

static void configure_sdhci(void)
{
	/* Program eMMC drive strength to 16/16/16 mA */
	write32p(SDC1_TLMM_CFG_ADDR, 0x9FFF);
	/* Program SD card drive strength to 16/10/10 mA */
	write32p(SDC2_TLMM_CFG_ADDR, 0x1FE4);
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
	configure_sdhci();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
