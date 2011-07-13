/**
 * @file
 *
 * mnParTrainDr.c
 *
 * Feature which performs Memory DQS training on each node with each node training
 * its own memory through code running on a core in the associated processor.
 * This way memory can be trained in parallel by more than one processor.
 *
 * This file contains the Deerhound specific parallel training function.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/HCTRN)
 * @e \$Revision: 6516 $ @e \$Date: 2008-06-24 06:06:40 -0500 (Tue, 24 Jun 2008) $
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




#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mndr.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuApicUtilities.h"
#include "mfParallelTraining.h"
#include "heapManager.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_DR_MNPARTRAINDR_FILECODE

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/

BOOLEAN
STATIC
MemConstructRemoteNBBlockDR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       DIE_STRUCT *MCTPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr
);

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *       This is the training function which set up the environment for remote
 * training on the ap and launches the remote routine.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Launch training on AP successfully.
 *     @return          FALSE - Fail to launch training on AP.
 */
BOOLEAN
MemFParallelTrainingDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  AMD_CONFIG_PARAMS *StdHeader;
  DIE_STRUCT *MCTPtr;
  REMOTE_TRAINING_ENV *EnvPtr;
  AP_TASK TrainingTask;
  UINT8 Socket;
  UINT8 Module;
  UINT8 APCore;
  UINT8 p;
  UINT32 LowCore;
  UINT32 HighCore;
  UINT32 BspSocket;
  UINT32 BspModule;
  UINT32 BspCore;
  AGESA_STATUS Status;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT16 MctDataSize;

  StdHeader = &(NBPtr->MemPtr->StdHeader);
  MCTPtr = NBPtr->MCTPtr;
  Socket = MCTPtr->SocketId;
  Module = MCTPtr->DieId;

  //
  // Allocate buffer for REMOTE_TRAINING_ENV
  //
  MctDataSize = MAX_DCTS_PER_NODE_DR * (
                  sizeof (DCT_STRUCT) + (
                    MAX_CHANNELS_PER_DCT_DR * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK))
                  )
                );
  AllocHeapParams.RequestedBufferSize = MctDataSize + sizeof (REMOTE_TRAINING_ENV);
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_PAR_TRN_HANDLE, Socket, Module, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    EnvPtr = (REMOTE_TRAINING_ENV *) AllocHeapParams.BufferPtr;
    AllocHeapParams.BufferPtr += sizeof (REMOTE_TRAINING_ENV);

    //
    // Setup Remote training environment
    //
    LibAmdMemCopy (&(EnvPtr->StdHeader), StdHeader, sizeof (AMD_CONFIG_PARAMS), StdHeader);
    LibAmdMemCopy (&(EnvPtr->DieStruct), MCTPtr, sizeof (DIE_STRUCT), StdHeader);
    for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
      EnvPtr->GetPlatformCfg[p] = NBPtr->MemPtr->GetPlatformCfg[p];
    }
    EnvPtr->ErrorHandling = NBPtr->MemPtr->ErrorHandling;
    EnvPtr->NBBlockCtor = MemConstructRemoteNBBlockDR;
    EnvPtr->FeatPtr = NBPtr->FeatPtr;
    EnvPtr->HoleBase = NBPtr->RefPtr->HoleBase;
    EnvPtr->BottomIo = NBPtr->RefPtr->BottomIo;
    EnvPtr->SysLimit = NBPtr->RefPtr->SysLimit;
    EnvPtr->TableBasedAlterations = NBPtr->RefPtr->TableBasedAlterations;
    EnvPtr->PlatformMemoryConfiguration = NBPtr->RefPtr->PlatformMemoryConfiguration;

    LibAmdMemCopy (AllocHeapParams.BufferPtr, MCTPtr->DctData, MctDataSize, StdHeader);

    //
    // Get Socket, Core of the BSP
    //
    IdentifyCore (StdHeader, &BspSocket, &BspModule, &BspCore, &Status);
    EnvPtr->BspSocket = ((UINT8)BspSocket & 0x000000FF);
    EnvPtr->BspCore = ((UINT8)BspCore & 0x000000FF);

    //
    // Set up the remote task structure
    //
    TrainingTask.DataTransfer.DataPtr = EnvPtr;
    TrainingTask.DataTransfer.DataSizeInDwords = (AllocHeapParams.RequestedBufferSize + 3) / 4;
    TrainingTask.DataTransfer.DataTransferFlags = 0;
    TrainingTask.ExeFlags = 0;
    TrainingTask.FuncAddress.PfApTaskI = (PF_AP_TASK_I)MemFParallelTraining;

    //
    // Get Target AP Core
    //
    GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader);
    APCore = (UINT8) (LowCore & 0x000000FF);

    //
    // Launch Remote Training
    //
    ApUtilRunCodeOnSocketCore (Socket, APCore, &TrainingTask, StdHeader);

    HeapDeallocateBuffer (AllocHeapParams.BufferHandle, StdHeader);
    return TRUE;
  } else {
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_REMOTE_TRAINING_ENV, NBPtr->Node, 0, 0, 0, StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE); // Could not allocated heap space for "REMOTE_TRAINING_ENV"
    return FALSE;
  }
}

BOOLEAN
STATIC
MemConstructRemoteNBBlockDR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       DIE_STRUCT *MCTPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr
  )
{
  NBPtr->MCTPtr = MCTPtr;
  NBPtr->PciAddr.AddressValue = MCTPtr->PciAddr.AddressValue;

  MemNInitNBDataDr (NBPtr);

  FeatPtr->InitCPG (NBPtr);
  NBPtr->FeatPtr = FeatPtr;

  MemNSwitchDCTNb (NBPtr, 0);
  return TRUE;
}
