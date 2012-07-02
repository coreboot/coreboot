/* $NoKeywords:$ */
/**
 * @file
 *
 * mfchi.c
 *
 * Feature Channel interleaving support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Chintlv)
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
#include "mm.h"
#include "mn.h"
#include "mfchi.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_CHINTLV_MFCHI_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_ (0x10000ul >> 10)

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
 *   MemFInterleaveChannels:
 *
 *  Applies DIMM channel interleaving if enabled, if not ganged mode, and
 *  there are valid dimms in both channels.  Called once per Node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveChannels (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 DramBase;
  UINT32 DctSelBase;
  UINT32 HoleSize;
  UINT32 HoleBase;
  UINT32 HoleOffset;
  UINT32 Dct0Size;
  UINT32 Dct1Size;
  UINT32 SmallerDct;
  UINT8 DctSelIntLvAddr;
  UINT8 DctSelHi;
  UINT8 DctSelHiRngEn;
  UINT32 HoleValid;

  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;

  DctSelIntLvAddr = NBPtr->DefDctSelIntLvAddr;
  if (RefPtr->EnableChannelIntlv) {
    HoleSize = 0;
    HoleBase = 0;
    if (RefPtr->GStatus[GsbSoftHole] || RefPtr->GStatus[GsbHWHole]) {
      // HoleBase scaled from [47:16] to [47:26]
      HoleBase = RefPtr->HoleBase >> 10;
      HoleSize = _4GB_ - HoleBase;
    }

    MCTPtr = NBPtr->MCTPtr;

    HoleValid = NBPtr->GetBitField (NBPtr, BFDramHoleValid);
    if ((!MCTPtr->GangedMode) &&
        (MCTPtr->DctData[0].Timings.DctMemSize != 0) &&
        (MCTPtr->DctData[1].Timings.DctMemSize != 0)) {
      // DramBase scaled [47:16] to [47:26]
      DramBase = MCTPtr->NodeSysBase >> 10;
      // Scale NodeSysLimit [47:16] to [47:26]
      Dct1Size = (MCTPtr->NodeSysLimit + 1) >> 10;
      Dct0Size = NBPtr->GetBitField (NBPtr, BFDctSelBaseOffset);
      if ((Dct0Size >= _4GB_) && (DramBase < HoleBase)) {
        Dct0Size -= HoleSize;
      }
      if ((Dct1Size >= _4GB_) && (DramBase < HoleBase)) {
        Dct1Size -= HoleSize;
      }
      Dct1Size -= Dct0Size;
      Dct0Size -= DramBase;

       // Select the bigger size DCT to put in DctSelHi
      DctSelHiRngEn = 1;
      DctSelHi = 0;
      SmallerDct = Dct1Size;
      if (Dct1Size == Dct0Size) {
        SmallerDct = 0;
        DctSelHiRngEn = 0;
      } else if (Dct1Size > Dct0Size) {
        SmallerDct = Dct0Size;
        DctSelHi = 1;
      }

      if (SmallerDct != 0) {
        DctSelBase = (SmallerDct * 2) + DramBase;
      } else {
        DctSelBase = 0;
      }
      if ((DctSelBase >= HoleBase) && (DramBase < HoleBase)) {
        DctSelBase += HoleSize;
      }
      IDS_OPTION_HOOK (IDS_CHANNEL_INTERLEAVE, &DctSelIntLvAddr, &(NBPtr->MemPtr->StdHeader));
      NBPtr->SetBitField (NBPtr, BFDctSelBaseAddr, DctSelBase >> 1);
      NBPtr->SetBitField (NBPtr, BFDctSelHiRngEn, DctSelHiRngEn);
      NBPtr->SetBitField (NBPtr, BFDctSelHi, DctSelHi);
      NBPtr->SetBitField (NBPtr, BFDctSelIntLvAddr, DctSelIntLvAddr);
      NBPtr->SetBitField (NBPtr, BFDctSelIntLvEn, 1);

       // DctSelBaseOffset = DctSelBaseAddr - Interleaved region
      NBPtr->SetBitField (NBPtr, BFDctSelBaseOffset, DctSelBase - SmallerDct);

       // Adjust DramHoleOffset
      if (HoleValid != 0) {
        HoleOffset = DramBase;
        if ((DctSelBase < HoleBase) && (DctSelBase != 0)) {
          HoleOffset += (DctSelBase - DramBase) >> 1;
        }
        HoleOffset += HoleSize;
        NBPtr->SetBitField (NBPtr, BFDramHoleOffset, HoleOffset << 3);
      }
    } else {
      //
      // Channel Interleaving is requested but cannot be enabled
      //
      PutEventLog (AGESA_WARNING, MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED, NBPtr->Node, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, MCTPtr);
    }

    return TRUE;
  } else {
    return FALSE;
  }
}
