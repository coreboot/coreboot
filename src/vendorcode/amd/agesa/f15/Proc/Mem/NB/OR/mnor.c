/* $NoKeywords:$ */
/**
 * @file
 *
 * mnor.c
 *
 * Common Northbridge  functions for Orochi
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/OR)
 * @e \$Revision: 52421 $ @e \$Date: 2011-05-05 21:03:23 -0600 (Thu, 05 May 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2012 Advanced Micro Devices, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of Advanced Micro Devices, Inc. nor the names of
*       its contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* ***************************************************************************
*
*/

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mnor.h"
#include "mu.h"
#include "S3.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "F15PackageType.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_NB_OR_MNOR_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

#define SPLIT_CHANNEL   (UINT32) 0x20000000
#define CHANNEL_SELECT  (UINT32) 0x10000000

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/**
 * @todo:  Add Comments with field descriptions
 */
CONST MEM_FREQ_CHANGE_PARAM FreqChangeParamOr = {0x0190, 7, 7, 14, 3, 18, 470, 946};

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;
extern PSO_ENTRY DefaultPlatformMemoryConfiguration[];
extern OPTION_MEM_FEATURE_NB* memNTrainFlowControl[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the northbridge block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *     @param[in]        *FeatPtr  - Pointer to the MEM_FEAT_BLOCK_NB
 *     @param[in]        *SharedPtr - Pointer to the MEM_SHARED_DATA
 *     @param[in]        NodeID  - UINT8 indicating node ID of the NB object.
 *
 *     @return     Boolean indicating that this is the correct memory
 *                 controller type for the node number that was passed in.
 */

BOOLEAN
MemConstructNBBlockOR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  )
{
  UINT8 Dct;
  UINT8 Channel;
  UINT8 SpdSocketIndex;
  UINT8 SpdChannelIndex;
  DIE_STRUCT *MCTPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[NodeID].SocketId, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedOr (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->RefPtr = MemPtr->ParameterListPtr;
  NBPtr->SharedPtr = SharedPtr;

  MCTPtr = &(MemPtr->DiesPerSystem[NodeID]);
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->MCTPtr->NodeId = NodeID;
  NBPtr->PciAddr.AddressValue = MCTPtr->PciAddr.AddressValue;
  NBPtr->VarMtrrHiMsk = GetVarMtrrHiMsk (&(MemPtr->DiesPerSystem[NodeID].LogicalCpuid), &(MemPtr->StdHeader));

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_OR * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_OR * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK))
                                          )
                                        );
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, NodeID, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE); // Could not allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
    return FALSE;
  }

  MCTPtr->DctCount = MAX_DCTS_PER_NODE_OR;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_OR * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_OR;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].Dct = Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_OR * sizeof (CH_DEF_STRUCT);
  }
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;

  //
  // Initialize Socket List
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    MemPtr->SocketList[MCTPtr->SocketId].ChannelPtr[(MCTPtr->DieId * 2) + Dct] = &(MCTPtr->DctData[Dct].ChData[0]);
    MemPtr->SocketList[MCTPtr->SocketId].TimingsPtr[(MCTPtr->DieId * 2) + Dct] = &(MCTPtr->DctData[Dct].Timings);
    MCTPtr->DctData[Dct].ChData[0].ChannelID = (MCTPtr->DieId * 2) + Dct;
  }

  MemNInitNBDataOr (NBPtr);

  FeatPtr->InitCPG (NBPtr);
  FeatPtr->InitHwRxEn (NBPtr);
  FeatPtr->InitEarlySampleSupport (NBPtr);
  NBPtr->FeatPtr = FeatPtr;


  //
  // Calculate SPD Offsets per channel and assign pointers to the data.  At this point, we calculate the Node-Dct-Channel
  // centric offsets and store the pointers to the first DIMM of each channel in the Channel Definition struct for that
  // channel.  This pointer is then used later to calculate the offsets to be used for each logical dimm once the
  // dimm types(QR or not) are known. This is done in the Technology block constructor.
  //
  // Calculate the SpdSocketIndex separately from the SpdChannelIndex.
  // This will facilitate modifications due to some processors that might
  // map the DCT-CHANNEL differently.
  //
  SpdSocketIndex = GetSpdSocketIndex (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, &MemPtr->StdHeader);
  //
  // Traverse the Dct/Channel structures
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_OR; Dct++) {
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_OR; Channel++) {
      //
      // Calculate the number of Dimms on this channel using the
      //   die/dct/channel to Socket/channel conversion.
      //
      SpdChannelIndex = GetSpdChannelIndex (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                            NBPtr->MCTPtr->SocketId,
                                            MemNGetSocketRelativeChannelNb (NBPtr, Dct, Channel),
                                            &MemPtr->StdHeader);
      NBPtr->MCTPtr->DctData[Dct].ChData[Channel].SpdPtr = &(MemPtr->SpdDataStructure[SpdSocketIndex + SpdChannelIndex]);
    }
  }

  //
  // Initialize Dct and DctCfgSel bit
  //
  MemNSetBitFieldNb (NBPtr, BFDctCfgSel, 0);
  MemNSwitchDCTNb (NBPtr, 0);

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes member functions and variables of NB block.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitNBDataOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32  PackageType;

  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;

  MemNInitNBRegTableOr (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_OR;
  NBPtr->ChannelCount = MAX_CHANNELS_PER_DCT_OR;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_OR;
  NBPtr->Ganged = FALSE;
  NBPtr->PosTrnPattern = POS_PATTERN_256B;
  NBPtr->MemCleared = FALSE;
  NBPtr->StartupSpeed = DDR667_FREQUENCY;
  NBPtr->RcvrEnDlyLimit = 0x1FF;
  NBPtr->DefDctSelIntLvAddr = 3;
  NBPtr->NbFreqChgState = 0;
  NBPtr->MaxRxEnSeedTotal = 0x3FF;
  NBPtr->MinRxEnSeedGross = 0;
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &FreqChangeParamOr;
  NBPtr->CsRegMsk = 0x7FF83FE0;
  NBPtr->TotalMaxVrefRange = 0x20;
  NBPtr->TotalRdDQSDlyRange = 0x40;
  NBPtr->MaxSeedCount = MAX____DQS_SEED_COUNT;
  NBPtr->PhaseLaneMask = 0x3FFFF;
  NBPtr->MaxDiamondStep = 3;

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);

  NBPtr->SetMaxLatency = MemNSetMaxLatencyOr;
  NBPtr->getMaxLatParams = MemNGetMaxLatParamsOr;
  NBPtr->InitializeMCT = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  NBPtr->FinalizeMCT = MemNFinalizeMctOr;
  NBPtr->SendMrsCmd = MemNSendMrsCmdUnb;
  NBPtr->sendZQCmd = MemNSendZQCmdNb;
  NBPtr->WritePattern = MemNWritePatternOr;
  NBPtr->ReadPattern = MemNReadPatternOr;
  NBPtr->GenHwRcvEnReads = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;
  NBPtr->CompareTestPattern = MemNCompareTestPatternNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternNb;
  NBPtr->StitchMemory = MemNStitchMemoryNb;
  NBPtr->AutoConfig = MemNAutoConfigOr;
  NBPtr->PlatformSpec = MemNPlatformSpecUnb;
  NBPtr->InitMCT = MemNInitMCTNb;
  NBPtr->DisableDCT = MemNDisableDCTUnb;
  NBPtr->StartupDCT = MemNStartupDCTUnb;
  NBPtr->SyncTargetSpeed = MemNSyncTargetSpeedNb;
  NBPtr->ChangeFrequency = MemNChangeFrequencyUnb;
  NBPtr->RampUpFrequency = MemNRampUpFrequencyNb;
  NBPtr->ChangeNbFrequency = MemNChangeNbFrequencyUnb;
  NBPtr->ChangeNbFrequencyWrap = MemNChangeNbFrequencyWrapUnb;
  NBPtr->ProgramNbPsDependentRegs = MemNProgramNbPstateDependentRegistersUnb;
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsUnb;
  NBPtr->SyncDctsReady = MemNSyncDctsReadyNb;
  NBPtr->HtMemMapInit = MemNHtMemMapInitNb;
  NBPtr->SyncAddrMapToAllNodes = MemNSyncAddrMapToAllNodesNb;
  NBPtr->CpuMemTyping = MemNCPUMemTypingNb;
  NBPtr->BeforeDqsTraining = MemNBeforeDQSTrainingOr;
  NBPtr->AfterDqsTraining = MemNAfterDQSTrainingOr;
  NBPtr->OtherTiming = MemNOtherTimingOr;
  NBPtr->UMAMemTyping = MemNUMAMemTypingNb;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelOr;
  NBPtr->TechBlockSwitch = MemNTechBlockSwitchOr;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldOr;
  NBPtr->SetEccSymbolSize = MemNSetEccSymbolSizeNb;
  NBPtr->TrainingFlow = (VOID (*) (MEM_NB_BLOCK *)) MemNTrainingFlowUnb;
  MemNInitNBDataNb (NBPtr);
  NBPtr->PollBitField = MemNPollBitFieldNb;
  NBPtr->BrdcstCheck = MemNBrdcstCheckNb;
  NBPtr->BrdcstSet = MemNBrdcstSetNb;
  NBPtr->GetTrainDly = MemNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemNSetTrainDlyNb;
  NBPtr->PhyFenceTraining = MemNPhyFenceTrainingUnb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->RankEnabled = MemNRankEnabledNb;
  NBPtr->MemNBeforeDramInitNb = MemNBeforeDramInitOr;
  NBPtr->MemNcmnGetSetTrainDly = MemNcmnGetSetTrainDlyUnb;
  NBPtr->MemPPhyFenceTrainingNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNInitPhyComp = MemNInitPhyCompOr;
  NBPtr->MemNBeforePlatformSpecNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNPlatformSpecificFormFactorInitNb = MemNPlatformSpecificFormFactorInitTblDrvNb;
  NBPtr->MemNPFenceAdjustNb = MemNPFenceAdjustOr;
  NBPtr->GetTrainDlyParms = MemNGetTrainDlyParmsUnb;
  NBPtr->TrainingPatternInit = MemNTrainingPatternInitNb;
  NBPtr->TrainingPatternFinalize = MemNTrainingPatternFinalizeNb;
  NBPtr->GetApproximateWriteDatDelay = MemNGetApproximateWriteDatDelayNb;
  NBPtr->CSPerChannel = MemNCSPerChannelNb;
  NBPtr->CSPerDelay = MemNCSPerDelayNb;
  NBPtr->FlushPattern = MemNFlushPatternNb;
  NBPtr->MinDataEyeWidth = MemNMinDataEyeWidthNb;
  NBPtr->MemNCapSpeedBatteryLife = MemNCapSpeedBatteryLifeOr;
  NBPtr->GetUmaSize = MemNGetUmaSizeNb;
  NBPtr->GetMemClkFreqId = MemNGetMemClkFreqIdUnb;
  NBPtr->EnableSwapIntlvRgn = MemNEnableSwapIntlvRgnNb;
  NBPtr->WaitXMemClks = MemNWaitXMemClksNb;
  NBPtr->MemNGetDramTerm = MemNGetDramTermTblDrvNb;
  NBPtr->MemNGetDynDramTerm = MemNGetDynDramTermTblDrvNb;
  NBPtr->MemNGetMR0CL = MemNGetMR0CLTblDrvNb;
  NBPtr->MemNGetMR0WR = MemNGetMR0WRTblDrvNb;
  NBPtr->MemNSaveMR0 = MemNSaveMR0Or;
  NBPtr->MemNGetMR2CWL = MemNGetMR2CWLUnb;
  NBPtr->AllocateC6Storage = MemNAllocateC6StorageUnb;
  NBPtr->InPhaseCompareRdDqs__Pattern = MemNInPhaseCompareRdDqs__PatternUnb;
  NBPtr->Phase180CompareRdDqs__Pattern = MemN180CompareRdDqs__PatternUnb;
  NBPtr->AgressorContinuousWrites = MemNAgressorContinuousWritesUnb;
  NBPtr->GetPrbs__RdDqsSeed = MemNGetPrbs__RdDqsSeedUnb;
  NBPtr->InitializeRdDqs__VictimContinuousWrites = MemNInitializeRdDqs__VictimContinuousWritesUnb;
  NBPtr->FinalizeRdDqs__VictimContinuousWrites = MemNFinalizeRdDqs__VictimContinuousWritesUnb;
  NBPtr->InitializeRdDqs__VictimChipSelContinuousWrites = MemNInitializeRdDqs__VictimChipSelContinuousWritesUnb;
  NBPtr->StartRdDqs__VictimContinuousWrites = MemNStartRdDqs__VictimContinuousWritesUnb;

  NBPtr->IsSupported[SetSpareEn] = TRUE;
  NBPtr->IsSupported[CheckSpareEn] = TRUE;
  NBPtr->IsSupported[SetDllShutDown] = TRUE;
  NBPtr->IsSupported[CheckEccDLLPwrDnConfig] = TRUE;
  NBPtr->IsSupported[DimmBasedOnSpeed] = FALSE;
  NBPtr->IsSupported[CheckMaxDramRate] = TRUE;
  NBPtr->IsSupported[Check1GAlign] = FALSE;
  NBPtr->IsSupported[CheckDisDllShutdownSR] = FALSE;
  NBPtr->IsSupported[CheckMemClkCSPresent] = TRUE;
  NBPtr->IsSupported[CheckMaxRdDqsDlyPtr] = TRUE;
  NBPtr->IsSupported[CheckPhyFenceTraining] = TRUE;
  NBPtr->IsSupported[CheckSendAllMRCmds] = TRUE;
  NBPtr->IsSupported[CheckGetMCTSysAddr] = FALSE;
  NBPtr->IsSupported[CheckFindPSOverideWithSocket] = TRUE;
  NBPtr->IsSupported[CheckFindPSDct] = FALSE;
  NBPtr->IsSupported[FenceTrnBeforeDramInit] = TRUE;
  NBPtr->IsSupported[UnifiedNbFence] = TRUE;
  NBPtr->IsSupported[CheckODTControls] = TRUE;
  NBPtr->IsSupported[CheckDummyCLRead] = TRUE;
  NBPtr->IsSupported[CheckDllStdBy] = FALSE;
  NBPtr->IsSupported[CheckSlewWithMarginImprv] = FALSE;
  NBPtr->IsSupported[CheckSlewWithoutMarginImprv] = TRUE;
  NBPtr->IsSupported[CheckDllSpeedUp] = TRUE;
  NBPtr->IsSupported[CheckDllRegDis] = FALSE;
  NBPtr->IsSupported[PchgPDMode] = TRUE;
  NBPtr->IsSupported[EccByteTraining] = TRUE;
  NBPtr->IsSupported[CheckDramTerm] = TRUE;
  NBPtr->IsSupported[CheckDramTermDyn] = TRUE;
  NBPtr->IsSupported[CheckQoff] = TRUE;
  NBPtr->IsSupported[CheckDrvImpCtrl] = TRUE;
  NBPtr->IsSupported[CheckSetSameDctODTsEn] = TRUE;
  NBPtr->IsSupported[WLSeedAdjust] = TRUE;
  NBPtr->IsSupported[WLNegativeDelay] = TRUE;
  NBPtr->IsSupported[TwoStageDramInit] = TRUE;
  NBPtr->IsSupported[ForceEnMemHoleRemapping] = TRUE;
  NBPtr->IsSupported[ProgramCsrComparator] = TRUE;
  NBPtr->IsSupported[SetTDqsForx8DimmOnly] = TRUE;
  NBPtr->IsSupported[WlRttNomFor1of3Cfg] = TRUE;

  NBPtr->FamilySpecificHook[ExitPhyAssistedTraining] = MemNExitPhyAssistedTrainingOr;
  NBPtr->FamilySpecificHook[DCTSelectSwitch] = MemNDctCfgSelectUnb;
  NBPtr->FamilySpecificHook[ScrubberErratum] = MemNScrubberErratumOr;
  NBPtr->FamilySpecificHook[AfterSaveRestore] = MemNAfterSaveRestoreUnb;
  NBPtr->FamilySpecificHook[OverrideDataTxFifoWrDly] = MemNDataTxFifoWrDlyOverrideOr;
  NBPtr->FamilySpecificHook[OverrideRcvEnSeed] = MemNOverrideRcvEnSeedOr;
  NBPtr->FamilySpecificHook[OverrideRcvEnSeedPassN] = MemNOverrideRcvEnSeedPassNOr;
  NBPtr->FamilySpecificHook[OverrideWLSeed] = MemNOverrideWLSeedOr;
  NBPtr->FamilySpecificHook[AfterMemClkFreqChg] = MemNAfterMemClkFreqChgOr;
  NBPtr->FamilySpecificHook[CalcWrDqDqsEarly] = MemNCalcWrDqDqsEarlyUnb;
  NBPtr->FamilySpecificHook[TrainWlPerNibble] = MemNTrainWlPerNibbleOr;
  NBPtr->FamilySpecificHook[TrainWlPerNibbleAdjustWLDly] = MemNTrainWlPerNibbleAdjustWLDlyOr;
  NBPtr->FamilySpecificHook[TrainWlPerNibbleSeed] = MemNTrainWlPerNibbleSeedOr;
  NBPtr->FamilySpecificHook[TrainRxEnPerNibble] = MemNTrainRxEnPerNibbleOr;
  NBPtr->FamilySpecificHook[TrainRxEnAdjustDlyPerNibble] = MemNTrainRxEnAdjustDlyPerNibbleOr;
  NBPtr->FamilySpecificHook[TrainRxEnGetAvgDlyPerNibble] = MemNTrainRxEnGetAvgDlyPerNibbleOr;
  NBPtr->FamilySpecificHook[InitPerNibbleTrn] = MemNInitPerNibbleTrnOr;
  NBPtr->FamilySpecificHook[TrainingNibbleZero] = MemNTrainingNibbleZeroOr;
  NBPtr->FamilySpecificHook[BeforeSetCsTri] = MemNBeforeSetCsTriOr;
  NBPtr->FamilySpecificHook[AdjustRdDqsDlyOffset] = MemNAdjustRdDqsDlyOffsetUnb;
  NBPtr->FamilySpecificHook[EnableParityAfterMemRst] = MemNEnableParityAfterMemRstOr;
  NBPtr->FamilySpecificHook[GetDdrMaxRate] = MemNGetMaxDdrRateUnb;
  NBPtr->FamilySpecificHook[ProgOdtControl] = MemNProgOdtControlOr;
  NBPtr->FamilySpecificHook[SetSkewMemClk] = MemNSetSkewMemClkUnb;
  NBPtr->FamilySpecificHook[ReleaseNbPstate] = MemNReleaseNbPstateOr;
  NBPtr->FamilySpecificHook[InitializeRxEnSeedlessTraining] = MemNInitializeRxEnSeedlessTrainingUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrNoWindBLError] = MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrSmallWindBLError] = MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb;
  NBPtr->FamilySpecificHook[InitialzeRxEnSeedlessByteLaneError] = MemNInitialzeRxEnSeedlessByteLaneErrorUnb;
  NBPtr->FamilySpecificHook[AdjustWrDqsBeforeSeedScaling] = MemNAdjustWrDqsBeforeSeedScalingOr;
  NBPtr->FamilySpecificHook[Adjust2DPhaseMaskBasedOnEcc] = MemNAdjust2DPhaseMaskBasedOnEccUnb;

  PackageType = LibAmdGetPackageType (&(NBPtr->MemPtr->StdHeader));
  if (PackageType == PACKAGE_TYPE_AM3r2) {
    // AM3r2 does not support 1.35V
    NBPtr->IsSupported[PerformanceOnly] = TRUE;

    // AM3r2 does not support Dll shutdown
    NBPtr->IsSupported[SetDllShutDown] = FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the default values in the MEM_DATA_STRUCT
 *
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *
 */
VOID
MemNInitDefaultsOR (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Socket;
  UINT8 Channel;
  MEM_PARAMETER_STRUCT *RefPtr;
  ASSERT (MemPtr != NULL);
  RefPtr = MemPtr->ParameterListPtr;

  // Memory Map/Mgt.
  // Mask Bottom IO with 0xF8 to force hole size to have granularity of 128MB
  RefPtr->BottomIo = 0xE0;
  RefPtr->UmaMode = UserOptions.CfgUmaMode;
  RefPtr->UmaSize = UserOptions.CfgUmaSize;
  RefPtr->MemHoleRemapping = TRUE;
  RefPtr->LimitMemoryToBelow1Tb = UserOptions.CfgLimitMemoryToBelow1Tb;
  //


  // Dram Timing
  RefPtr->UserTimingMode = UserOptions.CfgTimingModeSelect;
  RefPtr->MemClockValue = UserOptions.CfgMemoryClockSelect;
  for (Socket = 0; Socket < MAX_SOCKETS_SUPPORTED; Socket++) {
    for (Channel = 0; Channel < MAX_CHANNELS_PER_SOCKET; Channel++) {
      MemPtr->SocketList[Socket].ChannelPtr[Channel] = NULL;
      MemPtr->SocketList[Socket].TimingsPtr[Channel] = NULL;
    }
  }

  // Memory Clear
  RefPtr->EnableMemClr = TRUE;

  // TableBasedAlterations
  RefPtr->TableBasedAlterations = NULL;

  // Platform config table
  RefPtr->PlatformMemoryConfiguration = DefaultPlatformMemoryConfiguration;

  // Memory Restore
  RefPtr->MemRestoreCtl = FALSE;
  RefPtr->SaveMemContextCtl = FALSE;
  AmdS3ParamsInitializer (&RefPtr->MemContext);

  // Dram Configuration
  RefPtr->EnableBankIntlv = UserOptions.CfgMemoryEnableBankInterleaving;
  RefPtr->EnableNodeIntlv = UserOptions.CfgMemoryEnableNodeInterleaving;
  RefPtr->EnableChannelIntlv = UserOptions.CfgMemoryChannelInterleaving;
  RefPtr->EnableBankSwizzle = UserOptions.CfgBankSwizzle;
  RefPtr->EnableParity = UserOptions.CfgMemoryParityEnable;
  RefPtr->EnableOnLineSpareCtl = UserOptions.CfgOnlineSpare;

  // Dram Power
  RefPtr->EnablePowerDown = UserOptions.CfgMemoryPowerDown;

  // ECC
  RefPtr->EnableEccFeature = UserOptions.CfgEnableEccFeature;

  // Vref
  RefPtr->ExternalVrefCtl = UserOptions.CfgExternalVrefCtlFeature;

  //Training Mode
  RefPtr->ForceTrainMode = UserOptions.CfgForceTrainMode;
}
/*-----------------------------------------------------------------------------*/
/**
 *
 *      This function writes training pattern
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] Pattern[] - Pattern to write
 *     @param[in] Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
MemNWritePatternOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  )
{
  Address = MemUSetUpperFSbase (Address, NBPtr->MemPtr);
  MemUWriteCachelines (Address, Pattern, ClCount);
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *      This function reads training pattern
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] Buffer[] - Buffer to fill
 *     @param[in] Address - System Address [47:16]
 *     @param[in] ClCount - Number of cache lines
 *
 */

VOID
MemNReadPatternOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  Address = MemUSetUpperFSbase (Address, NBPtr->MemPtr);
  MemUReadCachelines (Buffer, Address, ClCount);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates DQS training for Unified NB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
memNEnableTrainSequenceOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN Retval;
  Retval = TRUE;
  if (!MemNIsIdSupportedOr (NBPtr, &(NBPtr->MemPtr->DiesPerSystem[NBPtr->MCTPtr->NodeId].LogicalCpuid))) {
    Retval = FALSE;
  }
  return Retval;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function save the MR0 value sent to memory during initialization
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       MrsAddress   - MR0 value to be saved
 *     @return         none
 */
VOID
MemNSaveMR0Or (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 MrsAddress
  )
{
  AGESA_STATUS Status;
  LOCATE_HEAP_PTR LocateHeapStructPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT32 ChipSel;
  MR0_DATA_ARRAY_PTR pMR0Data;

  ChipSel = NBPtr->GetBitField (NBPtr, BFMrsChipSel);
  LocateHeapStructPtr.BufferHandle = AMD_MEM_S3_MR0_DATA_HANDLE;
  LocateHeapStructPtr.BufferPtr = NULL;
  Status = HeapLocateBuffer (&LocateHeapStructPtr, &NBPtr->MemPtr->StdHeader);
  if (Status == AGESA_SUCCESS) {
    // MR0 data already present in heap
    pMR0Data = (MR0_DATA_ARRAY_PTR) (LocateHeapStructPtr.BufferPtr);
    ASSERT (pMR0Data != NULL);
  } else {
    AllocHeapParams.RequestedBufferSize = sizeof (MR0_DATA_STRUCT) * MAX_NODES_SUPPORTED_OR * MAX_DCTS_PER_NODE_OR;
    AllocHeapParams.BufferHandle = AMD_MEM_S3_MR0_DATA_HANDLE;
    AllocHeapParams.Persist = HEAP_SYSTEM_MEM;

    //
    // Allocate data buffer in heap
    //
    Status = HeapAllocateBuffer (&AllocHeapParams, &NBPtr->MemPtr->StdHeader);
    ASSERT (Status == AGESA_SUCCESS);
    pMR0Data = (MR0_DATA_ARRAY_PTR) (AllocHeapParams.BufferPtr);
    ASSERT (pMR0Data != NULL);
    LibAmdMemFill (pMR0Data, 0, sizeof (MR0_DATA_STRUCT) * MAX_NODES_SUPPORTED_OR * MAX_DCTS_PER_NODE_OR, &NBPtr->MemPtr->StdHeader);
  }
  (*pMR0Data)[NBPtr->Node][NBPtr->Dct].MR0Value = (UINT16) MrsAddress;
  (*pMR0Data)[NBPtr->Node][NBPtr->Dct].ChipSelEnMap |= (((UINT16)1) << ChipSel);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tLog last MR0\n\t\tNode: %d, Dct: %d, CS: %d, MR0: %08X\n", NBPtr->Node, NBPtr->Dct, ChipSel, MrsAddress);
}
