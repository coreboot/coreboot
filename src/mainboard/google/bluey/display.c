/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <bootmode.h>
#include <bootsplash.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <elog.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/display/edp_ctrl.h>
#include <soc/display/edp_reg.h>
#include <soc/display/mdssreg.h>
#include <soc/pmic_gpio.h>
#include <soc/symbols_common.h>
#include <soc/rpmh_config.h>
#include <timer.h>
#include <timestamp.h>
#include <ec/google/chromeec/ec.h>

#include "board.h"
#include "display.h"

#define BATTERY_CHARGING_SPLASH_TIMEOUT_MS (CONFIG_PLATFORM_POST_RENDER_DELAY_SEC * 1000)
static struct stopwatch splash_sw;

/* Threshold for selecting lower-resolution assets */
#define FHD_WIDTH_THRESHOLD		1920

static struct {
	uint32_t x_res;
} cached_display_params;

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

static void edp_enable_backlight(void)
{
	/* Enable backlight PWM */
	if (CONFIG(MAINBOARD_HAS_BACKLIGHT_PWM))
		pmic_gpio_output(BACKLIGHT_PWM_PMIC_ID, BACKLIGHT_PWM_PMIC_GPIO, true);

	/* Enable backlight */
	pmic_gpio_output(BACKLIGHT_CONTROL_PMIC_ID, BACKLIGHT_CONTROL_PMIC_GPIO, true);
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

	configure_vbif_qos();
	mdss_layer_mixer_setup(edid);
	mdss_source_pipe_config(edid, fb_addr);

	intf_tg_setup(edid);
	intf_fetch_start_config(edid);

	merge_3d_active(edid);
}

static void qcom_mdp_start(uintptr_t fb_addr)
{
	stopwatch_init_msecs_expire(&splash_sw, BATTERY_CHARGING_SPLASH_TIMEOUT_MS);

	write32(&mdp_intf->timing_eng_enable, 1);
}

void display_stop(void)
{
	if (!get_lb_framebuffer())
		return;

	while (!stopwatch_expired(&splash_sw))
		mdelay(100);

	write32(&mdp_intf->timing_eng_enable, 0);
	mdelay(20);
	write32(&edp_ahbclk->sw_reset, 1);
	mdelay(20);
	write32(&edp_ahbclk->sw_reset, 0);

	/* Disable backlight */
	pmic_gpio_output(BACKLIGHT_CONTROL_PMIC_ID, BACKLIGHT_CONTROL_PMIC_GPIO, false);

	/* Disable backlight PWM */
	if (CONFIG(MAINBOARD_HAS_BACKLIGHT_PWM))
		pmic_gpio_output(BACKLIGHT_PWM_PMIC_ID, BACKLIGHT_PWM_PMIC_GPIO, false);

	/* Panel power off */
	gpio_output(GPIO_PANEL_POWER_ON, 0);

	/* Disable power for Touchscreen if available */
	if (CONFIG_MAINBOARD_GPIO_PIN_FOR_TOUCHSCREEN_POWER)
		gpio_output(GPIO_TS_POWER_EN, 0);
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

	qcom_mdp_start(fb_addr);

	edp_enable_backlight();

	timestamp_add_now(TS_FIRMWARE_SPLASH_RENDERED);

	printk(BIOS_DEBUG, "Firmware Splash Screen : Enabled\n");

	elog_add_event_byte(ELOG_TYPE_FW_SPLASH_SCREEN, 1);
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
	enum lb_fb_orientation orientation = LB_FB_ORIENTATION_NORMAL;

	/* Initialize RPMh subsystem and display power rails */
	if (display_rpmh_init() != CB_SUCCESS)
		return;

	enable_mdss_clk();
	qcom_mdss_edp_init(&edid, fb_addr);
	if (edid.mode.ha == 0)
		return;

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);
	fb = fb_new_framebuffer_info_from_edid(&edid, fb_addr);
	fb_set_orientation(fb, orientation);

	cached_display_params.x_res = edid.x_resolution;
	display_logo(orientation, fb_addr, &edid);
}
