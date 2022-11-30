/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <gpio.h>
#include <soc/ddp.h>
#include <soc/dptx.h>
#include <soc/dsi.h>
#include <soc/gpio_common.h>
#include <soc/mtcmos.h>

#include "display.h"
#include "panel.h"

int configure_display(void)
{
	struct edid edid;
	struct fb_info *info;
	const char *name;
	struct panel_description *panel = get_active_panel();
	if (!panel)
		return -1;

	printk(BIOS_INFO, "%s: Starting display initialization\n", __func__);

	mtcmos_display_power_on();

	panel->configure_panel_backlight();
	panel->power_on();

	mtk_ddp_init();
	mdelay(200);

	if (panel->disp_path == DISP_PATH_EDP) {
		if (mtk_edp_init(&edid) < 0) {
			printk(BIOS_ERR, "%s: Failed to initialize eDP\n", __func__);
			return -1;
		}

	} else {
		u32 mipi_dsi_flags = (MIPI_DSI_MODE_VIDEO |
				      MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
				      MIPI_DSI_MODE_LPM |
				      MIPI_DSI_MODE_EOT_PACKET);

		edid = panel->s->edid;

		if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid,
				 panel->s->init) < 0) {
			printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
			return -1;
		}
	}

	name = edid.ascii_string;
	if (name[0] == '\0')
		name = "unknown name";
	printk(BIOS_INFO, "%s: '%s %s' %dx%d@%dHz\n", __func__,
	       edid.manufacturer_name, name, edid.mode.ha, edid.mode.va,
	       edid.mode.refresh);

	edid_set_framebuffer_bits_per_pixel(&edid, 32, 0);

	mtk_ddp_mode_set(&edid, panel->disp_path);
	info = fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	if (info)
		fb_set_orientation(info, LB_FB_ORIENTATION_NORMAL);

	return 0;
}
