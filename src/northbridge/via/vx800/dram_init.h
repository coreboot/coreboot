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

#ifndef __DRAMINIT_H_
#define __DRAMINIT_H_

//Dram Size
#define M   (1024*1024)
#define M1  (1*M)
#define M64  (64*M)
#define M128  (128*M)
#define M256  (256*M)
#define M384  (384*M)
#define M512  (512*M)

// UMA size
#define 	UMASIZE  M64

#define  ENABLE_CHC   0		//CHC enable, how ever, this CHC,used some reg define in CHB
#define  ENABLE_CHB   0		//CHB enable , CHB is VX800's, VX855 no this CHB.
//Dram Freq
#define DIMMFREQ_800	400
#define DIMMFREQ_667	333
//#define DIMMFREQ_600  300
#define DIMMFREQ_533	266
#define DIMMFREQ_400	200
#define DIMMFREQ_333	166
#define DIMMFREQ_266	133
#define DIMMFREQ_200	100

//Dram Type
#define  RAMTYPE_FPMDRAM   1
#define  RAMTYPE_EDO       2
#define  RAMTYPE_PipelinedNibble 3
#define  RAMTYPE_SDRAM     4
#define  RAMTYPE_ROM       5
#define  RAMTYPE_SGRAMDDR  6
#define  RAMTYPE_SDRAMDDR  7
#define  RAMTYPE_SDRAMDDR2 8

/* CAS latency constant */
#define CASLAN_15         15
#define CASLAN_2           20
#define CASLAN_25         25
#define CASLAN_3           30
#define CASLAN_35         35
#define CASLAN_4           40
#define CASLAN_45         45
#define CASLAN_5           50
#define CASLAN_NULL     00

//Burst length
#define  BURSTLENGTH8    8
#define  BURSTLENGTH4    4

//Data Width
//#define  DATAWIDTHX16    16
//#define  DATAWIDTHX8       8
//#define  DATAWIDTHX4       4

#define SPD_MEMORY_TYPE              2	/*Memory type FPM,EDO,SDRAM,DDR,DDR2 */
#define SPD_SDRAM_ROW_ADDR           3	/*Number of row addresses on this assembly */
#define SPD_SDRAM_COL_ADDR           4	/*Number of column addresses on this assembly */
#define SPD_SDRAM_DIMM_RANKS         5	/*Number of RANKS on this assembly */
#define SPD_SDRAM_MOD_DATA_WIDTH     6	/*Data width of this assembly */
#define SPD_SDRAM_TCLK_X             9	/*Cycle time at Maximum supported CAS latency (CL=X) */
#define SPD_SDRAM_TAC_X              10	/*Access time for highest CL */
#define SPD_SDRAM_CONFIG_TYPE        11	/*Non-parity , Parity or ECC */
#define SPD_SDRAM_REFRESH            12	/*Refresh rate/type */
#define SPD_SDRAM_WIDTH              13	/*Primary sdram width */
#define SPD_SDRAM_MIN_CLK_DLY        15	/*Minimum clock delay */
#define SPD_SDRAM_BURSTLENGTH        16	/*Burst Lengths supported */
#define SPD_SDRAM_NO_OF_BANKS        17	/*Number of banks on this assembly */
#define SPD_SDRAM_CAS_LATENCY        18	/*CAS latency */
#define SPD_SDRAM_DIMM_TYPE_DDR2     20	/*DIMM type information; identifies the DDR2 memory module type */
#define SPD_SDRAM_DEV_ATTR_DDR1      20	/*WE latency */
#define SPD_SDRAM_MODULES_ATTR       21	/*This byte depicts various aspects of the modules; DDR DDR2 have different aspects */
#define SPD_SDRAM_DEV_ATTR_GEN       22	/*General device attributes */
#define SPD_SDRAM_TCLK_X_1           23	/*Minimum clock cycle time at Reduced CL, DDR: X-0.5 DDR2: X-1 */
#define SPD_SDRAM_TAC_X_1            24	/*Maximum Data Access time from Clock at reduced CL,DDR: X-0.5 DDR2: X-1 */
#define SPD_SDRAM_TCLK_X_2           25	/*Minimum clock cycle time at reduced CL, DDR: X-1 DDR2: X-2 */
#define SPD_SDRAM_TAC_X_2            26	/*Maximum Data Access time from Clock at reduced CL, DDR: X-1 DDR2: X-2 */
#define SPD_SDRAM_TRP                27	/*minimum row precharge time */
#define SPD_SDRAM_TRRD               28	/*minimum row active to row active delay */
#define SPD_SDRAM_TRCD               29	/*minimum RAS to CAS delay */
#define SPD_SDRAM_TRAS               30	/*minimum active to precharge time */
#define SPD_SDRAM_TWR                36	/*write recovery time, only DDR2 use it */
#define SPD_SDRAM_TWTR               37	/*internal write to read command delay, only DDR2 use it */
#define SPD_SDRAM_TRTP               38	/*internal read to prechange command delay, only DDR2 use it */
#define SPD_SDRAM_TRFC2              40	/*extension of byte 41 tRC and byte 42 tRFC, only DDR2 use it */
#define SPC_SDRAM_TRC				 41	/*minimum active to active/refresh time */
#define SPD_SDRAM_TRFC               42	/*minimum refresh to active / refresh command period */

#define  SPD_DATA_SIZE 44
//Dram cofig are
/*the most number of socket*/
#define  MAX_RAM_SLOTS  2

#define MAX_SOCKETS MAX_RAM_SLOTS
#define  MAX_DIMMS     MAX_SOCKETS	/*every sockets can plug one DIMM */
/*the most number of RANKs on a DIMM*/
#define  MAX_RANKS  MAX_SOCKETS*2

struct mem_controller {
	u8 channel0[MAX_DIMMS];
};

typedef struct _DRAM_CONFIG_DATA {
	u8 DramClk;
	u8 DramTiming;
	u8 CasLatency;
	u8 BankIntlv;
	u8 Trp;
	u8 Tras;
	u8 Trcd;
	u8 Trfc;
	u8 Trrd;
	u8 Trtp;
	u8 Twtr;
	u8 Twr;

	u8 CmdRate;
	u8 DualEn;
	//u8    IntLv0;
	//u8    IntLv1;
	//u8    Ba0Sel;
	//u8    Ba1Sel;
	//u8    Ba2Sel;
	u8 BaScmb;
	u8 DrdyTiming;
	//u8    Above4G;
	//u8    RdsaitMode;
	//u8    Rdsait;
	//u8    TopPerf;

	u16 UMASize;
} DRAM_CONFIG_DATA;

/*DIMM(assembly) information*/
typedef struct _DIMM_INFO_tag {
	u8 bPresence;
	u8 SPDDataBuf[SPD_DATA_SIZE];	/*get all information from spd data */
} DIMM_INFO;

typedef struct _DRAM_SYS_ATTR_tag {
	DIMM_INFO DimmInfo[MAX_DIMMS];

	u8 RankPresentMap;	/*bit0,1 Rank0,1 on DIMM0, bit2,3 Rank2,3 on DIMM1,
				   bit4,5 Rank4,5 on DIMM2, bit6,7 Rank6,7 on DIMM3 */
	u8 DimmNumChA;		/*Dimm number */
	u8 DimmNumChB;
	u8 RankNumChA;		/*the number of Ranks on the mortherbaord */
	u8 RankNumChB;
	u8 LoadNumChA;		/*the number of chips on all DIMM */
	u8 LoadNumChB;

	u8 DramType;		/*DDR1 or DDR2 */
	u16 DramFreq;
	u16 DramCyc;		/*10ns, 7.5ns, 6ns, 5ns, 3.75ns, 3ns, 2.5ns   =1/SysFreq, unit: 100*ns. */

	//u16    HFreq; /*100, 133, 166, 200, 266, 333, 400*/

	u8 CL;			/* CAS lantency */
	u8 CmdRate;		/*1T or 2T */

	u32 RankSize[MAX_RANKS];
	u8 Dual_Channel;
	DRAM_CONFIG_DATA ConfigData;
	u8 reserved[4];

} DRAM_SYS_ATTR;

typedef struct _DRAM_SIZE_INFO {
	u32 RankLength[MAX_RANKS];
} DRAM_SIZE_INFO;

//detection.c
/*Step1 detect DRAM type, Read SPD data,command rate*/
CB_STATUS DRAMDetect(DRAM_SYS_ATTR * DramAttr);
//freq_setting.c
/*Step2 set Frequency, calculate CAL*/
void DRAMFreqSetting(DRAM_SYS_ATTR * DramAttr);
//timing_setting.c
/*Step3 Set DRAM	Timing*/
void DRAMTimingSetting(DRAM_SYS_ATTR * DramAttr);
//drdy_bl.c
/*Step4 DRDY*/
void DRAMDRDYSetting(DRAM_SYS_ATTR * DramAttr);
//drdy_bl.c
/*Step5 Burst Length*/
void DRAMBurstLength(DRAM_SYS_ATTR * DramAttr);
//driving_setting.c
/*Step6 DRAM Driving Adjustment*/
void DRAMDriving(DRAM_SYS_ATTR * DramAttr);

//clk_ctrl.c
/*Step7 duty cycle control*/
void DutyCycleCtrl(DRAM_SYS_ATTR * DramAttr);
//clk_ctrl.c
/*Step8 DRAM clock phase and delay control*/
void DRAMClkCtrl(DRAM_SYS_ATTR * DramAttr);

//dev_init.c
/*Step9 set register before init DRAM device*/
void DRAMRegInitValue(DRAM_SYS_ATTR * DramAttr);

//dev_init.c
/*Step10 DDR and DDR2 initialize process*/
void DRAMInitializeProc(DRAM_SYS_ATTR * DramAttr);

//dqs_search.c
/*Step11 Search DQS and DQ output delay*/
void DRAMDQSOutputSearch(DRAM_SYS_ATTR * DramAttr);

//dqs_search.c
/*Step12 Search DQS  input delay*/
void DRAMDQSInputSearch(DRAM_SYS_ATTR * DramAttr);

//rank_map.c
/*Step13 Interleav function in rankmap.c*/
void DRAMBankInterleave(DRAM_SYS_ATTR * DramAttr);

//rank_map.c
/*Step14 Sizing*/
void DRAMSizingMATypeM(DRAM_SYS_ATTR * DramAttr);

//final_setting.c
/*Step15 DDR fresh counter setting*/
void DRAMRefreshCounter(DRAM_SYS_ATTR * DramAttr);

//final_setting.c
/*Step16 Final register setting for improve performance*/
void DRAMRegFinalValue(DRAM_SYS_ATTR * DramAttr);

/*set UMA*/
void SetUMARam(void);

CB_STATUS InstallMemory(DRAM_SYS_ATTR * DramAttr, u32 RamSize);
CB_STATUS DDR2_DRAM_INIT(void);

#endif
