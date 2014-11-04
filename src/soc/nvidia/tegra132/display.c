/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <lib.h>
#include <stdlib.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <device/device.h>
#include <device/i2c.h>
#include <string.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <edid.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/funitcfg.h>
#include "chip.h"
#include <soc/display.h>

int dump = 0;
unsigned long READL(void * p)
{
        unsigned long value;

	/*
	 * In case of hard hung on readl(p), we can set dump > 1 to print out
	 * the address accessed.
	 */
        if (dump > 1)
		printk(BIOS_SPEW, "readl %p\n", p);

        value = readl(p);
        if (dump)
		printk(BIOS_SPEW, "readl %p %08lx\n", p, value);
        return value;
}

void WRITEL(unsigned long value, void * p)
{
        if (dump)
		printk(BIOS_SPEW, "writel %p %08lx\n", p, value);
        writel(value, p);
}

/* return in 1000ths of a Hertz */
static int tegra_calc_refresh(const struct soc_nvidia_tegra132_config *config)
{
	int refresh;
	int h_total = htotal(config);
	int v_total = vtotal(config);
	int pclk = config->pixel_clock;

	if (!pclk || !h_total || !v_total)
		return 0;
	refresh = pclk / h_total;
	refresh *= 1000;
	refresh /= v_total;
	return refresh;
}

static void print_mode(const struct soc_nvidia_tegra132_config *config)
{
	if (config) {
		int refresh = tegra_calc_refresh(config);
		printk(BIOS_ERR,
			"MODE:%dx%d@%d.%03uHz pclk=%d\n",
			config->xres, config->yres,
			refresh / 1000, refresh % 1000,
			config->pixel_clock);
	}
}

static int update_display_mode(struct display_controller *disp_ctrl,
			       struct soc_nvidia_tegra132_config *config)
{
	print_mode(config);

	printk(BIOS_ERR, "config:      xres:yres: %d x %d\n ",
			config->xres, config->yres);
	printk(BIOS_ERR, "   href_sync:vref_sync: %d x %d\n ",
			config->href_to_sync, config->vref_to_sync);
	printk(BIOS_ERR, " hsyn_width:vsyn_width: %d x %d\n ",
			config->hsync_width, config->vsync_width);
	printk(BIOS_ERR, " hfnt_porch:vfnt_porch: %d x %d\n ",
			config->hfront_porch, config->vfront_porch);
	printk(BIOS_ERR, "   hbk_porch:vbk_porch: %d x %d\n ",
			config->hback_porch, config->vback_porch);

	WRITEL(0x0, &disp_ctrl->disp.disp_timing_opt);
	WRITEL(0x0, &disp_ctrl->disp.disp_color_ctrl);

	// select DSI
	WRITEL(DSI_ENABLE, &disp_ctrl->disp.disp_win_opt);

	WRITEL(config->vref_to_sync << 16 | config->href_to_sync,
		&disp_ctrl->disp.ref_to_sync);

	WRITEL(config->vsync_width << 16 | config->hsync_width,
		&disp_ctrl->disp.sync_width);


	WRITEL((config->vback_porch << 16) | config->hback_porch,
		&disp_ctrl->disp.back_porch);

	WRITEL((config->vfront_porch << 16) | config->hfront_porch,
		&disp_ctrl->disp.front_porch);

	WRITEL(config->xres | (config->yres << 16),
		&disp_ctrl->disp.disp_active);

	/**
	 * We want to use PLLD_out0, which is PLLD / 2:
	 *   PixelClock = (PLLD / 2) / ShiftClockDiv / PixelClockDiv.
	 *
	 * Currently most panels work inside clock range 50MHz~100MHz, and PLLD
	 * has some requirements to have VCO in range 500MHz~1000MHz (see
	 * clock.c for more detail). To simplify calculation, we set
	 * PixelClockDiv to 1 and ShiftClockDiv to 1. In future these values
	 * may be calculated by clock_display, to allow wider frequency range.
	 *
	 * Note ShiftClockDiv is a 7.1 format value.
	 */
	const u32 shift_clock_div = 1;
	WRITEL((PIXEL_CLK_DIVIDER_PCD1 << PIXEL_CLK_DIVIDER_SHIFT) |
	       ((shift_clock_div - 1) * 2 + 1) << SHIFT_CLK_DIVIDER_SHIFT,
	       &disp_ctrl->disp.disp_clk_ctrl);
	printk(BIOS_DEBUG, "%s: PixelClock=%u, ShiftClockDiv=%u\n",
	       __func__, config->pixel_clock, shift_clock_div);
	return 0;
}

/*
 * update_window:
 *   set up window registers and activate window except two:
 *   frame buffer base address register (WINBUF_START_ADDR) and
 *   display enable register (_DISP_DISP_WIN_OPTIONS). This is
 *   becasue framebuffer is not available until payload stage.
 */
static void update_window(const struct soc_nvidia_tegra132_config *config)
{
	struct display_controller *disp_ctrl =
			(void *)config->display_controller;
	u32 val;

	WRITEL(WINDOW_A_SELECT, &disp_ctrl->cmd.disp_win_header);

	WRITEL(((config->yres << 16) | config->xres), &disp_ctrl->win.size);
	WRITEL(((config->yres << 16) |
		(config->xres * config->framebuffer_bits_per_pixel / 8)),
		&disp_ctrl->win.prescaled_size);

	val = ALIGN_UP((config->xres * config->framebuffer_bits_per_pixel / 8),
			64);
	WRITEL(val, &disp_ctrl->win.line_stride);

	WRITEL(config->color_depth, &disp_ctrl->win.color_depth);
	WRITEL(COLOR_BLACK, &disp_ctrl->disp.blend_background_color);
	WRITEL((V_DDA_INC(0x1000) | H_DDA_INC(0x1000)),
		&disp_ctrl->win.dda_increment);

	WRITEL(DISP_CTRL_MODE_C_DISPLAY, &disp_ctrl->cmd.disp_cmd);

	WRITEL(WRITE_MUX_ACTIVE, &disp_ctrl->cmd.state_access);

	WRITEL(0, &disp_ctrl->win.buffer_addr_mode);

	val = PW0_ENABLE | PW1_ENABLE | PW2_ENABLE | PW3_ENABLE |
		PW4_ENABLE | PM0_ENABLE | PM1_ENABLE;
	WRITEL(val, &disp_ctrl->cmd.disp_pow_ctrl);

	val = GENERAL_UPDATE | WIN_A_UPDATE;
	val |= GENERAL_ACT_REQ | WIN_A_ACT_REQ;
	WRITEL(val, &disp_ctrl->cmd.state_ctrl);
}

static int tegra_dc_init(struct display_controller *disp_ctrl)
{
	/* do not accept interrupts during initialization */
	WRITEL(0x00000000, &disp_ctrl->cmd.int_mask);
	WRITEL(WRITE_MUX_ASSEMBLY | READ_MUX_ASSEMBLY,
		&disp_ctrl->cmd.state_access);
	WRITEL(WINDOW_A_SELECT, &disp_ctrl->cmd.disp_win_header);
	WRITEL(0x00000000, &disp_ctrl->win.win_opt);
	WRITEL(0x00000000, &disp_ctrl->win.byte_swap);
	WRITEL(0x00000000, &disp_ctrl->win.buffer_ctrl);

	WRITEL(0x00000000, &disp_ctrl->win.pos);
	WRITEL(0x00000000, &disp_ctrl->win.h_initial_dda);
	WRITEL(0x00000000, &disp_ctrl->win.v_initial_dda);
	WRITEL(0x00000000, &disp_ctrl->win.dda_increment);
	WRITEL(0x00000000, &disp_ctrl->win.dv_ctrl);

	WRITEL(0x01000000, &disp_ctrl->win.blend_layer_ctrl);
	WRITEL(0x00000000, &disp_ctrl->win.blend_match_select);
	WRITEL(0x00000000, &disp_ctrl->win.blend_nomatch_select);
	WRITEL(0x00000000, &disp_ctrl->win.blend_alpha_1bit);

	WRITEL(0x00000000, &disp_ctrl->winbuf.start_addr_hi);
	WRITEL(0x00000000, &disp_ctrl->winbuf.addr_h_offset);
	WRITEL(0x00000000, &disp_ctrl->winbuf.addr_v_offset);

	WRITEL(0x00000000, &disp_ctrl->com.crc_checksum);
	WRITEL(0x00000000, &disp_ctrl->com.pin_output_enb[0]);
	WRITEL(0x00000000, &disp_ctrl->com.pin_output_enb[1]);
	WRITEL(0x00000000, &disp_ctrl->com.pin_output_enb[2]);
	WRITEL(0x00000000, &disp_ctrl->com.pin_output_enb[3]);
	WRITEL(0x00000000, &disp_ctrl->disp.disp_signal_opt0);

	return 0;
}

void display_startup(device_t dev)
{
	struct soc_nvidia_tegra132_config *config = dev->chip_info;
	struct display_controller *disp_ctrl =
			(void *)config->display_controller;
	u32 plld_rate;

	u32 framebuffer_size_mb = config->framebuffer_size / MiB;
	u32 framebuffer_base_mb= config->framebuffer_base / MiB;

	printk(BIOS_INFO, "%s: entry: disp_ctrl: %p.\n",
		 __func__, disp_ctrl);

	if (disp_ctrl == NULL) {
		printk(BIOS_ERR, "Error: No dc is assigned by dt.\n");
		return;
	}

	if (framebuffer_size_mb == 0){
		framebuffer_size_mb = ALIGN_UP(config->xres * config->yres *
			(config->framebuffer_bits_per_pixel / 8), MiB)/MiB;
	}

	config->framebuffer_size = framebuffer_size_mb * MiB;
	config->framebuffer_base = framebuffer_base_mb * MiB;

	/*
	 * The plld is programmed with the assumption of the SHIFT_CLK_DIVIDER
	 * and PIXEL_CLK_DIVIDER are zero (divide by 1). See the
	 * update_display_mode() for detail.
	 */
	/* set default plld */
	plld_rate = clock_display(config->pixel_clock * 2);
	if (plld_rate == 0) {
		printk(BIOS_ERR, "dc: clock init failed\n");
		return;
	} else if (plld_rate != config->pixel_clock * 2) {
		printk(BIOS_WARNING, "dc: plld rounded to %u\n", plld_rate);
	}

	/* set disp1's clock source to PLLD_OUT0 */
	clock_configure_source(disp1, PLLD, (plld_rate/KHz)/2);

	/* Init dc */
	if (tegra_dc_init(disp_ctrl)) {
		printk(BIOS_ERR, "dc: init failed\n");
		return;
	}

	/* Configure dc mode */
	if (update_display_mode(disp_ctrl, config)) {
		printk(BIOS_ERR, "dc: failed to configure display mode.\n");
		return;
	}

	/* Configure and enable dsi controller and panel */
	if (dsi_enable(config)) {
		printk(BIOS_ERR, "%s: failed to enable dsi controllers.\n",
			__func__);
		return;
	}

	/* Set up window */
	update_window(config);
	printk(BIOS_INFO, "%s: display init done.\n", __func__);

	/*
	 * Pass panel information to cb tables
	 */
	struct edid edid;
	/* Align bytes_per_line to 64 bytes as required by dc */
	edid.bytes_per_line = ALIGN_UP((config->xres *
				config->framebuffer_bits_per_pixel / 8), 64);
	edid.x_resolution = edid.bytes_per_line /
				(config->framebuffer_bits_per_pixel / 8);
	edid.y_resolution = config->yres;
	edid.framebuffer_bits_per_pixel = config->framebuffer_bits_per_pixel;

	printk(BIOS_INFO, "%s: bytes_per_line: %d, bits_per_pixel: %d\n "
			"               x_res x y_res: %d x %d, size: %d\n",
			 __func__, edid.bytes_per_line,
			edid.framebuffer_bits_per_pixel,
			edid.x_resolution, edid.y_resolution,
			(edid.bytes_per_line * edid.y_resolution));

	set_vbe_mode_info_valid(&edid, 0);

	/*
	 * After this point, it is payload's responsibility to allocate
	 * framebuffer and sets the base address to dc's
	 * WINBUF_START_ADDR register and enables window by setting dc's
	 * DISP_DISP_WIN_OPTIONS register.
	 */
}

