/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Arthur Heymans <arthur@aheymans.xyz>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <stdint.h>
#include "x4x.h"

const struct dll_setting default_ddr2_667_ctrl[7] = {
	/* tap  pi db  delay coarse*/
	{13, 0, 1, 0, 0, 0}, /* clkset0 */
	{4,  1, 0, 0, 0, 0}, /* ctrl0 */
	{13, 0, 1, 0, 0, 0}, /* clkset1 */
	{4,  5, 0, 0, 0, 0}, /* cmd */
	{4,  1, 0, 0, 0, 0}, /* ctrl1 */
	{4,  1, 0, 0, 0, 0}, /* ctrl2 */
	{4,  1, 0, 0, 0, 0}, /* ctrl3 */
};

const struct dll_setting default_ddr2_667_dqs[TOTAL_BYTELANES] = {
	{1,  5, 1, 1, 1, 0},
	{1,  6, 1, 1, 1, 0},
	{2,  0, 1, 1, 1, 0},
	{2,  1, 1, 1, 1, 0},
	{2,  1, 1, 1, 1, 0},
	{14, 6, 1, 0, 0, 0},
	{14, 3, 1, 0, 0, 0},
	{14, 0, 1, 0, 0, 0},
};

const struct dll_setting default_ddr2_667_dq[TOTAL_BYTELANES] = {
	{9,  0, 0, 0, 1, 0},
	{9,  1, 0, 0, 1, 0},
	{9,  2, 0, 0, 1, 0},
	{9,  2, 0, 0, 1, 0},
	{9,  1, 0, 0, 1, 0},
	{6,  4, 0, 0, 1, 0},
	{6,  2, 0, 0, 1, 0},
	{5,  4, 0, 0, 1, 0}
};

const struct dll_setting default_ddr2_800_ctrl[7] = {
	/* tap  pi db  delay coarse */
	{11, 5, 1, 0, 0, 0},
	{0,  5, 1, 1, 0, 0},
	{11, 5, 1, 0, 0, 0},
	{1,  4, 1, 1, 0, 0},
	{0,  5, 1, 1, 0, 0},
	{0,  5, 1, 1, 0, 0},
	{0,  5, 1, 1, 0, 0},
};

const struct dll_setting default_ddr2_800_dqs[TOTAL_BYTELANES] = {
	{2,  5, 1, 1, 1, 0},
	{2,  6, 1, 1, 1, 0},
	{3,  0, 1, 1, 1, 0},
	{3,  0, 1, 1, 1, 0},
	{3,  3, 1, 1, 1, 0},
	{2,  0, 1, 1, 1, 0},
	{1,  3, 1, 1, 1, 0},
	{0,  3, 1, 1, 1, 0},
};

const struct dll_setting default_ddr2_800_dq[TOTAL_BYTELANES] = {
	{9,  3, 0, 0, 1, 0},
	{9,  4, 0, 0, 1, 0},
	{9,  5, 0, 0, 1, 0},
	{9,  6, 0, 0, 1, 0},
	{10, 0, 0, 0, 1, 0},
	{8,  1, 0, 0, 1, 0},
	{7,  5, 0, 0, 1, 0},
	{6,  2, 0, 0, 1, 0}
};

const struct dll_setting default_ddr3_800_ctrl[2][7] = {
	{ /* 1N */
		/* tap  pi db(2)  delay coarse */
		{8, 2, 0, 0, 0, 0},
		{8, 4, 0, 0, 0, 0},
		{9, 5, 0, 0, 0, 0},
		{6, 1, 0, 0, 0, 0},
		{8, 4, 0, 0, 0, 0},
		{10, 0, 0, 0, 0, 0},
		{10, 0, 0, 0, 0, 0}, },
	{ /* 2N */
		{2, 2, 1, 1, 0, 0},
		{2, 4, 1, 1, 0, 0},
		{3, 5, 0, 0, 0, 0},
		{3, 2, 1, 1, 0, 0},
		{2, 4, 1, 1, 0, 0},
		{3, 6, 0, 0, 0, 0},
		{3, 6, 0, 0, 0, 0}, }
};

const struct dll_setting default_ddr3_800_dqs[2][TOTAL_BYTELANES] = {
	{ /* 1N */
		{12, 0, 1, 0, 0, 0},
		{1, 1, 1, 1, 1, 0},
		{2, 4, 1, 1, 1, 0},
		{3, 5, 0, 0, 1, 0},
		{4, 3, 0, 0, 1, 0},
		{5, 2, 0, 0, 1, 0},
		{6, 1, 0, 0, 1, 0},
		{6, 4, 0, 0, 1, 0}, },
	{ /* 2N */
		{5, 6, 0, 0, 0, 0},
		{8, 0, 0, 0, 0, 0},
		{9, 4, 0, 0, 0, 0},
		{10, 4, 1, 0, 0, 0},
		{11, 3, 1, 0, 0, 0},
		{12, 1, 1, 0, 0, 0},
		{0, 1, 1, 1, 1, 0},
		{0, 3, 1, 1, 1, 0}, }
};

const struct dll_setting default_ddr3_800_dq[2][TOTAL_BYTELANES] = {
	{ /* 1N */
		{4, 1, 0, 0, 1, 0},
		{6, 4, 0, 0, 1, 0},
		{8, 1, 0, 0, 1, 0},
		{8, 6, 0, 0, 1, 0},
		{9, 5, 0, 0, 1, 0},
		{10, 2, 0, 0, 1, 0},
		{10, 6, 1, 0, 1, 0},
		{11, 4, 1, 0, 1, 0} },
	{ /* 2N */
		{11, 0, 1, 0, 0, 0},
		{0, 3, 1, 1, 1, 0},
		{2, 1, 1, 1, 1, 0},
		{2, 5, 1, 1, 1, 0},
		{3, 5, 0, 0, 1, 0},
		{4, 2, 0, 0, 1, 0},
		{4, 6, 0, 0, 1, 0},
		{5, 4, 0, 0, 1, 0}, }
};

const struct dll_setting default_ddr3_1067_ctrl[2][7] = {
	{ /* 1N */
		{8, 5, 0, 0, 0, 0},
		{7, 6, 0, 0, 0, 0},
		{10, 2, 1, 0, 0, 0},
		{4, 4, 0, 0, 0, 0},
		{7, 6, 0, 0, 0, 0},
		{9, 2, 1, 0, 0, 0},
		{9, 2, 1, 0, 0, 0}, },
	{ /* 2N */
		{1, 5, 1, 1, 0, 0},
		{0, 6, 1, 1, 0, 0},
		{3, 2, 0, 0, 0, 0},
		{2, 6, 1, 1, 0, 0},
		{0, 6, 1, 1, 0, 0},
		{2, 2, 1, 1, 0, 0},
		{2, 2, 1, 1, 0, 0}, }
};

const struct dll_setting default_ddr3_1067_dqs[2][TOTAL_BYTELANES] = {
	{ /* 1N */
		{2, 5, 1, 1, 1, 0},
		{5, 1, 0, 0, 1, 0},
		{6, 6, 0, 0, 1, 0},
		{8, 0, 0, 0, 1, 0},
		{8, 6, 0, 0, 1, 0},
		{9, 6, 1, 0, 1, 0},
		{10, 6, 1, 0, 1, 0},
		{0, 1, 1, 1, 0, 1}, },
	{ /* 2N */
		{6, 4, 0, 0, 0, 0},
		{9, 1, 1, 0, 0, 0},
		{10, 6, 1, 0, 0, 0},
		{1, 0, 1, 1, 1, 0},
		{1, 6, 1, 1, 1, 0},
		{2, 5, 1, 1, 1, 0},
		{3, 5, 0, 0, 1, 0},
		{4, 1, 0, 0, 1, 0},
	}
};

const struct dll_setting default_ddr3_1067_dq[2][TOTAL_BYTELANES] = {
	{ /* 1N */
		{6, 5, 0, 0, 1, 0},
		{9, 3, 1, 0, 1, 0},
		{0, 2, 1, 1, 0, 1},
		{1, 0, 1, 1, 0, 1},
		{2, 0, 1, 1, 0, 1},
		{2, 5, 1, 1, 0, 1},
		{3, 2, 0, 0, 0, 1},
		{4, 1, 0, 0, 0, 1}, },
	{ /* 2N */
		{10, 5, 1, 0, 0, 0},
		{2, 3, 1, 1, 1, 0},
		{4, 1, 0, 0, 1, 0},
		{5, 0, 0, 0, 1, 0},
		{6, 0, 0, 0, 1, 0},
		{6, 5, 0, 0, 1, 0},
		{7, 2, 0, 0, 1, 0},
		{8, 1, 0, 0, 1, 0},
	}
};

const struct dll_setting default_ddr3_1333_ctrl[2][7] = {
	{ /* 1N */
		{8, 5, 0, 0, 0, 0},
		{9, 0, 1, 0, 0, 0},
		{10, 2, 1, 0, 0, 0},
		{0, 0, 1, 1, 0, 0},
		{9, 0, 1, 0, 0, 0},
		{10, 4, 1, 0, 0, 0},
		{10, 4, 1, 0, 0, 0}, },
	{ /* 2N */
		{1, 6, 1, 1, 0, 0},
		{2, 2, 1, 1, 0, 0},
		{4, 2, 0, 0, 0, 0},
		{3, 1, 1, 1, 0, 0},
		{2, 2, 1, 1, 0, 0},
		{4, 5, 0, 0, 0, 0},
		{4, 5, 0, 0, 0, 0}, }
};

const struct dll_setting default_ddr3_1333_dqs[2][TOTAL_BYTELANES] = {
	{ /* 1N */
		{2, 4, 1, 1, 1, 0},
		{5, 1, 0, 0, 1, 0},
		{6, 6, 0, 0, 1, 0},
		{8, 0, 0, 0, 1, 0},
		{8, 6, 0, 0, 1, 0},
		{9, 5, 1, 0, 1, 0},
		{10, 6, 1, 0, 1, 0},
		{0, 1, 1, 1, 0, 1}, },
	{ /* 2N */
		{10, 4, 0, 0, 0, 0},
		{0, 3, 1, 1, 1, 0},
		{3, 2, 1, 1, 1, 0},
		{5, 0, 0, 0, 1, 0},
		{6, 1, 0, 0, 1, 0},
		{7, 4, 0, 0, 1, 0},
		{9, 2, 0, 0, 1, 0},
		{9, 6, 0, 0, 1, 0}, }
};

const struct dll_setting default_ddr3_1333_dq[2][TOTAL_BYTELANES] = {
	{ /* 1N */
		{6, 5, 0, 0, 1, 0},
		{9, 3, 1, 0, 1, 0},
		{0, 2, 1, 1, 0, 1},
		{1, 0, 1, 1, 0, 1},
		{2, 0, 1, 1, 0, 1},
		{2, 5, 1, 1, 0, 1},
		{3, 2, 0, 0, 0, 1},
		{4, 1, 0, 0, 0, 1}, },
	{ /* 2N */
		{1, 3, 1, 1, 1, 0},
		{5, 6, 0, 0, 1, 0},
		{8, 5, 0, 0, 1, 0},
		{10, 2, 0, 0, 1, 0},
		{11, 1, 0, 0, 1, 0},
		{12, 3, 1, 0, 1, 0},
		{13, 6, 1, 0, 1, 0},
		{0, 3, 1, 1, 0, 1}, }
};

const u8 ddr3_emrs1_rtt_nom_config[16][4] = {  /* [Config][Rank] */
	{0x00, 0x00, 0x00, 0x00},	/* NC_NC */
	{0x11, 0x00, 0x00, 0x00},	/* 8S_NC */
	{0x11, 0x11, 0x00, 0x00},	/* 8D_NC */
	{0x11, 0x00, 0x00, 0x00},	/* 16S_NC */
	{0x00, 0x00, 0x11, 0x00},	/* NC_8S */
	{0x81, 0x00, 0x81, 0x00},	/* 8S_8S */
	{0x81, 0x81, 0x81, 0x00},	/* 8D_8S */
	{0x81, 0x00, 0x81, 0x00},	/* 16S_8S */
	{0x00, 0x00, 0x11, 0x11},	/* NC_8D */
	{0x81, 0x00, 0x81, 0x81},	/* 8S_8D */
	{0x81, 0x81, 0x81, 0x81},	/* 8D_8D */
	{0x81, 0x00, 0x81, 0x81},	/* 16S_8D */
	{0x00, 0x00, 0x11, 0x00},	/* NC_16S */
	{0x81, 0x00, 0x81, 0x00},	/* 8S_16S */
	{0x81, 0x81, 0x81, 0x00},	/* 8D_16S */
	{0x81, 0x00, 0x81, 0x00},	/* 16S_16S */
};

const u8 post_jedec_tab[3][4][2]= /* [FSB][DDR freq][17:13 or 12:8] */
{			/* FSB DDR */
	{{0x3, 0x5}, /* 800 667 */
	{0x3, 0x4}, /* 800 800 */
	},
	{{0x4, 0x8}, /* 1067 667 */
	{0x4, 0x6}, /* 1067 800 */
	{0x3, 0x5}, /* 1067 1066  */
	},
	{{0x5, 0x9}, /* 1333 667 */
	{0x4, 0x7}, /* 1333 800 */
	{0x4, 0x7}, /* 1333 1066 */
	{0x4, 0x7} /* 1333 1333 */
	},
};


const u32 ddr3_c2_tab[2][3][6][2] = { /* [n-mode][ddr3 freq][CAS][reg] */
		/* 115h[15:0]	117h[23:0] */
	{ /* 1N mode */
		{ /* DDR3 800MHz */
			{0x0189, 0x000aaa}, /* CAS = 5 */
			{0x0189, 0x101aaa}, /* CAS = 6 */
		},
		{ /* DDR3 1067MHz */
			{0x0000, 0x000000}, /* CAS = 5 - Not supported */
			{0x0089, 0x000bbb}, /* CAS = 6 */
			{0x0099, 0x101bbb}, /* CAS = 7 */
			{0x0099, 0x202bbb} /* CAS = 8 */
		},{ /* DDR3 1333 */
			{0x0000, 0x000000}, /* CAS = 5 - Not supported */
			{0x0000, 0x000000}, /* CAS = 6 - Not supported */
			{0x0000, 0x000000}, /* CAS = 7 - Not supported */
			{0x129a, 0x0078dc}, /* CAS = 8 */
			{0x028a, 0x0078dc}, /* CAS = 9 */
			{0x028a, 0x1088dc}, /* CAS = 10 */
		},
	},
	{ /* 2N mode */
		{ /* DDR3 800MHz */
			{0x0189, 0x000aaa}, /* CAS = 5 */
			{0x0189, 0x101aaa}, /* CAS = 6 */
			{0x0000, 0x000000}, /* CAS = 7 - Not supported */
			{0x0000, 0x000000} /* CAS = 8 - Not supported */
		},
		{ /* DDR3 1067 */
			{0x0000, 0x000000}, /* CAS = 5 - Not supported */
			{0x0089, 0x000bbb}, /* CAS = 6 */
			{0x0099, 0x101bbb}, /* CAS = 7 */
			{0x0099, 0x202bbb} /* CAS = 8 */
		},{ /* DDR3 1333MHz */
			{0x0000, 0x000000}, /* CAS = 5 - Not supported  */
			{0x0000, 0x000000}, /* CAS = 6 - Not supported */
			{0x0000, 0x000000}, /* CAS = 7 - Not supported */
			{0x019a, 0x0078dc}, /* CAS = 8 */
			{0x019a, 0x1088dc}, /* CAS = 9 */
			{0x019a, 0x2098dc}, /* CAS = 10 */
		},
	}
};

const u8 ddr3_c2_x264[3][6] = { /* [freq][cas] */
	/* DDR3 800MHz */
	{0x78, /* CAS = 5  */
	 0x89}, /* CAS = 6 */
	/* DDR3 1066 */
	{0x00, /* CAS = 5 - Not supported */
	 0xff, /* CAS = 6 */
	 0x8a, /* CAS = 7 */
	 0x9a}, /* CAS = 8 */
	/* DDR3 1333 */
	{0x00, /* CAS = 5 - Not supported */
	 0x00, /* CAS = 6 - Not supported */
	 0xff, /* CAS = 7 - Not supported */
	 0xff, /* CAS = 8 */
	 0xff, /* CAS = 9 */
	 0xff}, /* CAS = 10 */
};

const u16 ddr3_c2_x23c[3][6]={ /* [freq][cas] */
	/* DDR3 800MHz */
	{0x9bbb, /* CAS = 5 */
	 0x8bbb}, /* CAS = 6 */
	/* DDR3 1066MHz */
	{0x0000, /* CAS = 5 - Not supported */
	 0x9baa, /* CAS = 6 */
	 0x8caa, /* CAS = 7 */
	 0x7daa}, /* CAS = 8 */

	/* DDR3 1333MHz */
	{0x0000, /* CAS = 5 - Not supported */
	 0x0000, /* CAS = 6 - Not supported */
	 0x0000, /* CAS = 7 - Not supported */
	 0xaecb, /* CAS = 8 */
	 0x9fcb, /* CAS = 9 */
	 0x8fcb}, /* CAS = 10 */
};
