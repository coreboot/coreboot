/* $NoKeywords:$ */
/**
 * @file
 *
 * mnkb.c
 *
 * Common Northbridge  functions for KB
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/KB)
 * @e \$Revision: 87494 $ @e \$Date: 2013-02-04 12:06:47 -0600 (Mon, 04 Feb 2013) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "PlatformMemoryConfiguration.h"
#include "mm.h"
#include "mn.h"
#include "mnkb.h"
#include "mu.h"
#include "S3.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "IdsF16KbAllService.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_NB_KB_MNKB_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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
BOOLEAN
STATIC
MemNRegAccessFenceKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

/**
 * Array for frequency change related parameters.
 */
CONST MEM_FREQ_CHANGE_PARAM FreqChangeParamKB = {0x0190, 0, 0, 0, 0, 0, 0, 0};

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
MemConstructNBBlockKB (
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
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem->SocketId, &(MemPtr->DiesPerSystem->LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedKB (&(MemPtr->DiesPerSystem->LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->RefPtr = MemPtr->ParameterListPtr;
  NBPtr->SharedPtr = SharedPtr;

  MCTPtr = MemPtr->DiesPerSystem;
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->MCTPtr->NodeId = NodeID;
  NBPtr->PciAddr.AddressValue = MCTPtr->PciAddr.AddressValue;
  NBPtr->VarMtrrHiMsk = GetVarMtrrHiMsk (&(MemPtr->DiesPerSystem->LogicalCpuid), &(MemPtr->StdHeader));

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_KB * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_KB * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK) + sizeof (CH_TIMING_STRUCT))
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

  MCTPtr->DctCount = MAX_DCTS_PER_NODE_KB;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_KB * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_KB;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].Dct = Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_KB * sizeof (CH_DEF_STRUCT);
    MCTPtr->DctData[Dct].TimingsMemPs1 = (CH_TIMING_STRUCT *) AllocHeapParams.BufferPtr;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_KB * sizeof (CH_TIMING_STRUCT);
  }
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;

  //
  // Initialize Socket List
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    MemPtr->SocketList[MCTPtr->SocketId].ChannelPtr[(MCTPtr->DieId * 2) + Dct] = &(MCTPtr->DctData[Dct].ChData[0]);
    MemPtr->SocketList[MCTPtr->SocketId].TimingsPtr[(MCTPtr->DieId * 2) + Dct] = &(MCTPtr->DctData[Dct].Timings);
    MCTPtr->DctData[Dct].ChData[0].ChannelID = (MCTPtr->DieId * 2) + Dct;
  }

  MemNInitNBDataKB (NBPtr);

  FeatPtr->InitCPG (NBPtr);
  FeatPtr->InitHwRxEn (NBPtr);
  FeatPtr->InitEarlySampleSupport (NBPtr);
  FeatPtr->InitRdWr2DTraining (NBPtr);

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
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_KB; Dct++) {
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_KB; Channel++) {
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

  if (MemNGetBitFieldNb (NBPtr, BFMemPstateDis) == 1) {
    // MemPstate is disabled
    NBPtr->MemPstateStage = 0;
  }

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
MemNInitNBDataKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;

  MemNInitNBRegTableKB (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_KB;
  NBPtr->ChannelCount = MAX_CHANNELS_PER_DCT_KB;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_KB;
  NBPtr->Ganged = FALSE;
  NBPtr->PosTrnPattern = POS_PATTERN_256B;
  NBPtr->MemCleared = FALSE;
  NBPtr->StartupSpeed = DDR667_FREQUENCY;
  NBPtr->RcvrEnDlyLimit = 0x1FF;
  NBPtr->DefDctSelIntLvAddr = 4;
  NBPtr->NbFreqChgState = 0;
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &FreqChangeParamKB;
  NBPtr->MaxRxEnSeedTotal = 0x1FF;
  NBPtr->MinRxEnSeedGross = 0;
  NBPtr->CsRegMsk = 0x7FF8FFE0;
  NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_NEEDED;
  NBPtr->MemPstate = MEMORY_PSTATE0;
  NBPtr->MemPstateStage = MEMORY_PSTATE_1ST_STAGE;
  NBPtr->CsPerChannel = MAX_CS_PER_CHANNEL_KB;
  NBPtr->CsPerDelay = 1;
  NBPtr->RdDqsDlyForMaxRdLat = 0x1F;
  NBPtr->TotalMaxVrefRange = 0x20;
  NBPtr->TotalRdDQSDlyRange = 0x40;
  NBPtr->MaxSeedCount = MAX_2D_DQS_SEED_COUNT;
  NBPtr->TotalBitTimes2DRdTraining = TOTAL_BIT_TIMES_2D_RD_TRAINING_KB;
  NBPtr->TotalBitTimes2DWrTraining = TOTAL_BIT_TIMES_2D_WR_TRAINING_KB;
  NBPtr->PhaseLaneMask = 0x3FFFF;
  NBPtr->MaxDiamondStep = 3;
  NBPtr->DiamondWidthRd = 9;
  NBPtr->DiamondWidthWr = 9;
  NBPtr->RdRolloverMultiple = 8;


  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);
  MemNInitNBDataNb (NBPtr);

  NBPtr->SetMaxLatency = MemNSetMaxLatencyKB;
  NBPtr->getMaxLatParams = MemNGetMaxLatParamsKB;
  NBPtr->InitializeMCT = MemNInitializeMctKB;
  NBPtr->FinalizeMCT = MemNFinalizeMctKB;
  NBPtr->SendMrsCmd = MemNSendMrsCmdUnb;
  NBPtr->sendZQCmd = MemNSendZQCmdNb;
  NBPtr->WritePattern = MemNWritePatternKB;
  NBPtr->ReadPattern = MemNReadPatternKB;
  NBPtr->GenHwRcvEnReads = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;
  NBPtr->CompareTestPattern = MemNCompareTestPatternNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternNb;
  NBPtr->StitchMemory = MemNStitchMemoryNb;
  NBPtr->AutoConfig = MemNAutoConfigKB;
  NBPtr->PlatformSpec = MemNPlatformSpecUnb;
  NBPtr->InitMCT = MemNInitMCTNb;
  NBPtr->DisableDCT = MemNDisableDCTUnb;
  NBPtr->StartupDCT = MemNStartupDCTUnb;
  NBPtr->SyncTargetSpeed = MemNSyncTargetSpeedNb;
  NBPtr->ChangeFrequency = MemNChangeFrequencyUnb;
  NBPtr->RampUpFrequency = MemNRampUpFrequencyUnb;
  NBPtr->ChangeNbFrequency = MemNChangeNbFrequencyUnb;
  NBPtr->ChangeNbFrequencyWrap = MemNChangeNbFrequencyWrapUnb;
  NBPtr->GetMemClkFreqInCurrentContext = MemNGetMemClkFreqInCurrentContextKB;
  NBPtr->ProgramNbPsDependentRegs = MemNProgramNbPstateDependentRegistersKB;
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsKB;
  NBPtr->SyncDctsReady = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  NBPtr->HtMemMapInit = MemNHtMemMapInitKB;
  NBPtr->SyncAddrMapToAllNodes = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->CpuMemTyping = MemNCPUMemTypingNb;
  NBPtr->BeforeDqsTraining = MemNBeforeDQSTrainingKB;
  NBPtr->AfterDqsTraining = MemNAfterDQSTrainingKB;
  NBPtr->OtherTiming = MemNOtherTimingKB;
  NBPtr->UMAMemTyping = MemNUMAMemTypingNb;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelNb;
  NBPtr->TechBlockSwitch = MemNTechBlockSwitchKB;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldKB;
  NBPtr->SetEccSymbolSize = MemNSetEccSymbolSizeNb;
  NBPtr->TrainingFlow = MemNTrainingFlowUnb;
  NBPtr->PollBitField = MemNPollBitFieldNb;
  NBPtr->BrdcstCheck = MemNBrdcstCheckNb;
  NBPtr->BrdcstSet = MemNBrdcstSetNb;
  NBPtr->GetTrainDly = MemNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemNSetTrainDlyNb;
  NBPtr->PhyFenceTraining = MemNPhyFenceTrainingUnb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->RankEnabled = MemNRankEnabledNb;
  NBPtr->MemNBeforeDramInitNb = MemNBeforeDramInitKB;
  NBPtr->MemNcmnGetSetTrainDly = MemNcmnGetSetTrainDlyUnb;
  NBPtr->MemPPhyFenceTrainingNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNInitPhyComp = MemNInitPhyCompKB;
  NBPtr->MemNBeforePlatformSpecNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNPlatformSpecificFormFactorInitNb = MemNPlatformSpecificFormFactorInitTblDrvNb;
  NBPtr->MemNPFenceAdjustNb = MemNPFenceAdjustKB;
  NBPtr->GetTrainDlyParms = MemNGetTrainDlyParmsUnb;
  NBPtr->TrainingPatternInit = MemNTrainingPatternInitNb;
  NBPtr->TrainingPatternFinalize = MemNTrainingPatternFinalizeNb;
  NBPtr->GetApproximateWriteDatDelay = MemNGetApproximateWriteDatDelayNb;
  NBPtr->FlushPattern = MemNFlushPatternNb;
  NBPtr->MinDataEyeWidth = MemNMinDataEyeWidthNb;
  NBPtr->MemNCapSpeedBatteryLife = MemNCapSpeedBatteryLifeKB;
  NBPtr->GetUmaSize = MemNGetUmaSizeKB;
  NBPtr->GetMemClkFreqId = MemNGetMemClkFreqIdUnb;
  NBPtr->EnableSwapIntlvRgn = (VOID (*) (MEM_NB_BLOCK *, UINT32, UINT32)) memDefRet;
  NBPtr->WaitXMemClks = MemNWaitXMemClksNb;
  NBPtr->MemNGetDramTerm = MemNGetDramTermTblDrvNb;
  NBPtr->MemNGetDynDramTerm = MemNGetDynDramTermTblDrvNb;
  NBPtr->MemNGetMR0CL = MemNGetMR0CLTblDrvNb;
  NBPtr->MemNGetMR0WR = MemNGetMR0WRTblDrvNb;
  NBPtr->MemNSaveMR0 = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;
  NBPtr->MemNGetMR2CWL = MemNGetMR2CWLUnb;
  NBPtr->AllocateC6Storage = MemNAllocateC6StorageKB;
  NBPtr->MemNBeforePlatformSpecNb = MemNBeforePlatformSpecKB;
  NBPtr->AgressorContinuousWrites = MemNAgressorContinuousWritesUnb;
  NBPtr->GetPrbs2dRdDqsSeed = (UINT32 (*) (struct _MEM_NB_BLOCK *NBPtr , UINT8 SeedCount)) memDefRet;
  NBPtr->InPhaseCompareRdDqs2DPattern = (UINT32 (*) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount)) memDefRet;
  NBPtr->Phase180CompareRdDqs2DPattern = (UINT32 (*) (struct _MEM_NB_BLOCK *NBPtr, UINT8 Buffer[], UINT8 Pattern[], UINT16 ByteCount)) memDefRet;
  NBPtr->InitializeRdDqs2dVictimContinuousWrites = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->FinalizeRdDqs2dVictimContinuousWrites = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->InitializeRdDqs2dVictimChipSelContinuousWrites = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->StartRdDqs2dVictimContinuousWrites = (VOID (*) (struct _MEM_NB_BLOCK *NBPtr , UINT8 SeedCount)) memDefRet;
  NBPtr->MemN2DRdDQSDataCollection = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNGetMemoryWidth = MemNGetMemoryWidthUnb;
  NBPtr->MemNDisableScrubber = MemNDisableScrubberKB;
  NBPtr->MemNRestoreScrubber = MemNRestoreScrubberKB;
  NBPtr->MemNSetEccExclusionRange = MemNSetEccExclusionRangeUnb;

  NBPtr->IsSupported[SetDllShutDown] = TRUE;
  NBPtr->IsSupported[CheckEccDLLPwrDnConfig] = TRUE;
  NBPtr->IsSupported[CheckMaxDramRate] = TRUE;
  NBPtr->IsSupported[CheckPhyFenceTraining] = TRUE;
  NBPtr->IsSupported[CheckSendAllMRCmds] = TRUE;
  NBPtr->IsSupported[CheckFindPSOverideWithSocket] = TRUE;
  NBPtr->IsSupported[FenceTrnBeforeDramInit] = TRUE;
  NBPtr->IsSupported[UnifiedNbFence] = TRUE;
  NBPtr->IsSupported[CheckODTControls] = TRUE;
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
  NBPtr->IsSupported[AdjustTrp] = TRUE; // erratum 638
  NBPtr->IsSupported[ForcePhyToM0] = TRUE;
  NBPtr->IsSupported[SwitchRdDqsDlyForMaxRdLatency] = TRUE;
  NBPtr->IsSupported[OptimizedPatternWrite2D] = TRUE;

  NBPtr->FamilySpecificHook[ExitPhyAssistedTraining] = MemNExitPhyAssistedTrainingKB;
  NBPtr->FamilySpecificHook[DCTSelectSwitch] = MemNDctCfgSelectUnb;
  NBPtr->FamilySpecificHook[AfterSaveRestore] = MemNAfterSaveRestoreUnb;
  NBPtr->FamilySpecificHook[OverrideRcvEnSeed] = MemNOverrideRcvEnSeedKB;
  NBPtr->FamilySpecificHook[OverrideWLSeed] = MemNOverrideWLSeedKB;
  NBPtr->FamilySpecificHook[CalcWrDqDqsEarly] = MemNCalcWrDqDqsEarlyKB;
  NBPtr->FamilySpecificHook[AdjustRdDqsDlyOffset] = MemNAdjustRdDqsDlyOffsetUnb;
  NBPtr->FamilySpecificHook[GetDdrMaxRate] = MemNGetMaxDdrRateUnb;
  NBPtr->FamilySpecificHook[AfterMemClkFreqVal] = MemNAdjustPllLockTimeKB;
  NBPtr->FamilySpecificHook[AdjustCSIntLvLowAddr] = MemNCSIntLvLowAddrAdjKB;
  NBPtr->FamilySpecificHook[ReleaseNbPstate] = MemNReleaseNbPstateKB;
  NBPtr->FamilySpecificHook[InitializeRxEnSeedlessTraining] = MemNInitializeRxEnSeedlessTrainingUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrNoWindBLError] = MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrSmallWindBLError] = MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb;
  NBPtr->FamilySpecificHook[InitialzeRxEnSeedlessByteLaneError] = MemNInitialzeRxEnSeedlessByteLaneErrorUnb;
  NBPtr->FamilySpecificHook[MemPstateStageChange] = MemNMemPstateStageChangeKB;
  NBPtr->FamilySpecificHook[ProgramFence2RxDll] = MemNProgramFence2RxDllKB;
  NBPtr->FamilySpecificHook[RdDqsDlyRestartChk] = MemNRdDqsDlyRestartChkKB;
  NBPtr->FamilySpecificHook[BeforeWrDatTrn] = MemNHookBfWrDatTrnKB;
  NBPtr->FamilySpecificHook[RegAccessFence] = MemNRegAccessFenceKB;
  NBPtr->FamilySpecificHook[AdjustWrDqsBeforeSeedScaling] = MemNAdjustWrDqsBeforeSeedScalingUnb;
  NBPtr->FamilySpecificHook[WLMR1] = MemNWLMR1KB;
  NBPtr->FamilySpecificHook[Adjust2DPhaseMaskBasedOnEcc] = MemNAdjust2DPhaseMaskBasedOnEccUnb;
  NBPtr->FamilySpecificHook[ProgramPOdtOff] = MemNProgramPOdtOffKB;
  NBPtr->FamilySpecificHook[RelocatePscTblEntryByMotherBoardLayer] = memNRelocatePscTableEntryByMotherBoardLayerKB;
  NBPtr->FamilySpecificHook[AdjustHwRcvEnSeedGross] = MemNAdjustHwRcvEnSeedGrossKB;
  NBPtr->FamilySpecificHook[ForceEccSymbolSize] = MemNForceEccSymbolSizeKB;
  NBPtr->FamilySpecificHook[AdjustRdDqsDlyForMaxRdLat] = MemNAdjustRdDqsDlyForMaxRdLatUnb;
  NBPtr->FamilySpecificHook[SetMaxRdLatBasedOnSeededRxEnDly] = MemNSetMaxRdLatBasedOnSeededRxEnDlyKB;
  NBPtr->FamilySpecificHook[DisableMemHoleMapping] = MemNDisableMemHoleMappingKB;
  NBPtr->FamilySpecificHook[RestoreMemHoleMapping] = MemNRestoreMemHoleMappingKB;
  NBPtr->FamilySpecificHook[PhyInitVref] = MemNPhyInitVrefKB;
  NBPtr->FamilySpecificHook[IntermediateMemclkFreqVal] = MemNIntermediateMemclkFreqValUnb;

  IEM_INSERT_CODE (IEM_NBBLOCK_INIT, IemInitNBDataOverrideKB, (NBPtr));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the default values in the MEM_DATA_STRUCT
 *
 *   NOTE: This function must first verify the NB Family.
 *
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *
 */
VOID
MemNInitDefaultsKB (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  MEM_PARAMETER_STRUCT *RefPtr;
  CPU_LOGICAL_ID LogicalCpuid;
  ASSERT (MemPtr != NULL);
  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfCurrentCore (&LogicalCpuid, &(MemPtr->StdHeader));
  if (MemNIsIdSupportedKB (&LogicalCpuid)) {
    RefPtr = MemPtr->ParameterListPtr;
    RefPtr->EnableNodeIntlv = FALSE;
    RefPtr->EnableChannelIntlv = FALSE;
    RefPtr->EnableParity = FALSE;
    RefPtr->EnableOnLineSpareCtl = FALSE;
  }
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
MemNWritePatternKB (
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
MemNReadPatternKB (
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
 *   This function initiates DQS training for KB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
memNEnableTrainSequenceKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN Retval;
  Retval = TRUE;
  if (!MemNIsIdSupportedKB (&(NBPtr->MemPtr->DiesPerSystem[NBPtr->MCTPtr->NodeId].LogicalCpuid))) {
    Retval = FALSE;
  }
  return Retval;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function obtains PSC table entry pointer by mother board layer design for KB
 *
 *     @param[in,out]   *NBPtr       - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *TblEntryPtr - Pointer to the PSC table entry
 *
 */

BOOLEAN
memNRelocatePscTableEntryByMotherBoardLayerKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *TblEntryPtr
  )
{
  PSC_TBL_ENTRY **TblEntry;
  UINT8 *SwitchCountPtr;
  UINT8 i;

  SwitchCountPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MOTHER_BOARD_LAYERS, 0, 0, 0, NULL, NULL);
  if (SwitchCountPtr != NULL) {
    //
    // Relocate the table entry pointer to the set for the current mother board design
    //
    TblEntry = *(PSC_TBL_ENTRY ***) TblEntryPtr;
    for (i = 0; i < *SwitchCountPtr; i ++) {
      while (*TblEntry != NULL) {
        TblEntry ++;
      }
      TblEntry ++;
    }
    *(PSC_TBL_ENTRY ***) TblEntryPtr = TblEntry;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function makes sure that previous phy register writes are done.
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 *
 */

BOOLEAN
STATIC
MemNRegAccessFenceKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // If subsequent writes to this array are scheduled, such as when writing several byte lanes during dram
  // training, then it is recommended to issue a dummy register read to ensure the last write.
  NBPtr->GetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (0, 0));

  return TRUE;
}

