/* $NoKeywords:$ */
/**
 * @file
 *
 * mfdimmexclud.c
 *
 * Feature DIMM exclude.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/EXCLUDIMM)
 * @e \$Revision: 47509 $ @e \$Date: 2011-02-23 06:15:32 +0800 (Wed, 23 Feb 2011) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */


#include "AGESA.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_EXCLUDIMM_MFDIMMEXCLUD_FILECODE

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
BOOLEAN
MemFRASExcludeDIMM (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check and disable Chip selects that fail training for each node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */
BOOLEAN
MemFRASExcludeDIMM (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 ReserveDCT;
  UINT8 q;
  BOOLEAN Flag;
  BOOLEAN IsCSIntlvEnabled;
  UINT16 CsTestFail;
  DIE_STRUCT *MCTPtr;
  BOOLEAN RetVal;

  ASSERT (NBPtr != NULL);
  ReserveDCT = NBPtr->Dct;
  CsTestFail = 0;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.CsTestFail != 0) {
      // When there is no new failed dimm that needs to be excluded, then no need to go through the process.
      switch (NBPtr->SharedPtr->DimmExcludeFlag) {
      case NORMAL:
        // See there is new dimm that needs to be excluded
        if ((NBPtr->DCTPtr->Timings.CsTestFail & NBPtr->DCTPtr->Timings.CsEnabled) != 0) {
          CsTestFail |= NBPtr->DCTPtr->Timings.CsTestFail;
        }
        break;
      case TRAINING:
        // Do not do any dimm excluding during training
        // Dimm exclude will be done at the end of training
        break;
      case END_TRAINING:
        // Exclude all dimms that have failures during training
        if ((NBPtr->DCTPtr->Timings.CsTrainFail != 0) ||
            ((NBPtr->DCTPtr->Timings.CsTestFail & NBPtr->DCTPtr->Timings.CsEnabled) != 0)) {
          CsTestFail |= NBPtr->DCTPtr->Timings.CsTestFail;
        }
        break;
      default:
        IDS_ERROR_TRAP;
      }
    }
  }

  if (CsTestFail != 0) {
    IsCSIntlvEnabled = FALSE;
    MCTPtr = NBPtr->MCTPtr;
    MCTPtr->NodeMemSize = 0;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].IsValid = FALSE;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysBase = 0;
    NBPtr->SharedPtr->NodeMap[NBPtr->Node].SysLimit = 0;
    NBPtr->SetBitField (NBPtr, BFDramBaseAddr, 0);
    NBPtr->SetBitField (NBPtr, BFDramLimitAddr, 0);

    if (MCTPtr->GangedMode) {
      // if ganged mode, disable all pairs of CS that fail.
      NBPtr->DCTPtr->Timings.CsTestFail |= CsTestFail;
    }

    // if chip select interleaving has been enabled, need to undo it before remapping memory
    if (NBPtr->FeatPtr->UndoInterleaveBanks (NBPtr)) {
      IsCSIntlvEnabled = TRUE;
    }

    Flag = TRUE;
    NBPtr->FamilySpecificHook[BfAfExcludeDimm] (NBPtr, &Flag);
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (!MCTPtr->GangedMode || (MCTPtr->Dct == 0)) {
        if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
          NBPtr->DCTPtr->Timings.DctMemSize = 0;

          NBPtr->DCTPtr->Timings.CsEnabled = 0;
          for (q = 0; q < MAX_CS_PER_CHANNEL; q++) {
            NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + q, 0);
          }

          Flag = NBPtr->StitchMemory (NBPtr);
          ASSERT (Flag == TRUE);
        }
      }
    }
    Flag = FALSE;
    NBPtr->FamilySpecificHook[BfAfExcludeDimm] (NBPtr, &Flag);

    // Re-enable chip select interleaving when remapping is done.
    if (IsCSIntlvEnabled) {
      NBPtr->FeatPtr->InterleaveBanks (NBPtr);
    }

    RetVal = TRUE;
  } else {
    RetVal = FALSE;
  }
  NBPtr->SwitchDCT (NBPtr, ReserveDCT);
  return RetVal;
}

