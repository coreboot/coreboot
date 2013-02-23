/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#ifndef DRIVINGCLKPHASEDATA_H
#define DRIVINGCLKPHASEDATA_H

//extern u8 DDR2_DQSA_Driving_Table[4] ;
//extern u8 DDR2_DQSB_Driving_Table[2] ;

//extern u8 DDR2_DQA_Driving_Table[4] ;
//extern u8 DDR2_DQB_Driving_Table[2] ;

//extern u8 DDR2_CSA_Driving_Table_x8[4] ;
//extern u8 DDR2_CSB_Driving_Table_x8[2] ;
//extern u8 DDR2_CSA_Driving_Table_x16[4];
//extern u8 DDR2_CSB_Driving_Table_x16[2];

#define  MA_Table   3
//extern u8 DDR2_MAA_Driving_Table[MA_Table][4];
//extern u8 DDR2_MAB_Driving_Table[MA_Table][2];

//extern u8 DDR2_DCLKA_Driving_Table[4] ;
//extern u8 DDR2_DCLKB_Driving_Table[4];

#define DUTY_CYCLE_FREQ_NUM   6
#define DUTY_CYCLE_REG_NUM     3
//extern u8 ChA_Duty_Control_DDR2[DUTY_CYCLE_REG_NUM][DUTY_CYCLE_FREQ_NUM];
//extern u8 ChB_Duty_Control_DDR2[DUTY_CYCLE_REG_NUM][DUTY_CYCLE_FREQ_NUM];

#define Clk_Phase_Table_DDR2_Width	  6
//extern u8 DDR2_ChA_Clk_Phase_Table_1R[3][Clk_Phase_Table_DDR2_Width];
//extern u8 DDR2_ChB_Clk_Phase_Table_1R[3][Clk_Phase_Table_DDR2_Width];
//extern u8 DDR2_ChA_Clk_Phase_Table_2R[3][Clk_Phase_Table_DDR2_Width];

#define WrtData_REG_NUM        4
#define WrtData_FREQ_NUM      6
//extern u8 DDR2_ChA_WrtData_Phase_Table[WrtData_REG_NUM ][WrtData_FREQ_NUM];
//extern u8 DDR2_ChB_WrtData_Phase_Table[WrtData_REG_NUM ][WrtData_FREQ_NUM];

#define DQ_DQS_Delay_Table_Width  4
//extern u8 DDR2_CHA_DQ_DQS_Delay_Table[4][DQ_DQS_Delay_Table_Width];
//extern u8 DDR2_CHB_DQ_DQS_Delay_Table[4][DQ_DQS_Delay_Table_Width];

#define DQS_INPUT_CAPTURE_REG_NUM            3
#define DQS_INPUT_CAPTURE_FREQ_NUM             6
//extern u8 DDR2_ChA_DQS_Input_Capture_Tbl[DQS_INPUT_CAPTURE_REG_NUM ][DQS_INPUT_CAPTURE_FREQ_NUM];
//extern u8 DDR2_ChB_DQS_Input_Capture_Tbl[DQS_INPUT_CAPTURE_REG_NUM ][DQS_INPUT_CAPTURE_FREQ_NUM];

//extern u8 Fixed_DQSA_1_2_Rank_Table[4][2];
//extern u8 Fixed_DQSA_3_4_Rank_Table[4][2];

//extern u8 Fixed_DQSB_1_2_Rank_Table[4][2];
//extern u8 Fixed_DQSB_3_4_Rank_Table[4][2];
#endif				/* DRIVINGCLKPHASEDATA_H */
