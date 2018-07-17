/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <delay.h>
#include <device/device.h>
#include <edid.h>
#include <lib.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/display.h>
#include <soc/sdram.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/nvidia/tegra/pwm.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "chip.h"

struct tegra_dc dc_data;

int dump = 0;
unsigned long READL(void *p)
{
        unsigned long value;

	/*
	 * In case of hard hung on readl(p), we can set dump > 1 to print out
	 * the address accessed.
	 */
        if (dump > 1)
		printk(BIOS_SPEW, "readl %p\n", p);

        value = read32(p);
        if (dump)
		printk(BIOS_SPEW, "readl %p %08lx\n", p, value);
        return value;
}

void WRITEL(unsigned long value, void *p)
{
        if (dump)
		printk(BIOS_SPEW, "writel %p %08lx\n", p, value);
        write32(p, value);
}

/* return in 1000ths of a Hertz */
static int tegra_dc_calc_refresh(const struct soc_nvidia_tegra124_config *config)
{
	int h_total, v_total, refresh;
	int pclk = config->pixel_clock;

	h_total = config->xres + config->hfront_porch + config->hback_porch +
		config->hsync_width;
	v_total = config->yres + config->vfront_porch + config->vback_porch +
		config->vsync_width;
	if (!pclk || !h_total || !v_total)
		return 0;
	refresh = pclk / h_total;
	refresh *= 1000;
	refresh /= v_total;
	return refresh;
}

static void print_mode(const struct soc_nvidia_tegra124_config *config)
{
	if (config) {
		int refresh = tegra_dc_calc_refresh(config);
		printk(BIOS_ERR,
			"MODE:%dx%d@%d.%03uHz pclk=%d\n",
			config->xres, config->yres,
			refresh / 1000, refresh % 1000,
			config->pixel_clock);
	}
}

static int update_display_mode(struct display_controller *disp_ctrl,
			       struct soc_nvidia_tegra124_config *config)
{
	print_mode(config);

	WRITEL(0x1, &disp_ctrl->disp.disp_timing_opt);

	WRITEL(config->vref_to_sync << 16 | config->href_to_sync,
		&disp_ctrl->disp.ref_to_sync);

	WRITEL(config->vsync_width << 16 | config->hsync_width,
		&disp_ctrl->disp.sync_width);

	WRITEL(((config->vback_porch - config->vref_to_sync) << 16) | config->hback_porch,
		&disp_ctrl->disp.back_porch);

	WRITEL(((config->vfront_porch + config->vref_to_sync) << 16) | config->hfront_porch,
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
	       ((shift_clock_div - 1) * 2) << SHIFT_CLK_DIVIDER_SHIFT,
	       &disp_ctrl->disp.disp_clk_ctrl);
	printk(BIOS_DEBUG, "%s: PixelClock=%u, ShiftClockDiv=%u\n",
	       __func__, config->pixel_clock, shift_clock_div);
	return 0;
}

static void update_window(struct display_controller *disp_ctrl,
			  struct soc_nvidia_tegra124_config *config)
{
	u32 val;

	WRITEL(WINDOW_A_SELECT, &disp_ctrl->cmd.disp_win_header);

	WRITEL(((config->yres << 16) | config->xres), &disp_ctrl->win.size);
	WRITEL(((config->yres << 16) |
		(config->xres * config->framebuffer_bits_per_pixel / 8)),
		&disp_ctrl->win.prescaled_size);
	WRITEL(((config->xres * config->framebuffer_bits_per_pixel / 8 + 31) /
		32 * 32), &disp_ctrl->win.line_stride);

	WRITEL(config->color_depth, &disp_ctrl->win.color_depth);

	WRITEL(config->framebuffer_base, &disp_ctrl->winbuf.start_addr);
	WRITEL((V_DDA_INC(0x1000) | H_DDA_INC(0x1000)), &disp_ctrl->win.dda_increment);

	WRITEL(COLOR_WHITE, &disp_ctrl->disp.blend_background_color);
	WRITEL(DISP_CTRL_MODE_C_DISPLAY, &disp_ctrl->cmd.disp_cmd);

	WRITEL(WRITE_MUX_ACTIVE, &disp_ctrl->cmd.state_access);

	val = GENERAL_ACT_REQ | WIN_A_ACT_REQ;
	val |= GENERAL_UPDATE | WIN_A_UPDATE;
	WRITEL(val, &disp_ctrl->cmd.state_ctrl);

	// Enable win_a
	val = READL(&disp_ctrl->win.win_opt);
	WRITEL(val | WIN_ENABLE, &disp_ctrl->win.win_opt);
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

uint32_t fb_base_mb(void)
{
	return sdram_max_addressable_mb() - FB_SIZE_MB;
}

/* this is really aimed at the lcd panel. That said, there are two display
 * devices on this part and we may someday want to extend it for other boards.
 */
void display_startup(struct device *dev)
{
	struct soc_nvidia_tegra124_config *config = dev->chip_info;
	struct display_controller *disp_ctrl = (void *)config->display_controller;
	struct pwm_controller	*pwm = (void *)TEGRA_PWM_BASE;
	struct tegra_dc		*dc = &dc_data;
	u32 plld_rate;

	/* init dc */
	dc->base = (void *)TEGRA_ARM_DISPLAYA;
	dc->config = config;
	config->dc_data = dc;

	/* Note dp_init may read EDID and change some config values. */
	dp_init(config);

	/* should probably just make it all MiB ... in future */
	u32 framebuffer_size_mb = config->framebuffer_size / MiB;
	u32 framebuffer_base_mb= config->framebuffer_base / MiB;

	/* light it all up */
	/* This one may have been done in romstage but that's ok for now. */
	if (config->panel_vdd_gpio){
		gpio_output(config->panel_vdd_gpio, 1);
		printk(BIOS_SPEW,"%s: panel_vdd setting gpio %08x to %d\n",
			__func__, config->panel_vdd_gpio, 1);
	}
	udelay(config->vdd_delay_ms * 1000);
	if (config->backlight_vdd_gpio){
		gpio_output(config->backlight_vdd_gpio, 1);
		printk(BIOS_SPEW,"%s: backlight vdd setting gpio %08x to %d\n",
			__func__, config->backlight_vdd_gpio, 1);
	}
	if (config->lvds_shutdown_gpio){
		gpio_output(config->lvds_shutdown_gpio, 0);
		printk(BIOS_SPEW,"%s: lvds shutdown setting gpio %08x to %d\n",
			__func__, config->lvds_shutdown_gpio, 0);
	}

	if (framebuffer_size_mb == 0){
		framebuffer_size_mb = ALIGN_UP(config->xres * config->yres *
			(config->framebuffer_bits_per_pixel / 8), MiB)/MiB;
	}

	if (! framebuffer_base_mb)
		framebuffer_base_mb = fb_base_mb();

	config->framebuffer_size = framebuffer_size_mb * MiB;
	config->framebuffer_base = framebuffer_base_mb * MiB;

	mmu_config_range(framebuffer_base_mb, framebuffer_size_mb,
		DCACHE_WRITETHROUGH);

	printk(BIOS_SPEW, "LCD frame buffer at %dMiB to %dMiB\n", framebuffer_base_mb,
		   framebuffer_base_mb + framebuffer_size_mb);

	/* GPIO magic here if needed to start powering up things. You
	 * really only want to enable vdd, wait a bit, and then enable
	 * the panel. However ... the timings in the tegra20 dts make
	 * no sense to me. I'm pretty sure they're wrong.
	 * The panel_vdd is done in the romstage, so we need only
	 * light things up here once we're sure it's all working.
	 */

	/* The plld is programmed with the assumption of the SHIFT_CLK_DIVIDER
	 * and PIXEL_CLK_DIVIDER are zero (divide by 1). See the
	 * update_display_mode() for detail.
	 */
	plld_rate = clock_display(config->pixel_clock * 2);
	if (plld_rate == 0) {
		printk(BIOS_ERR, "dc: clock init failed\n");
		return;
	} else if (plld_rate != config->pixel_clock * 2) {
		printk(BIOS_WARNING, "dc: plld rounded to %u\n", plld_rate);
		config->pixel_clock = plld_rate / 2;
	}

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

	/* Enable dp */
	dp_enable(dc->out);

	/* Init frame buffer */
	memset((void *)(framebuffer_base_mb*MiB), 0x00,
			framebuffer_size_mb*MiB);

	update_window(disp_ctrl, config);

	/* Set up Tegra PWM n (where n is specified in config->pwm) to drive the
	 * panel backlight.
	 */
	printk(BIOS_SPEW, "%s: enable panel backlight pwm\n", __func__);
	WRITEL(((1 << NV_PWM_CSR_ENABLE_SHIFT) |
		(220 << NV_PWM_CSR_PULSE_WIDTH_SHIFT) | /* 220/256 */
		0x02e), /* frequency divider */
	       &pwm->pwm[config->pwm].csr);

	udelay(config->pwm_to_bl_delay_ms * 1000);
	if (config->backlight_en_gpio){
		gpio_output(config->backlight_en_gpio, 1);
		printk(BIOS_SPEW,"%s: backlight enable setting gpio %08x to %d\n",
			__func__, config->backlight_en_gpio, 1);
	}

	printk(BIOS_INFO, "%s: display init done.\n", __func__);

	/* tell depthcharge ...
	 */
	struct edid edid;
	edid.mode.va = config->yres;
	edid.mode.ha = config->xres;
	edid_set_framebuffer_bits_per_pixel(&edid,
		config->framebuffer_bits_per_pixel, 32);
	set_vbe_mode_info_valid(&edid, (uintptr_t)(framebuffer_base_mb*MiB));
}
