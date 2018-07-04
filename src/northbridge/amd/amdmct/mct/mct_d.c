/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2017 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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
 */

/* Description: Main memory controller system configuration for DDR 2 */


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
 * Instead Trc (course) is rounded up to nearest 1 ns.
 *
 * Mini and Micro DIMM not supported. Only RDIMM, UDIMM, SO-DIMM defined types
 * supported.
 */

#include "mct_d.h"

#include <string.h>

static u8 ReconfigureDIMMspare_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void DQSTiming_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void LoadDQSSigTmgRegs_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void HTMemMapInit_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void DCTMemClr_Sync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
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
static u8 mct_PlatformSpec(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static void SPDSetBanks_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void StitchMemory_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static u8 Get_DefTrc_k_D(u8 k);
static u16 Get_40Tk_D(u8 k);
static u16 Get_Fk_D(u8 k);
static u8 Dimm_Supports_D(struct DCTStatStruc *pDCTstat, u8 i, u8 j, u8 k);
static u8 Sys_Capability_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, int j, int k);
static u8 Get_DIMMAddress_D(struct DCTStatStruc *pDCTstat, u8 i);
static void mct_initDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat);
static void mct_DramInit(struct MCTStatStruc *pMCTstat,
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
static u8 Check_DqsRcvEn_Diff(struct DCTStatStruc *pDCTstat, u8 dct,
				u32 dev, u32 index_reg, u32 index);
static u8 Get_DqsRcvEnGross_Diff(struct DCTStatStruc *pDCTstat,
					u32 dev, u32 index_reg);
static u8 Get_WrDatGross_Diff(struct DCTStatStruc *pDCTstat, u8 dct,
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
static void SetODTTriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);
static void InitPhyCompensation(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat, u8 dct);
static u32 mct_NodePresent_D(void);
static void WaitRoutine_D(u32 time);
static void mct_OtherTiming(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void mct_ResetDataStruct_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstatA);
static void mct_EarlyArbEn_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat);
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
static u32 mct_DisDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 DramConfigLo, u8 dct);
static void mct_EnDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct);

/*See mctAutoInitMCT header for index relationships to CL and T*/
static const u16 Table_F_k[]	= {00,200,266,333,400,533 };
static const u8 Table_T_k[]	= {0x00,0x50,0x3D,0x30,0x25, 0x18 };
static const u8 Table_CL2_j[]	= {0x04,0x08,0x10,0x20,0x40, 0x80 };
static const u8 Tab_defTrc_k[]	= {0x0,0x41,0x3C,0x3C,0x3A, 0x3A };
static const u16 Tab_40T_k[]	= {00,200,150,120,100,75 };
static const u8 Tab_BankAddr[]	= {0x0,0x08,0x09,0x10,0x0C,0x0D,0x11,0x0E,0x15,0x16,0x0F,0x17};
static const u8 Tab_tCL_j[]	= {0,2,3,4,5};
static const u8 Tab_1KTfawT_k[] = {00,8,10,13,14,20};
static const u8 Tab_2KTfawT_k[] = {00,10,14,17,18,24};
static const u8 Tab_L1CLKDis[]	= {8,8,6,4,2,0,8,8};
static const u8 Tab_M2CLKDis[]	= {2,0,8,8,2,0,2,0};
static const u8 Tab_S1CLKDis[]	= {8,0,8,8,8,0,8,0};
static const u8 Table_Comp_Rise_Slew_20x[] = {7, 3, 2, 2, 0xFF};
static const u8 Table_Comp_Rise_Slew_15x[] = {7, 7, 3, 2, 0xFF};
static const u8 Table_Comp_Fall_Slew_20x[] = {7, 5, 3, 2, 0xFF};
static const u8 Table_Comp_Fall_Slew_15x[] = {7, 7, 5, 3, 0xFF};

const u8 Table_DQSRcvEn_Offset[] = {0x00,0x01,0x10,0x11,0x2};

void mctAutoInitMCT_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstatA)
{
	/*
	 * Memory may be mapped contiguously all the way up to 4GB (depending
	 * on setup options). It is the responsibility of PCI subsystem to
	 * create an uncacheable IO region below 4GB and to adjust TOP_MEM
	 * downward prior to any IO mapping or accesses. It is the same
	 * responsibility of the CPU sub-system prior to accessing LAPIC.
	 *
	 * Slot Number is an external convention, and is determined by OEM with
	 * accompanying silk screening.  OEM may choose to use Slot number
	 * convention which is consistent with DIMM number conventions.
	 * All AMD engineering
	 * platforms do.
	 *
	 * Run-Time Requirements:
	 * 1. Complete Hypertransport Bus Configuration
	 * 2. SMBus Controller Initialized
	 * 3. Checksummed or Valid NVRAM bits
	 * 4. MCG_CTL=-1, MC4_CTL_EN = 0 for all CPUs
	 * 5. MCi_STS from shutdown/warm reset recorded (if desired) prior to
	 *     entry
	 * 6. All var MTRRs reset to zero
	 * 7. State of NB_CFG.DisDatMsk set properly on all CPUs
	 * 8. All CPUs at 2GHz Speed (unless DQS training is not installed).
	 * 9. All cHT links at max Speed/Width (unless DQS training is not
	 *     installed).
	 *
	 *
	 * Global relationship between index values and item values:
	 * j CL(j)       k   F(k)
	 * --------------------------
	 * 0 2.0         -   -
	 * 1 3.0         1   200 MHz
	 * 2 4.0         2   266 MHz
	 * 3 5.0         3   333 MHz
	 * 4 6.0         4   400 MHz
	 * 5 7.0         5   533 MHz
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

		if (mctGet_NVbits(NV_PACK_TYPE) == PT_GR) {
			uint32_t dword;
			pDCTstat->Dual_Node_Package = 1;

			/* Get the internal node number */
			dword = Get_NB32(pDCTstat->dev_nbmisc, 0xe8);
			dword = (dword >> 30) & 0x3;
			pDCTstat->Internal_Node_ID = dword;
		} else {
			pDCTstat->Dual_Node_Package = 0;
		}

		print_tx("mctAutoInitMCT_D: mct_init Node ", Node);
		mct_init(pMCTstat, pDCTstat);
		mctNodeIDDebugPort_D();
		pDCTstat->NodePresent = NodePresent_D(Node);
		if (pDCTstat->NodePresent) {		/* See if Node is there*/
			print_t("mctAutoInitMCT_D: clear_legacy_Mode\n");
			clear_legacy_Mode(pMCTstat, pDCTstat);
			pDCTstat->LogicalCPUID = mctGetLogicalCPUID_D(Node);

			print_t("mctAutoInitMCT_D: mct_InitialMCT_D\n");
			mct_InitialMCT_D(pMCTstat, pDCTstat);

			print_t("mctAutoInitMCT_D: mctSMBhub_Init\n");
			mctSMBhub_Init(Node);		/* Switch SMBUS crossbar to proper node*/

			print_t("mctAutoInitMCT_D: mct_initDCT\n");
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

	print_t("mctAutoInitMCT_D: SyncDCTsReady_D\n");
	SyncDCTsReady_D(pMCTstat, pDCTstatA);	/* Make sure DCTs are ready for accesses.*/

	print_t("mctAutoInitMCT_D: HTMemMapInit_D\n");
	HTMemMapInit_D(pMCTstat, pDCTstatA);	/* Map local memory into system address space.*/
	mctHookAfterHTMap();

	print_t("mctAutoInitMCT_D: CPUMemTyping_D\n");
	CPUMemTyping_D(pMCTstat, pDCTstatA);	/* Map dram into WB/UC CPU cacheability */
	mctHookAfterCPU();			/* Setup external northbridge(s) */

	print_t("mctAutoInitMCT_D: DQSTiming_D\n");
	DQSTiming_D(pMCTstat, pDCTstatA);	/* Get Receiver Enable and DQS signal timing*/

	print_t("mctAutoInitMCT_D: UMAMemTyping_D\n");
	UMAMemTyping_D(pMCTstat, pDCTstatA);	/* Fix up for UMA sizing */

	print_t("mctAutoInitMCT_D: :OtherTiming\n");
	mct_OtherTiming(pMCTstat, pDCTstatA);

	if (ReconfigureDIMMspare_D(pMCTstat, pDCTstatA)) { /* RESET# if 1st pass of DIMM spare enabled*/
		goto restartinit;
	}

	InterleaveNodes_D(pMCTstat, pDCTstatA);
	InterleaveChannels_D(pMCTstat, pDCTstatA);

	print_t("mctAutoInitMCT_D: ECCInit_D\n");
	if (ECCInit_D(pMCTstat, pDCTstatA)) {		/* Setup ECC control and ECC check-bits*/
		print_t("mctAutoInitMCT_D: MCTMemClr_D\n");
		MCTMemClr_D(pMCTstat,pDCTstatA);
	}

	mct_FinalMCT_D(pMCTstat, (pDCTstatA + 0));	// Node 0
	print_tx("mctAutoInitMCT_D Done: Global Status: ", pMCTstat->GStatus);
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

	print_t("DQSTiming_D: mct_BeforeDQSTrain_D:\n");
	mct_BeforeDQSTrain_D(pMCTstat, pDCTstatA);
	phyAssistedMemFnceTraining(pMCTstat, pDCTstatA);

	if (nv_DQSTrainCTL) {
		mctHookBeforeAnyTraining(pMCTstat, pDCTstatA);

		print_t("DQSTiming_D: TrainReceiverEn_D FirstPass:\n");
		TrainReceiverEn_D(pMCTstat, pDCTstatA, FirstPass);

		print_t("DQSTiming_D: mct_TrainDQSPos_D\n");
		mct_TrainDQSPos_D(pMCTstat, pDCTstatA);

		// Second Pass never used for Barcelona!
		//print_t("DQSTiming_D: TrainReceiverEn_D SecondPass:\n");
		//TrainReceiverEn_D(pMCTstat, pDCTstatA, SecondPass);

		print_t("DQSTiming_D: mctSetEccDQSRcvrEn_D\n");
		mctSetEccDQSRcvrEn_D(pMCTstat, pDCTstatA);

		print_t("DQSTiming_D: TrainMaxReadLatency_D\n");
//FIXME - currently uses calculated value		TrainMaxReadLatency_D(pMCTstat, pDCTstatA);
		mctHookAfterAnyTraining();
		mctSaveDQSSigTmg_D();

		print_t("DQSTiming_D: mct_EndDQSTraining_D\n");
		mct_EndDQSTraining_D(pMCTstat, pDCTstatA);

		print_t("DQSTiming_D: MCTMemClr_D\n");
		MCTMemClr_D(pMCTstat, pDCTstatA);
	} else {
		mctGetDQSSigTmg_D();	/* get values into data structure */
		LoadDQSSigTmgRegs_D(pMCTstat, pDCTstatA);	/* load values into registers.*/
		//mctDoWarmResetMemClr_D();
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

				}
			}
			for (Channel = 0; Channel < 2; Channel++) {
				SetEccDQSRcvrEn_D(pDCTstat, Channel);
			}

			for (Channel = 0; Channel < 2; Channel++) {
				u8 *p;
				index_reg = 0x98 + Channel * 0x100;

				/* NOTE:
				 * when 400, 533, 667, it will support dimm0/1/2/3,
				 * and set conf for dimm0, hw will copy to dimm1/2/3
				 * set for dimm1, hw will copy to dimm3
				 * Rev A/B only support DIMM0/1 when 800MHz and above
				 *   + 0x100 to next dimm
				 * Rev C support DIMM0/1/2/3 when 800MHz and above
				 *   + 0x100 to next dimm
				*/
				for (DIMM = 0; DIMM < 2; DIMM++) {
					if (DIMM == 0) {
						index = 0;	/* CHA Write Data Timing Low */
					} else {
						if (pDCTstat->Speed >= 4) {
							index = 0x100 * DIMM;
						} else {
							break;
						}
					}
					for (Dir = 0; Dir < 2; Dir++) {//RD/WR
						p = pDCTstat->persistentData.CH_D_DIR_B_DQS[Channel][DIMM][Dir];
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

			for (Channel = 0; Channel < 2; Channel++) {
				reg = 0x78 + Channel * 0x100;
				val = Get_NB32(dev, reg);
				val &= ~(0x3ff<<22);
				val |= ((u32) pDCTstat->CH_MaxRdLat[Channel] << 22);
				val &= ~(1 << DqsRcvEnTrain);
				Set_NB32(dev, reg, val);	/* program MaxRdLatency to correspond with current delay*/
			}
		}
	}
}

#ifdef UNUSED_CODE
static void ResetNBECCstat_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA);
static void ResetNBECCstat_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	/* Clear MC4_STS for all Nodes in the system.  This is required in some
	 * circumstances to clear left over garbage from cold reset, shutdown,
	 * or normal ECC memory conditioning.
	 */

	//FIXME: this function depends on pDCTstat Array (with Node id) - Is this really a problem?

	u32 dev;
	u8 Node;

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			dev = pDCTstat->dev_nbmisc;
			/*MCA NB Status Low (alias to MC4_STS[31:0] */
			Set_NB32(dev, 0x48, 0);
			/* MCA NB Status High (alias to MC4_STS[63:32] */
			Set_NB32(dev, 0x4C, 0);
		}
	}
}
#endif

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
					pMCTstat->GStatus |= 1 << GSB_SpIntRemapHole;
					pDCTstat->Status |= 1 << SB_SWNodeHole;
					pMCTstat->GStatus |= 1 << GSB_SoftHole;
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

		/* if Node limit > 1GB then set it to 1GB boundary for each node */
		if ((mctSetNodeBoundary_D()) && (limit > 0x00400000)) {
			limit++;
			limit &= 0xFFC00000;
			limit--;
		}
		val = limit & 0xFFFF0000;
		val |= Node;
		Set_NB32(dev, 0x44 + (Node << 3), val);	/* set DstNode */

		limit = pDCTstat->DCTSysLimit;
		if (limit) {
			NextBase = (limit & 0xFFFF0000) + 0x10000;
			if ((mctSetNodeBoundary_D()) && (NextBase > 0x00400000)) {
				NextBase++;
				NextBase &= 0xFFC00000;
				NextBase--;
			}
		}
	}

	/* Copy dram map from Node 0 to Node 1-7 */
	for (Node = 1; Node < MAX_NODES_SUPPORTED; Node++) {
		u32 reg;
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;

		if (pDCTstat->NodePresent) {
			printk(BIOS_DEBUG, " Copy dram map from Node 0 to Node %02x\n", Node);
			reg = 0x40;		/*Dram Base 0*/
			do {
				val = Get_NB32(dev, reg);
				Set_NB32(devx, reg, val);
				reg += 4;
			} while (reg < 0x80);
		} else {
			break;			/* stop at first absent Node */
		}
	}

	/*Copy dram map to F1x120/124*/
	mct_HTMemMapExt(pMCTstat, pDCTstatA);
}


void MCTMemClr_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{

	/* Initiates a memory clear operation for all node. The mem clr
	 * is done in parallel. After the memclr is complete, all processors
	 * status are checked to ensure that memclr has completed.
	 */
	u8 Node;
	struct DCTStatStruc *pDCTstat;

	if (!mctGet_NVbits(NV_DQSTrainCTL)) {
		// FIXME: callback to wrapper: mctDoWarmResetMemClr_D
	} else {	// NV_DQSTrainCTL == 1
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


void DCTMemClr_Init_D(struct MCTStatStruc *pMCTstat,
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


void MCTMemClrSync_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstatA)
{
	/* Ensures that memory clear has completed on all node.*/
	u8 Node;
	struct DCTStatStruc *pDCTstat;

	if (!mctGet_NVbits(NV_DQSTrainCTL)) {
		// callback to wrapper: mctDoWarmResetMemClr_D
	} else {	// NV_DQSTrainCTL == 1
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
	if (pDCTstat->DCTSysLimit) {
		reg = 0x110;

		do {
			val = Get_NB32(dev, reg);
		} while (val & (1 << MemClrBusy));

		do {
			val = Get_NB32(dev, reg);
		} while (!(val & (1 << Dr_MemClrStatus)));
	}

	/* Implement BKDG Rev 3.62 recommendations */
	val = 0x0FE40F80;
	if (!(mctGetLogicalCPUID(0) & AMD_FAM10_LT_D) && mctGet_NVbits(NV_Unganged))
		val |= (0x18 << 2);
	else
		val |= (0x10 << 2);
	val |= MCCH_FlushWrOnStpGnt;	// Set for S3
	Set_NB32(dev, 0x11C, val);
}


u8 NodePresent_D(u8 Node)
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
finish:
		;
	}

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
	if (mct_DIMMPresence(pMCTstat, pDCTstat, dct) < SC_StopError) {
		print_t("\t\tDCTInit_D: mct_DIMMPresence Done\n");
		if (mct_SPDCalcWidth(pMCTstat, pDCTstat, dct) < SC_StopError) {
			print_t("\t\tDCTInit_D: mct_SPDCalcWidth Done\n");
			if (AutoCycTiming_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
				print_t("\t\tDCTInit_D: AutoCycTiming_D Done\n");
				if (AutoConfig_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
					print_t("\t\tDCTInit_D: AutoConfig_D Done\n");
					if (PlatformSpec_D(pMCTstat, pDCTstat, dct) < SC_StopError) {
						print_t("\t\tDCTInit_D: PlatformSpec_D Done\n");
						stopDCTflag = 0;
						if (!(pMCTstat->GStatus & (1 << GSB_EnDIMMSpareNW))) {
							print_t("\t\tDCTInit_D: StartupDCT_D\n");
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
		/*To maximize power savings when DisDramInterface = 1b,
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

	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		struct DCTStatStruc *pDCTstat;
		pDCTstat = pDCTstatA + Node;
		mct_SyncDCTsReady(pDCTstat);
	}
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
	u8 byte;
	u32 reg;
	u32 reg_off = dct * 0x100;

	dev = pDCTstat->dev_dct;
	val = Get_NB32(dev, 0x94 + reg_off);
	if (val & (1 << MemClkFreqVal)) {
		print_t("\t\t\tStartupDCT_D: MemClkFreqVal\n");
		byte = mctGet_NVbits(NV_DQSTrainCTL);
		if (byte == 1) {
			/* Enable DQSRcvEn training mode */
			print_t("\t\t\tStartupDCT_D: DqsRcvEnTrain set\n");
			reg = 0x78 + reg_off;
			val = Get_NB32(dev, reg);
			/* Setting this bit forces a 1T window with hard left
			 * pass/fail edge and a probabilistic right pass/fail
			 * edge.  LEFT edge is referenced for final
			 * receiver enable position.*/
			val |= 1 << DqsRcvEnTrain;
			Set_NB32(dev, reg, val);
		}
		mctHookBeforeDramInit();	/* generalized Hook */
		print_t("\t\t\tStartupDCT_D: DramInit\n");
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

	while (reg < reg_end) {
		Set_NB32(dev, reg, val);
		reg += 4;
	}

	val = 0;
	dev = pDCTstat->dev_map;
	reg = 0xF0;
	Set_NB32(dev, reg, val);
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
	 * The Preset component is subdivided into three items and is the
	 * minimum of the set: Silicon revision, user limit setting when user
	 * timing mode is 'Auto' and memclock mode is 'Limit', OEM build
	 * specification of the maximum frequency. The Preset component is only
	 * applies when user timing mode is 'Auto'.
	 */

	u8 i;
	u8 Twr, Trtp;
	u8 Trp, Trrd, Trcd, Tras, Trc, Trfc[4], Rows;
	u32 DramTimingLo, DramTimingHi;
	u16 Tk10, Tk40;
	u8 Twtr;
	u8 LDIMM;
	u8 DDR2_1066;
	u8 byte;
	u32 dword;
	u32 dev;
	u32 reg;
	u32 reg_off;
	u32 val;
	u16 smbaddr;

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

		/* if "manual" memclock mode */
		if (mctGet_NVbits(NV_MCTUSRTMGMODE) == 2)
			pDCTstat->Speed = mctGet_NVbits(NV_MemCkVal) + 1;

	}
	mct_AfterGetCLT(pMCTstat, pDCTstat, dct);

	/* Gather all DIMM mini-max values for cycle timing data */
	Rows = 0;
	Trp = 0;
	Trrd = 0;
	Trcd = 0;
	Trtp = 0;
	Tras = 0;
	Trc = 0;
	Twr = 0;
	Twtr = 0;
	for (i = 0; i < 4; i++)
		Trfc[i] = 0;

	for (i = 0; i< MAX_DIMMS_SUPPORTED; i++) {
		LDIMM = i >> 1;
		if (pDCTstat->DIMMValid & (1 << i)) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, dct + i);
			byte = mctRead_SPD(smbaddr, SPD_ROWSZ);
			if (Rows < byte)
				Rows = byte;	/* keep track of largest row sz */

			byte = mctRead_SPD(smbaddr, SPD_TRP);
			if (Trp < byte)
				Trp = byte;

			byte = mctRead_SPD(smbaddr, SPD_TRRD);
			if (Trrd < byte)
				Trrd = byte;

			byte = mctRead_SPD(smbaddr, SPD_TRCD);
			if (Trcd < byte)
				Trcd = byte;

			byte = mctRead_SPD(smbaddr, SPD_TRTP);
			if (Trtp < byte)
				Trtp = byte;

			byte = mctRead_SPD(smbaddr, SPD_TWR);
			if (Twr < byte)
				Twr = byte;

			byte = mctRead_SPD(smbaddr, SPD_TWTR);
			if (Twtr < byte)
				Twtr = byte;

			val = mctRead_SPD(smbaddr, SPD_TRC);
			if ((val == 0) || (val == 0xFF)) {
				pDCTstat->ErrStatus |= 1<<SB_NoTrcTrfc;
				pDCTstat->ErrCode = SC_VarianceErr;
				val = Get_DefTrc_k_D(pDCTstat->Speed);
			} else {
				byte = mctRead_SPD(smbaddr, SPD_TRCRFC);
				if (byte & 0xF0) {
					val++;	/* round up in case fractional extension is non-zero.*/
				}
			}
			if (Trc < val)
				Trc = val;

			/* dev density = rank size/#devs per rank */
			byte = mctRead_SPD(smbaddr, SPD_BANKSZ);

			val = ((byte >> 5) | (byte << 3)) & 0xFF;
			val <<= 2;

			byte = mctRead_SPD(smbaddr, SPD_DEVWIDTH) & 0xFE;     /* dev density = 2^(rows+columns+banks) */
			if (byte == 4) {
				val >>= 4;
			} else if (byte == 8) {
				val >>= 3;
			} else if (byte == 16) {
				val >>= 2;
			}

			byte = bsr(val);

			if (Trfc[LDIMM] < byte)
				Trfc[LDIMM] = byte;

			byte = mctRead_SPD(smbaddr, SPD_TRAS);
			if (Tras < byte)
				Tras = byte;
		}	/* Dimm Present */
	}

	/* Convert  DRAM CycleTiming values and store into DCT structure */
	DDR2_1066 = 0;
	byte = pDCTstat->Speed;
	if (byte == 5)
		DDR2_1066 = 1;
	Tk40 = Get_40Tk_D(byte);
	Tk10 = Tk40 >> 2;

	/* Notes:
	 1. All secondary time values given in SPDs are in binary with units of ns.
	 2. Some time values are scaled by four, in order to have least count of 0.25 ns
	    (more accuracy).  JEDEC SPD spec. shows which ones are x1 and x4.
	 3. Internally to this SW, cycle time, Tk, is scaled by 10 to affect a
	    least count of 0.1 ns (more accuracy).
	 4. SPD values not scaled are multiplied by 10 and then divided by 10T to find
	    equivalent minimum number of bus clocks (a remainder causes round-up of clocks).
	 5. SPD values that are prescaled by 4 are multiplied by 10 and then divided by 40T to find
	    equivalent minimum number of bus clocks (a remainder causes round-up of clocks).*/

	/* Tras */
	dword = Tras * 40;
	pDCTstat->DIMMTras = (u16)dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TrasT_1066)
			val = Min_TrasT_1066;
		else if (val > Max_TrasT_1066)
			val = Max_TrasT_1066;
	} else {
		if (val < Min_TrasT)
			val = Min_TrasT;
		else if (val > Max_TrasT)
			val = Max_TrasT;
	}
	pDCTstat->Tras = val;

	/* Trp */
	dword = Trp * 10;
	pDCTstat->DIMMTrp = dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TrasT_1066)
			val = Min_TrpT_1066;
		else if (val > Max_TrpT_1066)
			val = Max_TrpT_1066;
	} else {
		if (val < Min_TrpT)
			val = Min_TrpT;
		else if (val > Max_TrpT)
			val = Max_TrpT;
	}
	pDCTstat->Trp = val;

	/*Trrd*/
	dword = Trrd * 10;
	pDCTstat->DIMMTrrd = dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TrrdT_1066)
			val = Min_TrrdT_1066;
		else if (val > Max_TrrdT_1066)
			val = Max_TrrdT_1066;
	} else {
		if (val < Min_TrrdT)
			val = Min_TrrdT;
		else if (val > Max_TrrdT)
			val = Max_TrrdT;
	}
	pDCTstat->Trrd = val;

	/* Trcd */
	dword = Trcd * 10;
	pDCTstat->DIMMTrcd = dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TrcdT_1066)
			val = Min_TrcdT_1066;
		else if (val > Max_TrcdT_1066)
			val = Max_TrcdT_1066;
	} else {
		if (val < Min_TrcdT)
			val = Min_TrcdT;
		else if (val > Max_TrcdT)
			val = Max_TrcdT;
	}
	pDCTstat->Trcd = val;

	/* Trc */
	dword = Trc * 40;
	pDCTstat->DIMMTrc = dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TrcT_1066)
			val = Min_TrcT_1066;
		else if (val > Max_TrcT_1066)
			val = Max_TrcT_1066;
	} else {
		if (val < Min_TrcT)
			val = Min_TrcT;
		else if (val > Max_TrcT)
			val = Max_TrcT;
	}
	pDCTstat->Trc = val;

	/* Trtp */
	dword = Trtp * 10;
	pDCTstat->DIMMTrtp = dword;
	val = pDCTstat->Speed;
	if (val <= 2) {		/* 7.75ns / Speed in ns to get clock # */
		val = 2;	/* for DDR400/DDR533 */
	} else {		/* Note a speed of 3 will be a Trtp of 3 */
		val = 3;	/* for DDR667/DDR800/DDR1066 */
	}
	pDCTstat->Trtp = val;

	/* Twr */
	dword = Twr * 10;
	pDCTstat->DIMMTwr = dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TwrT_1066)
			val = Min_TwrT_1066;
		else if (val > Max_TwrT_1066)
			val = Max_TwrT_1066;
	} else {
		if (val < Min_TwrT)
			val = Min_TwrT;
		else if (val > Max_TwrT)
			val = Max_TwrT;
	}
	pDCTstat->Twr = val;

	/* Twtr */
	dword = Twtr * 10;
	pDCTstat->DIMMTwtr = dword;
	val = dword / Tk40;
	if (dword % Tk40) {	/* round up number of busclocks */
		val++;
	}
	if (DDR2_1066) {
		if (val < Min_TwrT_1066)
			val = Min_TwtrT_1066;
		else if (val > Max_TwtrT_1066)
			val = Max_TwtrT_1066;
	} else {
		if (val < Min_TwtrT)
			val = Min_TwtrT;
		else if (val > Max_TwtrT)
			val = Max_TwtrT;
	}
	pDCTstat->Twtr = val;


	/* Trfc0-Trfc3 */
	for (i = 0; i < 4; i++)
		pDCTstat->Trfc[i] = Trfc[i];

	mctAdjustAutoCycTmg_D();

	/* Program DRAM Timing values */
	DramTimingLo = 0;	/* Dram Timing Low init */
	val = pDCTstat->CASL;
	val = Tab_tCL_j[val];
	DramTimingLo |= val;

	val = pDCTstat->Trcd;
	if (DDR2_1066)
		val -= Bias_TrcdT_1066;
	else
		val -= Bias_TrcdT;

	DramTimingLo |= val << 4;

	val = pDCTstat->Trp;
	if (DDR2_1066)
		val -= Bias_TrpT_1066;
	else {
		val -= Bias_TrpT;
		val <<= 1;
	}
	DramTimingLo |= val << 7;

	val = pDCTstat->Trtp;
	val -= Bias_TrtpT;
	DramTimingLo |= val << 11;

	val = pDCTstat->Tras;
	if (DDR2_1066)
		val -= Bias_TrasT_1066;
	else
		val -= Bias_TrasT;
	DramTimingLo |= val << 12;

	val = pDCTstat->Trc;
	val -= Bias_TrcT;
	DramTimingLo |= val << 16;

	if (!DDR2_1066) {
		val = pDCTstat->Twr;
		val -= Bias_TwrT;
		DramTimingLo |= val << 20;
	}

	val = pDCTstat->Trrd;
	if (DDR2_1066)
		val -= Bias_TrrdT_1066;
	else
		val -= Bias_TrrdT;
	DramTimingLo |= val << 22;


	DramTimingHi = 0;	/* Dram Timing Low init */
	val = pDCTstat->Twtr;
	if (DDR2_1066)
		val -= Bias_TwtrT_1066;
	else
		val -= Bias_TwtrT;
	DramTimingHi |= val << 8;

	val = 2;
	DramTimingHi |= val << 16;

	val = 0;
	for (i = 4; i > 0; i--) {
		val <<= 3;
		val |= Trfc[i-1];
	}
	DramTimingHi |= val << 20;


	dev = pDCTstat->dev_dct;
	reg_off = 0x100 * dct;
	print_tx("AutoCycTiming: DramTimingLo ", DramTimingLo);
	print_tx("AutoCycTiming: DramTimingHi ", DramTimingHi);

	Set_NB32(dev, 0x88 + reg_off, DramTimingLo);	/*DCT Timing Low*/
	DramTimingHi |=0x0000FC77;
	Set_NB32(dev, 0x8c + reg_off, DramTimingHi);	/*DCT Timing Hi*/

	if (DDR2_1066) {
		/* Twr */
		dword = pDCTstat->Twr;
		dword -= Bias_TwrT_1066;
		dword <<= 4;
		reg = 0x84 + reg_off;
		val = Get_NB32(dev, reg);
		val &= 0x8F;
		val |= dword;
		Set_NB32(dev, reg, val);
	}

	print_tx("AutoCycTiming: Status ", pDCTstat->Status);
	print_tx("AutoCycTiming: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("AutoCycTiming: ErrCode ", pDCTstat->ErrCode);
	print_t("AutoCycTiming: Done\n");

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

	u16 proposedFreq;
	u16 word;

	/* Get CPU Si Revision defined limit (NPT) */
	proposedFreq = 533;	 /* Rev F0 programmable max memclock is */

	/*Get User defined limit if  "limit" mode */
	if (mctGet_NVbits(NV_MCTUSRTMGMODE) == 1) {
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
	int i, j, k;
	u8 T1min, CL1min;

	/* i={0..7} (std. physical DIMM number)
	 * j is an integer which enumerates increasing CAS latency.
	 * k is an integer which enumerates decreasing cycle time.
	 * CL no. {0,1,2} corresponds to CL X, CL X-.5, or CL X-1 (per individual DIMM)
	 * Max timing values are per parameter, of all DIMMs, spec'd in ns like the SPD.
	 */

	CL1min = 0xFF;
	T1min = 0xFF;
	for (k = K_MAX; k >= K_MIN; k--) {
		for (j = J_MIN; j <= J_MAX; j++) {
			if (Sys_Capability_D(pMCTstat, pDCTstat, j, k)) {
				/* 1. check to see if DIMMi is populated.
				   2. check if DIMMi supports CLj and Tjk */
				for (i = 0; i < MAX_DIMMS_SUPPORTED; i++) {
					if (pDCTstat->DIMMValid & (1 << i)) {
						if (Dimm_Supports_D(pDCTstat, i, j, k))
							break;
					}
				}	/* while ++i */
				if (i == MAX_DIMMS_SUPPORTED) {
					T1min = k;
					CL1min = j;
					goto got_TCL;
				}
			}
		}	/* while ++j */
	}	/* while --k */

got_TCL:
	if (T1min != 0xFF) {
		pDCTstat->DIMMCASL = CL1min;	/*mfg. optimized */
		pDCTstat->DIMMAutoSpeed = T1min;
		print_tx("SPDGetTCL_D: DIMMCASL ", pDCTstat->DIMMCASL);
		print_tx("SPDGetTCL_D: DIMMAutoSpeed ", pDCTstat->DIMMAutoSpeed);

	} else {
		pDCTstat->DIMMCASL = CL_DEF;	/* failsafe values (running in min. mode) */
		pDCTstat->DIMMAutoSpeed = T_DEF;
		pDCTstat->ErrStatus |= 1 << SB_DimmMismatchT;
		pDCTstat->ErrStatus |= 1 << SB_MinimumMode;
		pDCTstat->ErrCode = SC_VarianceErr;
	}
	print_tx("SPDGetTCL_D: Status ", pDCTstat->Status);
	print_tx("SPDGetTCL_D: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("SPDGetTCL_D: ErrCode ", pDCTstat->ErrCode);
	print_t("SPDGetTCL_D: Done\n");
}


static u8 PlatformSpec_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 dev;
	u32 reg;
	u32 val;

	mctGet_PS_Cfg_D(pMCTstat, pDCTstat, dct);

	if (pDCTstat->GangedMode) {
		mctGet_PS_Cfg_D(pMCTstat, pDCTstat, 1);
	}

	if (pDCTstat->_2Tmode == 2) {
		dev = pDCTstat->dev_dct;
		reg = 0x94 + 0x100 * dct; /* Dram Configuration Hi */
		val = Get_NB32(dev, reg);
		val |= 1 << 20;		       /* 2T CMD mode */
		Set_NB32(dev, reg, val);
	}

	mct_PlatformSpec(pMCTstat, pDCTstat, dct);
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

	print_tx("AutoConfig_D: DCT: ", dct);

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

	if (mctGet_NVbits(NV_CLKHZAltVidC3))
		DramControl |= 1<<16;

	// FIXME: Add support(skip) for Ax and Cx versions
	DramControl |= 5;	/* RdPtrInit */


	/* Build Dram Config Lo Register Value */
	DramConfigLo |= 1 << 4;					/* 75 Ohms ODT */
	if (mctGet_NVbits(NV_MAX_DIMMS) == 8) {
		if (pDCTstat->Speed == 3) {
			if (pDCTstat->MAdimms[dct] == 4)
				DramConfigLo |= 1 << 5;		/* 50 Ohms ODT */
		} else if (pDCTstat->Speed == 4) {
			if (pDCTstat->MAdimms[dct] != 1)
				DramConfigLo |= 1 << 5;		/* 50 Ohms ODT */
		}
	} else {
		// FIXME: Skip for Ax versions
		if (pDCTstat->MAdimms[dct] == 4) {
			if (pDCTstat->DimmQRPresent != 0) {
				if ((pDCTstat->Speed == 3) || (pDCTstat->Speed == 4)) {
					DramConfigLo |= 1 << 5;	/* 50 Ohms ODT */
				}
			} else if (pDCTstat->MAdimms[dct] == 4) {
				if (pDCTstat->Speed == 4) {
					if (pDCTstat->DimmQRPresent != 0) {
						DramConfigLo |= 1 << 5;	/* 50 Ohms ODT */
					}
				}
			}
		} else if (pDCTstat->MAdimms[dct] == 2) {
			DramConfigLo |= 1 << 5;		/* 50 Ohms ODT */
		}

	}

	// FIXME: Skip for Ax versions
	/* callback not required - if (!mctParityControl_D()) */
	if (Status & (1 << SB_PARDIMMs)) {
		DramConfigLo |= 1 << ParEn;
		DramConfigMisc2 |= 1 << ActiveCmdAtRst;
	} else {
		DramConfigLo  &= ~(1 << ParEn);
		DramConfigMisc2 &= ~(1 << ActiveCmdAtRst);
	}

	if (mctGet_NVbits(NV_BurstLen32)) {
		if (!pDCTstat->GangedMode)
			DramConfigLo |= 1 << BurstLength32;
	}

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
			if (mctGet_NVbits(NV_ECC))
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

	if (mctGet_NVbits(NV_CKE_PDEN)) {
		DramConfigHi |= 1 << 15;		/* PowerDownEn */
		if (mctGet_NVbits(NV_CKE_CTL))
			/*Chip Select control of CKE*/
			DramConfigHi |= 1 << 16;
	}

	/* Control Bank Swizzle */
	if (0) /* call back not needed mctBankSwizzleControl_D()) */
		DramConfigHi &= ~(1 << BankSwizzleMode);
	else
		DramConfigHi |= 1 << BankSwizzleMode; /* recommended setting (default) */

	/* Check for Quadrank DIMM presence */
	if (pDCTstat->DimmQRPresent != 0) {
		byte = mctGet_NVbits(NV_4RANKType);
		if (byte == 2)
			DramConfigHi |= 1 << 17;	/* S4 (4-Rank SO-DIMMs) */
		else if (byte == 1)
			DramConfigHi |= 1 << 18;	/* R4 (4-Rank Registered DIMMs) */
	}

	if (0) /* call back not needed mctOverrideDcqBypMax_D) */
		val = mctGet_NVbits(NV_BYPMAX);
	else
		val = 0x0f; // recommended setting (default)
	DramConfigHi |= val << 24;

	val = pDCTstat->DIMM2Kpage;
	if (pDCTstat->GangedMode != 0) {
		if (dct != 0) {
			val &= 0x55;
		} else {
			val &= 0xAA;
		}
	}
	if (val)
		val = Tab_2KTfawT_k[pDCTstat->Speed];
	else
		val = Tab_1KTfawT_k[pDCTstat->Speed];

	if (pDCTstat->Speed == 5)
		val >>= 1;

	val -= Bias_TfawT;
	val <<= 28;
	DramConfigHi |= val;	/* Tfaw for 1K or 2K paged drams */

	// FIXME: Skip for Ax versions
	DramConfigHi |= 1 << DcqArbBypassEn;


	/* Build MemClkDis Value from Dram Timing Lo and
	   Dram Config Misc Registers
	 1. We will assume that MemClkDis field has been preset prior to this
	    point.
	 2. We will only set MemClkDis bits if a DIMM is NOT present AND if:
	    NV_AllMemClks <>0 AND SB_DiagClks == 0 */


	/* Dram Timing Low (owns Clock Enable bits) */
	DramTimingLo = Get_NB32(dev, 0x88 + reg_off);
	if (mctGet_NVbits(NV_AllMemClks) == 0) {
		/* Special Jedec SPD diagnostic bit - "enable all clocks" */
		if (!(pDCTstat->Status & (1 << SB_DiagClks))) {
			const u8 *p;
			byte = mctGet_NVbits(NV_PACK_TYPE);
			if (byte == PT_L1)
				p = Tab_L1CLKDis;
			else if (byte == PT_M2)
				p = Tab_M2CLKDis;
			else
				p = Tab_S1CLKDis;

			dword = 0;
			while (dword < MAX_DIMMS_SUPPORTED) {
				val = p[dword];
				print_tx("DramTimingLo: val=", val);
				if (!(pDCTstat->DIMMValid & (1 << val)))
					/*disable memclk*/
					DramTimingLo |= 1 << (dword+24);
				dword++;
			}
		}
	}

	print_tx("AutoConfig_D: DramControl: ", DramControl);
	print_tx("AutoConfig_D: DramTimingLo: ", DramTimingLo);
	print_tx("AutoConfig_D: DramConfigMisc: ", DramConfigMisc);
	print_tx("AutoConfig_D: DramConfigMisc2: ", DramConfigMisc2);
	print_tx("AutoConfig_D: DramConfigLo: ", DramConfigLo);
	print_tx("AutoConfig_D: DramConfigHi: ", DramConfigHi);

	/* Write Values to the registers */
	Set_NB32(dev, 0x78 + reg_off, DramControl);
	Set_NB32(dev, 0x88 + reg_off, DramTimingLo);
	Set_NB32(dev, 0xA0 + reg_off, DramConfigMisc);
	Set_NB32(dev, 0xA8 + reg_off, DramConfigMisc2);
	Set_NB32(dev, 0x90 + reg_off, DramConfigLo);
	mct_SetDramConfigHi_D(pDCTstat, dct, DramConfigHi);
	mct_ForceAutoPrecharge_D(pDCTstat, dct);
	mct_EarlyArbEn_D(pMCTstat, pDCTstat);
	mctHookAfterAutoCfg();

	print_tx("AutoConfig: Status ", pDCTstat->Status);
	print_tx("AutoConfig: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("AutoConfig: ErrCode ", pDCTstat->ErrCode);
	print_t("AutoConfig: Done\n");
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

	u8 ChipSel, Rows, Cols, Ranks ,Banks, DevWidth;
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

		if (pDCTstat->DIMMValid & (1 << byte)) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, (ChipSel + dct));

			byte = mctRead_SPD(smbaddr, SPD_ROWSZ);
			Rows = byte & 0x1f;

			byte = mctRead_SPD(smbaddr, SPD_COLSZ);
			Cols = byte & 0x1f;

			Banks = mctRead_SPD(smbaddr, SPD_LBANKS);

			byte = mctRead_SPD(smbaddr, SPD_DEVWIDTH);
			DevWidth = byte & 0x7f; /* bits 0-6 = bank 0 width */

			byte = mctRead_SPD(smbaddr, SPD_DMBANKS);
			Ranks = (byte & 7) + 1;

			/* Configure Bank encoding
			 * Use a 6-bit key into a lookup table.
			 * Key (index) = CCCBRR, where CCC is the number of
			 * Columns minus 9,RR is the number of Rows minus 13,
			 * and B is the number of banks minus 2.
			 * See "6-bit Bank Addressing Table" at the end of
			 * this file.*/
			byte = Cols - 9;	/* 9 Cols is smallest dev size */
			byte <<= 3;		/* make room for row and bank bits*/
			if (Banks == 8)
				byte |= 4;

			/* 13 Rows is smallest dev size */
			byte |= Rows - 13;	/* CCCBRR internal encode */

			for (dword = 0; dword < 12; dword++) {
				if (byte == Tab_BankAddr[dword])
					break;
			}

			if (dword < 12) {

				/* bit no. of CS field in address mapping reg.*/
				dword <<= (ChipSel << 1);
				BankAddrReg |= dword;

				/* Mask value=(2pow(rows+cols+banks+3)-1)>>8,
				   or 2pow(rows+cols+banks-5)-1*/
				csMask = 0;

				byte = Rows + Cols;		/* cl = rows+cols*/
				if (Banks == 8)
					byte -= 2;		/* 3 banks - 5 */
				else
					byte -= 3;		/* 2 banks - 5 */
								/* mask size (64-bit rank only) */

				if (pDCTstat->Status & (1 << SB_128bitmode))
					byte++;		/* double mask size if in 128-bit mode*/

				csMask |= 1 << byte;
				csMask--;

				/*set ChipSelect population indicator even bits*/
				pDCTstat->CSPresent |= (1 << ChipSel);
				if (Ranks >= 2)
					/*set ChipSelect population indicator odd bits*/
					pDCTstat->CSPresent |= 1 << (ChipSel + 1);

				reg = 0x60+(ChipSel << 1) + reg_off;	/*Dram CS Mask Register */
				val = csMask;
				val &= 0x1FF83FE0;	/* Mask out reserved bits.*/
				Set_NB32(dev, reg, val);
			}
		} else {
			if (pDCTstat->DIMMSPDCSE & (1 << ChipSel))
				pDCTstat->CSTestFail |= (1 << ChipSel);
		}	/* if DIMMValid*/
	}	/* while ChipSel*/

	SetCSTriState(pMCTstat, pDCTstat, dct);
	/* SetCKETriState */
	SetODTTriState(pMCTstat, pDCTstat, dct);

	if (pDCTstat->Status & (1 << SB_128bitmode)) {
		SetCSTriState(pMCTstat, pDCTstat, 1); /* force dct1) */
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

	print_tx("SPDSetBanks: Status ", pDCTstat->Status);
	print_tx("SPDSetBanks: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("SPDSetBanks: ErrCode ", pDCTstat->ErrCode);
	print_t("SPDSetBanks: Done\n");
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
	for (i = 0; i < MAX_DIMMS_SUPPORTED; i += 2) {
		if ((pDCTstat->DIMMValid & (1 << i)) && (pDCTstat->DIMMValid & (1<<(i+1)))) {
			smbaddr = Get_DIMMAddress_D(pDCTstat, i);
			smbaddr1 = Get_DIMMAddress_D(pDCTstat, i+1);

			byte = mctRead_SPD(smbaddr, SPD_ROWSZ) & 0x1f;
			byte1 = mctRead_SPD(smbaddr1, SPD_ROWSZ) & 0x1f;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO);
				break;
			}

			byte =	 mctRead_SPD(smbaddr, SPD_COLSZ) & 0x1f;
			byte1 =	 mctRead_SPD(smbaddr1, SPD_COLSZ) & 0x1f;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO);
				break;
			}

			byte = mctRead_SPD(smbaddr, SPD_BANKSZ);
			byte1 = mctRead_SPD(smbaddr1, SPD_BANKSZ);
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO);
				break;
			}

			byte = mctRead_SPD(smbaddr, SPD_DEVWIDTH) & 0x7f;
			byte1 = mctRead_SPD(smbaddr1, SPD_DEVWIDTH) & 0x7f;
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO);
				break;
			}

			byte = mctRead_SPD(smbaddr, SPD_DMBANKS) & 7;	 /* #ranks-1 */
			byte1 = mctRead_SPD(smbaddr1, SPD_DMBANKS) & 7;	  /* #ranks-1 */
			if (byte != byte1) {
				pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO);
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

	/* CS Sparing 1 = enabled, 0 = disabled */
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
			if (!mctGet_NVbits(NV_DQSTrainCTL)) { /*DQS Training 1 = enabled, 0 = disabled */
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
	for (p = 0; p < MAX_DIMMS_SUPPORTED; p++) {
		BiggestBank = 0;
		for (q = 0; q < MAX_CS_SUPPORTED; q++) { /* from DIMMS to CS */
			if (pDCTstat->CSPresent & (1 << q)) {  /* bank present? */
				reg  = 0x40 + (q << 2) + reg_off;  /* Base[q] reg.*/
				val = Get_NB32(dev, reg);
				if (!(val & 3)) {	/* (CSEnable|Spare == 1)bank is enabled already? */
					reg = 0x60 + ((q << 1) & 0xc) + reg_off; /*Mask[q] reg.*/
					val = Get_NB32(dev, reg);
					val >>= 19;
					val++;
					val <<= 19;
					Sizeq = val;  //never used
					if (val > BiggestBank) {
						/*Bingo! possibly Map this chip-select next! */
						BiggestBank = val;
						b = q;
					}
				}
			}	/*if bank present */
		}	/* while q */
		if (BiggestBank !=0) {
			curcsBase = nxtcsBase;		/* curcsBase = nxtcsBase*/
			/* DRAM CS Base b Address Register offset */
			reg = 0x40 + (b << 2) + reg_off;
			if (_DSpareEn) {
				BiggestBank = 0;
				val = 1 << Spare;	/* Spare Enable*/
			} else {
				val = curcsBase;
				val |= 1 << CSEnable;	/* Bank Enable */
			}
			Set_NB32(dev, reg, val);
			if (_DSpareEn)
				_DSpareEn = 0;
			else
				/* let nxtcsBase+=Size[b] */
				nxtcsBase += BiggestBank;
		}

		/* bank present but disabled?*/
		if (pDCTstat->CSTestFail & (1 << p)) {
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

	print_tx("StitchMemory: Status ", pDCTstat->Status);
	print_tx("StitchMemory: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("StitchMemory: ErrCode ", pDCTstat->ErrCode);
	print_t("StitchMemory: Done\n");
}


static u8 Get_Tk_D(u8 k)
{
	return Table_T_k[k];
}


static u8 Get_CLj_D(u8 j)
{
	return Table_CL2_j[j];
}

static u8 Get_DefTrc_k_D(u8 k)
{
	return Tab_defTrc_k[k];
}


static u16 Get_40Tk_D(u8 k)
{
	return Tab_40T_k[k]; /* FIXME: k or k<<1 ?*/
}


static u16 Get_Fk_D(u8 k)
{
	return Table_F_k[k]; /* FIXME: k or k<<1 ? */
}


static u8 Dimm_Supports_D(struct DCTStatStruc *pDCTstat,
				u8 i, u8 j, u8 k)
{
	u8 Tk, CLj, CL_i;
	u8 ret = 0;

	u32 DIMMi;
	u8 byte;
	u16 word, wordx;

	DIMMi = Get_DIMMAddress_D(pDCTstat, i);

	CLj = Get_CLj_D(j);

	/* check if DIMMi supports CLj */
	CL_i = mctRead_SPD(DIMMi, SPD_CASLAT);
	byte = CL_i & CLj;
	if (byte) {
		/*find out if its CL X, CLX-1, or CLX-2 */
		word = bsr(byte);	/* bit position of CLj */
		wordx = bsr(CL_i);	/* bit position of CLX of CLi */
		wordx -= word;		/* CL number (CL no. = 0,1, 2, or 3) */
		wordx <<= 3;		/* 8 bits per SPD byte index */
		/*get T from SPD byte 9, 23, 25*/
		word = (EncodedTSPD >> wordx) & 0xFF;
		Tk = Get_Tk_D(k);
		byte = mctRead_SPD(DIMMi, word);	/* DIMMi speed */
		if (Tk < byte) {
			ret = 1;
		} else if (byte == 0) {
			pDCTstat->ErrStatus |= 1 << SB_NoCycTime;
			ret = 1;
		} else {
			ret = 0;	/* DIMM is capable! */
		}
	} else {
		ret = 1;
	}
	return ret;
}


static u8 DIMMPresence_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	/* Check DIMMs present, verify checksum, flag SDRAM type,
	 * build population indicator bitmaps, and preload bus loading
	 * of DIMMs into DCTStatStruc.
	 * MAAload = number of devices on the "A" bus.
	 * MABload = number of devices on the "B" bus.
	 * MAAdimms = number of DIMMs on the "A" bus slots.
	 * MABdimms = number of DIMMs on the "B" bus slots.
	 * DATAAload = number of ranks on the "A" bus slots.
	 * DATABload = number of ranks on the "B" bus slots.
	 */

	u16 i, j, k;
	u8 smbaddr, Index;
	u16 Checksum;
	u8 SPDCtrl;
	u16 RegDIMMPresent, MaxDimms;
	u8 devwidth;
	u16 DimmSlots;
	u8 byte = 0, bytex;
	u16 word;

	/* preload data structure with addrs */
	mctGet_DIMMAddr(pDCTstat, pDCTstat->Node_ID);

	DimmSlots = MaxDimms = mctGet_NVbits(NV_MAX_DIMMS);

	SPDCtrl = mctGet_NVbits(NV_SPDCHK_RESTRT);

	RegDIMMPresent = 0;
	pDCTstat->DimmQRPresent = 0;

	for (i = 0;  i< MAX_DIMMS_SUPPORTED; i++) {
		if (i >= MaxDimms)
			break;

		if ((pDCTstat->DimmQRPresent & (1 << i)) || (i < DimmSlots)) {
			print_tx("\t DIMMPresence: i=", i);
			smbaddr = Get_DIMMAddress_D(pDCTstat, i);
			print_tx("\t DIMMPresence: smbaddr=", smbaddr);
			if (smbaddr) {
				Checksum = 0;
				for (Index = 0; Index < 64; Index++) {
					int status;
					status = mctRead_SPD(smbaddr, Index);
					if (status < 0)
						break;
					byte = status & 0xFF;
					if (Index < 63)
						Checksum += byte;
				}

				if (Index == 64) {
					pDCTstat->DIMMPresent |= 1 << i;
					if ((Checksum & 0xFF) == byte) {
						byte = mctRead_SPD(smbaddr, SPD_TYPE);
						if (byte == JED_DDR2SDRAM) {
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
							pDCTstat->ErrStatus |= 1 << SB_DIMMChkSum;
							byte = mctRead_SPD(smbaddr, SPD_TYPE);
							if (byte == JED_DDR2SDRAM)
								pDCTstat->DIMMValid |= 1 << i;
						}
					}
					/* Get module information for SMBIOS */
					if (pDCTstat->DIMMValid & (1 << i)) {
						pDCTstat->DimmManufacturerID[i] = 0;
						for (k = 0; k < 8; k++)
							pDCTstat->DimmManufacturerID[i] |= ((uint64_t)mctRead_SPD(smbaddr, SPD_MANID_START + k)) << (k * 8);
						for (k = 0; k < SPD_PARTN_LENGTH; k++)
							pDCTstat->DimmPartNumber[i][k] = mctRead_SPD(smbaddr, SPD_PARTN_START + k);
						pDCTstat->DimmPartNumber[i][SPD_PARTN_LENGTH] = 0;
						pDCTstat->DimmRevisionNumber[i] = 0;
						for (k = 0; k < 2; k++)
							pDCTstat->DimmRevisionNumber[i] |= ((uint16_t)mctRead_SPD(smbaddr, SPD_REVNO_START + k)) << (k * 8);
						pDCTstat->DimmSerialNumber[i] = 0;
						for (k = 0; k < 4; k++)
							pDCTstat->DimmSerialNumber[i] |= ((uint32_t)mctRead_SPD(smbaddr, SPD_SERIAL_START + k)) << (k * 8);
						pDCTstat->DimmRows[i] = mctRead_SPD(smbaddr, SPD_ROWSZ) & 0xf;
						pDCTstat->DimmCols[i] = mctRead_SPD(smbaddr, SPD_COLSZ) & 0xf;
						pDCTstat->DimmRanks[i] = (mctRead_SPD(smbaddr, SPD_DMBANKS) & 0x7) + 1;
						pDCTstat->DimmBanks[i] = mctRead_SPD(smbaddr, SPD_LBANKS);
						pDCTstat->DimmWidth[i] = mctRead_SPD(smbaddr, SPD_DEVWIDTH);
					}
					/* Check module type */
					byte = mctRead_SPD(smbaddr, SPD_DIMMTYPE);
					if (byte & JED_REGADCMSK) {
						RegDIMMPresent |= 1 << i;
						pDCTstat->DimmRegistered[i] = 1;
					} else {
						pDCTstat->DimmRegistered[i] = 0;
					}
					/* Check ECC capable */
					byte = mctRead_SPD(smbaddr, SPD_EDCTYPE);
					if (byte & JED_ECC) {
						/* DIMM is ECC capable */
						pDCTstat->DimmECCPresent |= 1 << i;
					}
					if (byte & JED_ADRCPAR) {
						/* DIMM is ECC capable */
						pDCTstat->DimmPARPresent |= 1 << i;
					}
					/* Check if x4 device */
					devwidth = mctRead_SPD(smbaddr, SPD_DEVWIDTH) & 0xFE;
					if (devwidth == 4) {
						/* DIMM is made with x4 or x16 drams */
						pDCTstat->Dimmx4Present |= 1 << i;
					} else if (devwidth == 8) {
						pDCTstat->Dimmx8Present |= 1 << i;
					} else if (devwidth == 16) {
						pDCTstat->Dimmx16Present |= 1 << i;
					}
					/* check page size */
					byte = mctRead_SPD(smbaddr, SPD_COLSZ);
					byte &= 0x0F;
					word = 1 << byte;
					word >>= 3;
					word *= devwidth;	/* (((2^COLBITS) / 8) * ORG) / 2048 */
					word >>= 11;
					if (word)
						pDCTstat->DIMM2Kpage |= 1 << i;

					/*Check if SPD diag bit 'analysis probe installed' is set */
					byte = mctRead_SPD(smbaddr, SPD_ATTRIB);
					if (byte & JED_PROBEMSK)
						pDCTstat->Status |= 1 << SB_DiagClks;

					byte = mctRead_SPD(smbaddr, SPD_DMBANKS);
					if (!(byte & (1 << SPDPLBit)))
						pDCTstat->DimmPlPresent |= 1 << i;
					byte &= 7;
					byte++;		 /* ranks */
					if (byte > 2) {
						/* if any DIMMs are QR, we have to make two passes through DIMMs*/
						if (pDCTstat->DimmQRPresent == 0) {
							MaxDimms <<= 1;
						}
						if (i < DimmSlots) {
							pDCTstat->DimmQRPresent |= (1 << i) | (1 << (i+4));
						}
						byte = 2;	/* upper two ranks of QR DIMM will be counted on another DIMM number iteration*/
					} else if (byte == 2) {
						pDCTstat->DimmDRPresent |= 1 << i;
					}
					bytex = devwidth;
					if (devwidth == 16)
						bytex = 4;
					else if (devwidth == 4)
						bytex = 16;

					if (byte == 2)
						bytex <<= 1;	/*double Addr bus load value for dual rank DIMMs*/

					j = i & (1<<0);
					pDCTstat->DATAload[j] += byte;	/*number of ranks on DATA bus*/
					pDCTstat->MAload[j] += bytex;	/*number of devices on CMD/ADDR bus*/
					pDCTstat->MAdimms[j]++;		/*number of DIMMs on A bus */
					/*check for DRAM package Year <= 06*/
					byte = mctRead_SPD(smbaddr, SPD_MANDATEYR);
					if (byte < MYEAR06) {
						/*Year < 06 and hence Week < 24 of 06 */
						pDCTstat->DimmYr06 |= 1 << i;
						pDCTstat->DimmWk2406 |= 1 << i;
					} else if (byte == MYEAR06) {
						/*Year = 06, check if Week <= 24 */
						pDCTstat->DimmYr06 |= 1 << i;
						byte = mctRead_SPD(smbaddr, SPD_MANDATEWK);
						if (byte <= MWEEK24)
							pDCTstat->DimmWk2406 |= 1 << i;
					}
				}
			}
		}
	}
	print_tx("\t DIMMPresence: DIMMValid=", pDCTstat->DIMMValid);
	print_tx("\t DIMMPresence: DIMMPresent=", pDCTstat->DIMMPresent);
	print_tx("\t DIMMPresence: RegDIMMPresent=", RegDIMMPresent);
	print_tx("\t DIMMPresence: DimmECCPresent=", pDCTstat->DimmECCPresent);
	print_tx("\t DIMMPresence: DimmPARPresent=", pDCTstat->DimmPARPresent);
	print_tx("\t DIMMPresence: Dimmx4Present=", pDCTstat->Dimmx4Present);
	print_tx("\t DIMMPresence: Dimmx8Present=", pDCTstat->Dimmx8Present);
	print_tx("\t DIMMPresence: Dimmx16Present=", pDCTstat->Dimmx16Present);
	print_tx("\t DIMMPresence: DimmPlPresent=", pDCTstat->DimmPlPresent);
	print_tx("\t DIMMPresence: DimmDRPresent=", pDCTstat->DimmDRPresent);
	print_tx("\t DIMMPresence: DimmQRPresent=", pDCTstat->DimmQRPresent);
	print_tx("\t DIMMPresence: DATAload[0]=", pDCTstat->DATAload[0]);
	print_tx("\t DIMMPresence: MAload[0]=", pDCTstat->MAload[0]);
	print_tx("\t DIMMPresence: MAdimms[0]=", pDCTstat->MAdimms[0]);
	print_tx("\t DIMMPresence: DATAload[1]=", pDCTstat->DATAload[1]);
	print_tx("\t DIMMPresence: MAload[1]=", pDCTstat->MAload[1]);
	print_tx("\t DIMMPresence: MAdimms[1]=", pDCTstat->MAdimms[1]);

	if (pDCTstat->DIMMValid != 0) {	/* If any DIMMs are present...*/
		if (RegDIMMPresent != 0) {
			if ((RegDIMMPresent ^ pDCTstat->DIMMValid) !=0) {
				/* module type DIMM mismatch (reg'ed, unbuffered) */
				pDCTstat->ErrStatus |= 1 << SB_DimmMismatchM;
				pDCTstat->ErrCode = SC_StopError;
			} else{
				/* all DIMMs are registered */
				pDCTstat->Status |= 1 << SB_Registered;
			}
		}
		if (pDCTstat->DimmECCPresent != 0) {
			if ((pDCTstat->DimmECCPresent ^ pDCTstat->DIMMValid) == 0) {
				/* all DIMMs are ECC capable */
				pDCTstat->Status |= 1 << SB_ECCDIMMs;
			}
		}
		if (pDCTstat->DimmPARPresent != 0) {
			if ((pDCTstat->DimmPARPresent ^ pDCTstat->DIMMValid) == 0) {
				/*all DIMMs are Parity capable */
				pDCTstat->Status |= 1 << SB_PARDIMMs;
			}
		}
	} else {
		/* no DIMMs present or no DIMMs that qualified. */
		pDCTstat->ErrStatus |= 1 << SB_NoDimms;
		pDCTstat->ErrCode = SC_StopError;
	}

	print_tx("\t DIMMPresence: Status ", pDCTstat->Status);
	print_tx("\t DIMMPresence: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("\t DIMMPresence: ErrCode ", pDCTstat->ErrCode);
	print_t("\t DIMMPresence: Done\n");

	mctHookAfterDIMMpre();

	return pDCTstat->ErrCode;
}


static u8 Sys_Capability_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, int j, int k)
{
	/* Determine if system is capable of operating at given input
	 * parameters for CL, and T.  There are three components to
	 * determining "maximum frequency" in AUTO mode: SPD component,
	 * Bus load component, and "Preset" max frequency component.
	 * This procedure is used to help find the SPD component and relies
	 * on pre-determination of the bus load component and the Preset
	 * components.  The generalized algorithm for finding maximum
	 * frequency is structured this way so as to optimize for CAS
	 * latency (which might get better as a result of reduced frequency).
	 * See "Global relationship between index values and item values"
	 * for definition of CAS latency index (j) and Frequency index (k).
	 */
	u8 freqOK, ClOK;
	u8 ret = 0;

	if (Get_Fk_D(k) > pDCTstat->PresetmaxFreq)
		freqOK = 0;
	else
		freqOK = 1;

	/* compare proposed CAS latency with AMD Si capabilities */
	if ((j < J_MIN) || (j > J_MAX))
		ClOK = 0;
	else
		ClOK = 1;

	if (freqOK && ClOK)
		ret = 1;

	return ret;
}


static u8 Get_DIMMAddress_D(struct DCTStatStruc *pDCTstat, u8 i)
{
	u8 *p;

	p = pDCTstat->DIMMAddr;
	return p[i];
}


static void mct_initDCT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 val;
	u8 err_code;

	/* Config. DCT0 for Ganged or unganged mode */
	print_t("\tmct_initDCT: DCTInit_D 0\n");
	DCTInit_D(pMCTstat, pDCTstat, 0);
	if (pDCTstat->ErrCode == SC_FatalErr) {
		// Do nothing goto exitDCTInit;	/* any fatal errors? */
	} else {
		/* Configure DCT1 if unganged and enabled*/
		if (!pDCTstat->GangedMode) {
			if (pDCTstat->DIMMValidDCT[1] > 0) {
				print_t("\tmct_initDCT: DCTInit_D 1\n");
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
// exitDCTInit:
}


static void mct_DramInit(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;

	mct_BeforeDramInit_Prod_D(pMCTstat, pDCTstat);
	// FIXME: for rev A: mct_BeforeDramInit_D(pDCTstat, dct);

	/* Disable auto refresh before Dram init when in ganged mode (Erratum 278) */
	if (pDCTstat->LogicalCPUID & (AMD_DR_B0 | AMD_DR_B1 | AMD_DR_BA)) {
		if (pDCTstat->GangedMode) {
			val = Get_NB32(pDCTstat->dev_dct, 0x8C + (0x100 * dct));
			val |= 1 << DisAutoRefresh;
			Set_NB32(pDCTstat->dev_dct, 0x8C + (0x100 * dct), val);
		}
	}

	mct_DramInit_Hw_D(pMCTstat, pDCTstat, dct);

	/* Re-enable auto refresh after Dram init when in ganged mode
	 * to ensure both DCTs are in sync (Erratum 278)
	 */

	if (pDCTstat->LogicalCPUID & (AMD_DR_B0 | AMD_DR_B1 | AMD_DR_BA)) {
		if (pDCTstat->GangedMode) {
			do {
				val = Get_NB32(pDCTstat->dev_dct, 0x90 + (0x100 * dct));
			} while (!(val & (1 << InitDram)));

			WaitRoutine_D(50);

			val = Get_NB32(pDCTstat->dev_dct, 0x8C + (0x100 * dct));
			val &= ~(1 << DisAutoRefresh);
			Set_NB32(pDCTstat->dev_dct, 0x8C + (0x100 * dct), val);
			val |= 1 << DisAutoRefresh;
			Set_NB32(pDCTstat->dev_dct, 0x8C + (0x100 * dct), val);
			val &= ~(1 << DisAutoRefresh);
			Set_NB32(pDCTstat->dev_dct, 0x8C + (0x100 * dct), val);
		}
	}
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
		if (mctGet_NVbits(NV_Unganged))
			pDCTstat->ErrStatus |= (1 << SB_DimmMismatchO);

		if (!(pDCTstat->ErrStatus & (1 << SB_DimmMismatchO))) {
			pDCTstat->GangedMode = 1;
			/* valid 128-bit mode population. */
			pDCTstat->Status |= 1 << SB_128bitmode;
			reg = 0x110;
			val = Get_NB32(pDCTstat->dev_dct, reg);
			val |= 1 << DctGangEn;
			Set_NB32(pDCTstat->dev_dct, reg, val);
			print_tx("setMode: DRAM Controller Select Low Register = ", val);
		}
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
		i_start = 0;
		i_end = 2;
	} else {
		i_start = dct;
		i_end = dct + 1;
	}
	for (i = i_start; i < i_end; i++) {
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
		print_tx("mct_SyncDCTsReady: Node ", pDCTstat->Node_ID);
		dev = pDCTstat->dev_dct;

		if ((pDCTstat->DIMMValidDCT[0]) || (pDCTstat->DIMMValidDCT[1])) {		/* This Node has dram */
			do {
				val = Get_NB32(dev, 0x110);
			} while (!(val & (1 << DramEnabled)));
			print_t("mct_SyncDCTsReady: DramEnabled\n");
		}
	}	/* Node is present */
}


static void mct_AfterGetCLT(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	if (!pDCTstat->GangedMode) {
		if (dct == 0) {
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

	if (dct == 0) {
		SPDCalcWidth_D(pMCTstat, pDCTstat);
		ret = mct_setMode(pMCTstat, pDCTstat);
	} else {
		ret = pDCTstat->ErrCode;
	}

	print_tx("SPDCalcWidth: Status ", pDCTstat->Status);
	print_tx("SPDCalcWidth: ErrStatus ", pDCTstat->ErrStatus);
	print_tx("SPDCalcWidth: ErrCode ", pDCTstat->ErrCode);
	print_t("SPDCalcWidth: Done\n");

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
				print_tx("AfterStitch DCT0 and DCT1: DRAM Controller Select Low Register = ", val);
				print_tx("AfterStitch DCT0 and DCT1: DRAM Controller Select High Register = ", dword);

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
				print_tx("AfterStitch DCT1 only: DRAM Controller Select Low Register = ", val);
				print_tx("AfterStitch DCT1 only: DRAM Controller Select High Register = ", dword);
			}
		}
	} else {
		pDCTstat->NodeSysLimit += pDCTstat->DCTSysLimit;
	}
	print_tx("AfterStitch pDCTstat->NodeSysBase = ", pDCTstat->NodeSysBase);
	print_tx("mct_AfterStitchMemory: pDCTstat->NodeSysLimit ", pDCTstat->NodeSysLimit);
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
			if (pDCTstat->DIMMValidDCT[1] && !pDCTstat->GangedMode) {
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

	Get_Trdrd(pMCTstat, pDCTstat, dct);
	Get_Twrwr(pMCTstat, pDCTstat, dct);
	Get_Twrrd(pMCTstat, pDCTstat, dct);
	Get_TrwtTO(pMCTstat, pDCTstat, dct);
	Get_TrwtWB(pMCTstat, pDCTstat);

	reg = 0x8C + reg_off;		/* Dram Timing Hi */
	val = Get_NB32(dev, reg);
	val &= 0xffff0300;
	dword = pDCTstat->TrwtTO; //0x07
	val |= dword << 4;
	dword = pDCTstat->Twrrd; //0x03
	val |= dword << 10;
	dword = pDCTstat->Twrwr; //0x03
	val |= dword << 12;
	dword = pDCTstat->Trdrd; //0x03
	val |= dword << 14;
	dword = pDCTstat->TrwtWB; //0x07
	val |= dword;
	val = OtherTiming_A_D(pDCTstat, val);
	Set_NB32(dev, reg, val);

}


static void Get_Trdrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 Trdrd;
	u8 byte;
	u32 dword;
	u32 val;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	if ((pDCTstat->Dimmx4Present != 0) && (pDCTstat->Dimmx8Present != 0)) {
		/* mixed (x4 or x8) DIMM types
		  the largest DqsRcvEnGrossDelay of any DIMM minus the DqsRcvEnGrossDelay
		  of any other DIMM is equal to the Critical Gross Delay Difference (CGDD) for Trdrd.*/
		byte = Get_DqsRcvEnGross_Diff(pDCTstat, dev, index_reg);
		if (byte == 0)
			Trdrd = 1;
		else
			Trdrd = 2;

	} else {
		/*
		 Trdrd with non-mixed DIMM types
		 RdDqsTime are the same for all DIMMs and DqsRcvEn difference between
		 any two DIMMs is less than half of a MEMCLK, BIOS should program Trdrd to 0000b,
		 else BIOS should program Trdrd to 0001b.

		 RdDqsTime are the same for all DIMMs
		 DDR400~DDR667 only use one set register
		 DDR800 have two set register for DIMM0 and DIMM1 */
		Trdrd = 1;
		if (pDCTstat->Speed > 3) {
			/* DIMM0+DIMM1 exist */ //NOTE it should be 5
			val = bsf(pDCTstat->DIMMValid);
			dword = bsr(pDCTstat->DIMMValid);
			if (dword != val && dword != 0)  {
				/* DCT Read DQS Timing Control - DIMM0 - Low */
				dword = Get_NB32_index_wait(dev, index_reg, 0x05);
				/* DCT Read DQS Timing Control - DIMM1 - Low */
				val = Get_NB32_index_wait(dev, index_reg, 0x105);
				if (val != dword)
					goto Trdrd_1;

				/* DCT Read DQS Timing Control - DIMM0 - High */
				dword = Get_NB32_index_wait(dev, index_reg, 0x06);
				/* DCT Read DQS Timing Control - DIMM1 - High */
				val = Get_NB32_index_wait(dev, index_reg, 0x106);
				if (val != dword)
					goto Trdrd_1;
			}
		}

		/* DqsRcvEn difference between any two DIMMs is
		   less than half of a MEMCLK */
		/* DqsRcvEn byte 1,0*/
		if (Check_DqsRcvEn_Diff(pDCTstat, dct, dev, index_reg, 0x10))
			goto Trdrd_1;
		/* DqsRcvEn byte 3,2*/
		if (Check_DqsRcvEn_Diff(pDCTstat, dct, dev, index_reg, 0x11))
			goto Trdrd_1;
		/* DqsRcvEn byte 5,4*/
		if (Check_DqsRcvEn_Diff(pDCTstat, dct, dev, index_reg, 0x20))
			goto Trdrd_1;
		/* DqsRcvEn byte 7,6*/
		if (Check_DqsRcvEn_Diff(pDCTstat, dct, dev, index_reg, 0x21))
			goto Trdrd_1;
		/* DqsRcvEn ECC*/
		if (Check_DqsRcvEn_Diff(pDCTstat, dct, dev, index_reg, 0x12))
			goto Trdrd_1;
		Trdrd = 0;
	Trdrd_1:
		;
	}
	pDCTstat->Trdrd = Trdrd;

}


static void Get_Twrwr(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 Twrwr = 0;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;
	u32 val;
	u32 dword;

	/* WrDatGrossDlyByte only use one set register when DDR400~DDR667
	   DDR800 have two set register for DIMM0 and DIMM1 */
	if (pDCTstat->Speed > 3) {
		val = bsf(pDCTstat->DIMMValid);
		dword = bsr(pDCTstat->DIMMValid);
		if (dword != val && dword != 0)  {
			/*the largest WrDatGrossDlyByte of any DIMM minus the
			  WrDatGrossDlyByte of any other DIMM is equal to CGDD */
			val = Get_WrDatGross_Diff(pDCTstat, dct, dev, index_reg);
		}
		if (val == 0)
			Twrwr = 2;
		else
			Twrwr = 3;
	}
	pDCTstat->Twrwr = Twrwr;
}


static void Get_Twrrd(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 byte, bytex, val;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	/* On any given byte lane, the largest WrDatGrossDlyByte delay of
	   any DIMM minus the DqsRcvEnGrossDelay delay of any other DIMM is
	   equal to the Critical Gross Delay Difference (CGDD) for Twrrd.*/

	/* WrDatGrossDlyByte only use one set register when DDR400~DDR667
	   DDR800 have two set register for DIMM0 and DIMM1 */
	if (pDCTstat->Speed > 3) {
		val = Get_WrDatGross_Diff(pDCTstat, dct, dev, index_reg);
	} else {
		val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 1);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM0 */
		pDCTstat->WrDatGrossH = (u8) val; /* low byte = max value */
	}

	Get_DqsRcvEnGross_Diff(pDCTstat, dev, index_reg);

	bytex = pDCTstat->DqsRcvEnGrossL;
	byte = pDCTstat->WrDatGrossH;
	if (byte > bytex) {
		byte -= bytex;
		if (byte == 1)
			bytex = 1;
		else
			bytex = 2;
	} else {
		bytex = 0;
	}
	pDCTstat->Twrrd = bytex;
}


static void Get_TrwtTO(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 dct)
{
	u8 byte, bytex;
	u32 index_reg = 0x98 + 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	/* On any given byte lane, the largest WrDatGrossDlyByte delay of
	   any DIMM minus the DqsRcvEnGrossDelay delay of any other DIMM is
	   equal to the Critical Gross Delay Difference (CGDD) for TrwtTO. */
	Get_DqsRcvEnGross_Diff(pDCTstat, dev, index_reg);
	Get_WrDatGross_Diff(pDCTstat, dct, dev, index_reg);
	bytex = pDCTstat->DqsRcvEnGrossL;
	byte = pDCTstat->WrDatGrossH;
	if (bytex > byte) {
		bytex -= byte;
		if ((bytex == 1) || (bytex == 2))
			bytex = 3;
		else
			bytex = 4;
	} else {
		byte -= bytex;
		if ((byte == 0) || (byte == 1))
			bytex = 2;
		else
			bytex = 1;
	}

	pDCTstat->TrwtTO = bytex;
}


static void Get_TrwtWB(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	/* TrwtWB ensures read-to-write data-bus turnaround.
	   This value should be one more than the programmed TrwtTO.*/
	pDCTstat->TrwtWB = pDCTstat->TrwtTO + 1;
}


static u8 Check_DqsRcvEn_Diff(struct DCTStatStruc *pDCTstat,
					u8 dct, u32 dev, u32 index_reg,
					u32 index)
{
	u8 Smallest_0, Largest_0, Smallest_1, Largest_1;
	u8 i;
	u32 val;
	u8 byte;
	u8 ecc_reg = 0;

	Smallest_0 = 0xFF;
	Smallest_1 = 0xFF;
	Largest_0 = 0;
	Largest_1 = 0;

	if (index == 0x12)
		ecc_reg = 1;

	for (i = 0; i < 8; i+=2) {
		if (pDCTstat->DIMMValid & (1 << i)) {
			val = Get_NB32_index_wait(dev, index_reg, index);
			byte = val & 0xFF;
			if (byte < Smallest_0)
				Smallest_0 = byte;
			if (byte > Largest_0)
				Largest_0 = byte;
			if (!(ecc_reg)) {
				byte = (val >> 16) & 0xFF;
				if (byte < Smallest_1)
					Smallest_1 = byte;
				if (byte > Largest_1)
					Largest_1 = byte;
			}
		}
		index += 3;
	}	/* while ++i */

	/* check if total DqsRcvEn delay difference between any
	   two DIMMs is less than half of a MEMCLK */
	if ((Largest_0 - Smallest_0) > 31)
		return 1;
	if (!(ecc_reg))
		if ((Largest_1 - Smallest_1) > 31)
			return 1;
	return 0;
}


static u8 Get_DqsRcvEnGross_Diff(struct DCTStatStruc *pDCTstat,
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

	pDCTstat->DqsRcvEnGrossL = Largest;
	return Largest - Smallest;
}


static u8 Get_WrDatGross_Diff(struct DCTStatStruc *pDCTstat,
					u8 dct, u32 dev, u32 index_reg)
{
	u8 Smallest, Largest;
	u32 val;
	u8 byte, bytex;

	/* The largest WrDatGrossDlyByte of any DIMM minus the
	  WrDatGrossDlyByte of any other DIMM is equal to CGDD */
	val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x01);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM0 */
	Largest = val & 0xFF;
	Smallest = (val >> 8) & 0xFF;
	val = Get_WrDatGross_MaxMin(pDCTstat, dct, dev, index_reg, 0x101);	/* WrDatGrossDlyByte byte 0,1,2,3 for DIMM1 */
	byte = val & 0xFF;
	bytex = (val >> 8) & 0xFF;
	if (bytex < Smallest)
		Smallest = bytex;
	if (byte > Largest)
		Largest = byte;

	// FIXME: Add Cx support.

	pDCTstat->WrDatGrossH = Largest;
	return Largest - Smallest;
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

	for (i = 0; i < 8; i+=2) {
		if (pDCTstat->DIMMValid & (1 << i)) {
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
	for (i = 0; i < 2; i++) {
		val = Get_NB32_index_wait(dev, index_reg, index);
		val &= 0x60606060;
		val >>= 5;
		for (j = 0; j < 4; j++) {
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



static void mct_FinalMCT_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	print_t("\tmct_FinalMCT_D: Clr Cl, Wb\n");


	/* ClrClToNB_D postponed until we're done executing from ROM */
	mct_ClrWbEnhWsbDis_D(pMCTstat, pDCTstat);
}


static void mct_InitialMCT_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstat)
{
	print_t("\tmct_InitialMCT_D: Set Cl, Wb\n");
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
	for (Node = 0; Node < MAX_NODES_SUPPORTED; Node++) {
		pDCTstat = pDCTstatA + Node;
		devx = pDCTstat->dev_map;

		/* get base/limit from Node0 */
		reg = 0x40 + (Node << 3);		/* Node0/Dram Base 0 */
		val = Get_NB32(dev, reg);
		Drambase = val >> (16 + 3);

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

			if (pMCTstat->GStatus & (1 << GSB_HWHole)) {
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
	u8 cs;
	u32 index;
	u16 word;

	/* Tri-state unused chipselects when motherboard
	   termination is available */

	// FIXME: skip for Ax

	word = pDCTstat->CSPresent;
	if (pDCTstat->Status & (1 << SB_Registered)) {
		for (cs = 0; cs < 8; cs++) {
			if (word & (1 << cs)) {
				if (!(cs & 1))
					word |= 1 << (cs + 1);
			}
		}
	}
	word = (~word) & 0xFF;
	index  = 0x0c;
	val = Get_NB32_index_wait(dev, index_reg, index);
	val |= word;
	Set_NB32_index_wait(dev, index_reg, index, val);
}


#ifdef UNUSED_CODE
static void SetCKETriState(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 val;
	u32 dev;
	u32 index_reg = 0x98 + 0x100 * dct;
	u8 cs;
	u32 index;
	u16 word;

	/* Tri-state unused CKEs when motherboard termination is available */

	// FIXME: skip for Ax

	dev = pDCTstat->dev_dct;
	word = 0x101;
	for (cs = 0; cs < 8; cs++) {
		if (pDCTstat->CSPresent & (1 << cs)) {
			if (!(cs & 1))
				word &= 0xFF00;
			else
				word &= 0x00FF;
		}
	}

	index  = 0x0c;
	val = Get_NB32_index_wait(dev, index_reg, index);
	if ((word & 0x00FF) == 1)
		val |= 1 << 12;
	else
		val &= ~(1 << 12);

	if ((word >> 8) == 1)
		val |= 1 << 13;
	else
		val &= ~(1 << 13);

	Set_NB32_index_wait(dev, index_reg, index, val);
}
#endif

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

	// FIXME: skip for Ax

	dev = pDCTstat->dev_dct;

	/* Tri-state unused ODTs when motherboard termination is available */
	max_dimms = (u8) mctGet_NVbits(NV_MAX_DIMMS);
	odt = 0x0F;	/* tristate all the pins then clear the used ones. */

	for (cs = 0; cs < 8; cs += 2) {
		if (pDCTstat->CSPresent & (1 << cs)) {
			odt &= ~(1 << (cs / 2));

			/* if quad-rank capable platform clear additional pins */
			if (max_dimms != MAX_CS_SUPPORTED) {
				if (pDCTstat->CSPresent & (1 << (cs + 1)))
					odt &= ~(4 << (cs / 2));
			}
		}
	}

	index  = 0x0C;
	val = Get_NB32_index_wait(dev, index_reg, index);
	val |= (odt << 8);
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
	for (i = 0; i < 6; i++) {
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


static void WaitRoutine_D(u32 time)
{
	while (time) {
		_EXECFENCE;
		time--;
	}
}


static void mct_EarlyArbEn_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
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

	reg = 0x78;
	val = Get_NB32(dev, reg);

	//FIXME: check for Cx
	if (CheckNBCOFEarlyArbEn(pMCTstat, pDCTstat))
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
		val = Get_NB32(dev, reg * 0x100);	/* get the DCT1 value */

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

	// Yes this could be nicer but this was how the asm was....
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
	struct DCTStatStruc *pDCTstat;

	/* Initialize Data structures by clearing all entries to 0 */
	memset(pMCTstat, 0x00, sizeof(*pMCTstat));

	for (Node = 0; Node < 8; Node++) {
		pDCTstat = pDCTstatA + Node;

		/* Clear all entries except persistentData */
		memset(pDCTstat, 0x00, sizeof(*pDCTstat) - sizeof(pDCTstat->persistentData));
	}
}


static void mct_BeforeDramInit_Prod_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u8 i;
	u32 reg_off;
	u32 dev = pDCTstat->dev_dct;

	// FIXME: skip for Ax
	if ((pDCTstat->Speed == 3) || (pDCTstat->Speed == 2)) { // MemClkFreq = 667MHz or 533MHz
		for (i = 0; i < 2; i++) {
			reg_off = 0x100 * i;
			Set_NB32(dev,  0x98 + reg_off, 0x0D000030);
			Set_NB32(dev,  0x9C + reg_off, 0x00000806);
			Set_NB32(dev,  0x98 + reg_off, 0x4D040F30);
		}
	}
}


void mct_AdjustDelayRange_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat, u8 *dqs_pos)
{
	// FIXME: Skip for Ax
	if ((pDCTstat->Speed == 3) || (pDCTstat->Speed == 2)) { // MemClkFreq = 667MHz or 533MHz
		*dqs_pos = 32;
	}
}

static u32 mct_DisDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u32 DramConfigLo, u8 dct)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	/* Write 0000_07D0h to register F2x[1, 0]98_x4D0FE006 */
	if (pDCTstat->LogicalCPUID & (AMD_DA_C2 | AMD_RB_C3)) {
		Set_NB32(dev,  0x9C + reg_off, 0x7D0);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE006);
		Set_NB32(dev,  0x9C + reg_off, 0x190);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE007);
	}

	return DramConfigLo | /* DisDllShutdownSR */ 1 << 27;
}

static void mct_EnDllShutdownSR(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat, u8 dct)
{
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct, val;

	/* Write 0000_07D0h to register F2x[1, 0]98_x4D0FE006 */
	if (pDCTstat->LogicalCPUID & (AMD_DA_C2 | AMD_RB_C3)) {
		Set_NB32(dev,  0x9C + reg_off, 0x1C);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE006);
		Set_NB32(dev,  0x9C + reg_off, 0x13D);
		Set_NB32(dev,  0x98 + reg_off, 0x4D0FE007);

		val = Get_NB32(dev, 0x90 + reg_off);
		val &= ~(1 << 27/* DisDllShutdownSR */);
		Set_NB32(dev, 0x90 + reg_off, val);
	}
}

void mct_SetClToNB_D(struct MCTStatStruc *pMCTstat,
			struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	// FIXME: Maybe check the CPUID? - not for now.
	// pDCTstat->LogicalCPUID;

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

	// FIXME: Maybe check the CPUID? - not for now.
	// pDCTstat->LogicalCPUID;

	msr = BU_CFG2;
	_RDMSR(msr, &lo, &hi);
	if (!pDCTstat->ClToNB_flag)
		lo &= ~(1 << ClLinesToNbDis);
	_WRMSR(msr, lo, hi);

}


void mct_SetWbEnhWsbDis_D(struct MCTStatStruc *pMCTstat,
				struct DCTStatStruc *pDCTstat)
{
	u32 lo, hi;
	u32 msr;

	// FIXME: Maybe check the CPUID? - not for now.
	// pDCTstat->LogicalCPUID;

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

	// FIXME: Maybe check the CPUID? - not for now.
	// pDCTstat->LogicalCPUID;

	msr = BU_CFG;
	_RDMSR(msr, &lo, &hi);
	hi &= ~(1 << WbEnhWsbDis_D);
	_WRMSR(msr, lo, hi);
}


void mct_SetDramConfigHi_D(struct DCTStatStruc *pDCTstat, u32 dct,
				u32 DramConfigHi)
{
	/* Bug#15114: Comp. update interrupted by Freq. change can cause
	 * subsequent update to be invalid during any MemClk frequency change:
	 * Solution: From the bug report:
	 *  1. A software-initiated frequency change should be wrapped into the
	 *     following sequence :
	 *	a) Disable Compensation (F2[1, 0]9C_x08[30])
	 *	b) Reset the Begin Compensation bit (D3CMP->COMP_CONFIG[0]) in
	 *	   all the compensation engines
	 *	c) Do frequency change
	 *	d) Enable Compensation (F2[1, 0]9C_x08[30])
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
	Set_NB32_index_wait(dev, index_reg, index, val | (1 << DisAutoComp));

	//FIXME: check for Bx Cx CPU
	// if Ax mct_SetDramConfigHi_Samp_D

	/* errata#177 */
	index = 0x4D014F00;	/* F2x[1, 0]9C_x[D0FFFFF:D000000] DRAM Phy Debug Registers */
	index |= 1 << DctAccessWrite;
	val = 0;
	Set_NB32_index_wait(dev, index_reg, index, val);

	Set_NB32(dev, 0x94 + 0x100 * dct, DramConfigHi);

	index = 0x08;
	val = Get_NB32_index_wait(dev, index_reg, index);
	Set_NB32_index_wait(dev, index_reg, index, val & (~(1 << DisAutoComp)));
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
	 * Solution: At least, set WrDqs fine delay to be 0 for DDR2 training.
	 */

	for (Node = 0; Node < 8; Node++) {
		pDCTstat = pDCTstatA + Node;

		if (pDCTstat->NodePresent) {
			mct_BeforeDQSTrain_Samp_D(pMCTstat, pDCTstat);
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
	if (lo & (1<<17)) {		/* save the old value */
		wrap32dis = 1;
	}
	lo |= (1<<17);			/* HWCR.wrap32dis */
	lo &= ~(1<<15);			/* SSEDIS */
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
	if (!wrap32dis) {
		addr = HWCR;
		_RDMSR(addr, &lo, &hi);
		lo &= ~(1<<17);		/* restore HWCR.wrap32dis */
		_WRMSR(addr, lo, hi);
	}
}


void mct_EnableDatIntlv_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
{
	u32 dev = pDCTstat->dev_dct;
	u32 val;

	/*  Enable F2x110[DctDatIntlv] */
	// Call back not required mctHookBeforeDatIntlv_D()
	// FIXME Skip for Ax
	if (!pDCTstat->GangedMode) {
		val = Get_NB32(dev, 0x110);
		val |= 1 << 5;			// DctDatIntlv
		Set_NB32(dev, 0x110, val);

		// FIXME Skip for Cx
		dev = pDCTstat->dev_nbmisc;
		val = Get_NB32(dev, 0x8C);	// NB Configuration Hi
		val |= 1 << (36-32);		// DisDatMask
		Set_NB32(dev, 0x8C, val);
	}
}

#ifdef UNUSED_CODE
static void mct_SetupSync_D(struct MCTStatStruc *pMCTstat,
					struct DCTStatStruc *pDCTstat)
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
		val |= ChSetupSync;
		Set_NB32(dev, 0x78, val);
	}
}
#endif

static void AfterDramInit_D(struct DCTStatStruc *pDCTstat, u8 dct) {

	u32 val;
	u32 reg_off = 0x100 * dct;
	u32 dev = pDCTstat->dev_dct;

	if (pDCTstat->LogicalCPUID & (AMD_DR_B2 | AMD_DR_B3)) {
		mct_Wait(10000);	/* Wait 50 us*/
		val = Get_NB32(dev, 0x110);
		if (val & (1 << DramEnabled)) {
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
 *  RR = rows-13 binary
 *  B = Banks-2 binary
 *  CCC = Columns-9 binary
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
