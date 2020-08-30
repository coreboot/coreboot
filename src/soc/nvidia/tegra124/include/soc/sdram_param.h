/* SPDX-License-Identifier: GPL-2.0-only */

/**
 * Defines the SDRAM parameter structure.
 *
 * Note that PLLM is used by EMC. The field names are in camel case to ease
 * directly converting BCT config files (*.cfg) into C structure.
 */

#ifndef __SOC_NVIDIA_TEGRA124_SDRAM_PARAM_H__
#define __SOC_NVIDIA_TEGRA124_SDRAM_PARAM_H__

#include <stddef.h>
#include <stdint.h>

enum {
	/* Specifies the memory type to be undefined */
	NvBootMemoryType_None = 0,

	/* Specifies the memory type to be DDR SDRAM */
	NvBootMemoryType_Ddr = 0,

	/* Specifies the memory type to be LPDDR SDRAM */
	NvBootMemoryType_LpDdr = 0,

	/* Specifies the memory type to be DDR2 SDRAM */
	NvBootMemoryType_Ddr2 = 0,

	/* Specifies the memory type to be LPDDR2 SDRAM */
	NvBootMemoryType_LpDdr2,

	/* Specifies the memory type to be DDR3 SDRAM */
	NvBootMemoryType_Ddr3,

	NvBootMemoryType_Num,

	/* Specifies an entry in the ram_code table that's not in use */
	NvBootMemoryType_Unused = 0x7FFFFFF,
};

enum {
	BOOT_ROM_PATCH_CONTROL_ENABLE_MASK = 0x1 << 31,
	BOOT_ROM_PATCH_CONTROL_OFFSET_SHIFT = 0,
	BOOT_ROM_PATCH_CONTROL_OFFSET_MASK = 0x7FFFFFFF << 0,
	BOOT_ROM_PATCH_CONTROL_BASE_ADDRESS = 0x70000000,

	EMC_ZCAL_WARM_COLD_BOOT_ENABLES_COLDBOOT_MASK = 1 << 0,
};

/**
 * Defines the SDRAM parameter structure
 */
struct sdram_params {

	/* Specifies the type of memory device */
	uint32_t MemoryType;

	/* MC/EMC clock source configuration */

	/* Specifies the M value for PllM */
	uint32_t PllMInputDivider;
	/* Specifies the N value for PllM */
	uint32_t PllMFeedbackDivider;
	/* Specifies the time to wait for PLLM to lock (in microseconds) */
	uint32_t PllMStableTime;
	/* Specifies misc. control bits */
	uint32_t PllMSetupControl;
	/* Enables the Div by 2 */
	uint32_t PllMSelectDiv2;
	/* Powers down VCO output Level shifter */
	uint32_t PllMPDLshiftPh45;
	/* Powers down VCO output Level shifter */
	uint32_t PllMPDLshiftPh90;
	/* Powers down VCO output Level shifter */
	uint32_t PllMPDLshiftPh135;
	/* Specifies value for Charge Pump Gain Control */
	uint32_t PllMKCP;
	/* Specifies VCO gain */
	uint32_t PllMKVCO;
	/* Spare BCT param */
	uint32_t EmcBctSpare0;
	/* Spare BCT param */
	uint32_t EmcBctSpare1;
	/* Spare BCT param */
	uint32_t EmcBctSpare2;
	/* Spare BCT param */
	uint32_t EmcBctSpare3;
	/* Spare BCT param */
	uint32_t EmcBctSpare4;
	/* Spare BCT param */
	uint32_t EmcBctSpare5;
	/* Spare BCT param */
	uint32_t EmcBctSpare6;
	/* Spare BCT param */
	uint32_t EmcBctSpare7;
	/* Spare BCT param */
	uint32_t EmcBctSpare8;
	/* Spare BCT param */
	uint32_t EmcBctSpare9;
	/* Spare BCT param */
	uint32_t EmcBctSpare10;
	/* Spare BCT param */
	uint32_t EmcBctSpare11;
	/* Defines EMC_2X_CLK_SRC, EMC_2X_CLK_DIVISOR, EMC_INVERT_DCD */
	uint32_t EmcClockSource;

	/* Auto-calibration of EMC pads */

	/* Specifies the value for EMC_AUTO_CAL_INTERVAL */
	uint32_t EmcAutoCalInterval;
	/*
	 * Specifies the value for EMC_AUTO_CAL_CONFIG
	 * Note: Trigger bits are set by the SDRAM code.
	 */
	uint32_t EmcAutoCalConfig;

	/* Specifies the value for EMC_AUTO_CAL_CONFIG2 */
	uint32_t EmcAutoCalConfig2;

	/* Specifies the value for EMC_AUTO_CAL_CONFIG3 */
	uint32_t EmcAutoCalConfig3;

	/*
	 * Specifies the time for the calibration
	 * to stabilize (in microseconds)
	 */
	uint32_t EmcAutoCalWait;

	/*
	 * DRAM size information
	 * Specifies the value for EMC_ADR_CFG
	 */
	uint32_t EmcAdrCfg;

	/*
	 * Specifies the time to wait after asserting pin
	 * CKE (in microseconds)
	 */
	uint32_t EmcPinProgramWait;
	/* Specifies the extra delay before/after pin RESET/CKE command */
	uint32_t EmcPinExtraWait;
	/*
	 * Specifies the extra delay after the first writing
	 * of EMC_TIMING_CONTROL
	 */
	uint32_t EmcTimingControlWait;

	/* Timing parameters required for the SDRAM */

	/* Specifies the value for EMC_RC */
	uint32_t EmcRc;
	/* Specifies the value for EMC_RFC */
	uint32_t EmcRfc;
	/* Specifies the value for EMC_RFC_SLR */
	uint32_t EmcRfcSlr;
	/* Specifies the value for EMC_RAS */
	uint32_t EmcRas;
	/* Specifies the value for EMC_RP */
	uint32_t EmcRp;
	/* Specifies the value for EMC_R2R */
	uint32_t EmcR2r;
	/* Specifies the value for EMC_W2W */
	uint32_t EmcW2w;
	/* Specifies the value for EMC_R2W */
	uint32_t EmcR2w;
	/* Specifies the value for EMC_W2R */
	uint32_t EmcW2r;
	/* Specifies the value for EMC_R2P */
	uint32_t EmcR2p;
	/* Specifies the value for EMC_W2P */
	uint32_t EmcW2p;
	/* Specifies the value for EMC_RD_RCD */
	uint32_t EmcRdRcd;
	/* Specifies the value for EMC_WR_RCD */
	uint32_t EmcWrRcd;
	/* Specifies the value for EMC_RRD */
	uint32_t EmcRrd;
	/* Specifies the value for EMC_REXT */
	uint32_t EmcRext;
	/* Specifies the value for EMC_WEXT */
	uint32_t EmcWext;
	/* Specifies the value for EMC_WDV */
	uint32_t EmcWdv;
	/* Specifies the value for EMC_WDV_MASK */
	uint32_t EmcWdvMask;
	/* Specifies the value for EMC_QUSE */
	uint32_t EmcQUse;
	/* Specifies the value for EMC_QUSE_WIDTH */
	uint32_t EmcQuseWidth;
	/* Specifies the value for EMC_IBDLY */
	uint32_t EmcIbdly;
	/* Specifies the value for EMC_EINPUT */
	uint32_t EmcEInput;
	/* Specifies the value for EMC_EINPUT_DURATION */
	uint32_t EmcEInputDuration;
	/* Specifies the value for EMC_PUTERM_EXTRA */
	uint32_t EmcPutermExtra;
	/* Specifies the value for EMC_PUTERM_WIDTH */
	uint32_t EmcPutermWidth;
	/* Specifies the value for EMC_PUTERM_ADJ */
	uint32_t EmcPutermAdj;
	/* Specifies the value for EMC_CDB_CNTL_1 */
	uint32_t EmcCdbCntl1;
	/* Specifies the value for EMC_CDB_CNTL_2 */
	uint32_t EmcCdbCntl2;
	/* Specifies the value for EMC_CDB_CNTL_3 */
	uint32_t EmcCdbCntl3;
	/* Specifies the value for EMC_QRST */
	uint32_t EmcQRst;
	/* Specifies the value for EMC_QSAFE */
	uint32_t EmcQSafe;
	/* Specifies the value for EMC_RDV */
	uint32_t EmcRdv;
	/* Specifies the value for EMC_RDV_MASK */
	uint32_t EmcRdvMask;
	/* Specifies the value for EMC_QPOP */
	uint32_t EmcQpop;
	/* Specifies the value for EMC_CTT */
	uint32_t EmcCtt;
	/* Specifies the value for EMC_CTT_DURATION */
	uint32_t EmcCttDuration;
	/* Specifies the value for EMC_REFRESH */
	uint32_t EmcRefresh;
	/* Specifies the value for EMC_BURST_REFRESH_NUM */
	uint32_t EmcBurstRefreshNum;
	/* Specifies the value for EMC_PRE_REFRESH_REQ_CNT */
	uint32_t EmcPreRefreshReqCnt;
	/* Specifies the value for EMC_PDEX2WR */
	uint32_t EmcPdEx2Wr;
	/* Specifies the value for EMC_PDEX2RD */
	uint32_t EmcPdEx2Rd;
	/* Specifies the value for EMC_PCHG2PDEN */
	uint32_t EmcPChg2Pden;
	/* Specifies the value for EMC_ACT2PDEN */
	uint32_t EmcAct2Pden;
	/* Specifies the value for EMC_AR2PDEN */
	uint32_t EmcAr2Pden;
	/* Specifies the value for EMC_RW2PDEN */
	uint32_t EmcRw2Pden;
	/* Specifies the value for EMC_TXSR */
	uint32_t EmcTxsr;
	/* Specifies the value for EMC_TXSRDLL */
	uint32_t EmcTxsrDll;
	/* Specifies the value for EMC_TCKE */
	uint32_t EmcTcke;
	/* Specifies the value for EMC_TCKESR */
	uint32_t EmcTckesr;
	/* Specifies the value for EMC_TPD */
	uint32_t EmcTpd;
	/* Specifies the value for EMC_TFAW */
	uint32_t EmcTfaw;
	/* Specifies the value for EMC_TRPAB */
	uint32_t EmcTrpab;
	/* Specifies the value for EMC_TCLKSTABLE */
	uint32_t EmcTClkStable;
	/* Specifies the value for EMC_TCLKSTOP */
	uint32_t EmcTClkStop;
	/* Specifies the value for EMC_TREFBW */
	uint32_t EmcTRefBw;

	/* FBIO configuration values */

	/* Specifies the value for EMC_FBIO_CFG5 */
	uint32_t EmcFbioCfg5;
	/* Specifies the value for EMC_FBIO_CFG6 */
	uint32_t EmcFbioCfg6;
	/* Specifies the value for EMC_FBIO_SPARE */
	uint32_t EmcFbioSpare;

	/* Specifies the value for EMC_CFG_RSV */
	uint32_t EmcCfgRsv;

	/* MRS command values */

	/* Specifies the value for EMC_MRS */
	uint32_t EmcMrs;
	/* Specifies the MP0 command to initialize mode registers */
	uint32_t EmcEmrs;
	/* Specifies the MP2 command to initialize mode registers */
	uint32_t EmcEmrs2;
	/* Specifies the MP3 command to initialize mode registers */
	uint32_t EmcEmrs3;
	/* Specifies the programming to LPDDR2 Mode Register 1 at cold boot */
	uint32_t EmcMrw1;
	/* Specifies the programming to LPDDR2 Mode Register 2 at cold boot */
	uint32_t EmcMrw2;
	/* Specifies the programming to LPDDR2 Mode Register 3 at cold boot */
	uint32_t EmcMrw3;
	/* Specifies the programming to LPDDR2 Mode Register 11 at cold boot */
	uint32_t EmcMrw4;
	/*
	 * Specifies the programming to extra LPDDR2 Mode Register
	 * at cold boot
	 */
	uint32_t EmcMrwExtra;
	/*
	 * Specifies the programming to extra LPDDR2 Mode Register
	 * at warm boot
	 */
	uint32_t EmcWarmBootMrwExtra;
	/*
	 * Specify the enable of extra Mode Register programming at
	 * warm boot
	 */
	uint32_t EmcWarmBootExtraModeRegWriteEnable;
	/*
	 * Specify the enable of extra Mode Register programming at
	 * cold boot
	 */
	uint32_t EmcExtraModeRegWriteEnable;

	/* Specifies the EMC_MRW reset command value */
	uint32_t EmcMrwResetCommand;
	/* Specifies the EMC Reset wait time (in microseconds) */
	uint32_t EmcMrwResetNInitWait;
	/* Specifies the value for EMC_MRS_WAIT_CNT */
	uint32_t EmcMrsWaitCnt;
	/* Specifies the value for EMC_MRS_WAIT_CNT2 */
	uint32_t EmcMrsWaitCnt2;

	/* EMC miscellaneous configurations */

	/* Specifies the value for EMC_CFG */
	uint32_t EmcCfg;
	/* Specifies the value for EMC_CFG_2 */
	uint32_t EmcCfg2;
	/* Specifies the pipe bypass controls */
	uint32_t EmcCfgPipe;
	/* Specifies the value for EMC_DBG */
	uint32_t EmcDbg;
	/* Specifies the value for EMC_CMDQ */
	uint32_t EmcCmdQ;
	/* Specifies the value for EMC_MC2EMCQ */
	uint32_t EmcMc2EmcQ;
	/* Specifies the value for EMC_DYN_SELF_REF_CONTROL */
	uint32_t EmcDynSelfRefControl;

	/* Specifies the value for MEM_INIT_DONE */
	uint32_t AhbArbitrationXbarCtrlMemInitDone;

	/* Specifies the value for EMC_CFG_DIG_DLL */
	uint32_t EmcCfgDigDll;
	/* Specifies the value for EMC_CFG_DIG_DLL_PERIOD */
	uint32_t EmcCfgDigDllPeriod;
	/* Specifies the value of *DEV_SELECTN of various EMC registers */
	uint32_t EmcDevSelect;

	/* Specifies the value for EMC_SEL_DPD_CTRL */
	uint32_t EmcSelDpdCtrl;

	/* Pads trimmer delays */

	/* Specifies the value for EMC_DLL_XFORM_DQS0 */
	uint32_t EmcDllXformDqs0;
	/* Specifies the value for EMC_DLL_XFORM_DQS1 */
	uint32_t EmcDllXformDqs1;
	/* Specifies the value for EMC_DLL_XFORM_DQS2 */
	uint32_t EmcDllXformDqs2;
	/* Specifies the value for EMC_DLL_XFORM_DQS3 */
	uint32_t EmcDllXformDqs3;
	/* Specifies the value for EMC_DLL_XFORM_DQS4 */
	uint32_t EmcDllXformDqs4;
	/* Specifies the value for EMC_DLL_XFORM_DQS5 */
	uint32_t EmcDllXformDqs5;
	/* Specifies the value for EMC_DLL_XFORM_DQS6 */
	uint32_t EmcDllXformDqs6;
	/* Specifies the value for EMC_DLL_XFORM_DQS7 */
	uint32_t EmcDllXformDqs7;
	/* Specifies the value for EMC_DLL_XFORM_DQS8 */
	uint32_t EmcDllXformDqs8;
	/* Specifies the value for EMC_DLL_XFORM_DQS9 */
	uint32_t EmcDllXformDqs9;
	/* Specifies the value for EMC_DLL_XFORM_DQS10 */
	uint32_t EmcDllXformDqs10;
	/* Specifies the value for EMC_DLL_XFORM_DQS11 */
	uint32_t EmcDllXformDqs11;
	/* Specifies the value for EMC_DLL_XFORM_DQS12 */
	uint32_t EmcDllXformDqs12;
	/* Specifies the value for EMC_DLL_XFORM_DQS13 */
	uint32_t EmcDllXformDqs13;
	/* Specifies the value for EMC_DLL_XFORM_DQS14 */
	uint32_t EmcDllXformDqs14;
	/* Specifies the value for EMC_DLL_XFORM_DQS15 */
	uint32_t EmcDllXformDqs15;
	/* Specifies the value for EMC_DLL_XFORM_QUSE0 */
	uint32_t EmcDllXformQUse0;
	/* Specifies the value for EMC_DLL_XFORM_QUSE1 */
	uint32_t EmcDllXformQUse1;
	/* Specifies the value for EMC_DLL_XFORM_QUSE2 */
	uint32_t EmcDllXformQUse2;
	/* Specifies the value for EMC_DLL_XFORM_QUSE3 */
	uint32_t EmcDllXformQUse3;
	/* Specifies the value for EMC_DLL_XFORM_QUSE4 */
	uint32_t EmcDllXformQUse4;
	/* Specifies the value for EMC_DLL_XFORM_QUSE5 */
	uint32_t EmcDllXformQUse5;
	/* Specifies the value for EMC_DLL_XFORM_QUSE6 */
	uint32_t EmcDllXformQUse6;
	/* Specifies the value for EMC_DLL_XFORM_QUSE7 */
	uint32_t EmcDllXformQUse7;
	/* Specifies the value for EMC_DLL_XFORM_ADDR0 */
	uint32_t EmcDllXformAddr0;
	/* Specifies the value for EMC_DLL_XFORM_ADDR1 */
	uint32_t EmcDllXformAddr1;
	/* Specifies the value for EMC_DLL_XFORM_ADDR2 */
	uint32_t EmcDllXformAddr2;
	/* Specifies the value for EMC_DLL_XFORM_ADDR3 */
	uint32_t EmcDllXformAddr3;
	/* Specifies the value for EMC_DLL_XFORM_ADDR4 */
	uint32_t EmcDllXformAddr4;
	/* Specifies the value for EMC_DLL_XFORM_ADDR5 */
	uint32_t EmcDllXformAddr5;
	/* Specifies the value for EMC_DLL_XFORM_QUSE8 */
	uint32_t EmcDllXformQUse8;
	/* Specifies the value for EMC_DLL_XFORM_QUSE9 */
	uint32_t EmcDllXformQUse9;
	/* Specifies the value for EMC_DLL_XFORM_QUSE10 */
	uint32_t EmcDllXformQUse10;
	/* Specifies the value for EMC_DLL_XFORM_QUSE11 */
	uint32_t EmcDllXformQUse11;
	/* Specifies the value for EMC_DLL_XFORM_QUSE12 */
	uint32_t EmcDllXformQUse12;
	/* Specifies the value for EMC_DLL_XFORM_QUSE13 */
	uint32_t EmcDllXformQUse13;
	/* Specifies the value for EMC_DLL_XFORM_QUSE14 */
	uint32_t EmcDllXformQUse14;
	/* Specifies the value for EMC_DLL_XFORM_QUSE15 */
	uint32_t EmcDllXformQUse15;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS0 */
	uint32_t EmcDliTrimTxDqs0;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS1 */
	uint32_t EmcDliTrimTxDqs1;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS2 */
	uint32_t EmcDliTrimTxDqs2;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS3 */
	uint32_t EmcDliTrimTxDqs3;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS4 */
	uint32_t EmcDliTrimTxDqs4;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS5 */
	uint32_t EmcDliTrimTxDqs5;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS6 */
	uint32_t EmcDliTrimTxDqs6;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS7 */
	uint32_t EmcDliTrimTxDqs7;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS8 */
	uint32_t EmcDliTrimTxDqs8;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS9 */
	uint32_t EmcDliTrimTxDqs9;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS10 */
	uint32_t EmcDliTrimTxDqs10;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS11 */
	uint32_t EmcDliTrimTxDqs11;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS12 */
	uint32_t EmcDliTrimTxDqs12;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS13 */
	uint32_t EmcDliTrimTxDqs13;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS14 */
	uint32_t EmcDliTrimTxDqs14;
	/* Specifies the value for EMC_DLI_TRIM_TXDQS15 */
	uint32_t EmcDliTrimTxDqs15;
	/* Specifies the value for EMC_DLL_XFORM_DQ0 */
	uint32_t EmcDllXformDq0;
	/* Specifies the value for EMC_DLL_XFORM_DQ1 */
	uint32_t EmcDllXformDq1;
	/* Specifies the value for EMC_DLL_XFORM_DQ2 */
	uint32_t EmcDllXformDq2;
	/* Specifies the value for EMC_DLL_XFORM_DQ3 */
	uint32_t EmcDllXformDq3;
	/* Specifies the value for EMC_DLL_XFORM_DQ4 */
	uint32_t EmcDllXformDq4;
	/* Specifies the value for EMC_DLL_XFORM_DQ5 */
	uint32_t EmcDllXformDq5;
	/* Specifies the value for EMC_DLL_XFORM_DQ6 */
	uint32_t EmcDllXformDq6;
	/* Specifies the value for EMC_DLL_XFORM_DQ7 */
	uint32_t EmcDllXformDq7;

	/*
	 * Specifies the delay after asserting CKE pin during a WarmBoot0
	 * sequence (in microseconds)
	 */
	uint32_t WarmBootWait;

	/* Specifies the value for EMC_CTT_TERM_CTRL */
	uint32_t EmcCttTermCtrl;

	/* Specifies the value for EMC_ODT_WRITE */
	uint32_t EmcOdtWrite;
	/* Specifies the value for EMC_ODT_WRITE */
	uint32_t EmcOdtRead;

	/* Periodic ZQ calibration */

	/*
	 * Specifies the value for EMC_ZCAL_INTERVAL
	 * Value 0 disables ZQ calibration
	 */
	uint32_t EmcZcalInterval;
	/* Specifies the value for EMC_ZCAL_WAIT_CNT */
	uint32_t EmcZcalWaitCnt;
	/* Specifies the value for EMC_ZCAL_MRW_CMD */
	uint32_t EmcZcalMrwCmd;

	/* DRAM initialization sequence flow control */

	/* Specifies the MRS command value for resetting DLL */
	uint32_t EmcMrsResetDll;
	/* Specifies the command for ZQ initialization of device 0 */
	uint32_t EmcZcalInitDev0;
	/* Specifies the command for ZQ initialization of device 1 */
	uint32_t EmcZcalInitDev1;
	/*
	 * Specifies the wait time after programming a ZQ initialization
	 * command (in microseconds)
	 */
	uint32_t EmcZcalInitWait;
	/*
	 * Specifies the enable for ZQ calibration at cold boot [bit 0]
	 * and warm boot [bit 1]
	 */
	uint32_t EmcZcalWarmColdBootEnables;

	/*
	 * Specifies the MRW command to LPDDR2 for ZQ calibration
	 * on warmboot
	 */
	/* Is issued to both devices separately */
	uint32_t EmcMrwLpddr2ZcalWarmBoot;
	/*
	 * Specifies the ZQ command to DDR3 for ZQ calibration on warmboot
	 * Is issued to both devices separately
	 */
	uint32_t EmcZqCalDdr3WarmBoot;
	/*
	 * Specifies the wait time for ZQ calibration on warmboot
	 * (in microseconds)
	 */
	uint32_t EmcZcalWarmBootWait;
	/*
	 * Specifies the enable for DRAM Mode Register programming
	 * at warm boot
	 */
	uint32_t EmcMrsWarmBootEnable;
	/*
	 * Specifies the wait time after sending an MRS DLL reset command
	 * in microseconds)
	 */
	uint32_t EmcMrsResetDllWait;
	/* Specifies the extra MRS command to initialize mode registers */
	uint32_t EmcMrsExtra;
	/* Specifies the extra MRS command at warm boot */
	uint32_t EmcWarmBootMrsExtra;
	/* Specifies the EMRS command to enable the DDR2 DLL */
	uint32_t EmcEmrsDdr2DllEnable;
	/* Specifies the MRS command to reset the DDR2 DLL */
	uint32_t EmcMrsDdr2DllReset;
	/* Specifies the EMRS command to set OCD calibration */
	uint32_t EmcEmrsDdr2OcdCalib;
	/*
	 * Specifies the wait between initializing DDR and setting OCD
	 * calibration (in microseconds)
	 */
	uint32_t EmcDdr2Wait;
	/* Specifies the value for EMC_CLKEN_OVERRIDE */
	uint32_t EmcClkenOverride;
	/* Specifies the value for MC_DIS_EXTRA_SNAP_LEVELS */
	uint32_t McDisExtraSnapLevels;
	/*
	 * Specifies LOG2 of the extra refresh numbers after booting
	 * Program 0 to disable
	 */
	uint32_t EmcExtraRefreshNum;
	/* Specifies the master override for all EMC clocks */
	uint32_t EmcClkenOverrideAllWarmBoot;
	/* Specifies the master override for all MC clocks */
	uint32_t McClkenOverrideAllWarmBoot;
	/* Specifies digital dll period, choosing between 4 to 64 ms */
	uint32_t EmcCfgDigDllPeriodWarmBoot;

	/* Pad controls */

	/* Specifies the value for PMC_VDDP_SEL */
	uint32_t PmcVddpSel;
	/* Specifies the wait time after programming PMC_VDDP_SEL */
	uint32_t PmcVddpSelWait;
	/* Specifies the value for PMC_DDR_PWR */
	uint32_t PmcDdrPwr;
	/* Specifies the value for PMC_DDR_CFG */
	uint32_t PmcDdrCfg;
	/* Specifies the value for PMC_IO_DPD3_REQ */
	uint32_t PmcIoDpd3Req;
	/* Specifies the wait time after programming PMC_IO_DPD3_REQ */
	uint32_t PmcIoDpd3ReqWait;
	/* Specifies the value for PMC_REG_SHORT */
	uint32_t PmcRegShort;
	/* Specifies the value for PMC_NO_IOPOWER */
	uint32_t PmcNoIoPower;
	/* Specifies the wait time after programming PMC_POR_DPD_CTRL */
	uint32_t PmcPorDpdCtrlWait;
	/* Specifies the value for EMC_XM2CMDPADCTRL */
	uint32_t EmcXm2CmdPadCtrl;
	/* Specifies the value for EMC_XM2CMDPADCTRL2 */
	uint32_t EmcXm2CmdPadCtrl2;
	/* Specifies the value for EMC_XM2CMDPADCTRL3 */
	uint32_t EmcXm2CmdPadCtrl3;
	/* Specifies the value for EMC_XM2CMDPADCTRL4 */
	uint32_t EmcXm2CmdPadCtrl4;
	/* Specifies the value for EMC_XM2CMDPADCTRL5 */
	uint32_t EmcXm2CmdPadCtrl5;
	/* Specifies the value for EMC_XM2DQSPADCTRL */
	uint32_t EmcXm2DqsPadCtrl;
	/* Specifies the value for EMC_XM2DQSPADCTRL2 */
	uint32_t EmcXm2DqsPadCtrl2;
	/* Specifies the value for EMC_XM2DQSPADCTRL3 */
	uint32_t EmcXm2DqsPadCtrl3;
	/* Specifies the value for EMC_XM2DQSPADCTRL4 */
	uint32_t EmcXm2DqsPadCtrl4;
	/* Specifies the value for EMC_XM2DQSPADCTRL5 */
	uint32_t EmcXm2DqsPadCtrl5;
	/* Specifies the value for EMC_XM2DQSPADCTRL6 */
	uint32_t EmcXm2DqsPadCtrl6;
	/* Specifies the value for EMC_XM2DQPADCTRL */
	uint32_t EmcXm2DqPadCtrl;
	/* Specifies the value for EMC_XM2DQPADCTRL2 */
	uint32_t EmcXm2DqPadCtrl2;
	/* Specifies the value for EMC_XM2DQPADCTRL3 */
	uint32_t EmcXm2DqPadCtrl3;
	/* Specifies the value for EMC_XM2CLKPADCTRL */
	uint32_t EmcXm2ClkPadCtrl;
	/* Specifies the value for EMC_XM2CLKPADCTRL2 */
	uint32_t EmcXm2ClkPadCtrl2;
	/* Specifies the value for EMC_XM2COMPPADCTRL */
	uint32_t EmcXm2CompPadCtrl;
	/* Specifies the value for EMC_XM2VTTGENPADCTRL */
	uint32_t EmcXm2VttGenPadCtrl;
	/* Specifies the value for EMC_XM2VTTGENPADCTRL2 */
	uint32_t EmcXm2VttGenPadCtrl2;
	/* Specifies the value for EMC_XM2VTTGENPADCTRL3 */
	uint32_t EmcXm2VttGenPadCtrl3;
	/* Specifies the value for EMC_ACPD_CONTROL */
	uint32_t EmcAcpdControl;

	/* Specifies the value for EMC_SWIZZLE_RANK0_BYTE_CFG */
	uint32_t EmcSwizzleRank0ByteCfg;
	/* Specifies the value for EMC_SWIZZLE_RANK0_BYTE0 */
	uint32_t EmcSwizzleRank0Byte0;
	/* Specifies the value for EMC_SWIZZLE_RANK0_BYTE1 */
	uint32_t EmcSwizzleRank0Byte1;
	/* Specifies the value for EMC_SWIZZLE_RANK0_BYTE2 */
	uint32_t EmcSwizzleRank0Byte2;
	/* Specifies the value for EMC_SWIZZLE_RANK0_BYTE3 */
	uint32_t EmcSwizzleRank0Byte3;
	/* Specifies the value for EMC_SWIZZLE_RANK1_BYTE_CFG */
	uint32_t EmcSwizzleRank1ByteCfg;
	/* Specifies the value for EMC_SWIZZLE_RANK1_BYTE0 */
	uint32_t EmcSwizzleRank1Byte0;
	/* Specifies the value for EMC_SWIZZLE_RANK1_BYTE1 */
	uint32_t EmcSwizzleRank1Byte1;
	/* Specifies the value for EMC_SWIZZLE_RANK1_BYTE2 */
	uint32_t EmcSwizzleRank1Byte2;
	/* Specifies the value for EMC_SWIZZLE_RANK1_BYTE3 */
	uint32_t EmcSwizzleRank1Byte3;

	/* Specifies the value for EMC_DSR_VTTGEN_DRV */
	uint32_t EmcDsrVttgenDrv;

	/* Specifies the value for EMC_TXDSRVTTGEN */
	uint32_t EmcTxdsrvttgen;
	/* Specifies the value for EMC_BGBIAS_CTL */
	uint32_t EmcBgbiasCtl0;

	/* DRAM size information */

	/* Specifies the value for MC_EMEM_ADR_CFG */
	uint32_t McEmemAdrCfg;
	/* Specifies the value for MC_EMEM_ADR_CFG_DEV0 */
	uint32_t McEmemAdrCfgDev0;
	/* Specifies the value for MC_EMEM_ADR_CFG_DEV1 */
	uint32_t McEmemAdrCfgDev1;
	/* Specifies the value for MC_EMEM_BANK_SWIZZLE_CFG0 */
	uint32_t McEmemAdrCfgBankMask0;
	/* Specifies the value for MC_EMEM_BANK_SWIZZLE_CFG1 */
	uint32_t McEmemAdrCfgBankMask1;
	/* Specifies the value for MC_EMEM_BANK_SWIZZLE_CFG2 */
	uint32_t McEmemAdrCfgBankMask2;
	/* Specifies the value for MC_EMEM_BANK_SWIZZLE_CFG3 */
	uint32_t McEmemAdrCfgBankSwizzle3;

	/*
	 * Specifies the value for MC_EMEM_CFG which holds the external memory
	 * size (in KBytes)
	 */
	uint32_t McEmemCfg;

	/* MC arbitration configuration */

	/* Specifies the value for MC_EMEM_ARB_CFG */
	uint32_t McEmemArbCfg;
	/* Specifies the value for MC_EMEM_ARB_OUTSTANDING_REQ */
	uint32_t McEmemArbOutstandingReq;
	/* Specifies the value for MC_EMEM_ARB_TIMING_RCD */
	uint32_t McEmemArbTimingRcd;
	/* Specifies the value for MC_EMEM_ARB_TIMING_RP */
	uint32_t McEmemArbTimingRp;
	/* Specifies the value for MC_EMEM_ARB_TIMING_RC */
	uint32_t McEmemArbTimingRc;
	/* Specifies the value for MC_EMEM_ARB_TIMING_RAS */
	uint32_t McEmemArbTimingRas;
	/* Specifies the value for MC_EMEM_ARB_TIMING_FAW */
	uint32_t McEmemArbTimingFaw;
	/* Specifies the value for MC_EMEM_ARB_TIMING_RRD */
	uint32_t McEmemArbTimingRrd;
	/* Specifies the value for MC_EMEM_ARB_TIMING_RAP2PRE */
	uint32_t McEmemArbTimingRap2Pre;
	/* Specifies the value for MC_EMEM_ARB_TIMING_WAP2PRE */
	uint32_t McEmemArbTimingWap2Pre;
	/* Specifies the value for MC_EMEM_ARB_TIMING_R2R */
	uint32_t McEmemArbTimingR2R;
	/* Specifies the value for MC_EMEM_ARB_TIMING_W2W */
	uint32_t McEmemArbTimingW2W;
	/* Specifies the value for MC_EMEM_ARB_TIMING_R2W */
	uint32_t McEmemArbTimingR2W;
	/* Specifies the value for MC_EMEM_ARB_TIMING_W2R */
	uint32_t McEmemArbTimingW2R;
	/* Specifies the value for MC_EMEM_ARB_DA_TURNS */
	uint32_t McEmemArbDaTurns;
	/* Specifies the value for MC_EMEM_ARB_DA_COVERS */
	uint32_t McEmemArbDaCovers;
	/* Specifies the value for MC_EMEM_ARB_MISC0 */
	uint32_t McEmemArbMisc0;
	/* Specifies the value for MC_EMEM_ARB_MISC1 */
	uint32_t McEmemArbMisc1;
	/* Specifies the value for MC_EMEM_ARB_RING1_THROTTLE */
	uint32_t McEmemArbRing1Throttle;
	/* Specifies the value for MC_EMEM_ARB_OVERRIDE */
	uint32_t McEmemArbOverride;
	/* Specifies the value for MC_EMEM_ARB_OVERRIDE_1 */
	uint32_t McEmemArbOverride1;
	/* Specifies the value for MC_EMEM_ARB_RSV */
	uint32_t McEmemArbRsv;

	/* Specifies the value for MC_CLKEN_OVERRIDE */
	uint32_t McClkenOverride;

	/* Specifies the value for MC_STAT_CONTROL */
	uint32_t McStatControl;
	/* Specifies the value for MC_DISPLAY_SNAP_RING */
	uint32_t McDisplaySnapRing;
	/* Specifies the value for MC_VIDEO_PROTECT_BOM */
	uint32_t McVideoProtectBom;
	/* Specifies the value for MC_VIDEO_PROTECT_BOM_ADR_HI */
	uint32_t McVideoProtectBomAdrHi;
	/* Specifies the value for MC_VIDEO_PROTECT_SIZE_MB */
	uint32_t McVideoProtectSizeMb;
	/* Specifies the value for MC_VIDEO_PROTECT_VPR_OVERRIDE */
	uint32_t McVideoProtectVprOverride;
	/* Specifies the value for MC_VIDEO_PROTECT_VPR_OVERRIDE1 */
	uint32_t McVideoProtectVprOverride1;
	/* Specifies the value for MC_VIDEO_PROTECT_GPU_OVERRIDE_0 */
	uint32_t McVideoProtectGpuOverride0;
	/* Specifies the value for MC_VIDEO_PROTECT_GPU_OVERRIDE_1 */
	uint32_t McVideoProtectGpuOverride1;
	/* Specifies the value for MC_SEC_CARVEOUT_BOM */
	uint32_t McSecCarveoutBom;
	/* Specifies the value for MC_SEC_CARVEOUT_ADR_HI */
	uint32_t McSecCarveoutAdrHi;
	/* Specifies the value for MC_SEC_CARVEOUT_SIZE_MB */
	uint32_t McSecCarveoutSizeMb;
	/* Specifies the value for MC_VIDEO_PROTECT_REG_CTRL.VIDEO_PROTECT_WRITE_ACCESS */
	uint32_t McVideoProtectWriteAccess;
	/* Specifies the value for MC_SEC_CARVEOUT_REG_CTRL.SEC_CARVEOUT_WRITE_ACCESS */
	uint32_t McSecCarveoutProtectWriteAccess;

	/* Specifies enable for CA training */
	uint32_t EmcCaTrainingEnable;
	/* Specifies the value for EMC_CA_TRAINING_TIMING_CNTRL1 */
	uint32_t EmcCaTrainingTimingCntl1;
	/* Specifies the value for EMC_CA_TRAINING_TIMING_CNTRL2 */
	uint32_t EmcCaTrainingTimingCntl2;
	/* Set if bit 6 select is greater than bit 7 select; uses aremc.spec packet SWIZZLE_BIT6_GT_BIT7 */
	uint32_t SwizzleRankByteEncode;
	/* Specifies enable and offset for patched boot ROM write */
	uint32_t BootRomPatchControl;
	/* Specifies data for patched boot ROM write */
	uint32_t BootRomPatchData;
	/* Specifies the value for MC_MTS_CARVEOUT_BOM */
	uint32_t McMtsCarveoutBom;
	/* Specifies the value for MC_MTS_CARVEOUT_ADR_HI */
	uint32_t McMtsCarveoutAdrHi;
	/* Specifies the value for MC_MTS_CARVEOUT_SIZE_MB */
	uint32_t McMtsCarveoutSizeMb;
	/* Specifies the value for MC_MTS_CARVEOUT_REG_CTRL */
	uint32_t McMtsCarveoutRegCtrl;

	/* End of generated code by warmboot_code_gen */
};

check_member(sdram_params, McMtsCarveoutRegCtrl, 0x4d0);

#endif /* __SOC_NVIDIA_TEGRA124_SDRAM_PARAM_H__ */
