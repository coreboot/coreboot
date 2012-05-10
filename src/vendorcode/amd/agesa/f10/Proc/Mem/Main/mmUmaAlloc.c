/**
 * @file
 *
 * mmUmaAlloc.c
 *
 * Main Memory Feature implementation file for UMA allocation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 6474 $ @e \$Date: 2008-06-20 03:07:59 -0500 (Fri, 20 Jun 2008) $
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
#include "amdlib.h"
#include "heapManager.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "mport.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_MAIN_MMUMAALLOC_FILECODE
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
/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
extern BUILD_OPT_CFG UserOptions;

BOOLEAN
MemMUmaAlloc (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *    UMA allocation mechanism.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemMUmaAlloc (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT32 TOM;
  UINT32 TOM2;
  UINT32 UmaSize;
  UINT32 TopOfChIntlv;
  UINT32 UmaAlignment;
  UINT32 UmaAbove4GBase;
  UINT32 UmaBelow4GBase;
  BOOLEAN DctSelIntLvEn;
  BOOLEAN UmaAbove4GEn;
  S_UINT64 SMsr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UMA_INFO *UmaInfoPtr;

  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  MemPtr = MemMainPtr->MemPtr;
  NBPtr = &(MemMainPtr->NBPtr[BSP_DIE]);
  RefPtr = NBPtr->RefPtr;

  TOM2 = 0;
  SMsr.lo = SMsr.hi = 0;
  UmaAbove4GBase = 0;
  RefPtr->UmaBase = 0;
  UmaAlignment = (UINT32) UserOptions.CfgUmaAlignment;
  UmaAbove4GEn = UserOptions.CfgUmaAbove4G;
  DctSelIntLvEn = (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) == 1) ? TRUE : FALSE;
  TopOfChIntlv = NBPtr->GetBitField (NBPtr, BFDctSelBaseAddr) << (27 - 16);

  // Allocate heap for UMA_INFO
  AllocHeapParams.RequestedBufferSize = sizeof (UMA_INFO);
  AllocHeapParams.BufferHandle = AMD_UMA_INFO_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    ASSERT(FALSE); // Could not allocate heap for Uma information.
    return FALSE;
  }
  UmaInfoPtr = (UMA_INFO *) AllocHeapParams.BufferPtr;
  // Default all the fields of UMA_INFO
  UmaInfoPtr->UmaMode = (UINT8) UMA_NONE;
  UmaInfoPtr->UmaSize = 0;
  UmaInfoPtr->UmaBase = 0;
  UmaInfoPtr->UmaAttributes = 0;
  UmaInfoPtr->MemClock = NBPtr->DCTPtr->Timings.TargetSpeed;

  switch (RefPtr->UmaMode) {
  case UMA_NONE:
    UmaSize = 0;
    break;
  case UMA_SPECIFIED:
    UmaSize = RefPtr->UmaSize;
    break;
  case UMA_AUTO:
    UmaSize = NBPtr->GetUmaSize (NBPtr);
    break;
  default:
    UmaSize = 0;
    IDS_ERROR_TRAP;
  }

  if (UmaSize != 0) {
    //TOM scaled from [47:0] to [47:16]
    LibAmdMsrRead (TOP_MEM, (UINT64 *)&SMsr, &(NBPtr->MemPtr->StdHeader));
    TOM = (SMsr.lo >> 16) | (SMsr.hi << (32 - 16));

    UmaBelow4GBase = (TOM - UmaSize) & UmaAlignment;
    // Initialize Ref->UmaBase to UmaBelow4GBase
    RefPtr->UmaBase = UmaBelow4GBase;

    // Uma Above 4G support
    if (UmaAbove4GEn) {
      //TOM2 scaled from [47:0] to [47:16]
      LibAmdMsrRead (TOP_MEM2, (UINT64 *)&SMsr, &(NBPtr->MemPtr->StdHeader));
      TOM2 = (SMsr.lo >> 16) | (SMsr.hi << (32 - 16));
      if (TOM2 != 0) {
        UmaAbove4GBase = (TOM2 - UmaSize) & UmaAlignment;
        //Set UmaAbove4GBase to 0 if UmaAbove4GBase is below 4GB
        if (UmaAbove4GBase < _4GB_RJ16) {
          UmaAbove4GBase = 0;
        }
        if (UmaAbove4GBase != 0) {
          RefPtr->UmaBase = UmaAbove4GBase;
          // 1. TopOfChIntlv == 0 indicates that whole DCT0 and DCT1 memory are interleaved.
          // 2. TopOfChIntlv >= TOM tells us :
          //   -All or portion of Uma region that above 4G is NOT interleaved.
          //   -Whole Uma region that below 4G is interleaved.
          if (DctSelIntLvEn && (TopOfChIntlv >= TOM)) {
            RefPtr->UmaBase = UmaBelow4GBase;
          }
        }
      }
    }

    UmaInfoPtr->UmaMode = (UINT8) (RefPtr->UmaMode);
    UmaInfoPtr->UmaBase = (UINT64) ((UINT64) RefPtr->UmaBase << 16);

    if (RefPtr->UmaBase >= _4GB_RJ16) {
      // UmaSize might be extended if it is 128MB or 256MB .. aligned, so update it.
      RefPtr->UmaSize = TOM2 - UmaAbove4GBase;
      // Uma Typing
      MemNSetMTRRUmaRegionUCNb (NBPtr, &UmaAbove4GBase, &TOM2);
      if (DctSelIntLvEn && (TopOfChIntlv == 0)) {
        UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE;
      }
    } else {
      // UmaSize might be extended if it is 128MB or 256MB .. aligned, so update it.
      RefPtr->UmaSize = TOM - UmaBelow4GBase;
      // Uma Typing
      NBPtr->UMAMemTyping (NBPtr);
      if (DctSelIntLvEn && ((TopOfChIntlv == 0) || (TopOfChIntlv >= TOM))) {
        UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE;
      }
    }
    UmaInfoPtr->UmaSize = (RefPtr->UmaSize) << 16;
  }

  return TRUE;
}

