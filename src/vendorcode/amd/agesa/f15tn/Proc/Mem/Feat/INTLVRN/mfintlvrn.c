/* $NoKeywords:$ */
/**
 * @file
 *
 * mfrintlv.c
 *
 * Feature Region interleaving support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Intlvrgn)
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
#include "amdlib.h"
#include "heapManager.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mfintlvrn.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_INTLVRN_MFINTLVRN_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_RJ27 ((UINT32)4 << (30 - 27))
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
 *   MemFInterleaveRegion:
 *
 *  Applies region interleaving if both DCTs have different size of memory, and
 *  the channel interleaving region doesn't have UMA covered.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemFInterleaveRegion (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 TOM;
  UINT32 TOM2;
  UINT32 TOMused;
  UINT32 UmaBase;
  UINT32 DctSelBase;
  S_UINT64 SMsr;
  LOCATE_HEAP_PTR LocHeap;
  UMA_INFO *UmaInfoPtr;

  MEM_DATA_STRUCT *MemPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  MemPtr = NBPtr->MemPtr;
  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  UmaBase = (UINT32) RefPtr->UmaBase >> (27 - 16);

  //TOM scaled from [47:0] to [47:27]
  LibAmdMsrRead (TOP_MEM, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo += (16 << 20);  // Add 16MB to gain back C6 region if C6 is enabled
  TOM = (SMsr.lo >> 27) | (SMsr.hi << (32 - 27));

  //TOM2 scaled from [47:0] to [47:27]
  LibAmdMsrRead (TOP_MEM2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  TOM2 = (SMsr.lo >> 27) | (SMsr.hi << (32 - 27));

  TOMused = (UmaBase >= _4GB_RJ27) ? TOM2 : TOM;

  if (UmaBase != 0) {
    //Check if channel interleaving is enabled ? if so, go to next step.
    if (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) == 1) {
      DctSelBase = NBPtr->GetBitField (NBPtr, BFDctSelBaseAddr);
      //Skip if DctSelBase is equal to 0, because DCT0 has as the same memory size as DCT1.
      if (DctSelBase != 0) {
        //We need not enable swapped interleaved region when channel interleaving region has covered all of the UMA.
        if (DctSelBase < TOMused) {
          NBPtr->EnableSwapIntlvRgn (NBPtr, UmaBase, TOMused);

          // Set UMA attribute to interleaved after interleaved region has been swapped
          LocHeap.BufferHandle = AMD_UMA_INFO_HANDLE;
          if (HeapLocateBuffer (&LocHeap, &(NBPtr->MemPtr->StdHeader)) == AGESA_SUCCESS) {
            UmaInfoPtr = (UMA_INFO *) LocHeap.BufferPtr;
            UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE | UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
          } else {
            ASSERT (FALSE);
          }
        }
      }
    }
  }
}


