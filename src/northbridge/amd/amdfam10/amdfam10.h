/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#ifndef AMDFAM10_H
#define AMDFAM10_H

#include <inttypes.h>
#include <compiler.h>
#include <arch/io.h>
#include <device/device.h>
#include "early_ht.h"

#include "inline_helper.c"
struct DCTStatStruc;
struct MCTStatStruc;

#define RES_PCI_IO 0x10
#define RES_PORT_IO_8 0x22
#define RES_PORT_IO_32 0x20
#define RES_MEM_IO 0x40

#define NODE_ID		0x60
#define HT_INIT_CONTROL	0x6c
#define HTIC_ColdR_Detect	(1<<4)
#define HTIC_BIOSR_Detect	(1<<5)
#define HTIC_INIT_Detect	(1<<6)

/* Definitions of various FAM10 registers */
/* Function 0 */
#define HT_TRANSACTION_CONTROL 0x68
#define  HTTC_DIS_RD_B_P		(1 << 0)
#define  HTTC_DIS_RD_DW_P		(1 << 1)
#define  HTTC_DIS_WR_B_P		(1 << 2)
#define  HTTC_DIS_WR_DW_P		(1 << 3)
#define  HTTC_DIS_MTS			(1 << 4)
#define  HTTC_CPU1_EN			(1 << 5)
#define  HTTC_CPU_REQ_PASS_PW		(1 << 6)
#define  HTTC_CPU_RD_RSP_PASS_PW	(1 << 7)
#define  HTTC_DIS_P_MEM_C		(1 << 8)
#define  HTTC_DIS_RMT_MEM_C		(1 << 9)
#define  HTTC_DIS_FILL_P		(1 << 10)
#define  HTTC_RSP_PASS_PW		(1 << 11)
#define  HTTC_BUF_REL_PRI_SHIFT	13
#define  HTTC_BUF_REL_PRI_MASK		3
#define   HTTC_BUF_REL_PRI_64		0
#define   HTTC_BUF_REL_PRI_16		1
#define   HTTC_BUF_REL_PRI_8		2
#define   HTTC_BUF_REL_PRI_2		3
#define  HTTC_LIMIT_CLDT_CFG		(1 << 15)
#define  HTTC_LINT_EN			(1 << 16)
#define  HTTC_APIC_EXT_BRD_CST		(1 << 17)
#define  HTTC_APIC_EXT_ID		(1 << 18)
#define  HTTC_APIC_EXT_SPUR		(1 << 19)
#define  HTTC_SEQ_ID_SRC_NODE_EN	(1 << 20)
#define  HTTC_DS_NP_REQ_LIMIT_SHIFT	21
#define  HTTC_DS_NP_REQ_LIMIT_MASK	3
#define   HTTC_DS_NP_REQ_LIMIT_NONE	0
#define   HTTC_DS_NP_REQ_LIMIT_1	1
#define   HTTC_DS_NP_REQ_LIMIT_4	2
#define   HTTC_DS_NP_REQ_LIMIT_8	3


/* Function 1 */
#define PCI_IO_BASE0	0xc0
#define PCI_IO_BASE1	0xc8
#define PCI_IO_BASE2	0xd0
#define PCI_IO_BASE3	0xd8
#define PCI_IO_BASE_VGA_EN	(1 << 4)
#define PCI_IO_BASE_NO_ISA	(1 << 5)

/* Function 2 */
// 0x1xx is for DCT1
#define DRAM_CSBASE	0x40
#define DRAM_CSMASK	0x60
#define DRAM_BANK_ADDR_MAP 0x80

#define DRAM_CTRL	0x78
#define  DC_RdPtrInit_SHIFT 0
#define  DC_RdPrtInit_MASK  0xf
#define  DC_Twrrd3_2_SHIFT 8  /*DDR3 */
#define  DC_Twrrd3_2_MASK 3
#define  DC_Twrwr3_2_SHIFT 10 /*DDR3 */
#define  DC_Twrwr3_2_MASK 3
#define  DC_Trdrd3_2_SHIFT 12 /*DDR3 */
#define  DC_Trdrd3_2_MASK 3
#define  DC_AltVidC3MemClkTriEn (1<<16)
#define  DC_DqsRcvEnTrain (1<<18)
#define  DC_MaxRdLatency_SHIFT 22
#define  DC_MaxRdLatency_MASK 0x3ff

#define DRAM_INIT	0x7c
#define  DI_MrsAddress_SHIFT 0
#define  DI_MrsAddress_MASK 0xffff
#define  DI_MrsBank_SHIFT 16
#define  DI_MrsBank_MASK 7
#define  DI_MrsChipSel_SHIFT 20
#define  DI_MrsChipSel_MASK 7
#define  DI_SendRchgAll (1<<24)
#define  DI_SendAutoRefresh (1<<25)
#define  DI_SendMrsCmd	  (1<<26)
#define  DI_DeassertMemRstX (1<<27)
#define  DI_AssertCke	 (1<<28)
#define  DI_SendZQCmd	(1<<29) /*DDR3 */
#define  DI_EnMrsCmd	(1<<30)
#define  DI_EnDramInit	 (1<<31)

#define DRAM_MRS	0x84
#define  DM_BurstCtrl_SHIFT 0
#define  DM_BurstCtrl_MASK 3
#define  DM_DrvImpCtrl_SHIFT 2 /* DDR3 */
#define  DM_DrvImpCtrl_MASK 3
#define  DM_Twr_SHIFT 4 /* DDR3 */
#define  DM_Twr_MASK 7
#define  DM_Twr_BASE 4
#define  DM_Twr_MIN  5
#define  DM_Twr_MAX  12
#define  DM_DramTerm_SHIFT 7 /*DDR3 */
#define  DM_DramTerm_MASK 7
#define  DM_DramTermDyn_SHIFT 10 /* DDR3 */
#define  DM_DramTermDyn_MASK 3
#define  DM_Ooff (1<<13)
#define  DM_ASR (1<<18)
#define  DM_SRT (1<<19)
#define  DM_Tcwl_SHIFT 20
#define  DM_Tcwl_MASK 7
#define  DM_PchgPDModeSel (1<<23) /* DDR3 */
#define  DM_MPrLoc_SHIFT 24 /* DDR3 */
#define  DM_MPrLoc_MASK 3
#define  DM_MprEn (1<<26) /* DDR3 */

#define DRAM_TIMING_LOW	   0x88
#define	 DTL_TCL_SHIFT	   0
#define	 DTL_TCL_MASK	   0xf
#define	  DTL_TCL_BASE	   1 /* DDR3 =4 */
#define	  DTL_TCL_MIN	   3 /* DDR3 =4 */
#define	  DTL_TCL_MAX	   6 /* DDR3 =12 */
#define	 DTL_TRCD_SHIFT	   4
#define	 DTL_TRCD_MASK	   3 /* DDR3 =7 */
#define	  DTL_TRCD_BASE	   3 /* DDR3 =5 */
#define	  DTL_TRCD_MIN	   3 /* DDR3 =5 */
#define   DTL_TRCD_MAX	    6 /* DDR3 =12 */
#define	 DTL_TRP_SHIFT	   8 /* DDR3 =7 */
#define	 DTL_TRP_MASK	   3 /* DDR3 =7 */
#define	  DTL_TRP_BASE	   3 /* DDR3 =5 */
#define	  DTL_TRP_MIN	   3 /* DDR3 =5 */
#define   DTL_TRP_MAX	    6 /* DDR3 =12 */
#define	 DTL_TRTP_SHIFT	   11 /*DDR3 =10 */
#define	 DTL_TRTP_MASK	   1  /*DDR3 =3 */
#define	  DTL_TRTP_BASE	   2  /* DDR3 =4 */
#define	  DTL_TRTP_MIN	   2  /* 4 for 64 bytes*/  /* DDR3 =4 for 32bytes or 64bytes */
#define   DTL_TRTP_MAX	    3  /* 5 for 64 bytes */ /* DDR3 =7 for 32Bytes or 64bytes */
#define	 DTL_TRAS_SHIFT	   12
#define	 DTL_TRAS_MASK	   0xf
#define	  DTL_TRAS_BASE	   3 /* DDR3 =15 */
#define	  DTL_TRAS_MIN	   5 /* DDR3 =15 */
#define	  DTL_TRAS_MAX	   18 /*DDR3 =30 */
#define	 DTL_TRC_SHIFT	   16
#define	 DTL_TRC_MASK	   0xf /* DDR3 =0x1f */
#define	  DTL_TRC_BASE	   11
#define	  DTL_TRC_MIN	   11
#define	  DTL_TRC_MAX	   26  /* DDR3 =43 */
#define	 DTL_TWR_SHIFT	   20 /* only for DDR2, DDR3's is on DC */
#define	 DTL_TWR_MASK	   3
#define	  DTL_TWR_BASE	   3
#define	  DTL_TWR_MIN	   3
#define	  DTL_TWR_MAX	   6
#define  DTL_TRRD_SHIFT    22
#define   DTL_TRRD_MASK    3
#define   DTL_TRRD_BASE    2 /* DDR3 =4 */
#define   DTL_TRRD_MIN	   2 /* DDR3 =4 */
#define   DTL_TRRD_MAX	    5 /* DDR3 =7 */
#define  DTL_MemClkDis_SHIFT 24    /* Channel A */
#define  DTL_MemClkDis3       (1 << 26)
#define  DTL_MemClkDis2       (1 << 27)
#define  DTL_MemClkDis1       (1 << 28)
#define  DTL_MemClkDis0       (1 << 29)
/* DTL_MemClkDis for m2 and s1g1 is different */

#define DRAM_TIMING_HIGH   0x8c
#define  DTH_TRWTWB_SHIFT 0
#define  DTH_TRWTWB_MASK 3
#define   DTH_TRWTWB_BASE 3  /* DDR3 =4 */
#define   DTH_TRWTWB_MIN  3  /* DDR3 =5 */
#define   DTH_TRWTWB_MAX  10 /* DDR3 =11 */
#define  DTH_TRWTTO_SHIFT  4
#define  DTH_TRWTTO_MASK   7
#define   DTH_TRWTTO_BASE   2 /* DDR3 =3 */
#define   DTH_TRWTTO_MIN    2 /* DDR3 =3 */
#define   DTH_TRWTTO_MAX    9 /* DDR3 =10 */
#define	 DTH_TWTR_SHIFT	   8
#define	 DTH_TWTR_MASK	   3
#define	  DTH_TWTR_BASE	   0  /* DDR3 =4 */
#define	  DTH_TWTR_MIN	   1  /* DDR3 =4 */
#define	  DTH_TWTR_MAX	   3  /* DDR3 =7 */
#define	 DTH_TWRRD_SHIFT   10
#define	 DTH_TWRRD_MASK	   3	/* For DDR3 3_2 is at 0x78 DC */
#define	  DTH_TWRRD_BASE   0  /* DDR3 =0 */
#define	  DTH_TWRRD_MIN	   0  /* DDR3 =2 */
#define	  DTH_TWRRD_MAX	   3  /* DDR3 =12 */
#define  DTH_TWRWR_SHIFT   12
#define  DTH_TWRWR_MASK    3	/* For DDR3 3_2 is at 0x78 DC */
#define   DTH_TWRWR_BASE   1
#define   DTH_TWRWR_MIN    1	/* DDR3 =3 */
#define   DTH_TWRWR_MAX    3	/* DDR3 =12 */
#define  DTH_TRDRD_SHIFT   14
#define  DTH_TRDRD_MASK    3  /* For DDR3 3_2 is at 0x78 DC */
#define   DTH_TRDRD_BASE   2
#define   DTH_TRDRD_MIN    2
#define   DTH_TRDRD_MAX    5	/* DDR3 =10 */
#define	 DTH_TREF_SHIFT	   16
#define	 DTH_TREF_MASK	   3
#define	  DTH_TREF_7_8_US  2
#define	  DTH_TREF_3_9_US  3
#define  DTH_DisAutoRefresh (1<<18)
#define  DTH_TRFC0_SHIFT   20 /* for Logical DIMM0 */
#define  DTH_TRFC_MASK	     7
#define	  DTH_TRFC_75_256M   0
#define	  DTH_TRFC_105_512M  1
#define   DTH_TRFC_127_5_1G  2
#define   DTH_TRFC_195_2G    3
#define   DTH_TRFC_327_5_4G  4
#define  DTH_TRFC1_SHIFT   23 /*for Logical DIMM1 */
#define  DTH_TRFC2_SHIFT   26 /*for Logical DIMM2 */
#define  DTH_TRFC3_SHIFT   29 /*for Logical DIMM3 */

#define DRAM_CONFIG_LOW	   0x90
#define	 DCL_InitDram	   (1<<0)
#define	 DCL_ExitSelfRef   (1<<1)
#define  DCL_PllLockTime_SHIFT 2
#define  DCL_PllLockTime_MASK 3
#define   DCL_PllLockTime_15US 0
#define   DCL_PllLockTime_6US 1
#define  DCL_DramTerm_SHIFT 4
#define  DCL_DramTerm_MASK  3
#define   DCL_DramTerm_No   0
#define   DCL_DramTerm_75_OH 1
#define   DCL_DramTerm_150_OH 2
#define   DCL_DramTerm_50_OH 3
#define  DCL_DisDqsBar	    (1<<6) /* only for DDR2 */
#define  DCL_DramDrvWeak   (1<<7) /* only for DDR2 */
#define  DCL_ParEn	   (1<<8)
#define  DCL_SelfRefRateEn (1<<9) /* only for DDR2 */
#define  DCL_BurstLength32 (1<<10) /* only for DDR3 */
#define  DCL_Width128	   (1<<11)
#define  DCL_X4Dimm_SHIFT  12
#define  DCL_X4Dimm_MASK   0xf
#define  DCL_UnBuffDimm    (1<<16)
#define  DCL_EnPhyDqsRcvEnTr (1<<18)
#define	 DCL_DimmEccEn	   (1<<19)
#define  DCL_DynPageCloseEn (1<<20)
#define  DCL_IdleCycInit_SHIFT 21
#define  DCL_IdleCycInit_MASK	3
#define   DCL_IdleCycInit_16CLK 0
#define   DCL_IdleCycInit_32CLK 1
#define   DCL_IdleCycInit_64CLK 2
#define   DCL_IdleCycInit_96CLK 3
#define  DCL_ForceAutoPchg   (1<<23)

#define DRAM_CONFIG_HIGH   0x94
#define  DCH_MemClkFreq_SHIFT 0
#define  DCH_MemClkFreq_MASK  7
#define   DCH_MemClkFreq_200MHz 0  /* DDR2 */
#define   DCH_MemClkFreq_266MHz 1  /* DDR2 */
#define   DCH_MemClkFreq_333MHz 2  /* DDR2 */
#define	  DCH_MemClkFreq_400MHz 3  /* DDR2 and DDR 3*/
#define	  DCH_MemClkFreq_533MHz 4  /* DDR 3 */
#define	  DCH_MemClkFreq_667MHz 5  /* DDR 3 */
#define	  DCH_MemClkFreq_800MHz 6  /* DDR 3 */
#define  DCH_MemClkFreqVal	(1<<3)
#define  DCH_Ddr3Mode		(1<<8)
#define  DCH_LegacyBiosMode	(1<<9)
#define  DCH_ZqcsInterval_SHIFT 10
#define  DCH_ZqcsInterval_MASK	3
#define  DCH_ZqcsInterval_DIS	0
#define  DCH_ZqcsInterval_64MS	 1
#define  DCH_ZqcsInterval_128MS  2
#define  DCH_ZqcsInterval_256MS  3
#define  DCH_RDqsEn	      (1<<12) /* only for DDR2 */
#define  DCH_DisSimulRdWr	(1<<13)
#define  DCH_DisDramInterface (1<<14)
#define  DCH_PowerDownEn      (1<<15)
#define  DCH_PowerDownMode_SHIFT 16
#define  DCH_PowerDownMode_MASK 1
#define   DCH_PowerDownMode_Channel_CKE 0
#define   DCH_PowerDownMode_ChipSelect_CKE 1
#define  DCH_FourRankSODimm	(1<<17)
#define  DCH_FourRankRDimm	(1<<18)
#define  DCH_SlowAccessMode	(1<<20)
#define  DCH_BankSwizzleMode	 (1<<22)
#define  DCH_DcqBypassMax_SHIFT 24
#define  DCH_DcqBypassMax_MASK	 0xf
#define   DCH_DcqBypassMax_BASE 0
#define   DCH_DcqBypassMax_MIN	 0
#define   DCH_DcqBypassMax_MAX	 15
#define  DCH_FourActWindow_SHIFT 28
#define  DCH_FourActWindow_MASK 0xf
#define   DCH_FourActWindow_BASE 7 /* DDR3 15 */
#define   DCH_FourActWindow_MIN 8  /* DDR3 16 */
#define   DCH_FourActWindow_MAX 20 /* DDR3 30 */


// for 0x98 index and 0x9c data for DCT0
// for 0x198 index and 0x19c data for DCT1
// even at ganged mode, 0x198/0x19c will be used for channel B

#define DRAM_CTRL_ADDI_DATA_OFFSET	0x98
#define  DCAO_DctOffset_SHIFT	0
#define  DCAO_DctOffset_MASK	0x3fffffff
#define  DCAO_DctAccessWrite	(1<<30)
#define  DCAO_DctAccessDone	(1<<31)

#define DRAM_CTRL_ADDI_DATA_PORT	 0x9c

#define DRAM_OUTPUT_DRV_COMP_CTRL	0x00
#define  DODCC_CkeDrvStren_SHIFT 0
#define  DODCC_CkeDrvStren_MASK  3
#define   DODCC_CkeDrvStren_1_0X  0
#define   DODCC_CkeDrvStren_1_25X 1
#define   DODCC_CkeDrvStren_1_5X  2
#define   DODCC_CkeDrvStren_2_0X  3
#define  DODCC_CsOdtDrvStren_SHIFT 4
#define  DODCC_CsOdtDrvStren_MASK  3
#define   DODCC_CsOdtDrvStren_1_0X  0
#define   DODCC_CsOdtDrvStren_1_25X 1
#define   DODCC_CsOdtDrvStren_1_5X  2
#define   DODCC_CsOdtDrvStren_2_0X  3
#define  DODCC_AddrCmdDrvStren_SHIFT 8
#define  DODCC_AddrCmdDrvStren_MASK  3
#define   DODCC_AddrCmdDrvStren_1_0X  0
#define   DODCC_AddrCmdDrvStren_1_25X 1
#define   DODCC_AddrCmdDrvStren_1_5X  2
#define   DODCC_AddrCmdDrvStren_2_0X  3
#define  DODCC_ClkDrvStren_SHIFT 12
#define  DODCC_ClkDrvStren_MASK  3
#define   DODCC_ClkDrvStren_0_75X  0
#define   DODCC_ClkDrvStren_1_0X 1
#define   DODCC_ClkDrvStren_1_25X  2
#define   DODCC_ClkDrvStren_1_5X  3
#define  DODCC_DataDrvStren_SHIFT 16
#define  DODCC_DataDrvStren_MASK  3
#define   DODCC_DataDrvStren_0_75X  0
#define   DODCC_DataDrvStren_1_0X 1
#define   DODCC_DataDrvStren_1_25X  2
#define   DODCC_DataDrvStren_1_5X  3
#define  DODCC_DqsDrvStren_SHIFT 20
#define  DODCC_DqsDrvStren_MASK  3
#define   DODCC_DqsDrvStren_0_75X  0
#define   DODCC_DqsDrvStren_1_0X 1
#define   DODCC_DqsDrvStren_1_25X  2
#define   DODCC_DqsDrvStren_1_5X  3
#define  DODCC_ProcOdt_SHIFT 28
#define  DODCC_ProcOdt_MASK  3
#define   DODCC_ProcOdt_300_OHMS  0
#define   DODCC_ProcOdt_150_OHMS 1
#define   DODCC_ProcOdt_75_OHMS  2

/*
   for DDR2 400, 533, 667, F2x[1,0]9C_x[02:01], [03], [06:05], [07] control timing of all DIMMs
   for DDR2 800, DDR3 800, 1067, 1333, 1600, F2x[1,0]9C_x[02:01], [03], [06:05], [07] control timing of DIMM0
					     F2x[1,0]9C_x[102:101], [103], [106:105], [107] control timing of DIMM1
	So Socket F with Four Logical DIMM will only support DDR2 800  ?
*/
/* there are index	   +100	   ===> for DIMM1
that are corresponding to 0x01, 0x02, 0x03, 0x05, 0x06, 0x07
*/
//02/15/2006 18:37
#define DRAM_WRITE_DATA_TIMING_CTRL_LOW 0x01
#define  DWDTC_WrDatFineDlyByte0_SHIFT 0
#define  DWDTC_WrDatFineDlyByte_MASK  0x1f
#define   DWDTC_WrDatFineDlyByte_BASE 0
#define   DWDTC_WrDatFineDlyByte_MIN  0
#define   DWDTC_WrDatFineDlyByte_MAX  31 // 1/64 MEMCLK
#define  DWDTC_WrDatGrossDlyByte0_SHIFT 5
#define  DWDTC_WrDatGrossDlyByte_MASK	0x3
#define   DWDTC_WrDatGrossDlyByte_NO_DELAY 0
#define   DWDTC_WrDatGrossDlyByte_0_5_	 1
#define   DWDTC_WrDatGrossDlyByte_1  2
#define  DWDTC_WrDatFineDlyByte1_SHIFT 8
#define  DWDTC_WrDatGrossDlyByte1_SHIFT 13
#define  DWDTC_WrDatFineDlyByte2_SHIFT 16
#define  DWDTC_WrDatGrossDlyByte2_SHIFT 21
#define  DWDTC_WrDatFineDlyByte3_SHIFT 24
#define  DWDTC_WrDatGrossDlyByte3_SHIFT 29

#define DRAM_WRITE_DATA_TIMING_CTRL_HIGH 0x02
#define  DWDTC_WrDatFineDlyByte4_SHIFT 0
#define  DWDTC_WrDatGrossDlyByte4_SHIFT 5
#define  DWDTC_WrDatFineDlyByte5_SHIFT 8
#define  DWDTC_WrDatGrossDlyByte5_SHIFT 13
#define  DWDTC_WrDatFineDlyByte6_SHIFT 16
#define  DWDTC_WrDatGrossDlyByte6_SHIFT 21
#define  DWDTC_WrDatFineDlyByte7_SHIFT 24
#define  DWDTC_WrDatGrossDlyByte7_SHIFT 29

#define DRAM_WRITE_ECC_TIMING_CTRL 0x03
#define  DWETC_WrChkFinDly_SHIFT 0
#define  DWETC_WrChkGrossDly_SHIFT 5

#define DRAM_ADDR_CMD_TIMING_CTRL 0x04
#define  DACTC_CkeFineDelay_SHIFT 0
#define  DACTC_CkeFineDelay_MASK  0x1f
#define   DACTC_CkeFineDelay_BASE 0
#define   DACTC_CkeFineDelay_MIN  0
#define   DACTC_CkeFineDelay_MAX 31
#define  DACTC_CkeSetup	(1<<5)
#define  DACTC_CsOdtFineDelay_SHIFT 8
#define  DACTC_CsOdtFineDelay_MASK  0x1f
#define   DACTC_CsOdtFineDelay_BASE 0
#define   DACTC_CsOdtFineDelay_MIN  0
#define   DACTC_CsOdtFineDelay_MAX 31
#define  DACTC_CsOdtSetup   (1<<13)
#define  DACTC_AddrCmdFineDelay_SHIFT 16
#define  DACTC_AddrCmdFineDelay_MASK  0x1f
#define   DACTC_AddrCmdFineDelay_BASE 0
#define   DACTC_AddrCmdFineDelay_MIN  0
#define   DACTC_AddrCmdFineDelay_MAX 31
#define  DACTC_AddrCmdSetup   (1<<21)

#define DRAM_READ_DQS_TIMING_CTRL_LOW 0x05
#define  DRDTC_RdDqsTimeByte0_SHIFT 0
#define  DRDTC_RdDqsTimeByte_MASK  0x3f
#define   DRDTC_RdDqsTimeByte_BASE 0
#define   DRDTC_RdDqsTimeByte_MIN  0
#define   DRDTC_RdDqsTimeByte_MAX  63 // 1/128 MEMCLK
#define  DRDTC_RdDqsTimeByte1_SHIFT 8
#define  DRDTC_RdDqsTimeByte2_SHIFT 16
#define  DRDTC_RdDqsTimeByte3_SHIFT 24

#define DRAM_READ_DQS_TIMING_CTRL_HIGH 0x06
#define  DRDTC_RdDqsTimeByte4_SHIFT 0
#define  DRDTC_RdDqsTimeByte5_SHIFT 8
#define  DRDTC_RdDqsTimeByte6_SHIFT 16
#define  DRDTC_RdDqsTimeByte7_SHIFT 24

#define DRAM_READ_DQS_ECC_TIMING_CTRL 0x07
#define  DRDETC_RdDqsTimeCheck_SHIFT 0

#define DRAM_PHY_CTRL 0x08
#define  DPC_WrtLvTrEn	(1<<0)
#define  DPC_WrtLvTrMode (1<<1)
#define  DPC_TrNibbleSel (1<<2)
#define  DPC_TrDimmSel_SHIFT 4
#define   DPC_TrDimmSel_MASK 3 /* 0-->dimm0, 1-->dimm1, 2--->dimm2, 3--->dimm3 */
#define  DPC_WrLvOdt_SHIFT 8
#define   DPC_WrLvOdt_MASK 0xf /* bit 0-->odt 0, ...*/
#define  DPC_WrLvODtEn (1<<12)
#define  DPC_DqsRcvTrEn (1<<13)
#define  DPC_DisAutoComp (1<<30)
#define  DPC_AsyncCompUpdate (1<<31)

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_0_0 0x10 //DIMM0 Channel A
#define  DDRETC_DqsRcvEnFineDelayByte0_SHIFT 0
#define   DDRETC_DqsRcvEnFineDelayByte0_MASK 0x1f
#define  DDRETC_DqsRcvEnGrossDelayByte0_SHIFT 5
#define   DDRETC_DqsRcvEnGrossDelayByte0_MASK 0x3
#define  DDRETC_DqsRcvEnFineDelayByte1_SHIFT 8
#define  DDRETC_DqsRcvEnGrossDelayByte1_SHIFT 13
#define  DDRETC_DqsRcvEnFineDelayByte2_SHIFT 16
#define  DDRETC_DqsRcvEnGrossDelayByte2_SHIFT 21
#define  DDRETC_DqsRcvEnFineDelayByte3_SHIFT 24
#define  DDRETC_DqsRcvEnGrossDelayByte3_SHIFT 29

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_0_1 0x11 //DIMM0 Channel A
#define  DDRETC_DqsRcvEnFineDelayByte4_SHIFT 0
#define  DDRETC_DqsRcvEnGrossDelayByte4_SHIFT 5
#define  DDRETC_DqsRcvEnFineDelayByte5_SHIFT 8
#define  DDRETC_DqsRcvEnGrossDelayByte5_SHIFT 13
#define  DDRETC_DqsRcvEnFineDelayByte6_SHIFT 16
#define  DDRETC_DqsRcvEnGrossDelayByte6_SHIFT 21
#define  DDRETC_DqsRcvEnFineDelayByte7_SHIFT 24
#define  DDRETC_DqsRcvEnGrossDelayByte7_SHIFT 29

#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_0_0 0x12
#define  DDRETCE_WrChkFineDlyByte0_SHIFT 0
#define  DDRETCE_WrChkGrossDlyByte0_SHIFT 5

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_0_2 0x20   //DIMM0 channel B
#define  DDRETC_DqsRcvEnFineDelayByte8_SHIFT 0
#define  DDRETC_DqsRcvEnGrossDelayByte8_SHIFT 5
#define  DDRETC_DqsRcvEnFineDelayByte9_SHIFT 8
#define  DDRETC_DqsRcvEnGrossDelayByte9_SHIFT 13
#define  DDRETC_DqsRcvEnFineDelayByte10_SHIFT 16
#define  DDRETC_DqsRcvEnGrossDelayByte10_SHIFT 21
#define  DDRETC_DqsRcvEnFineDelayByte11_SHIFT 24
#define  DDRETC_DqsRcvEnGrossDelayByte11_SHIFT 29

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_0_3 0x21  // DIMM0 Channel B
#define  DDRETC_DqsRcvEnFineDelayByte12_SHIFT 0
#define  DDRETC_DqsRcvEnGrossDelayByte12_SHIFT 5
#define  DDRETC_DqsRcvEnFineDelayByte13_SHIFT 8
#define  DDRETC_DqsRcvEnGrossDelayByte13_SHIFT 13
#define  DDRETC_DqsRcvEnFineDelayByte14_SHIFT 16
#define  DDRETC_DqsRcvEnGrossDelayByte14_SHIFT 21
#define  DDRETC_DqsRcvEnFineDelayByte15_SHIFT 24
#define  DDRETC_DqsRcvEnGrossDelayByte15_SHIFT 29

#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_0_1 0x22
#define  DDRETCE_WrChkFineDlyByte1_SHIFT 0
#define  DDRETCE_WrChkGrossDlyByte1_SHIFT 5

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_1_0 0x13  //DIMM1
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_1_1 0x14
#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_1_0 0x15
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_1_2 0x23
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_1_3 0x24
#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_1_1 0x25

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_2_0 0x16   // DIMM2
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_2_1 0x17
#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_2_0 0x18
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_2_2 0x26
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_2_3 0x27
#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_2_1 0x28

#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_3_0 0x19   // DIMM3
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_3_1 0x1a
#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_3_0 0x1b
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_3_2 0x29
#define DRAM_DQS_RECV_ENABLE_TIME_CTRL_3_3 0x2a
#define DRAM_DQS_RECV_ENABLE_TIMING_CTRL_ECC_3_1 0x2b

/* 04.06.2006 19:12 */

#define DRAM_PHASE_RECOVERY_CTRL_0 0x50
#define  DPRC_PhRecFineDlyByte0_SHIFT 0
#define   DDWTC_PhRecFineDlyByte0_MASK 0x1f
#define  DDWTC_PhRecGrossDlyByte0_SHIFT 5
#define   DDWTC_PhRecGrossDlyByte0_MASK 0x3
#define  DDWTC_PhRecFineDlyByte1_SHIFT 8
#define  DDWTC_PhRecGrossDlyByte1_SHIFT 13
#define  DDWTC_PhRecFineDlyByte2_SHIFT 16
#define  DDWTC_PhRecGrossDlyByte2_SHIFT 21
#define  DDWTC_PhRecFineDlyByte3_SHIFT 24
#define  DDWTC_PhRecGrossDlyByte3_SHIFT 29

#define DRAM_PHASE_RECOVERY_CTRL_1 0x51
#define  DPRC_PhRecFineDlyByte4_SHIFT 0
#define  DDWTC_PhRecGrossDlyByte4_SHIFT 5
#define  DDWTC_PhRecFineDlyByte5_SHIFT 8
#define  DDWTC_PhRecGrossDlyByte5_SHIFT 13
#define  DDWTC_PhRecFineDlyByte6_SHIFT 16
#define  DDWTC_PhRecGrossDlyByte6_SHIFT 21
#define  DDWTC_PhRecFineDlyByte7_SHIFT 24
#define  DDWTC_PhRecGrossDlyByte7_SHIFT 29

#define DRAM_ECC_PHASE_RECOVERY_CTRL 0x52
#define  DEPRC_PhRecEccDlyByte0_SHIFT 0
#define  DEPRC_PhRecEccGrossDlyByte0_SHIFT 5

#define DRAM_WRITE_LEVEL_ERROR 0x53 /* read only */
#define  DWLE_WrLvErr_SHIFT 0
#define   DWLE_WrLvErr_MASK 0xff

#define DRAM_CTRL_MISC 0xa0
#define  DCM_MemCleared (1<<0) /* RD  == F2x110 [MemCleared] */
#define  DCM_DramEnabled (1<<9) /* RD == F2x110 [DramEnabled] */

#define NB_TIME_STAMP_COUNT_LOW 0xb0
#define  TscLow_SHIFT 0
#define  TscLow_MASK 0xffffffff

#define NB_TIME_STAMP_COUNT_HIGH 0xb4
#define  TscHigh_SHIFT 0
#define  TscHigh_Mask 0xff

#define DCT_DEBUG_CTRL 0xf0 /* 0xf0 for DCT0,	0x1f0 is for DCT1*/
#define  DDC_DllAdjust_SHIFT 0
#define   DDC_DllAdjust_MASK 0xff
#define  DDC_DllSlower (1<<8)
#define  DDC_DllFaster (1<<9)
#define  DDC_WrtDqsAdjust_SHIFT 16
#define   DDC_WrtDqsAdjust_MASK 0x7
#define  DDC_WrtDqsAdjustEn (1<<19)

#define DRAM_CTRL_SEL_LOW 0x110
#define  DCSL_DctSelHiRngEn (1<<0)
#define  DCSL_DctSelHi (1<<1)
#define  DCSL_DctSelIntLvEn (1<<2)
#define  DCSL_MemClrInit (1<<3) /* WR only */
#define  DCSL_DctGangEn (1<<4)
#define  DCSL_DctDataIntLv (1<<5)
#define  DCSL_DctSelIntLvAddr_SHIFT
#define   DCSL_DctSelIntLvAddr_MASK 3
#define  DCSL_DramEnable (1<<8)  /* RD only */
#define  DCSL_MemClrBusy (1<<9)  /* RD only */
#define  DCSL_MemCleared (1<<10) /* RD only */
#define  DCSL_DctSelBaseAddr_47_27_SHIFT 11
#define   DCSL_DctSelBaseAddr_47_27_MASK 0x1fffff

#define DRAM_CTRL_SEL_HIGH 0x114
#define  DCSH_DctSelBaseOffset_47_26_SHIFT 10
#define   DCSH_DctSelBaseOffset_47_26_MASK 0x3fffff

#define MEM_CTRL_CONF_LOW 0x118
#define  MCCL_MctPriCpuRd (1<<0)
#define  MCCL_MctPriCpuWr (1<<1)
#define  MCCL_MctPriIsocRd_SHIFT 4
#define   MCCL_MctPriIsoc_MASK 0x3
#define  MCCL_MctPriIsocWr_SHIFT 6
#define   MCCL_MctPriIsocWe_MASK 0x3
#define  MCCL_MctPriDefault_SHIFT 8
#define   MCCL_MctPriDefault_MASK 0x3
#define  MCCL_MctPriWr_SHIFT 10
#define   MCCL_MctPriWr_MASK 0x3
#define  MCCL_MctPriIsoc_SHIFT 12
#define   MCCL_MctPriIsoc_MASK 0x3
#define  MCCL_MctPriTrace_SHIFT 14
#define   MCCL_MctPriTrace_MASK 0x3
#define  MCCL_MctPriScrub_SHIFT 16
#define   MCCL_MctPriScrub_MASK 0x3
#define  MCCL_McqMedPriByPassMax_SHIFT 20
#define   MCCL_McqMedPriByPassMax_MASK 0x7
#define  MCCL_McqHiPriByPassMax_SHIFT 24
#define   MCCL_McqHiPriByPassMax_MASK 0x7
#define  MCCL_MctVarPriCntLmt_SHIFT 28
#define   MCCL_MctVarPriCntLmt_MASK 0x7

#define MEM_CTRL_CONF_HIGH 0x11c
#define  MCCH_DctWrLimit_SHIFT 0
#define   MCCH_DctWrLimit_MASK 0x3
#define  MCCH_MctWrLimit_SHIFT 2
#define   MCCH_MctWrLimit_MASK 0x1f
#define  MCCH_MctPrefReqLimit_SHIFT 7
#define   MCCH_MctPrefReqLimit_MASK 0x1f
#define  MCCH_PrefCpuDis (1<<12)
#define  MCCH_PrefIoDis (1<<13)
#define  MCCH_PrefIoFixStrideEn (1<<14)
#define  MCCH_PrefFixStrideEn (1<<15)
#define  MCCH_PrefFixDist_SHIFT 16
#define   MCCH_PrefFixDist_MASK 0x3
#define  MCCH_PrefConfSat_SHIFT 18
#define   MCCH_PrefConfSat_MASK 0x3
#define  MCCH_PrefOneConf_SHIFT 20
#define   MCCH_PrefOneConf_MASK 0x3
#define  MCCH_PrefTwoConf_SHIFT 22
#define   MCCH_PrefTwoConf_MASK 0x7
#define  MCCH_PrefThreeConf_SHIFT 25
#define   MCCH_prefThreeConf_MASK 0x7
#define  MCCH_PrefDramTrainMode (1<<28)
#define  MCCH_FlushWrOnStpGnt (1<<29)
#define  MCCH_FlushWr (1<<30)
#define  MCCH_MctScrubEn (1<<31)


/* Function 3 */
#define MCA_NB_CONTROL		0x40
#define  MNCT_CorrEccEn (1<<0)
#define  MNCT_UnCorrEccEn (1<<1)
#define  MNCT_CrcErr0En	(1<<2) /* Link 0 */
#define  MNCT_CrcErr1En (1<<3)
#define  MNCT_CrcErr2En (1<<4)
#define  MBCT_SyncPkt0En (1<<5) /* Link 0 */
#define  MBCT_SyncPkt1En (1<<6)
#define  MBCT_SyncPkt2En (1<<7)
#define  MBCT_MstrAbrtEn (1<<8)
#define  MBCT_TgtAbrtEn	 (1<<9)
#define  MBCT_GartTblEkEn (1<<10)
#define  MBCT_AtomicRMWEn  (1<<11)
#define  MBCT_WdogTmrRptEn (1<<12)
#define  MBCT_DevErrEn	(1<<13)
#define  MBCT_L3ArrayCorEn (1<<14)
#define  MBCT_L3ArrayUncEn (1<<15)
#define  MBCT_HtProtEn	(1<<16)
#define  MBCT_HtDataEn	(1<<17)
#define  MBCT_DramParEn	(1<<18)
#define  MBCT_RtryHt0En (1<<19) /* Link 0 */
#define  MBCT_RtryHt1En (1<<20)
#define  MBCT_RtryHt2En (1<<21)
#define  MBCT_RtryHt3En (1<<22)
#define  MBCT_CrcErr3En (1<<23) /* Link 3*/
#define  MBCT_SyncPkt3En (1<<24) /* Link 4 */
#define  MBCT_McaUsPwDatErrEn (1<<25)
#define  MBCT_NbArrayParEn (1<<26)
#define  MBCT_TblWlkDatErrEn (1<<27)
#define  MBCT_FbDimmCorErrEn (1<<28)
#define  MBCT_FbDimmUnCorErrEn (1<<29)



#define MCA_NB_CONFIG	    0x44
#define   MNC_CpuRdDatErrEn   (1<<1)
#define   MNC_SyncOnUcEccEn   (1<<2)
#define   MNC_SynvPktGenDis   (1<<3)
#define   MNC_SyncPktPropDis  (1<<4)
#define   MNC_IoMstAbortDis   (1<<5)
#define   MNC_CpuErrDis	      (1<<6)
#define   MNC_IoErrDis	      (1<<7)
#define   MNC_WdogTmrDis      (1<<8)
#define   MNC_WdogTmrCntSel_2_0_SHIFT 9 /* 3 is ar f3x180 */
#define    MNC_WdogTmrCntSel_2_0_MASK 0x3
#define   MNC_WdogTmrBaseSel_SHIFT 12
#define    MNC_WdogTmrBaseSel_MASK 0x3
#define   MNC_LdtLinkSel_SHIFT 14
#define    MNC_LdtLinkSel_MASK 0x3
#define   MNC_GenCrcErrByte0	(1<<16)
#define   MNC_GenCrcErrByte1	(1<<17)
#define   MNC_SubLinkSel_SHIFT 18
#define    MNC_SubLinkSel_MASK 0x3
#define   MNC_SyncOnWdogEn  (1<<20)
#define   MNC_SyncOnAnyErrEn (1<<21)
#define   MNC_DramEccEn       (1<<22)
#define   MNC_ChipKillEccEn  (1<<23)
#define   MNC_IoRdDatErrEn (1<<24)
#define   MNC_DisPciCfgCpuErrRsp (1<<25)
#define   MNC_CorrMcaExcEn (1<<26)
#define   MNC_NbMcaToMstCpuEn (1<<27)
#define   MNC_DisTgtAbtCpuErrRsp (1<<28)
#define   MNC_DisMstAbtCpuErrRsp (1<<29)
#define   MNC_SyncOnDramAdrParErrEn (1<<30)
#define   MNC_NbMcaLogEn (1<<31)

#define MCA_NB_STATUS_LOW 0x48
#define  MNSL_ErrorCode_SHIFT 0
#define   MNSL_ErrorCode_MASK 0xffff
#define  MNSL_ErrorCodeExt_SHIFT 16
#define   MNSL_ErrorCodeExt_MASK 0x1f
#define  MNSL_Syndrome_15_8_SHIFT 24
#define   MNSL_Syndrome_15_8_MASK 0xff

#define MCA_NB_STATUS_HIGH 0x4c
#define  MNSH_ErrCPU_SHIFT 0
#define   MNSH_ErrCPU_MASK 0xf
#define  MNSH_LDTLink_SHIFT 4
#define   MNSH_LDTLink_MASK 0xf
#define  MNSH_ErrScrub (1<<8)
#define  MNSH_SubLink (1<<9)
#define  MNSH_McaStatusSubCache_SHIFT 10
#define   MNSH_McaStatusSubCache_MASK 0x3
#define  MNSH_Deffered (1<<12)
#define  MNSH_UnCorrECC (1<<13)
#define  MNSH_CorrECC (1<<14)
#define  MNSH_Syndrome_7_0_SHIFT 15
#define   MNSH_Syndrome_7_0_MASK 0xff
#define  MNSH_PCC (1<<25)
#define  MNSH_ErrAddrVal (1<<26)
#define  MNSH_ErrMiscVal (1<<27)
#define  MNSH_ErrEn  (1<<28)
#define  MNSH_ErrUnCorr (1<<29)
#define  MNSH_ErrOver (1<<30)
#define  MNSH_ErrValid (1<<31)

#define MCA_NB_ADDR_LOW 0x50
#define  MNAL_ErrAddr_31_1_SHIFT 1
#define   MNAL_ErrAddr_31_1_MASK 0x7fffffff

#define MCA_NB_ADDR_HIGH 0x54
#define  MNAL_ErrAddr_47_32_SHIFT 0
#define   MNAL_ErrAddr_47_32_MASK 0xffff

#define DRAM_SCRUB_RATE_CTRL	   0x58
#define	  SCRUB_NONE	    0
#define	  SCRUB_40ns	    1
#define	  SCRUB_80ns	    2
#define	  SCRUB_160ns	    3
#define	  SCRUB_320ns	    4
#define	  SCRUB_640ns	    5
#define	  SCRUB_1_28us	    6
#define	  SCRUB_2_56us	    7
#define	  SCRUB_5_12us	    8
#define	  SCRUB_10_2us	    9
#define	  SCRUB_20_5us	   0xa
#define	  SCRUB_41_0us	   0xb
#define	  SCRUB_81_9us	   0xc
#define	  SCRUB_163_8us	   0xd
#define	  SCRUB_327_7us	   0xe
#define	  SCRUB_655_4us	   0xf
#define	  SCRUB_1_31ms	   0x10
#define	  SCRUB_2_62ms	   0x11
#define	  SCRUB_5_24ms	   0x12
#define	  SCRUB_10_49ms	   0x13
#define	  SCRUB_20_97ms	   0x14
#define	  SCRUB_42ms	   0x15
#define	  SCRUB_84ms	   0x16
#define	 DSRC_DramScrub_SHFIT  0
#define	  DSRC_DramScrub_MASK	0x1f
#define	 DSRC_L2Scrub_SHIFT	   8
#define	  DSRC_L2Scrub_MASK	   0x1f
#define	 DSRC_DcacheScrub_SHIFT	  16
#define	  DSRC_DcacheScrub_MASK	   0x1f
#define	 DSRC_L3Scrub_SHIFT	   24
#define	  DSRC_L3Scrub_MASK	   0x1f

#define DRAM_SCRUB_ADDR_LOW	   0x5C
#define  DSAL_ScrubReDirEn (1<<0)
#define  DSAL_ScrubAddrLo_SHIFT 6
#define   DSAL_ScrubAddrLo_MASK 0x3ffffff

#define DRAM_SCRUB_ADDR_HIGH	   0x60
#define  DSAH_ScrubAddrHi_SHIFT 0
#define   DSAH_ScrubAddrHi_MASK 0xffff

#define HW_THERMAL_CTRL 0x64

#define SW_THERMAL_CTRL 0x68

#define DATA_BUF_CNT 0x6c

#define SRI_XBAR_CMD_BUF_CNT 0x70

#define XBAR_SRI_CMD_BUF_CNT 0x74

#define MCT_XBAR_CMD_BUF_CNT 0x78

#define ACPI_PWR_STATE_CTRL 0x80 /* till 0x84 */

#define NB_CONFIG_LOW 0x88
#define NB_CONFIG_HIGH 0x8c

#define GART_APERTURE_CTRL 0x90

#define GART_APERTURE_BASE 0x94

#define GART_TBL_BASE 0x98

#define GART_CACHE_CTRL 0x9c

#define PWR_CTRL_MISC 0xa0

#define RPT_TEMP_CTRL 0xa4

#define ON_LINE_SPARE_CTRL 0xb0

#define SBI_P_STATE_LIMIT 0xc4

#define CLK_PWR_TIMING_CTRL0 0xd4
#define CLK_PWR_TIMING_CTRL1 0xd8
#define CLK_PWR_TIMING_CTRL2 0xdc

#define THERMTRIP_STATUS 0xE4


#define NORTHBRIDGE_CAP	   0xE8
#define	 NBCAP_TwoChanDRAMcap	      (1 << 0)
#define	 NBCAP_DualNodeMPcap	      (1 << 1)
#define	 NBCAP_EightNodeMPcap	      (1 << 2)
#define	 NBCAP_ECCcap	      (1 << 3)
#define	 NBCAP_ChipkillECCcap	(1 << 4)
#define	 NBCAP_DdrMaxRate_SHIFT	  5
#define	 NBCAP_DdrMaxRate_MASK	  7
#define	  NBCAP_DdrMaxRate_400	7
#define	  NBCAP_DdrMaxRate_533	6
#define	  NBCAP_DdrMaxRate_667	5
#define	  NBCAP_DdrMaxRate_800	4
#define	  NBCAP_DdrMaxRate_1067 3
#define	  NBCAP_DdrMaxRate_1333 2
#define	  NBCAP_DdrMaxRate_1600 1
#define	  NBCAP_DdrMaxRate_3_2G	 6
#define	  NBCAP_DdrMaxRate_4_0G	 5
#define	  NBCAP_DdrMaxRate_4_8G	 4
#define	  NBCAP_DdrMaxRate_6_4G 3
#define	  NBCAP_DdrMaxRate_8_0G 2
#define	  NBCAP_DdrMaxRate_9_6G 1
#define	 NBCAP_Mem_ctrl_cap	      (1 << 8)
#define  MBCAP_SVMCap	     (1<<9)
#define  NBCAP_HtcCap		(1<<10)
#define  NBCAP_CmpCap_SHIFT	12
#define  NBCAP_CmpCap_MASK	3
#define  NBCAP_MpCap_SHIFT 16
#define  NBCAP_MpCap_MASK 7
#define   NBCAP_MpCap_1N 7
#define   NBCAP_MpCap_2N 6
#define   NBCAP_MpCap_4N 5
#define   NBCAP_MpCap_8N 4
#define   NBCAP_MpCap_32N 0
#define  NBCAP_UnGangEn_SHIFT 20
#define   NBCAP_UnGangEn_MASK 0xf
#define  NBCAP_L3Cap (1<<25)
#define  NBCAP_HtAcCap (1<<26)

/* 04/04/2006 18:00 */

#define EXT_NB_MCA_CTRL	0x180

#define NB_EXT_CONF	0x188
#define DOWNCORE_CTRL	0x190
#define  DWNCC_DisCore_SHIFT 0
#define	 DWNCC_DisCore_MASK 0xf

/* Function 5 for FBDIMM */
#define FBD_DRAM_TIMING_LOW

#define LinkConnected	   (1 << 0)
#define InitComplete	   (1 << 1)
#define NonCoherent	   (1 << 2)
#define ConnectionPending (1 << 4)

// Use the LAPIC timer count register to hold each core's init status
//   Format: byte 0 - state
//           byte 1 - fid_max
//           byte 2 - nb_cof_vid_update
//           byte 3 - apic id

#define LAPIC_MSG_REG 0x380
#define F10_APSTATE_STARTED 0x13  // start of AP execution
#define F10_APSTATE_ASLEEP  0x14  // AP sleeping
#define F10_APSTATE_STOPPED 0x15  // allow AP to stop
#define F10_APSTATE_RESET   0x01  // waiting for warm reset

#define MAX_CORES_SUPPORTED 128

#include "nums.h"

#ifdef __PRE_RAM__
#if NODE_NUMS == 64
	 #define NODE_PCI(x, fn) ((x < 32)?(PCI_DEV(CONFIG_CBB,(CONFIG_CDB+x),fn)):(PCI_DEV((CONFIG_CBB-1),(CONFIG_CDB+x-32),fn)))
#else
	 #define NODE_PCI(x, fn) PCI_DEV(CONFIG_CBB,(CONFIG_CDB+x),fn)
#endif
#endif

/* Include wrapper for MCT (works for DDR2 or DDR3) */
#include <northbridge/amd/amdmct/wrappers/mcti.h>

struct link_pair_t {
	pci_devfn_t udev;
	u32 upos;
	u32 uoffs;
	pci_devfn_t dev;
	u32 pos;
	u32 offs;
	u8 host;
	u8 nodeid;
	u8 linkn;
	u8 rsv;
} __packed;

struct nodes_info_t {
	u32 nodes_in_group; // could be 2, 3, 4, 5, 6, 7, 8
	u32 groups_in_plane; // could be 1, 2, 3, 4, 5
	u32 planes; // could be 1, 2
	u32 up_planes; // down planes will be [up_planes, planes)
} __packed;

struct ht_link_config {
	uint32_t ht_speed_limit; // Speed in MHz; 0 for autodetect (default)
};

/* be careful with the alignment of sysinfo, bacause sysinfo may be shared by coreboot_car and ramstage stage. and ramstage may be running at 64bit later.*/

struct sys_info {
	int32_t needs_reset;

	u8 ln[NODE_NUMS*NODE_NUMS];// [0, 3] link n, [4, 7] will be hop num
	u16 ln_tn[NODE_NUMS*8]; // for 0x0zzz: bit [0,7] target node num, bit[8,11] respone link from target num; 0x80ff mean not inited, 0x4yyy mean non coherent and yyy is link pair index
	struct nodes_info_t nodes_info;
	u32 nodes;

	u8 host_link_freq[NODE_NUMS*8]; // record freq for every link from cpu, 0x0f means don't need to touch it
	u16 host_link_freq_cap[NODE_NUMS*8]; //cap

	struct ht_link_config ht_link_cfg;

	u32 segbit;
	u32 sbdn;
	u32 sblk;
	u32 sbbusn;

	u32 ht_c_num;
	u32 ht_c_conf_bus[HC_NUMS]; // 4-->32

	struct link_pair_t link_pair[HC_NUMS*4];// enough? only in_conherent, 32 chain and every chain have 4 HT device
	u32 link_pair_num;

	struct mem_controller ctrl[NODE_NUMS];

	struct MCTStatStruc MCTstat;
	struct DCTStatStruc DCTstatA[NODE_NUMS];
} __packed;


/*
#ifdef __PRE_RAM__
extern struct sys_info sysinfo_car;
#endif
*/
#ifndef __PRE_RAM__
struct device *get_node_pci(u32 nodeid, u32 fn);
#endif

#ifdef __PRE_RAM__
void showallroutes(int level, pci_devfn_t dev);

void setup_resource_map_offset(const u32 *register_values, u32 max, u32
		offset_pci_dev, u32 offset_io_base);

void setup_resource_map_x_offset(const u32 *register_values, u32 max, u32
		offset_pci_dev, u32 offset_io_base);

void setup_resource_map_x(const u32 *register_values, u32 max);
void setup_resource_map(const u32 *register_values, u32 max);

/* reset_test.c */
u32 cpu_init_detected(u8 nodeid);
u32 bios_reset_detected(void);
u32 cold_reset_detected(void);
u32 other_reset_detected(void);
u32 warm_reset_detect(u8 nodeid);
void distinguish_cpu_resets(u8 nodeid);
u32 get_sblk(void);
u8 get_sbbusn(u8 sblk);
void set_bios_reset(void);

#endif

#include "northbridge/amd/amdht/porting.h"
BOOL AMD_CB_ManualBUIDSwapList(u8 Node, u8 Link, const u8 **List);

struct acpi_rsdp;

#ifndef __SIMPLE_DEVICE__
unsigned long northbridge_write_acpi_tables(struct device *device,
					    unsigned long start,
					    struct acpi_rsdp *rsdp);
void northbridge_acpi_write_vars(struct device *device);
#endif

#endif /* AMDFAM10_H */
