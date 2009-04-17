/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* CX700 has 48 bytes of scratch registers in D0F4 starting at Reg. 0xd0 */
#define	SCRATCH_REG_BASE		0xd0
#define	SCRATCH_RANK_0			0xd0
#define	SCRATCH_RANK_1			0xd1
#define	SCRATCH_RANK_2			0xd2
#define	SCRATCH_RANK_3			0xd3
#define	SCRATCH_DIMM_NUM		0xd4
#define	SCRATCH_RANK_NUM		0xd5
#define	SCRATCH_RANK_MAP		0xd6
#define	SCRATCH_DRAM_FREQ		0xd7
#define	SCRATCH_DRAM_NB_ODT		0xd8
#define	SCRATCH_RANK0_SIZE_REG		0xe0	/* RxE0~RxE3 */
#define	SCRATCH_RANK0_MA_REG		0xe4	/* RxE4~RxE7 */
#define	SCRATCH_CHA_DQSI_LOW_REG	0xe8
#define	SCRATCH_CHA_DQSI_HIGH_REG	0xe9
#define	SCRATCH_ChA_DQSI_REG		0xea
#define	SCRATCH_DRAM_256M_BIT		0xee
#define	SCRATCH_FLAGS			0xef

static const u8 Reg_Val[] = {
/*	REG,  VALUE */
	0x70, 0x33,
	0x71, 0x11,
	0x72, 0x33,
	0x73, 0x11,
	0x74, 0x20,
	0x75, 0x2e,
	0x76, 0x64,
	0x77, 0x00,
	0x78, 0x44,
	0x79, 0xaa,
	0x7a, 0x33,
	0x7b, 0xaa,
	0x7c, 0x00,
	0x7e, 0x33,
	0x7f, 0x33,
	0x80, 0x44,
	0x81, 0x44,
	0x82, 0x44,
	0x83, 0x02,
	0x50, 0x88,
	0x51, 0x7b,
	0x52, 0x6f,
	0x53, 0x88,
	0x54, 0x0e,
	0x55, 0x00,
	0x56, 0x00,
	0x59, 0x00,
	0x5d, 0x72,
	0x5e, 0x88,
	0x5f, 0xc7,
	0x68, 0x01,
};

/* Host registers initial value */
static const u8 Host_Reg_Val[] = {
/*	REG,  VALUE */
	0x60, 0xff,
	0x61, 0xff,
	0x62, 0x0f,
	0x63, 0xff,
	0x64, 0xff,
	0x65, 0x0f,
	0x66, 0xff,
	0x67, 0x30,
};

static const u8 Mem_Reg_Init[] = {
/*	REG,  AND,  OR	*/
	0x50, 0x11, 0x66,
	0x51, 0x11, 0x66,
	0x52, 0x00, 0x11,
	0x53, 0x00, 0x0f,
	0x54, 0x00, 0x00,
	0x55, 0x00, 0x00,
	0x56, 0x00, 0x00,
	0x57, 0x00, 0x00,
	0x60, 0x00, 0x00,
	0x62, 0xf7, 0x08,
	0x65, 0x00, 0xd9,
	0x66, 0x00, 0x80,
	0x67, 0x00, 0x50,	/* OR 0x00 ?? */
	0x69, 0xf0, 0x00,
	0x6a, 0x00, 0x00,
	0x6d, 0xcf, 0xc0,
	0x6e, 0xff, 0x80,
	0x75, 0x0f, 0x40,
	0x77, 0x00, 0x00,
	0x80, 0x00, 0x00,
	0x81, 0x00, 0x00,
	0x82, 0x00, 0x00,
	0x83, 0x00, 0x00,
	0x84, 0x00, 0x00,
	0x85, 0x00, 0x00,
	0x86, 0xff, 0x2c,	/* OR 0x28 if we don't want enable top 1M SM memory */
	0x40, 0x00, 0x00,
	0x7c, 0x00, 0x00,
	0x7e, 0x00, 0x00,
	0xa4, 0xfe, 0x00,
	0xb0, 0x7f, 0x80,
	0xb1, 0x00, 0xaa,
	0xb4, 0xfd, 0x02,
	0xb8, 0xfe, 0x00,
};

static const u8 Dram_Driving_ODT_CTRL[] = {
/*	REG,  VALUE */
	0xd6, 0xa8,
	0xd4, 0x80,
	0xd0, 0x88,
	0xd3, 0x01,
	0xd8, 0x00,
	0xda, 0x80,
};

#define	Rank0_ODT	0x00
#define	Rank1_ODT	0x01
#define	Rank2_ODT	0x02
#define	Rank3_ODT	0x03
#define	NA_ODT		0x00
#define	NB_ODT_75ohm	0x00
#define	NB_ODT_150ohm	0x01
#define	DDR2_ODT_75ohm  0x20
#define	DDR2_ODT_150ohm 0x40

static const u8 ODT_TBL[] = {
/*	RankMap, ODT Control Bits,						 	DRAM & NB ODT setting	*/
	0x01,	 ((NA_ODT << 6)    | (NA_ODT << 4)    | (NA_ODT << 2)    | Rank0_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x03, 	 ((NA_ODT << 6)    | (NA_ODT << 4)    | (Rank0_ODT << 2) | Rank1_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x04,	 ((NA_ODT << 6)    | (Rank2_ODT << 4) | (NA_ODT << 2)    | NA_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x05,	 ((NA_ODT << 6)    | (Rank0_ODT << 4) | (NA_ODT << 2)    | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
	0x07,	 ((NA_ODT << 6)    | (Rank0_ODT << 4) | (Rank2_ODT << 2) | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
	0x0c,	 ((Rank2_ODT << 6) | (Rank3_ODT << 4) | (NA_ODT << 2)    | NA_ODT),	(DDR2_ODT_150ohm | NB_ODT_75ohm),
	0x0d,	 ((Rank0_ODT << 6) | (Rank0_ODT << 4) | (NA_ODT << 2)    | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
	0x0f,	 ((Rank0_ODT << 6) | (Rank0_ODT << 4) | (Rank2_ODT << 2) | Rank2_ODT),	(DDR2_ODT_75ohm  | NB_ODT_150ohm),
};

static const u8 DQS_DQ_TBL[] = {
/* RxE0: DRAM Timing DQS */
/* RxE2: DRAM Timing DQ */
/*	RxE0, RxE2 */
	0xee, 0xba,
	0xee, 0xba,
	0xcc, 0xba,
	0xcc, 0xba,
};

static const u8 Duty_Control_DDR2[] = {
/*	RxEC, RxED, RxEE, RXEF */
	/* DDRII533 1~2 rank, DDRII400 */
	0x84, 0x10, 0x00, 0x10,
	/* DDRII533 3~4 rank    */
	0x44, 0x10, 0x00, 0x10,
};

static const u8 ChA_Clk_Phase_DDR2_Table[] = {
/*	Rx91, Rx92, Rx93 */
	/* DDRII533 1 rank */
	0x04, 0x05, 0x06,
	/* DDRII533 2~4 rank */
	0x04, 0x05, 0x05,
	/* DDRII400 */
	0x02, 0x04, 0x04,
};

static const u8 DQ_DQS_Table[] = {
/*	REG,  VALUE */
/* DRAM DQ/DQS Output Delay Control */
	0xdc, 0x65,
	0xdd, 0x01,
	0xde, 0xc0,
/* DRAM DQ/DQS input Capture Control */
	0x78, 0x83,
	0x79, 0x83,
	0x7a, 0x00,
};

static const u8 DQSOChA_DDR2_Driving_Table[] = {
/*	Rx70, Rx71 */
	/* DDRII533 1~2 rank */
	0x00, 0x01,
	/* DDRII533 3~4 rank */
	0x03, 0x00,
	/* DDRII400 1~2 rank */
	0x00, 0x04,
	/* DDRII400 3~4 rank */
	0x00, 0x01,
};

/************************************************************************/
/* Chipset Performance UP and other setting after DRAM Sizing Registers	*/
/************************************************************************/
static const u8 Dram_Table[] = {
/*	REG,  AND,  OR	*/
	0x60, 0xff, 0x03,
	0x66, 0xcf, 0x80,
	0x68, 0x00, 0x00,
	0x69, 0xfd, 0x03,
	0x6e, 0xff, 0x01,
	0x95, 0xff, 0x40,
};

static const u8 Host_Table[] = {
/*	REG,	AND,	OR */
	0x51, 0x81, 0x7a,
	0x55, 0xff, 0x06,
	0x5e, 0x00, 0x88,
	0x5d, 0xff, 0xb2,
};

static const u8 Init_Rank_Reg_Table[] = {
	/* Rank Ending Address Registers */
	0x40, 0x41, 0x42, 0x43,
	/* Rank Beginning Address Registers */
	0x48, 0x49, 0x4a, 0x4b,
	/* Physical-to-Virtual Rank Mapping Registers */
	0x54, 0x55,
};

static const u16 DDR2_MRS_table[] = {
/* CL:	2,     3,     4,     5 */
	0x150, 0x1d0, 0x250, 0x2d0,	/* BL=4 ;Use 1X-bandwidth MA table to init DRAM */
	0x158, 0x1d8, 0x258, 0x2d8,	/* BL=8 ;Use 1X-bandwidth MA table to init DRAM */
};

#define	MRS_DDR2_TWR2	((0 << 15) | (0 << 20) | (1 << 12))
#define	MRS_DDR2_TWR3	((0 << 15) | (1 << 20) | (0 << 12))
#define	MRS_DDR2_TWR4	((0 << 15) | (1 << 20) | (1 << 12))
#define	MRS_DDR2_TWR5	((1 << 15) | (0 << 20) | (0 << 12))
static const u32 DDR2_Twr_table[] = {
	MRS_DDR2_TWR2,
	MRS_DDR2_TWR3,
	MRS_DDR2_TWR4,
	MRS_DDR2_TWR5,
};

static const u8 DQSI_Rate_Table[] = {
	8,			/* DDRII 200 */
	8,			/* DDRII 266 */
	8,			/* DDRII 333 */
	7,			/* DDRII 400 */
	8,			/* DDRII 533 */
	8,			/* DDRII 666 */
};

static const u8 REFC_Table[] = {
	0x65, 0x32,		/* DDRII 100 */
	0x86, 0x43,		/* DDRII 266 */
	0xa8, 0x54,		/* DDRII 333 */
	0xca, 0x65,		/* DDRII 400 */
	0xca, 0x86,		/* DDRII 533 */
	0xca, 0xa8,		/* DDRII 666 */
};
