/* $NoKeywords:$ */
/**
 * @file
 *
 * mm.c
 *
 * Main Memory Entrypoint file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MM_FILECODE
/* features */

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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function deallocates heap buffers that were allocated in AmdMemAuto
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
MemAmdFinalize (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Die;

  for (Die = 0; Die < MemPtr->DieCount; Die++ ) {
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_TRN_DATA_HANDLE, Die, 0, 0), &MemPtr->StdHeader);
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_DCT_STRUCT_HANDLE, Die, 0, 0), &MemPtr->StdHeader);
  }

  HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_DIE_STRUCT_HANDLE, 0, 0, 0), &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_S3_SAVE_HANDLE, &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_MEM_SPD_HANDLE, &MemPtr->StdHeader);
  HeapDeallocateBuffer (AMD_MEM_AUTO_HANDLE, &MemPtr->StdHeader);
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 * MemSocketScan - Scan all nodes, recording the physical Socket number,
 * Die Number (relative to the socket), and PCI Device address of each
 * populated socket.
 *
 * This information is used by the northbridge block to map a dram
 * channel on a particular DCT, on a particular CPU Die, in a particular
 * socket to a the DRAM SPD Data for the DIMMS physically connected to
 * that channel.
 *
 * Also, the customer socket map is populated with pointers to the
 * appropriate channel structures, so that the customer can locate the
 * appropriate channel configuration data.
 *
 * This socket scan will always result in Die 0 as the BSP.
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
AGESA_STATUS
MemSocketScan (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  UINT8 DieIndex;
  UINT8 DieCount;
  UINT32 SocketId;
  UINT32 DieId;
  UINT8 Die;
  PCI_ADDR Address;
  AGESA_STATUS AgesaStatus;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  ASSERT (mmPtr != NULL);
  ASSERT (mmPtr->MemPtr != NULL);
  MemPtr = mmPtr->MemPtr;

  //
  //  Count the number of dies in the system
  //
  DieCount = 0;
  for (Die = 0; Die < MAX_NODES_SUPPORTED; Die++) {
    if (GetSocketModuleOfNode ((UINT32)Die, &SocketId, &DieId, (VOID *)MemPtr)) {
      DieCount++;
    }
  }
  MemPtr->DieCount = DieCount;
  mmPtr->DieCount = DieCount;

  if (DieCount > 0) {
    //
    //  Allocate buffer for DIE_STRUCTs
    //
    AllocHeapParams.RequestedBufferSize = ((UINT16)DieCount * sizeof (DIE_STRUCT));
    AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DIE_STRUCT_HANDLE, 0, 0, 0);
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
      MemPtr->DiesPerSystem = (DIE_STRUCT *)AllocHeapParams.BufferPtr;
      //
      //  Find SocketId, DieId, and PCI address of each node
      //
      DieIndex = 0;
      for (Die = 0; Die < MAX_NODES_SUPPORTED; Die++) {
        if (GetSocketModuleOfNode ((UINT32)Die, &SocketId, &DieId, (VOID *)MemPtr)) {
          if (GetPciAddress ((VOID *)MemPtr, (UINT8)SocketId, (UINT8)DieId, &Address, &AgesaStatus)) {
            MemPtr->DiesPerSystem[DieIndex].SocketId = (UINT8)SocketId;
            MemPtr->DiesPerSystem[DieIndex].DieId = (UINT8)DieId;
            MemPtr->DiesPerSystem[DieIndex].PciAddr.AddressValue = Address.AddressValue;

            DieIndex++;
          }
        }
      }
      AgesaStatus = AGESA_SUCCESS;
    } else {
      ASSERT(FALSE); // Heap allocation failed for DIE_STRUCTs
      AgesaStatus = AGESA_FATAL;
    }
  } else {
    ASSERT(FALSE); // No die in the system
    AgesaStatus = AGESA_FATAL;
  }
  return AgesaStatus;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function sets memory errors into MemDataStruct
 *
 *
 *     @param[in,out]   *MCTPtr  - Pointer to the DIE_STRUCT
 *     @param[in]       Errorval - Error value to update
 */

VOID
SetMemError (
  IN       AGESA_STATUS  Errorval,
  IN OUT   DIE_STRUCT *MCTPtr
  )
{
  if (MCTPtr->ErrCode < Errorval) {
    MCTPtr->ErrCode = Errorval;
  }
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function is default function for the fultion list
 *
 *     @param[in,out]   *pMemData   - Pointer to the MEM_DATA_STRUCT
 */
VOID
AmdMemFunctionListDef (
  IN OUT   VOID *pMemData
  )
{
}
