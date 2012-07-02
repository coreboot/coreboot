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
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
