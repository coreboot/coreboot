/* $NoKeywords:$ */
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
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

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

BOOLEAN
MemFUndoInterleaveBanks (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

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
  UINT8 Offset;
  UINT8 Dct;

  ASSERT (NBPtr != NULL);

  Dct = NBPtr->Dct;

  // Check if CS interleaving can be enabled
  EnChipSels = 0;
  BankEncd0 = 0xFF;
  Offset = 0;
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
    // Family specific CS interleaving low address adjustment
    NBPtr->FamilySpecificHook[AdjustCSIntLvLowAddr] (NBPtr, &i);

    if (NBPtr->MCTPtr->Status[Sb128bitmode]) {
      i++;
      j++;
    }

    for (Cs = 0; Cs < MAX_CS_PER_CHANNEL; Cs += 2) {
      //
      // LRDIMMS - Add an offset to the bit positions specified based on D18F2x[6C:60]_dct[1:0][RankDef] as follows:
      //    RankDef=0xb: 0 RankDef=10b: 1 RankDef=11b: 2
      //    Using RankMult information:  Lo/HiBit <<= (Mult >> 1)
      //
      if (NBPtr->MCTPtr->Status[SbLrdimms]) {
        Offset = ((NBPtr->ChannelPtr->LrDimmRankMult[Cs >> 1]) >> 1);
      }
      BaseRegS0 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs);
      BaseRegS1 = NBPtr->GetBitField (NBPtr, BFCSBaseAddr0Reg + Cs + 1);
      if (((BaseRegS0 | BaseRegS1) & 1) != 0) {
        // Swap Mask register bits
        MaskReg = NBPtr->GetBitField (NBPtr, BFCSMask0Reg + (Cs / 2));
        CsIntSwap (&MaskReg, EnChipSels, (i + Offset), (j + Offset));
        NBPtr->SetBitField (NBPtr, BFCSMask0Reg + (Cs / 2), MaskReg);

        // Swap Base register bits
        CsIntSwap (&BaseRegS0, EnChipSels, (i + Offset), (j + Offset));
        NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + Cs, BaseRegS0);
        CsIntSwap (&BaseRegS1, EnChipSels, (i + Offset), (j + Offset));
        NBPtr->SetBitField (NBPtr, BFCSBaseAddr0Reg + Cs + 1, BaseRegS1);
      }
    }
    //
    // Bank Interleaving is requested and has been enabled as well
    //
    NBPtr->MCTPtr->DctData[Dct].BkIntDis = FALSE;
    return TRUE;
  } else {
    //
    // Bank Interleaving is requested but cannot be enabled
    //
    PutEventLog (AGESA_WARNING, MEM_WARNING_BANK_INTERLEAVING_NOT_ENABLED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    NBPtr->MCTPtr->DctData[Dct].BkIntDis = TRUE;
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
