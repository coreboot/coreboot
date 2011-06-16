/* $NoKeywords:$ */
/**
 * @file
 *
 * mnln.c
 *
 * Common Northbridge functions for LN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/LN)
 * @e \$Revision: 48400 $ @e \$Date: 2011-03-08 16:28:12 +0800 (Tue, 08 Mar 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright (c) 2011, Advanced Micro Devices, Inc.
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
#include "mnln.h"
#include "mu.h"
#include "S3.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_LN_MNLN_FILECODE
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
CONST MEM_FREQ_CHANGE_PARAM FreqChangeParamLN = {0x0190, 7, 7, 14, 3, 18, 470, 946};
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

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
 *   This function initializes the northbridge block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *     @param[in]        *FeatPtr  - Pointer to the MEM_FEAT_BLOCK_NB
 *     @param[in]        *SharedPtr - Pointer to the MEM_SHARED_DATA
 *     @param[in]        NodeID  - UINT8 indicating node ID of the NB object.
 *
 *     @retval     Boolean indicating that this is the correct memory
 *                 controller type for the node number that was passed in.
 */

BOOLEAN
MemConstructNBBlockLN (
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
  if (!MemNIsIdSupportedLN (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
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
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_LN * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_LN * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK))
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

  MCTPtr->DctCount = MAX_DCTS_PER_NODE_LN;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_LN * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_LN;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].Dct = Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_LN * sizeof (CH_DEF_STRUCT);
  }
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;


  //
  // Initialize Socket List
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    MemPtr->SocketList[MCTPtr->SocketId].ChannelPtr[Dct] = &(MCTPtr->DctData[Dct].ChData[0]);
    MemPtr->SocketList[MCTPtr->SocketId].TimingsPtr[Dct] = &(MCTPtr->DctData[Dct].Timings);
    MCTPtr->DctData[Dct].ChData[0].ChannelID = Dct;
  }

  //
  // Initialize NB block member variables
  //
  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;

  MemNInitNBRegTableLN (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = 0;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_LN;
  NBPtr->ChannelCount = MAX_CHANNELS_PER_DCT_LN;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_LN;
  NBPtr->Ganged = FALSE;
  NBPtr->PosTrnPattern = POS_PATTERN_256B;
  NBPtr->MemCleared = FALSE;
  NBPtr->StartupSpeed = DDR800_FREQUENCY;
  NBPtr->RcvrEnDlyLimit = 0x1FF;
  NBPtr->NbFreqChgState = 0;
  NBPtr->DefDctSelIntLvAddr = 5;
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &FreqChangeParamLN;
  NBPtr->CsRegMsk = 0x1FF83FE0;
  NBPtr->MaxRxEnSeedTotal = 0x33F;
  NBPtr->MinRxEnSeedGross = 0;

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);

  NBPtr->SetMaxLatency = MemNSetMaxLatencyLN;
  NBPtr->getMaxLatParams = MemNGetMaxLatParamsClientLN;
  NBPtr->InitializeMCT = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  NBPtr->FinalizeMCT = MemNFinalizeMctLN;
  NBPtr->SendMrsCmd = MemNSendMrsCmdLN;
  NBPtr->sendZQCmd = MemNSendZQCmdNb;
  NBPtr->WritePattern = MemNWritePatternLN;
  NBPtr->ReadPattern = MemNReadPatternLN;
  NBPtr->GenHwRcvEnReads = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;

  NBPtr->CompareTestPattern = MemNCompareTestPatternNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternNb;
  NBPtr->InitMCT = MemNInitMCTNb;
  NBPtr->StitchMemory = MemNStitchMemoryNb;
  NBPtr->AutoConfig = MemNAutoConfigLN;
  NBPtr->PlatformSpec = MemNPlatformSpecUnb;
  NBPtr->DisableDCT = MemNDisableDCTClientNb;
  NBPtr->StartupDCT = MemNStartupDCTUnb;
  NBPtr->SyncTargetSpeed = MemNSyncTargetSpeedNb;
  NBPtr->MemNCapSpeedBatteryLife = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->ChangeFrequency = MemNChangeFrequencyClientNb;
  NBPtr->RampUpFrequency = MemNRampUpFrequencyNb;
  NBPtr->ChangeNbFrequency = MemNChangeNbFrequencyNb;
  NBPtr->ProgramNbPsDependentRegs = MemNProgramNbPstateDependentRegistersClientNb;
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsClientNb;
  NBPtr->SyncDctsReady = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  NBPtr->HtMemMapInit = MemNHtMemMapInitLN;
  NBPtr->SyncAddrMapToAllNodes = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->CpuMemTyping = MemNCPUMemTypingNb;
  NBPtr->UMAMemTyping = MemNUMAMemTypingNb;
  NBPtr->BeforeDqsTraining = MemNBeforeDQSTrainingLN;
  NBPtr->AfterDqsTraining = MemNAfterDQSTrainingLN;
  NBPtr->OtherTiming = MemNOtherTimingLN;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelNb;
  NBPtr->TechBlockSwitch = MemNTechBlockSwitchLN;
  NBPtr->SetEccSymbolSize = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->TrainingFlow = (VOID (*) (MEM_NB_BLOCK *))(memNTrainFlowControl[DDR3_TRAIN_FLOW]);
  NBPtr->MinDataEyeWidth = MemNMinDataEyeWidthNb;
  NBPtr->ChangeNbFrequencyWrap = MemNChangeNbFrequencyWrapLN;
  NBPtr->AllocateC6Storage = MemNAllocateC6StorageClientNb;

  MemNInitNBDataNb (NBPtr);
  FeatPtr->InitHwRxEn (NBPtr);

  NBPtr->PollBitField = MemNPollBitFieldNb;
  NBPtr->BrdcstCheck = MemNBrdcstCheckNb;
  NBPtr->BrdcstSet = MemNBrdcstSetNb;
  NBPtr->GetTrainDly = MemNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemNSetTrainDlyNb;
  NBPtr->PhyFenceTraining = MemNPhyFenceTrainingUnb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->RankEnabled = MemNRankEnabledNb;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldLN;
  NBPtr->MemNBeforeDramInitNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNBeforePlatformSpecNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNInitPhyComp = MemNInitPhyCompClientNb;
  NBPtr->MemNcmnGetSetTrainDly = MemNcmnGetSetTrainDlyClientNb;
  NBPtr->MemPPhyFenceTrainingNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNPlatformSpecificFormFactorInitNb = MemNPlatformSpecificFormFactorInitLN;
  NBPtr->MemNPFenceAdjustNb = MemNPFenceAdjustUnb;
  NBPtr->GetTrainDlyParms = MemNGetTrainDlyParmsClientNb;
  NBPtr->TrainingPatternInit = MemNTrainingPatternInitNb;
  NBPtr->TrainingPatternFinalize = MemNTrainingPatternFinalizeNb;
  NBPtr->GetApproximateWriteDatDelay = MemNGetApproximateWriteDatDelayNb;
  NBPtr->CSPerChannel = MemNCSPerChannelLN;
  NBPtr->CSPerDelay = MemNCSPerDelayNb;
  NBPtr->FlushPattern = MemNFlushPatternNb;
  NBPtr->GetUmaSize = MemNGetUmaSizeLN;
  NBPtr->GetMemClkFreqId = MemNGetMemClkFreqIdClientNb;
  NBPtr->EnableSwapIntlvRgn = MemNEnableSwapIntlvRgnLN;
  NBPtr->WaitXMemClks = MemNWaitXMemClksNb;
  NBPtr->MemNGetDramTerm = MemNGetDramTermNb;
  NBPtr->MemNGetDynDramTerm = MemNGetDynDramTermNb;
  NBPtr->MemNGetMR0CL = MemNGetMR0CLNb;
  NBPtr->MemNGetMR0WR = MemNGetMR0WRLN;
  NBPtr->MemNSaveMR0 = (VOID (*) (MEM_NB_BLOCK *, UINT32)) memDefRet;
  NBPtr->MemNGetMR2CWL = MemNGetMR2CWLNb;

  NBPtr->IsSupported[SetDllShutDown] = TRUE;
  NBPtr->IsSupported[CheckPhyFenceTraining] = TRUE;
  NBPtr->IsSupported[CheckSendAllMRCmds] = TRUE;
  NBPtr->IsSupported[CheckFindPSDct] = TRUE;
  NBPtr->IsSupported[FenceTrnBeforeDramInit] = TRUE;
  NBPtr->IsSupported[WLSeedAdjust] = TRUE;
  NBPtr->IsSupported[UnifiedNbFence] = TRUE;
  NBPtr->IsSupported[CheckODTControls] = TRUE;
  NBPtr->IsSupported[ReverseMaxRdLatTrain] = TRUE;
  NBPtr->IsSupported[SkipErrTrain] = TRUE;
  NBPtr->IsSupported[DramSrHys] = TRUE;
  NBPtr->IsSupported[CheckMaxDramRate] = TRUE;
  NBPtr->IsSupported[SchedDlySlot1Extra] = TRUE;
  NBPtr->IsSupported[CsrPhyPllPdEn] = TRUE;
  NBPtr->IsSupported[AdjustTrc] = TRUE;
  NBPtr->IsSupported[ProgramCsrComparator] = TRUE;
  NBPtr->IsSupported[CheckDrvImpCtrl] = TRUE;
  NBPtr->IsSupported[EnProcOdtAdvForUDIMM] = TRUE;

  NBPtr->FamilySpecificHook[AddlMaxRdLatTrain] = MemNSlot1MaxRdLatTrainClientNb;
  NBPtr->FamilySpecificHook[BeforePhyFenceTraining] = MemNBeforePhyFenceTrainingClientNb;
  NBPtr->FamilySpecificHook[ReEnablePhyComp] = MemNReEnablePhyCompNb;
  NBPtr->FamilySpecificHook[AdjustTxpdll] = MemNAdjustTxpdllClientNb;
  NBPtr->FamilySpecificHook[DisLowPwrDrvStr] = MemNDisLowPwrDrvStrLN;
  NBPtr->FamilySpecificHook[CalcWrDqDqsEarly] = MemNCalcWrDqDqsEarlyClientNb;
  NBPtr->FamilySpecificHook[InitializeRxEnSeedlessTraining] = MemNInitializeRxEnSeedlessTrainingUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrNoWindBLError] = MemNTrackRxEnSeedlessRdWrNoWindBLErrorUnb;
  NBPtr->FamilySpecificHook[TrackRxEnSeedlessRdWrSmallWindBLError] = MemNTrackRxEnSeedlessRdWrSmallWindBLErrorUnb;
  NBPtr->FamilySpecificHook[InitialzeRxEnSeedlessByteLaneError] = MemNInitialzeRxEnSeedlessByteLaneErrorUnb;
  NBPtr->FamilySpecificHook[OverridePrevPassRcvEnDly] = MemNOverridePrevPassRcvEnDlyLN;
  NBPtr->FamilySpecificHook[ResetRxFifoPtr] = MemNResetRxFifoPtrClientNb;
  NBPtr->FamilySpecificHook[BfAfExcludeDimm] = MemNBfAfExcludeDimmClientNb;

  FeatPtr->InitCPG (NBPtr);
  FeatPtr->InitEarlySampleSupport (NBPtr);

  NBPtr->FeatPtr = FeatPtr;

  //
  // Calculate SPD Offsets per channel and assign pointers
  // to the data.
  //
  SpdSocketIndex = GetSpdSocketIndex (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, &MemPtr->StdHeader);
  //
  // Traverse the Dct/Channel structures
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_LN; Channel++) {
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

  MemNSwitchDCTNb (NBPtr, 0);
  NBPtr->Channel = 0;

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initializes the default values in the MEM_DATA_STRUCT
 *
 *     @param[in,out]   *MemPtr  - Pointer to the MEM_DATA_STRUCT
 *
 *     @retval      None
 */
VOID
MemNInitDefaultsLN (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Socket;
  UINT8 Channel;
  MEM_PARAMETER_STRUCT *RefPtr;
  AGESA_TESTPOINT (TpProcMemBeforeMemDataInit, &(MemPtr->StdHeader));
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
  RefPtr->EnableNodeIntlv = FALSE;
  RefPtr->EnableChannelIntlv = UserOptions.CfgMemoryChannelInterleaving;
  RefPtr->EnableBankSwizzle = UserOptions.CfgBankSwizzle;
  RefPtr->EnableParity = FALSE;
  RefPtr->EnableOnLineSpareCtl = FALSE;

  // Dram Power
  RefPtr->EnablePowerDown = UserOptions.CfgMemoryPowerDown;

  // ECC
  RefPtr->EnableEccFeature = UserOptions.CfgEnableEccFeature;
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
MemNWritePatternLN (
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
MemNReadPatternLN (
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
 *   This function initiates DQS training for Client NB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
memNEnableTrainSequenceLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN Retval;
  Retval = TRUE;
  if (!MemNIsIdSupportedLN (NBPtr, &(NBPtr->MemPtr->DiesPerSystem[NBPtr->MCTPtr->NodeId].LogicalCpuid))) {
    Retval = FALSE;
  }
  return Retval;
}
