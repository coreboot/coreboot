/* $NoKeywords:$ */
/**
 * @file
 *
 * mfParallelTraining.c
 *
 * This is the parallel training feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/PARTRN)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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




#include "AGESA.h"
#include "amdlib.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "mfParallelTraining.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_PARTRN_MFPARALLELTRAINING_FILECODE

/*-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
extern MEM_TECH_CONSTRUCTOR* memTechInstalled[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *    This is the main function to perform parallel training on all nodes.
 *    This is the routine which will run on the remote AP.
 *
 *     @param[in,out]   *EnvPtr   - Pointer to the Training Environment Data
 *     @param[in,out]   *StdHeader   - Pointer to the Standard Header of the AP
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */
BOOLEAN
MemFParallelTraining (
  IN OUT   REMOTE_TRAINING_ENV *EnvPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  MEM_PARAMETER_STRUCT ParameterList;
  MEM_NB_BLOCK NB;
  MEM_TECH_BLOCK TB;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  UINT8 p;
  UINT8 i;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 *BufferPtr;
  UINT8 DctCount;
  UINT8 ChannelCount;
  UINT8 RowCount;
  UINT8 ColumnCount;
  UINT16 SizeOfNewBuffer;
  AP_DATA_TRANSFER ReturnData;

  //
  // Initialize Parameters
  //
  ReturnData.DataPtr = NULL;
  ReturnData.DataSizeInDwords = 0;
  ReturnData.DataTransferFlags = 0;

  ASSERT (EnvPtr != NULL);
  //
  // Replace Standard header of a AP
  //
  LibAmdMemCopy (StdHeader, &(EnvPtr->StdHeader), sizeof (AMD_CONFIG_PARAMS), &(EnvPtr->StdHeader));


  //
  //  Allocate buffer for training data
  //
  BufferPtr = (UINT8 *) (&EnvPtr->DieStruct);
  DctCount = EnvPtr->DieStruct.DctCount;
  BufferPtr += sizeof (DIE_STRUCT);
  ChannelCount = ((DCT_STRUCT *) BufferPtr)->ChannelCount;
  BufferPtr += DctCount * sizeof (DCT_STRUCT);
  RowCount = ((CH_DEF_STRUCT *) BufferPtr)->RowCount;
  ColumnCount = ((CH_DEF_STRUCT *) BufferPtr)->ColumnCount;

  SizeOfNewBuffer = sizeof (DIE_STRUCT) +
                    DctCount * (
                      sizeof (DCT_STRUCT) + (
                        ChannelCount * (
                          sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK) + (
                            RowCount * ColumnCount * NUMBER_OF_DELAY_TABLES +
                            (MAX_BYTELANES_PER_CHANNEL * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES)
                          )
                        )
                      )
                    );
  AllocHeapParams.RequestedBufferSize = SizeOfNewBuffer;
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_PAR_TRN_HANDLE, 0, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    BufferPtr = AllocHeapParams.BufferPtr;
    LibAmdMemCopy ( BufferPtr,
                    &(EnvPtr->DieStruct),
                    sizeof (DIE_STRUCT) + DctCount * (sizeof (DCT_STRUCT) + ChannelCount * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK))),
                    StdHeader
                  );

    //
    //  Fix up pointers
    //
    MCTPtr = (DIE_STRUCT *) BufferPtr;
    BufferPtr += sizeof (DIE_STRUCT);
    MCTPtr->DctData = (DCT_STRUCT *) BufferPtr;
    BufferPtr += MCTPtr->DctCount * sizeof (DCT_STRUCT);
    for (Dct = 0; Dct < MCTPtr->DctCount; Dct++) {
      MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) BufferPtr;
      BufferPtr += MCTPtr->DctData[Dct].ChannelCount * sizeof (CH_DEF_STRUCT);
      for (Channel = 0; Channel < MCTPtr->DctData[Dct].ChannelCount; Channel++) {
        MCTPtr->DctData[Dct].ChData[Channel].MCTPtr = MCTPtr;
        MCTPtr->DctData[Dct].ChData[Channel].DCTPtr = &MCTPtr->DctData[Dct];
      }
    }
    NB.PSBlock = (MEM_PS_BLOCK *) BufferPtr;
    BufferPtr += DctCount * ChannelCount * sizeof (MEM_PS_BLOCK);

    ReturnData.DataPtr = AllocHeapParams.BufferPtr;
    ReturnData.DataSizeInDwords = (SizeOfNewBuffer + 3) / 4;
    ReturnData.DataTransferFlags = 0;

    //
    // Allocate Memory for the MEM_DATA_STRUCT we will use
    //
    AllocHeapParams.RequestedBufferSize = sizeof (MEM_DATA_STRUCT);
    AllocHeapParams.BufferHandle = AMD_MEM_DATA_HANDLE;
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
      MemPtr = (MEM_DATA_STRUCT *)AllocHeapParams.BufferPtr;

      LibAmdMemCopy (&(MemPtr->StdHeader), &(EnvPtr->StdHeader), sizeof (AMD_CONFIG_PARAMS), StdHeader);

      //
      // Copy Parameters from environment
      //
      ParameterList.HoleBase = EnvPtr->HoleBase;
      ParameterList.BottomIo = EnvPtr->BottomIo;
      ParameterList.UmaSize = EnvPtr->UmaSize;
      ParameterList.SysLimit = EnvPtr->SysLimit;
      ParameterList.TableBasedAlterations = EnvPtr->TableBasedAlterations;
      ParameterList.PlatformMemoryConfiguration = EnvPtr->PlatformMemoryConfiguration;
      MemPtr->ParameterListPtr = &ParameterList;

      for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
        MemPtr->GetPlatformCfg[p] = EnvPtr->GetPlatformCfg[p];
      }

      MemPtr->ErrorHandling = EnvPtr->ErrorHandling;
      //
      // Create Local NBBlock and Tech Block
      //
      EnvPtr->NBBlockCtor (&NB, MCTPtr, EnvPtr->FeatPtr);
      NB.RefPtr = &ParameterList;
      NB.MemPtr = MemPtr;
      i = 0;
      while (memTechInstalled[i] != NULL) {
        if (memTechInstalled[i] (&TB, &NB)) {
          break;
        }
        i++;
      }
      NB.TechPtr = &TB;
      NB.TechBlockSwitch (&NB);

      //
      // Setup CPU Mem Type MSRs on the AP
      //
      NB.CpuMemTyping (&NB);

      IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", NB.Node);
      //
      // Call Technology Specific Training routine
      //
      NB.TrainingFlow (&NB);
      //
      // Copy training data to ReturnData buffer
      //
      LibAmdMemCopy ( BufferPtr,
                      MCTPtr->DctData[0].ChData[0].RcvEnDlys,
                      ((DctCount * ChannelCount) * (
                         (RowCount * ColumnCount * NUMBER_OF_DELAY_TABLES) +
                         (MAX_BYTELANES_PER_CHANNEL * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES)
                        )
                      ),
                      StdHeader);

      HeapDeallocateBuffer (AMD_MEM_DATA_HANDLE, StdHeader);
      //
      // Restore pointers
      //
      for (Dct = 0; Dct < MCTPtr->DctCount; Dct++) {
        for (Channel = 0; Channel < MCTPtr->DctData[Dct].ChannelCount; Channel++) {
          MCTPtr->DctData[Dct].ChData[Channel].MCTPtr = &EnvPtr->DieStruct;
          MCTPtr->DctData[Dct].ChData[Channel].DCTPtr = &EnvPtr->DieStruct.DctData[Dct];

          MCTPtr->DctData[Dct].ChData[Channel].RcvEnDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RcvEnDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDqsDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDqsDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDatDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDatDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsMinDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsMinDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsMaxDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsMaxDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDatMinDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDatMinDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDatMaxDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDatMaxDlys;
          MCTPtr->DctData[Dct].ChData[Channel].FailingBitMask = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].FailingBitMask;
        }
        MCTPtr->DctData[Dct].ChData = EnvPtr->DieStruct.DctData[Dct].ChData;
      }
      MCTPtr->DctData = EnvPtr->DieStruct.DctData;
    }

    //
    // Signal to BSP that training is complete and Send Results
    //
    ASSERT (ReturnData.DataPtr != NULL);
    ApUtilTransmitBuffer (EnvPtr->BspSocket, EnvPtr->BspCore, &ReturnData, StdHeader);

    //
    // Clean up and exit.
    //
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_PAR_TRN_HANDLE, 0, 0, 0), StdHeader);
  } else {
    MCTPtr = &EnvPtr->DieStruct;
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA, MCTPtr->NodeId, 0, 0, 0, StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE); // Could not allocate heap for buffer for parallel training data
  }
  return TRUE;
}
