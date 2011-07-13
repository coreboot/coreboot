/**
 * @file
 *
 * mfrintlv.c
 *
 * Feature Region interleaving support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Intlvrgn)
 * @e \$Revision: 9110 $ @e \$Date: 2008-10-28 01:16:14 +0800 (Tue, 28 Oct 2008) $
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mfintlvrn.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_FEAT_INTLVRN_MFINTLVRN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_RJ27 ((UINT32)4 << (30 - 27))
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
 *   MemFInterleaveRegion:
 *
 *  Applies region interleaving if both DCTs have different size of memory, and
 *  the channel interleaving region doesn't have UMA covered.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemFInterleaveRegion (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 TOM;
  UINT32 TOM2;
  UINT32 TOMused;
  UINT32 UmaBase;
  UINT32 DctSelBase;
  S_UINT64 SMsr;

  MEM_DATA_STRUCT *MemPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  MemPtr = NBPtr->MemPtr;
  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  UmaBase = (UINT32) RefPtr->UmaBase >> (27 - 16);

  //TOM scaled from [47:0] to [47:27]
  LibAmdMsrRead (TOP_MEM, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  TOM = (SMsr.lo >> 27) | (SMsr.hi << (32 - 27));

  //TOM2 scaled from [47:0] to [47:27]
  LibAmdMsrRead (TOP_MEM2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  TOM2 = (SMsr.lo >> 27) | (SMsr.hi << (32 - 27));

  TOMused = (UmaBase >= _4GB_RJ27) ? TOM2 : TOM;

  if (UmaBase != 0) {
    //Check if channel interleaving is enabled ? if so, go to next step.
    if (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) == 1) {
      DctSelBase = NBPtr->GetBitField (NBPtr, BFDctSelBaseAddr);
      //Skip if DctSelBase is equal to 0, because DCT0 has as the same memory size as DCT1.
      if (DctSelBase != 0) {
        //We need not enable swapped interleaved region when channel interleaving region has covered all of the UMA.
        if (DctSelBase < TOMused) {
          IDS_HDT_CONSOLE ("@\tStarting Interleave Region for Socket %d Die %d\n", NBPtr->MCTPtr->SocketId, NBPtr->Node);
          NBPtr->EnableSwapIntlvRgn (NBPtr, UmaBase, TOMused);
        }
      }
    }
  }
}


