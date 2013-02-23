/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include "northbridge/via/vx800/driving_clk_phase_data.h"

// DQS Driving
// Reg0xE0, 0xE1
// According to #Bank to set DRAM DQS Driving
// #Bank    1     2     3     4     5     6     7     8
static const u8 DDR2_DQSA_Driving_Table[4] = { 0xEE, 0xEE, 0xEE, 0xEE };
static const u8 DDR2_DQSB_Driving_Table[2] = { 0xEE, 0xEE };

// DQ Driving
// Reg0xE2, 0xE3
// For DDR2: According to bank to set DRAM DQ Driving
static const u8 DDR2_DQA_Driving_Table[4] = { 0xAC, 0xAC, 0xAC, 0xAC };
static const u8 DDR2_DQB_Driving_Table[2] = { 0xCA, 0xCA };

// CS Driving
// Reg0xE4, 0xE5
// According to #Bank to set DRAM CS Driving
// DDR1 #Bank          1     2     3     4         5         6     7     8
static const u8 DDR2_CSA_Driving_Table_x8[4] =  { 0x44, 0x44, 0x44, 0x44 };
static const u8 DDR2_CSB_Driving_Table_x8[2] =  { 0x44, 0x44 };
static const u8 DDR2_CSA_Driving_Table_x16[4] = { 0x44, 0x44, 0x44, 0x44 };
static const u8 DDR2_CSB_Driving_Table_x16[2] = { 0x44, 0x44 };

// MAA Driving
// Reg0xE8, Reg0xE9
static const u8 DDR2_MAA_Driving_Table[MA_Table][5] = {
	// Chip number, 400,  533,  667   800    ;(SRAS, SCAS, SWE)RxE8
	{6,             0x86, 0x86, 0x86, 0x86}, // total MAA chips = 00 ~ 06
	{18,            0x86, 0x86, 0x86, 0x86}, // total MAA chips = 06 ~ 18
	{255,           0xDB, 0xDB, 0xDB, 0xDB}, // total MAA chips = 18 ~
};

static const u8 DDR2_MAB_Driving_Table[MA_Table][2] = {
	// Chip number, Value                    ;(SRAS, SCAS, SWE)RxE9
	{6,             0x86},			 // total MAB chips = 00 ~ 06
	{18,            0x86},			 // total MAB chips = 06 ~ 18
	{255,           0xDB},			 // total MAB chips = 18 ~
};

// DCLK Driving
// Reg0xE6, 0xE7
// For DDR2: According to #Freq to set DRAM DCLK Driving
//                              freq            400M, 533M, 667M, 800M
static const u8 DDR2_DCLKA_Driving_Table[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
static const u8 DDR2_DCLKB_Driving_Table[4] = { 0xFF, 0xFF, 0xFF, 0xFF };

/*
 * Duty cycle
 * Duty cycle Control for DQ/DQS/DDRCKG in ChA & ChB
 * D0F3RxEC/D0F3RxED/D0F3RxEE/D0F3RxEF
 * According to DRAM frequency to control Duty Cycle
 */
static const u8 ChA_Duty_Control_DDR2[DUTY_CYCLE_REG_NUM][DUTY_CYCLE_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0xEC, 0x00, 0x30, 0x30, 0x30, 0x30},	// 1Rank
	{0xEE, 0x0F, 0x40, 0x40, 0x00, 0x00},
	{0xEF, 0xCF, 0x00, 0x30, 0x30, 0x30},
};

static const u8 ChB_Duty_Control_DDR2[DUTY_CYCLE_REG_NUM][DUTY_CYCLE_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0xED, 0x00, 0x88, 0x88, 0x84, 0x88},	// 1Rank
	{0xEE, 0xF0, 0x00, 0x00, 0x00, 0x00},
	{0xEF, 0xFC, 0x00, 0x00, 0x00, 0x00},
};

/*
 * DRAM Clock Phase Control for FeedBack Mode
 * Modify NB Reg: Rx90[7]/Rx91/Rx92/Rx93/Rx94
 * Processing:
 *   1. Program VIA_NB3DRAM_REG90[7]=0b for FeedBack mode.
 *   2. Program clock phase value with ChA/B DCLK enable,
 *      VIA_NB3DRAM_REG91[7:3]=00b
 *   3. Check ChB rank #, if 0, VIA_NB3DRAM_REG91[7]=1b, to disable ChB DCLKO
 *      ChA DCLKO can't be disabled, so always program VIA_NB3DRAM_REG91[3]=0b.
 */
static const u8 DDR2_ChA_Clk_Phase_Table_1R[3][Clk_Phase_Table_DDR2_Width] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x91, 0xF8, 0x02, 0x01, 0x00, 0x07},	// 1Rank
	{0x92, 0xF8, 0x04, 0x03, 0x03, 0x02},
	{0x93, 0xF8, 0x06, 0x05, 0x04, 0x03},
};

static const u8 DDR2_ChB_Clk_Phase_Table_1R[3][Clk_Phase_Table_DDR2_Width] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x91, 0x0F, 0x20, 0x10, 0x00, 0x70},	// 1Rank
	{0x92, 0x0F, 0x40, 0x30, 0x30, 0x20},
	{0x93, 0x0F, 0x60, 0x50, 0x40, 0x30},
};

/* vt6413c */
#if 0
static const u8 DDR2_ChA_Clk_Phase_Table_2R[3][Clk_Phase_Table_DDR2_Width] = {
	//     (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x91, 0xF8, 0x04, 0x03, 0x04, 0x01 },	// 1Rank
	{0x92, 0xF8, 0x03, 0x06, 0x05, 0x04 },
	{0x93, 0xF8, 0x03, 0x07, 0x06, 0x05 },
};
#endif

/* vt6413d */
static const u8 DDR2_ChA_Clk_Phase_Table_2R[3][Clk_Phase_Table_DDR2_Width] = {
	//     (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x91, 0xF8, 0x02, 0x01, 0x00, 0x07},	// 1Rank
	{0x92, 0xF8, 0x04, 0x03, 0x03, 0x02},
	{0x93, 0xF8, 0x06, 0x05, 0x04, 0x03},
};

/*
 * DRAM Write Data phase control
 * Modify NB Reg: Rx74/Rx75/Rx76
 */
/* vt6413c */
#if 0
static const u8 DDR2_ChA_WrtData_Phase_Table[WrtData_REG_NUM ][WrtData_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x74, 0xF8, 0x03, 0x04, 0x05, 0x02 },	// 1Rank
	{0x75, 0xF8, 0x03, 0x04, 0x05, 0x02 },
	{0x76, 0x00, 0x10, 0x80, 0x00, 0x07 },
};
#endif

/* vt6413D */
static const u8 DDR2_ChA_WrtData_Phase_Table[WrtData_REG_NUM][WrtData_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x74, 0xF8, 0x01, 0x00, 0x00, 0x07},	// 1Rank
	{0x75, 0xF8, 0x01, 0x00, 0x00, 0x07},
	{0x76, 0x10, 0x80, 0x87, 0x07, 0x06},
	{0x8C, 0xFC, 0x03, 0x03, 0x03, 0x03},
};

#if 0
static const u8 DDR2_ChB_WrtData_Phase_Table[WrtData_REG_NUM ][WrtData_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x74, 0x8F, 0x30, 0x40, 0x30, 0x20 },	// 1Rank
	{0x75, 0x8F, 0x30, 0x40, 0x30, 0x20 },
	{0x8A, 0x00, 0x10, 0x80, 0x07, 0x07 },
};
#endif

/*
 * DQ/DQS Output Delay Control
 * Modify NB D0F3: RxF0/RxF1/RxF2/RxF3
 */
static const u8 DDR2_CHA_DQ_DQS_Delay_Table[4][DQ_DQS_Delay_Table_Width] = {
	//RxF0 RxF1  RxF2  RxF3
	{0x00, 0x00, 0x00, 0x00},	/* DDR400 */
	{0x00, 0x00, 0x00, 0x00},	/* DDR533 */
	{0x00, 0x00, 0x00, 0x00},	/* DDR667 */
	{0x00, 0x00, 0x00, 0x00},	/* DDR800 */
};

static const u8 DDR2_CHB_DQ_DQS_Delay_Table[4][DQ_DQS_Delay_Table_Width] = {
	//RxF4 RxF5  RxF6  RxF7
	{0x00, 0x00, 0x00, 0x00},	/* DDR400 */
	{0x00, 0x00, 0x00, 0x00},	/* DDR533 */
	{0x00, 0x00, 0x00, 0x00},	/* DDR667 */
	{0x00, 0x00, 0x00, 0x00},	/* DDR800 */
};

/*
 * DQ/DQS input Capture Control
 * modify NB D0F3_Reg:Rx78/Rx79/Rx7A/Rx7B
 */
/* vt6413C */
#if 0
static const u8 DDR2_ChA_DQS_Input_Capture_Tbl[DQS_INPUT_CAPTURE_REG_NUM ][DQS_INPUT_CAPTURE_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x78, 0x00, 0x83, 0x8D, 0x87, 0x83 },	// 1Rank
	{0x7A, 0xF0, 0x00, 0x00, 0x00, 0x00 },
	{0x7B, 0x00, 0x10, 0x30, 0x20, 0x10 }
};
#endif

/* vt6413D */
static const u8 DDR2_ChA_DQS_Input_Capture_Tbl[DQS_INPUT_CAPTURE_REG_NUM][DQS_INPUT_CAPTURE_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x78, 0xC0, 0x0D, 0x07, 0x03, 0x01},	// 1Rank
	{0x7A, 0xF0, 0x00, 0x00, 0x00, 0x00},
	{0x7B, 0x00, 0x34, 0x34, 0x20, 0x10}
};

static const u8 DDR2_ChB_DQS_Input_Capture_Tbl[DQS_INPUT_CAPTURE_REG_NUM][DQS_INPUT_CAPTURE_FREQ_NUM] = {
	//    (And NOT) DDR800 DDR667 DDR533 DDR400
	//Reg  Mask  Value Value Value Value
	{0x79, 0x00, 0x89, 0x89, 0x87, 0x83},	// 1Rank
	{0x7A, 0x0F, 0x00, 0x00, 0x00, 0x00},
	{0x8B, 0x00, 0x34, 0x34, 0x20, 0x10}
};

static const u8 Fixed_DQSA_1_2_Rank_Table[4][2] = {
	//Rx70 Rx71
	{0x00, 0x05},		/* DDR800 */
	{0x00, 0x06},		/* DDR667 */
	{0x00, 0x04},		/* DDR533 */
	{0x00, 0x05},		/* DDR400 */
};

static const u8 Fixed_DQSA_3_4_Rank_Table[4][2] = {
	//Rx70 Rx71
	{0x00, 0x04},		/* DDR800 */
	{0x00, 0x04},		/* DDR667 */
	{0x00, 0x03},		/* DDR533 */
	{0x00, 0x04},		/* DDR400 */
};
