/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

/* Description: Main memory controller system configuration for DDR 3 */

/* KNOWN ISSUES - ERRATA
 *
 * Trtp is not calculated correctly when the controller is in 64-bit mode, it
 * is 1 busclock off.	No fix planned.	 The controller is not ordinarily in
 * 64-bit mode.
 *
 * 32 Byte burst not supported. No fix planned. The controller is not
 * ordinarily in 64-bit mode.
 *
 * Trc precision does not use extra Jedec defined fractional component.
 * InsteadTrc (course) is rounded up to nearest 1 ns.
 *
 * Mini and Micro DIMM not supported. Only RDIMM, UDIMM, SO-DIMM defined types
 * supported.
 */

static u8 ReconfigureDIMMspare_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void DQSTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void LoadDQSSigTmgRegs_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void HTMemMapInit_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void MCTMemClr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void DCTMemClr_Init_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void DCTMemClr_Sync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void MCTMemClrSync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static u8 NodePresent_D(u8 Node);
static void SyncDCTsReady_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void StartupDCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void ClearDCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static u8 AutoCycTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void GetPresetmaxF_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void SPDGetTCL_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static u8 AutoConfig_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u8 PlatformSpec_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void SPDSetBanks_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void StitchMemory_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u16 Get_Fk_D(u8 k);
static u8 Get_DIMMAddress_D(struct DCTStatStruc *pDCTstat, u8 i);
static void mct_initDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_DramInit(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u8 mct_PlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u8 mct_BeforePlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_SyncDCTsReady(struct DCTStatStruc *pDCTstat);
static void Get_Trdrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_AfterGetCLT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u8 mct_SPDCalcWidth(struct MCTStatStruc *pMCTstat,\
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_AfterStitchMemory(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u8 mct_DIMMPresence(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void Set_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_Twrwr(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_Twrrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_TrwtTO(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct);
static void Get_TrwtWB(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);
static void Get_DqsRcvEnGross_Diff(struct DCTStatStruc *pDCTstat,
					u32 dev, u32 index_reg);
static void Get_WrDatGross_Diff(struct DCTStatStruc *pDCTstat, u8 dct,
					u32 dev, u32 index_reg);
static u16 Get_DqsRcvEnGross_MaxMin(struct DCTStatStruc *pDCTstat,
				u32 dev, u32 index_reg, u32 index);
static void mct_FinalMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static u16 Get_WrDatGross_MaxMin(struct DCTStatStruc *pDCTstat, u8 dct,
				u32 dev, u32 index_reg, u32 index);
static void mct_InitialMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_init(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);
static void clear_legacy_Mode(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_HTMemMapExt(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void SetCSTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void SetCKETriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void SetODTTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void InitPhyCompensation(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u32 mct_NodePresent_D(void);
static void mct_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void mct_ResetDataStruct_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void mct_EarlyArbEn_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_BeforeDramInit_Prod_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
void mct_ClrClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat);
static u8 CheckNBCOFEarlyArbEn(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
void mct_ClrWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
static void mct_BeforeDQSTrain_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void AfterDramInit_D(struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_ResetDLL_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void ProgDramMRSReg_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void mct_DramInit_Sw_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u32 mct_DisDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 DramConfigLo, u8 dct);
static void mct_EnDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);

static u32 mct_MR1Odt_RDimm(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct, u32 MrsChipSel);
static u32 mct_DramTermDyn_RDimm(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dimm);
static u32 mct_SetDramConfigMisc2(struct DCTStatStruc *pDCTstat, u8 dct, u32 misc2);
static void mct_BeforeDQSTrainSamp(struct DCTStatStruc *pDCTstat);
static void mct_WriteLevelization_HW(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA);
static u8 Get_Latency_Diff(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void SyncSetting(struct DCTStatStruc *pDCTstat);
static u8 crcCheck(u8 smbaddr);
static void mct_ExtMCTConfig_Bx(struct DCTStatStruc *pDCTstat);
static void mct_ExtMCTConfig_Cx(struct DCTStatStruc *pDCTstat);

/*See mctAutoInitMCT header for index relationships to CL and T*/
static const u16 Table_F_k[]	= {00,200,266,333,400,533 };
static const u8 Tab_BankAddr[]	= {0x3F,0x01,0x09,0x3F,0x3F,0x11,0x0A,0x19,0x12,0x1A,0x21,0x22,0x23};
static const u8 Table_DQSRcvEn_Offset[] = {0x00,0x01,0x10,0x11,0x2};

/****************************************************************************
   Describe how platform maps MemClk pins to logical DIMMs. The MemClk pins
   are identified based on BKDG definition of Fn2x88[MemClkDis] bitmap.
   AGESA will base on this value to disable unused MemClk to save power.

   If MEMCLK_MAPPING or MEMCLK_MAPPING contains all zeroes, AGESA will use
   default MemClkDis setting based on package type.

   Example:
   BKDG definition of Fn2x88[MemClkDis] bitmap for AM3 package is like below:
        Bit AM3/S1g3 pin name
        0   M[B,A]_CLK_H/L[0]
        1   M[B,A]_CLK_H/L[1]
        2   M[B,A]_CLK_H/L[2]
        3   M[B,A]_CLK_H/L[3]
        4   M[B,A]_CLK_H/L[4]
        5   M[B,A]_CLK_H/L[5]
        6   M[B,A]_CLK_H/L[6]
        7   M[B,A]_CLK_H/L[7]

   And platform has the following routing:
        CS0   M[B,A]_CLK_H/L[4]
        CS1   M[B,A]_CLK_H/L[2]
        CS2   M[B,A]_CLK_H/L[3]
        CS3   M[B,A]_CLK_H/L[5]

   Then:
                        ;    CS0        CS1        CS2        CS3        CS4        CS5        CS6        CS7
   MEMCLK_MAPPING  EQU    00010000b, 00000100b, 00001000b, 00100000b, 00000000b, 00000000b, 00000000b, 00000000b
*/

/* Note: If you are not sure about the pin mappings at initial stage, we dont have to disable MemClk.
 * Set entries in the tables all 0xFF. */
static const u8 Tab_L1CLKDis[]  = {0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04};
static const u8 Tab_AM3CLKDis[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
static const u8 Tab_S1CLKDis[]  = {0xA2, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u8 Tab_ManualCLKDis[]= {0x10, 0x04, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00};

static const u8 Table_Comp_Rise_Slew_20x[] = {7, 3, 2, 2, 0xFF};
static const u8 Table_Comp_Rise_Slew_15x[] = {7, 7, 3, 2, 0xFF};
static const u8 Table_Comp_Fall_Slew_20x[] = {7, 5, 3, 2, 0xFF};
static const u8 Table_Comp_Fall_Slew_15x[] = {7, 7, 5, 3, 0xFF};

static void mctAutoInitMCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	/*
	 * Memory may be mapped contiguously all the way up to 4GB (depending on setup
	 * options).  It is the responsibility of PCI subsystem to create an uncacheable
	 * IO region below 4GB and to adjust TOP_MEM downward prior to any IO mapping or
	 * accesses.  It is the same responsibility of the CPU sub-system prior to
	 * accessing LAPIC.
	 *
	 * Slot Number is an external convention, and is determined by OEM with accompanying
	 * silk screening.  OEM may choose to use Slot number convention which is consistent
	 * with DIMM number conventions.  All AMD engineering platforms do.
	 *
	 * Build Requirements:
	 * 1. MCT_SEG0_START and MCT_SEG0_END macros to begin and end the code segment,
	 *    defined in mcti.inc.
	 *
	 * Run-Time Requirements:
	 * 1. Complete Hypertransport Bus Configuration
	 * 2. SMBus Controller Initialized
	 * 1. BSP in Big Real Mode
	 * 2. Stack at SS:SP, located somewhere between A000:0000 and F000:FFFF
	 * 3. Checksummed or Valid NVRAM bits
	 * 4. MCG_CTL=-1, MC4_CTL_EN=0 for all CPUs
	 * 5. MCi_STS from shutdown/warm reset recorded (if desired) prior to entry
	 * 6. All var MTRRs reset to zero
	 * 7. State of NB_CFG.DisDatMsk set properly on all CPUs
	 * 8. All CPUs at 2Ghz Speed (unless DQS training is not installed).
	 * 9. All cHT links at max Speed/Width (unless DQS training is not installed).
	 *
	 *
	 * Global relationship between index values and item values:
	 *
	 * pDCTstat.CASL pDCTstat.Speed
	 * j CL(j)       k   F(k)
	 * --------------------------
	 * 0 2.0         -   -
	 * 1 3.0         1   200 Mhz
	 * 2 4.0         2   266 Mhz
	 * 3 5.0         3   333 Mhz
	 * 4 6.0         4   400 Mhz
	 * 5 7.0         5   533 Mhz
	 * 6 8.0         6   667 Mhz
	 * 7 9.0         7   800 Mhz
	 */
	u8 Node, NodesWmem;
	u32 node_sys_base;

restartinit:
	mctInitMemGPIOs_A_D();		/* Set any required GPIOs*/
	NodesWmem = 0;
	node_sys_base = 0;
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		pDCTstat->Node_ID = Node;
		pDCTstat->dev_host = PA_HOST(Node);
		pDCTstat->dev_map = PA_MAP(Node);
		pDCTstat->dev_dct = PA_DCT(Node);
		pDCTstat->dev_nbmisc = PA_NBMISC(Node);
		pDCTstat->NodeSysBase = node_sys_base;

		mct_init(pMCTstat, pDCTstat);
		mctNodeIDDebugPort_D();
		pDCTstat->NodePresent = NodePresent_D(Node);
		if (pDCTstat->NodePresent) {		/* See if Node is there*/
			clear_legacy_Mode(pMCTstat, pDCTstat);
			pDCTstat->LogicalCPUID = mctGetLogicalCPUID_D(Node);

			mct_InitialMCT_D(pMCTstat, pDCTstat);

			mctSMBhub_Init(Node);		/* Switch SMBUS crossbar to proper node*/

			mct_initDCT(pMCTstat, pDCTstat);
			if (pDCTstat->ErrCode == SC_FatalErr) {
				goto fatalexit;		/* any fatal errors?*/
			} else if (pDCTstat->ErrCode < SC_StopError) {
				NodesWmem++;
			}
		}	/* if Node present */
		node_sys_base = pDCTstat->NodeSysBase;
		node_sys_base += (pDCTstat->NodeSysLimit + 2) & ~0x0F;
	}
	if (NodesWmem == 0) {
		printk(BIOS_DEBUG, "No Nodes?!\n");
		goto fatalexit;
	}

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: SyncDCTsReady_D\n");
	SyncDCTsReady_D(pMCTstat, pDCTstatA);	/* Make sure DCTs are ready for accesses.*/

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: HTMemMapInit_D\n");
	HTMemMapInit_D(pMCTstat, pDCTstatA);	/* Map local memory into system address space.*/
	mctHookAfterHTMap();

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: CPUMemTyping_D\n");
	CPUMemTyping_D(pMCTstat, pDCTstatA);	/* Map dram into WB/UC CPU cacheability */
	mctHookAfterCPU();			/* Setup external northbridge(s) */

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: DQSTiming_D\n");
	DQSTiming_D(pMCTstat, pDCTstatA);	/* Get Receiver Enable and DQS signal timing*/

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: UMAMemTyping_D\n");
	UMAMemTyping_D(pMCTstat, pDCTstatA);	/* Fix up for UMA sizing */

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: :OtherTiming\n");
	mct_OtherTiming(pMCTstat, pDCTstatA);

	if (ReconfigureDIMMspare_D(pMCTstat, pDCTstatA)) { /* RESET# if 1st pass of DIMM spare enabled*/
		goto restartinit;
	}

	InterleaveNodes_D(pMCTstat, pDCTstatA);
	InterleaveChannels_D(pMCTstat, pDCTstatA);

	printk(BIOS_DEBUG, "mctAutoInitMCT_D: ECCInit_D\n");
	if (ECCInit_D(pMCTstat, pDCTstatA)) {		/* Setup ECC control and ECC check-bits*/
		printk(BIOS_DEBUG, "mctAutoInitMCT_D: MCTMemClr_D\n");
		MCTMemClr_D(pMCTstat,pDCTstatA);
	}

	mct_FinalMCT_D(pMCTstat, pDCTstatA);
	printk(BIOS_DEBUG, "mctAutoInitMCT_D Done: Global Status: %x\n", pMCTstat->GStatus);
	return;

fatalexit:
	die("mct_d: fatalexit");
}

static u8 ReconfigureDIMMspare_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 ret;

	if (mctGet_NVbits(NV_CS_SpareCTL)) {
		if (MCT_DIMM_SPARE_NO_WARM) {
			/* Do no warm-reset DIMM spare */
			if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
				LoadDQSSigTmgRegs_D(pMCTstat, pDCTstatA);
				ret = 0;
			} else {
				mct_ResetDataStruct_D(pMCTstat, pDCTstatA);
				pMCTstat->GStatus |= 1 << GSB_EnDIMMSpareNW;
				ret = 1;
			}
		} else {
			/* Do warm-reset DIMM spare */
			if (mctGet_NVbits(NV_DQSTrainCTL))
				mctWarmReset_D();
			ret = 0;
		}
	} else {
		ret = 0;
	}

	return ret;
}

static void DQSTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 nv_DQSTrainCTL;

	if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
		return;
	}

	nv_DQSTrainCTL = mctGet_NVbits(NV_DQSTrainCTL);
	/* FIXME: BOZO- DQS training every time*/
	nv_DQSTrainCTL = 1;

	mct_BeforeDQSTrain_D(pMCTstat, pDCTstatA);
	phyAssistedMemFnceTraining(pMCTstat, pDCTstatA);

	if (nv_DQSTrainCTL) {
		mctHookBeforeAnyTraining(pMCTstat, pDCTstatA);
		/* TODO: should be in mctHookBeforeAnyTraining */
		_WRMSR(0x26C, 0x04040404, 0x04040404);
		_WRMSR(0x26D, 0x04040404, 0x04040404);
		_WRMSR(0x26E, 0x04040404, 0x04040404);
		_WRMSR(0x26F, 0x04040404, 0x04040404);
		mct_WriteLevelization_HW(pMCTstat, pDCTstatA);

		TrainReceiverEn_D(pMCTstat, pDCTstatA, FirstPass);

		mct_TrainDQSPos_D(pMCTstat, pDCTstatA);

		/* Second Pass never used for Barcelona! */
		/* TrainReceiverEn_D(pMCTstat, pDCTstatA, SecondPass); */

		mctSetEccDQSRcvrEn_D(pMCTstat, pDCTstatA);

		/* FIXME - currently uses calculated value	TrainMaxReadLatency_D(pMCTstat, pDCTstatA); */
		mctHookAfterAnyTraining();
		mctSaveDQSSigTmg_D();

		MCTMemClr_D(pMCTstat, pDCTstatA);
	} else {
		mctGetDQSSigTmg_D();	/* get values into data structure */
		LoadDQSSigTmgRegs_D(pMCTstat, pDCTstatA);	/* load values into registers.*/
		/* mctDoWarmResetMemClr_D(); */
		MCTMemClr_D(pMCTstat, pDCTstatA);
	}
}

static void LoadDQSSigTmgRegs_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 Node, Receiver, Channel, Dir, DIMM;
	u32 dev;
	u32 index_reg;
	u32 reg;
	u32 index;
	u32 val;
	u8 ByteLane;
	u8 txdqs;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->DCTSysLimit) {
			dev = pDCTstat->dev_dct;
			for (Channel = 0;Channel < 2; Channel++) {
				/* there are four receiver pairs,
				   loosely associated with chipselects.*/
				index_reg = 0x98 + Channel * 0x100;
				for (Receiver = 0; Receiver < 8; Receiver += 2) {
					/* Set Receiver Enable Values */
					mct_SetRcvrEnDly_D(pDCTstat,
						0, /* RcvrEnDly */
						1, /* FinalValue, From stack */
						Channel,
						Receiver,
						dev, index_reg,
						(Receiver >> 1) * 3 + 0x10, /* Addl_Index */
						2); /* Pass Second Pass ? */
					/* Restore Write levelization training data */
					for (ByteLane = 0; ByteLane < 9; ByteLane ++) {
						txdqs = pDCTstat->CH_D_B_TxDqs[Channel][Receiver >> 1][ByteLane];
						index = Table_DQSRcvEn_Offset[ByteLane >> 1];
						index += (Receiver >> 1) * 3 + 0x10 + 0x20; /* Addl_Index */
						val = Get_NB32_index_wait(dev, 0x98 + 0x100*Channel, index);
						if (ByteLane & 1) { /* odd byte lane */
							val &= ~(0xFF << 16);
							val |= txdqs << 16;
						} else {
							val &= ~0xFF;
							val |= txdqs;
						}
						Set_NB32_index_wait(dev, 0x98 + 0x100*Channel, index, val);
					}
				}
			}
			for (Channel = 0; Channel<2; Channel++) {
				SetEccDQSRcvrEn_D(pDCTstat, Channel);
			}

			for (Channel = 0; Channel < 2; Channel++) {
				u8 *p;
				index_reg = 0x98 + Channel * 0x100;

				/* NOTE:
				 * when 400, 533, 667, it will support dimm0/1/2/3,
				 * and set conf for dimm0, hw will copy to dimm1/2/3
				 * set for dimm1, hw will copy to dimm3
				 * Rev A/B only support DIMM0/1 when 800Mhz and above
				 *   + 0x100 to next dimm
				 * Rev C support DIMM0/1/2/3 when 800Mhz and above
				 *   + 0x100 to next dimm
				*/
				for (DIMM = 0; DIMM < 4; DIMM++) {
					if (DIMM == 0) {
						index = 0;	/* CHA Write Data Timing Low */
					} else {
						if (pDCTstat->Speed >= 4) {
							index = 0x100 * DIMM;
						} else {
							break;
						}
					}
					for (Dir = 0; Dir < 2; Dir++) {/* RD/WR */
						p = pDCTstat->CH_D_DIR_B_DQS[Channel][DIMM][Dir];
						val = stream_to_int(p); /* CHA Read Data Timing High */
						Set_NB32_index_wait(dev, index_reg, index+1, val);
						val = stream_to_int(p+4); /* CHA Write Data Timing High */
						Set_NB32_index_wait(dev, index_reg, index+2, val);
						val = *(p+8); /* CHA Write ECC Timing */
						Set_NB32_index_wait(dev, index_reg, index+3, val);
						index += 4;
					}
				}
			}

			for (Channel = 0; Channel<2; Channel++) {
				reg = 0x78 + Channel * 0x100;
				val = Get_NB32(dev, reg);
				val &= ~(0x3ff<<22);
				val |= ((u32) pDCTstat->CH_MaxRdLat[Channel] << 22);
				val &= ~(1<<DqsRcvEnTrain);
				Set_NB32(dev, reg, val);	/* program MaxRdLatency to correspond with current delay*/
			}
		}
	}
}

static void HTMemMapInit_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u32 NextBase, BottomIO;
	u8 _MemHoleRemap, DramHoleBase, DramHoleOffset;
	u32 HoleSize, DramSelBaseAddr;

	u32 val;
	u32 base;
	u32 limit;
	u32 dev, devx;
	struct DCTStatStruc *pDCTstat;

	_MemHoleRemap = mctGet_NVbits(NV_MemHole);

	if (pMCTstat->HoleBase == 0) {
		DramHoleBase = mctGet_NVbits(NV_BottomIO);
	} else {
		DramHoleBase = pMCTstat->HoleBase >> (24-8);
	}

	BottomIO = DramHoleBase << (24-8);

	NextBase = 0;
	pDCTstat = pDCTstatA + 0;
	dev = pDCTstat->dev_map;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;
		DramSelBaseAddr = 0;
		pDCTstat = pDCTstatA + Node; /* ??? */
		if (!pDCTstat->GangedMode) {
			DramSelBaseAddr = pDCTstat->NodeSysLimit - pDCTstat->DCTSysLimit;
			/*In unganged mode, we must add DCT0 and DCT1 to DCTSysLimit */
			val = pDCTstat->NodeSysLimit;
			if ((val & 0xFF) == 0xFE) {
				DramSelBaseAddr++;
				val++;
			}
			pDCTstat->DCTSysLimit = val;
		}

		base  = pDCTstat->DCTSysBase;
		limit = pDCTstat->DCTSysLimit;
		if (limit > base) {
			base  += NextBase;
			limit += NextBase;
			DramSelBaseAddr += NextBase;
			printk(BIOS_DEBUG, " Node: %02x  base: %02x  limit: %02x  BottomIO: %02x\n", Node, base, limit, BottomIO);

			if (_MemHoleRemap) {
				if ((base < BottomIO) && (limit >= BottomIO)) {
					/* HW Dram Remap */
					pDCTstat->Status |= 1 << SB_HWHole;
					pMCTstat->GStatus |= 1 << GSB_HWHole;
					pDCTstat->DCTSysBase = base;
					pDCTstat->DCTSysLimit = limit;
					pDCTstat->DCTHoleBase = BottomIO;
					pMCTstat->HoleBase = BottomIO;
					HoleSize = _4GB_RJ8 - BottomIO; /* HoleSize[39:8] */
					if ((DramSelBaseAddr > 0) && (DramSelBaseAddr < BottomIO))
						base = DramSelBaseAddr;
					val = ((base + HoleSize) >> (24-8)) & 0xFF;
					DramHoleOffset = val;
					val <<= 8; /* shl 16, rol 24 */
					val |= DramHoleBase << 24;
					val |= 1  << DramHoleValid;
					Set_NB32(devx, 0xF0, val); /* Dram Hole Address Reg */
					pDCTstat->DCTSysLimit += HoleSize;
					base = pDCTstat->DCTSysBase;
					limit = pDCTstat->DCTSysLimit;
				} else if (base == BottomIO) {
					/* SW Node Hoist */
					pMCTstat->GStatus |= 1<<GSB_SpIntRemapHole;
					pDCTstat->Status |= 1<<SB_SWNodeHole;
					pMCTstat->GStatus |= 1<<GSB_SoftHole;
					pMCTstat->HoleBase = base;
					limit -= base;
					base = _4GB_RJ8;
					limit += base;
					pDCTstat->DCTSysBase = base;
					pDCTstat->DCTSysLimit = limit;
				} else {
					/* No Remapping.  Normal Contiguous mapping */
					pDCTstat->DCTSysBase = base;
					pDCTstat->DCTSysLimit = limit;
				}
			} else {
				/*No Remapping.  Normal Contiguous mapping*/
				pDCTstat->DCTSysBase = base;
				pDCTstat->DCTSysLimit = limit;
			}
			base |= 3;		/* set WE,RE fields*/
			pMCTstat->SysLimit = limit;
		}
		Set_NB32(dev, 0x40 + (Node << 3), base); /* [Node] + Dram Base 0 */

		val = limit & 0xFFFF0000;
		val |= Node;
		Set_NB32(dev, 0x44 + (Node << 3), val);	/* set DstNode */

		printk(BIOS_DEBUG, " Node: %02x  base: %02x  limit: %02x \n", Node, base, limit);
		limit = pDCTstat->DCTSysLimit;
		if (limit) {
			NextBase = (limit & 0xFFFF0000) + 0x10000;
		}
	}

	/* Copy dram map from Node 0 to Node 1-7 */
	for (Node = 1; Node < MAX_NODES_SUPPORTED; Node++) {
		u32 reg;
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;

		if (pDCTstat->NodePresent) {
			reg = 0x40;		/*Dram Base 0*/
			do {
				val = Get_NB32(dev, reg);
				Set_NB32(devx, reg, val);
				reg += 4;
			} while ( reg < 0x80);
		} else {
			break;			/* stop at first absent Node */
		}
	}

	/*Copy dram map to F1x120/124*/
	mct_HTMemMapExt(pMCTstat, pDCTstatA);
}

static void MCTMemClr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{

	/* Initiates a memory clear operation for all node. The mem clr
	 * is done in parallel. After the memclr is complete, all processors
	 * status are checked to ensure that memclr has completed.
	 */
	u8 Node;
	struct DCTStatStruc *pDCTstat;

	if (!mctGet_NVbits(NV_DQSTrainCTL)){
		/* FIXME: callback to wrapper: mctDoWarmResetMemClr_D */
	} else {	/* NV_DQSTrainCTL == 1 */
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				DCTMemClr_Init_D(pMCTstat, pDCTstat);
			}
		}
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				DCTMemClr_Sync_D(pMCTstat, pDCTstat);
			}
		}
	}
}

static void DCTMemClr_Init_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 val;
	u32 dev;
	u32 reg;

	/* Initiates a memory clear operation on one node */
	if (pDCTstat->DCTSysLimit) {
		dev = pDCTstat->dev_dct;
		reg = 0x110;

		do {
			val = Get_NB32(dev, reg);
		} while (val & (1 << MemClrBusy));

		val |= (1 << MemClrInit);
		Set_NB32(dev, reg, val);
	}
}

static void MCTMemClrSync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	/* Ensures that memory clear has completed on all node.*/
	u8 Node;
	struct DCTStatStruc *pDCTstat;

	if (!mctGet_NVbits(NV_DQSTrainCTL)){
		/* callback to wrapper: mctDoWarmResetMemClr_D */
	} else {	/* NV_DQSTrainCTL == 1 */
		for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
			pDCTstat = pDCTstatA + Node;

			if (pDCTstat->NodePresent) {
				DCTMemClr_Sync_D(pMCTstat, pDCTstat);
			}
		}
	}
}

static void DCTMemClr_Sync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 val;
	u32 dev = pDCTstat->dev_dct;
	u32 reg;

	/* Ensure that a memory clear operation has completed on one node */
	if (pDCTstat->DCTSysLimit){
		reg = 0x110;

		do {
			val = Get_NB32(dev, reg);
		} while (val & (1 << MemClrBusy));

		do {
			val = Get_NB32(dev, reg);
		} while (!(val & (1 << Dr_MemClrStatus)));
	}

	val = 0x0FE40FC0;		/* BKDG recommended */
	val |= MCCH_FlushWrOnStpGnt;	/* Set for S3 */
	Set_NB32(dev, 0x11C, val);
}

static u8 NodePresent_D(u8 Node)
{
	/*
	 * Determine if a single Hammer Node exists within the network.
	 */
	u32 dev;
	u32 val;
	u32 dword;
	u8 ret = 0;

	dev = PA_HOST(Node);		/*test device/vendor id at host bridge  */
	val = Get_NB32(dev, 0);
	dword = mct_NodePresent_D();	/* FIXME: BOZO -11001022h rev for F */
	if (val == dword) {		/* AMD Hammer Family CPU HT Configuration */
		if (oemNodePresent_D(Node, &ret))
			goto finish;
		/* Node ID register */
		val = Get_NB32(dev, 0x60);
		val &= 0x07;
		dword = Node;
		if (val  == dword)	/* current nodeID = requested nodeID ? */
			ret = 1;
	}
finish:
	return ret;
}

static void DCTInit_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat, u8 dct)
{
	/*
	 * Initialize DRAM on single Athlon 64/Opteron Node.
	 */
	u8 stopDCTflag;
	u32 val;

	ClearDCT_D(pMCTstat, pDCTstat, dct);
	stopDCTflag = 1;		/*preload flag with 'disable' */
	/* enable DDR3 support */
	val = Get_NB32(pDCTstat->dev_dct, 0x94 + dct * 0x100);
	val |= 1 << Ddr3Mode;
	Set_NB32(pDCTstat->dev_dct, 0x94 + dct * 0x100, val);
	if (mct_DIMMPresence(pMCTstat, pDCTstat, dct) < SC_StopError) {
		printk(BIOS_DEBUG, "\t\tDCTInit_D: mct_DIMMPresence Done\n");
		if (mct_SPDCalcWidth(pMCTstat, pDCTstat, dct) < SC_StopError) {
			printk(BIOS_DEBUG, "\t\tDCTInit_D: mct_SPDCalcWidth Done\n");
			if (AutoCycTiming_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
				printk(BIOS_DEBUG, "\t\tDCTInit_D: AutoCycTiming_D Done\n");
				if (AutoConfig_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
					printk(BIOS_DEBUG, "\t\tDCTInit_D: AutoConfig_D Done\n");
					if (PlatformSpec_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
						printk(BIOS_DEBUG, "\t\tDCTInit_D: PlatformSpec_D Done\n");
						stopDCTflag = 0;
						if (!(pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW))) {
							printk(BIOS_DEBUG, "\t\tDCTInit_D: StartupDCT_D\n");
							StartupDCT_D(pMCTstat, pDCTstat, dct);   /*yeaahhh! */
						}
					}
				}
			}
		}
	}

	if (stopDCTflag) {
		u32 reg_off = dct * 0x100;
		val = 1<<DisDramInterface;
		Set_NB32(pDCTstat->dev_dct, reg_off+0x94, val);
		/*To maximize power savings when DisDramInterface=1b,
		  all of the MemClkDis bits should also be set.*/
		val = 0xFF000000;
		Set_NB32(pDCTstat->dev_dct, reg_off+0x88, val);
	} else {
		mct_EnDllShutdownSR(pMCTstat, pDCTstat, dct);
	}
}

static void SyncDCTsReady_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	/* Wait (and block further access to dram) for all DCTs to be ready,
	 * by polling all InitDram bits and waiting for possible memory clear
	 * operations to be complete.  Read MemClkFreqVal bit to see if
	 * the DIMMs are present in this node.
	 */
	u8 Node;
	u32 val;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		mct_SyncDCTsReady(pDCTstat);
	}
	/* v6.1.3 */
	/* re-enable phy compensation engine when dram init is completed on all nodes. */
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		if (pDCTstat->NodePresent) {
			if (pDCTstat->DIMMValidDCT[0] > 0 || pDCTstat->DIMMValidDCT[1] > 0) {
				/* re-enable phy compensation engine when dram init on both DCTs is completed. */
				val = Get_NB32_index_wait(pDCTstat->dev_dct, 0x98, 0x8);
				val &= ~(1 << DisAutoComp);
				Set_NB32_index_wait(pDCTstat->dev_dct, 0x98, 0x8, val);
			}
		}
	}
	/* wait 750us before any memory access can be made. */
	mct_Wait(15000);
}

static void StartupDCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Read MemClkFreqVal bit to see if the DIMMs are present in this node.
	 * If the DIMMs are present then set the DRAM Enable bit for this node.
	 *
	 * Setting dram init starts up the DCT state machine, initializes the
	 * dram devices with MRS commands, and kicks off any
	 * HW memory clear process that the chip is capable of.	The sooner
	 * that dram init is set for all nodes, the faster the memory system
	 * initialization can complete.	Thus, the init loop is unrolled into
	 * two loops so as to start the processes for non BSP nodes sooner.
	 * This procedure will not wait for the process to finish.
	 * Synchronization is handled elsewhere.
	 */
	u32 val;
	u32 dev;
	u32 reg_off = dct * 0x100;

	dev = pDCTstat->dev_dct;
	val = Get_NB32(dev, 0x94 + reg_off);
	if (val & (1<<MemClkFreqVal)) {
		mctHookBeforeDramInit();	/* generalized Hook */
		if (!(pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)))
		    mct_DramInit(pMCTstat, pDCTstat, dct);
		AfterDramInit_D(pDCTstat, dct);
		mctHookAfterDramInit();		/* generalized Hook*/
	}
}

static void ClearDCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_end;
	u32 dev = pDCTstat->dev_dct;
	u32 reg = 0x40 + 0x100 * dct;
	u32 val = 0;

	if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
		reg_end = 0x78 + 0x100 * dct;
	} else {
		reg_end = 0xA4 + 0x100 * dct;
	}

	while(reg < reg_end) {
		if ((reg & 0xFF) == 0x90) {
			if (pDCTstat->LogicalCPUID & AMD_DR_Dx) {
				val = Get_NB32(dev, reg); /* get DRAMConfigLow */
				val |= 0x08000000; /* preserve value of DisDllShutdownSR for only Rev.D */
			}
		}
		Set_NB32(dev, reg, val);
		val = 0;
		reg += 4;
	}

	val = 0;
	dev = pDCTstat->dev_map;
	reg = 0xF0;
	Set_NB32(dev, reg, val);
}

static void SPD2ndTiming(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 i;
	u16 Twr, Trtp;
	u16 Trp, Trrd, Trcd, Tras, Trc;
	u8 Trfc[4];
	u16 Tfaw;
	u32 DramTimingLo, DramTimingHi;
	u8 tCK16x;
	u16 Twtr;
	u8 LDIMM;
	u8 MTB16x;
	u8 byte;
	u32 dword;
	u32 dev;
	u32 reg_off;
	u32 val;
	u16 smbaddr;

	/* Gather all DIMM mini-max values for cycle timing data */
	Trp = 0;
	Trrd = 0;
	Trcd = 0;
	Trtp = 0;
	Tras = 0;
	Trc = 0;
	Twr = 0;
	Twtr = 0;
	for (i=0; i < 4; i++)
		Trfc[i] = 0;
	Tfaw = 0;

	for ( i = 0; i< MAX_DIMMS_SUPPORTED; i++) {
		LDIMM = i >> 1;
		if (pDCTstat->DIMMValid & (1 << i)) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, (dct + i));

			val = mctRead_SPD(smbaddr, SPD_MTBDivisor); /* MTB=Dividend/Divisor */
			MTB16x = ((mctRead_SPD(smbaddr, SPD_MTBDividend) & 0xFF)<<4);
			MTB16x /= val; /* transfer to MTB*16 */

			byte = mctRead_SPD(smbaddr, SPD_tRPmin);
			val = byte * MTB16x;
			if (Trp < val)
				Trp = val;

			byte = mctRead_SPD(smbaddr, SPD_tRRDmin);
			val = byte * MTB16x;
			if (Trrd < val)
				Trrd = val;

			byte = mctRead_SPD(smbaddr, SPD_tRCDmin);
			val = byte * MTB16x;
			if (Trcd < val)
				Trcd = val;

			byte = mctRead_SPD(smbaddr, SPD_tRTPmin);
			val = byte * MTB16x;
			if (Trtp < val)
				Trtp = val;

			byte = mctRead_SPD(smbaddr, SPD_tWRmin);
			val = byte * MTB16x;
			if (Twr < val)
				Twr = val;

			byte = mctRead_SPD(smbaddr, SPD_tWTRmin);
			val = byte * MTB16x;
			if (Twtr < val)
				Twtr = val;

			val = mctRead_SPD(smbaddr, SPD_Upper_tRAS_tRC) & 0xFF;
			val >>= 4;
			val <<= 8;
			val |= mctRead_SPD(smbaddr, SPD_tRCmin) & 0xFF;
			val *= MTB16x;
			if (Trc < val)
				Trc = val;

			byte = mctRead_SPD(smbaddr, SPD_Density) & 0xF;
			if (Trfc[LDIMM] < byte)
				Trfc[LDIMM] = byte;

			val = mctRead_SPD(smbaddr, SPD_Upper_tRAS_tRC) & 0xF;
			val <<= 8;
			val |= (mctRead_SPD(smbaddr, SPD_tRASmin) & 0xFF);
			val *= MTB16x;
			if (Tras < val)
				Tras = val;

			val = mctRead_SPD(smbaddr, SPD_Upper_tFAW) & 0xF;
			val <<= 8;
			val |= mctRead_SPD(smbaddr, SPD_tFAWmin) & 0xFF;
			val *= MTB16x;
			if (Tfaw < val)
				Tfaw = val;
		}	/* Dimm Present */
	}

	/* Convert  DRAM CycleTiming values and store into DCT structure */
	byte = pDCTstat->DIMMAutoSpeed;
	if (byte == 7)
		tCK16x = 20;
	else if (byte == 6)
		tCK16x = 24;
	else if (byte == 5)
		tCK16x = 30;
	else
		tCK16x = 40;

	/* Notes:
	 1. All secondary time values given in SPDs are in binary with units of ns.
	 2. Some time values are scaled by 16, in order to have least count of 0.25 ns
	    (more accuracy).  JEDEC SPD spec. shows which ones are x1 and x4.
	 3. Internally to this SW, cycle time, tCK16x, is scaled by 16 to match time values
	*/

	/* Tras */
	pDCTstat->DIMMTras = (u16)Tras;
	val = Tras / tCK16x;
	if (Tras % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrasT)
		val = Min_TrasT;
	else if (val > Max_TrasT)
		val = Max_TrasT;
	pDCTstat->Tras = val;

	/* Trp */
	pDCTstat->DIMMTrp = Trp;
	val = Trp / tCK16x;
	if (Trp % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrpT)
		val = Min_TrpT;
	else if (val > Max_TrpT)
		val = Max_TrpT;
	pDCTstat->Trp = val;

	/*Trrd*/
	pDCTstat->DIMMTrrd = Trrd;
	val = Trrd / tCK16x;
	if (Trrd % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrrdT)
		val = Min_TrrdT;
	else if (val > Max_TrrdT)
		val = Max_TrrdT;
	pDCTstat->Trrd = val;

	/* Trcd */
	pDCTstat->DIMMTrcd = Trcd;
	val = Trcd / tCK16x;
	if (Trcd % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrcdT)
		val = Min_TrcdT;
	else if (val > Max_TrcdT)
		val = Max_TrcdT;
	pDCTstat->Trcd = val;

	/* Trc */
	pDCTstat->DIMMTrc = Trc;
	val = Trc / tCK16x;
	if (Trc % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TrcT)
		val = Min_TrcT;
	else if (val > Max_TrcT)
		val = Max_TrcT;
	pDCTstat->Trc = val;

	/* Trtp */
	pDCTstat->DIMMTrtp = Trtp;
	val = Trtp / tCK16x;
	if (Trtp % tCK16x) {
		val ++;
	}
	if (val < Min_TrtpT)
		val = Min_TrtpT;
	else if (val > Max_TrtpT)
		val = Max_TrtpT;
	pDCTstat->Trtp = val;

	/* Twr */
	pDCTstat->DIMMTwr = Twr;
	val = Twr / tCK16x;
	if (Twr % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TwrT)
		val = Min_TwrT;
	else if (val > Max_TwrT)
		val = Max_TwrT;
	pDCTstat->Twr = val;

	/* Twtr */
	pDCTstat->DIMMTwtr = Twtr;
	val = Twtr / tCK16x;
	if (Twtr % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TwtrT)
		val = Min_TwtrT;
	else if (val > Max_TwtrT)
		val = Max_TwtrT;
	pDCTstat->Twtr = val;

	/* Trfc0-Trfc3 */
	for (i=0; i<4; i++)
		pDCTstat->Trfc[i] = Trfc[i];

	/* Tfaw */
	pDCTstat->DIMMTfaw = Tfaw;
	val = Tfaw / tCK16x;
	if (Tfaw % tCK16x) {	/* round up number of busclocks */
		val++;
	}
	if (val < Min_TfawT)
		val = Min_TfawT;
	else if (val > Max_TfawT)
		val = Max_TfawT;
	pDCTstat->Tfaw = val;

	mctAdjustAutoCycTmg_D();

	/* Program DRAM Timing values */
	DramTimingLo = 0;	/* Dram Timing Low init */
	val = pDCTstat->CASL - 2; /* pDCTstat.CASL to reg. definition */
	DramTimingLo |= val;

	val = pDCTstat->Trcd - Bias_TrcdT;
	DramTimingLo |= val<<4;

	val = pDCTstat->Trp - Bias_TrpT;
	val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
	DramTimingLo |= val<<7;

	val = pDCTstat->Trtp - Bias_TrtpT;
	DramTimingLo |= val<<10;

	val = pDCTstat->Tras - Bias_TrasT;
	DramTimingLo |= val<<12;

	val = pDCTstat->Trc - Bias_TrcT;
	DramTimingLo |= val<<16;

	val = pDCTstat->Trrd - Bias_TrrdT;
	DramTimingLo |= val<<22;

	DramTimingHi = 0;	/* Dram Timing High init */
	val = pDCTstat->Twtr - Bias_TwtrT;
	DramTimingHi |= val<<8;

	val = 2;
	DramTimingHi |= val<<16;

	val = 0;
	for (i=4;i>0;i--) {
		val <<= 3;
		val |= Trfc[i-1];
	}
	DramTimingHi |= val << 20;

	dev = pDCTstat->dev_dct;
	reg_off = 0x100 * dct;
	/* Twr */
	val = pDCTstat->Twr;
	if (val == 10)
		val = 9;
	else if (val == 12)
		val = 10;
	val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
	val -= Bias_TwrT;
	val <<= 4;
	dword = Get_NB32(dev, 0x84 + reg_off);
	dword &= ~0x70;
	dword |= val;
	Set_NB32(dev, 0x84 + reg_off, dword);

	/* Tfaw */
	val = pDCTstat->Tfaw;
	val = mct_AdjustSPDTimings(pMCTstat, pDCTstat, val);
	val -= Bias_TfawT;
	val >>= 1;
	val <<= 28;
	dword = Get_NB32(dev, 0x94 + reg_off);
	dword &= ~0xf0000000;
	dword |= val;
	Set_NB32(dev, 0x94 + reg_off, dword);

	/* dev = pDCTstat->dev_dct; */
	/* reg_off = 0x100 * dct; */

	if (pDCTstat->Speed > 4) {
		val = Get_NB32(dev, 0x88 + reg_off);
		val &= 0xFF000000;
		DramTimingLo |= val;
	}
	Set_NB32(dev, 0x88 + reg_off, DramTimingLo);	/*DCT Timing Low*/

	if (pDCTstat->Speed > 4) {
		DramTimingHi |= 1 << DisAutoRefresh;
	}
	DramTimingHi |= 0x000018FF;
	Set_NB32(dev, 0x8c + reg_off, DramTimingHi);	/*DCT Timing Hi*/

	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */
}

static u8 AutoCycTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Initialize  DCT Timing registers as per DIMM SPD.
	 * For primary timing (T, CL) use best case T value.
	 * For secondary timing params., use most aggressive settings
	 * of slowest DIMM.
	 *
	 * There are three components to determining "maximum frequency":
	 * SPD component, Bus load component, and "Preset" max frequency
	 * component.
	 *
	 * The SPD component is a function of the min cycle time specified
	 * by each DIMM, and the interaction of cycle times from all DIMMs
	 * in conjunction with CAS latency. The SPD component only applies
	 * when user timing mode is 'Auto'.
	 *
	 * The Bus load component is a limiting factor determined by electrical
	 * characteristics on the bus as a result of varying number of device
	 * loads. The Bus load component is specific to each platform but may
	 * also be a function of other factors. The bus load component only
	 * applies when user timing mode is 'Auto'.
	 *
	 * The Preset component is subdivided into three items and is
	 * the minimum of the set: Silicon revision, user limit
	 * setting when user timing mode is 'Auto' and memclock mode
	 * is 'Limit', OEM build specification of the maximum
	 * frequency. The Preset component is only applies when user
	 * timing mode is 'Auto'.
	 */

	/* Get primary timing (CAS Latency and Cycle Time) */
	if (pDCTstat->Speed == 0) {
		mctGet_MaxLoadFreq(pDCTstat);

		/* and Factor in presets (setup options, Si cap, etc.) */
		GetPresetmaxF_D(pMCTstat, pDCTstat);

		/* Go get best T and CL as specified by DIMM mfgs. and OEM */
		SPDGetTCL_D(pMCTstat, pDCTstat, dct);
		/* skip callback mctForce800to1067_D */
		pDCTstat->Speed = pDCTstat->DIMMAutoSpeed;
		pDCTstat->CASL = pDCTstat->DIMMCASL;

	}
	mct_AfterGetCLT(pMCTstat, pDCTstat, dct);

	SPD2ndTiming(pMCTstat, pDCTstat, dct);

	printk(BIOS_DEBUG, "AutoCycTiming: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "AutoCycTiming: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "AutoCycTiming: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "AutoCycTiming: Done\n\n");

	mctHookAfterAutoCycTmg();

	return pDCTstat->ErrCode;
}

static void GetPresetmaxF_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Get max frequency from OEM platform definition, from any user
	 * override (limiting) of max frequency, and from any Si Revision
	 * Specific information.  Return the least of these three in
	 * DCTStatStruc.PresetmaxFreq.
	 */
	/* TODO: Set the proper max frequency in wrappers/mcti_d.c. */
	u16 proposedFreq;
	u16 word;

	/* Get CPU Si Revision defined limit (NPT) */
	proposedFreq = 800;	 /* Rev F0 programmable max memclock is */

	/*Get User defined limit if  "limit" mode */
	if ( mctGet_NVbits(NV_MCTUSRTMGMODE) == 1) {
		word = Get_Fk_D(mctGet_NVbits(NV_MemCkVal) + 1);
		if (word < proposedFreq)
			proposedFreq = word;

		/* Get Platform defined limit */
		word = mctGet_NVbits(NV_MAX_MEMCLK);
		if (word < proposedFreq)
			proposedFreq = word;

		word = pDCTstat->PresetmaxFreq;
		if (word > proposedFreq)
			word = proposedFreq;

		pDCTstat->PresetmaxFreq = word;
	}
	/* Check F3xE8[DdrMaxRate] for maximum DRAM data rate support */
}

static void SPDGetTCL_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Find the best T and CL primary timing parameter pair, per Mfg.,
	 * for the given set of DIMMs, and store into DCTStatStruc
	 * (.DIMMAutoSpeed and .DIMMCASL). See "Global relationship between
	 *  index values and item values" for definition of CAS latency
	 *  index (j) and Frequency index (k).
	 */
	u8 i, CASLatLow, CASLatHigh;
	u16 tAAmin16x;
	u8 MTB16x;
	u16 tCKmin16x;
	u16 tCKproposed16x;
	u8 CLactual, CLdesired, CLT_Fail;

	u8 smbaddr, byte = 0, bytex = 0;

	CASLatLow = 0xFF;
	CASLatHigh = 0xFF;
	tAAmin16x = 0;
	tCKmin16x = 0;
	CLT_Fail = 0;

	for (i = 0; i < MAX_DIMMS_SUPPORTED; i++) {
		if (pDCTstat->DIMMValid & (1 << i)) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, (dct + i));
			/* Step 1: Determine the common set of supported CAS Latency
			 * values for all modules on the memory channel using the CAS
			 * Latencies Supported in SPD bytes 14 and 15.
			 */
			byte = mctRead_SPD(smbaddr, SPD_CASLow);
			CASLatLow &= byte;
			byte = mctRead_SPD(smbaddr, SPD_CASHigh);
			CASLatHigh &= byte;
			/* Step 2: Determine tAAmin(all) which is the largest tAAmin
			   value for all modules on the memory channel (SPD byte 16). */
			byte = mctRead_SPD(smbaddr, SPD_MTBDivisor);

			MTB16x = ((mctRead_SPD(smbaddr, SPD_MTBDividend) & 0xFF)<<4);
			MTB16x /= byte; /* transfer to MTB*16 */

			byte = mctRead_SPD(smbaddr, SPD_tAAmin);
			if (tAAmin16x < byte * MTB16x)
				tAAmin16x = byte * MTB16x;
			/* Step 3: Determine tCKmin(all) which is the largest tCKmin
			   value for all modules on the memory channel (SPD byte 12). */
			byte = mctRead_SPD(smbaddr, SPD_tCKmin);

			if (tCKmin16x < byte * MTB16x)
				tCKmin16x = byte * MTB16x;
		}
	}
	/* calculate tCKproposed16x */
	tCKproposed16x =  16000 / pDCTstat->PresetmaxFreq;
	if (tCKmin16x > tCKproposed16x)
		tCKproposed16x = tCKmin16x;

	/* mctHookTwo1333DimmOverride(); */
	/* For UDIMM, if there are two DDR3-1333 on the same channel,
	   downgrade DDR speed to 1066. */

	/* TODO: get user manual tCK16x(Freq.) and overwrite current tCKproposed16x if manual. */
	if (tCKproposed16x == 20)
		pDCTstat->TargetFreq = 7;
	else if (tCKproposed16x <= 24) {
		pDCTstat->TargetFreq = 6;
		tCKproposed16x = 24;
	}
	else if (tCKproposed16x <= 30) {
		pDCTstat->TargetFreq = 5;
		tCKproposed16x = 30;
	}
	else {
		pDCTstat->TargetFreq = 4;
		tCKproposed16x = 40;
	}
	/* Running through this loop twice:
	   - First time find tCL at target frequency
	   - Second tim find tCL at 400MHz */

	for (;;) {
		CLT_Fail = 0;
		/* Step 4: For a proposed tCK value (tCKproposed) between tCKmin(all) and tCKmax,
		   determine the desired CAS Latency. If tCKproposed is not a standard JEDEC
		   value (2.5, 1.875, 1.5, or 1.25 ns) then tCKproposed must be adjusted to the
		   next lower standard tCK value for calculating CLdesired.
		   CLdesired = ceiling ( tAAmin(all) / tCKproposed )
		   where tAAmin is defined in Byte 16. The ceiling function requires that the
		   quotient be rounded up always. */
		CLdesired = tAAmin16x / tCKproposed16x;
		if (tAAmin16x % tCKproposed16x)
			CLdesired ++;
		/* Step 5: Chose an actual CAS Latency (CLactual) that is greather than or equal
		   to CLdesired and is supported by all modules on the memory channel as
		   determined in step 1. If no such value exists, choose a higher tCKproposed
		   value and repeat steps 4 and 5 until a solution is found. */
		for (i = 0, CLactual = 4; i < 15; i++, CLactual++) {
			if ((CASLatHigh << 8 | CASLatLow) & (1 << i)) {
				if (CLdesired <= CLactual)
					break;
			}
		}
		if (i == 15)
			CLT_Fail = 1;
		/* Step 6: Once the calculation of CLactual is completed, the BIOS must also
		   verify that this CAS Latency value does not exceed tAAmax, which is 20 ns
		   for all DDR3 speed grades, by multiplying CLactual times tCKproposed. If
		   not, choose a lower CL value and repeat steps 5 and 6 until a solution is found. */
		if (CLactual * tCKproposed16x > 320)
			CLT_Fail = 1;
		/* get CL and T */
		if (!CLT_Fail) {
			bytex = CLactual - 2;
			if (tCKproposed16x == 20)
				byte = 7;
			else if (tCKproposed16x == 24)
				byte = 6;
			else if (tCKproposed16x == 30)
				byte = 5;
			else
				byte = 4;
		} else {
			/* mctHookManualCLOverride */
			/* TODO: */
		}

		if (tCKproposed16x != 40) {
			if (pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW)) {
				pDCTstat->DIMMAutoSpeed = byte;
				pDCTstat->DIMMCASL = bytex;
				break;
			} else {
				pDCTstat->TargetCASL = bytex;
				tCKproposed16x = 40;
			}
		} else {
			pDCTstat->DIMMAutoSpeed = byte;
			pDCTstat->DIMMCASL = bytex;
			break;
		}
	}

	printk(BIOS_DEBUG, "SPDGetTCL_D: DIMMCASL %x\n", pDCTstat->DIMMCASL);
	printk(BIOS_DEBUG, "SPDGetTCL_D: DIMMAutoSpeed %x\n", pDCTstat->DIMMAutoSpeed);

	printk(BIOS_DEBUG, "SPDGetTCL_D: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "SPDGetTCL_D: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "SPDGetTCL_D: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "SPDGetTCL_D: Done\n\n");
}

static u8 PlatformSpec_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 dev;
	u32 reg;
	u32 val;

	mctGet_PS_Cfg_D(pMCTstat, pDCTstat, dct);

	if (pDCTstat->GangedMode == 1) {
		mctGet_PS_Cfg_D(pMCTstat, pDCTstat, 1);
		mct_BeforePlatformSpec(pMCTstat, pDCTstat, 1);
	}

	if ( pDCTstat->_2Tmode == 2) {
		dev = pDCTstat->dev_dct;
		reg = 0x94 + 0x100 * dct; /* Dram Configuration Hi */
		val = Get_NB32(dev, reg);
		val |= 1 << 20;		       /* 2T CMD mode */
		Set_NB32(dev, reg, val);
	}

	mct_BeforePlatformSpec(pMCTstat, pDCTstat, dct);
	mct_PlatformSpec(pMCTstat, pDCTstat, dct);
	if (pDCTstat->DIMMAutoSpeed == 4)
		InitPhyCompensation(pMCTstat, pDCTstat, dct);
	mctHookAfterPSCfg();

	return pDCTstat->ErrCode;
}

static u8 AutoConfig_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 DramControl, DramTimingLo, Status;
	u32 DramConfigLo, DramConfigHi, DramConfigMisc, DramConfigMisc2;
	u32 val;
	u32 reg_off;
	u32 dev;
	u16 word;
	u32 dword;
	u8 byte;

	DramConfigLo = 0;
	DramConfigHi = 0;
	DramConfigMisc = 0;
	DramConfigMisc2 = 0;

	/* set bank addressing and Masks, plus CS pops */
	SPDSetBanks_D(pMCTstat, pDCTstat, dct);
	if (pDCTstat->ErrCode == SC_StopError)
		goto AutoConfig_exit;

	/* map chip-selects into local address space */
	StitchMemory_D(pMCTstat, pDCTstat, dct);
	InterleaveBanks_D(pMCTstat, pDCTstat, dct);

	/* temp image of status (for convenience). RO usage! */
	Status = pDCTstat->Status;

	dev = pDCTstat->dev_dct;
	reg_off = 0x100 * dct;


	/* Build Dram Control Register Value */
	DramConfigMisc2 = Get_NB32 (dev, 0xA8 + reg_off);	/* Dram Control*/
	DramControl = Get_NB32 (dev, 0x78 + reg_off);		/* Dram Control*/

	/* FIXME: Skip mct_checkForDxSupport */
	/* REV_CALL mct_DoRdPtrInit if not Dx */
	if (pDCTstat->LogicalCPUID & AMD_DR_Bx)
		val = 5;
	else
		val = 6;
	DramControl &= ~0xFF;
	DramControl |= val;	/* RdPtrInit = 6 for Cx CPU */

	if (mctGet_NVbits(NV_CLKHZAltVidC3))
		DramControl |= 1<<16; /* check */

	DramControl |= 0x00002A00;

	/* FIXME: Skip for Ax versions */
	/* callback not required - if (!mctParityControl_D()) */
	if (Status & (1 << SB_128bitmode))
		DramConfigLo |= 1 << Width128;	/* 128-bit mode (normal) */

	word = dct;
	dword = X4Dimm;
	while (word < 8) {
		if (pDCTstat->Dimmx4Present & (1 << word))
			DramConfigLo |= 1 << dword;	/* X4Dimm[3:0] */
		word++;
		word++;
		dword++;
	}

	if (!(Status & (1 << SB_Registered)))
		DramConfigLo |= 1 << UnBuffDimm;	/* Unbuffered DIMMs */

	if (mctGet_NVbits(NV_ECC_CAP))
		if (Status & (1 << SB_ECCDIMMs))
			if ( mctGet_NVbits(NV_ECC))
				DramConfigLo |= 1 << DimmEcEn;

	DramConfigLo = mct_DisDllShutdownSR(pMCTstat, pDCTstat, DramConfigLo, dct);

	/* Build Dram Config Hi Register Value */
	dword = pDCTstat->Speed;
	DramConfigHi |= dword - 1;	/* get MemClk encoding */
	DramConfigHi |= 1 << MemClkFreqVal;

	if (Status & (1 << SB_Registered))
		if ((pDCTstat->Dimmx4Present != 0) && (pDCTstat->Dimmx8Present != 0))
			/* set only if x8 Registered DIMMs in System*/
			DramConfigHi |= 1 << RDqsEn;

	if (mctGet_NVbits(NV_CKE_CTL))
		/*Chip Select control of CKE*/
		DramConfigHi |= 1 << 16;

	/* Control Bank Swizzle */
	if (0) /* call back not needed mctBankSwizzleControl_D()) */
		DramConfigHi &= ~(1 << BankSwizzleMode);
	else
		DramConfigHi |= 1 << BankSwizzleMode; /* recommended setting (default) */

	/* Check for Quadrank DIMM presence */
	if ( pDCTstat->DimmQRPresent != 0) {
		byte = mctGet_NVbits(NV_4RANKType);
		if (byte == 2)
			DramConfigHi |= 1 << 17;	/* S4 (4-Rank SO-DIMMs) */
		else if (byte == 1)
			DramConfigHi |= 1 << 18;	/* R4 (4-Rank Registered DIMMs) */
	}

	if (0) /* call back not needed mctOverrideDcqBypMax_D ) */
		val = mctGet_NVbits(NV_BYPMAX);
	else
		val = 0x0f; /* recommended setting (default) */
	DramConfigHi |= val << 24;

	if (pDCTstat->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Bx))
		DramConfigHi |= 1 << DcqArbBypassEn;

	/* Build MemClkDis Value from Dram Timing Lo and
	   Dram Config Misc Registers
	 1. We will assume that MemClkDis field has been preset prior to this
	    point.
	 2. We will only set MemClkDis bits if a DIMM is NOT present AND if:
	    NV_AllMemClks <>0 AND SB_DiagClks ==0 */

	/* Dram Timing Low (owns Clock Enable bits) */
	DramTimingLo = Get_NB32(dev, 0x88 + reg_off);
	if (mctGet_NVbits(NV_AllMemClks) == 0) {
		/* Special Jedec SPD diagnostic bit - "enable all clocks" */
		if (!(pDCTstat->Status & (1<<SB_DiagClks))) {
			const u8 *p;
			const u32 *q;
			p = Tab_ManualCLKDis;
			q = (u32 *)p;

			byte = mctGet_NVbits(NV_PACK_TYPE);
			if (byte == PT_L1)
				p = Tab_L1CLKDis;
			else if (byte == PT_M2 || byte == PT_AS)
				p = Tab_AM3CLKDis;
			else
				p = Tab_S1CLKDis;

			dword = 0;
			byte = 0xFF;
			while(dword < MAX_CS_SUPPORTED) {
				if (pDCTstat->CSPresent & (1<<dword)){
					/* re-enable clocks for the enabled CS */
					val = p[dword];
					byte &= ~val;
				}
				dword++ ;
			}
			DramTimingLo |= byte << 24;
		}
	}

	printk(BIOS_DEBUG, "AutoConfig_D: DramControl: %x\n", DramControl);
	printk(BIOS_DEBUG, "AutoConfig_D: DramTimingLo: %x\n", DramTimingLo);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigMisc: %x\n", DramConfigMisc);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigMisc2: %x\n", DramConfigMisc2);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigLo: %x\n", DramConfigLo);
	printk(BIOS_DEBUG, "AutoConfig_D: DramConfigHi: %x\n", DramConfigHi);

	/* Write Values to the registers */
	Set_NB32(dev, 0x78 + reg_off, DramControl);
	Set_NB32(dev, 0x88 + reg_off, DramTimingLo);
	Set_NB32(dev, 0xA0 + reg_off, DramConfigMisc);
	DramConfigMisc2 = mct_SetDramConfigMisc2(pDCTstat, dct, DramConfigMisc2);
	Set_NB32(dev, 0xA8 + reg_off, DramConfigMisc2);
	Set_NB32(dev, 0x90 + reg_off, DramConfigLo);
	ProgDramMRSReg_D(pMCTstat, pDCTstat, dct);
	dword = Get_NB32(dev, 0x94 + reg_off);
	DramConfigHi |= dword;
	mct_SetDramConfigHi_D(pDCTstat, dct, DramConfigHi);
	mct_EarlyArbEn_D(pMCTstat, pDCTstat, dct);
	mctHookAfterAutoCfg();

	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "AutoConfig: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "AutoConfig: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "AutoConfig: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "AutoConfig: Done\n\n");
AutoConfig_exit:
	return pDCTstat->ErrCode;
}

static void SPDSetBanks_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Set bank addressing, program Mask values and build a chip-select
	 * population map. This routine programs PCI 0:24N:2x80 config register
	 * and PCI 0:24N:2x60,64,68,6C config registers (CS Mask 0-3).
	 */
	u8 ChipSel, Rows, Cols, Ranks, Banks;
	u32 BankAddrReg, csMask;

	u32 val;
	u32 reg;
	u32 dev;
	u32 reg_off;
	u8 byte;
	u16 word;
	u32 dword;
	u16 smbaddr;

	dev = pDCTstat->dev_dct;
	reg_off = 0x100 * dct;

	BankAddrReg = 0;
	for (ChipSel = 0; ChipSel < MAX_CS_SUPPORTED; ChipSel+=2) {
		byte = ChipSel;
		if ((pDCTstat->Status & (1 << SB_64MuxedMode)) && ChipSel >=4)
			byte -= 3;

		if (pDCTstat->DIMMValid & (1<<byte)) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, (ChipSel + dct));

			byte = mctRead_SPD(smbaddr, SPD_Addressing);
			Rows = (byte >> 3) & 0x7; /* Rows:0b=12-bit,... */
			Cols = byte & 0x7; /* Cols:0b=9-bit,... */

			byte = mctRead_SPD(smbaddr, SPD_Density);
			Banks = (byte >> 4) & 7; /* Banks:0b=3-bit,... */

			byte = mctRead_SPD(smbaddr, SPD_Organization);
			Ranks = ((byte >> 3) & 7) + 1;

			/* Configure Bank encoding
			 * Use a 6-bit key into a lookup table.
			 * Key (index) = RRRBCC, where CC is the number of Columns minus 9,
			 * RRR is the number of Rows minus 12, and B is the number of banks
			 * minus 3.
			 */
			byte = Cols;
			if (Banks == 1)
				byte |= 4;

			byte |= Rows << 3;	/* RRRBCC internal encode */

			for (dword=0; dword < 13; dword++) {
				if (byte == Tab_BankAddr[dword])
					break;
			}

			if (dword > 12)
				continue;

			/* bit no. of CS field in address mapping reg.*/
			dword <<= (ChipSel<<1);
			BankAddrReg |= dword;

			/* Mask value=(2pow(rows+cols+banks+3)-1)>>8,
			   or 2pow(rows+cols+banks-5)-1*/
			csMask = 0;

			byte = Rows + Cols;		/* cl=rows+cols*/
			byte += 21;			/* row:12+col:9 */
			byte -= 2;			/* 3 banks - 5 */

			if (pDCTstat->Status & (1 << SB_128bitmode))
				byte++;		/* double mask size if in 128-bit mode*/

			csMask |= 1 << byte;
			csMask--;

			/*set ChipSelect population indicator even bits*/
			pDCTstat->CSPresent |= (1<<ChipSel);
			if (Ranks >= 2)
				/*set ChipSelect population indicator odd bits*/
				pDCTstat->CSPresent |= 1 << (ChipSel + 1);

			reg = 0x60+(ChipSel<<1) + reg_off;	/*Dram CS Mask Register */
			val = csMask;
			val &= 0x1FF83FE0;	/* Mask out reserved bits.*/
			Set_NB32(dev, reg, val);
		} else {
			if (pDCTstat->DIMMSPDCSE & (1<<ChipSel))
				pDCTstat->CSTestFail |= (1<<ChipSel);
		}	/* if DIMMValid*/
	}	/* while ChipSel*/

	SetCSTriState(pMCTstat, pDCTstat, dct);
	SetCKETriState(pMCTstat, pDCTstat, dct);
	SetODTTriState(pMCTstat, pDCTstat, dct);

	if (pDCTstat->Status & (1 << SB_128bitmode)) {
		SetCSTriState(pMCTstat, pDCTstat, 1); /* force dct1) */
		SetCKETriState(pMCTstat, pDCTstat, 1); /* force dct1) */
		SetODTTriState(pMCTstat, pDCTstat, 1); /* force dct1) */
	}

	word = pDCTstat->CSPresent;
	mctGetCS_ExcludeMap();		/* mask out specified chip-selects */
	word ^= pDCTstat->CSPresent;
	pDCTstat->CSTestFail |= word;	/* enable ODT to disabled DIMMs */
	if (!pDCTstat->CSPresent)
		pDCTstat->ErrCode = SC_StopError;

	reg = 0x80 + reg_off;		/* Bank Addressing Register */
	Set_NB32(dev, reg, BankAddrReg);

	pDCTstat->CSPresent_DCT[dct] = pDCTstat->CSPresent;
	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "SPDSetBanks: CSPresent %x\n", pDCTstat->CSPresent_DCT[dct]);
	printk(BIOS_DEBUG, "SPDSetBanks: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "SPDSetBanks: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "SPDSetBanks: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "SPDSetBanks: Done\n\n");
}

static void SPDCalcWidth_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Per SPDs, check the symmetry of DIMM pairs (DIMM on Channel A
	 *  matching with DIMM on Channel B), the overall DIMM population,
	 * and determine the width mode: 64-bit, 64-bit muxed, 128-bit.
	 */
	u8 i;
	u8 smbaddr, smbaddr1;
	u8 byte, byte1;

	/* Check Symmetry of Channel A and Channel B DIMMs
	  (must be matched for 128-bit mode).*/
	for (i=0; i < MAX_DIMMS_SUPPORTED; i += 2) {
		if ((pDCTstat->DIMMValid & (1 << i)) && (pDCTstat->DIMMValid & (1<<(i+1)))) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, i);
			smbaddr1 = Get_DIMMAddress_D(pDCTstat, i+1);

			byte = mctRead_SPD(smbaddr, SPD_Addressing) & 0x7;
			byte1 = mctRead_SPD(smbaddr1, SPD_Addressing) & 0x7;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte =	 mctRead_SPD(smbaddr, SPD_Density) & 0x0f;
			byte1 =	 mctRead_SPD(smbaddr1, SPD_Density) & 0x0f;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte = mctRead_SPD(smbaddr, SPD_Organization) & 0x7;
			byte1 = mctRead_SPD(smbaddr1, SPD_Organization) & 0x7;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte = (mctRead_SPD(smbaddr, SPD_Organization) >> 3) & 0x7;
			byte1 = (mctRead_SPD(smbaddr1, SPD_Organization) >> 3) & 0x7;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

			byte = mctRead_SPD(smbaddr, SPD_DMBANKS) & 7;	 /* #ranks-1 */
			byte1 = mctRead_SPD(smbaddr1, SPD_DMBANKS) & 7;	  /* #ranks-1 */
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1<<SB_DimmMismatchO);
				break;
			}

		}
	}

}

static void StitchMemory_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Requires that Mask values for each bank be programmed first and that
	 * the chip-select population indicator is correctly set.
	 */
	u8 b = 0;
	u32 nxtcsBase, curcsBase;
	u8 p, q;
	u32 Sizeq, BiggestBank;
	u8 _DSpareEn;

	u16 word;
	u32 dev;
	u32 reg;
	u32 reg_off;
	u32 val;

	dev = pDCTstat->dev_dct;
	reg_off = 0x100 * dct;

	_DSpareEn = 0;

	/* CS Sparing 1=enabled, 0=disabled */
	if (mctGet_NVbits(NV_CS_SpareCTL) & 1) {
		if (MCT_DIMM_SPARE_NO_WARM) {
			/* Do no warm-reset DIMM spare */
			if (pMCTstat->GStatus & 1 << GSB_EnDIMMSpareNW) {
				word = pDCTstat->CSPresent;
				val = bsf(word);
				word &= ~(1<<val);
				if (word)
					/* Make sure at least two chip-selects are available */
					_DSpareEn = 1;
				else
					pDCTstat->ErrStatus |= 1 << SB_SpareDis;
			}
		} else {
			if (!mctGet_NVbits(NV_DQSTrainCTL)) { /*DQS Training 1=enabled, 0=disabled */
				word = pDCTstat->CSPresent;
				val = bsf(word);
				word &= ~(1 << val);
				if (word)
					/* Make sure at least two chip-selects are available */
					_DSpareEn = 1;
				else
					pDCTstat->ErrStatus |= 1 << SB_SpareDis;
			}
		}
	}

	nxtcsBase = 0;		/* Next available cs base ADDR[39:8] */
	for (p=0; p < MAX_DIMMS_SUPPORTED; p++) {
		BiggestBank = 0;
		for (q = 0; q < MAX_CS_SUPPORTED; q++) { /* from DIMMS to CS */
			if (pDCTstat->CSPresent & (1 << q)) {  /* bank present? */
				reg  = 0x40 + (q << 2) + reg_off;  /* Base[q] reg.*/
				val = Get_NB32(dev, reg);
				if (!(val & 3)) {	/* (CSEnable|Spare==1)bank is enabled already? */
					reg = 0x60 + (q << 1) + reg_off; /*Mask[q] reg.*/
					val = Get_NB32(dev, reg);
					val >>= 19;
					val++;
					val <<= 19;
					Sizeq = val;  /* never used */
					if (val > BiggestBank) {
						/*Bingo! possibly Map this chip-select next! */
						BiggestBank = val;
						b = q;
					}
				}
			}	/*if bank present */
		}	/* while q */
		if (BiggestBank !=0) {
			curcsBase = nxtcsBase;		/* curcsBase=nxtcsBase*/
			/* DRAM CS Base b Address Register offset */
			reg = 0x40 + (b << 2) + reg_off;
			if (_DSpareEn) {
				BiggestBank = 0;
				val = 1 << Spare;	/* Spare Enable*/
			} else {
				val = curcsBase;
				val |= 1 << CSEnable;	/* Bank Enable */
			}
			if (((reg - 0x40) >> 2) & 1) {
				if (!(pDCTstat->Status & (1 << SB_Registered))) {
					u16  dimValid;
					dimValid = pDCTstat->DIMMValid;
					if (dct & 1)
						dimValid <<= 1;
					if ((dimValid & pDCTstat->MirrPresU_NumRegR) != 0) {
						val |= 1 << onDimmMirror;
					}
				}
			}
			Set_NB32(dev, reg, val);
			if (_DSpareEn)
				_DSpareEn = 0;
			else
				/* let nxtcsBase+=Size[b] */
				nxtcsBase += BiggestBank;
		}

		/* bank present but disabled?*/
		if ( pDCTstat->CSTestFail & (1 << p)) {
			/* DRAM CS Base b Address Register offset */
			reg = (p << 2) + 0x40 + reg_off;
			val = 1 << TestFail;
			Set_NB32(dev, reg, val);
		}
	}

	if (nxtcsBase) {
		pDCTstat->DCTSysLimit = nxtcsBase - 1;
		mct_AfterStitchMemory(pMCTstat, pDCTstat, dct);
	}

	/* dump_pci_device(PCI_DEV(0, 0x18+pDCTstat->Node_ID, 2)); */

	printk(BIOS_DEBUG, "StitchMemory: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "StitchMemory: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "StitchMemory: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "StitchMemory: Done\n\n");
}

static u16 Get_Fk_D(u8 k)
{
	return Table_F_k[k]; /* FIXME: k or k<<1 ? */
}

static u8 DIMMPresence_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Check DIMMs present, verify checksum, flag SDRAM type,
	 * build population indicator bitmaps, and preload bus loading
	 * of DIMMs into DCTStatStruc.
	 * MAAload=number of devices on the "A" bus.
	 * MABload=number of devices on the "B" bus.
	 * MAAdimms=number of DIMMs on the "A" bus slots.
	 * MABdimms=number of DIMMs on the "B" bus slots.
	 * DATAAload=number of ranks on the "A" bus slots.
	 * DATABload=number of ranks on the "B" bus slots.
	 */
	u16 i, j;
	u8 smbaddr;
	u8 SPDCtrl;
	u16 RegDIMMPresent, MaxDimms;
	u8 devwidth;
	u16 DimmSlots;
	u8 byte = 0, bytex;

	/* preload data structure with addrs */
	mctGet_DIMMAddr(pDCTstat, pDCTstat->Node_ID);

	DimmSlots = MaxDimms = mctGet_NVbits(NV_MAX_DIMMS);

	SPDCtrl = mctGet_NVbits(NV_SPDCHK_RESTRT);

	RegDIMMPresent = 0;
	pDCTstat->DimmQRPresent = 0;

	for (i = 0; i < MAX_DIMMS_SUPPORTED; i++) {
		if (i >= MaxDimms)
			break;

		if ((pDCTstat->DimmQRPresent & (1 << i)) || (i < DimmSlots)) {
			int status;
			smbaddr = Get_DIMMAddress_D(pDCTstat, i);
			status = mctRead_SPD(smbaddr, SPD_ByteUse);
			if (status >= 0) { /* SPD access is ok */
				pDCTstat->DIMMPresent |= 1 << i;
				if (crcCheck(smbaddr)) { /* CRC is OK */
					byte = mctRead_SPD(smbaddr, SPD_TYPE);
					if (byte == JED_DDR3SDRAM) {
						/*Dimm is 'Present'*/
						pDCTstat->DIMMValid |= 1 << i;
					}
				} else {
					pDCTstat->DIMMSPDCSE = 1 << i;
					if (SPDCtrl == 0) {
						pDCTstat->ErrStatus |= 1 << SB_DIMMChkSum;
						pDCTstat->ErrCode = SC_StopError;
					} else {
						/*if NV_SPDCHK_RESTRT is set to 1, ignore faulty SPD checksum*/
						pDCTstat->ErrStatus |= 1<<SB_DIMMChkSum;
						byte = mctRead_SPD(smbaddr, SPD_TYPE);
						if (byte == JED_DDR3SDRAM)
							pDCTstat->DIMMValid |= 1 << i;
					}
				}
				/* Check module type */
				byte = mctRead_SPD(smbaddr, SPD_DIMMTYPE) & 0x7;
				if (byte == JED_RDIMM || byte == JED_MiniRDIMM)
					RegDIMMPresent |= 1 << i;
				/* Check ECC capable */
				byte = mctRead_SPD(smbaddr, SPD_BusWidth);
				if (byte & JED_ECC) {
					/* DIMM is ECC capable */
					pDCTstat->DimmECCPresent |= 1 << i;
				}
				/* Check if x4 device */
				devwidth = mctRead_SPD(smbaddr, SPD_Organization) & 0x7; /* 0:x4,1:x8,2:x16 */
				if (devwidth == 0) {
					/* DIMM is made with x4 or x16 drams */
					pDCTstat->Dimmx4Present |= 1 << i;
				} else if (devwidth == 1) {
					pDCTstat->Dimmx8Present |= 1 << i;
				} else if (devwidth == 2) {
					pDCTstat->Dimmx16Present |= 1 << i;
				}

				byte = (mctRead_SPD(smbaddr, SPD_Organization) >> 3);
				byte &= 7;
				if (byte == 3) { /* 4ranks */
					/* if any DIMMs are QR, we have to make two passes through DIMMs*/
					if ( pDCTstat->DimmQRPresent == 0) {
						MaxDimms <<= 1;
					}
					if (i < DimmSlots) {
						pDCTstat->DimmQRPresent |= (1 << i) | (1 << (i+4));
					} else {
						pDCTstat->MAdimms[i & 1] --;
					}
					byte = 1;	/* upper two ranks of QR DIMM will be counted on another DIMM number iteration*/
				} else if (byte == 1) { /* 2ranks */
					pDCTstat->DimmDRPresent |= 1 << i;
				}
				bytex = devwidth;
				if (devwidth == 0)
					bytex = 16;
				else if (devwidth == 1)
					bytex = 8;
				else if (devwidth == 2)
					bytex = 4;

				byte++;		/* al+1=rank# */
				if (byte == 2)
					bytex <<= 1;	/*double Addr bus load value for dual rank DIMMs*/

				j = i & (1<<0);
				pDCTstat->DATAload[j] += byte;	/*number of ranks on DATA bus*/
				pDCTstat->MAload[j] += bytex;	/*number of devices on CMD/ADDR bus*/
				pDCTstat->MAdimms[j]++;		/*number of DIMMs on A bus */

				/* check address mirror support for unbuffered dimm */
				/* check number of registers on a dimm for registered dimm */
				byte = mctRead_SPD(smbaddr, SPD_AddressMirror);
				if (RegDIMMPresent & (1 << i)) {
					if ((byte & 3) > 1)
						pDCTstat->MirrPresU_NumRegR |= 1 << i;
				} else {
					if ((byte & 1) == 1)
						pDCTstat->MirrPresU_NumRegR |= 1 << i;
				}
				/* Get byte62: Reference Raw Card information. We dont need it now. */
				/* byte = mctRead_SPD(smbaddr, SPD_RefRawCard); */
				/* Get Byte65/66 for register manufacture ID code */
				if ((0x97 == mctRead_SPD(smbaddr, SPD_RegManufactureID_H)) &&
				    (0x80 == mctRead_SPD(smbaddr, SPD_RegManufactureID_L))) {
					if (0x16 == mctRead_SPD(smbaddr, SPD_RegManRevID))
						pDCTstat->RegMan2Present |= 1 << i;
					else
						pDCTstat->RegMan1Present |= 1 << i;
				}
				/* Get Control word values for RC3. We dont need it. */
				byte = mctRead_SPD(smbaddr, 70);
				pDCTstat->CtrlWrd3 |= (byte >> 4) << (i << 2); /* C3 = SPD byte 70 [7:4] */
				/* Get Control word values for RC4, and RC5 */
				byte = mctRead_SPD(smbaddr, 71);
				pDCTstat->CtrlWrd4 |= (byte & 0xFF) << (i << 2); /* RC4 = SPD byte 71 [3:0] */
				pDCTstat->CtrlWrd5 |= (byte >> 4) << (i << 2); /* RC5 = SPD byte 71 [7:4] */
			}
		}
	}
	printk(BIOS_DEBUG, "\t DIMMPresence: DIMMValid=%x\n", pDCTstat->DIMMValid);
	printk(BIOS_DEBUG, "\t DIMMPresence: DIMMPresent=%x\n", pDCTstat->DIMMPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: RegDIMMPresent=%x\n", RegDIMMPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmECCPresent=%x\n", pDCTstat->DimmECCPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmPARPresent=%x\n", pDCTstat->DimmPARPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: Dimmx4Present=%x\n", pDCTstat->Dimmx4Present);
	printk(BIOS_DEBUG, "\t DIMMPresence: Dimmx8Present=%x\n", pDCTstat->Dimmx8Present);
	printk(BIOS_DEBUG, "\t DIMMPresence: Dimmx16Present=%x\n", pDCTstat->Dimmx16Present);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmPlPresent=%x\n", pDCTstat->DimmPlPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmDRPresent=%x\n", pDCTstat->DimmDRPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DimmQRPresent=%x\n", pDCTstat->DimmQRPresent);
	printk(BIOS_DEBUG, "\t DIMMPresence: DATAload[0]=%x\n", pDCTstat->DATAload[0]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAload[0]=%x\n", pDCTstat->MAload[0]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAdimms[0]=%x\n", pDCTstat->MAdimms[0]);
	printk(BIOS_DEBUG, "\t DIMMPresence: DATAload[1]=%x\n", pDCTstat->DATAload[1]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAload[1]=%x\n", pDCTstat->MAload[1]);
	printk(BIOS_DEBUG, "\t DIMMPresence: MAdimms[1]=%x\n", pDCTstat->MAdimms[1]);

	if (pDCTstat->DIMMValid != 0) {	/* If any DIMMs are present...*/
		if (RegDIMMPresent != 0) {
			if ((RegDIMMPresent ^ pDCTstat->DIMMValid) !=0) {
				/* module type DIMM mismatch (reg'ed, unbuffered) */
				pDCTstat->ErrStatus |= 1<<SB_DimmMismatchM;
				pDCTstat->ErrCode = SC_StopError;
			} else{
				/* all DIMMs are registered */
				pDCTstat->Status |= 1<<SB_Registered;
			}
		}
		if (pDCTstat->DimmECCPresent != 0) {
			if ((pDCTstat->DimmECCPresent ^ pDCTstat->DIMMValid )== 0) {
				/* all DIMMs are ECC capable */
				pDCTstat->Status |= 1<<SB_ECCDIMMs;
			}
		}
		if (pDCTstat->DimmPARPresent != 0) {
			if ((pDCTstat->DimmPARPresent ^ pDCTstat->DIMMValid) == 0) {
				/*all DIMMs are Parity capable */
				pDCTstat->Status |= 1<<SB_PARDIMMs;
			}
		}
	} else {
		/* no DIMMs present or no DIMMs that qualified. */
		pDCTstat->ErrStatus |= 1<<SB_NoDimms;
		pDCTstat->ErrCode = SC_StopError;
	}

	printk(BIOS_DEBUG, "\t DIMMPresence: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "\t DIMMPresence: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "\t DIMMPresence: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "\t DIMMPresence: Done\n\n");

	mctHookAfterDIMMpre();

	return pDCTstat->ErrCode;
}

static u8 Get_DIMMAddress_D(struct DCTStatStruc *pDCTstat, u8 i)
{
	u8 *p;

	p = pDCTstat->DIMMAddr;
	/* mct_BeforeGetDIMMAddress(); */
	return p[i];
}

static void mct_initDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 val;
	u8 err_code;

	/* Config. DCT0 for Ganged or unganged mode */
	DCTInit_D(pMCTstat, pDCTstat, 0);
	if (pDCTstat->ErrCode == SC_FatalErr) {
		/* Do nothing goto exitDCTInit; any fatal errors? */
	} else {
		/* Configure DCT1 if unganged and enabled*/
		if (!pDCTstat->GangedMode) {
			if (pDCTstat->DIMMValidDCT[1] > 0) {
				err_code = pDCTstat->ErrCode;		/* save DCT0 errors */
				pDCTstat->ErrCode = 0;
				DCTInit_D(pMCTstat, pDCTstat, 1);
				if (pDCTstat->ErrCode == 2)		/* DCT1 is not Running */
					pDCTstat->ErrCode = err_code;	/* Using DCT0 Error code to update pDCTstat.ErrCode */
			} else {
				val = 1 << DisDramInterface;
				Set_NB32(pDCTstat->dev_dct, 0x100 + 0x94, val);
			}
		}
	}
/* exitDCTInit: */
}

static void mct_DramInit(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	mct_BeforeDramInit_Prod_D(pMCTstat, pDCTstat);
	mct_DramInit_Sw_D(pMCTstat, pDCTstat, dct);
	/* mct_DramInit_Hw_D(pMCTstat, pDCTstat, dct); */
}

static u8 mct_setMode(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u8 byte;
	u8 bytex;
	u32 val;
	u32 reg;

	byte = bytex = pDCTstat->DIMMValid;
	bytex &= 0x55;		/* CHA DIMM pop */
	pDCTstat->DIMMValidDCT[0] = bytex;

	byte &= 0xAA;		/* CHB DIMM popa */
	byte >>= 1;
	pDCTstat->DIMMValidDCT[1] = byte;

	if (byte != bytex) {
		pDCTstat->ErrStatus &= ~(1 << SB_DimmMismatchO);
	} else {
		byte = mctGet_NVbits(NV_Unganged);
		if (byte)
			pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO); /* Set temp. to avoid setting of ganged mode */

		if (!(pDCTstat->ErrStatus & (1 << SB_DimmMismatchO))) {
			pDCTstat->GangedMode = 1;
			/* valid 128-bit mode population. */
			pDCTstat->Status |= 1 << SB_128bitmode;
			reg = 0x110;
			val = Get_NB32(pDCTstat->dev_dct, reg);
			val |= 1 << DctGangEn;
			Set_NB32(pDCTstat->dev_dct, reg, val);
		}
		if (byte)	/* NV_Unganged */
			pDCTstat->ErrStatus &= ~(1 << SB_DimmMismatchO); /* Clear so that there is no DIMM missmatch error */
	}
	return pDCTstat->ErrCode;
}

u32 Get_NB32(u32 dev, u32 reg)
{
	return pci_read_config32(dev, reg);
}

void Set_NB32(u32 dev, u32 reg, u32 val)
{
	pci_write_config32(dev, reg, val);
}


u32 Get_NB32_index(u32 dev, u32 index_reg, u32 index)
{
	u32 dword;

	Set_NB32(dev, index_reg, index);
	dword = Get_NB32(dev, index_reg+0x4);

	return dword;
}

void Set_NB32_index(u32 dev, u32 index_reg, u32 index, u32 data)
{
	Set_NB32(dev, index_reg, index);
	Set_NB32(dev, index_reg + 0x4, data);
}

u32 Get_NB32_index_wait(u32 dev, u32 index_reg, u32 index)
{

	u32 dword;


	index &= ~(1 << DctAccessWrite);
	Set_NB32(dev, index_reg, index);
	do {
		dword = Get_NB32(dev, index_reg);
	} while (!(dword & (1 << DctAccessDone)));
	dword = Get_NB32(dev, index_reg + 0x4);

	return dword;
}

void Set_NB32_index_wait(u32 dev, u32 index_reg, u32 index, u32 data)
{
	u32 dword;


	Set_NB32(dev, index_reg + 0x4, data);
	index |= (1 << DctAccessWrite);
	Set_NB32(dev, index_reg, index);
	do {
		dword = Get_NB32(dev, index_reg);
	} while (!(dword & (1 << DctAccessDone)));

}

static u8 mct_BeforePlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* mct_checkForCxDxSupport_D */
	if (pDCTstat->LogicalCPUID & AMD_DR_GT_Bx) {
		/* 1. Write 00000000h to F2x[1,0]9C_xD08E000 */
		Set_NB32_index_wait(pDCTstat->dev_dct, 0x98 + dct * 0x100, 0x0D08E000, 0);
		/* 2. If DRAM Configuration Register[MemClkFreq] (F2x[1,0]94[2:0]) is
		   greater than or equal to 011b (DDR-800 and higher),
		   then write 00000080h to F2x[1,0]9C_xD02E001,
		   else write 00000090h to F2x[1,0]9C_xD02E001. */
		if (pDCTstat->Speed >= 4)
			Set_NB32_index_wait(pDCTstat->dev_dct, 0x98 + dct * 0x100, 0xD02E001, 0x80);
		else
			Set_NB32_index_wait(pDCTstat->dev_dct, 0x98 + dct * 0x100, 0xD02E001, 0x90);
	}
	return pDCTstat->ErrCode;
}

static u8 mct_PlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	/* Get platform specific config/timing values from the interface layer
	 * and program them into DCT.
	 */

	u32 dev = pDCTstat->dev_dct;
	u32 index_reg;
	u8 i, i_start, i_end;

	if (pDCTstat->GangedMode) {
		SyncSetting(pDCTstat);
		/* mct_SetupSync_D */
		i_start = 0;
		i_end = 2;
	} else {
		i_start = dct;
		i_end = dct + 1;
	}
	for (i=i_start; i<i_end; i++) {
		index_reg = 0x98 + (i * 0x100);
		Set_NB32_index_wait(dev, index_reg, 0x00, pDCTstat->CH_ODC_CTL[i]); /* Channel A Output Driver Compensation Control */
		Set_NB32_index_wait(dev, index_reg, 0x04, pDCTstat->CH_ADDR_TMG[i]); /* Channel A Output Driver Compensation Control */
	}

	return pDCTstat->ErrCode;
}

static void mct_SyncDCTsReady(struct DCTStatStruc *pDCTstat)
{
	u32 dev;
	u32 val;

	if (pDCTstat->NodePresent) {
		dev = pDCTstat->dev_dct;

		if ((pDCTstat->DIMMValidDCT[0] ) || (pDCTstat->DIMMValidDCT[1])) {		/* This Node has dram */
			do {
				val = Get_NB32(dev, 0x110);
			} while (!(val & (1 << DramEnabled)));
		}
	}	/* Node is present */
}

static void mct_AfterGetCLT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (!pDCTstat->GangedMode) {
		if (dct == 0 ) {
			pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[dct];
			if (pDCTstat->DIMMValidDCT[dct] == 0)
				pDCTstat->ErrCode = SC_StopError;
		} else {
			pDCTstat->CSPresent = 0;
			pDCTstat->CSTestFail = 0;
			pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[dct];
			if (pDCTstat->DIMMValidDCT[dct] == 0)
				pDCTstat->ErrCode = SC_StopError;
		}
	}
}

static u8 mct_SPDCalcWidth(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 ret;
	u32 val;

	if ( dct == 0) {
		SPDCalcWidth_D(pMCTstat, pDCTstat);
		ret = mct_setMode(pMCTstat, pDCTstat);
	} else {
		ret = pDCTstat->ErrCode;
	}

	if (pDCTstat->DIMMValidDCT[0] == 0) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94);
		val |= 1 << DisDramInterface;
		Set_NB32(pDCTstat->dev_dct, 0x94, val);
	}
	if (pDCTstat->DIMMValidDCT[1] == 0) {
		val = Get_NB32(pDCTstat->dev_dct, 0x94 + 0x100);
		val |= 1 << DisDramInterface;
		Set_NB32(pDCTstat->dev_dct, 0x94 + 0x100, val);
	}

	printk(BIOS_DEBUG, "SPDCalcWidth: Status %x\n", pDCTstat->Status);
	printk(BIOS_DEBUG, "SPDCalcWidth: ErrStatus %x\n", pDCTstat->ErrStatus);
	printk(BIOS_DEBUG, "SPDCalcWidth: ErrCode %x\n", pDCTstat->ErrCode);
	printk(BIOS_DEBUG, "SPDCalcWidth: Done\n");
	/* Disable dram interface before DRAM init */

	return ret;
}

static void mct_AfterStitchMemory(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dword;
	u32 dev;
	u32 reg;
	u8 _MemHoleRemap;
	u32 DramHoleBase;

	_MemHoleRemap = mctGet_NVbits(NV_MemHole);
	DramHoleBase = mctGet_NVbits(NV_BottomIO);
	DramHoleBase <<= 8;
	/* Increase hole size so;[31:24]to[31:16]
	 * it has granularity of 128MB shl eax,8
	 * Set 'effective' bottom IOmov DramHoleBase,eax
	 */
	pMCTstat->HoleBase = (DramHoleBase & 0xFFFFF800) << 8;

	/* In unganged mode, we must add DCT0 and DCT1 to DCTSysLimit */
	if (!pDCTstat->GangedMode) {
		dev = pDCTstat->dev_dct;
		pDCTstat->NodeSysLimit += pDCTstat->DCTSysLimit;
		/* if DCT0 and DCT1 both exist, set DctSelBaseAddr[47:27] to the top of DCT0 */
		if (dct == 0) {
			if (pDCTstat->DIMMValidDCT[1] > 0) {
				dword = pDCTstat->DCTSysLimit + 1;
				dword += pDCTstat->NodeSysBase;
				dword >>= 8; /* scale [39:8] to [47:27],and to F2x110[31:11] */
				if ((dword >= DramHoleBase) && _MemHoleRemap) {
					pMCTstat->HoleBase = (DramHoleBase & 0xFFFFF800) << 8;
					val = pMCTstat->HoleBase;
					val >>= 16;
					val = (((~val) & 0xFF) + 1);
					val <<= 8;
					dword += val;
				}
				reg = 0x110;
				val = Get_NB32(dev, reg);
				val &= 0x7F;
				val |= dword;
				val |= 3;  /* Set F2x110[DctSelHiRngEn], F2x110[DctSelHi] */
				Set_NB32(dev, reg, val);

				reg = 0x114;
				val = dword;
				Set_NB32(dev, reg, val);
			}
		} else {
			/* Program the DctSelBaseAddr value to 0
			   if DCT 0 is disabled */
			if (pDCTstat->DIMMValidDCT[0] == 0) {
				dword = pDCTstat->NodeSysBase;
				dword >>= 8;
				if ((dword >= DramHoleBase) && _MemHoleRemap) {
					pMCTstat->HoleBase = (DramHoleBase & 0xFFFFF800) << 8;
					val = pMCTstat->HoleBase;
					val >>= 8;
					val &= ~(0xFFFF);
					val |= (((~val) & 0xFFFF) + 1);
					dword += val;
				}
				reg = 0x114;
				val = dword;
				Set_NB32(dev, reg, val);

				reg = 0x110;
				val |= 3;	/* Set F2x110[DctSelHiRngEn], F2x110[DctSelHi] */
				Set_NB32(dev, reg, val);
			}
		}
	} else {
		pDCTstat->NodeSysLimit += pDCTstat->DCTSysLimit;
	}
	printk(BIOS_DEBUG, "AfterStitch pDCTstat->NodeSysBase = %x\n", pDCTstat->NodeSysBase);
	printk(BIOS_DEBUG, "mct_AfterStitchMemory: pDCTstat->NodeSysLimit = %x\n", pDCTstat->NodeSysLimit);
}

static u8 mct_DIMMPresence(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 ret;

	if (dct == 0)
		ret = DIMMPresence_D(pMCTstat, pDCTstat);
	else
		ret = pDCTstat->ErrCode;

	return ret;
}

/* mct_BeforeGetDIMMAddress inline in C */

static void mct_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		if (pDCTstat->NodePresent) {
			if (pDCTstat->DIMMValidDCT[0]) {
				pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[0];
				Set_OtherTiming(pMCTstat, pDCTstat, 0);
			}
			if (pDCTstat->DIMMValidDCT[1] && !pDCTstat->GangedMode ) {
				pDCTstat->DIMMValid = pDCTstat->DIMMValidDCT[1];
				Set_OtherTiming(pMCTstat, pDCTstat, 1);
			}
		}	/* Node is present*/
	}	/* while Node */
}

static void Set_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg;
	u32 reg_off = 0x100 * dct;
	u32 val;
	u32 dword;
	u32 dev = pDCTstat->dev_dct;

	Get_DqsRcvEnGross_Diff(pDCTstat, dev, 0x98 + reg_off);
	Get_WrDatGross_Diff(pDCTstat, dct, dev, 0x98 + reg_off);
	Get_Trdrd(pMCTstat, pDCTstat, dct);
	Get_Twrwr(pMCTstat, pDCTstat, dct);
	Get_Twrrd(pMCTstat, pDCTstat, dct);
	Get_TrwtTO(pMCTstat, pDCTstat, dct);
	Get_TrwtWB(pMCTstat, pDCTstat);

	reg = 0x8C + reg_off;		/* Dram Timing Hi */
	val = Get_NB32(dev, reg);
	val &= 0xffff0300;
	dword = pDCTstat->TrwtTO;
	val |= dword << 4;
	dword = pDCTstat->Twrrd & 3;
	val |= dword << 10;
	dword = pDCTstat->Twrwr & 3;
	val |= dword << 12;
	dword = pDCTstat->Trdrd & 3;
	val |= dword << 14;
	dword = pDCTstat->TrwtWB;
	val |= dword;
	Set_NB32(dev, reg, val);

	reg = 0x78 + reg_off;
	val = Get_NB32(dev, reg);
	val &= 0xFFFFC0FF;
	dword = pDCTstat->Twrrd >> 2;
	val |= dword << 8;
	dword = pDCTstat->Twrwr >> 2;
	val |= dword << 10;
	dword = pDCTstat->Trdrd >> 2;
	val |= dword << 12;
	Set_NB32(dev, reg, val);
}

static void Get_Trdrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	int8_t Trdrd;

	Trdrd = ((int8_t)(pDCTstat->DqsRcvEnGrossMax - pDCTstat->DqsRcvEnGrossMin) >> 1) + 1;
	if (Trdrd > 8)
		Trdrd = 8;
	pDCTstat->Trdrd = Trdrd;
}

static void Get_Twrwr(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	int8_t Twrwr = 0;

	Twrwr = ((int8_t)(pDCTstat->WrDatGrossMax - pDCTstat->WrDatGrossMin) >> 1) + 2;

	if (Twrwr < 2)
		Twrwr = 2;
	else if (Twrwr > 9)
		Twrwr = 9;

	pDCTstat->Twrwr = Twrwr;
}

static void Get_Twrrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 LDplus1;
	int8_t Twrrd;

	LDplus1 = Get_Latency_Diff(pMCTstat, pDCTstat, dct);

	Twrrd = ((int8_t)(pDCTstat->WrDatGrossMax - pDCTstat->DqsRcvEnGrossMin) >> 1) + 4 - LDplus1;

	if (Twrrd < 2)
		Twrrd = 2;
	else if (Twrrd > 10)
		Twrrd = 10;
	pDCTstat->Twrrd = Twrrd;
}

static void Get_TrwtTO(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 LDplus1;
	int8_t TrwtTO;

	LDplus1 = Get_Latency_Diff(pMCTstat, pDCTstat, dct);

	TrwtTO = ((int8_t)(pDCTstat->DqsRcvEnGrossMax - pDCTstat->WrDatGrossMin) >> 1) + LDplus1;

	pDCTstat->TrwtTO = TrwtTO;
}

static void Get_TrwtWB(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	/* TrwtWB ensures read-to-write data-bus turnaround.
	   This value should be one more than the programmed TrwtTO.*/
	pDCTstat->TrwtWB = pDCTstat->TrwtTO;
}

static u8 Get_Latency_Diff(struct MCTStatStruc *pMCTstat,
			   struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_off =  0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 val1, val2;

	val1 = Get_NB32(dev, reg_off + 0x88) & 0xF;
	val2 = (Get_NB32(dev, reg_off + 0x84) >> 20) & 7;

	return val1 - val2;
}

static void Get_DqsRcvEnGross_Diff(struct DCTStatStruc *pDCTstat,
					u32 dev, u32 index_reg)
{
	u8 Smallest, Largest;
	u32 val;
	u8 byte, bytex;

	/* The largest DqsRcvEnGrossDelay of any DIMM minus the
	   DqsRcvEnGrossDelay of any other DIMM is equal to the Critical
	   Gross Delay Difference (CGDD) */
	/* DqsRcvEn byte 1,0 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, index_reg, 0x10);
	Largest = val & 0xFF;
	Smallest = (val >> 8) & 0xFF;

	/* DqsRcvEn byte 3,2 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, index_reg, 0x11);
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	/* DqsRcvEn byte 5,4 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, index_reg, 0x20);
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	/* DqsRcvEn byte 7,6 */
	val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, index_reg, 0x21);
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	if (pDCTstat->DimmECCPresent> 0) {
		/*DqsRcvEn Ecc */
		val = Get_DqsRcvEnGross_MaxMin(pDCTstat, dev, index_reg, 0x12);
		byte = val & 0xFF;
		bytex = (val >> 8) & 0xFF;
		if (bytex < Smallest)
			Smallest = bytex;
		if (byte > Largest)
			Largest = byte;
	}

	pDCTstat->DqsRcvEnGrossMax = Largest;
	pDCTstat->DqsRcvEnGrossMin = Smallest;
}

static void Get_WrDatGross_Diff(struct DCTStatStruc *pDCTstat,
					u8 dct, u32 dev, u32 index_reg)
{
	u8 Smallest = 0, Largest = 0;
	u32 val;
	u8 byte, bytex;

	/* The largest WrDatGrossDlyByte of any DIMM minus the
	  WrDatGrossDlyByte of any other DIMM is equal to CGDD */
	if (pDCTstat->DIMMValid & (1 << 0)) {
		val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x01);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM0 */
		Largest = val & 0xFF;
		Smallest = (val >> 8) & 0xFF;
	}
	if (pDCTstat->DIMMValid & (1 << 2)) {
		val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x101);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM1 */
		byte = val & 0xFF;
		bytex = (val >> 8) & 0xFF;
		if (bytex < Smallest)
			Smallest = bytex;
		if (byte > Largest)
			Largest = byte;
	}

	/* If Cx, 2 more dimm need to be checked to find out the largest and smallest */
	if (pDCTstat->LogicalCPUID & AMD_DR_Cx) {
		if (pDCTstat->DIMMValid & (1 << 4)) {
			val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x201);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM2 */
			byte = val & 0xFF;
			bytex = (val >> 8) & 0xFF;
			if (bytex < Smallest)
				Smallest = bytex;
			if (byte > Largest)
				Largest = byte;
		}
		if (pDCTstat->DIMMValid & (1 << 6)) {
			val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x301);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM2 */
			byte = val & 0xFF;
			bytex = (val >> 8) & 0xFF;
			if (bytex < Smallest)
				Smallest = bytex;
			if (byte > Largest)
				Largest = byte;
		}
	}

	pDCTstat->WrDatGrossMax = Largest;
	pDCTstat->WrDatGrossMin = Smallest;
}

static u16 Get_DqsRcvEnGross_MaxMin(struct DCTStatStruc *pDCTstat,
					u32 dev, u32 index_reg,
					u32 index)
{
	u8 Smallest, Largest;
	u8 i;
	u8 byte;
	u32 val;
	u16 word;
	u8 ecc_reg = 0;

	Smallest = 7;
	Largest = 0;

	if (index == 0x12)
		ecc_reg = 1;

	for (i=0; i < 8; i+=2) {
		if ( pDCTstat->DIMMValid & (1 << i)) {
			val = Get_NB32_index_wait(dev, index_reg, index);
			val &= 0x00E000E0;
			byte = (val >> 5) & 0xFF;
			if (byte < Smallest)
				Smallest = byte;
			if (byte > Largest)
				Largest = byte;
			if (!(ecc_reg)) {
				byte = (val >> (16 + 5)) & 0xFF;
				if (byte < Smallest)
					Smallest = byte;
				if (byte > Largest)
					Largest = byte;
			}
		}
		index += 3;
	}	/* while ++i */

	word = Smallest;
	word <<= 8;
	word |= Largest;

	return word;
}

static u16 Get_WrDatGross_MaxMin(struct DCTStatStruc *pDCTstat,
					u8 dct, u32 dev, u32 index_reg,
					u32 index)
{
	u8 Smallest, Largest;
	u8 i, j;
	u32 val;
	u8 byte;
	u16 word;

	Smallest = 3;
	Largest = 0;
	for (i=0; i < 2; i++) {
		val = Get_NB32_index_wait(dev, index_reg, index);
		val &= 0x60606060;
		val >>= 5;
		for (j=0; j < 4; j++) {
			byte = val & 0xFF;
			if (byte < Smallest)
				Smallest = byte;
			if (byte > Largest)
				Largest = byte;
			val >>= 8;
		}	/* while ++j */
		index++;
	}	/*while ++i*/

	if (pDCTstat->DimmECCPresent > 0) {
		index++;
		val = Get_NB32_index_wait(dev, index_reg, index);
		val &= 0x00000060;
		val >>= 5;
		byte = val & 0xFF;
		if (byte < Smallest)
			Smallest = byte;
		if (byte > Largest)
			Largest = byte;
	}

	word = Smallest;
	word <<= 8;
	word |= Largest;

	return word;
}

static void mct_PhyController_Config(struct MCTStatStruc *pMCTstat,
				     struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3 | AMD_RB_C3)) {
		if (pDCTstat->Dimmx4Present == 0) {
			/* Set bit7 RxDqsUDllPowerDown  to register F2x[1, 0]98_x0D0F0F13 for power saving */
			val = Get_NB32_index_wait(dev, index_reg, 0x0D0F0F13); /* Agesa v3 v6 might be wrong here. */
			val |= 1 << 7; /* BIOS should set this bit when x4 DIMMs are not present */
			Set_NB32_index_wait(dev, index_reg, 0x0D0F0F13, val);
		}
	}

	if (pDCTstat->LogicalCPUID & AMD_DR_DAC2_OR_C3) {
		if (pDCTstat->DimmECCPresent == 0) {
			/* Set bit4 PwrDn to register F2x[1, 0]98_x0D0F0830 for power saving */
			val = Get_NB32_index_wait(dev, index_reg, 0x0D0F0830);
			val |= 1 << 4; /* BIOS should set this bit if ECC DIMMs are not present */
			Set_NB32_index_wait(dev, index_reg, 0x0D0F0830, val);
		}
	}

}

static void mct_FinalMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	struct DCTStatStruc *pDCTstat;
	u32 val;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			mct_PhyController_Config(pMCTstat, pDCTstat, 0);
			mct_PhyController_Config(pMCTstat, pDCTstat, 1);
		}
		if (!(pDCTstat->LogicalCPUID & AMD_DR_Dx)) { /* mct_checkForDxSupport */
			mct_ExtMCTConfig_Cx(pDCTstat);
			mct_ExtMCTConfig_Bx(pDCTstat);
		} else {	/* For Dx CPU */
			val = 0x0CE00F00 | 1 << 29/* FlushWrOnStpGnt */;
			if (!(pDCTstat->GangedMode))
				val |= 0x20; /* MctWrLimit =  8 for Unganed mode */
			else
				val |= 0x40; /* MctWrLimit =  16 for ganed mode */
			Set_NB32(pDCTstat->dev_dct, 0x11C, val);

			val = Get_NB32(pDCTstat->dev_dct, 0x1B0);
			val &= 0xFFFFF8C0;
			val |= 0x101;	/* BKDG recommended settings */
			val |= 0x0FC00000; /* Agesa V5 */
			if (!(pDCTstat->GangedMode))
				val |= 1 << 12;
			else
				val &= ~(1 << 12);

			val &= 0x0FFFFFFF;
			switch (pDCTstat->Speed) {
			case 4:
				val |= 0x50000000; /* 5 for DDR800 */
				break;
			case 5:
				val |= 0x60000000; /* 6 for DDR1066 */
				break;
			case 6:
				val |= 0x80000000; /* 8 for DDR800 */
				break;
			default:
				val |= 0x90000000; /* 9 for DDR1600 */
				break;
			}
			Set_NB32(pDCTstat->dev_dct, 0x1B0, val);
		}
	}

	/* ClrClToNB_D postponed until we're done executing from ROM */
	mct_ClrWbEnhWsbDis_D(pMCTstat, pDCTstat);

	/* set F3x8C[DisFastTprWr] on all DR, if L3Size=0 */
	if (pDCTstat->LogicalCPUID & AMD_DR_ALL) {
		if (!(cpuid_edx(0x80000006) & 0xFFFC0000)) {
			val = Get_NB32(pDCTstat->dev_nbmisc, 0x8C);
			val |= 1 << 24;
			Set_NB32(pDCTstat->dev_nbmisc, 0x8C, val);
		}
	}
}

static void mct_InitialMCT_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	mct_SetClToNB_D(pMCTstat, pDCTstat);
	mct_SetWbEnhWsbDis_D(pMCTstat, pDCTstat);
}

static u32 mct_NodePresent_D(void)
{
	u32 val;
	val = 0x12001022;
	return val;
}

static void mct_init(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 addr;

	pDCTstat->GangedMode = 0;
	pDCTstat->DRPresent = 1;

	/* enable extend PCI configuration access */
	addr = 0xC001001F;
	_RDMSR(addr, &lo, &hi);
	if (hi & (1 << (46-32))) {
		pDCTstat->Status |= 1 << SB_ExtConfig;
	} else {
		hi |= 1 << (46-32);
		_WRMSR(addr, lo, hi);
	}
}

static void clear_legacy_Mode(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 reg;
	u32 val;
	u32 dev = pDCTstat->dev_dct;

	/* Clear Legacy BIOS Mode bit */
	reg = 0x94;
	val = Get_NB32(dev, reg);
	val &= ~(1<<LegacyBiosMode);
	Set_NB32(dev, reg, val);

	reg = 0x94 + 0x100;
	val = Get_NB32(dev, reg);
	val &= ~(1<<LegacyBiosMode);
	Set_NB32(dev, reg, val);
}

static void mct_HTMemMapExt(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u32 Drambase, Dramlimit;
	u32 val;
	u32 reg;
	u32 dev;
	u32 devx;
	u32 dword;
	struct DCTStatStruc *pDCTstat;

	pDCTstat = pDCTstatA + 0;
	dev = pDCTstat->dev_map;

	/* Copy dram map from F1x40/44,F1x48/4c,
	  to F1x120/124(Node0),F1x120/124(Node1),...*/
	for (Node=0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;

		/* get base/limit from Node0 */
		reg = 0x40 + (Node << 3);		/* Node0/Dram Base 0 */
		val = Get_NB32(dev, reg);
		Drambase = val >> ( 16 + 3);

		reg = 0x44 + (Node << 3);		/* Node0/Dram Base 0 */
		val = Get_NB32(dev, reg);
		Dramlimit = val >> (16 + 3);

		/* set base/limit to F1x120/124 per Node */
		if (pDCTstat->NodePresent) {
			reg = 0x120;		/* F1x120,DramBase[47:27] */
			val = Get_NB32(devx, reg);
			val &= 0xFFE00000;
			val |= Drambase;
			Set_NB32(devx, reg, val);

			reg = 0x124;
			val = Get_NB32(devx, reg);
			val &= 0xFFE00000;
			val |= Dramlimit;
			Set_NB32(devx, reg, val);

			if ( pMCTstat->GStatus & ( 1 << GSB_HWHole)) {
				reg = 0xF0;
				val = Get_NB32(devx, reg);
				val |= (1 << DramMemHoistValid);
				val &= ~(0xFF << 24);
				dword = (pMCTstat->HoleBase >> (24 - 8)) & 0xFF;
				dword <<= 24;
				val |= dword;
				Set_NB32(devx, reg, val);
			}

		}
	}
}

static void SetCSTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev = pDCTstat->dev_dct;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 index;
	u16 word;

	/* Tri-state unused chipselects when motherboard
	   termination is available */

	/* FIXME: skip for Ax */

	word = pDCTstat->CSPresent;
	if (pDCTstat->Status & (1 << SB_Registered)) {
		word |= (word & 0x55) << 1;
	}
	word = (~word) & 0xFF;
	index  = 0x0c;
	val = Get_NB32_index_wait(dev, index_reg, index);
	val |= word;
	Set_NB32_index_wait(dev, index_reg, index, val);
}

static void SetCKETriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 index;
	u16 word;

	/* Tri-state unused CKEs when motherboard termination is available */

	/* FIXME: skip for Ax */

	dev = pDCTstat->dev_dct;
	word = pDCTstat->CSPresent;

	index  = 0x0c;
	val = Get_NB32_index_wait(dev, index_reg, index);
	if ((word & 0x55) == 0)
		val |= 1 << 12;

	if ((word & 0xAA) == 0)
		val |= 1 << 13;

	Set_NB32_index_wait(dev, index_reg, index, val);
}

static void SetODTTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev;
	u32 index_reg = 0x98 + 0x100 * dct;
	u8 cs;
	u32 index;
	u8 odt;
	u8 max_dimms;

	/* FIXME: skip for Ax */

	dev = pDCTstat->dev_dct;

	/* Tri-state unused ODTs when motherboard termination is available */
	max_dimms = (u8) mctGet_NVbits(NV_MAX_DIMMS);
	odt = 0x0F;	/* ODT tri-state setting */

	if (pDCTstat->Status & (1 <<SB_Registered)) {
		for (cs = 0; cs < 8; cs += 2) {
			if (pDCTstat->CSPresent & (1 << cs)) {
				odt &= ~(1 << (cs / 2));
				if (mctGet_NVbits(NV_4RANKType) != 0) { /* quad-rank capable platform */
					if (pDCTstat->CSPresent & (1 << (cs + 1)))
						odt &= ~(4 << (cs / 2));
				}
			}
		}
	} else {		/* AM3 package */
		val = ~(pDCTstat->CSPresent);
		odt = val & 9;	/* swap bits 1 and 2 */
		if (val & (1 << 1))
			odt |= 1 << 2;
		if (val & (1 << 2))
			odt |= 1 << 1;
	}

	index  = 0x0C;
	val = Get_NB32_index_wait(dev, index_reg, index);
	val |= ((odt & 0xFF) << 8);	/* set bits 11:8 ODTTriState[3:0] */
	Set_NB32_index_wait(dev, index_reg, index, val);

}

static void InitPhyCompensation(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 i;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 val;
	u32 valx = 0;
	u32 dword;
	const u8 *p;

	val = Get_NB32_index_wait(dev, index_reg, 0x00);
	dword = 0;
	for (i=0; i < 6; i++) {
		switch (i) {
			case 0:
			case 4:
				p = Table_Comp_Rise_Slew_15x;
				valx = p[(val >> 16) & 3];
				break;
			case 1:
			case 5:
				p = Table_Comp_Fall_Slew_15x;
				valx = p[(val >> 16) & 3];
				break;
			case 2:
				p = Table_Comp_Rise_Slew_20x;
				valx = p[(val >> 8) & 3];
				break;
			case 3:
				p = Table_Comp_Fall_Slew_20x;
				valx = p[(val >> 8) & 3];
				break;

		}
		dword |= valx << (5 * i);
	}

	/* Override/Exception */
	if (!pDCTstat->GangedMode) {
		i = 0; /* use i for the dct setting required */
		if (pDCTstat->MAdimms[0] < 4)
			i = 1;
		if (((pDCTstat->Speed == 2) || (pDCTstat->Speed == 3)) && (pDCTstat->MAdimms[i] == 4)) {
			dword &= 0xF18FFF18;
			index_reg = 0x98;	/* force dct = 0 */
		}
	}

	Set_NB32_index_wait(dev, index_reg, 0x0a, dword);
}

static void mct_EarlyArbEn_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg;
	u32 val;
	u32 dev = pDCTstat->dev_dct;

	/* GhEnhancement #18429 modified by askar: For low NB CLK :
	 * Memclk ratio, the DCT may need to arbitrate early to avoid
	 * unnecessary bubbles.
	 * bit 19 of F2x[1,0]78 Dram  Control Register, set this bit only when
	 * NB CLK : Memclk ratio is between 3:1 (inclusive) to 4:5 (inclusive)
	 */
	reg = 0x78 + 0x100 * dct;
	val = Get_NB32(dev, reg);

	if (pDCTstat->LogicalCPUID & (AMD_DR_Cx | AMD_DR_Dx))
		val |= (1 << EarlyArbEn);
	else if (CheckNBCOFEarlyArbEn(pMCTstat, pDCTstat))
		val |= (1 << EarlyArbEn);

	Set_NB32(dev, reg, val);
}

static u8 CheckNBCOFEarlyArbEn(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 reg;
	u32 val;
	u32 tmp;
	u32 rem;
	u32 dev = pDCTstat->dev_dct;
	u32  hi, lo;
	u8 NbDid = 0;

	/* Check if NB COF >= 4*Memclk, if it is not, return a fatal error
	 */

	/* 3*(Fn2xD4[NBFid]+4)/(2^NbDid)/(3+Fn2x94[MemClkFreq]) */
	_RDMSR(0xC0010071, &lo, &hi);
	if (lo & (1 << 22))
		NbDid |= 1;

	reg = 0x94;
	val = Get_NB32(dev, reg);
	if (!(val & (1 << MemClkFreqVal)))
		val = Get_NB32(dev, reg + 0x100);	/* get the DCT1 value */

	val &= 0x07;
	val += 3;
	if (NbDid)
		val <<= 1;
	tmp = val;

	dev = pDCTstat->dev_nbmisc;
	reg = 0xD4;
	val = Get_NB32(dev, reg);
	val &= 0x1F;
	val += 3;
	val *= 3;
	val = val / tmp;
	rem = val % tmp;
	tmp >>= 1;

	/* Yes this could be nicer but this was how the asm was.... */
	if (val < 3) {				/* NClk:MemClk < 3:1 */
		return 0;
	} else if (val > 4) {			/* NClk:MemClk >= 5:1 */
		return 0;
	} else if ((val == 4) && (rem > tmp)) { /* NClk:MemClk > 4.5:1 */
		return 0;
	} else {
		return 1;			/* 3:1 <= NClk:MemClk <= 4.5:1*/
	}
}

static void mct_ResetDataStruct_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	u32 i;
	struct DCTStatStruc *pDCTstat;
	u32 start, stop;
	u8 *p;
	u16 host_serv1, host_serv2;

	/* Initialize Data structures by clearing all entries to 0 */
	p = (u8 *) pMCTstat;
	for (i = 0; i < sizeof(struct MCTStatStruc); i++) {
		p[i] = 0;
	}

	for (Node = 0; Node < 8; Node++) {
		pDCTstat = pDCTstatA + Node;
		host_serv1 = pDCTstat->HostBiosSrvc1;
		host_serv2 = pDCTstat->HostBiosSrvc2;

		p = (u8 *) pDCTstat;
		start = 0;
		stop = ((u32) &((struct DCTStatStruc *)0)->CH_MaxRdLat[2]);
		for (i = start; i < stop ; i++) {
			p[i] = 0;
		}

		start = ((u32) &((struct DCTStatStruc *)0)->CH_D_BC_RCVRDLY[2][4]);
		stop = sizeof(struct DCTStatStruc);
		for (i = start; i < stop; i++) {
			p[i] = 0;
		}
		pDCTstat->HostBiosSrvc1 = host_serv1;
		pDCTstat->HostBiosSrvc2 = host_serv2;
	}
}

static void mct_BeforeDramInit_Prod_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u8 i;
	u32 reg_off, dword;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->LogicalCPUID & AMD_DR_Dx) {
		if ((pDCTstat->Speed == 3))
			dword = 0x00000800;
		else
			dword = 0x00000000;
		for (i=0; i < 2; i++) {
			reg_off = 0x100 * i;
			Set_NB32(dev,  0x98 + reg_off, 0x0D000030);
			Set_NB32(dev,  0x9C + reg_off, dword);
			Set_NB32(dev,  0x98 + reg_off, 0x4D040F30);
		}
	}
}

static void mct_EnDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct, val;

	/* Write 0000_07D0h to register F2x[1, 0]98_x4D0FE006 */
	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3)) {
		Set_NB32(dev,  0x9C + reg_off, 0x1C);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE006);
		Set_NB32(dev,  0x9C + reg_off, 0x13D);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE007);

		val = Get_NB32(dev, 0x90 + reg_off);
		val &= ~(1 << 27/* DisDllShutdownSR */);
		Set_NB32(dev, 0x90 + reg_off, val);
	}
}

static u32 mct_DisDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 DramConfigLo, u8 dct)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	/* Write 0000_07D0h to register F2x[1, 0]98_x4D0FE006 */
	if (pDCTstat->LogicalCPUID & (AMD_DR_DAC2_OR_C3)) {
		Set_NB32(dev,  0x9C + reg_off, 0x7D0);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE006);
		Set_NB32(dev,  0x9C + reg_off, 0x190);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE007);

		DramConfigLo |=  /* DisDllShutdownSR */ 1 << 27;
	}

	return DramConfigLo;
}

void mct_SetClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG2;
	_RDMSR(msr, &lo, &hi);
	lo |= 1 << ClLinesToNbDis;
	_WRMSR(msr, lo, hi);
}

void mct_ClrClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{

	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG2;
	_RDMSR(msr, &lo, &hi);
	if (!pDCTstat->ClToNB_flag)
		lo &= ~(1<<ClLinesToNbDis);
	_WRMSR(msr, lo, hi);

}

void mct_SetWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG;
	_RDMSR(msr, &lo, &hi);
	hi |= (1 << WbEnhWsbDis_D);
	_WRMSR(msr, lo, hi);
}

void mct_ClrWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	/* FIXME: Maybe check the CPUID? - not for now. */
	/* pDCTstat->LogicalCPUID; */

	msr = BU_CFG;
	_RDMSR(msr, &lo, &hi);
	hi &= ~(1 << WbEnhWsbDis_D);
	_WRMSR(msr, lo, hi);
}

void ProgDramMRSReg_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 DramMRS, dword;
	u8 byte;

	DramMRS = 0;

	/* Set chip select CKE control mode */
	if (mctGet_NVbits(NV_CKE_CTL)) {
		if (pDCTstat->CSPresent == 3) {
			u16 word;
			word = pDCTstat->DIMMSPDCSE;
			if (dct == 0)
				word &= 0b01010100;
			else
				word &= 0b10101000;
			if (word == 0)
				DramMRS |= 1 << 23;
		}
	}
	/*
	 DRAM MRS Register
	 DrvImpCtrl: drive impedance control.01b(34 ohm driver; Ron34 = Rzq/7)
	*/
	DramMRS |= 1 << 2;
	/* Dram nominal termination: */
	byte = pDCTstat->MAdimms[dct];
	if (!(pDCTstat->Status & (1 << SB_Registered))) {
		DramMRS |= 1 << 7; /* 60 ohms */
		if (byte & 2) {
			if (pDCTstat->Speed < 6)
				DramMRS |= 1 << 8; /* 40 ohms */
			else
				DramMRS |= 1 << 9; /* 30 ohms */
		}
	}
	/* Dram dynamic termination: Disable(1DIMM), 120ohm(>=2DIMM) */
	if (!(pDCTstat->Status & (1 << SB_Registered))) {
		if (byte >= 2) {
			if (pDCTstat->Speed == 7)
				DramMRS |= 1 << 10;
			else
				DramMRS |= 1 << 11;
		}
	} else {
		DramMRS |= mct_DramTermDyn_RDimm(pMCTstat, pDCTstat, byte);
	}

	/* burst length control */
	if (pDCTstat->Status & (1 << SB_128bitmode))
		DramMRS |= 1 << 1;
	/* Qoff=0, output buffers enabled */
	/* Tcwl */
	DramMRS |= (pDCTstat->Speed - 4) << 20;
	/* ASR=1, auto self refresh */
	/* SRT=0 */
	DramMRS |= 1 << 18;

	dword = Get_NB32(pDCTstat->dev_dct, 0x100 * dct + 0x84);
	dword &= ~0x00FC2F8F;
	dword |= DramMRS;
	Set_NB32(pDCTstat->dev_dct, 0x100 * dct + 0x84, dword);
}

void mct_SetDramConfigHi_D(struct DCTStatStruc *pDCTstat, u32 dct,
				u32 DramConfigHi)
{
	/* Bug#15114: Comp. update interrupted by Freq. change can cause
	 * subsequent update to be invalid during any MemClk frequency change:
	 * Solution: From the bug report:
	 *  1. A software-initiated frequency change should be wrapped into the
	 *     following sequence :
	 * 	- a) Disable Compensation (F2[1, 0]9C_x08[30] )
	 * 	b) Reset the Begin Compensation bit (D3CMP->COMP_CONFIG[0]) in all the compensation engines
	 * 	c) Do frequency change
	 * 	d) Enable Compensation (F2[1, 0]9C_x08[30] )
	 *  2. A software-initiated Disable Compensation should always be
	 *     followed by step b) of the above steps.
	 * Silicon Status: Fixed In Rev B0
	 *
	 * Errata#177: DRAM Phy Automatic Compensation Updates May Be Invalid
	 * Solution: BIOS should disable the phy automatic compensation prior
	 * to initiating a memory clock frequency change as follows:
	 *  1. Disable PhyAutoComp by writing 1'b1 to F2x[1, 0]9C_x08[30]
	 *  2. Reset the Begin Compensation bits by writing 32'h0 to
	 *     F2x[1, 0]9C_x4D004F00
	 *  3. Perform frequency change
	 *  4. Enable PhyAutoComp by writing 1'b0 to F2x[1, 0]9C_08[30]
	 *  In addition, any time software disables the automatic phy
	 *   compensation it should reset the begin compensation bit per step 2.
	 *   Silicon Status: Fixed in DR-B0
	 */

	u32 dev = pDCTstat->dev_dct;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 index;

	u32 val;

	index = 0x08;
	val = Get_NB32_index_wait(dev, index_reg, index);
	if (!(val & (1 << DisAutoComp)))
		Set_NB32_index_wait(dev, index_reg, index, val | (1 << DisAutoComp));

	mct_Wait(100);

	Set_NB32(dev, 0x94 + 0x100 * dct, DramConfigHi);
}

static void mct_BeforeDQSTrain_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA)
{
	u8 Node;
	struct DCTStatStruc *pDCTstat;

	/* Errata 178
	 *
	 * Bug#15115: Uncertainty In The Sync Chain Leads To Setup Violations
	 *            In TX FIFO
	 * Solution: BIOS should program DRAM Control Register[RdPtrInit] =
	 *            5h, (F2x[1, 0]78[3:0] = 5h).
	 * Silicon Status: Fixed In Rev B0
	 *
	 * Bug#15880: Determine validity of reset settings for DDR PHY timing.
	 * Solution: At least, set WrDqs fine delay to be 0 for DDR3 training.
	 */
	for (Node = 0; Node < 8; Node++) {
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			mct_BeforeDQSTrainSamp(pDCTstat); /* only Bx */
			mct_ResetDLL_D(pMCTstat, pDCTstat, 0);
			mct_ResetDLL_D(pMCTstat, pDCTstat, 1);
		}
	}
}

static void mct_ResetDLL_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 Receiver;
	u32 dev = pDCTstat->dev_dct;
	u32 reg_off = 0x100 * dct;
	u32 addr;
	u32 lo, hi;
	u8 wrap32dis = 0;
	u8 valid = 0;

	/* Skip reset DLL for B3 */
	if (pDCTstat->LogicalCPUID & AMD_DR_B3) {
		return;
	}

	addr = HWCR;
	_RDMSR(addr, &lo, &hi);
	if(lo & (1<<17)) {		/* save the old value */
		wrap32dis = 1;
	}
	lo |= (1<<17);			/* HWCR.wrap32dis */
	/* Setting wrap32dis allows 64-bit memory references in 32bit mode */
	_WRMSR(addr, lo, hi);

	pDCTstat->Channel = dct;
	Receiver = mct_InitReceiver_D(pDCTstat, dct);
	/* there are four receiver pairs, loosely associated with chipselects.*/
	for (; Receiver < 8; Receiver += 2) {
		if (mct_RcvrRankEnabled_D(pMCTstat, pDCTstat, dct, Receiver)) {
			addr = mct_GetRcvrSysAddr_D(pMCTstat, pDCTstat, dct, Receiver, &valid);
			if (valid) {
				mct_Read1LTestPattern_D(pMCTstat, pDCTstat, addr);	/* cache fills */

				/* Write 0000_8000h to register F2x[1,0]9C_xD080F0C */
				Set_NB32_index_wait(dev, 0x98 + reg_off, 0x4D080F0C, 0x00008000);
				mct_Wait(80); /* wait >= 300ns */

				/* Write 0000_0000h to register F2x[1,0]9C_xD080F0C */
				Set_NB32_index_wait(dev, 0x98 + reg_off, 0x4D080F0C, 0x00000000);
				mct_Wait(800); /* wait >= 2us */
				break;
			}
		}
	}

	if(!wrap32dis) {
		addr = HWCR;
		_RDMSR(addr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(addr, lo, hi);
	}
}

static void mct_EnableDatIntlv_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	/*  Enable F2x110[DctDatIntlv] */
	/* Call back not required mctHookBeforeDatIntlv_D() */
	/* FIXME Skip for Ax */
	if (!pDCTstat->GangedMode) {
		val = Get_NB32(dev, 0x110);
		val |= 1 << 5;			/* DctDatIntlv */
		Set_NB32(dev, 0x110, val);

		/* FIXME Skip for Cx */
		dev = pDCTstat->dev_nbmisc;
		val = Get_NB32(dev, 0x8C);	/* NB Configuration Hi */
		val |= 1 << (36-32);		/* DisDatMask */
		Set_NB32(dev, 0x8C, val);
	}
}

static void SetDllSpeedUp_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev = pDCTstat->dev_dct;
	u32 reg_off = 0x100 * dct;

	if (pDCTstat->Speed >= 7) { /* DDR1600 and above */
		/* Set bit13 PowerDown to register F2x[1, 0]98_x0D080F10 */
		Set_NB32(dev, reg_off + 0x98, 0x0D080F10);
		val = Get_NB32(dev, reg_off + 0x9C);
		val |= 1 < 13;
		Set_NB32(dev, reg_off + 0x9C, val);
		Set_NB32(dev, reg_off + 0x98, 0x4D080F10);

		/* Set bit13 PowerDown to register F2x[1, 0]98_x0D080F11 */
		Set_NB32(dev, reg_off + 0x98, 0x0D080F11);
		val = Get_NB32(dev, reg_off + 0x9C);
		val |= 1 < 13;
		Set_NB32(dev, reg_off + 0x9C, val);
		Set_NB32(dev, reg_off + 0x98, 0x4D080F11);

		/* Set bit13 PowerDown to register F2x[1, 0]98_x0D088F30 */
		Set_NB32(dev, reg_off + 0x98, 0x0D088F30);
		val = Get_NB32(dev, reg_off + 0x9C);
		val |= 1 < 13;
		Set_NB32(dev, reg_off + 0x9C, val);
		Set_NB32(dev, reg_off + 0x98, 0x4D088F30);

		/* Set bit13 PowerDown to register F2x[1, 0]98_x0D08CF30 */
		Set_NB32(dev, reg_off + 0x98, 0x0D08CF30);
		val = Get_NB32(dev, reg_off + 0x9C);
		val |= 1 < 13;
		Set_NB32(dev, reg_off + 0x9C, val);
		Set_NB32(dev, reg_off + 0x98, 0x4D08CF30);

	}
}

static void SyncSetting(struct DCTStatStruc *pDCTstat)
{
	/* set F2x78[ChSetupSync] when F2x[1, 0]9C_x04[AddrCmdSetup, CsOdtSetup,
	 * CkeSetup] setups for one DCT are all 0s and at least one of the setups,
	 * F2x[1, 0]9C_x04[AddrCmdSetup, CsOdtSetup, CkeSetup], of the other
	 * controller is 1
	 */
	u32 cha, chb;
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	cha = pDCTstat->CH_ADDR_TMG[0] & 0x0202020;
	chb = pDCTstat->CH_ADDR_TMG[1] & 0x0202020;

	if ((cha != chb) && ((cha == 0) || (chb == 0))) {
		val = Get_NB32(dev, 0x78);
		val |= 1 << ChSetupSync;
		Set_NB32(dev, 0x78, val);
	}
}

static void AfterDramInit_D(struct DCTStatStruc *pDCTstat, u8 dct) {

	u32 val;
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->LogicalCPUID & (AMD_DR_B2 | AMD_DR_B3)) {
		mct_Wait(10000);	/* Wait 50 us*/
		val = Get_NB32(dev, 0x110);
		if (!(val & (1 << DramEnabled))) {
			/* If 50 us expires while DramEnable =0 then do the following */
			val = Get_NB32(dev, 0x90 + reg_off);
			val &= ~(1 << Width128);		/* Program Width128 = 0 */
			Set_NB32(dev, 0x90 + reg_off, val);

			val = Get_NB32_index_wait(dev, 0x98 + reg_off, 0x05);	/* Perform dummy CSR read to F2x09C_x05 */

			if (pDCTstat->GangedMode) {
				val = Get_NB32(dev, 0x90 + reg_off);
				val |= 1 << Width128;		/* Program Width128 = 0 */
				Set_NB32(dev, 0x90 + reg_off, val);
			}
		}
	}
}

/* ==========================================================
 *  6-bit Bank Addressing Table
 *  RR=rows-13 binary
 *  B=Banks-2 binary
 *  CCC=Columns-9 binary
 * ==========================================================
 *  DCT	CCCBRR	Rows	Banks	Columns	64-bit CS Size
 *  Encoding
 *  0000	000000	13	2	9	128MB
 *  0001	001000	13	2	10	256MB
 *  0010	001001	14	2	10	512MB
 *  0011	010000	13	2	11	512MB
 *  0100	001100	13	3	10	512MB
 *  0101	001101	14	3	10	1GB
 *  0110	010001	14	2	11	1GB
 *  0111	001110	15	3	10	2GB
 *  1000	010101	14	3	11	2GB
 *  1001	010110	15	3	11	4GB
 *  1010	001111	16	3	10	4GB
 *  1011	010111	16	3	11	8GB
 */
u8 crcCheck(u8 smbaddr)
{
	u8 byte_use;
	u8 Index;
	u16 CRC;
	u8 byte, i;

	byte_use = mctRead_SPD(smbaddr, SPD_ByteUse);
	if (byte_use & 0x80)
		byte_use = 117;
	else
		byte_use = 126;

	CRC = 0;
	for (Index = 0; Index < byte_use; Index ++) {
		byte = mctRead_SPD(smbaddr, Index);
		CRC ^= byte << 8;
		for (i=0; i<8; i++) {
			if (CRC & 0x8000) {
				CRC <<= 1;
				CRC ^= 0x1021;
			} else
				CRC <<= 1;
		}
	}
	return CRC == (mctRead_SPD(smbaddr, SPD_byte_127) << 8 | mctRead_SPD(smbaddr, SPD_byte_126));
}
