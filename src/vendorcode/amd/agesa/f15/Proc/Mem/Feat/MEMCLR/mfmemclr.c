/* $NoKeywords:$ */
/**
 * @file
 *
 * mfmemclr.c
 *
 * Feature function for memory clear operation
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Memclr)
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




#include "AGESA.h"
#include "mm.h"
#include "mn.h"
#include "mfmemclr.h"
#include "Ids.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_MEMCLR_MFMEMCLR_FILECODE
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

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Initiates memory clear operation on one node with Dram on it.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemFMctMemClr_Init (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  AGESA_TESTPOINT (TpProcMemMemClr, &NBPtr->MemPtr->StdHeader);
  if (NBPtr->RefPtr->EnableMemClr == TRUE) {
    if (NBPtr->MCTPtr->NodeMemSize != 0) {
      if (!NBPtr->MemCleared) {
        NBPtr->PollBitField (NBPtr, BFMemClrBusy, 0, SPECIAL_PCI_ACCESS_TIMEOUT, FALSE);
        if (NBPtr->GetBitField (NBPtr, BFDramEnabled) == 1) {
          NBPtr->FamilySpecificHook[BeforeMemClr] (NBPtr, NBPtr);
          NBPtr->SetBitField (NBPtr, BFDramBaseAddr, 0);
          NBPtr->SetBitField (NBPtr, BFMemClrInit, 1);
        }
      }
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Ensures memory clear operation has completed on one node with Dram on it.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemFMctMemClr_Sync (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 MicroSecondToWait;

  MicroSecondToWait = 0;
  if (NBPtr->RefPtr->EnableMemClr == TRUE) {
    if (NBPtr->MCTPtr->NodeMemSize != 0) {
      // Calculate Timeout value:
      // Timeout (in microsecond) = Memory Size * 1.5 ns / 8 Byte * 4 (Margin) * 1000 (change millisecond to us)
      // NodeMemSize is system address right shifted by 16, so shift it 4 bits to right to convert it to MB.
      // 1.5 / 8 * 4 * 1000 = 750
      MicroSecondToWait = (NBPtr->MCTPtr->NodeMemSize >> 4) * 750;

      if (!NBPtr->MemCleared) {
        NBPtr->PollBitField (NBPtr, BFMemClrBusy, 0, MicroSecondToWait, FALSE);
        NBPtr->PollBitField (NBPtr, BFMemCleared, 1, MicroSecondToWait, FALSE);
        NBPtr->SetBitField (NBPtr, BFDramBaseAddr, NBPtr->MCTPtr->NodeSysBase >> (27 - 16));
        NBPtr->MemCleared = TRUE;
      }
    }
  }
  return TRUE;
}

