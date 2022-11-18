/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/mmio.h>
#include <console/console.h>
#include <soc/clk.h>
#include <soc/dp.h>
#include <soc/fimd.h>
#include <soc/periph.h>
#include <soc/sysreg.h>

/* fairly useful debugging stuff. */
#if 0
static inline void fwadl(unsigned long l,void *v) {
	writel(l, v);
	printk(BIOS_SPEW, "W %p %p\n", v, (void *)l);
}
#define lwritel(a,b) fwadl((unsigned long)(a), (void *)(b))

static inline unsigned long fradl(void *v) {
	unsigned long l = readl(v);
	printk(BIOS_SPEW, "R %p %p\n", v, (void *)l);
	return l;
}

#define lreadl(a) fradl((void *)(a))

#else
#define lwritel(a,b) write32((void *)(b), (unsigned long)(a))
#define lreadl(a) read32((void *)(a))
#endif

/* not sure where we want this so ... */
static unsigned long get_lcd_clk(void)
{
	u32 pclk, sclk;
	unsigned int sel;
	unsigned int ratio;

	/*
	 * CLK_SRC_DISP10
	 * CLKMUX_FIMD1 [4]
	 * 0: SCLK_RPLL
	 * 1: SCLK_SPLL
	 */
	sel = lreadl(&exynos_clock->clk_src_disp10);
	sel &= (1 << 4);

	if (sel){
		sclk = get_pll_clk(SPLL);
	} else {
		sclk = get_pll_clk(RPLL);
	}

	/*
	 * CLK_DIV_DISP10
	 * FIMD1_RATIO [3:0]
	 */
	ratio = lreadl(&exynos_clock->clk_div_disp10);
	ratio = ratio & 0xf;

	pclk = sclk / (ratio + 1);

	return pclk;
}

static void exynos_fimd_set_dualrgb(vidinfo_t *vid, unsigned int enabled)
{
	unsigned int cfg = 0;
	printk(BIOS_SPEW, "%s %s\n", __func__, enabled ? "enabled" : "not enabled");
	if (enabled) {
		cfg = EXYNOS_DUALRGB_BYPASS_DUAL | EXYNOS_DUALRGB_LINESPLIT |
			EXYNOS_DUALRGB_VDEN_EN_ENABLE;

		/* in case of Line Split mode, MAIN_CNT doesn't need to be set. */
		cfg |= EXYNOS_DUALRGB_SUB_CNT(vid->vl_col / 2) |
			EXYNOS_DUALRGB_MAIN_CNT(0);
	}

	lwritel(cfg, &FIMD_CTRL->dualrgb);
}

static void exynos_fimd_set_dp_clkcon(unsigned int enabled)
{
	unsigned int cfg = 0;

	if (enabled){
		cfg = EXYNOS_DP_CLK_ENABLE;
	}

	lwritel(cfg, &FIMD_CTRL->dp_mie_clkcon);
}

static void exynos_fimd_set_par(vidinfo_t *vid, unsigned int win_id)
{
	unsigned int cfg = 0;
	printk(BIOS_SPEW, "%s %d\n", __func__, win_id);
	/* set window control */
	cfg = lreadl(&FIMD_CTRL->wincon0 +
		    EXYNOS_WINCON(win_id));

	cfg &= ~(EXYNOS_WINCON_BITSWP_ENABLE | EXYNOS_WINCON_BYTESWP_ENABLE |
		 EXYNOS_WINCON_HAWSWP_ENABLE | EXYNOS_WINCON_WSWP_ENABLE |
		 EXYNOS_WINCON_BURSTLEN_MASK | EXYNOS_WINCON_BPPMODE_MASK |
		 EXYNOS_WINCON_INRGB_MASK | EXYNOS_WINCON_DATAPATH_MASK);

	/* DATAPATH is DMA */
	cfg |= EXYNOS_WINCON_DATAPATH_DMA;

	cfg |= EXYNOS_WINCON_HAWSWP_ENABLE;

	/* dma burst is 16 */
	cfg |= EXYNOS_WINCON_BURSTLEN_16WORD;

	cfg |= EXYNOS_WINCON_BPPMODE_16BPP_565;

	lwritel(cfg, &FIMD_CTRL->wincon0 +
	       EXYNOS_WINCON(win_id));

	/* set window position to x=0, y=0*/
	cfg = EXYNOS_VIDOSD_LEFT_X(0) | EXYNOS_VIDOSD_TOP_Y(0);
	lwritel(cfg, &FIMD_CTRL->vidosd0a +
	       EXYNOS_VIDOSD(win_id));

	cfg = EXYNOS_VIDOSD_RIGHT_X(vid->vl_col - 1) |
		EXYNOS_VIDOSD_BOTTOM_Y(vid->vl_row - 1) |
		EXYNOS_VIDOSD_RIGHT_X_E(1) |
		EXYNOS_VIDOSD_BOTTOM_Y_E(0);

	lwritel(cfg, &FIMD_CTRL->vidosd0b +
	       EXYNOS_VIDOSD(win_id));
	/* set window size for window0*/
	cfg = EXYNOS_VIDOSD_SIZE(vid->vl_col * vid->vl_row);
	lwritel(cfg, &FIMD_CTRL->vidosd0c +
	       EXYNOS_VIDOSD(win_id));
}

static void exynos_fimd_set_buffer_address(vidinfo_t *vid,
	void *screen_base, int win_id)
{
	u32 start_addr, end_addr;
	printk(BIOS_SPEW, "%s %d\n", __func__, win_id);
	start_addr = (u32)screen_base;
	end_addr = start_addr + ((vid->vl_col * ((1<<vid->vl_bpix) / 8)) *
				 vid->vl_row);

	lwritel(start_addr, &FIMD_CTRL->vidw00add0b0 +
	       EXYNOS_BUFFER_OFFSET(win_id));
	lwritel(end_addr, &FIMD_CTRL->vidw00add1b0 +
	       EXYNOS_BUFFER_OFFSET(win_id));
}

static void exynos_fimd_set_clock(vidinfo_t *vid)
{
	unsigned int cfg = 0, div = 0, remainder = 0, remainder_div;
	unsigned long pixel_clock;
	unsigned long long src_clock;
	printk(BIOS_SPEW, "%s\n", __func__);
	if (vid->dual_lcd_enabled) {
		pixel_clock = vid->vl_freq *
			(vid->vl_hspw + vid->vl_hfpd +
			 vid->vl_hbpd + vid->vl_col / 2) *
			(vid->vl_vspw + vid->vl_vfpd +
			 vid->vl_vbpd + vid->vl_row);
	} else if (vid->interface_mode == FIMD_CPU_INTERFACE) {
		pixel_clock = vid->vl_freq *
			vid->vl_width * vid->vl_height *
			(vid->cs_setup + vid->wr_setup +
			 vid->wr_act + vid->wr_hold + 1);
	} else {
		pixel_clock = vid->vl_freq *
			(vid->vl_hspw + vid->vl_hfpd +
			 vid->vl_hbpd + vid->vl_col) *
			(vid->vl_vspw + vid->vl_vfpd +
			 vid->vl_vbpd + vid->vl_row);
	}
	printk(BIOS_SPEW, "Pixel clock is %lx\n", pixel_clock);

	cfg = lreadl(&FIMD_CTRL->vidcon0);
	cfg &= ~(EXYNOS_VIDCON0_CLKSEL_MASK | EXYNOS_VIDCON0_CLKVALUP_MASK |
		 EXYNOS_VIDCON0_CLKVAL_F(0xFF) | EXYNOS_VIDCON0_VCLKEN_MASK |
		 EXYNOS_VIDCON0_CLKDIR_MASK);
	cfg |= (EXYNOS_VIDCON0_CLKSEL_SCLK | EXYNOS_VIDCON0_CLKVALUP_ALWAYS |
		EXYNOS_VIDCON0_VCLKEN_NORMAL | EXYNOS_VIDCON0_CLKDIR_DIVIDED);

	src_clock = (unsigned long long)get_lcd_clk();

	/* get quotient and remainder. */
	remainder = src_clock % pixel_clock;
	src_clock /= pixel_clock;

	div = src_clock;

	remainder *= 10;
	remainder_div = remainder / pixel_clock;

	/* round about one places of decimals. */
	if (remainder_div >= 5)
		div++;

	/* in case of dual lcd mode. */
	if (vid->dual_lcd_enabled)
		div--;

	cfg |= EXYNOS_VIDCON0_CLKVAL_F(div - 1);
	lwritel(cfg, &FIMD_CTRL->vidcon0);
}

void exynos_set_trigger(void)
{
	unsigned int cfg = 0;
	printk(BIOS_SPEW, "%s\n", __func__);
	cfg = lreadl(&FIMD_CTRL->trigcon);

	cfg |= (EXYNOS_I80SOFT_TRIG_EN | EXYNOS_I80START_TRIG);

	lwritel(cfg, &FIMD_CTRL->trigcon);
}

int exynos_is_i80_frame_done(void)
{
	unsigned int cfg = 0;
	int status;
	printk(BIOS_SPEW, "%s\n", __func__);
	cfg = lreadl(&FIMD_CTRL->trigcon);

	/* frame done func is valid only when TRIMODE[0] is set to 1. */
	status = (cfg & EXYNOS_I80STATUS_TRIG_DONE) ==
		EXYNOS_I80STATUS_TRIG_DONE;

	return status;
}

static void exynos_fimd_lcd_on(void)
{
	unsigned int cfg = 0;

	printk(BIOS_SPEW, "%s\n", __func__);
	/* display on */
	cfg = lreadl(&FIMD_CTRL->vidcon0);
	cfg |= (EXYNOS_VIDCON0_ENVID_ENABLE | EXYNOS_VIDCON0_ENVID_F_ENABLE);
	lwritel(cfg, &FIMD_CTRL->vidcon0);
}

static void exynos_fimd_window_on(unsigned int win_id)
{
	unsigned int cfg = 0;
	printk(BIOS_SPEW, "%s %d\n", __func__, win_id);
	/* enable window */
	cfg = lreadl(&FIMD_CTRL->wincon0 +
		    EXYNOS_WINCON(win_id));
	cfg |= EXYNOS_WINCON_ENWIN_ENABLE;
	lwritel(cfg, &FIMD_CTRL->wincon0 +
	       EXYNOS_WINCON(win_id));

	cfg = lreadl(&FIMD_CTRL->winshmap);
	cfg |= EXYNOS_WINSHMAP_CH_ENABLE(win_id);
	lwritel(cfg, &FIMD_CTRL->winshmap);
	cfg = lreadl(&FIMD_CTRL->winshmap);
}

void exynos_fimd_lcd_off(void)
{
	unsigned int cfg = 0;
	printk(BIOS_SPEW, "%s\n", __func__);

	cfg = lreadl(&FIMD_CTRL->vidcon0);
	cfg &= (EXYNOS_VIDCON0_ENVID_DISABLE | EXYNOS_VIDCON0_ENVID_F_DISABLE);
	lwritel(cfg, &FIMD_CTRL->vidcon0);
}

void exynos_fimd_window_off(unsigned int win_id)
{
	unsigned int cfg = 0;
	printk(BIOS_SPEW, "%s %d\n", __func__, win_id);

	cfg = lreadl(&FIMD_CTRL->wincon0 +
		    EXYNOS_WINCON(win_id));
	cfg &= EXYNOS_WINCON_ENWIN_DISABLE;
	lwritel(cfg, &FIMD_CTRL->wincon0 +
	       EXYNOS_WINCON(win_id));

	cfg = lreadl(&FIMD_CTRL->winshmap);
	cfg &= ~EXYNOS_WINSHMAP_CH_DISABLE(win_id);
	lwritel(cfg, &FIMD_CTRL->winshmap);
}

static void exynos5_set_system_display(void)
{
	unsigned int cfg = 0;

	/*
	 * system register path set
	 * 0: MIE/MDNIE
	 * 1: FIMD Bypass
	 */
	cfg = lreadl(&exynos_sysreg->disp1blk_cfg);
	cfg |= (1 << 15);
	lwritel(cfg, &exynos_sysreg->disp1blk_cfg);
}

void exynos_fimd_lcd_init(vidinfo_t *vid)
{
	unsigned int cfg = 0, rgb_mode;
	struct exynos_fb *fimd;

	fimd = (void *)(FIMD_CTRL_ADDR + EXYNOS5_LCD_IF_BASE_OFFSET);

	printk(BIOS_SPEW, "%s\n", __func__);
	exynos5_set_system_display();

	rgb_mode = vid->rgb_mode;

	if (vid->interface_mode == FIMD_RGB_INTERFACE) {
		printk(BIOS_SPEW, "%s FIMD_RGB_INTERFACE\n", __func__);

		cfg |= EXYNOS_VIDCON0_VIDOUT_RGB;
		lwritel(cfg, &FIMD_CTRL->vidcon0);

		cfg = lreadl(&FIMD_CTRL->vidcon2);
		cfg &= ~(EXYNOS_VIDCON2_WB_MASK |
			EXYNOS_VIDCON2_TVFORMATSEL_MASK |
			EXYNOS_VIDCON2_TVFORMATSEL_YUV_MASK);
		cfg |= EXYNOS_VIDCON2_WB_DISABLE;
		lwritel(cfg, &FIMD_CTRL->vidcon2);

		/* set polarity */
		cfg = 0;
		if (!vid->vl_clkp)
			cfg |= EXYNOS_VIDCON1_IVCLK_RISING_EDGE;
		if (!vid->vl_hsp)
			cfg |= EXYNOS_VIDCON1_IHSYNC_INVERT;
		if (!vid->vl_vsp)
			cfg |= EXYNOS_VIDCON1_IVSYNC_INVERT;
		if (!vid->vl_dp)
			cfg |= EXYNOS_VIDCON1_IVDEN_INVERT;

		lwritel(cfg, &fimd->vidcon1);

		/* set timing */
		cfg = EXYNOS_VIDTCON0_VFPD(vid->vl_vfpd - 1);
		cfg |= EXYNOS_VIDTCON0_VBPD(vid->vl_vbpd - 1);
		cfg |= EXYNOS_VIDTCON0_VSPW(vid->vl_vspw - 1);
		lwritel(cfg, &fimd->vidtcon0);

		cfg = EXYNOS_VIDTCON1_HFPD(vid->vl_hfpd - 1);
		cfg |= EXYNOS_VIDTCON1_HBPD(vid->vl_hbpd - 1);
		cfg |= EXYNOS_VIDTCON1_HSPW(vid->vl_hspw - 1);

		lwritel(cfg, &fimd->vidtcon1);

		/* set lcd size */
		cfg = EXYNOS_VIDTCON2_HOZVAL(vid->vl_col - 1) |
			EXYNOS_VIDTCON2_LINEVAL(vid->vl_row - 1) |
			EXYNOS_VIDTCON2_HOZVAL_E(vid->vl_col - 1) |
			EXYNOS_VIDTCON2_LINEVAL_E(vid->vl_row - 1);

		lwritel(cfg, &fimd->vidtcon2);
	}

	/* set display mode */
	cfg = lreadl(&FIMD_CTRL->vidcon0);
	cfg &= ~EXYNOS_VIDCON0_PNRMODE_MASK;
	cfg |= (rgb_mode << EXYNOS_VIDCON0_PNRMODE_SHIFT);
	lwritel(cfg, &FIMD_CTRL->vidcon0);

	/* set par */
	exynos_fimd_set_par(vid, vid->win_id);

	/* set memory address */
	exynos_fimd_set_buffer_address(vid, vid->screen_base, vid->win_id);

	/* set buffer size */
	cfg = EXYNOS_VIDADDR_PAGEWIDTH(vid->vl_col * (1<<vid->vl_bpix) / 8) |
		EXYNOS_VIDADDR_PAGEWIDTH_E(vid->vl_col * (1<<vid->vl_bpix) / 8) |
		EXYNOS_VIDADDR_OFFSIZE(0) |
		EXYNOS_VIDADDR_OFFSIZE_E(0);

	lwritel(cfg, &FIMD_CTRL->vidw00add2 +
					EXYNOS_BUFFER_SIZE(vid->win_id));

	/* set clock */
	exynos_fimd_set_clock(vid);

	/* set rgb mode to dual lcd. */
	exynos_fimd_set_dualrgb(vid, vid->dual_lcd_enabled);

	/* display on */
	exynos_fimd_lcd_on();

	/* window on */
	exynos_fimd_window_on(vid->win_id);

	exynos_fimd_set_dp_clkcon(vid->dp_enabled);
	exynos5_set_system_display();
	printk(BIOS_SPEW, "%s: done\n", __func__);
}

unsigned long exynos_fimd_calc_fbsize(vidinfo_t *vid)
{
	printk(BIOS_SPEW, "%s\n", __func__);
	return vid->vl_col * vid->vl_row * ((1<<vid->vl_bpix) / 8);
}

void exynos_fimd_lcd_disable(void)
{
	int i;
	printk(BIOS_SPEW, "%s\n", __func__);

	for (i = 0; i < 4; i++)
		exynos_fimd_window_off(i);
}
