/**
 * @file
 *
 * mfemp.c
 *
 * Feature EMP initialization functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/ECC)
 * @e \$Revision: 6168 $ @e \$Date: 2008-05-28 22:26:15 -0500 (Wed, 28 May 2008) $
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




#include "AGESA.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_FEAT_ECC_MFEMP_FILECODE
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
BOOLEAN
STATIC
IsPowerOfTwo (
  IN       UINT32 Value
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
BOOLEAN
MemFInitEMP (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes EMP (Enhanced Memory Protection)
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInitEMP (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  if (RefPtr->EnableEccFeature) {
    if (NBPtr->GetBitField (NBPtr, BFEnhMemProtCap) == 0) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_EMP_NOT_SUPPORTED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      MCTPtr->ErrStatus[EsbEMPNotSupported] = TRUE;
    } else if (RefPtr->EnableChannelIntlv || RefPtr->EnableBankIntlv || RefPtr->EnableBankSwizzle) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_EMP_CONFLICT, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      MCTPtr->ErrStatus[EsbEMPConflict] = TRUE;
    } else if ((!MCTPtr->GangedMode) &&
               (!IsPowerOfTwo (MCTPtr->DctData[0].Timings.DctMemSize) &&
                !IsPowerOfTwo (MCTPtr->DctData[1].Timings.DctMemSize))) {
      PutEventLog (AGESA_WARNING, MEM_WARNING_EMP_NOT_ENABLED, 0, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      MCTPtr->ErrStatus[EsbEMPDis] = TRUE;
    } else {
      // Reduce memory size to 7/8 of the original memory size
      ASSERT ((MCTPtr->NodeMemSize % 8) == 0);
      NBPtr->SetBitField (NBPtr, BFDramHoleValid, 0);
      MCTPtr->NodeMemSize = (MCTPtr->NodeMemSize / 8) * 7;
      NBPtr->HtMemMapInit (NBPtr);
      NBPtr->CpuMemTyping (NBPtr);

      // Enable EMP
      NBPtr->SetBitField (NBPtr, BFDramEccEn, 1);

      // Scrub CTL settings for Dcache, L2
      NBPtr->SetBitField (NBPtr, BFL2Scrub, UserOptions.CfgScrubL2Rate);
      NBPtr->SetBitField (NBPtr, BFDcacheScrub, UserOptions.CfgScrubDcRate);

      NBPtr->SetBitField (NBPtr, BFSyncOnUcEccEn, UserOptions.CfgEccSyncFlood);
      return TRUE;
    }
  }
  return FALSE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function checks to see if the input is power of two.
 *
 *     @param[in]    Value   - Value to check for power of two
 *
 *     @return          TRUE     - is power of two
 *                      FALSE    - is not power of two
 */
BOOLEAN
STATIC
IsPowerOfTwo (
  IN       UINT32 Value
  )
{
  return (BOOLEAN) ((Value & (Value - 1)) == 0);
}
