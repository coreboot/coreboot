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
BOOLEAN
MemFRASExcludeDIMM (
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

          // Set F2x94[DisDramInterface] = 1 if all chip selects fail training on the DCT
          if ((NBPtr->DCTPtr->Timings.CsPresent & ~NBPtr->DCTPtr->Timings.CsTestFail) == 0) {
            NBPtr->DisableDCT (NBPtr);
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

