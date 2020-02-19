/* SPDX-License-Identifier: MIT */
/*
 * Copied from Linux drivers/gpu/drm/ast/ast_mode.c
 */

#include <console/console.h>
#include <edid.h>
#include <device/pci_def.h>
#include <framebuffer_info.h>

#include "ast_drv.h"

/*
 * Set framebuffer MMIO address, which must fall into BAR0 MMIO window.
 *
 * Complete reimplementation as the original expects multiple kernel internal
 * subsystems to be present.
 */
int ast_crtc_do_set_base(struct drm_crtc *crtc)
{
	struct ast_private *ast = crtc->dev->dev_private;
	struct drm_framebuffer *fb = crtc->primary->fb;

	/* PCI BAR 0 */
	struct resource *res = find_resource(crtc->dev->pdev, PCI_BASE_ADDRESS_0);
	if (!res) {
		printk(BIOS_ERR, "BAR0 resource not found.\n");
		return -EIO;
	}

	if (res->size < fb->pitches[0] * crtc->mode.vdisplay) {
		dev_err(dev->pdev, "Framebuffer doesn't fit into BAR0 MMIO window\n");
		return -ENOMEM;
	}

	fb->mmio_addr = (uintptr_t)res2mmio(res, 4095, 4095);

	ast_set_offset_reg(crtc);
	ast_set_start_address_crt1(ast, fb->mmio_addr);

	return 0;
}

static void ast_edid_to_drmmode(struct edid *edid, struct drm_display_mode *mode)
{
	memset(mode, 0, sizeof(*mode));

	mode->hdisplay = edid->mode.ha;
	mode->vdisplay = edid->mode.va;
	mode->crtc_hdisplay = edid->mode.ha;
	mode->crtc_vdisplay = edid->mode.va;

	/* EDID clock is in 10kHz, but drm clock is in KHz */
	mode->clock = edid->mode.pixel_clock * 10;
	mode->vrefresh = edid->mode.refresh;

	mode->crtc_hblank_start = edid->mode.ha;
	mode->crtc_hblank_end = edid->mode.ha + edid->mode.hbl;
	mode->crtc_hsync_start = edid->mode.ha + edid->mode.hso;
	mode->crtc_hsync_end = edid->mode.ha + edid->mode.hso + edid->mode.hspw;
	mode->crtc_htotal = mode->crtc_hblank_end;

	mode->crtc_vblank_start = edid->mode.va;
	mode->crtc_vblank_end = edid->mode.va + edid->mode.vbl;
	mode->crtc_vsync_start = edid->mode.va + edid->mode.vso;
	mode->crtc_vsync_end = edid->mode.va + edid->mode.vso + edid->mode.vspw;
	mode->crtc_vtotal = mode->crtc_vblank_end;

	mode->flags = 0;
	if (edid->mode.phsync == '+')
		mode->flags |= DRM_MODE_FLAG_PHSYNC;
	else
		mode->flags |= DRM_MODE_FLAG_NHSYNC;

	if (edid->mode.pvsync == '+')
		mode->flags |= DRM_MODE_FLAG_PVSYNC;
	else
		mode->flags |= DRM_MODE_FLAG_NVSYNC;
}

static int ast_select_mode(struct drm_connector *connector,
			   struct edid *edid)
{
	struct ast_private *ast = connector->dev->dev_private;
	bool widescreen;
	u8 raw[128];
	bool flags = false;

	if (ast->tx_chip_type == AST_TX_DP501) {
		ast->dp501_maxclk = 0xff;
		flags = ast_dp501_read_edid(connector->dev, (u8 *)raw);
		if (flags)
			ast->dp501_maxclk = ast_get_dp501_max_clk(connector->dev);
		else
			dev_err(dev->pdev, "I2C transmission error\n");
	}

	if (!flags)
		ast_software_i2c_read(ast, raw);

	if (decode_edid(raw, sizeof(raw), edid) != EDID_CONFORMANT) {
		/*
		 * Servers often run headless, so a missing EDID is not an error.
		 * We still need to initialize a framebuffer for KVM, though.
		 */
		dev_info(dev->pdev, "Failed to decode EDID\n");
		printk(BIOS_DEBUG, "Assuming VGA for KVM\n");

		memset(edid, 0, sizeof(*edid));

		edid->mode.pixel_clock = 6411;
		edid->mode.refresh = 60;
		edid->mode.ha = 1024;
		edid->mode.hspw = 4;
		edid->mode.hso = 56;
		edid->mode.hbl = 264;
		edid->mode.phsync = '-';

		edid->mode.va = 768;
		edid->mode.vspw = 3;
		edid->mode.vso = 1;
		edid->mode.vbl = 26;
		edid->mode.pvsync = '+';
	}

	printk(BIOS_DEBUG, "AST: Display has %dpx x %dpx\n", edid->mode.ha, edid->mode.va);

	widescreen = !!(((edid->mode.ha * 4) % (edid->mode.va * 3)));

	while (ast_mode_valid(connector, edid->mode.ha, edid->mode.va) != MODE_OK) {
		/* Select a compatible smaller mode */
		if (edid->mode.ha > 1920 && widescreen) {
			edid->mode.ha = 1920;
			edid->mode.va = 1080;
		} else if (edid->mode.ha >= 1920 && widescreen) {
			edid->mode.ha = 1680;
			edid->mode.va = 1050;
		} else if (edid->mode.ha >= 1680 && widescreen) {
			edid->mode.ha = 1600;
			edid->mode.va = 900;
		} else if (edid->mode.ha >= 1680 && !widescreen) {
			edid->mode.ha = 1600;
			edid->mode.va = 1200;
		} else if (edid->mode.ha >= 1600 && widescreen) {
			edid->mode.ha = 1440;
			edid->mode.va = 900;
		} else if (edid->mode.ha >= 1440 && widescreen) {
			edid->mode.ha = 1360;
			edid->mode.va = 768;
		} else if (edid->mode.ha >= 1360 && widescreen) {
			edid->mode.ha = 1280;
			edid->mode.va = 800;
		} else if (edid->mode.ha >= 1360 && !widescreen) {
			edid->mode.ha = 1280;
			edid->mode.va = 1024;
		} else if (edid->mode.ha >= 1280) {
			edid->mode.ha = 1024;
			edid->mode.va = 768;
		} else if (edid->mode.ha >= 1024) {
			edid->mode.ha = 800;
			edid->mode.va = 600;
		} else if (edid->mode.ha >= 800) {
			edid->mode.ha = 640;
			edid->mode.va = 480;
		} else {
			dev_err(dev->pdev, "No compatible mode found.\n");

			return -EIO;
		}
	};

	return 0;
}

int ast_driver_framebuffer_init(struct drm_device *dev, int flags)
{
	struct drm_display_mode adjusted_mode;
	struct drm_crtc crtc;
	struct drm_format format;
	struct drm_primary primary;
	struct drm_framebuffer fb;
	struct drm_connector connector;
	struct edid edid;
	int ret;

	/* Init wrapper structs */
	connector.dev = dev;

	format.cpp[0] = 4; /* 32 BPP */
	fb.format = &format;

	primary.fb = &fb;

	crtc.dev = dev;
	crtc.primary = &primary;

	/* Read EDID and find mode */
	ret = ast_select_mode(&connector, &edid);
	if (ret) {
		dev_err(dev->pdev, "Failed to select mode.\n");
		return ret;
	}

	/* Updated edid for fb_fill_framebuffer_info */
	edid.x_resolution = edid.mode.ha;
	edid.y_resolution = edid.mode.va;
	edid.framebuffer_bits_per_pixel = format.cpp[0] * 8;
	edid.bytes_per_line = ALIGN_UP(edid.x_resolution * format.cpp[0], 8);

	/* Updated framebuffer info for ast_crtc_mode_set */
	fb.pitches[0] = edid.bytes_per_line;

	printk(BIOS_DEBUG, "Using framebuffer %dpx x %dpx pitch %d @ %d BPP\n",
	       edid.x_resolution, edid.y_resolution, edid.bytes_per_line,
	       edid.framebuffer_bits_per_pixel);

	/* Convert EDID to AST DRM mode */
	ast_edid_to_drmmode(&edid, &crtc.mode);

	memcpy(&adjusted_mode, &crtc.mode, sizeof(crtc.mode));

	ret = ast_crtc_mode_set(&crtc, &crtc.mode, &adjusted_mode);
	if (ret) {
		dev_err(dev->pdev, "Failed to set mode.\n");
		return ret;
	}

	ast_hide_cursor(&crtc);

	/* Advertise new mode */
	fb_new_framebuffer_info_from_edid(&edid, fb.mmio_addr);

	/* Clear display */
	memset((void *)(uintptr_t)fb.mmio_addr, 0, edid.bytes_per_line * edid.y_resolution);

	return 0;
}
