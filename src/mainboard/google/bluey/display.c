/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/i2c_simple.h>
#include <ec/google/chromeec/ec.h>
#include <edid.h>
#include <elog.h>
#include <gpio.h>
#include <mipi/panel.h>
#include <soc/clock.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/edp_reg.h>
#include <soc/display/mdssreg.h>
#include <soc/display/mipi_dsi.h>
#include <soc/display/panel_ops.h>
#include <soc/pmic_gpio.h>
#include <soc/qcom_qup_se.h>
#include <soc/qupv3_i2c_common.h>
#include <soc/rpmh_config.h>
#include <soc/symbols_common.h>
#include <string.h>
#include <timer.h>
#include <timestamp.h>

#include "board.h"
#include "display.h"

#define BATTERY_CHARGING_SPLASH_TIMEOUT_MS (CONFIG_PLATFORM_POST_RENDER_DELAY_SEC * 1000)
static struct stopwatch splash_sw;

/* Threshold for selecting lower-resolution assets */
#define FHD_WIDTH_THRESHOLD		1920

/* AW37503 I2C LCD bias supply definitions */
#define PMIC_AW37503_SLAVE		0x3e
#define PMIC_AW37503_BUS		QUPV3_1_SE1
#define AW37503_REG_VPOS		0x00
#define AW37503_REG_VNEG		0x01
#define AW37503_REG_APPS		0x03
#define AW37503_REG_CONTROL		0x04
#define AW37503_REG_WPRTEN		0x21
#define AW37503_VENDOR_ID_MASK		0x0f
#define AW37503_VENDOR_ID		0x01
#define AW37503_VOUT_MASK		0x1f
#define AW37503_VOUT_5P9V		0x13 /* 4.0 V + 19 * 0.1 V */
#define AW37503_WPRTEN_UNLOCK		0x4c
#define AW37503_WPRTEN_LOCK		0x00
#define AW37503_APPS_CONFIG		0x43

#define PANEL_BIAS_POST_ON_MS		20
#define PANEL_BIAS_POST_OFF_MS		5

/* PMIC L13B LDO rail for display backlight */
#define BACKLIGHT_LDO_RAIL		"ldob13"
#define BACKLIGHT_LDO_UV		3072000

static struct {
	uint32_t x_res;
} cached_display_params;

static enum cb_err aw37503_read(unsigned int bus, u8 reg, u8 *value)
{
	int ret = i2c_readb(bus, PMIC_AW37503_SLAVE, reg, value);

	if (ret) {
		printk(BIOS_ERR, "Display: AW37503 reg=0x%02x read failed: %d\n", reg, ret);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err aw37503_write(unsigned int bus, u8 reg, u8 value)
{
	int ret = i2c_writeb(bus, PMIC_AW37503_SLAVE, reg, value);

	if (ret) {
		printk(BIOS_ERR, "Display: AW37503 reg=0x%02x write failed: %d\n", reg, ret);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err aw37503_set_voltage(unsigned int bus, u8 reg)
{
	u8 value;

	if (aw37503_read(bus, reg, &value) != CB_SUCCESS)
		return CB_ERR;

	value = (value & ~AW37503_VOUT_MASK) | AW37503_VOUT_5P9V;
	if (aw37503_write(bus, reg, value) != CB_SUCCESS)
		return CB_ERR;

	if (aw37503_read(bus, reg, &value) != CB_SUCCESS)
		return CB_ERR;

	if ((value & AW37503_VOUT_MASK) != AW37503_VOUT_5P9V) {
		printk(BIOS_ERR, "Display: AW37503 reg=0x%02x readback=0x%02x mismatch\n",
		       reg, value);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err aw37503_init(unsigned int bus)
{
	u8 control, apps;

	if (aw37503_read(bus, AW37503_REG_CONTROL, &control) != CB_SUCCESS)
		return CB_ERR;
	if ((control & AW37503_VENDOR_ID_MASK) != AW37503_VENDOR_ID) {
		printk(BIOS_ERR, "Display: AW37503 vendor ID mismatch: 0x%02x\n", control);
		return CB_ERR;
	}

	if (aw37503_set_voltage(bus, AW37503_REG_VPOS) != CB_SUCCESS ||
	    aw37503_set_voltage(bus, AW37503_REG_VNEG) != CB_SUCCESS)
		return CB_ERR;

	enum cb_err ret = aw37503_write(bus, AW37503_REG_WPRTEN, AW37503_WPRTEN_UNLOCK);
	if (ret == CB_SUCCESS)
		ret = aw37503_write(bus, AW37503_REG_APPS, AW37503_APPS_CONFIG);

	if (aw37503_write(bus, AW37503_REG_WPRTEN, AW37503_WPRTEN_LOCK) != CB_SUCCESS)
		return CB_ERR;
	if (ret != CB_SUCCESS)
		return CB_ERR;

	if (aw37503_read(bus, AW37503_REG_APPS, &apps) != CB_SUCCESS)
		return CB_ERR;
	if (apps != AW37503_APPS_CONFIG) {
		printk(BIOS_ERR, "Display: AW37503 APPS readback=0x%02x mismatch\n", apps);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "Display: AW37503 bias +/-5.9V configured\n");
	return CB_SUCCESS;
}

/*
 * Helper to determine if the current panel is low-resolution (<= FHD).
 */
static bool is_low_res_panel(void)
{
	return cached_display_params.x_res <= FHD_WIDTH_THRESHOLD;
}

/*
 * Mainboard-specific override for logo filenames.
 */
const char *mainboard_bmp_logo_filename(void)
{
	if (is_low_res_panel())
		return "cb_plus_logo.bmp";

	return "cb_logo.bmp";
}

#if CONFIG(PLATFORM_HAS_SECONDARY_BOOT_INDICATOR_LOGO)
bool platform_use_secondary_logo(void)
{
	return is_low_res_panel();
}
#endif

static void panel_backlight_control(bool enable)
{
	if (enable) {
		if (CONFIG(MAINBOARD_HAS_BACKLIGHT_LDO))
			rpmh_vrm_enable(BACKLIGHT_LDO_RAIL, BACKLIGHT_LDO_UV, RPMH_VRM_HPM_MODE);
		if (CONFIG(MAINBOARD_HAS_BACKLIGHT_PWM))
			pmic_gpio_output(BACKLIGHT_PWM_PMIC_ID, BACKLIGHT_PWM_PMIC_GPIO, true);
		pmic_gpio_output(BACKLIGHT_CONTROL_PMIC_ID, BACKLIGHT_CONTROL_PMIC_GPIO, true);
	} else {
		pmic_gpio_output(BACKLIGHT_CONTROL_PMIC_ID, BACKLIGHT_CONTROL_PMIC_GPIO, false);
		if (CONFIG(MAINBOARD_HAS_BACKLIGHT_PWM))
			pmic_gpio_output(BACKLIGHT_PWM_PMIC_ID, BACKLIGHT_PWM_PMIC_GPIO, false);
		if (CONFIG(MAINBOARD_HAS_BACKLIGHT_LDO))
			rpmh_vrm_disable(BACKLIGHT_LDO_RAIL);
	}
}

static void handle_dsi_reset(bool power_on)
{
	if (!CONFIG(MAINBOARD_HAS_MIPI_DSI))
		return;

	mdelay(5);
	if (power_on) {
		const struct mdss_panel_ops *panel = mdss_get_panel_ops();
		u16 pre_ms = (panel && panel->reset_pre_ms) ? panel->reset_pre_ms : 10;
		u16 low_ms = (panel && panel->reset_low_ms) ? panel->reset_low_ms : 10;
		u16 post_ms = (panel && panel->reset_post_ms) ? panel->reset_post_ms : 10;

		gpio_output(GPIO_PANEL_RESET, 1);
		mdelay(pre_ms);
		gpio_output(GPIO_PANEL_RESET, 0);
		mdelay(low_ms);
		gpio_output(GPIO_PANEL_RESET, 1);
		mdelay(post_ms);
	} else {
		gpio_output(GPIO_PANEL_RESET, 0);
		mdelay(5);
	}
}

static enum cb_err panel_power_on(void)
{
	if (CONFIG(MAINBOARD_HAS_LCD_VOLTAGE_VIA_I2C)) {
		i2c_init(PMIC_AW37503_BUS, I2C_SPEED_FAST);
		if (aw37503_init(PMIC_AW37503_BUS) != CB_SUCCESS) {
			printk(BIOS_ERR, "Display: AW37503 setup failed\n");
			return CB_ERR;
		}
		if (CONFIG_MAINBOARD_GPIO_PIN_FOR_PANEL_BIAS) {
			gpio_output(GPIO_PANEL_BIAS_EN, 1);
			mdelay(PANEL_BIAS_POST_ON_MS);
		}
	}

	panel_backlight_control(true);

	handle_dsi_reset(true);

	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_RESET)
		gpio_output(GPIO_TS_RESET_L, 1);

	return CB_SUCCESS;
}

static void panel_power_off(void)
{
	panel_backlight_control(false);

	handle_dsi_reset(false);

	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_RESET)
		gpio_output(GPIO_TS_RESET_L, 0);

	if (CONFIG(MAINBOARD_HAS_LCD_VOLTAGE_VIA_I2C)) {
		if (CONFIG_MAINBOARD_GPIO_PIN_FOR_PANEL_BIAS) {
			gpio_output(GPIO_PANEL_BIAS_EN, 0);
			mdelay(PANEL_BIAS_POST_OFF_MS);
		}
	}

	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_PANEL_AUX_POWER)
		gpio_output(GPIO_PANEL_AUX_POWER_ON, 0);

	/* Panel power off */
	gpio_output(GPIO_PANEL_POWER_ON, 0);

	/* Disable power for Touchscreen if available */
	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_POWER)
		gpio_output(GPIO_TS_POWER_EN, 0);
}

static void qcom_mdss_edp_init(struct edid *edid, uintptr_t fb_addr)
{
	if (edp_ctrl_init(edid) != CB_SUCCESS) {
		if (CONFIG(MAINBOARD_LCD_FAIL_DETECT)) {
			if (google_chromeec_lightbar_sequence(
				CONFIG_MAINBOARD_LIGHTBAR_CMD_SEQ_DIAG_LCD))
				printk(BIOS_ERR,
					"Failed to send LED/lightbar(0x%x) command to EC.\n",
					CONFIG_MAINBOARD_LIGHTBAR_CMD_SEQ_DIAG_LCD);
		}
		return;
	}

	bool dual = (edid->mode.ha > MDSS_MAX_SINGLE_PIPE_PIXEL_WIDTH) ||
		    (calculate_mode_mdp_clk(&edid->mode) > MDSS_MAX_MDP_CLK);
	struct mdss_panel_config edp_panel_config = {
		.pipe_count = dual ? 2 : 1,
		.dsc_ratio_x1000 = 0,
		.intf_num   = 5,  /* eDP uses INTF_5 on x1p42100 */
		.dsi_count  = 1,
	};
	const struct mdss_panel_config *cfg = &edp_panel_config;
	configure_vbif_qos();
	mdss_layer_mixer_setup(edid, cfg);
	mdss_source_pipe_config(edid, fb_addr, cfg);

	intf_tg_setup(edid, cfg);
	intf_fetch_start_config(edid, cfg);

	merge_3d_active(edid, cfg);

	/*
	 * eDP-specific: compression mode registers.
	 * These were previously in merge_3d_active() eDP block but belong here
	 * since they are eDP-only and should not affect DSI path.
	 */
	write32(&edp_lclk->vsc_db16_db17_db18_pb8, 0x10100);
	write32(&edp_lclk->compression_mode_ctrl, 0x2800);
	write32(&mdp_edp_intf->intf_config2, 0x111);
	write32(&edp_lclk->db_ctrl, 0x01);
	write32(&mdp_edp_intf->intf_config, 0x800000);
}

static enum cb_err qcom_mdp_start(void)
{
	stopwatch_init_msecs_expire(&splash_sw, BATTERY_CHARGING_SPLASH_TIMEOUT_MS);

	if (CONFIG(MAINBOARD_HAS_MIPI_DSI)) {
		const struct mdss_panel_ops *panel = mdss_get_panel_ops();

		if (!panel || !panel->config) {
			printk(BIOS_ERR, "Display: missing panel configuration\n");
			return CB_ERR;
		}

		const struct mdss_panel_config *cfg = panel->config;

		/*
		 * Enable the selected timing engine (DSI0: INTF_1, DSI1: INTF_2).
		 * For dual DSI (2-2-2), INTF_1 drives INTF_2 as slave via the
		 * broadcast mechanism, so only INTF_1 timing engine is enabled.
		 */
		if (!cfg->command_mode) {
			struct mdp_intf_regs *intf = cfg->dsi_controller ?
				mdp_intf2 : mdp_intf;

			write32(&intf->timing_eng_enable, 1);
			if (!(read32(&intf->timing_eng_enable) & BIT(0))) {
				printk(BIOS_ERR, "Display: INTF_%u timing engine failed\n",
				       cfg->dsi_controller ? 2 : 1);
				return CB_ERR;
			}
		} else {
			mdss_dsi_command_mode_start();
		}
	} else {
		write32(&mdp_edp_intf->timing_eng_enable, 1);
	}

	return CB_SUCCESS;
}

void display_stop(void)
{
	if (!get_lb_framebuffer())
		return;

	while (!stopwatch_expired(&splash_sw))
		mdelay(100);

	if (CONFIG(MAINBOARD_HAS_MIPI_DSI)) {
		/* DSI path: stop timing engine and disable clocks */
		mdss_dsi_display_stop();
	} else {
		/* eDP path */
		write32(&mdp_edp_intf->timing_eng_enable, 0);
		mdelay(20);
		write32(&edp_ahbclk->sw_reset, 1);
		mdelay(20);
		write32(&edp_ahbclk->sw_reset, 0);
	}

	panel_power_off();
}

static void display_logo(enum lb_fb_orientation orientation, uintptr_t fb_addr,
			 const struct edid *edid)
{
	if (!CONFIG(BMP_LOGO) || !fb_addr)
		return;

	memset((void *)fb_addr, 0, edid->bytes_per_line * edid->y_resolution);

	struct logo_config config = {
		.panel_orientation = orientation,
		.halignment = FW_SPLASH_HALIGNMENT_CENTER,
		.valignment = FW_SPLASH_VALIGNMENT_CENTER,
		.logo_bottom_margin = is_low_res_panel() ? 100 : 200,
	};
	render_logo_to_framebuffer(&config);

	if (qcom_mdp_start() != CB_SUCCESS) {
		printk(BIOS_ERR, "Display: failed to start framebuffer scanout\n");
		return;
	}

	if (!CONFIG(MAINBOARD_HAS_MIPI_DSI) && panel_power_on() != CB_SUCCESS) {
		printk(BIOS_ERR, "Display: panel power-on failed\n");
		return;
	}

	timestamp_add_now(TS_FIRMWARE_SPLASH_RENDERED);

	printk(BIOS_DEBUG, "Firmware Splash Screen : Enabled\n");

	elog_add_event_byte(ELOG_TYPE_FW_SPLASH_SCREEN, 1);
}

/*
 * qcom_mdss_dsi_init() - initialize DSI display for MIPI panel.
 * Called from display_startup() when MAINBOARD_HAS_MIPI_DSI is selected.
 * Returns edid populated by DSI init for common framebuffer registration.
 */
static void qcom_mdss_dsi_init(struct edid *edid)
{
	if (mdss_dsi_display_init(edid, panel_power_on) != CB_SUCCESS) {
		printk(BIOS_ERR, "DSI display init failed\n");
		return;
	}

	if (CONFIG(MAINBOARD_HAS_BACKLIGHT_PWM))
		return;

	/*
	 * AMOLED panels without external PWM backlight use DCS 0x51 for
	 * brightness control.
	 */
	static const u8 brightness_cmd[] = {
		PANEL_DCS(0x51, 0xff, 0x0f),
		PANEL_END,
	};
	const struct mdss_panel_ops *panel = mdss_get_panel_ops();
	const u8 *cmds = (panel && panel->backlight_cmds) ?
		panel->backlight_cmds : brightness_cmd;

	if (mdss_dsi_panel_initialize(cmds) != CB_SUCCESS)
		printk(BIOS_ERR, "Display: backlight command failed\n");
}

void display_startup(void)
{
	if (!display_init_required() || (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	struct edid edid = {};
	struct fb_info *fb;
	uintptr_t fb_addr = (REGION_SIZE(framebuffer)) ? (uintptr_t)_framebuffer : 0;
	enum lb_fb_orientation orientation = CONFIG_MAINBOARD_PANEL_ORIENTATION;

	/* Initialize RPMh subsystem and display power rails */
	if (display_rpmh_init() != CB_SUCCESS)
		return;

	enable_mdss_clk();

	if (CONFIG(MAINBOARD_HAS_MIPI_DSI))
		qcom_mdss_dsi_init(&edid);
	else
		qcom_mdss_edp_init(&edid, fb_addr);

	/* Common: register framebuffer and render logo */
	if (edid.mode.ha == 0)
		return;

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);
	fb = fb_new_framebuffer_info_from_edid(&edid, fb_addr);
	fb_set_orientation(fb, orientation);

	cached_display_params.x_res = edid.x_resolution;
	display_logo(orientation, fb_addr, &edid);
}
