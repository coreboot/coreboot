/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
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

/*
 * Description: Include file for all generic DDR 2 MCT files.
 */
#ifndef MCT_D_H
#define MCT_D_H



/*===========================================================================
	CPU - K8/FAM10
===========================================================================*/
#define PT_L1		0		/* CPU Package Type */
#define PT_M2		1
#define PT_S1		2
#define PT_GR		3

#define J_MIN		0		/* j loop constraint. 1=CL 2.0 T*/
#define J_MAX		5		/* j loop constraint. 5=CL 7.0 T*/
#define K_MIN		1		/* k loop constraint. 1=200 Mhz*/
#define K_MAX		5		/* k loop constraint. 5=533 Mhz*/
#define CL_DEF		2		/* Default value for failsafe operation. 2=CL 4.0 T*/
#define T_DEF		1		/* Default value for failsafe operation. 1=5ns (cycle time)*/

#define BSCRate	1		/* reg bit field=rate of dram scrubber for ecc*/
					/* memory initialization (ecc and check-bits).*/
					/* 1=40 ns/64 bytes.*/
#define FirstPass	1		/* First pass through RcvEn training*/
#define SecondPass	2		/* Second pass through Rcven training*/

#define RCVREN_MARGIN	6		/* number of DLL taps to delay beyond first passing position*/
#define MAXASYNCLATCTL_2	2	/* Max Async Latency Control value*/
#define MAXASYNCLATCTL_3	3	/* Max Async Latency Control value*/

#define DQS_FAIL	1
#define DQS_PASS	0
#define DQS_WRITEDIR	1
#define DQS_READDIR	0
#define MIN_DQS_WNDW	3
#define secPassOffset	6
#define Pass1MemClkDly	0x20		/* Add 1/2 Memlock delay */
#define MAX_RD_LAT	0x3FF
#define MIN_FENCE	14
#define MAX_FENCE	20
#define MIN_DQS_WR_FENCE	14
#define MAX_DQS_WR_FENCE	20
#define FenceTrnFinDlySeed	19
#define EarlyArbEn	19

#define PA_HOST(Node)	((((0x18+Node) << 3)+0) << 12)	/* Node 0 Host Bus function PCI Address bits [15:0]*/
#define PA_MAP(Node)	((((0x18+Node) << 3)+1) << 12)	/* Node 0 MAP function PCI Address bits [15:0]*/
#define PA_DCT(Node)	((((0x18+Node) << 3)+2) << 12)	/* Node 0 DCT function PCI Address bits [15:0]*/
//#define PA_EXT_DCT	(((00 << 3)+4) << 8)	/*Node 0 DCT extended configuration registers*/
//#define PA_DCTADDL	(((00 << 3)+2) << 8)	/*Node x DCT function, Additional Registers PCI Address bits [15:0]*/
//#define PA_EXT_DCTADDL (((00 << 3)+5) << 8)	/*Node x DCT function, Additional Registers PCI Address bits [15:0]*/

#define PA_NBMISC(Node)	((((0x18+Node) << 3)+3) << 12)	/*Node 0 Misc PCI Address bits [15:0]*/
//#define PA_NBDEVOP	(((00 << 3)+3) << 8)  /*Node 0 Misc PCI Address bits [15:0]*/

#define DCC_EN		1		/* X:2:0x94[19]*/
#define ILD_Lmt	3		/* X:2:0x94[18:16]*/

#define EncodedTSPD	0x00191709	/* encodes which SPD byte to get T from*/
					/* versus CL X, CL X-.5, and CL X-1*/

#define Bias_TrpT	3		/* bias to convert bus clocks to bit field value*/
#define Bias_TrrdT	2
#define Bias_TrcdT	3
#define Bias_TrasT	3
#define Bias_TrcT	11
#define Bias_TrtpT	2
#define Bias_TwrT	3
#define Bias_TwtrT	0
#define Bias_TfawT	7

#define Min_TrpT	3		/* min programmable value in busclocks*/
#define Max_TrpT	6		/* max programmable value in busclocks*/
#define Min_TrrdT	2
#define Max_TrrdT	5
#define Min_TrcdT	3
#define Max_TrcdT	6
#define Min_TrasT	5
#define Max_TrasT	18
#define Min_TrcT	11
#define Max_TrcT	26
#define Min_TrtpT	2
#define Max_TrtpT	3
#define Min_TwrT	3
#define Max_TwrT	6
#define Min_TwtrT	1
#define Max_TwtrT	3

/*DDR2-1066 support*/
#define Bias_TrcdT_1066	5
#define Bias_TrasT_1066	15
#define Bias_TrrdT_1066	4
#define Bias_TwrT_1066		4
#define Bias_TrpT_1066		5
#define Bias_TwtrT_1066	4
#define Bias_TfawT_1066	15

#define Min_TrcdT_1066		5
#define Max_TrcdT_1066		12
#define Min_TrasT_1066		15
#define Max_TrasT_1066		30
#define Min_TrcT_1066		11
#define Max_TrcT_1066		42
#define Min_TrrdT_1066		4
#define Max_TrrdT_1066		7
#define Min_TwrT_1066		5
#define Max_TwrT_1066		8
#define Min_TrpT_1066		5
#define Max_TrpT_1066		12
#define Min_TwtrT_1066		4
#define Max_TwtrT_1066		7

/*common register bit names*/
#define DramHoleValid		0	/* func 1, offset F0h, bit 0*/
#define DramMemHoistValid	1	/* func 1, offset F0h, bit 1*/
#define CSEnable		0	/* func 2, offset 40h-5C, bit 0*/
#define Spare			1	/* func 2, offset 40h-5C, bit 1*/
#define TestFail		2	/* func 2, offset 40h-5C, bit 2*/
#define DqsRcvEnTrain		18	/* func 2, offset 78h, bit 18*/
#define EnDramInit		31	/* func 2, offset 7Ch, bit 31*/
#define DisAutoRefresh		18	/* func 2, offset 8Ch, bit 18*/
#define InitDram		0	/* func 2, offset 90h, bit 0*/
#define BurstLength32		10	/* func 2, offset 90h, bit 10*/
#define Width128		11	/* func 2, offset 90h, bit 11*/
#define X4Dimm			12	/* func 2, offset 90h, bit 12*/
#define UnBuffDimm		16	/* func 2, offset 90h, bit 16*/
#define DimmEcEn		19	/* func 2, offset 90h, bit 19*/
#define MemClkFreqVal		3	/* func 2, offset 94h, bit 3*/
#define RDqsEn			12	/* func 2, offset 94h, bit 12*/
#define DisDramInterface	14	/* func 2, offset 94h, bit 14*/
#define DctAccessWrite		30	/* func 2, offset 98h, bit 30*/
#define DctAccessDone		31	/* func 2, offset 98h, bit 31*/
#define MemClrStatus		0	/* func 2, offset A0h, bit 0*/
#define PwrSavingsEn		10	/* func 2, offset A0h, bit 10*/
#define Mod64BitMux		4	/* func 2, offset A0h, bit 4*/
#define DisableJitter		1	/* func 2, offset A0h, bit 1*/
#define MemClrDis		1	/* func 3, offset F8h, FNC 4, bit 1*/
#define SyncOnUcEccEn		2	/* func 3, offset 44h, bit 2*/
#define Dr_MemClrStatus	10	/* func 3, offset 110h, bit 10*/
#define MemClrBusy		9	/* func 3, offset 110h, bit 9*/
#define DctGangEn		4	/* func 3, offset 110h, bit 4*/
#define MemClrInit		3	/* func 3, offset 110h, bit 3*/
#define AssertCke		28	/* func 2, offset 7Ch, bit 28*/
#define DeassertMemRstX	27	/* func 2, offset 7Ch, bit 27*/
#define SendMrsCmd		26	/* func 2, offset 7Ch, bit 26*/
#define SendAutoRefresh	25	/* func 2, offset 7Ch, bit 25*/
#define SendPchgAll		24	/* func 2, offset 7Ch, bit 24*/
#define DisDqsBar		6	/* func 2, offset 90h, bit 6*/
#define DramEnabled		8	/* func 2, offset 110h, bit 8*/
#define LegacyBiosMode		9	/* func 2, offset 94h, bit 9*/
#define PrefDramTrainMode	28	/* func 2, offset 11Ch, bit 28*/
#define FlushWr		30	/* func 2, offset 11Ch, bit 30*/
#define DisAutoComp		30	/* func 2, offset 9Ch, Index 8, bit 30*/
#define DqsRcvTrEn		13	/* func 2, offset 9Ch, Index 8, bit 13*/
#define ForceAutoPchg		23	/* func 2, offset 90h, bit 23*/
#define ClLinesToNbDis		15	/* Bu_CFG2, bit 15*/
#define WbEnhWsbDis_D		(48-32)
#define PhyFenceTrEn		3	/* func 2, offset 9Ch, Index 8, bit 3 */
#define ParEn			8	/* func 2, offset 90h, bit 8 */
#define DcqArbBypassEn		19	/* func 2, offset 94h, bit 19 */
#define ActiveCmdAtRst		1	/* func 2, offset A8H, bit 1 */
#define FlushWrOnStpGnt	29	/* func 2, offset 11Ch, bit 29 */
#define BankSwizzleMode	22	/* func 2, offset 94h, bit 22 */
#define ChSetupSync		15	/* func 2, offset 78h, bit 15 */



/*=============================================================================
	SW Initialization
============================================================================*/
#define DLL_Enable	1
#define OCD_Default	2
#define OCD_Exit	3



/*=============================================================================
	Jedec DDR II
=============================================================================*/
#define SPD_TYPE	2		/*SPD byte read location*/
	#define JED_DDRSDRAM	0x07	/*Jedec defined bit field*/
	#define JED_DDR2SDRAM	0x08	/*Jedec defined bit field*/

#define SPD_DIMMTYPE	20
#define SPD_ATTRIB	21
	#define JED_DIFCKMSK	0x20	/*Differential Clock Input*/
	#define JED_REGADCMSK	0x11	/*Registered Address/Control*/
	#define JED_PROBEMSK	0x40	/*Analysis Probe installed*/
#define SPD_DEVATTRIB	22
#define SPD_EDCTYPE	11
	#define JED_ECC		0x02
	#define JED_ADRCPAR	0x04
#define SPD_ROWSZ	3
#define SPD_COLSZ	4
#define SPD_LBANKS	17		/*number of [logical] banks on each device*/
#define SPD_DMBANKS	5		/*number of physical banks on dimm*/
	#define SPDPLBit	4	/* Dram package bit*/
#define SPD_BANKSZ	31		/*capacity of physical bank*/
#define SPD_DEVWIDTH	13
#define SPD_CASLAT	18
#define SPD_TRP	27
#define SPD_TRRD	28
#define SPD_TRCD	29
#define SPD_TRAS	30
#define SPD_TWR	36
#define SPD_TWTR	37
#define SPD_TRTP	38
#define SPD_TRCRFC	40
#define SPD_TRC	41
#define SPD_TRFC	42

#define SPD_MANDATEYR	93		/*Module Manufacturing Year (BCD)*/

#define SPD_MANDATEWK	94		/*Module Manufacturing Week (BCD)*/

/*-----------------------------
	Jedec DDR II related equates
-----------------------------*/
#define MYEAR06	6	/* Manufacturing Year BCD encoding of 2006 - 06d*/
#define MWEEK24	0x24	/* Manufacturing Week BCD encoding of June - 24d*/

/*=============================================================================
	Macros
=============================================================================*/

#define _2GB_RJ8	(2<<(30-8))
#define _4GB_RJ8	(4<<(30-8))
#define _4GB_RJ4	(4<<(30-4))

#define BigPagex8_RJ8	(1<<(17+3-8))	/*128KB * 8 >> 8 */

/*=============================================================================
	Global MCT Status Structure
=============================================================================*/
struct MCTStatStruc {
	u32 GStatus;		/* Global Status bitfield*/
	u32 HoleBase;		/* If not zero, BASE[39:8] (system address)
				      of sub 4GB dram hole for HW remapping.*/
	u32 Sub4GCacheTop;	/* If not zero, the 32-bit top of cacheable memory.*/
	u32 SysLimit;		/* LIMIT[39:8] (system address)*/
};

/*=============================================================================
	Global MCT Configuration Status Word (GStatus)
=============================================================================*/
/*These should begin at bit 0 of GStatus[31:0]*/
#define GSB_MTRRshort	0		/* Ran out of MTRRs while mapping memory*/
#define GSB_ECCDIMMs	1		/* All banks of all Nodes are ECC capable*/
#define GSB_DramECCDis	2		/* Dram ECC requested but not enabled.*/
#define GSB_SoftHole	3		/* A Node Base gap was created*/
#define GSB_HWHole	4		/* A HW dram remap was created*/
#define GSB_NodeIntlv	5		/* Node Memory interleaving was enabled*/
#define GSB_SpIntRemapHole	16	/* Special condition for Node Interleave and HW remapping*/
#define GSB_EnDIMMSpareNW	17	/* Indicates that DIMM Spare can be used without a warm reset */
					/* NOTE: This is a local bit used by memory code */


/*===============================================================================
	Local DCT Status structure (a structure for each DCT)
===============================================================================*/

struct DCTStatStruc {		/* A per Node structure*/
/* DCTStatStruct_F -  start */
	u8 Node_ID;		/* Node ID of current controller*/
	u8 ErrCode;		/* Current error condition of Node
		0= no error
		1= Variance Error, DCT is running but not in an optimal configuration.
		2= Stop Error, DCT is NOT running
		3= Fatal Error, DCT/MCT initialization has been halted.*/
	u32 ErrStatus;		/* Error Status bit Field */
	u32 Status;		/* Status bit Field*/
	u8 DIMMAddr[8];		/* SPD address of DIMM controlled by MA0_CS_L[0,1]*/
		/* SPD address of..MB0_CS_L[0,1]*/
		/* SPD address of..MA1_CS_L[0,1]*/
		/* SPD address of..MB1_CS_L[0,1]*/
		/* SPD address of..MA2_CS_L[0,1]*/
		/* SPD address of..MB2_CS_L[0,1]*/
		/* SPD address of..MA3_CS_L[0,1]*/
		/* SPD address of..MB3_CS_L[0,1]*/
	u16 DIMMPresent;		/*For each bit n 0..7, 1=DIMM n is present.
		DIMM#  Select Signal
		0  MA0_CS_L[0,1]
		1  MB0_CS_L[0,1]
		2  MA1_CS_L[0,1]
		3  MB1_CS_L[0,1]
		4  MA2_CS_L[0,1]
		5  MB2_CS_L[0,1]
		6  MA3_CS_L[0,1]
		7  MB3_CS_L[0,1]*/
	u16 DIMMValid;		/* For each bit n 0..7, 1=DIMM n is valid and is/will be configured*/
	u16 DIMMMismatch;	/* For each bit n 0..7, 1=DIMM n is mismatched, channel B is always considered the mismatch */
	u16 DIMMSPDCSE;		/* For each bit n 0..7, 1=DIMM n SPD checksum error*/
	u16 DimmECCPresent;	/* For each bit n 0..7, 1=DIMM n is ECC capable.*/
	u16 DimmPARPresent;	/* For each bit n 0..7, 1=DIMM n is ADR/CMD Parity capable.*/
	u16 Dimmx4Present;	/* For each bit n 0..7, 1=DIMM n contains x4 data devices.*/
	u16 Dimmx8Present;	/* For each bit n 0..7, 1=DIMM n contains x8 data devices.*/
	u16 Dimmx16Present;	/* For each bit n 0..7, 1=DIMM n contains x16 data devices.*/
	u16 DIMM2Kpage;		/* For each bit n 0..7, 1=DIMM n contains 1K page devices.*/
	u8 MAload[2];		/* Number of devices loading MAA bus*/
		/* Number of devices loading MAB bus*/
	u8 MAdimms[2];		/*Number of DIMMs loading CH A*/
		/* Number of DIMMs loading CH B*/
	u8 DATAload[2];		/*Number of ranks loading CH A DATA*/
		/* Number of ranks loading CH B DATA*/
	u8 DIMMAutoSpeed;	/*Max valid Mfg. Speed of DIMMs
		1=200Mhz
		2=266Mhz
		3=333Mhz
		4=400Mhz
		5=533Mhz*/
	u8 DIMMCASL;		/* Min valid Mfg. CL bitfield
		0=2.0
		1=3.0
		2=4.0
		3=5.0
		4=6.0 */
	u16 DIMMTrcd;		/* Minimax Trcd*40 (ns) of DIMMs*/
	u16 DIMMTrp;		/* Minimax Trp*40 (ns) of DIMMs*/
	u16 DIMMTrtp;		/* Minimax Trtp*40 (ns) of DIMMs*/
	u16 DIMMTras;		/* Minimax Tras*40 (ns) of DIMMs*/
	u16 DIMMTrc;		/* Minimax Trc*40 (ns) of DIMMs*/
	u16 DIMMTwr;		/* Minimax Twr*40 (ns) of DIMMs*/
	u16 DIMMTrrd;		/* Minimax Trrd*40 (ns) of DIMMs*/
	u16 DIMMTwtr;		/* Minimax Twtr*40 (ns) of DIMMs*/
	u8 Speed;		/* Bus Speed (to set Controller)
		1=200Mhz
		2=266Mhz
		3=333Mhz
		4=400Mhz */
	u8 CASL;		/* CAS latency DCT setting
		0=2.0
		1=3.0
		2=4.0
		3=5.0
		4=6.0 */
	u8 Trcd;		/* DCT Trcd (busclocks) */
	u8 Trp;			/* DCT Trp (busclocks) */
	u8 Trtp;		/* DCT Trtp (busclocks) */
	u8 Tras;		/* DCT Tras (busclocks) */
	u8 Trc;			/* DCT Trc (busclocks) */
	u8 Twr;			/* DCT Twr (busclocks) */
	u8 Trrd;		/* DCT Trrd (busclocks) */
	u8 Twtr;		/* DCT Twtr (busclocks) */
	u8 Trfc[4];		/* DCT Logical DIMM0 Trfc
		0=75ns (for 256Mb devs)
		1=105ns (for 512Mb devs)
		2=127.5ns (for 1Gb devs)
		3=195ns (for 2Gb devs)
		4=327.5ns (for 4Gb devs) */
		/* DCT Logical DIMM1 Trfc (see Trfc0 for format) */
		/* DCT Logical DIMM2 Trfc (see Trfc0 for format) */
		/* DCT Logical DIMM3 Trfc (see Trfc0 for format) */
	u16 CSPresent;		/* For each bit n 0..7, 1=Chip-select n is present */
	u16 CSTestFail;		/* For each bit n 0..7, 1=Chip-select n is present but disabled */
	u32 DCTSysBase;		/* BASE[39:8] (system address) of this Node's DCTs. */
	u32 DCTHoleBase;	/* If not zero, BASE[39:8] (system address) of dram hole for HW remapping.  Dram hole exists on this Node's DCTs. */
	u32 DCTSysLimit;	/* LIMIT[39:8] (system address) of this Node's DCTs */
	u16 PresetmaxFreq;	/* Maximum OEM defined DDR frequency
		200=200Mhz (DDR400)
		266=266Mhz (DDR533)
		333=333Mhz (DDR667)
		400=400Mhz (DDR800) */
	u8 _2Tmode;		/* 1T or 2T CMD mode (slow access mode)
		1=1T
		2=2T */
	u8 TrwtTO;		/* DCT TrwtTO (busclocks)*/
	u8 Twrrd;		/* DCT Twrrd (busclocks)*/
	u8 Twrwr;		/* DCT Twrwr (busclocks)*/
	u8 Trdrd;		/* DCT Trdrd (busclocks)*/
	u32 CH_ODC_CTL[2];	/* Output Driver Strength (see BKDG FN2:Offset 9Ch, index 00h*/
	u32 CH_ADDR_TMG[2];	/* Address Bus Timing (see BKDG FN2:Offset 9Ch, index 04h*/
		/* Output Driver Strength (see BKDG FN2:Offset 9Ch, index 20h*/
		/* Address Bus Timing (see BKDG FN2:Offset 9Ch, index 24h*/
	u16 CH_EccDQSLike[2];	/* CHA DQS ECC byte like...*/
	u8 CH_EccDQSScale[2];	/* CHA DQS ECC byte scale*/
		/* CHA DQS ECC byte like...*/
		/* CHA DQS ECC byte scale*/
	u8 MaxAsyncLat;		/* Max Asynchronous Latency (ns)*/
	// NOTE: Not used in Barcelona - u8 CH_D_RCVRDLY[2][4];
		/* CHA DIMM 0 - 4 Receiver Enable Delay*/
		/* CHB DIMM 0 - 4 Receiver Enable Delay */
	// NOTE: Not used in Barcelona - u8 CH_D_B_DQS[2][2][8];
		/* CHA Byte 0-7 Write DQS Delay */
		/* CHA Byte 0-7 Read DQS Delay */
		/* CHB Byte 0-7 Write DQS Delay */
		/* CHB Byte 0-7 Read DQS Delay */
	u32 PtrPatternBufA;	/* Ptr on stack to aligned DQS testing pattern*/
	u32 PtrPatternBufB;	/* Ptr on stack to aligned DQS testing pattern*/
	u8 Channel;		/* Current Channel (0= CH A, 1=CH B)*/
	u8 ByteLane;		/* Current Byte Lane (0..7)*/
	u8 Direction;		/* Current DQS-DQ training write direction (0=read, 1=write)*/
	u8 Pattern;		/* Current pattern*/
	u8 DQSDelay;		/* Current DQS delay value*/
	u32 TrainErrors;	/* Current Training Errors*/

	u32 AMC_TSC_DeltaLo;	/* Time Stamp Counter measurement of AMC, Low dword*/
	u32 AMC_TSC_DeltaHi;	/* Time Stamp Counter measurement of AMC, High dword*/
	// NOTE: Not used in Barcelona - u8 CH_D_DIR_MaxMin_B_Dly[2][4][2][2][8];
		/* CH A byte lane 0 - 7 minimum filtered window  passing DQS delay value*/
		/* CH A byte lane 0 - 7 maximum filtered window  passing DQS delay value*/
		/* CH B byte lane 0 - 7 minimum filtered window  passing DQS delay value*/
		/* CH B byte lane 0 - 7 maximum filtered window  passing DQS delay value*/
		/* CH A byte lane 0 - 7 minimum filtered window  passing DQS delay value*/
		/* CH A byte lane 0 - 7 maximum filtered window  passing DQS delay value*/
		/* CH B byte lane 0 - 7 minimum filtered window  passing DQS delay value*/
		/* CH B byte lane 0 - 7 maximum filtered window  passing DQS delay value*/
	u32 LogicalCPUID;	/* The logical CPUID of the node*/
	u16 HostBiosSrvc1;	/* Word sized general purpose field for use by host BIOS.  Scratch space.*/
	u32 HostBiosSrvc2;	/* Dword sized general purpose field for use by host BIOS.  Scratch space.*/
	u16 DimmQRPresent;	/* QuadRank DIMM present?*/
	u16 DimmTrainFail;	/* Bitmap showing which dimms failed training*/
	u16 CSTrainFail;	/* Bitmap showing which chipselects failed training*/
	u16 DimmYr06;		/* Bitmap indicating which Dimms have a manufactur's year code <= 2006*/
	u16 DimmWk2406;		/* Bitmap indicating which Dimms have a manufactur's week code <= 24 of 2006 (June)*/
	u16 DimmDRPresent;	/* Bitmap indicating that Dual Rank Dimms are present*/
	u16 DimmPlPresent;	/* Bitmap indicating that Planar (1) or Stacked (0) Dimms are present.*/
	u16 ChannelTrainFai;	/* Bitmap showing the channel information about failed Chip Selects
		0 in any bit field indicates Channel 0
		1 in any bit field indicates Channel 1 */
	u16 CSUsrTestFail;	/* Chip selects excluded by user */
/* DCTStatStruct_F -  end */

	u16 CH_MaxRdLat[2];	/* Max Read Latency (ns) for DCT 0*/
		/* Max Read Latency (ns) for DCT 1*/
	u8 CH_D_DIR_B_DQS[2][4][2][9];	/* [A/B] [DIMM1-4] [R/W] [DQS] */
		/* CHA DIMM0 Byte 0 - 7 and Check Write DQS Delay*/
		/* CHA DIMM0 Byte 0 - 7 and Check Read DQS Delay*/
		/* CHA DIMM1 Byte 0 - 7 and Check Write DQS Delay*/
		/* CHA DIMM1 Byte 0 - 7 and Check Read DQS Delay*/
		/* CHB DIMM0 Byte 0 - 7 and Check Write DQS Delay*/
		/* CHB DIMM0 Byte 0 - 7 and Check Read DQS Delay*/
		/* CHB DIMM1 Byte 0 - 7 and Check Write DQS Delay*/
		/* CHB DIMM1 Byte 0 - 7 and Check  Read DQS Delay*/
	u8 CH_D_B_RCVRDLY[2][4][8];	/* [A/B] [DIMM0-3] [DQS] */
		/* CHA DIMM 0 Receiver Enable Delay*/
		/* CHA DIMM 1 Receiver Enable Delay*/
		/* CHA DIMM 2 Receiver Enable Delay*/
		/* CHA DIMM 3 Receiver Enable Delay*/

		/* CHB DIMM 0 Receiver Enable Delay*/
		/* CHB DIMM 1 Receiver Enable Delay*/
		/* CHB DIMM 2 Receiver Enable Delay*/
		/* CHB DIMM 3 Receiver Enable Delay*/
	u8 CH_D_BC_RCVRDLY[2][4];
		/* CHA DIMM 0 - 4 Check Byte Receiver Enable Delay*/
		/* CHB DIMM 0 - 4 Check Byte Receiver Enable Delay*/
	u8 DIMMValidDCT[2];	/* DIMM# in DCT0*/
				/* DIMM# in DCT1*/
	u8 MaxDCTs;		/* Max number of DCTs in system*/
	// NOTE: removed u8 DCT. Use ->dev_ for pci R/W;	/*DCT pointer*/
	u8 GangedMode;		/* Ganged mode enabled, 0 = disabled, 1 = enabled*/
	u8 DRPresent;		/* Family 10 present flag, 0 = not Fam10, 1 = Fam10*/
	u32 NodeSysLimit;	/* BASE[39:8],for DCT0+DCT1 system address*/
	u8 WrDatGrossH;
	u8 DqsRcvEnGrossL;
	// NOTE: Not used - u8 NodeSpeed		/* Bus Speed (to set Controller)
		/* 1=200Mhz */
		/* 2=266Mhz */
		/* 3=333Mhz */
	// NOTE: Not used - u8 NodeCASL		/* CAS latency DCT setting
		/* 0=2.0 */
		/* 1=3.0 */
		/* 2=4.0 */
		/* 3=5.0 */
		/* 4=6.0 */
	u8 TrwtWB;
	u8 CurrRcvrCHADelay;	/* for keep current RcvrEnDly of chA*/
	u16 T1000;		/* get the T1000 figure (cycle time (ns)*1K)*/
	u8 DqsRcvEn_Pass;	/* for TrainRcvrEn byte lane pass flag*/
	u8 DqsRcvEn_Saved;	/* for TrainRcvrEn byte lane saved flag*/
	u8 SeedPass1Remainder;	/* for Phy assisted DQS receiver enable training*/

	/* for second pass  - Second pass should never run for Fam10*/
	// NOTE: Not used for Barcelona - u8 CH_D_B_RCVRDLY_1[2][4][8];	/* CHA DIMM 0 Receiver Enable Delay*/
		/* CHA DIMM 1 Receiver Enable Delay*/
		/* CHA DIMM 2 Receiver Enable Delay*/
		/* CHA DIMM 3 Receiver Enable Delay*/

		/* CHB DIMM 0 Receiver Enable Delay*/
		/* CHB DIMM 1 Receiver Enable Delay*/
		/* CHB DIMM 2 Receiver Enable Delay*/
		/* CHB DIMM 3 Receiver Enable Delay*/

	u8 ClToNB_flag;	/* is used to restore ClLinesToNbDis bit after memory */
	u32 NodeSysBase;	/* for channel interleave usage */

/* New for LB Support */
	u8 NodePresent;
	u32 dev_host;
	u32 dev_map;
	u32 dev_dct;
	u32 dev_nbmisc;
};

/*===============================================================================
	Local Error Status Codes (DCTStatStruc.ErrCode)
===============================================================================*/
#define SC_RunningOK		0
#define SC_VarianceErr		1	/* Running non-optimally*/
#define SC_StopError		2	/* Not Running*/
#define SC_FatalErr		3	/* Fatal Error, MCTB has exited immediately*/

/*===============================================================================
	Local Error Status (DCTStatStruc.ErrStatus[31:0])
===============================================================================*/
#define SB_NoDimms		0
#define SB_DIMMChkSum		1
#define SB_DimmMismatchM	2	/* dimm module type(buffer) mismatch*/
#define SB_DimmMismatchT	3	/* dimm CL/T mismatch*/
#define SB_DimmMismatchO	4	/* dimm organization mismatch (128-bit)*/
#define SB_NoTrcTrfc		5	/* SPD missing Trc or Trfc info*/
#define SB_NoCycTime		6	/* SPD missing byte 23 or 25*/
#define SB_BkIntDis		7	/* Bank interleave requested but not enabled*/
#define SB_DramECCDis		8	/* Dram ECC requested but not enabled*/
#define SB_SpareDis		9	/* Online spare requested but not enabled*/
#define SB_MinimumMode		10	/* Running in Minimum Mode*/
#define SB_NORCVREN		11	/* No DQS Receiver Enable pass window found*/
#define SB_CHA2BRCVREN		12	/* DQS Rcvr En pass window CHA to CH B too large*/
#define SB_SmallRCVR		13	/* DQS Rcvr En pass window too small (far right of dynamic range)*/
#define SB_NODQSPOS		14	/* No DQS-DQ passing positions*/
#define SB_SMALLDQS		15	/* DQS-DQ passing window too small*/
#define SB_DCBKScrubDis	16	/* DCache scrub requested but not enabled */

/*===============================================================================
	Local Configuration Status (DCTStatStruc.Status[31:0])
===============================================================================*/
#define SB_Registered		0	/* All DIMMs are Registered*/
#define SB_ECCDIMMs		1	/* All banks ECC capable*/
#define SB_PARDIMMs		2	/* All banks Addr/CMD Parity capable*/
#define SB_DiagClks		3	/* Jedec ALL slots clock enable diag mode*/
#define SB_128bitmode		4	/* DCT in 128-bit mode operation*/
#define SB_64MuxedMode		5	/* DCT in 64-bit mux'ed mode.*/
#define SB_2TMode		6	/* 2T CMD timing mode is enabled.*/
#define SB_SWNodeHole		7	/* Remapping of Node Base on this Node to create a gap.*/
#define SB_HWHole		8	/* Memory Hole created on this Node using HW remapping.*/
#define SB_Over400MHz		9	/* DCT freq >= 400MHz flag*/
#define SB_DQSPos_Pass2	10	/* Using for TrainDQSPos DIMM0/1, when freq>=400MHz*/
#define SB_DQSRcvLimit		11	/* Using for DQSRcvEnTrain to know we have reached to upper bound.*/
#define SB_ExtConfig		12	/* Indicator the default setting for extend PCI configuration support*/




/*===============================================================================
	NVRAM/run-time-configurable Items
===============================================================================*/
/*Platform Configuration*/
#define NV_PACK_TYPE		0	/* CPU Package Type (2-bits)
					    0=NPT L1
					    1=NPT M2
					    2=NPT S1*/
#define NV_MAX_NODES		1	/* Number of Nodes/Sockets (4-bits)*/
#define NV_MAX_DIMMS		2	/* Number of DIMM slots for the specified Node ID (4-bits)*/
#define NV_MAX_MEMCLK		3	/* Maximum platform demonstrated Memclock (10-bits)
					    200=200Mhz (DDR400)
					    266=266Mhz (DDR533)
					    333=333Mhz (DDR667)
					    400=400Mhz (DDR800)*/
#define NV_ECC_CAP		4	/* Bus ECC capable (1-bits)
					    0=Platform not capable
					    1=Platform is capable*/
#define NV_4RANKType		5	/* Quad Rank DIMM slot type (2-bits)
					    0=Normal
					    1=R4 (4-Rank Registered DIMMs in AMD server configuration)
					    2=S4 (Unbuffered SO-DIMMs)*/
#define NV_BYPMAX		6	/* Value to set DcqBypassMax field (See Function 2, Offset 94h, [27:24] of BKDG for field definition).
					    4=4 times bypass (normal for non-UMA systems)
					    7=7 times bypass (normal for UMA systems)*/
#define NV_RDWRQBYP		7	/* Value to set RdWrQByp field (See Function 2, Offset A0h, [3:2] of BKDG for field definition).
					    2=8 times (normal for non-UMA systems)
					    3=16 times (normal for UMA systems)*/


/*Dram Timing*/
#define NV_MCTUSRTMGMODE	10	/* User Memclock Mode (2-bits)
					    0=Auto, no user limit
					    1=Auto, user limit provided in NV_MemCkVal
					    2=Manual, user value provided in NV_MemCkVal*/
#define NV_MemCkVal		11	/* Memory Clock Value (2-bits)
					    0=200Mhz
					    1=266Mhz
					    2=333Mhz
					    3=400Mhz*/

/*Dram Configuration*/
#define NV_BankIntlv		20	/* Dram Bank (chip-select) Interleaving (1-bits)
					    0=disable
					    1=enable*/
#define NV_AllMemClks		21	/* Turn on All DIMM clocks (1-bits)
					    0=normal
					    1=enable all memclocks*/
#define NV_SPDCHK_RESTRT	22	/* SPD Check control bitmap (1-bits)
					    0=Exit current node init if any DIMM has SPD checksum error
					    1=Ignore faulty SPD checksums (Note: DIMM cannot be enabled)*/
#define NV_DQSTrainCTL		23	/* DQS Signal Timing Training Control
					    0=skip DQS training
					    1=perform DQS training*/
#define NV_NodeIntlv		24	/* Node Memory Interleaving (1-bits)
					    0=disable
					    1=enable*/
#define NV_BurstLen32		25	/* BurstLength32 for 64-bit mode (1-bits)
					    0=disable (normal)
					    1=enable (4 beat burst when width is 64-bits)*/

/*Dram Power*/
#define NV_CKE_PDEN		30	/* CKE based power down mode (1-bits)
					    0=disable
					    1=enable*/
#define NV_CKE_CTL		31	/* CKE based power down control (1-bits)
					    0=per Channel control
					    1=per Chip select control*/
#define NV_CLKHZAltVidC3	32	/* Memclock tri-stating during C3 and Alt VID (1-bits)
					    0=disable
					    1=enable*/

/*Memory Map/Mgt.*/
#define NV_BottomIO		40	/* Bottom of 32-bit IO space (8-bits)
					    NV_BottomIO[7:0]=Addr[31:24]*/
#define NV_BottomUMA		41	/* Bottom of shared graphics dram (8-bits)
					    NV_BottomUMA[7:0]=Addr[31:24]*/
#define NV_MemHole		42	/* Memory Hole Remapping (1-bits)
					    0=disable
					    1=enable  */

/*ECC*/
#define NV_ECC			50	/* Dram ECC enable*/
#define NV_NBECC		52	/* ECC MCE enable*/
#define NV_ChipKill		53	/* Chip-Kill ECC Mode enable*/
#define NV_ECCRedir		54	/* Dram ECC Redirection enable*/
#define NV_DramBKScrub		55	/* Dram ECC Background Scrubber CTL*/
#define NV_L2BKScrub		56	/* L2 ECC Background Scrubber CTL*/
#define NV_DCBKScrub		57	/* DCache ECC Background Scrubber CTL*/
#define NV_CS_SpareCTL		58	/* Chip Select Spare Control bit 0:
					       0=disable Spare
					       1=enable Spare */
					/* Chip Select Spare Control bit 1-4:
					     Reserved, must be zero*/
#define NV_SyncOnUnEccEn	61	/* SyncOnUnEccEn control
					   0=disable
					   1=enable*/
#define NV_Unganged		62

#define NV_ChannelIntlv	63	/* Channel Interleaving (3-bits)
					xx0b = disable
					yy1b = enable with DctSelIntLvAddr set to yyb */


u32 Get_NB32(u32 dev, u32 reg);
void Set_NB32(u32 dev, u32 reg, u32 val);
u32 Get_NB32_index(u32 dev, u32 index_reg, u32 index);
void Set_NB32_index(u32 dev, u32 index_reg, u32 index, u32 data);
u32 Get_NB32_index_wait(u32 dev, u32 index_reg, u32 index);
void Set_NB32_index_wait(u32 dev, u32 index_reg, u32 index, u32 data);
u32 OtherTiming_A_D(struct DCTStatStruc *pDCTstat, u32 val);
void mct_ForceAutoPrecharge_D(struct DCTStatStruc *pDCTstat, u32 dct);
u32 Modify_D3CMP(struct DCTStatStruc *pDCTstat, u32 dct, u32 value);
u8 mct_checkNumberOfDqsRcvEn_1Pass(u8 pass);
u32 SetupDqsPattern_1PassA(u8 Pass);
u32 SetupDqsPattern_1PassB(u8 Pass);
u8 mct_Get_Start_RcvrEnDly_1Pass(u8 Pass);
u8 mct_Average_RcvrEnDly_Pass(struct DCTStatStruc *pDCTstat, u8 RcvrEnDly, u8 RcvrEnDlyLimit, u8 Channel, u8 Receiver, u8 Pass);
void CPUMemTyping_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void UMAMemTyping_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
u32 mctGetLogicalCPUID(u32 Node);
u8 ECCInit_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void TrainReceiverEn_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA, u8 Pass);
void mct_TrainDQSPos_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void mctSetEccDQSRcvrEn_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void TrainMaxReadLatency_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void mct_EndDQSTraining_D(struct MCTStatStruc *pMCTstat,struct DCTStatStruc *pDCTstatA);
void mct_SetRcvrEnDly_D(struct DCTStatStruc *pDCTstat, u8 RcvrEnDly, u8 FinalValue, u8 Channel, u8 Receiver, u32 dev, u32 index_reg, u8 Addl_Index, u8 Pass);
void SetEccDQSRcvrEn_D(struct DCTStatStruc *pDCTstat, u8 Channel);
void mctGet_PS_Cfg_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u32 dct);
void InterleaveBanks_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct);
void mct_SetDramConfigHi_D(struct DCTStatStruc *pDCTstat, u32 dct, u32 DramConfigHi);
void mct_DramInit_Hw_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct);
void SyncSetting(struct DCTStatStruc *pDCTstat);
void mct_SetClToNB_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
void mct_SetWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
void mct_TrainRcvrEn_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 Pass);
void mct_EnableDimmEccEn_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 _DisableDramECC);
u32 procOdtWorkaround(struct DCTStatStruc *pDCTstat, u32 dct, u32 val);
void mct_BeforeDramInit_D(struct DCTStatStruc *pDCTstat, u32 dct);
void mctGet_DIMMAddr(struct DCTStatStruc *pDCTstat, u32 node);
void mctSMBhub_Init(u32 node);
int mctRead_SPD(u32 smaddr, u32 reg);
void InterleaveNodes_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void InterleaveChannels_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
void mct_BeforeDQSTrain_Samp_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat);
void StoreDQSDatStrucVal_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 ChipSel);
void phyAssistedMemFnceTraining(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
u8 mct_SaveRcvEnDly_D_1Pass(struct DCTStatStruc *pDCTstat, u8 pass);
u32 CheckNBCOFAutoPrechg(struct DCTStatStruc *pDCTstat, u32 dct);
u8 mct_AdjustDQSPosDelay_D(struct DCTStatStruc *pDCTstat, u8 dly);
void mct_AdjustScrub_D(struct DCTStatStruc *pDCTstat, u16 *scrub_request);
u8 mct_InitReceiver_D(struct DCTStatStruc *pDCTstat, u8 dct);
void mct_Wait(u32 cycles);
u8 mct_RcvrRankEnabled_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 Channel, u8 ChipSel);
u32 mct_GetRcvrSysAddr_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 channel, u8 receiver, u8 *valid);
void mct_Read1LTestPattern_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u32 addr);
void EarlySampleSupport_D(void);
#endif
