/* $NoKeywords:$ */
/**
 * @file
 *
 * mnon.c
 *
 * Common Northbridge functions for ON
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 48511 $ @e \$Date: 2011-03-09 13:53:13 -0700 (Wed, 09 Mar 2011) $
 *
 **/
/*
 *****************************************************************************
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
#include "mnon.h"
#include "mu.h"
#include "merrhdl.h"
#include "S3.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNON_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
CONST MEM_FREQ_CHANGE_PARAM FreqChangeParamON = {0x1838, 2, 3, 10, 2, 9, 665, 1000};
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
MemConstructNBBlockON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  CONST IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  )
{
  INT32 i;
  DIE_STRUCT *MCTPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  //
  // Determine if this is the expected NB Type
  //
  GetLogicalIdOfSocket (MemPtr->DiesPerSystem->SocketId, &(MemPtr->DiesPerSystem->LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedON (NBPtr, &(MemPtr->DiesPerSystem->LogicalCpuid))) {
    return FALSE;
  }

  NBPtr->MemPtr = MemPtr;
  NBPtr->RefPtr = MemPtr->ParameterListPtr;
  NBPtr->SharedPtr = SharedPtr;

  MCTPtr = MemPtr->DiesPerSystem;
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->MCTPtr->NodeId = 0;
  NBPtr->PciAddr.AddressValue = MCTPtr->PciAddr.AddressValue;
  NBPtr->VarMtrrHiMsk = GetVarMtrrHiMsk (&(MemPtr->DiesPerSystem->LogicalCpuid), &(MemPtr->StdHeader));

  //
  // Allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
  //
  AllocHeapParams.RequestedBufferSize = (sizeof (DCT_STRUCT) + sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK));
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, NodeID, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE); // Could not allocate buffer for DCT_STRUCTs and CH_DEF_STRUCTs
    return FALSE;
  }

  MCTPtr->Dct = 0;
  MCTPtr->DctCount = MAX_DCTS_PER_NODE_ON;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += sizeof (DCT_STRUCT);
  MCTPtr->DctData->ChannelCount = 1;
  MCTPtr->DctData->ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += sizeof (CH_DEF_STRUCT);
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;

  //
  // Initialize Socket List
  //
  *(MemPtr->SocketList[MCTPtr->SocketId].ChannelPtr) = MCTPtr->DctData->ChData;
  *(MemPtr->SocketList[MCTPtr->SocketId].TimingsPtr) = &(MCTPtr->DctData->Timings);
  MCTPtr->DctData->ChData->ChannelID = 0;

  //
  // Initialize NB block member variables
  //

  NBPtr->DCTPtr = NBPtr->MCTPtr->DctData;
  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;
  NBPtr->ChannelPtr = NBPtr->DCTPtr->ChData;

  MemNInitNBRegTableON (NBPtr, NBPtr->NBRegTable);
  NBPtr->Node = 0;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_ON;
  NBPtr->ChannelCount = MAX_CHANNELS_PER_DCT_ON;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_ON;
  NBPtr->Ganged = FALSE;
  NBPtr->PosTrnPattern = POS_PATTERN_256B;
  NBPtr->MemCleared = FALSE;
  NBPtr->StartupSpeed = DDR800_FREQUENCY;
  NBPtr->RcvrEnDlyLimit = 0xFF;
  NBPtr->DefDctSelIntLvAddr = 3;
  NBPtr->FreqChangeParam = (MEM_FREQ_CHANGE_PARAM *) &FreqChangeParamON;
  NBPtr->NbFreqChgState = 0;

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);

  for (i = 0; i < EnumSize; i++) {
    NBPtr->IsSupported[i] = FALSE;
  }

  for (i = 0; i < NumberOfHooks; i++) {
    NBPtr->FamilySpecificHook[i] = (BOOLEAN (*) (MEM_NB_BLOCK*, VOID*)) memDefTrue;
  }

  FeatPtr->InitHwRxEn (NBPtr);


  NBPtr->SwitchDCT = (VOID (*) (MEM_NB_BLOCK*, UINT8)) memDefRet;
  NBPtr->SwitchChannel = (VOID (*) (MEM_NB_BLOCK*, UINT8)) memDefRet;
  NBPtr->GetBitField = MemNGetBitFieldNb;
  NBPtr->SetBitField = MemNSetBitFieldNb;
  NBPtr->SetMaxLatency = MemNSetMaxLatencyON;
  NBPtr->getMaxLatParams = MemNGetMaxLatParamsClientON;
  NBPtr->InitializeMCT = (BOOLEAN (*) (MEM_NB_BLOCK*)) memDefTrue;
  NBPtr->FinalizeMCT = MemNFinalizeMctON;
  NBPtr->SendMrsCmd = MemNSendMrsCmdON;
  NBPtr->sendZQCmd = MemNSendZQCmdNb;
  NBPtr->WritePattern = MemNWritePatternON;
  NBPtr->ReadPattern = MemNReadPatternON;
  NBPtr->GenHwRcvEnReads = (VOID (*) (MEM_NB_BLOCK*, UINT32)) memDefRet;

  NBPtr->CompareTestPattern = MemNCompareTestPatternNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternNb;
  NBPtr->InitMCT = MemNInitMCTNb;
  NBPtr->StitchMemory = MemNStitchMemoryON;
  NBPtr->AutoConfig = MemNAutoConfigON;
  NBPtr->PlatformSpec = MemNPlatformSpecUnb;
  NBPtr->DisableDCT = MemNDisableDCTNb;
  NBPtr->StartupDCT = MemNStartupDCTUnb;
  NBPtr->SyncTargetSpeed = MemNSyncTargetSpeedNb;
  NBPtr->MemNCapSpeedBatteryLife = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->ChangeFrequency = MemNChangeFrequencyUnb;
  NBPtr->RampUpFrequency = MemNRampUpFrequencyNb;
  NBPtr->ChangeNbFrequency = MemNChangeNbFrequencyNb;
  NBPtr->ProgramNbPsDependentRegs = MemNProgramNbPstateDependentRegistersClientNb;
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsClientNb;
  NBPtr->SyncDctsReady = (BOOLEAN (*) (MEM_NB_BLOCK *)) memDefTrue;
  NBPtr->HtMemMapInit = MemNHtMemMapInitON;
  NBPtr->SyncAddrMapToAllNodes = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->CpuMemTyping = MemNCPUMemTypingNb;
  NBPtr->UMAMemTyping = MemNUMAMemTypingNb;
  NBPtr->BeforeDqsTraining = MemNBeforeDQSTrainingON;
  NBPtr->AfterDqsTraining = MemNAfterDQSTrainingON;
  NBPtr->OtherTiming = MemNOtherTimingON;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelNb;
  NBPtr->TechBlockSwitch = MemNTechBlockSwitchON;
  NBPtr->SetEccSymbolSize = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->TrainingFlow = (VOID (*) (MEM_NB_BLOCK *)) memNTrainFlowControl[DDR3_TRAIN_FLOW];
  NBPtr->MinDataEyeWidth = MemNMinDataEyeWidthNb;
  NBPtr->PollBitField = MemNPollBitFieldNb;
  NBPtr->BrdcstCheck = MemNBrdcstCheckON;
  NBPtr->BrdcstSet = MemNSetBitFieldNb;
  NBPtr->GetTrainDly = MemNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemNSetTrainDlyNb;
  NBPtr->PhyFenceTraining = MemNPhyFenceTrainingUnb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->RankEnabled = MemNRankEnabledNb;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldON;
  NBPtr->MemNBeforeDramInitNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNBeforePlatformSpecNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNInitPhyComp = MemNInitPhyCompClientNb;
  NBPtr->MemNcmnGetSetTrainDly = MemNcmnGetSetTrainDlyClientNb;
  NBPtr->MemPPhyFenceTrainingNb = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  NBPtr->MemNPlatformSpecificFormFactorInitNb = MemNPlatformSpecificFormFactorInitON;
  NBPtr->MemNPFenceAdjustNb = (VOID (*) (MEM_NB_BLOCK *, UINT16 *)) memDefRet;
  NBPtr->GetTrainDlyParms = MemNGetTrainDlyParmsClientNb;
  NBPtr->TrainingPatternInit = MemNTrainingPatternInitNb;
  NBPtr->TrainingPatternFinalize = MemNTrainingPatternFinalizeNb;
  NBPtr->GetApproximateWriteDatDelay = MemNGetApproximateWriteDatDelayNb;
  NBPtr->CSPerChannel = MemNCSPerChannelON;
  NBPtr->CSPerDelay = MemNCSPerDelayNb;
  NBPtr->FlushPattern = MemNFlushPatternNb;
  NBPtr->GetUmaSize = MemNGetUmaSizeON;
  NBPtr->GetMemClkFreqId = MemNGetMemClkFreqIdClientNb;
  NBPtr->EnableSwapIntlvRgn = (VOID (*) (MEM_NB_BLOCK *, UINT32, UINT32)) memDefRet;
  NBPtr->ChangeNbFrequencyWrap = MemNChangeNbFrequencyWrapON;
  NBPtr->WaitXMemClks = MemNWaitXMemClksNb;
  NBPtr->MemNGetDramTerm = MemNGetDramTermNb;
  NBPtr->MemNGetDynDramTerm = MemNGetDynDramTermNb;
  NBPtr->MemNGetMR0CL = MemNGetMR0CLNb;
  NBPtr->MemNGetMR0WR = MemNGetMR0WRNb;
  NBPtr->MemNGetMR2CWL = MemNGetMR2CWLNb;
  NBPtr->AllocateC6Storage = MemNAllocateC6StorageClientNb;

  NBPtr->IsSupported[SetDllShutDown] = TRUE;
  NBPtr->IsSupported[CheckPhyFenceTraining] = TRUE;
  NBPtr->IsSupported[CheckSendAllMRCmds] = TRUE;
  NBPtr->IsSupported[CheckFindPSDct] = TRUE;
  NBPtr->IsSupported[CheckODTControls] = TRUE;
  NBPtr->IsSupported[FenceTrnBeforeDramInit] = TRUE;
  NBPtr->IsSupported[WLSeedAdjust] = TRUE;
  NBPtr->IsSupported[ReverseMaxRdLatTrain] = TRUE;
  NBPtr->IsSupported[DramSrHys] = TRUE;
  NBPtr->IsSupported[CheckMaxDramRate] = TRUE;
  NBPtr->IsSupported[AdjustTwr] = TRUE;
  NBPtr->IsSupported[UnifiedNbFence] = TRUE;
  NBPtr->IsSupported[ChannelPDMode] = TRUE;   // Erratum 435
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F14_ON_C0) != 0) {
    NBPtr->IsSupported[AdjustTrc] = TRUE;
  }

  NBPtr->FamilySpecificHook[OverrideRcvEnSeed] = MemNOverrideRcvEnSeedON;
  NBPtr->FamilySpecificHook[BeforePhyFenceTraining] = MemNBeforePhyFenceTrainingClientNb;
  NBPtr->FamilySpecificHook[AdjustTxpdll] = MemNAdjustTxpdllClientNb;
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F14_ON_Cx) == 0) {
    // Do not do phase B enforcement for Rev C
  NBPtr->FamilySpecificHook[ForceRdDqsPhaseB] = MemNForceRdDqsPhaseBON;
  }
  NBPtr->FamilySpecificHook[SetDqsODT] = MemNSetDqsODTON;
  NBPtr->FamilySpecificHook[ResetRxFifoPtr] = MemNResetRxFifoPtrON;
  NBPtr->FamilySpecificHook[BfAfExcludeDimm] = MemNBfAfExcludeDimmClientNb;
  NBPtr->FamilySpecificHook[BeforeMemClkFreqVal] = MemNBeforeMemClkFreqValON;

  FeatPtr->InitCPG (NBPtr);
  FeatPtr->InitEarlySampleSupport (NBPtr);
  NBPtr->FeatPtr = FeatPtr;
  //
  // Calculate SPD Offsets per channel and assign pointers
  // to the data.
  //
  NBPtr->MCTPtr->DctData->ChData->SpdPtr = MemPtr->SpdDataStructure;

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
MemNInitDefaultsON (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Socket;
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

  // Dram Timing
  RefPtr->UserTimingMode = UserOptions.CfgTimingModeSelect;
  RefPtr->MemClockValue = UserOptions.CfgMemoryClockSelect;
  for (Socket = 0; Socket < MAX_SOCKETS_SUPPORTED; Socket++) {
    MemPtr->SocketList[Socket].ChannelPtr[0] = NULL;
    MemPtr->SocketList[Socket].TimingsPtr[0] = NULL;
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
  RefPtr->EnableChannelIntlv = FALSE;
  RefPtr->EnableBankSwizzle = UserOptions.CfgBankSwizzle;
  RefPtr->EnableParity = FALSE;
  RefPtr->EnableOnLineSpareCtl = FALSE;

  // Dram Power
  RefPtr->EnablePowerDown = UserOptions.CfgMemoryPowerDown;

  // ECC
  RefPtr->EnableEccFeature = FALSE;
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
MemNWritePatternON (
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
MemNReadPatternON (
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
memNEnableTrainSequenceON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BOOLEAN Retval;
  Retval = TRUE;
  if (!MemNIsIdSupportedON (NBPtr, &(NBPtr->MemPtr->DiesPerSystem[NBPtr->MCTPtr->NodeId].LogicalCpuid))) {
    Retval = FALSE;
  }
  return Retval;
}
