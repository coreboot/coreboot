/*
 * This file is part of the coreboot project.
 *
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

/* Register map for Exynos5 FIMD */

#ifndef CPU_SAMSUNG_EXYNOS5420_FIMD_H
#define CPU_SAMSUNG_EXYNOS5420_FIMD_H

#include <soc/cpu.h>

/* FIMD register map */
struct exynos5_fimd {
	/* This is an incomplete list. Add registers as and when required */
	u32 vidcon0;
	u8 res1[0x1c];
	u32 wincon0;
	u32 wincon1;
	u32 wincon2;
	u32 wincon3;
	u32 wincon4;
	u32 shadowcon;
	u8 res2[0x8];
	u32 vidosd0a;
	u32 vidosd0b;
	u32 vidosd0c;
	u8 res3[0x54];
	u32 vidw00add0b0;
	u8 res4[0x2c];
	u32 vidw00add1b0;
	u8 res5[0x2c];
	u32 vidw00add2;
	u8 res6[0x3c];
	u32 w1keycon0;
	u32 w1keycon1;
	u32 w2keycon0;
	u32 w2keycon1;
	u32 w3keycon0;
	u32 w3keycon1;
	u32 w4keycon0;
	u32 w4keycon1;
	u8 res7[0x20];
	u32 win0map;
	u8 res8[0xdc];
	u32 blendcon;
	u8 res9[0x18];
	u32 dpclkcon;
};
check_member(exynos5_fimd, dpclkcon, 0x27c);

#define W0_SHADOW_PROTECT	(0x1 << 10)
#define COMPKEY_F		0xffffff
#define ENVID_F_ON		(0x1 << 0)
#define ENVID_ON		(0x1 << 1)
#define CLKVAL_F		0xb
#define CLKVAL_F_OFFSET		6

/*
 * Structure containing display panel specific data for FIMD
 */
struct exynos5_fimd_panel {
	unsigned int is_dp:1;		/* Display Panel interface is eDP */
	unsigned int is_mipi:1;		/* Display Panel interface is MIPI */
	unsigned int fixvclk:2;	/* VCLK hold scheme at data underflow */

	/*
	 * Polarity of the VCLK active edge
	 *	0-falling
	 *	1-rising
	 */
	unsigned int ivclk:1;
	unsigned int clkval_f;		/* Divider to create pixel clock */

	unsigned int upper_margin;	/* Vertical Backporch */
	unsigned int lower_margin;	/* Vertical frontporch */
	unsigned int vsync;		/* Vertical Sync Pulse Width */
	unsigned int left_margin;	/* Horizontal Backporch */
	unsigned int right_margin;	/* Horizontal Frontporch */
	unsigned int hsync;		/* Horizontal Sync Pulse Width */
	unsigned int xres;		/* X Resolution */
	unsigned int yres;		/* Y Resolution */
};

/* LCDIF Register Map */
struct exynos5_disp_ctrl {
	u32 vidout_con;
	u32 vidcon1;
	u8 res1[0x8];
	u32 vidtcon0;
	u32 vidtcon1;
	u32 vidtcon2;
	u32 vidtcon3;
	u8 res2[0x184];
	u32 trigcon;
};
check_member(exynos5_disp_ctrl, trigcon, 0x1a4);

#define VCLK_RISING_EDGE		(1 << 7)
#define VCLK_RUNNING			(1 << 9)

#define CHANNEL0_EN			(1 << 0)

#define VSYNC_PULSE_WIDTH_VAL		0x3
#define VSYNC_PULSE_WIDTH_OFFSET	0
#define V_FRONT_PORCH_VAL		0x3
#define V_FRONT_PORCH_OFFSET		8
#define V_BACK_PORCH_VAL		0x3
#define V_BACK_PORCH_OFFSET		16

#define HSYNC_PULSE_WIDTH_VAL		0x3
#define HSYNC_PULSE_WIDTH_OFFSET	0
#define H_FRONT_PORCH_VAL		0x3
#define H_FRONT_PORCH_OFFSET		8
#define H_BACK_PORCH_VAL		0x3
#define H_BACK_PORCH_OFFSET		16

#define HOZVAL_OFFSET	0
#define LINEVAL_OFFSET	11

#define BPPMODE_F_RGB_16BIT_565		0x5
#define BPPMODE_F_OFFSET		2
#define ENWIN_F_ENABLE			(1 << 0)
#define HALF_WORD_SWAP_EN		(1 << 16)

#define OSD_RIGHTBOTX_F_OFFSET		11
#define OSD_RIGHTBOTY_F_OFFSET		0

#define FIMD_CTRL_ADDR			0x14400000
#define FIMD_CTRL			((struct exynos_fb *)FIMD_CTRL_ADDR)

/* from u-boot fb.h. It needs to be merged with these dp structs maybe. */
enum {
	FIMD_RGB_INTERFACE = 1,
	FIMD_CPU_INTERFACE = 2,
};

enum exynos_fb_rgb_mode_t {
	MODE_RGB_P = 0,
	MODE_BGR_P = 1,
	MODE_RGB_S = 2,
	MODE_BGR_S = 3,
};

typedef struct vidinfo {
	u16 vl_col;		/* Number of columns (i.e. 640) */
	u16 vl_row;		/* Number of rows (i.e. 480) */
	u16 vl_width;	/* Width of display area in millimeters */
	u16 vl_height;	/* Height of display area in millimeters */

	/* LCD configuration register */
	u8 vl_freq;		/* Frequency */
	u8 vl_clkp;		/* Clock polarity */
	u8 vl_oep;		/* Output Enable polarity */
	u8 vl_hsp;		/* Horizontal Sync polarity */
	u8 vl_vsp;		/* Vertical Sync polarity */
	u8 vl_dp;		/* Data polarity */
	u8 vl_bpix;		/* Bits per pixel */

	/* Horizontal control register. Timing from data sheet */
	u8 vl_hspw;		/* Horz sync pulse width */
	u8 vl_hfpd;		/* Wait before of line */
	u8 vl_hbpd;		/* Wait end of line */

	/* Vertical control register. */
	u8	vl_vspw;	/* Vertical sync pulse width */
	u8	vl_vfpd;	/* Wait before of frame */
	u8	vl_vbpd;	/* Wait end of frame */
	u8  vl_cmd_allow_len; /* Wait end of frame */

	unsigned int win_id;
	unsigned int init_delay;
	unsigned int power_on_delay;
	unsigned int reset_delay;
	unsigned int interface_mode;
	unsigned int mipi_enabled;
	unsigned int dp_enabled;
	unsigned int cs_setup;
	unsigned int wr_setup;
	unsigned int wr_act;
	unsigned int wr_hold;
	unsigned int rgb_mode;
	unsigned int resolution;

	/* parent clock name(MPLL, EPLL or VPLL) */
	unsigned int pclk_name;
	/* ratio value for source clock from parent clock. */
	unsigned int sclk_div;

	unsigned int dual_lcd_enabled;
	void *screen_base;
	void	*cmap;		/* Points at 8 to 16 bit conversion map. */
} vidinfo_t;

/* fimd.c */
void exynos_set_trigger(void);
int exynos_is_i80_frame_done(void);
void exynos_fimd_lcd_off(void);
void exynos_fimd_window_off(unsigned int win_id);
unsigned long exynos_fimd_calc_fbsize(vidinfo_t *vid);
void exynos_fimd_lcd_disable(void);
void exynos_fimd_lcd_init(vidinfo_t *vid);

#endif
