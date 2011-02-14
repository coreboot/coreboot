/* $NoKeywords:$ */
/**
 * @file
 *
 * mnoton.c
 *
 * Northbridge Non-SPD timings for ON
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 39158 $ @e \$Date: 2010-10-07 21:34:36 +0800 (Thu, 07 Oct 2010) $
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnon.h"
#include "mu.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNOTON_FILECODE


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
MemNSetOtherTimingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemNPowerDownCtlON (
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
MemNOtherTimingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetOtherTimingON (NBPtr);
  MemNPowerDownCtlON (NBPtr);

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
 *   This function sets the non-SPD timings into the PCI registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNSetOtherTimingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  INT16 ROD;
  INT16 WOD;
  UINT8 LD;
  UINT8 Tcwl;
  INT16 CDDTrdrd;
  INT16 Trdrd;
  INT16 CDDTwrwr;
  INT16 Twrwr;
  INT16 CDDTwrrdSD;
  INT16 TwrrdSD;
  INT16 CDDTwrrd;
  INT16 Twrrd;
  INT16 CDDTrwtTO;
  INT16 TrwtTO;

  ROD = (DEFAULT_RD_ODT_ON_ON > 6) ? (DEFAULT_RD_ODT_ON_ON - 6) : 0;
  WOD = (DEFAULT_RD_ODT_ON_ON > 6) ? (DEFAULT_WR_ODT_ON_ON - 6) : 0;

  Tcwl = (UINT8) (NBPtr->DCTPtr->Timings.Speed / 133) + 2;
  LD = NBPtr->DCTPtr->Timings.CasL - Tcwl;

  // TrdrdSD = 3
  MemNSetBitFieldNb (NBPtr, BFTrdrdSD, 2 - 2 + 1);

  // Trdrd = CEIL(MAX(ROD + 3, CDDTrdrd/2 + (F2x[94]SlowAccessMode ? 3 : 3.5)))
  CDDTrdrd = (MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessRcvEnDly, FALSE, TRUE) + (INT16) ((NBPtr->ChannelPtr->SlowMode ? 6 : 7) + 1)) / (INT16) 2;
  Trdrd = MAX (ROD + 3, CDDTrdrd);
  Trdrd = MIN (MAX (Trdrd, 2), 10);
  MemNSetBitFieldNb (NBPtr, BFTrdrd, (UINT8) (Trdrd - 2));

  // TwrwrSD = WOD + 3
  MemNSetBitFieldNb (NBPtr, BFTwrwrSD, (WOD + 3 - 1));

  // Twrwr = CEIL(MAX(WOD + 3, CDDTwrwr / 2 + 3.5))
  CDDTwrwr = (MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessWrDqsDly, FALSE, TRUE) + (INT16) (7 + 1)) / (INT16) 2;
  Twrwr = MAX (WOD + 3, CDDTwrwr);
  Twrwr = MIN (MAX (Twrwr, 1), 10);
  MemNSetBitFieldNb (NBPtr, BFTwrwr, (UINT8) (Twrwr - 1));

  // TwrrdSD = CEIL(MAX(1, MAX(WOD, CDDTwrrdSD / 2 + 0.5) - LD + 3))
  CDDTwrrdSD = (MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessRcvEnDly, TRUE, FALSE) + (INT16) (1 + 1)) / (INT16) 2;
  TwrrdSD = MAX (WOD, CDDTwrrdSD) - LD + 3;
  TwrrdSD = MIN (MAX (TwrrdSD, 1), 11);
  MemNSetBitFieldNb (NBPtr, BFTwrrdSD, (UINT8) (TwrrdSD - 1));

  // Twrrd = CEIL(MAX(1, MAX(WOD, CDDTwrrd / 2 + 0.5) - LD + 3))
  CDDTwrrd = (MemNCalcCDDNb (NBPtr, AccessWrDqsDly, AccessRcvEnDly, FALSE, TRUE) + (INT16) (1 + 1)) / (INT16) 2;
  Twrrd = MAX (WOD, CDDTwrrd) - LD + 3;
  Twrrd = MIN (MAX (Twrrd, 1), 11);
  Twrrd = MAX (Twrrd, TwrrdSD);
  MemNSetBitFieldNb (NBPtr, BFTwrrd, (UINT8) (Twrrd - 1));

  // TrwtTO = CEIL(MAX(ROD, CDDTrwtTO / 2 - 0.5) + LD + 3).
  CDDTrwtTO = (MemNCalcCDDNb (NBPtr, AccessRcvEnDly, AccessWrDqsDly, TRUE, TRUE) + (INT16) (1 - 1)) / (INT16) 2;
  TrwtTO = MAX (ROD, CDDTrwtTO) + LD + 3;
  TrwtTO = MIN (MAX (TrwtTO, 3), 17);
  MemNSetBitFieldNb (NBPtr, BFTrwtTO, (UINT8) (TrwtTO - 2));

  // TrwtWB should be set to 0xF for ON.
  MemNSetBitFieldNb (NBPtr, BFTrwtWB, 4);
}

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
MemNPowerDownCtlON (
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
