/**
 * @file
 *
 * mnNi.c
 *
 * Common Northbridge functions for Nile
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/NI)
 * @e \$Revision: 11190 $ @e \$Date: 2009-03-02 10:39:45 -0600 (Mon, 02 Mar 2009) $
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
#include "mnda.h"
#include "mnNi.h"
#include "mu.h"
#include "S3.h"
#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_NI_MNNI_FILECODE


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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;
extern PSO_ENTRY DefaultPlatformMemoryConfiguration[];

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
MemConstructNBBlockNi (
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
  if (!MemNIsIdSupportedDA (NBPtr, &(MemPtr->DiesPerSystem[NodeID].LogicalCpuid))) {
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
  AllocHeapParams.RequestedBufferSize = MAX_DCTS_PER_NODE_DA * (
                                          sizeof (DCT_STRUCT) + (
                                            MAX_CHANNELS_PER_DCT_DA * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK))
                                          )
                                        );
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, NodeID, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_DCT_STRUCT_AND_CH_DEF_STRUCTs, NBPtr->Node, 0, 0, 0, &MemPtr->StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    return FALSE;
  }

  MCTPtr->DctCount = MAX_DCTS_PER_NODE_DA;
  MCTPtr->DctData = (DCT_STRUCT *) AllocHeapParams.BufferPtr;
  AllocHeapParams.BufferPtr += MAX_DCTS_PER_NODE_DA * sizeof (DCT_STRUCT);
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_DA; Dct++) {
    MCTPtr->DctData[Dct].Dct = Dct;
    MCTPtr->DctData[Dct].ChannelCount = MAX_CHANNELS_PER_DCT_DA;
    MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    MCTPtr->DctData[Dct].ChData[0].Dct = Dct;
    AllocHeapParams.BufferPtr += MAX_CHANNELS_PER_DCT_DA * sizeof (CH_DEF_STRUCT);
  }
  NBPtr->PSBlock = (MEM_PS_BLOCK *) AllocHeapParams.BufferPtr;

  //
  // Initialize Socket List
  //
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_DA; Dct++) {
    MemPtr->SocketList[MCTPtr->SocketId].ChannelPtr[Dct] = &(MCTPtr->DctData[Dct].ChData[0]);
    MemPtr->SocketList[MCTPtr->SocketId].TimingsPtr[Dct] = &(MCTPtr->DctData[Dct].Timings);
    MCTPtr->DctData[Dct].ChData[0].ChannelID = Dct;
  }

  MemNInitNBDataDA (NBPtr);

  FeatPtr->InitCPG (NBPtr);
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
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_DA; Dct++) {
    for (Channel = 0; Channel < MAX_CHANNELS_PER_DCT_DA; Channel++) {
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
MemNInitNBDataNi (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT32 i;

  NBPtr->DctCachePtr = NBPtr->DctCache;
  NBPtr->PsPtr = NBPtr->PSBlock;

  InitNBRegTableDA (NBPtr->NBRegTable);
  NBPtr->Node = ((UINT8) NBPtr->PciAddr.Address.Device) - 24;
  NBPtr->Dct = 0;
  NBPtr->Channel = 0;
  NBPtr->DctCount = MAX_DCTS_PER_NODE_DA;
  NBPtr->ChannelCount = MAX_CHANNELS_PER_DCT_DA;
  NBPtr->NodeCount = MAX_NODES_SUPPORTED_DA;
  NBPtr->Ganged = FALSE;
  NBPtr->PosTrnPattern = POS_PATTERN_72B;
  NBPtr->MemCleared = FALSE;
  NBPtr->StartupSpeed = DDR800_FREQUENCY;
  NBPtr->RcvrEnDlyLimit = 0xFF;
  NBPtr->DefDctSelIntLvAddr = 3;

  for (i = 0; i < EnumSize; i++) {
    NBPtr->IsSupported[i] = FALSE;
  }

  LibAmdMemFill (NBPtr->DctCache, 0, sizeof (NBPtr->DctCache), &NBPtr->MemPtr->StdHeader);

  NBPtr->SetMaxLatency = MemNSetMaxLatencyNb;
  NBPtr->getMaxLatParams = MemNGetMaxLatParamsNb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->InitializeMCT = MemNInitializeMctNb;
  NBPtr->FinalizeMCT = MemNFinalizeMctDA;
  NBPtr->SendMrsCmd = MemNSendMrsCmdDA;
  NBPtr->sendZQCmd = MemNSendZQCmdNb;
  NBPtr->WritePattern = MemNWritePatternDA;
  NBPtr->ReadPattern = MemNReadPatternDA;
  NBPtr->GenHwRcvEnReads = (VOID (*) (MEM_NB_BLOCK*, UINT32)) memDefRet;
  NBPtr->CompareTestPattern = MemNCompareTestPatternNb;
  NBPtr->InsDlyCompareTestPattern = MemNInsDlyCompareTestPatternNb;
  NBPtr->StitchMemory = MemNStitchMemoryNb;
  NBPtr->AutoConfig = memNAutoConfigDA;
  NBPtr->PlatformSpec = MemNPlatformSpecNb;
  NBPtr->InitMCT = MemNInitMCTNb;
  NBPtr->DisableDCT = MemNDisableDCTNb;
  NBPtr->StartupDCT = MemNStartupDCTNb;
  NBPtr->SyncTargetSpeed = MemNSyncTargetSpeedNb;
  NBPtr->ChangeFrequency = MemNChangeFrequencyNb;
  NBPtr->RampUpFrequency = MemNRampUpFrequencyNb;
  NBPtr->ChangeNbFrequency = MemNDefFalseNb;
  NBPtr->ProgramCycTimings = MemNProgramCycTimingsNb;
  NBPtr->SyncDctsReady = MemNSyncDctsReadyNb;
  NBPtr->HtMemMapInit = MemNHtMemMapInitNb;
  NBPtr->SyncAddrMapToAllNodes = MemNSyncAddrMapToAllNodesNb;
  NBPtr->CpuMemTyping = MemNCPUMemTypingNb;
  NBPtr->BeforeDqsTraining = MemNBeforeDQSTrainingNb;
  NBPtr->AfterDqsTraining = (VOID (*) (MEM_NB_BLOCK*)) memDefRet;
  NBPtr->OtherTiming = MemNOtherTimingDA;
  NBPtr->UMAMemTyping = MemNUMAMemTypingNb;
  NBPtr->TechBlockSwitch = MemNTechBlockSwitchNb;
  NBPtr->MemNCmnGetSetFieldNb = MemNCmnGetSetFieldDA;
  NBPtr->SetEccSymbolSize = (VOID (*) (MEM_NB_BLOCK*)) memDefRet;
  NBPtr->TrainingFlow = MemNTrainingFlowNb;
  NBPtr->MinDataEyeWidth = MemNMinDataEyeWidthNb;
  MemNInitNBDataNb (NBPtr);
  NBPtr->PollBitField = MemNPollBitFieldNb;
  NBPtr->BrdcstCheck = MemNBrdcstCheckNb;
  NBPtr->BrdcstSet = MemNBrdcstSetNb;
  NBPtr->GetTrainDly = MemNGetTrainDlyNb;
  NBPtr->SetTrainDly = MemNSetTrainDlyNb;
  NBPtr->PhyFenceTraining = MemNPhyFenceTrainingNb;
  NBPtr->GetSysAddr = MemNGetMCTSysAddrNb;
  NBPtr->RankEnabled = MemNRankEnabledNb;
  NBPtr->MemPPhyFenceTrainingNb = MemNTrainPhyFenceNb;
  NBPtr->MemNPlatformSpecificFormFactorInitNb = MemNPlatformSpecificFormFactorInitDA;
  NBPtr->MemNBeforePlatformSpecNb = MemNBeforePlatformSpecDA;
  NBPtr->MemNcmnGetSetTrainDly = MemNcmnGetSetTrainDlyNb;
  NBPtr->MemPNodeMemBoundaryNb = MemPNodeMemBoundaryDA;
  NBPtr->MemNInitPhyComp = MemNInitPhyCompNb;
  NBPtr->GetSocketRelativeChannel = MemNGetSocketRelativeChannelNb;
  NBPtr->MemNBeforeDramInitNb = MemNBeforeDramInitDA;
  NBPtr->MemNPFenceAdjustNb = (VOID (*) (MEM_NB_BLOCK *, UINT16 *)) memDefRet;
  NBPtr->GetTrainDlyParms = MemNGetTrainDlyParmsNb;
  NBPtr->TrainingPatternInit = MemNTrainingPatternInitNb;
  NBPtr->TrainingPatternFinalize = MemNTrainingPatternFinalizeNb;
  NBPtr->GetApproximateWriteDatDelay = MemNGetApproximateWriteDatDelayNb;
  NBPtr->CSPerChannel = MemNCSPerChannelNb;
  NBPtr->CSPerDelay = MemNCSPerDelayNb;
  NBPtr->FlushPattern = MemNFlushPatternNb;
  NBPtr->GetUmaSize = MemNGetUmaSizeNb;
  NBPtr->GetMemClkFreqId = MemNGetMemClkFreqIdNb;
  NBPtr->MemNCapSpeedBatteryLife = MemNCapSpeedBatteryLifeDA;
  NBPtr->EnableSwapIntlvRgn = MemNEnableSwapIntlvRgnNb;

  NBPtr->IsSupported[SetSpareEn] = TRUE;
  NBPtr->IsSupported[CheckSpareEn] = TRUE;
  NBPtr->IsSupported[SetDllShutDown] = TRUE;
  NBPtr->IsSupported[DimmBasedOnSpeed] = TRUE;
  NBPtr->IsSupported[CheckMaxDramRate] = TRUE;
  NBPtr->IsSupported[Check1GAlign] = TRUE;
  NBPtr->IsSupported[CheckMaxRdDqsDlyPtr] = TRUE;
  NBPtr->IsSupported[CheckPhyFenceTraining] = TRUE;
  NBPtr->IsSupported[CheckGetMCTSysAddr] = TRUE;
  NBPtr->IsSupported[CheckFindPSDct] = TRUE;
  NBPtr->IsSupported[CheckDllStdBy] = TRUE;
  NBPtr->IsSupported[CheckDynamicDramTerm] = TRUE;
  NBPtr->IsSupported[CheckSlewWithMarginImprv] = TRUE;
  NBPtr->IsSupported[CheckDllSpeedUp] = TRUE;
  NBPtr->IsSupported[CheckDllRegDis] = TRUE;
  NBPtr->IsSupported[ForceEnMemHoleRemapping] = TRUE;
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
MemNInitDefaultsNi (
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
MemNWritePatternNi (
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
MemNReadPatternNi (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  )
{
  Address = MemUSetUpperFSbase (Address, NBPtr->MemPtr);
  MemUReadCachelines (Buffer, Address, ClCount);
}

