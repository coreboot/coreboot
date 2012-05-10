/* $NoKeywords:$ */
/**
 * @file
 *
 * mnotln.c
 *
 * Northbridge Non-SPD timings for LN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/LN)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnln.h"
#include "mu.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_LN_MNOTLN_FILECODE


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
VOID
STATIC
MemNPowerDownCtlLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the non-SPD timings
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNOtherTimingLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  INT16 WOD;
  INT16 ROD;
  UINT8 LD;
  UINT8 Tcwl;
  INT16 CDDTrdrd;
  INT16 CDDTwrwr;
  INT16 CDDTwrrdSD;
  INT16 CDDTwrrd;
  INT16 CDDTrwtTO;
  INT16 Trdrd;
  INT16 Twrwr;
  INT16 TwrrdSD;
  INT16 Twrrd;
  INT16 TrwtTO;

  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_LN; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctDimmValid > 0) {
      // Enable power down
      MemNPowerDownCtlLN (NBPtr);

      // Calculate needed terms
      ROD = DEFAULT_RD_ODT_ON_LN - 6;
      WOD = DEFAULT_WR_ODT_ON_LN - 6;
      Tcwl = (UINT8) (NBPtr->DCTPtr->Timings.Speed / 133) + 2;
      LD = NBPtr->DCTPtr->Timings.CasL - Tcwl;
      CDDTrdrd = (MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessRcvEnDly, FALSE, TRUE) + 1 + 1) / 2;    // +0.5 CLK
      CDDTwrwr = (MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessWrDqsDly, FALSE, TRUE) + 1 + 1) / 2;    // +0.5 CLK
      CDDTwrrdSD = (MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessRcvEnDly, TRUE, FALSE) + 1 + 1) / 2;  // +0.5 CLK
      CDDTwrrd = (MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessRcvEnDly, FALSE, TRUE) + 1 + 1) / 2;    // +0.5 CLK
      CDDTrwtTO = (MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessWrDqsDly, TRUE, TRUE) - 1 + 1) / 2;    // -0.5 CLK

      // Program non-SPD timings
      MemNSetBitFieldNb (NBPtr, BFTrdrdSD, 3 - 2);
      Trdrd = MAX (ROD, CDDTrdrd) + 3;
      ASSERT (Trdrd <= 10);
      MemNSetBitFieldNb (NBPtr, BFTrdrd, (UINT8) (Trdrd - 2));
      // Twrwr and TwrwrSD
      MemNSetBitFieldNb (NBPtr, BFTwrwrSD, WOD + 3 - 1);
      Twrwr = MAX (WOD + 3, CDDTwrwr + 3);
      ASSERT (Twrwr <= 10);
      MemNSetBitFieldNb (NBPtr, BFTwrwr, (UINT8) (Twrwr - 1));
      // Twrrd and TwrrdSD
      TwrrdSD = MAX (1, MAX (WOD, CDDTwrrdSD) - LD + 3);
      ASSERT (TwrrdSD <= 11);
      MemNSetBitFieldNb (NBPtr, BFTwrrdSD, (UINT8) (TwrrdSD - 1));
      Twrrd = MAX (TwrrdSD, MAX (WOD, CDDTwrrd) - LD + 3);
      ASSERT (Twrrd <= 11);
      MemNSetBitFieldNb (NBPtr, BFTwrrd, (UINT8) (Twrrd - 1));
      // TrwtTO and TrwtWB
      TrwtTO = MAX (ROD, CDDTrwtTO) + LD + 3;
      ASSERT (TrwtTO <= 17);
      MemNSetBitFieldNb (NBPtr, BFTrwtTO, (UINT8) (TrwtTO - 2));
      MemNSetBitFieldNb (NBPtr, BFTrwtWB, 0x4);
    }
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function enables power down mode
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNPowerDownCtlLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (NBPtr->RefPtr->EnablePowerDown) {
    MemNSetTxpNb (NBPtr);
    MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 1);
  }

  if (NBPtr->RefPtr->EnableBankSwizzle) {
    MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 1);
  }
}



