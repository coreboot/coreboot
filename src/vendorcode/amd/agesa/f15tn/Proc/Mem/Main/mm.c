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
