/**
 * @file
 *
 * mrm.c
 *
 * Main configuration for Recovery mode
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
#include "Ids.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "heapManager.h"
#include "cpuServices.h"
#include "OptionMemoryRecovery.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_MRM_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DIES_PER_SOCKET     2

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
extern MEM_REC_NB_CONSTRUCTOR* MemRecNBInstalled[];
extern MEM_REC_TECH_CONSTRUCTOR* MemRecTechInstalled[];

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

VOID
STATIC
MemRecSPDDataProcess (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the Recovery memory configuration function for HY DDR3
 *
 *      Requirements:
 *
 *      Run-Time Requirements:
 *      1. Complete Hypertransport Bus Configuration
 *      2. AmdMemInitDataStructDef must be run to set default values
 *      3. MSR bit to allow access to high PCI regs set on all nodes
 *      4. BSP in Big Real Mode
 *      5. Stack available
 *      6. MCG_CTL=-1, MC4_EN=0 for all CPUs
 *      7. MCi_STS from shutdown/warm reset recorded (if desired) prior to entry
 *      8. All var MTRRs reset to zero
 *      9. State of NB_CFG.DisDatMsk set properly on all CPUs
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
AmdMemRecovery (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Socket;
  UINT8 Module;
  UINT8 i;
  AGESA_STATUS AgesaStatus;
  PCI_ADDR Address;
  MEM_NB_BLOCK NBBlock;
  MEM_TECH_BLOCK TechBlock;
  LOCATE_HEAP_PTR  SocketWithMem;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;

  IDS_HDT_CONSOLE_INIT (&MemPtr->StdHeader);

  //
  // Read SPD data
  //
  MemRecSPDDataProcess (MemPtr);

  //
  // Get the socket id from heap.
  //
  SocketWithMem.BufferHandle = AMD_REC_MEM_SOCKET_HANDLE;
  if (HeapLocateBuffer (&SocketWithMem, &MemPtr->StdHeader) == AGESA_SUCCESS) {
    Socket = *(UINT8 *) SocketWithMem.BufferPtr;
  } else {
    ASSERT(FALSE);  // Socket handle not found
    return AGESA_FATAL;
  }

  //
  // Allocate buffer for memory init structures
  //
  AllocHeapParams.RequestedBufferSize = MAX_DIES_PER_SOCKET * sizeof (DIE_STRUCT);
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_DIE_STRUCT_HANDLE, 0, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) != AGESA_SUCCESS) {
    ASSERT(FALSE); // Heap allocation failed to allocate Die struct
    return AGESA_FATAL;
  }
  MemPtr->DieCount = MAX_DIES_PER_SOCKET;
  MemPtr->DiesPerSystem = (DIE_STRUCT *)AllocHeapParams.BufferPtr;

  //
  // Discover populated CPUs
  //
  for (Module = 0; Module < MAX_DIES_PER_SOCKET; Module++) {
    if (GetPciAddress ((VOID *)MemPtr, Socket, Module, &Address, &AgesaStatus)) {
      MemPtr->DiesPerSystem[Module].SocketId = Socket;
      MemPtr->DiesPerSystem[Module].DieId = Module;
      MemPtr->DiesPerSystem[Module].PciAddr.AddressValue = Address.AddressValue;
    }
  }

  i = 0;
  while (MemRecNBInstalled[i] != NULL) {
    if (MemRecNBInstalled[i] (&NBBlock, MemPtr, 0) == TRUE) {
      break;
    }
    i++;
  };
  if (MemRecNBInstalled[i] == NULL) {
    ASSERT(FALSE);    // No NB installed
    return AGESA_FATAL;
  }
  MemRecTechInstalled[0] (&TechBlock, &NBBlock);
  NBBlock.TechPtr = &TechBlock;

  IDS_HDT_CONSOLE_EXIT (&MemPtr->StdHeader);

  return NBBlock.InitRecovery (&NBBlock);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function fills a default SPD buffer with SPD values for all DIMMs installed in the system
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *
 */

VOID
STATIC
MemRecSPDDataProcess (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  BOOLEAN FindSocketWithMem;
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 MaxSockets;
  UINT8 *SocketWithMem;
  UINT8 Socket;
  AGESA_STATUS AgesaStatus;
  SPD_DEF_STRUCT *DimmSPDPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  AGESA_READ_SPD_PARAMS SpdParam;
  ASSERT (MemPtr != NULL);
  FindSocketWithMem = FALSE;
  //
  // Allocate heap to save socket number with memory on it.
  //
  AllocHeapParams.RequestedBufferSize = sizeof (UINT8);
  AllocHeapParams.BufferHandle = AMD_REC_MEM_SOCKET_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
    SocketWithMem = (UINT8 *) AllocHeapParams.BufferPtr;
    *SocketWithMem = 0;

    //
    // Allocate heap for the table
    //
    MaxSockets = (UINT8) GetPlatformNumberOfSockets ();

    AllocHeapParams.RequestedBufferSize = (MaxSockets * MAX_CHANNELS_PER_SOCKET * MAX_DIMMS_PER_CHANNEL * sizeof (SPD_DEF_STRUCT));
    AllocHeapParams.BufferHandle = AMD_MEM_SPD_HANDLE;
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
      MemPtr->SpdDataStructure = (SPD_DEF_STRUCT *) AllocHeapParams.BufferPtr;
      //
      // Initialize SpdParam Structure
      //
      LibAmdMemCopy ((VOID *)&SpdParam, (VOID *)MemPtr, (UINTN)sizeof (SpdParam.StdHeader), &MemPtr->StdHeader);
      //
      // Populate SPDDataBuffer
      //

      SpdParam.MemData = MemPtr;
      for (Socket = 0; Socket < MaxSockets; Socket ++) {
        SpdParam.SocketId = Socket;
        for (Channel = 0; Channel < MAX_CHANNELS_PER_SOCKET; Channel++) {
          SpdParam.MemChannelId = Channel;
          for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
            SpdParam.DimmId = Dimm;
            DimmSPDPtr = &(MemPtr->SpdDataStructure[(Socket * MAX_CHANNELS_PER_SOCKET + Channel) * MAX_DIMMS_PER_CHANNEL + Dimm]);
            SpdParam.Buffer = DimmSPDPtr->Data;
            AgesaStatus = AgesaReadSpdRecovery (0, &SpdParam);
            if (AgesaStatus == AGESA_SUCCESS) {
              DimmSPDPtr->DimmPresent = TRUE;
              if (!FindSocketWithMem) {
                FindSocketWithMem = TRUE;
              }
            } else {
              DimmSPDPtr->DimmPresent = FALSE;
            }
          }
        }
        if (FindSocketWithMem) {
          *SocketWithMem = Socket;
          break;
        }
      }
    }
  }
}
