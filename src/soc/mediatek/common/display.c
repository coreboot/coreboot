/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <edid.h>
#include <framebuffer_info.h>
#include <soc/ddp.h>
#include <soc/display.h>
#include <soc/dptx.h>
#include <soc/dsi.h>
#include <soc/mtcmos.h>
#include <stdio.h>

static struct panel_serializable_data *get_mipi_cmd_from_cbfs(struct panel_description *desc)
{
	/*
	 * The CBFS file name is panel-{MANUFACTURER}-${PANEL_NAME}, where MANUFACTURER is 3
	 * characters and PANEL_NAME is usually 13 characters.
	 */
	char cbfs_name[64];
	static union {
		u8 raw[4 * 1024];  /* Most panels only need < 2K. */
		struct panel_serializable_data s;
	} buffer;

	if (!desc->name) {
		printk(BIOS_ERR, "Missing panel CBFS file name.\n");
		return NULL;
	}

	snprintf(cbfs_name, sizeof(cbfs_name), "panel-%s", desc->name);
	if (cbfs_load(cbfs_name, buffer.raw, sizeof(buffer)))
		return &buffer.s;

	printk(BIOS_ERR, "Missing %s in CBFS.\n", cbfs_name);
	return NULL;
}

__weak int mtk_edp_init(struct mtk_dp *mtk_dp, struct edid *edid)
{
	printk(BIOS_WARNING, "%s: Not supported\n", __func__);
	return -1;
}

__weak int mtk_edp_enable(struct mtk_dp *mtk_dp)
{
	printk(BIOS_WARNING, "%s: Not supported\n", __func__);
	return -1;
}

__weak int mtk_dsi_init(u32 mode_flags, u32 format, u32 lanes,
			const struct edid *edid, const u8 *init_commands)
{
	printk(BIOS_WARNING, "%s: Not supported\n", __func__);
	return -1;
}

int mtk_display_init(void)
{
	struct edid edid = {0};
	struct mtk_dp mtk_edp = {0};
	struct fb_info *info;
	const char *name;
	struct panel_description *panel = get_active_panel();

	if (!panel || panel->disp_path == DISP_PATH_NONE) {
		printk(BIOS_ERR, "%s: Failed to get the active panel\n", __func__);
		return -1;
	}

	printk(BIOS_INFO, "%s: Starting display initialization\n", __func__);

	mtcmos_display_power_on();
	mtcmos_protect_display_bus();

	if (panel->configure_backlight)
		panel->configure_backlight();
	if (panel->power_on)
		panel->power_on();

	mtk_ddp_init();

	if (panel->disp_path == DISP_PATH_EDP) {
		mdelay(200);
		if (mtk_edp_init(&mtk_edp, &edid) < 0) {
			printk(BIOS_ERR, "%s: Failed to initialize eDP\n", __func__);
			return -1;
		}
	} else {
		struct panel_serializable_data *mipi_data = NULL;

		if (panel->get_edid) {
			if (panel->get_edid(&edid) < 0)
				return -1;
		} else {
			mipi_data = get_mipi_cmd_from_cbfs(panel);
			if (!mipi_data)
				return -1;
			edid = mipi_data->edid;
		}

		u32 mipi_dsi_flags = (MIPI_DSI_MODE_VIDEO |
				      MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
				      MIPI_DSI_MODE_LPM |
				      MIPI_DSI_MODE_EOT_PACKET);

		if (mtk_dsi_init(mipi_dsi_flags, MIPI_DSI_FMT_RGB888, 4, &edid,
				 mipi_data ? mipi_data->init : NULL) < 0) {
			printk(BIOS_ERR, "%s: Failed in DSI init\n", __func__);
			return -1;
		}

		if (panel->post_power_on && panel->post_power_on(&edid) < 0) {
			printk(BIOS_ERR, "%s: Failed to post power on bridge\n", __func__);
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

	if (panel->disp_path == DISP_PATH_EDP) {
		if (mtk_edp_enable(&mtk_edp) < 0) {
			printk(BIOS_ERR, "%s: Failed to enable eDP\n", __func__);
			return -1;
		}
	}

	info = fb_new_framebuffer_info_from_edid(&edid, (uintptr_t)0);
	if (info)
		fb_set_orientation(info, panel->orientation);

	return 0;
}
