/* $NoKeywords:$ */
/**
 * @file
 *
 * mfspr.c
 *
 * Feature enable online spare
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Olspare)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
 *
 **/
/*
 *****************************************************************************
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
#include "mm.h"
#include "mn.h"
#include "mfspr.h"
#include "Ids.h"
#include "amdlib.h"
#include "Filecode.h"
#include "GeneralServices.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_OLSPARE_MFSPR_FILECODE
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
 *  Enable online spare on current node if it is requested.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFOnlineSpare (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 q;
  UINT8  Value8;
  BOOLEAN Flag;
  BOOLEAN OnlineSprEnabled[MAX_CHANNELS_PER_SOCKET];

  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;
  Flag = FALSE;
  if (RefPtr->EnableOnLineSpareCtl != 0) {
    RefPtr->GStatus[GsbEnDIMMSpareNW] = TRUE;
    MCTPtr = NBPtr->MCTPtr;

    // Check if online spare can be enabled on current node
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      ASSERT (Dct < sizeof (OnlineSprEnabled));
      NBPtr->SwitchDCT (NBPtr, Dct);
      OnlineSprEnabled[Dct] = FALSE;
      if ((MCTPtr->GangedMode == 0) || (MCTPtr->Dct == 0)) {
        if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
          // Make sure at least two chip-selects are available
          Value8 = LibAmdBitScanReverse (NBPtr->DCTPtr->Timings.CsEnabled);
          if (Value8 > LibAmdBitScanForward (NBPtr->DCTPtr->Timings.CsEnabled)) {
            OnlineSprEnabled[Dct] = TRUE;
            Flag = TRUE;
          } else {
            PutEventLog (AGESA_ERROR, MEM_ERROR_DIMM_SPARING_NOT_ENABLED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
            MCTPtr->ErrStatus[EsbSpareDis] = TRUE;
          }
        }
      }
    }

    // If we don't have spared rank on any DCT, we don't run the rest part of the code.
    if (!Flag) {
      return FALSE;
    }

    MCTPtr->NodeMemSize = 0;
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (OnlineSprEnabled[Dct]) {
        // Only run StitchMemory if we need to set a spare rank.
        NBPtr->DCTPtr->Timings.DctMemSize = 0;
        for (q = 0; q < MAX_CS_PER_CHANNEL; q++) {
          NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + q, 0);
        }
        Flag = NBPtr->StitchMemory (NBPtr);
        ASSERT (Flag == TRUE);
      } else if ((MCTPtr->GangedMode == 0) && (NBPtr->DCTPtr->Timings.DctMemSize != 0)) {
        // Otherwise, need to adjust the memory size on the node.
        MCTPtr->NodeMemSize += NBPtr->DCTPtr->Timings.DctMemSize;
        MCTPtr->NodeSysLimit = MCTPtr->NodeMemSize - 1;
      }
    }
    return TRUE;
  } else {
    return FALSE;
  }
}
