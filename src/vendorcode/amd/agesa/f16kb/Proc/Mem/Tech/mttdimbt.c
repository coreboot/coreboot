/* $NoKeywords:$ */
/**
 * @file
 *
 * mttdimmbt.c
 *
 * Technology Dimm Based Training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_MTTDIMBT_FILECODE

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

VOID
STATIC
MemTInitDqsPos4RcvrEnByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );

VOID
STATIC
MemTSetRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly
  );

VOID
STATIC
MemTLoadRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  );

BOOLEAN
STATIC
MemTSaveRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT16 CmpResultRank0,
  IN       UINT16 CmpResultRank1
  );

VOID
STATIC
MemTResetDctWrPtrByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  );

UINT16
STATIC
MemTCompare1ClPatternByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[]
  );

VOID
STATIC
MemTSkipChipSelPass1Byte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *ChipSelPtr
  );

VOID
STATIC
MemTSkipChipSelPass2Byte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *ChipSelPtr
  );

UINT8
STATIC
MemTMaxByteLanesByte ( VOID );

UINT8
STATIC
MemTDlyTableWidthByte ( VOID );

VOID
STATIC
MemTSetDqsDelayCsrByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ByteLane,
  IN       UINT8 Dly
  );

VOID
STATIC
MemTDqsWindowSaveByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ByteLane,
  IN       UINT8 DlyMin,
  IN       UINT8 DlyMax
  );

BOOLEAN
STATIC
MemTFindMaxRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  );

UINT16
STATIC
MemTCompare1ClPatternOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT8 Side,
  IN       UINT8 Receiver,
  IN       BOOLEAN Side1En
  );

VOID
STATIC
MemTLoadRcvrEnDlyOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  );

VOID
STATIC
MemTSetRcvrEnDlyOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly
  );

VOID
STATIC
MemTLoadInitialRcvEnDlyOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  );

UINT8
STATIC
MemTFindMinMaxGrossDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       TRN_DLY_TYPE TrnDlyType,
  IN       BOOLEAN IfMax
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function enables byte based training if called
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTDimmByteTrainInit (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dct;
  UINT8 Channel;
  UINT8 DctCount;
  UINT8 ChannelCount;
  DIE_STRUCT *MCTPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MCTPtr = NBPtr->MCTPtr;

  ASSERT ((NBPtr->CsPerDelay == 1) || (NBPtr->CsPerDelay == 2));

  TechPtr->InitDQSPos4RcvrEn = MemTInitDqsPos4RcvrEnByte;
  TechPtr->SetRcvrEnDly = MemTSetRcvrEnDlyByte;
  TechPtr->LoadRcvrEnDly = MemTLoadRcvrEnDlyByte;
  TechPtr->SaveRcvrEnDly = MemTSaveRcvrEnDlyByte;
  TechPtr->SaveRcvrEnDlyFilter = MemTSaveRcvrEnDlyByteFilterOpt;
  TechPtr->ResetDCTWrPtr = MemTResetDctWrPtrByte;
  TechPtr->Compare1ClPattern = MemTCompare1ClPatternByte;
  TechPtr->SkipChipSelPass1 = MemTSkipChipSelPass1Byte;
  TechPtr->SkipChipSelPass2 = MemTSkipChipSelPass2Byte;
  TechPtr->MaxByteLanes = MemTMaxByteLanesByte;
  TechPtr->DlyTableWidth = MemTDlyTableWidthByte;
  TechPtr->SetDQSDelayCSR = MemTSetDqsDelayCsrByte;
  TechPtr->DQSWindowSave = MemTDqsWindowSaveByte;
  TechPtr->FindMaxDlyForMaxRdLat = MemTFindMaxRcvrEnDlyByte;
  TechPtr->Compare1ClPatternOpt = MemTCompare1ClPatternOptByte;
  TechPtr->LoadRcvrEnDlyOpt = MemTLoadRcvrEnDlyOptByte;
  TechPtr->SetRcvrEnDlyOpt = MemTSetRcvrEnDlyOptByte;
  TechPtr->InitializeVariablesOpt = MemTInitializeVariablesOptByte;
  TechPtr->GetMaxValueOpt = MemTGetMaxValueOptByte;
  TechPtr->SetSweepErrorOpt = MemTSetSweepErrorOptByte;
  TechPtr->CheckRcvrEnDlyLimitOpt = MemTCheckRcvrEnDlyLimitOptByte;
  TechPtr->LoadInitialRcvrEnDlyOpt = MemTLoadInitialRcvEnDlyOptByte;
  TechPtr->GetMinMaxGrossDly = MemTFindMinMaxGrossDlyByte;
  // Dynamically allocate buffers for storing trained timings.
  DctCount = MCTPtr->DctCount;
  ChannelCount = MCTPtr->DctData[0].ChannelCount;
  AllocHeapParams.RequestedBufferSize = ((DctCount * ChannelCount) *
                                         ((MAX_DIMMS * MAX_DELAYS * NUMBER_OF_DELAY_TABLES) +
                                          (MAX_DELAYS * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES) +
                                           (MAX_DIMMS * MAX_NUMBER_LANES)
                                          )
                                         );

  if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
    AllocHeapParams.RequestedBufferSize *= 2;
  }

  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_TRN_DATA_HANDLE, MCTPtr->NodeId, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &NBPtr->MemPtr->StdHeader) == AGESA_SUCCESS) {
    for (Dct = 0; Dct < DctCount; Dct++) {
      for (Channel = 0; Channel < ChannelCount; Channel++) {
        MCTPtr->DctData[Dct].ChData[Channel].RowCount = MAX_DIMMS;
        MCTPtr->DctData[Dct].ChData[Channel].ColumnCount = MAX_DELAYS;

        MCTPtr->DctData[Dct].ChData[Channel].RcvEnDlys = (UINT16 *) AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS) * 2;
        MCTPtr->DctData[Dct].ChData[Channel].WrDqsDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].RdDqsDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].WrDatDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].RdDqs2dDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_NUMBER_LANES);
        MCTPtr->DctData[Dct].ChData[Channel].RdDqsMinDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].RdDqsMaxDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].WrDatMinDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].WrDatMaxDlys = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
        MCTPtr->DctData[Dct].ChData[Channel].FailingBitMask = AllocHeapParams.BufferPtr;
        AllocHeapParams.BufferPtr += (MAX_CS_PER_CHANNEL * MAX_DELAYS);
        if (NBPtr->MemPstateStage == MEMORY_PSTATE_1ST_STAGE) {
          MCTPtr->DctData[Dct].ChData[Channel].RcvEnDlysMemPs1 = (UINT16 *) AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS) * 2;
          MCTPtr->DctData[Dct].ChData[Channel].WrDqsDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].WrDatDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].RdDqs2dDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_NUMBER_LANES);
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsMinDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsMaxDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].WrDatMinDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].WrDatMaxDlysMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_DIMMS * MAX_DELAYS);
          MCTPtr->DctData[Dct].ChData[Channel].FailingBitMaskMemPs1 = AllocHeapParams.BufferPtr;
          AllocHeapParams.BufferPtr += (MAX_CS_PER_CHANNEL * MAX_DELAYS);

        }
      }
    }
  } else {
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_DYN_STORING_OF_TRAINED_TIMINGS, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE);  // Could not dynamically allocate buffers for storing trained timings
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function initializes the DQS Positions in preparation for Receiver Enable Training.
 *  Write Position is no delay, Read Position is 1/2 Memclock delay
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
STATIC
MemTInitDqsPos4RcvrEnByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dimm;
  UINT8 ByteLane;
  UINT8 WrDqs;

  for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
    for (ByteLane = 0; ByteLane < MAX_DELAYS; ByteLane++) {
      WrDqs = TechPtr->NBPtr->ChannelPtr->WrDqsDlys[(Dimm * MAX_DELAYS) + ByteLane];
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessWrDatDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), WrDqs);
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRdDqsDly, DIMM_BYTE_ACCESS (Dimm, ByteLane), 0x3F);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function programs DqsRcvEnDly to additional index for DQS receiver enabled training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 */

VOID
STATIC
MemTSetRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly
  )
{
  UINT8 ByteLane;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  for (ByteLane = 0; ByteLane < MAX_BYTELANES; ByteLane++) {
    TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver >> 1, ByteLane), RcvEnDly);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function loads the DqsRcvEnDly from saved data and program to additional index
 *  for DQS receiver enabled training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *
 */

VOID
STATIC
MemTLoadRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  )
{
  UINT8 i;
  UINT8 Dimm;
  UINT16 Saved;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Dimm = Receiver >> 1;
  Saved = TechPtr->DqsRcvEnSaved;
  for (i = 0; i < MAX_BYTELANES; i++) {
    if (Saved & 1) {
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver >> 1, i),
      ChannelPtr->RcvEnDlys[Dimm * MAX_DELAYS + i]);
    }
    Saved >>= 1;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function saves passing DqsRcvEnDly values to the stack
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 *     @param[in]       CmpResultRank0 - compare result for Rank 0
 *     @param[in]       CmpResultRank1 - compare result for Rank 1
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
 */

BOOLEAN
STATIC
MemTSaveRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT16 CmpResultRank0,
  IN       UINT16 CmpResultRank1
  )
{
  UINT8 i;
  UINT8 Passed;
  UINT8 Saved;
  UINT8 Mask;
  UINT8 Dimm;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Passed = (UINT8) ((CmpResultRank0 & CmpResultRank1) & 0xFF);

  Saved = (UINT8) (TechPtr->DqsRcvEnSaved & Passed); //@attention - false passes filter (subject to be replaced with a better solution)
  Dimm = Receiver >> 1;
  Mask = 1;
  for (i = 0; i < MAX_BYTELANES; i++) {
    if (Passed & Mask) {
      if (!(Saved & Mask)) {
        ChannelPtr->RcvEnDlys[Dimm * MAX_DELAYS + i] = RcvEnDly + 0x20;     // @attention -1 pass only
        IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tBL %d = %02x", i, RcvEnDly + 0x20);
      }
      Saved |= Mask;
    }
    Mask <<= 1;
  }
  TechPtr->DqsRcvEnSaved = Saved;

  if (Saved == 0xFF) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function performs a filtering functionality and saves passing DqsRcvEnDly
 *  values to the stack
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 *     @param[in]       CmpResultRank0 - compare result for Rank 0
 *     @param[in]       CmpResultRank1 - compare result for Rank 1
 *
 *     @return  TRUE - All bytelanes pass
 *     @return  FALSE - Some bytelanes fail
 */

BOOLEAN
MemTSaveRcvrEnDlyByteFilter (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly,
  IN       UINT16 CmpResultRank0,
  IN       UINT16 CmpResultRank1
  )
{
  UINT8 i;
  UINT8 Passed;
  UINT8 Saved;
  UINT8 Mask;
  UINT8 Dimm;
  UINT8 MaxFilterDly;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_DCT_CACHE *DctCachePtr;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  DctCachePtr = TechPtr->NBPtr->DctCachePtr;

  MaxFilterDly = TechPtr->MaxFilterDly;
  Passed = (UINT8) ((CmpResultRank0 & CmpResultRank1) & 0xFF);

  Dimm = Receiver >> 1;
  Saved = (UINT8) TechPtr->DqsRcvEnSaved;
  Mask = 1;
  for (i = 0; i < MAX_BYTELANES; i++) {
    if ((Passed & Mask) != 0) {
      DctCachePtr->RcvEnDlyCounts [i] += 1;
      if ((Saved & Mask) == 0) {
        ChannelPtr->RcvEnDlys[Dimm * MAX_DELAYS + i] = RcvEnDly + 0x20;
        Saved |= Mask;
        IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tBL %d = %02x", i, RcvEnDly + 0x20);
      }
    } else {
      if (DctCachePtr->RcvEnDlyCounts [i] <= MaxFilterDly) {
        DctCachePtr->RcvEnDlyCounts [i] = 0;
        Saved &= ~Mask;
      }
    }
    Mask <<= 1;
  }

  //-----------------------
  TechPtr->DqsRcvEnSaved = (UINT16) Saved;

  Saved = 0;
  for (i = 0; i < MAX_BYTELANES; i++) {
    if (DctCachePtr->RcvEnDlyCounts [i] >= MaxFilterDly) {
      Saved |= (UINT8) 1 << i;
    }
  }

  if (Saved == 0xFF) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function compares test pattern with data in buffer and return a pass/fail bitmap
 *       for 8 Bytes
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *
 *     @return  PASS - Bit map of results of comparison
 */

UINT16
STATIC
MemTCompare1ClPatternByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[]
  )
{
  UINT16 i;
  UINT16 j;
  UINT16 Pass;
  DIE_STRUCT *MCTPtr;

  MCTPtr = TechPtr->NBPtr->MCTPtr;
  if (MCTPtr->GangedMode && MCTPtr->Dct) {
    j = 8;
  } else {
    j = 0;
  }

  Pass = 0xFFFF;
  IDS_HDT_CONSOLE (MEM_FLOW, " -");
  for (i = 0; i < 8; i++) {
    if (Buffer[j] != Pattern[j]) {
      // if bytelane n fails
      Pass &= ~((UINT16)1 << (j % 8));    // clear bit n
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "  %c", (Buffer[j] == Pattern[j]) ? 'P' : '.');
    j++;
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t        -");
    for (i = 0, j -= 8; i < 8; i++, j++) {
      IDS_HDT_CONSOLE (MEM_FLOW, " %02x", Buffer[j]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t        -");
    for (i = 0, j -= 8; i < 8; i++, j++) {
      IDS_HDT_CONSOLE (MEM_FLOW, " %02x", Pattern[j]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\n");
  );

  return Pass;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       The function resets the DCT input buffer write pointer.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver - Chip select
 *
 */

VOID
STATIC
MemTResetDctWrPtrByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  )
{
  UINT8 i;
  UINT16 RcvEnDly;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  for (i = 0; i < MAX_BYTELANES; i++) {
    RcvEnDly = (UINT16) TechPtr->NBPtr->GetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver / 2, i));
    TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver / 2, i), RcvEnDly);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function skips odd chip select if training at 800MT or above.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       *ChipSelPtr - Pointer to variable contains Chip select index
 *
 */

VOID
STATIC
MemTSkipChipSelPass1Byte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *ChipSelPtr
  )
{
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;
  // if the even chip select failed training, need to set CsTrainFail for odd chip select if present.
  if (NBPtr->DCTPtr->Timings.CsPresent & ((UINT16)1 << ((*ChipSelPtr) + 1))) {
    if (NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16)1 << *ChipSelPtr)) {
      NBPtr->DCTPtr->Timings.CsTrainFail |= (UINT16)1 << ((*ChipSelPtr) + 1);
      if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, NBPtr->DCTPtr->Timings.CsTrainFail, &NBPtr->MemPtr->StdHeader)) {
        ASSERT (FALSE);
      }
    }
  }
  (*ChipSelPtr)++;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  MemTSkipChipSelPass2Byte:
 *
 *       This function skips odd chip select if training at 800MT or above.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in,out]   *ChipSelPtr - Pointer to variable contains Chip select index
 *
 */

VOID
STATIC
MemTSkipChipSelPass2Byte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   UINT8 *ChipSelPtr
  )
{
  if (*ChipSelPtr & 1) {
    *ChipSelPtr = MAX_CS_PER_CHANNEL;    // skip all successions
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function determines the maximum number of byte lanes
 *
 *     @return  Max number of Bytelanes
 */

UINT8
STATIC
MemTMaxByteLanesByte ( VOID )
{
  return MAX_BYTELANES;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function determines the width of the delay tables (eg. RcvEnDlys, WrDqsDlys,...)
 *
 *     @return  Delay table width in bytes
 */

UINT8
STATIC
MemTDlyTableWidthByte ( VOID )
{
  return MAX_DELAYS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function writes the Delay value to a certain byte lane
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       ByteLane  -  Bytelane number being targeted
 *     @param[in]       Dly - Delay value
 *
 */

VOID
STATIC
MemTSetDqsDelayCsrByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ByteLane,
  IN       UINT8 Dly
  )
{
  UINT8 Reg;
  UINT8 Dimm;

  ASSERT (ByteLane <= MAX_BYTELANES);

  if (!(TechPtr->DqsRdWrPosSaved & ((UINT8)1 << ByteLane))) {
    Dimm = TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay;

    if (TechPtr->Direction == DQS_WRITE_DIR) {
      Dly = Dly + ((UINT8) TechPtr->NBPtr->ChannelPtr->WrDqsDlys[(Dimm * MAX_DELAYS) + ByteLane]);
      Reg = AccessWrDatDly;
    } else {
      Reg = AccessRdDqsDly;
    }

    TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, Reg, DIMM_BYTE_ACCESS (Dimm, ByteLane), Dly);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function programs the trained DQS delay for the specified byte lane
 *       and stores its DQS window for reference.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       ByteLane  -  Bytelane number being targeted
 *     @param[in]       DlyMin - Minimum delay value
 *     @param[in]       DlyMax- Maximum delay value
 *
 */

VOID
STATIC
MemTDqsWindowSaveByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 ByteLane,
  IN       UINT8 DlyMin,
  IN       UINT8 DlyMax
  )
{
  UINT8 DqsDelay;
  UINT8 Dimm;
  CH_DEF_STRUCT *ChanPtr;

  ASSERT (ByteLane <= MAX_BYTELANES);
  ChanPtr = TechPtr->NBPtr->ChannelPtr;

  DqsDelay = ((DlyMin + DlyMax + 1) / 2) & 0x3F;
  MemTSetDqsDelayCsrByte (TechPtr, ByteLane, DqsDelay);
  TechPtr->DqsRdWrPosSaved |= (UINT8)1 << ByteLane;
  TechPtr->DqsRdWrPosSaved |= 0xFF00;

  Dimm = (TechPtr->ChipSel / TechPtr->NBPtr->CsPerDelay) * MAX_DELAYS + ByteLane;
  if (TechPtr->Direction == DQS_READ_DIR) {
    ChanPtr->RdDqsDlys[Dimm] = DqsDelay;
  } else {
    ChanPtr->WrDatDlys[Dimm] = DqsDelay + ChanPtr->WrDqsDlys[Dimm];
  }

  if (TechPtr->Direction == DQS_READ_DIR) {
    ChanPtr->RdDqsMinDlys[ByteLane] = DlyMin;
    ChanPtr->RdDqsMaxDlys[ByteLane] = DlyMax;
  } else {
    ChanPtr->WrDatMinDlys[ByteLane] = DlyMin;
    ChanPtr->WrDatMaxDlys[ByteLane] = DlyMax;
  }

}


/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function finds the DIMM that has the largest receiver enable delay.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[out]      *ChipSel   - Pointer to the Chip select that has the largest receiver enable delay.
 *
 *     @return   TRUE - A chip select can be found.
 *     @return   FALSE - A chip select cannot be found.
 */

BOOLEAN
STATIC
MemTFindMaxRcvrEnDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  )
{
  UINT8  ChipSelect;
  UINT8  ByteLane;
  UINT16 RcvEnDly;
  UINT16 MaxDly;
  UINT8  MaxDlyCs;
  BOOLEAN RetVal;

  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  RetVal = FALSE;
  MaxDly = 0;
  MaxDlyCs = 0;
  for (ChipSelect = 0; ChipSelect < NBPtr->CsPerChannel; ChipSelect = ChipSelect + NBPtr->CsPerDelay) {
    if ((NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSelect)) != 0) {
      if ((NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSelect)) == 0) {
        // Only choose the dimm that does not fail training
        for (ByteLane = 0; ByteLane < ((NBPtr->MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8); ByteLane++) {
          RcvEnDly = ChannelPtr->RcvEnDlys[ChipSelect / NBPtr->CsPerDelay * MAX_DELAYS + ByteLane];
          if (RcvEnDly > MaxDly) {
            MaxDly = RcvEnDly;
            MaxDlyCs = ChipSelect;
            RetVal = TRUE;
          }
        }
      }
    }
  }

  if (NBPtr->MCTPtr->Status[Sb128bitmode] != 0) {
    //The RcvrEnDlys of DCT1 DIMMs should also be considered while ganging.
    NBPtr->SwitchDCT (NBPtr, 1);
    ChannelPtr = NBPtr->ChannelPtr;
    for (ChipSelect = 0; ChipSelect < NBPtr->CsPerChannel; ChipSelect = ChipSelect + NBPtr->CsPerDelay) {
      for (ByteLane = 0; ByteLane < ((NBPtr->MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8); ByteLane++) {
        RcvEnDly = ChannelPtr->RcvEnDlys[ChipSelect / NBPtr->CsPerDelay * MAX_DELAYS + ByteLane];
        if (RcvEnDly > MaxDly) {
          MaxDly = RcvEnDly;
          MaxDlyCs = ChipSelect;
        }
      }
    }
    NBPtr->SwitchDCT (NBPtr, 0);
  }

  TechPtr->MaxDlyForMaxRdLat = MaxDly;
  *ChipSel = MaxDlyCs;
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function finds the DIMM that has the largest receiver enable delay + Read DQS Delay.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[out]      *ChipSel   - Pointer to the Chip select that has the largest receiver enable delay
 *                                   + Read DQS Delay.
 *
 *     @return   TRUE - A chip select can be found.
 *     @return   FALSE - A chip select cannot be found.
 */

BOOLEAN
MemTFindMaxRcvrEnDlyRdDqsDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  )
{
  UINT8  ChipSelect;
  UINT8  ByteLane;
  UINT16 RcvEnDly;
  UINT16 RdDqsDly;
  UINT16 TotalDly;
  UINT16 MaxDly;
  UINT8  MaxDlyCs;
  BOOLEAN RetVal;

  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  RetVal = FALSE;
  MaxDly = 0;
  MaxDlyCs = 0;
  for (ChipSelect = 0; ChipSelect < NBPtr->CsPerChannel; ChipSelect = ChipSelect + NBPtr->CsPerDelay) {
    if ((NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSelect)) == 0) {
      // Only choose the dimm that does not fail training
      for (ByteLane = 0; ByteLane < MAX_BYTELANES; ByteLane++) {
        RcvEnDly = ChannelPtr->RcvEnDlys[ChipSelect / NBPtr->CsPerDelay * MAX_DELAYS + ByteLane];
        // Before Dqs Position Training, this value is 0. So the maximum value for
        // RdDqsDly needs to be added later when calculating the MaxRdLatency value
        // after RcvEnDly training but before DQS Position Training.
        RdDqsDly = ChannelPtr->RdDqsDlys[ChipSelect / NBPtr->CsPerDelay * MAX_DELAYS + ByteLane];
        TotalDly = RcvEnDly + (RdDqsDly >> 1);
        if (TotalDly > MaxDly) {
          MaxDly = TotalDly;
          MaxDlyCs = ChipSelect;
          RetVal = TRUE;
        }
      }
    }
  }

  TechPtr->MaxDlyForMaxRdLat = MaxDly;
  *ChipSel = MaxDlyCs;
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function finds the DIMM that has the largest receiver enable delay + Read DQS Delay for UNB
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[out]      *ChipSel   - Pointer to the Chip select that has the largest receiver enable delay
 *                                   + Read DQS Delay.
 *
 *     @return   TRUE - A chip select can be found.
 *     @return   FALSE - A chip select cannot be found.
 */

BOOLEAN
MemTFindMaxRcvrEnDlyRdDqsDlyByteUnb (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  )
{
  UINT8  ChipSelect;
  UINT8  ByteLane;
  UINT16 RcvEnDly;
  UINT16 RdDqsDly;
  UINT16 TotalDly;
  UINT16 MaxDly;
  UINT8  MaxDlyCs;
  BOOLEAN RetVal;
  UINT16 *RcvEnDlyPtr;
  UINT8 *RdDqsDlyPtr;

  MEM_NB_BLOCK  *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = NBPtr->ChannelPtr;
  RcvEnDlyPtr = ChannelPtr->RcvEnDlys;
  RdDqsDlyPtr = ChannelPtr->RdDqsDlys;
  if (NBPtr->MemPstate == MEMORY_PSTATE1) {
    RcvEnDlyPtr = ChannelPtr->RcvEnDlysMemPs1;
    RdDqsDlyPtr = ChannelPtr->RdDqsDlysMemPs1;
  }

  RetVal = FALSE;
  MaxDly = 0;
  MaxDlyCs = 0;
  for (ChipSelect = 0; ChipSelect < NBPtr->CsPerChannel; ChipSelect = ChipSelect + NBPtr->CsPerDelay) {
    if ((NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSelect)) == 0) {
      // Only choose the dimm that does not fail training
      for (ByteLane = 0; ByteLane < MAX_BYTELANES; ByteLane++) {
        RcvEnDly = RcvEnDlyPtr[ChipSelect / NBPtr->CsPerDelay * MAX_DELAYS + ByteLane];
        // Before Dqs Position Training, this value is 0. So the maximum value for
        // RdDqsDly needs to be added later when calculating the MaxRdLatency value
        // after RcvEnDly training but before DQS Position Training.
        RdDqsDly = RdDqsDlyPtr[ChipSelect / NBPtr->CsPerDelay * MAX_DELAYS + ByteLane];
        TotalDly = RcvEnDly + ((NBPtr->IsSupported[SwitchRdDqsDlyForMaxRdLatency] && NBPtr->RdDqsDlyForMaxRdLat != 0) ? NBPtr->RdDqsDlyForMaxRdLat: RdDqsDly);
        if (TotalDly > MaxDly) {
          MaxDly = TotalDly;
          MaxDlyCs = ChipSelect;
          RetVal = TRUE;
        }
      }
    }
  }

  TechPtr->MaxDlyForMaxRdLat = MaxDly;
  *ChipSel = MaxDlyCs;
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function finds the minimum or maximum gross dly among all the bytes.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       TrnDlyType - Target Dly type
 *     @param[in]       IfMax - If this is for maximum value or minimum
 *
 *     @return   minimum gross dly
 */
UINT8
STATIC
MemTFindMinMaxGrossDlyByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       TRN_DLY_TYPE TrnDlyType,
  IN       BOOLEAN IfMax
  )
{
  UINT8 ChipSelect;
  UINT8 ByteLane;
  UINT16 CsEnabled;
  UINT8 MinMaxGrossDly;
  UINT8 TrnDly;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;
  CsEnabled = NBPtr->DCTPtr->Timings.CsEnabled;
  MinMaxGrossDly = IfMax ? 0 : 0xFF;

  for (ChipSelect = 0; ChipSelect < NBPtr->CsPerChannel; ChipSelect = ChipSelect + NBPtr->CsPerDelay) {
    if ((CsEnabled & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSelect)) != 0) {
      for (ByteLane = 0; ByteLane < ((NBPtr->MCTPtr->Status[SbEccDimms] && NBPtr->IsSupported[EccByteTraining]) ? 9 : 8); ByteLane++) {
        TrnDly = (UINT8) (GetTrainDlyFromHeapNb (NBPtr, TrnDlyType, DIMM_BYTE_ACCESS (ChipSelect / NBPtr->CsPerDelay, ByteLane)) >> 5);
        if ((IfMax && (TrnDly > MinMaxGrossDly)) || (!IfMax && (TrnDly < MinMaxGrossDly))) {
          MinMaxGrossDly = TrnDly;
        }
      }
    }
  }

  return MinMaxGrossDly;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function compares test pattern with data in buffer and return a pass/fail bitmap
 *       for 8 Bytes for optimized receiver enable training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Buffer[]  -  Buffer data from DRAM (Measured data from DRAM) to compare
 *     @param[in]       Pattern[]  - Pattern (Expected data in ROM/CACHE) to compare against
 *     @param[in]       Side - current side being targeted
 *     @param[in]       Receiver - Current receiver value
 *     @param[in]       Side1En - Indicates if the second side of the DIMM is being used
 *     @return  PASS - Bit map of results of comparison
 */

UINT16
STATIC
MemTCompare1ClPatternOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT8 Side,
  IN       UINT8 Receiver,
  IN       BOOLEAN  Side1En
  )
{
  UINT16 i;
  UINT16 j;
  UINT16 Pass;
  DIE_STRUCT *MCTPtr;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;
  MCTPtr = TechPtr->NBPtr->MCTPtr;

  if (MCTPtr->GangedMode && MCTPtr->Dct) {
    j = 8;
  } else {
    j = 0;
  }

  Pass = 0xFFFF;
  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tDelay[BL] -");
  for (i = 0; i < 8; i++) {
    IDS_HDT_CONSOLE (MEM_FLOW, " %02x", TechPtr->RcvrEnDlyOpt[i] & 0xFF);
    if (Buffer[j] != Pattern[j]) {
      // if bytelane n fails
      Pass &= ~((UINT16)1 << (j % 8));    // clear bit n
      TechPtr->DqsRcvEnFirstPassValOpt[i] = 0;
      TechPtr->GetFirstPassValOpt[i] = FALSE;
      TechPtr->IncBy1ForNextCountOpt[i] = FALSE;
      TechPtr->DqsRcvEnSavedOpt[i] = FALSE;
      if (TechPtr->FilterStatusOpt[i] != DONE_FILTER) {
        if (Side == ((Side1En ? 4 : 2) - 1)) {
          TechPtr->RcvrEnDlyOpt[i] += FILTER_FIRST_STAGE_COUNT;
        }
      }
    } else {
      if (TechPtr->FilterSidePassCountOpt[i] == ((Side1En ? 4 : 2) - 1)) {
        //Only apply filter if all sides have passed
        if (TechPtr->FilterStatusOpt[i] != DONE_FILTER) {
          if (TechPtr->GetFirstPassValOpt[i] == FALSE) {
            // This is the first Pass, mark the start of filter check
            TechPtr->DqsRcvEnFirstPassValOpt[i] = TechPtr->RcvrEnDlyOpt[i];
            TechPtr->GetFirstPassValOpt[i] = TRUE;
            TechPtr->IncBy1ForNextCountOpt[i] = FALSE;
            TechPtr->RcvrEnDlyOpt[i]++;
          } else {
            if ((TechPtr->RcvrEnDlyOpt[i] - TechPtr->DqsRcvEnFirstPassValOpt[i]) < FILTER_WINDOW_SIZE) {
              if (TechPtr->IncBy1ForNextCountOpt[i] == FALSE) {
                TechPtr->RcvrEnDlyOpt[i] += FILTER_SECOND_STAGE_COUNT;
                TechPtr->IncBy1ForNextCountOpt[i] = TRUE;
              } else {
                TechPtr->RcvrEnDlyOpt[i]++;
                TechPtr->IncBy1ForNextCountOpt[i] = FALSE;
              }
            } else {
              // End sweep and add offset to first pass
              TechPtr->MaxRcvrEnDlyBlOpt[i] = TechPtr->DqsRcvEnFirstPassValOpt[i];
              TechPtr->RcvrEnDlyOpt[i] = TechPtr->DqsRcvEnFirstPassValOpt[i] + FILTER_OFFSET_VALUE;
              TechPtr->FilterStatusOpt[i] = DONE_FILTER;
              TechPtr->FilterCountOpt++;
            }
          }
        } else {
          TechPtr->FilterSidePassCountOpt[i]++;
        }
      } else {
        if (TechPtr->GetFirstPassValOpt[i] == FALSE) {
          if (Side == ((Side1En ? 4 : 2) - 1)) {
            TechPtr->RcvrEnDlyOpt[i] += FILTER_FIRST_STAGE_COUNT;
          }
        }
        TechPtr->FilterSidePassCountOpt[i]++;
      }
      TechPtr->DqsRcvEnSavedOpt[i] = TRUE;
      ChannelPtr->RcvEnDlys[(Receiver >> 1) * MAX_DELAYS + i] = TechPtr->RcvrEnDlyOpt[i];
    }
    if (Side == ((Side1En ? 4 : 2) - 1)) {
      TechPtr->FilterSidePassCountOpt[i] = 0;
    }
    if (TechPtr->RcvrEnDlyOpt[i] >= TechPtr->RcvrEnDlyLimitOpt[i]) {
      TechPtr->FilterCountOpt++;
    }

    j++;
  }

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\tPass/Fail -");
    for (i = 0, j -= 8; i < 8; i++, j++) {
      IDS_HDT_CONSOLE (MEM_FLOW, "  %c", (Buffer[j] == Pattern[j]) ? 'P' : '.');
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t Measured -");
    for (i = 0, j -= 8; i < 8; i++, j++) {
      IDS_HDT_CONSOLE (MEM_FLOW, " %02x", Buffer[j]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\t\t\t Expected -");
    for (i = 0, j -= 8; i < 8; i++, j++) {
      IDS_HDT_CONSOLE (MEM_FLOW, " %02x", Pattern[j]);
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\n");
  );

  return Pass;
}
/*-----------------------------------------------------------------------------
 *
 *  This function initializes variables for optimized training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 * ----------------------------------------------------------------------------
 */
VOID
MemTInitializeVariablesOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 ByteLane;
  for (ByteLane = 0; ByteLane < MAX_BYTELANES_PER_CHANNEL; ByteLane++) {
    TechPtr->RcvrEnDlyLimitOpt[ByteLane] = FILTER_MAX_REC_EN_DLY_VALUE;      // @attention - limit depends on proc type
    TechPtr->DqsRcvEnSavedOpt[ByteLane] = FALSE;
    TechPtr->RcvrEnDlyOpt[ByteLane] = FILTER_NEW_RECEIVER_START_VALUE;
    TechPtr->GetFirstPassValOpt[ByteLane] = FALSE;
    TechPtr->DqsRcvEnFirstPassValOpt[ByteLane] = 0;
    TechPtr->RevertPassValOpt[ByteLane] = FALSE;
    TechPtr->MaxRcvrEnDlyBlOpt[ByteLane] = 0;
    TechPtr->FilterStatusOpt[ByteLane] = START_FILTER;
    TechPtr->FilterCountOpt = 0;
    TechPtr->FilterSidePassCountOpt[ByteLane] = 0;
    TechPtr->IncBy1ForNextCountOpt[ByteLane] = FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function loads the DqsRcvEnDly from saved data and program to additional index
 *  for optimized DQS receiver enabled training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *
 */

VOID
STATIC
MemTLoadRcvrEnDlyOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  )
{
  UINT8 i;
  UINT8 Dimm;
  CH_DEF_STRUCT *ChannelPtr;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);
  ChannelPtr = TechPtr->NBPtr->ChannelPtr;

  Dimm = Receiver >> 1;
  for (i = 0; i < 8; i++) {
    if (TechPtr->DqsRcvEnSavedOpt[i]) {
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver >> 1, i),
      ChannelPtr->RcvEnDlys[Dimm * MAX_DELAYS + i]);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function programs DqsRcvEnDly to additional index for DQS receiver enabled training
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 *     @param[in]       RcvEnDly  - receiver enable delay to be saved
 */

VOID
STATIC
MemTSetRcvrEnDlyOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT16 RcvEnDly
  )
{
  UINT8 ByteLane;

  ASSERT (Receiver < MAX_CS_PER_CHANNEL);

  for (ByteLane = 0; ByteLane < 8; ByteLane++) {
    if (TechPtr->FilterStatusOpt[ByteLane] != DONE_FILTER) {
      TechPtr->NBPtr->SetTrainDly (TechPtr->NBPtr, AccessRcvEnDly, DIMM_BYTE_ACCESS (Receiver >> 1, ByteLane), TechPtr->RcvrEnDlyOpt[ByteLane]);
    }
  }
}
/*-----------------------------------------------------------------------------
 *
 *  This sets any Errors generated from Dly sweep
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]   DCT   - current DCT
 *     @param[in]   Receiver   - current receiver
 *
 *     @return     FALSE - Fatal error occurs.
 *     @return     TRUE  - No fatal error occurs.
 * ----------------------------------------------------------------------------
 */
BOOLEAN
MemTSetSweepErrorOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver,
  IN       UINT8 Dct,
  IN       BOOLEAN ErrorCheck
  )
{
  UINT8 ByteLane;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;
  for (ByteLane = 0; ByteLane < MAX_BYTELANES_PER_CHANNEL; ByteLane++) {
    if (TechPtr->RcvrEnDlyOpt[ByteLane] == TechPtr->RcvrEnDlyLimitOpt[ByteLane]) {
      // no passing window
      if (ErrorCheck) {
        return FALSE;
      }
      PutEventLog (AGESA_ERROR, MEM_ERROR_RCVR_EN_NO_PASSING_WINDOW, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, ByteLane, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, MCTPtr);
    }
    if (TechPtr->RcvrEnDlyOpt[ByteLane] > (TechPtr->RcvrEnDlyLimitOpt[ByteLane] - 1)) {
      // passing window too narrow, too far delayed
      if (ErrorCheck) {
        return FALSE;
      }
      PutEventLog (AGESA_ERROR, MEM_ERROR_RCVR_EN_VALUE_TOO_LARGE, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, ByteLane, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, MCTPtr);
      DCTPtr->Timings.CsTrainFail |= (UINT16) (3 << Receiver) & DCTPtr->Timings.CsPresent;
      MCTPtr->ChannelTrainFail |= (UINT32)1 << Dct;
      if (!NBPtr->MemPtr->ErrorHandling (MCTPtr, NBPtr->Dct, DCTPtr->Timings.CsTrainFail, &MemPtr->StdHeader)) {
        ASSERT (FALSE);
        return FALSE;
      }
    }
  }
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *  This function determines the maximum receiver delay value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @retval  MaxRcvrValue - Maximum receiver delay value for all bytelanes
 * ----------------------------------------------------------------------------
 */

UINT16
MemTGetMaxValueOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 ByteLane;
  UINT16 MaxRcvrValue;
  MaxRcvrValue = 0;
  for (ByteLane = 0; ByteLane < MAX_BYTELANES_PER_CHANNEL; ByteLane++) {
    if (TechPtr->MaxRcvrEnDlyBlOpt[ByteLane] > MaxRcvrValue) {
      MaxRcvrValue = TechPtr->MaxRcvrEnDlyBlOpt[ByteLane];
    }
  }
  MaxRcvrValue += FILTER_OFFSET_VALUE;
  return MaxRcvrValue;
}
/*-----------------------------------------------------------------------------
 *
 *  This function determines if the sweep loop should complete.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @retval  TRUE - All bytelanes pass
 *              FALSE - Some bytelanes fail
 * ----------------------------------------------------------------------------
 */

BOOLEAN
MemTCheckRcvrEnDlyLimitOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  if (TechPtr->FilterCountOpt >= (UINT16)MAX_CS_PER_CHANNEL) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function load the result of write levelization training into RcvrEnDlyOpt,
 *  using it as the initial value for Receiver DQS training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Receiver  - Current Chip select value
 */
VOID
STATIC
MemTLoadInitialRcvEnDlyOptByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Receiver
  )
{
  UINT8 ByteLane;
  MEM_NB_BLOCK *NBPtr;

  NBPtr = TechPtr->NBPtr;
  for (ByteLane = 0; ByteLane < MAX_BYTELANES_PER_CHANNEL; ByteLane++) {
    TechPtr->RcvrEnDlyOpt[ByteLane] = NBPtr->ChannelPtr->WrDqsDlys[((Receiver >> 1) * TechPtr->DlyTableWidth ()) + ByteLane];
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function finds the DIMM that has the largest receiver enable delay that are trained by PMU
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[out]      *ChipSel   - Pointer to the Chip select that has the largest receiver enable delay
 *                                   + Read DQS Delay.
 *
 *     @return   TRUE - A chip select can be found.
 *     @return   FALSE - A chip select cannot be found.
 */

BOOLEAN
MemTFindMaxRcvrEnDlyTrainedByPmuByte (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
     OUT   UINT8 *ChipSel
  )
{
  UINT8   ChipSelect;
  BOOLEAN RetVal;
  UINT16  MaxDly;
  UINT8   MaxDlyCs;

  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;

  RetVal = FALSE;
  MaxDly = 0;
  MaxDlyCs = 0;
  for (ChipSelect = 0; ChipSelect < NBPtr->CsPerChannel; ChipSelect = ChipSelect + NBPtr->CsPerDelay) {
    /// @todo Fix this when BKDG has updated algorithm
    if ((NBPtr->DCTPtr->Timings.CsPresent & ((UINT16) ((NBPtr->CsPerDelay == 2)? 3 : 1) << ChipSelect)) != 0) {
      MaxDly = 0;
      MaxDlyCs = ChipSelect;
      RetVal = TRUE;
    }
  }

  TechPtr->MaxDlyForMaxRdLat = MaxDly;
  *ChipSel = MaxDlyCs;
  return RetVal;
}
