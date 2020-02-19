/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <stdint.h>
#include <device/device.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/display.h>
#include <framebuffer_info.h>

#include "chip.h"

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
static int tegra_calc_refresh(const struct soc_nvidia_tegra210_config *config)
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

static void print_mode(const struct soc_nvidia_tegra210_config *config)
{
	if (config) {
		int refresh = tegra_calc_refresh(config);
		printk(BIOS_ERR,
			"Panel Mode: %dx%d@%d.%03uHz pclk=%d\n",
			config->xres, config->yres,
			refresh / 1000, refresh % 1000,
			config->pixel_clock);
	}
}

int update_display_mode(struct display_controller *disp_ctrl,
			struct soc_nvidia_tegra210_config *config)
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

	/* select win opt */
	WRITEL(config->win_opt, &disp_ctrl->disp.disp_win_opt);

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

	/*
	 *   PixelClock = (PLLD / 2) / ShiftClockDiv / PixelClockDiv.
	 *
	 *   default: Set both shift_clk_div and pixel_clock_div to 1
	 */
	update_display_shift_clock_divider(disp_ctrl, SHIFT_CLK_DIVIDER(1));

	return 0;
}

void update_display_shift_clock_divider(struct display_controller *disp_ctrl,
					u32 shift_clock_div)
{
	WRITEL((PIXEL_CLK_DIVIDER_PCD1 << PIXEL_CLK_DIVIDER_SHIFT) |
	       (shift_clock_div & 0xff) << SHIFT_CLK_DIVIDER_SHIFT,
	       &disp_ctrl->disp.disp_clk_ctrl);
	printk(BIOS_DEBUG, "%s: ShiftClockDiv=%u\n",
	       __func__, shift_clock_div);
}

/*
 * update_window:
 *   set up window registers and activate window except two:
 *   frame buffer base address register (WINBUF_START_ADDR) and
 *   display enable register (_DISP_DISP_WIN_OPTIONS). This is
 *   because framebuffer is not available until payload stage.
 */
void update_window(const struct soc_nvidia_tegra210_config *config)
{
	struct display_controller *disp_ctrl =
			(void *)config->display_controller;
	u32 val;

	WRITEL(WINDOW_A_SELECT, &disp_ctrl->cmd.disp_win_header);

	WRITEL(((config->yres << 16) | config->xres), &disp_ctrl->win.size);

	WRITEL(((config->display_yres << 16) |
		(config->display_xres *
		config->framebuffer_bits_per_pixel / 8)),
		&disp_ctrl->win.prescaled_size);

	val = ALIGN_UP((config->display_xres *
		config->framebuffer_bits_per_pixel / 8), 64);
	WRITEL(val, &disp_ctrl->win.line_stride);

	WRITEL(config->color_depth, &disp_ctrl->win.color_depth);
	WRITEL(COLOR_BLACK, &disp_ctrl->disp.blend_background_color);

	WRITEL(((DDA_INC(config->display_yres, config->yres) << 16) |
		DDA_INC(config->display_xres, config->xres)),
		&disp_ctrl->win.dda_increment);

	WRITEL(DISP_CTRL_MODE_C_DISPLAY, &disp_ctrl->cmd.disp_cmd);

	WRITEL(WRITE_MUX_ACTIVE, &disp_ctrl->cmd.state_access);

	WRITEL(0, &disp_ctrl->win.buffer_addr_mode);

	val = PW0_ENABLE | PW1_ENABLE | PW2_ENABLE | PW3_ENABLE |
		PW4_ENABLE | PM0_ENABLE | PM1_ENABLE;
	WRITEL(val, &disp_ctrl->cmd.disp_pow_ctrl);

	val = GENERAL_UPDATE | WIN_A_UPDATE;
	WRITEL(val, &disp_ctrl->cmd.state_ctrl);

	val = GENERAL_ACT_REQ | WIN_A_ACT_REQ;
	WRITEL(val, &disp_ctrl->cmd.state_ctrl);
}

int tegra_dc_init(struct display_controller *disp_ctrl)
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

/*
 * Save mode to cb tables
 */
void pass_mode_info_to_payload(
			struct soc_nvidia_tegra210_config *config)
{
	const uint32_t bytes_per_line = ALIGN_UP(config->display_xres *
		DIV_ROUND_UP(config->framebuffer_bits_per_pixel, 8), 64);
	/* The framebuffer address is zero to let the payload allocate it */
	fb_add_framebuffer_info(0, config->display_xres, config->display_yres,
					bytes_per_line, config->framebuffer_bits_per_pixel);
}
