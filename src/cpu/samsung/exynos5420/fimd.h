/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

/* Register map for Exynos5 FIMD */

#ifndef CPU_SAMSUNG_EXYNOS5420_FIMD_H
#define CPU_SAMSUNG_EXYNOS5420_FIMD_H

/* FIMD register map */
struct exynos5_fimd {
	/* This is an incomplete list. Add registers as and when required */
	unsigned int vidcon0;
	unsigned char res1[0x1c];
	unsigned int wincon0;
	unsigned int wincon1;
	unsigned int wincon2;
	unsigned int wincon3;
	unsigned int wincon4;
	unsigned int shadowcon;
	unsigned char res2[0x8];
	unsigned int vidosd0a;
	unsigned int vidosd0b;
	unsigned int vidosd0c;
	unsigned char res3[0x54];
	unsigned int vidw00add0b0;
	unsigned char res4[0x2c];
	unsigned int vidw00add1b0;
	unsigned char res5[0x2c];
	unsigned int vidw00add2;
	unsigned char res6[0x3c];
	unsigned int w1keycon0;
	unsigned int w1keycon1;
	unsigned int w2keycon0;
	unsigned int w2keycon1;
	unsigned int w3keycon0;
	unsigned int w3keycon1;
	unsigned int w4keycon0;
	unsigned int w4keycon1;
	unsigned char res7[0x20];
	unsigned int win0map;
	unsigned char res8[0xdc];
	unsigned int blendcon;
	unsigned char res9[0x18];
	unsigned int dpclkcon;
};

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
	unsigned int left_margin;	/* Horizantal Backporch */
	unsigned int right_margin;	/* Horizontal Frontporch */
	unsigned int hsync;		/* Horizontal Sync Pulse Width */
	unsigned int xres;		/* X Resolution */
	unsigned int yres;		/* Y Resopultion */
};

/* LCDIF Register Map */
struct exynos5_disp_ctrl {
	unsigned int vidout_con;
	unsigned int vidcon1;
	unsigned char res1[0x8];
	unsigned int vidtcon0;
	unsigned int vidtcon1;
	unsigned int vidtcon2;
	unsigned int vidtcon3;
	unsigned char res2[0x184];
	unsigned int trigcon;
};

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
#endif
