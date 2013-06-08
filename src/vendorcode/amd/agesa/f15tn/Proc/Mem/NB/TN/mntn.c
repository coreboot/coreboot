/* $NoKeywords:$ */
/**
 * @file
 *
 * mntn.c
 *
 * Common Northbridge  functions for TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/TN)
 * @e \$Revision: 64574 $ @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "mm.h"
#include "mn.h"
#include "mntn.h"
#include "mu.h"
#include "S3.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_NB_TN_MNTN_FILECODE
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
MemNRegAccessFenceTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

/**
 * Array for frequency change related parameters.
 */
CONST MEM_FREQ_CHANGE_PARAM FreqChangeParamTN = {0x0190, 0, 0, 0, 0, 0, 0, 0};

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
MemConstructNBBlockTN (
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
  if (!MemNIsIdSupportedTN (NBPtr, &(MemPtr->DiesPerSystem->LogicalCpuid))) {
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
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_TN * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_TN * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK) + sizeof (CH_TIMING_STRUCT))
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

  MCTPtr->DctCount = MAX_DCTS_PER_NODE_TN;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_TN * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_TN;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].Dct = Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_TN * sizeof (CH_DEF_STRUCT);
    MCTPtr->DctData[Dct].TimingsMemPs1 = (CH_TIMING_STRUCT *) AllocHeapParams.BufferPtr;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_TN * sizeof (CH_TIMING_STRUCT);
  }
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;

  //
  // Initialize Socket List
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    MemPtr->SocketList[MCTPtr->SocketId].ChannelPtr[(MCTPtr->DieId * 2) + Dct] = &(MCTPtr->DctData[Dct].ChData[0]);
    MemPtr->SocketList[MCTPtr->SocketId].TimingsPtr[(MCTPtr->DieId * 2) + Dct] = &(MCTPtr->DctData[Dct].Timings);
    MCTPtr->DctData[Dct].ChData[0].ChannelID = (MCTPtr->DieId * 2) + Dct;
  }

  MemNInitNBDataTN (NBPtr);

  FeatPtr->InitCPG (NBPtr);
  FeatPtr->InitHwRxEn (NBPtr);
  FeatPtr->excel221 (NBPtr);

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
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_TN; Dct++) {
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_TN; Channel++) {
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
MemNInitNBDataTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;

  MemNInitNBRegTableTN (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_TN;
  NBPtr->ChannelCount = MAX_CHANNELS_PER_DCT_TN;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_TN;
  NBPtr->Ganged = FALSE;
  NBPtr->PosTrnPattern = POS_PATTERN_256B;
  NBPtr->MemCleared = FALSE;
  NBPtr->StartupSpeed = DDR667_FREQUENCY;
  NBPtr->RcvrEnDlyLimit = 0x1FF;
  NBPtr->DefDctSelIntLvAddr = 4;
  NBPtr->NbFreqChgState = 0;
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &FreqChangeParamTN;
  NBPtr->MaxRxEnSeedTotal = 0x1FF;
  NBPtr->MinRxEnSeedGross = 0;
  NBPtr->CsRegMsk = 0x7FF8FFE0;
  NBPtr->RdDqsDlyRetrnStat = RDDQSDLY_RTN_NEEDED;
  NBPtr->MemPstate = MEMORY_PSTATE0;
  NBPtr->MemPstateStage = MEMORY_PSTATE_1ST_STAGE;
  NBPtr->CsPerChannel = MAX_CS_PER_CHANNEL_TN;
  NBPtr->CsPerDelay = 1;
  NBPtr->TotalMaxVrefRange = 0x20;
  NBPtr->TotalRdDQSDlyRange = 0x40;
  NBPtr->PhaseLaneMask = 0x3FFFF;
  NBPtr->MaxDiamondStep = 3;

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);
  MemNInitNBDataNb (NBPtr);

  NBPtr->SetMaxLatency = MemNSetMaxLatencyTN;
  NBPtr->getMaxLatParams = MemNGetMaxLatParamsTN;
  NBPtr->InitializeMCT = MemNInitializeMctTN;
  NBPtr->FinalizeMCT = MemNFinalizeMctTN;
  NBPtr->SendMrsCmd = MemNSendMrsCmdUnb;
  NBPtr->sendZQCmd = MemNSendZQCmdNb;
  NBPtr->WritePattern = MemNWritePatternTN;
  NBPtr->ReadPattern = MemNReadPatternTN;
  NBPtr->GenHwRcvEnReads = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;
  NBPtr->CompareTestPattern = MemNCompareTestPatternNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternNb;
  NBPtr->StitchMemory = MemNStitchMemoryNb;
  NBPtr->AutoConfig = MemNAutoConfigTN;
  NBPtr->PlatformSpec = MemNPlatformSpecUnb;
  NBPtr->InitMCT = MemNInitMCTNb;
  NBPtr->DisableDCT = MemNDisableDCTUnb;
  NBPtr->StartupDCT = MemNStartupDCTUnb;
  NBPtr->SyncTargetSpeed = MemNSyncTargetSpeedNb;
  NBPtr->ChangeFrequency = MemNChangeFrequencyUnb;
  NBPtr->RampUpFrequency = MemNRampUpFrequencyUnb;
  NBPtr->ChangeNbFrequency = MemNChangeNbFrequencyUnb;
  NBPtr->ChangeNbFrequencyWrap = MemNChangeNbFrequencyWrapUnb;
  NBPtr->ProgramNbPsDependentRegs = MemNProgramNbPstateDependentRegistersTN;
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsUnb;
  NBPtr->SyncDctsReady = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  NBPtr->HtMemMapInit = MemNHtMemMapInitTN;
  NBPtr->SyncAddrMapToAllNodes = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->CpuMemTyping = MemNCPUMemTypingNb;
  NBPtr->BeforeDqsTraining = MemNBeforeDQSTrainingTN;
  NBPtr->AfterDqsTraining = MemNAfterDQSTrainingTN;
  NBPtr->OtherTiming = MemNOtherTimingTN;
  NBPtr->UMAMemTyping = MemNUMAMemTypingNb;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelNb;
  NBPtr->TechBlockSwitch = MemNTechBlockSwitchTN;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldTN;
  NBPtr->SetEccSymbolSize = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->TrainingFlow = MemNTrainingFlowUnb;
  NBPtr->PollBitField = MemNPollBitFieldNb;
  NBPtr->BrdcstCheck = MemNBrdcstCheckNb;
  NBPtr->BrdcstSet = MemNBrdcstSetNb;
  NBPtr->GetTrainDly = MemNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemNSetTrainDlyNb;
  NBPtr->PhyFenceTraining = MemNPhyFenceTrainingUnb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->RankEnabled = MemNRankEnabledNb;
  NBPtr->MemNBeforeDramInitNb = MemNBeforeDramInitTN;
  NBPtr->MemNcmnGetSetTrainDly = MemNcmnGetSetTrainDlyUnb;
  NBPtr->MemPPhyFenceTrainingNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNInitPhyComp = MemNInitPhyCompTN;
  NBPtr->MemNBeforePlatformSpecNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNPlatformSpecificFormFactorInitNb = MemNPlatformSpecificFormFactorInitTblDrvNb;
  NBPtr->MemNPFenceAdjustNb = MemNPFenceAdjustTN;
  NBPtr->GetTrainDlyParms = MemNGetTrainDlyParmsUnb;
  NBPtr->TrainingPatternInit = MemNTrainingPatternInitNb;
  NBPtr->TrainingPatternFinalize = MemNTrainingPatternFinalizeNb;
  NBPtr->GetApproximateWriteDatDelay = MemNGetApproximateWriteDatDelayNb;
  NBPtr->FlushPattern = MemNFlushPatternNb;
  NBPtr->MinDataEyeWidth = MemNMinDataEyeWidthNb;
  NBPtr->MemNCapSpeedBatteryLife = MemNCapSpeedBatteryLifeTN;
  NBPtr->GetUmaSize = MemNGetUmaSizeTN;
  NBPtr->GetMemClkFreqId = MemNGetMemClkFreqIdUnb;
  NBPtr->EnableSwapIntlvRgn = MemNEnableSwapIntlvRgnNb;
  NBPtr->WaitXMemClks = MemNWaitXMemClksNb;
  NBPtr->MemNGetDramTerm = MemNGetDramTermTblDrvNb;
  NBPtr->MemNGetDynDramTerm = MemNGetDynDramTermTblDrvNb;
  NBPtr->MemNGetMR0CL = MemNGetMR0CLTblDrvNb;
  NBPtr->MemNGetMR0WR = MemNGetMR0WRTblDrvNb;
  NBPtr->MemNSaveMR0 = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;
  NBPtr->MemNGetMR2CWL = MemNGetMR2CWLUnb;
  NBPtr->AllocateC6Storage = MemNAllocateC6StorageTN;
  NBPtr->MemNBeforePlatformSpecNb = MemNBeforePlatformSpecTN;
  NBPtr->MemNGetMemoryWidth = MemNGetMemoryWidthUnb;

  NBPtr->IsSupported[SetDllShutDown] = TRUE;
  NBPtr->IsSupported[CheckMaxDramRate] = TRUE;
  NBPtr->IsSupported[CheckPhyFenceTraining] = TRUE;
  NBPtr->IsSupported[CheckSendAllMRCmds] = TRUE;
  NBPtr->IsSupported[CheckFindPSOverideWithSocket] = TRUE;
  NBPtr->IsSupported[FenceTrnBeforeDramInit] = TRUE;
  NBPtr->IsSupported[UnifiedNbFence] = TRUE;
  NBPtr->IsSupported[CheckODTControls] = TRUE;
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

  NBPtr->FamilySpecificHook[ExitPhyAssistedTraining] = MemNExitPhyAssistedTrainingTN;
  NBPtr->FamilySpecificHook[DCTSelectSwitch] = MemNDctCfgSelectUnb;
  NBPtr->FamilySpecificHook[AfterSaveRestore] = MemNAfterSaveRestoreUnb;
  NBPtr->FamilySpecificHook[OverrideRcvEnSeed] = MemNOverrideRcvEnSeedTN;
  NBPtr->FamilySpecificHook[OverrideWLSeed] = MemNOverrideWLSeedTN;
  NBPtr->FamilySpecificHook[CalcWrDqDqsEarly] = MemNCalcWrDqDqsEarlyUnb;
  NBPtr->FamilySpecificHook[AdjustRdDqsDlyOffset] = MemNAdjustRdDqsDlyOffsetUnb;
  NBPtr->FamilySpecificHook[GetDdrMaxRate] = MemNGetMaxDdrRateUnb;
  NBPtr->FamilySpecificHook[SetSkewMemClk] = MemNSetSkewMemClkUnb;
  NBPtr->FamilySpecificHook[AfterMemClkFreqVal] = MemNAdjustPllLockTimeTN;
  NBPtr->FamilySpecificHook[AdjustCSIntLvLowAddr] = MemNCSIntLvLowAddrAdjTN;
  NBPtr->FamilySpecificHook[ReleaseNbPstate] = MemNReleaseNbPstateTN;
  NBPtr->FamilySpecificHook[InitializeRxEnSeedlessTraining] = MemNInitializeRxEnSeedlessTrainingUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrNoWindBLError] = MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrSmallWindBLError] = MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb;
  NBPtr->FamilySpecificHook[InitialzeRxEnSeedlessByteLaneError] = MemNInitialzeRxEnSeedlessByteLaneErrorUnb;
  NBPtr->FamilySpecificHook[MemPstateStageChange] = MemNMemPstateStageChangeTN;
  NBPtr->FamilySpecificHook[ProgramFence2RxDll] = MemNProgramFence2RxDllTN;
  NBPtr->FamilySpecificHook[RdDqsDlyRestartChk] = MemNRdDqsDlyRestartChkTN;
  NBPtr->FamilySpecificHook[BeforeWrDatTrn] = MemNHookBfWrDatTrnTN;
  NBPtr->FamilySpecificHook[RegAccessFence] = MemNRegAccessFenceTN;
  NBPtr->FamilySpecificHook[AdjustWrDqsBeforeSeedScaling] = MemNAdjustWrDqsBeforeSeedScalingUnb;
  NBPtr->FamilySpecificHook[WLMR1] = MemNWLMR1TN;
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
MemNInitDefaultsTN (
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
MemNWritePatternTN (
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
MemNReadPatternTN (
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
 *   This function initiates DQS training for TN
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
memNEnableTrainSequenceTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN Retval;
  Retval = TRUE;
  if (!MemNIsIdSupportedTN (NBPtr, &(NBPtr->MemPtr->DiesPerSystem[NBPtr->MCTPtr->NodeId].LogicalCpuid))) {
    Retval = FALSE;
  }
  return Retval;
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
MemNRegAccessFenceTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // If subsequent writes to this array are scheduled, such as when writing several byte lanes during dram
  // training, then it is recommended to issue a dummy register read to ensure the last write.
  NBPtr->GetTrainDly (NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (0, 0));

  return TRUE;
}
