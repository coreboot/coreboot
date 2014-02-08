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
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <cbmem.h>
#include <edid.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/nvidia/tegra124/sdram.h>
#include "chip.h"
#include <soc/display.h>

int dump = 0;
unsigned long READL(void * p);
void WRITEL(unsigned long value, void * p);
unsigned long READL(void * p)
{
        unsigned long value = readl(p);
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

static const u32 rgb_enb_tab[PIN_REG_COUNT] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

static const u32 rgb_polarity_tab[PIN_REG_COUNT] = {
	0x00000000,
	0x01000000,
	0x00000000,
	0x00000000,
};

static const u32 rgb_data_tab[PIN_REG_COUNT] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
};

static const u32 rgb_sel_tab[PIN_OUTPUT_SEL_COUNT] = {
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00210222,
	0x00002200,
	0x00020000,
};

static int update_display_mode(struct dc_disp_reg *disp,
			       struct soc_nvidia_tegra124_config *config)
{
	u32 val;
	u32 rate;
	u32 div;

	WRITEL(0x0, &disp->disp_timing_opt);

	WRITEL(config->vref_to_sync << 16 | config->href_to_sync,
		   &disp->ref_to_sync);
	WRITEL(config->vsync_width << 16 | config->hsync_width, &disp->sync_width);
	WRITEL(config->vback_porch << 16 | config->hback_porch, &disp->back_porch);
	WRITEL(config->vfront_porch << 16 | config->hfront_porch,
		   &disp->front_porch);

	WRITEL(config->xres | (config->yres << 16), &disp->disp_active);

	val = DE_SELECT_ACTIVE << DE_SELECT_SHIFT;
	val |= DE_CONTROL_NORMAL << DE_CONTROL_SHIFT;
	WRITEL(val, &disp->data_enable_opt);

	val = DATA_FORMAT_DF1P1C << DATA_FORMAT_SHIFT;
	val |= DATA_ALIGNMENT_MSB << DATA_ALIGNMENT_SHIFT;
	val |= DATA_ORDER_RED_BLUE << DATA_ORDER_SHIFT;
	WRITEL(val, &disp->disp_interface_ctrl);

	/*
	 * The pixel clock divider is in 7.1 format (where the bottom bit
	 * represents 0.5). Here we calculate the divider needed to get from
	 * the display clock (typically 600MHz) to the pixel clock. We round
	 * up or down as requried.
	 * We use pllp for now.
	 */
	rate = 600 * 1000000;
	div = ((rate * 2 + config->pixel_clock / 2) / config->pixel_clock) - 2;
	printk(BIOS_SPEW, "Display clock %d, divider %d\n", rate, div);

	WRITEL(0x00010001, &disp->shift_clk_opt);

	val = PIXEL_CLK_DIVIDER_PCD1 << PIXEL_CLK_DIVIDER_SHIFT;
	val |= div << SHIFT_CLK_DIVIDER_SHIFT;
	WRITEL(val, &disp->disp_clk_ctrl);

	return 0;
}

static int setup_window(struct disp_ctl_win *win,
						struct soc_nvidia_tegra124_config *config)
{
	int log2_bpp = log2(config->framebuffer_bits_per_pixel);
	win->x = 0;
	win->y = 0;
	win->w = config->xres;
	win->h = config->yres;
	win->out_x = 0;
	win->out_y = 0;
	win->out_w = config->xres;
	win->out_h = config->yres;
	win->phys_addr = config->framebuffer_base;
	win->stride = config->xres * (1 << log2_bpp) / 8;
	printk(BIOS_SPEW, "%s: depth = %d\n", __func__, log2_bpp);
	switch (log2_bpp) {
		case 5:
		case 24:
			win->fmt = COLOR_DEPTH_R8G8B8A8;
			win->bpp = 32;
			break;
		case 4:
			win->fmt = COLOR_DEPTH_B5G6R5;
			win->bpp = 16;
			break;

		default:
			printk(BIOS_SPEW, "Unsupported LCD bit depth");
			return -1;
	}

	return 0;
}

static void update_window(struct display_controller *dc,
						  struct disp_ctl_win *win,
						  struct soc_nvidia_tegra124_config *config)
{
	u32 h_dda, v_dda;
	u32 val;

	val = READL(&dc->cmd.disp_win_header);
	val |= WINDOW_A_SELECT;
	WRITEL(val, &dc->cmd.disp_win_header);

	WRITEL(win->fmt, &dc->win.color_depth);

	clrsetbits_le32(&dc->win.byte_swap, BYTE_SWAP_MASK,
					BYTE_SWAP_NOSWAP << BYTE_SWAP_SHIFT);

	val = win->out_x << H_POSITION_SHIFT;
	val |= win->out_y << V_POSITION_SHIFT;
	WRITEL(val, &dc->win.pos);

	val = win->out_w << H_SIZE_SHIFT;
	val |= win->out_h << V_SIZE_SHIFT;
	WRITEL(val, &dc->win.size);

	val = (win->w * win->bpp / 8) << H_PRESCALED_SIZE_SHIFT;
	val |= win->h << V_PRESCALED_SIZE_SHIFT;
	WRITEL(val, &dc->win.prescaled_size);

	WRITEL(0, &dc->win.h_initial_dda);
	WRITEL(0, &dc->win.v_initial_dda);

	h_dda = (win->w * 0x1000) / MAX(win->out_w - 1, 1);
	v_dda = (win->h * 0x1000) / MAX(win->out_h - 1, 1);

	val = h_dda << H_DDA_INC_SHIFT;
	val |= v_dda << V_DDA_INC_SHIFT;
	WRITEL(val, &dc->win.dda_increment);

	WRITEL(win->stride, &dc->win.line_stride);
	WRITEL(0, &dc->win.buf_stride);

	val = WIN_ENABLE;
	if (win->bpp < 24)
		val |= COLOR_EXPAND;
	WRITEL(val, &dc->win.win_opt);

	WRITEL((u32) win->phys_addr, &dc->winbuf.start_addr);
	WRITEL(win->x, &dc->winbuf.addr_h_offset);
	WRITEL(win->y, &dc->winbuf.addr_v_offset);

	WRITEL(0xff00, &dc->win.blend_nokey);
	WRITEL(0xff00, &dc->win.blend_1win);

	val = GENERAL_ACT_REQ | WIN_A_ACT_REQ;
	val |= GENERAL_UPDATE | WIN_A_UPDATE;
	WRITEL(val, &dc->cmd.state_ctrl);
}

uint32_t fb_base_mb(void)
{
	return sdram_max_addressable_mb() - FB_SIZE_MB;
}

/* this is really aimed at the lcd panel. That said, there are two display
 * devices on this part and we may someday want to extend it for other boards.
 */
void display_startup(device_t dev)
{
	u32 val;
	int i;
	struct soc_nvidia_tegra124_config *config = dev->chip_info;
	struct display_controller *dc = (void *)config->display_controller;
	struct disp_ctl_win window;

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
	delay(1);
	if (config->backlight_vdd_gpio){
		gpio_output(config->backlight_vdd_gpio, 1);
		printk(BIOS_SPEW,"%s: backlight vdd setting gpio %08x to %d\n",
			__func__, config->backlight_vdd_gpio, 1);
	}
	delay(1);
	if (config->lvds_shutdown_gpio){
		gpio_output(config->lvds_shutdown_gpio, 0);
		printk(BIOS_SPEW,"%s: lvds shutdown setting gpio %08x to %d\n",
			__func__, config->lvds_shutdown_gpio, 0);
	}
	if (config->backlight_en_gpio){
		gpio_output(config->backlight_en_gpio, 1);
		printk(BIOS_SPEW,"%s: backlight enable setting gpio %08x to %d\n",
			__func__, config->backlight_en_gpio, 1);
	}

	if (config->pwm){
		gpio_output(config->pwm, 1);
		printk(BIOS_SPEW,"%s: pwm setting gpio %08x to %d\n",
			__func__, config->pwm, 1);
	}
	printk(BIOS_SPEW,
		"%s: xres %d yres %d framebuffer_bits_per_pixel %d\n",
		__func__,
	       config->xres, config->yres, config->framebuffer_bits_per_pixel);
	if (framebuffer_size_mb == 0){
		framebuffer_size_mb = ALIGN_UP(config->xres * config->yres *
			(config->framebuffer_bits_per_pixel / 8), MiB)/MiB;
	}

	if (! framebuffer_base_mb)
		framebuffer_base_mb = fb_base_mb();

	mmu_config_range(framebuffer_base_mb, framebuffer_size_mb,
		config->cache_policy);

	/* Enable flushing after LCD writes if requested */
	/* I don't understand this part yet.
	   lcd_set_flush_dcache(config.cache_type & FDT_LCD_CACHE_FLUSH);
	 */
	printk(BIOS_SPEW, "LCD frame buffer at %dMiB to %dMiB\n", framebuffer_base_mb,
		   framebuffer_base_mb + framebuffer_size_mb);

	/* GPIO magic here if needed to start powering up things. You
	 * really only want to enable vdd, wait a bit, and then enable
	 * the panel. However ... the timings in the tegra20 dts make
	 * no sense to me. I'm pretty sure they're wrong.
	 * The panel_vdd is done in the romstage, so we need only
	 * light things up here once we're sure it's all working.
	 */

	/* init dc_a */
	init_dca_regs();

	/* power up perip */
	dp_io_powerup();

	/* bringup dp */
	dp_bringup(framebuffer_base_mb*MiB);

	/* init frame buffer */
	memset((void *)(framebuffer_base_mb*MiB), 0x00,
			framebuffer_size_mb*MiB);

	/* tell depthcharge ...
	 */
	struct edid edid;
	edid.x_resolution = 1376;
	edid.y_resolution = 768;
	edid.bytes_per_line = 1376 * 2;
	edid.framebuffer_bits_per_pixel = 16;
	set_vbe_mode_info_valid(&edid, (uintptr_t)(framebuffer_base_mb*MiB));

	if (0){
/* do we still need these? */
	WRITEL(0x00000100, &dc->cmd.gen_incr_syncpt_ctrl);
	WRITEL(0x0000011a, &dc->cmd.cont_syncpt_vsync);
	WRITEL(0x00000000, &dc->cmd.int_type);
	WRITEL(0x00000000, &dc->cmd.int_polarity);
	WRITEL(0x00000000, &dc->cmd.int_mask);
	WRITEL(0x00000000, &dc->cmd.int_enb);

	val = PW0_ENABLE | PW1_ENABLE | PW2_ENABLE;
	val |= PW3_ENABLE | PW4_ENABLE | PM0_ENABLE;
	val |= PM1_ENABLE;
	WRITEL(val, &dc->cmd.disp_pow_ctrl);

	val = READL(&dc->cmd.disp_cmd);
	val |= CTRL_MODE_C_DISPLAY << CTRL_MODE_SHIFT;
	WRITEL(val, &dc->cmd.disp_cmd);

	WRITEL(0x00000020, &dc->disp.mem_high_pri);
	WRITEL(0x00000001, &dc->disp.mem_high_pri_timer);

	for (i = 0; i < PIN_REG_COUNT; i++) {
		WRITEL(rgb_enb_tab[i], &dc->com.pin_output_enb[i]);
		WRITEL(rgb_polarity_tab[i], &dc->com.pin_output_polarity[i]);
		WRITEL(rgb_data_tab[i], &dc->com.pin_output_data[i]);
	}

	for (i = 0; i < PIN_OUTPUT_SEL_COUNT; i++)
		WRITEL(rgb_sel_tab[i], &dc->com.pin_output_sel[i]);

	if (config->pixel_clock)
		update_display_mode(&dc->disp, config);

	if (!setup_window(&window, config))
		update_window(dc, &window, config);
	}
}

