/**
 * @file
 *
 * mfcsi.c
 *
 * Feature bank interleaving support (AKA Chip Select Interleaving )
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Csintlv)
 * @e \$Revision: 55148 $ @e \$Date: 2011-06-16 16:16:12 -0600 (Thu, 16 Jun 2011) $
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


/* This file contains functions for Chip Select interleaving */



#include "AGESA.h"
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mfcsi.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_FEAT_CSINTLV_MFCSI_FILECODE
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
MemFDctInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
CsIntSwap (
  IN OUT   UINT32 *BaseMaskRegPtr,
  IN       UINT8 EnChipSels,
  IN       UINT8 LoBit,
  IN       UINT8 HiBit
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function Applies DIMM bank (chip-select) interleaving if enabled
 *      and if all criteria are met.  Interleaves chip-selects on page boundaries.
 *      This function calls subfunctions that sets up CS interleaving on multiple Sockets
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  BOOLEAN RetFlag;

  ASSERT (NBPtr != NULL);

  RetFlag = FALSE;
  if (NBPtr->RefPtr->EnableBankIntlv) {
    if (NBPtr->MCTPtr->NodeMemSize) {
      for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
        NBPtr->SwitchDCT (NBPtr, Dct);
        RetFlag |= MemFDctInterleaveBanks (NBPtr);
      }
    }
  }
  return RetFlag;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *     This function checks if bank interleaving has been enabled or not. If yes, it will
 *     undo bank interleaving. Otherwise, it does nothing.
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Bank interleaving has been enabled.
 *     @return          FALSE - Bank interleaving has not been enabled.
 */

BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Cs;
  UINT8 Dct;
  UINT32 CSMask;
  BOOLEAN CSIntlvEnabled;
  BOOLEAN RetFlag;

  ASSERT (NBPtr != NULL);

  RetFlag = FALSE;

  if (NBPtr->RefPtr->EnableBankIntlv) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize) {
        CSIntlvEnabled = FALSE;
        for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs++) {
          if ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs) & 1) != 0) {
            CSMask = NBPtr->GetBitField (NBPtr, BFCSMask0Reg + (Cs / 2));
            if (((CSMask >> 5) & 0x1FF) != 0x1FF) {
              CSIntlvEnabled = TRUE;
              break;
            }
          }
        }
        if (CSIntlvEnabled) {
          MemFDctInterleaveBanks (NBPtr);
          RetFlag = TRUE;
        }
      }
    }
  }
  return RetFlag;
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
 *      This function Applies DIMM bank (chip-select) interleaving if enabled
 *      and if all criteria are met.  Interleaves chip-selects on page boundaries.
 *      This function is run once per Socket
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  Register bits have been swapped.
 *     @return          FALSE - Register bits have not been swapped.
 *
 */

BOOLEAN
STATIC
MemFDctInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Cs;
  UINT8 EnChipSels;
  UINT8 BankEncd;
  UINT8 BankEncd0;
  UINT8 i;
  UINT8 j;
  UINT32 BankAddrReg;
  UINT32 BaseRegS0;
  UINT32 BaseRegS1;
  UINT32 MaskReg;

  ASSERT (NBPtr != NULL);

  // Check if CS interleaving can be enabled
  EnChipSels = 0;
  BankEncd0 = 0xFF;
  for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs++) {
    if ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs) & 3) != 0) {
      BankAddrReg = NBPtr->GetBitField (NBPtr, BFDramBankAddrReg);
      BankEncd = (UINT8) ((BankAddrReg >> ((Cs / 2) * 4)) & 0xF);
      if (BankEncd0 == 0xFF) {
        BankEncd0 = BankEncd;
      } else if (BankEncd0 != BankEncd) {
        break;
      }
      if ((NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs) & 1) != 0) {
        EnChipSels++;
      }
    }
  }

  // Swap Dram Base/Mask Addr to enable CS interleaving
  if ((Cs == MAX_CS_PER_CHANNEL) && ((EnChipSels == 2) || (EnChipSels == 4) || (EnChipSels == 8))) {
    NBPtr->TechPtr->GetCSIntLvAddr (BankEncd0, &i, &j);
    if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
      i++;
      j++;
    }

    for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs += 2) {
      BaseRegS0 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs);
      BaseRegS1 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs + 1);
      if (((BaseRegS0 | BaseRegS1) & 1) != 0) {
        // Swap Mask register bits
        MaskReg = NBPtr->GetBitField (NBPtr, BFCSMask0Reg + (Cs / 2));
        CsIntSwap (&MaskReg, EnChipSels, i, j);
        NBPtr->SetBitField (NBPtr, BFCSMask0Reg + (Cs / 2), MaskReg);

        // Swap Base register bits
        CsIntSwap (&BaseRegS0, EnChipSels, i, j);
        NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + Cs, BaseRegS0);
        CsIntSwap (&BaseRegS1, EnChipSels, i, j);
        NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + Cs + 1, BaseRegS1);
      }
    }
    return TRUE;
  } else {
    //
    // Bank Interleaving is requested but cannot be enabled
    //
    PutEventLog (AGESA_WARNING, MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    NBPtr->MCTPtr->ErrStatus[EsbBkIntDis] = TRUE;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This supporting function swaps Chip selects
 *
 *     @param[in,out]  *BaseMaskRegPtr   - Pointer to the Mask Register
 *     @param[in]      *EnChipSels    - Chip Selects to Enable
 *     @param[in]      *LoBit         - Lowest Bit
 *     @param[in]      *HiBit         - Highest Bit
 *
 *
 */

VOID
STATIC
CsIntSwap (
  IN OUT   UINT32 *BaseMaskRegPtr,
  IN       UINT8 EnChipSels,
  IN       UINT8 LoBit,
  IN       UINT8 HiBit
  )
{
  UINT8 BitDelta;
  UINT32 TempHi;
  UINT32 TempLo;
  UINT32 AddrLoMask;
  UINT32 AddrHiMask;

  ASSERT (BaseMaskRegPtr != NULL);
  ASSERT (HiBit > LoBit);

  BitDelta = HiBit - LoBit;
  AddrLoMask = (((UINT32)EnChipSels) - 1) << LoBit;
  AddrHiMask = AddrLoMask << BitDelta;

  TempHi = TempLo = *BaseMaskRegPtr;
  TempLo &= AddrLoMask;
  TempLo <<= BitDelta;     // move lower bits to upper bit position
  TempHi &= AddrHiMask;
  TempHi >>= BitDelta;     // move upper bits to lower bit position

  *BaseMaskRegPtr &= ~AddrLoMask;
  *BaseMaskRegPtr &= ~AddrHiMask;
  *BaseMaskRegPtr |= TempLo | TempHi;
}
