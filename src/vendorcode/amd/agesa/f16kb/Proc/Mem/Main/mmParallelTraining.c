/* $NoKeywords:$ */
/**
 * @file
 *
 * mmNodeInterleave.c
 *
 * Main Memory Feature implementation file for Node Interleaving
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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


#include "Porting.h"
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "cpuApicUtilities.h"
#include "GeneralServices.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "ma.h"
#include "mu.h"
#include "mfParallelTraining.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_MAIN_MMPARALLELTRAINING_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemMParallelTraining (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMParallelTraining (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  AMD_CONFIG_PARAMS *StdHeader;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK *NBPtr;
  DIE_INFO TrainInfo[MAX_NODES_SUPPORTED];
  AP_DATA_TRANSFER ReturnData;
  AGESA_STATUS Status;
  UINT8 ApSts;
  UINT8 Die;
  UINT8 Socket;
  UINT32 Module;
  UINT32 LowCore;
  UINT32 HighCore;
  UINT32 Time;
  UINT32 TimeOut;
  UINT32 TargetApicId;
  BOOLEAN StillTraining;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UINT8 *BufferPtr;
  BOOLEAN TimeoutEn;

  NBPtr = mmPtr->NBPtr;
  MemPtr = mmPtr->MemPtr;
  StdHeader = &(mmPtr->MemPtr->StdHeader);
  Time = 0;
  TimeOut = PARALLEL_TRAINING_TIMEOUT;
  TimeoutEn = TRUE;
  IDS_TIMEOUT_CTL (&TimeoutEn);

  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart parallel training\n");
  AGESA_TESTPOINT (TpProcMemBeforeAnyTraining, StdHeader);
  //
  // Initialize Training Info Array
  //
  for (Die = 0; Die < mmPtr->DieCount; Die ++) {
    Socket = TrainInfo[Die].Socket = NBPtr[Die].MCTPtr->SocketId;
    Module = NBPtr[Die].MCTPtr->DieId;
    GetGivenModuleCoreRange (Socket, Module, &LowCore, &HighCore, StdHeader);
    TrainInfo[Die].Core = (UINT8) (LowCore & 0x000000FF);
    IDS_HDT_CONSOLE (MEM_FLOW, "\tLaunch core %d of socket %d\n", LowCore, Socket);
    TrainInfo[Die].Training = FALSE;
  }
  //
  // Start Training on Each remote die.
  //
  for (Die = 0; Die < mmPtr->DieCount; Die ++ ) {
    if (Die != BSP_DIE) {
      NBPtr[Die].BeforeDqsTraining (&(mmPtr->NBPtr[Die]));
      if (NBPtr[Die].MCTPtr->NodeMemSize != 0) {
        if (!NBPtr[Die].FeatPtr->Training (&(mmPtr->NBPtr[Die]))) {
          // Fail to launch code on AP
          PutEventLog (AGESA_ERROR, MEM_ERROR_PARALLEL_TRAINING_LAUNCH_FAIL, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
          SetMemError (AGESA_ERROR, NBPtr[Die].MCTPtr);
          if (!MemPtr->ErrorHandling (NBPtr[Die].MCTPtr, EXCLUDE_ALL_DCT, EXCLUDE_ALL_CHIPSEL, &MemPtr->StdHeader)) {
            ASSERT (FALSE);
          }
        } else {
          TrainInfo[Die].Training = TRUE;
        }
      }
    }
  }
  //
  // Call training on BSP
  //
  IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", NBPtr[BSP_DIE].Node);
  NBPtr[BSP_DIE].BeforeDqsTraining (&(mmPtr->NBPtr[BSP_DIE]));
  NBPtr[BSP_DIE].TrainingFlow (&(mmPtr->NBPtr[BSP_DIE]));
  NBPtr[BSP_DIE].AfterDqsTraining (&(mmPtr->NBPtr[BSP_DIE]));

  //
  // Get Results from remote processors training
  //
  do {
    StillTraining = FALSE;
    for (Die = 0; Die < mmPtr->DieCount; Die ++ ) {
      //
      // For each Die that is training, read the status
      //
      if (TrainInfo[Die].Training == TRUE) {
        GetLocalApicIdForCore (TrainInfo[Die].Socket, TrainInfo[Die].Core, &TargetApicId, StdHeader);
        ApSts = ApUtilReadRemoteControlByte (TargetApicId, StdHeader);
        if ((ApSts & 0x80) == 0) {
          //
          // Allocate buffer for received data
          //
          AllocHeapParams.RequestedBufferSize = (
            sizeof (DIE_STRUCT) +
            NBPtr[Die].DctCount * (
              sizeof (DCT_STRUCT) + (
                NBPtr[Die].ChannelCount * (
                  sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK) + (
                   (NBPtr[Die].MCTPtr->DctData[0].ChData[0].RowCount *
                    NBPtr[Die].MCTPtr->DctData[0].ChData[0].ColumnCount *
                    NUMBER_OF_DELAY_TABLES) +
                    (MAX_BYTELANES_PER_CHANNEL * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES) +
                    (MAX_DIMMS_PER_CHANNEL * MAX_NUMBER_LANES)
                  )
                )
              )
            )
          ) + 3;
          AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_PAR_TRN_HANDLE, Die, 0, 0);
          AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
          if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
            //
            // Receive Training Results
            //

            ReturnData.DataPtr = AllocHeapParams.BufferPtr;
            ReturnData.DataSizeInDwords = (UINT16) AllocHeapParams.RequestedBufferSize / 4;
            ReturnData.DataTransferFlags = 0;
            Status = ApUtilReceiveBuffer (TrainInfo[Die].Socket, TrainInfo[Die].Core, &ReturnData, StdHeader);
            if (Status != AGESA_SUCCESS) {
              SetMemError (Status, NBPtr[Die].MCTPtr);
            }

            BufferPtr = AllocHeapParams.BufferPtr;
            LibAmdMemCopy (NBPtr[Die].MCTPtr, BufferPtr, sizeof (DIE_STRUCT), StdHeader);
            BufferPtr += sizeof (DIE_STRUCT);
            LibAmdMemCopy ( NBPtr[Die].MCTPtr->DctData,
                            BufferPtr,
                            NBPtr[Die].DctCount * (sizeof (DCT_STRUCT) + NBPtr[Die].ChannelCount * sizeof (CH_DEF_STRUCT)),
                            StdHeader);
            BufferPtr += NBPtr[Die].DctCount * (sizeof (DCT_STRUCT) + NBPtr[Die].ChannelCount * sizeof (CH_DEF_STRUCT));
            LibAmdMemCopy ( NBPtr[Die].PSBlock,
                            BufferPtr,
                            NBPtr[Die].DctCount * NBPtr[Die].ChannelCount * sizeof (MEM_PS_BLOCK),
                            StdHeader);
            BufferPtr += NBPtr[Die].DctCount * NBPtr[Die].ChannelCount * sizeof (MEM_PS_BLOCK);
            LibAmdMemCopy ( NBPtr[Die].MCTPtr->DctData[0].ChData[0].RcvEnDlys,
                            BufferPtr,
                            (NBPtr[Die].DctCount * NBPtr[Die].ChannelCount) *
                            ((NBPtr[Die].MCTPtr->DctData[0].ChData[0].RowCount *
                              NBPtr[Die].MCTPtr->DctData[0].ChData[0].ColumnCount *
                              NUMBER_OF_DELAY_TABLES) +
                              (MAX_BYTELANES_PER_CHANNEL * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES) +
                              (MAX_DIMMS_PER_CHANNEL * MAX_NUMBER_LANES)
                            ),
                            StdHeader);

            HeapDeallocateBuffer (AllocHeapParams.BufferHandle, StdHeader);

            NBPtr[Die].AfterDqsTraining (&(mmPtr->NBPtr[Die]));
            TrainInfo[Die].Training = FALSE;
          } else {
            PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_RECEIVED_DATA, NBPtr[Die].Node, 0, 0, 0, StdHeader);
            SetMemError (AGESA_FATAL, NBPtr[Die].MCTPtr);
            ASSERT(FALSE); // Insufficient Heap Space allocation for parallel training buffer
          }
        } else if (ApSts == CORE_IDLE) {
          // AP does not have buffer to transmit to BSP
          // AP fails to locate a buffer for data transfer
          TrainInfo[Die].Training = FALSE;
        } else {
          // Signal to loop through again
          StillTraining = TRUE;
        }
      }
    }
    // Wait for 1 us
    MemUWait10ns (100, NBPtr->MemPtr);
    Time ++;
  } while ((StillTraining) && ((Time < TimeOut) || !TimeoutEn)); // Continue until all Dies are finished
                                                // if cannot finish in 1 s, do fatal exit

  if (StillTraining && TimeoutEn) {
    // Parallel training time out, do fatal exit, as there is at least one AP hangs.
    PutEventLog (AGESA_FATAL, MEM_ERROR_PARALLEL_TRAINING_TIME_OUT, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_FATAL, NBPtr[BSP_DIE].MCTPtr);
    ASSERT(FALSE); // Timeout occurred while still training
  }

  for (Die = 0; Die < mmPtr->DieCount; Die ++ ) {
    if (NBPtr[Die].MCTPtr->ErrCode == AGESA_FATAL) {
      return FALSE;
    }
  }
  return TRUE;
}
