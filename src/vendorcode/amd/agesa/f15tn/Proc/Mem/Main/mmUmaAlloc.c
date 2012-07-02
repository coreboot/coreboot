/* $NoKeywords:$ */
/**
 * @file
 *
 * mmUmaAlloc.c
 *
 * Main Memory Feature implementation file for UMA allocation.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "mport.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMUMAALLOC_FILECODE
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
MemMUmaAlloc (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *    UMA allocation mechanism.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
BOOLEAN
MemMUmaAlloc (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT32 TOM;
  UINT32 TOM2;
  UINT32 UmaSize;
  UINT32 TopOfChIntlv;
  UINT32 DctSelHi;
  UINT32 UmaAlignment;
  UINT32 UmaAbove4GBase;
  UINT32 UmaBelow4GBase;
  BOOLEAN DctSelIntLvEn;
  BOOLEAN UmaAbove4GEn;
  S_UINT64 SMsr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  UMA_INFO *UmaInfoPtr;

  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  MemPtr = MemMainPtr->MemPtr;
  NBPtr = &(MemMainPtr->NBPtr[BSP_DIE]);
  RefPtr = NBPtr->RefPtr;

  TOM2 = 0;
  SMsr.lo = SMsr.hi = 0;
  UmaAbove4GBase = 0;
  RefPtr->UmaBase = 0;
  UmaAlignment = (UINT32) UserOptions.CfgUmaAlignment;
  UmaAbove4GEn = UserOptions.CfgUmaAbove4G;
  DctSelIntLvEn = (NBPtr->GetBitField (NBPtr, BFDctSelIntLvEn) != 0) ? TRUE : FALSE;
  TopOfChIntlv = NBPtr->GetBitField (NBPtr, BFDctSelBaseAddr) << (27 - 16);
  DctSelHi = NBPtr->GetBitField (NBPtr, BFDctSelHi);

  // Allocate heap for UMA_INFO
  AllocHeapParams.RequestedBufferSize = sizeof (UMA_INFO);
  AllocHeapParams.BufferHandle = AMD_UMA_INFO_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    ASSERT(FALSE); // Could not allocate heap for Uma information.
    return FALSE;
  }
  UmaInfoPtr = (UMA_INFO *) AllocHeapParams.BufferPtr;
  // Default all the fields of UMA_INFO
  UmaInfoPtr->UmaMode = (UINT8) UMA_NONE;
  UmaInfoPtr->UmaSize = 0;
  UmaInfoPtr->UmaBase = 0;
  UmaInfoPtr->UmaAttributes = 0;
  UmaInfoPtr->MemClock = NBPtr->DCTPtr->Timings.TargetSpeed;

  switch (RefPtr->UmaMode) {
  case UMA_NONE:
    UmaSize = 0;
    break;
  case UMA_SPECIFIED:
    UmaSize = RefPtr->UmaSize;
    break;
  case UMA_AUTO:
    UmaSize = NBPtr->GetUmaSize (NBPtr);
    break;
  default:
    UmaSize = 0;
    IDS_ERROR_TRAP;
  }

  if (UmaSize != 0) {
    //TOM scaled from [47:0] to [47:16]
    LibAmdMsrRead (TOP_MEM, (UINT64 *)&SMsr, &(NBPtr->MemPtr->StdHeader));
    TOM = (SMsr.lo >> 16) | (SMsr.hi << (32 - 16));

    UmaBelow4GBase = (TOM - UmaSize) & UmaAlignment;
    // Initialize Ref->UmaBase to UmaBelow4GBase
    RefPtr->UmaBase = UmaBelow4GBase;

    // Uma Above 4G support
    if (UmaAbove4GEn) {
      //TOM2 scaled from [47:0] to [47:16]
      LibAmdMsrRead (TOP_MEM2, (UINT64 *)&SMsr, &(NBPtr->MemPtr->StdHeader));
      TOM2 = (SMsr.lo >> 16) | (SMsr.hi << (32 - 16));
      if (TOM2 != 0) {
        UmaAbove4GBase = (TOM2 - UmaSize) & UmaAlignment;
        //Set UmaAbove4GBase to 0 if UmaAbove4GBase is below 4GB
        if (UmaAbove4GBase < _4GB_RJ16) {
          UmaAbove4GBase = 0;
        }
        if (UmaAbove4GBase != 0) {
          RefPtr->UmaBase = UmaAbove4GBase;
          // 1. TopOfChIntlv == 0 indicates that whole DCT0 and DCT1 memory are interleaved.
          // 2. TopOfChIntlv >= TOM tells us :
          //   -All or portion of Uma region that above 4G is NOT interleaved.
          //   -Whole Uma region that below 4G is interleaved.
          if (DctSelIntLvEn && (TopOfChIntlv >= TOM)) {
            RefPtr->UmaBase = UmaBelow4GBase;
          }
        }
      }
    }

    UmaInfoPtr->UmaMode = (UINT8) (RefPtr->UmaMode);
    UmaInfoPtr->UmaBase = (UINT64) ((UINT64) RefPtr->UmaBase << 16);

    if (RefPtr->UmaBase >= _4GB_RJ16) {
      // UmaSize might be extended if it is 128MB or 256MB .. aligned, so update it.
      RefPtr->UmaSize = TOM2 - UmaAbove4GBase;
      // Uma Typing
      MemNSetMTRRUmaRegionUCNb (NBPtr, &UmaAbove4GBase, &TOM2);
      if (DctSelIntLvEn && (TopOfChIntlv == 0)) {
        UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE | UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
      } else {
        // Entire UMA region is in the high DCT
        UmaInfoPtr->UmaAttributes = (DctSelHi == 0) ? UMA_ATTRIBUTE_ON_DCT0 : UMA_ATTRIBUTE_ON_DCT1;
      }
    } else {
      // UmaSize might be extended if it is 128MB or 256MB .. aligned, so update it.
      RefPtr->UmaSize = TOM - UmaBelow4GBase;
      // Uma Typing
      NBPtr->UMAMemTyping (NBPtr);
      if (DctSelIntLvEn && ((TopOfChIntlv == 0) || (TopOfChIntlv >= TOM))) {
        UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_INTERLEAVE | UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
      } else {
        if (UmaBelow4GBase >= TopOfChIntlv) {
          // Entire UMA region is in the high DCT
          UmaInfoPtr->UmaAttributes = (DctSelHi == 0) ? UMA_ATTRIBUTE_ON_DCT0 : UMA_ATTRIBUTE_ON_DCT1;
        } else if (TopOfChIntlv >= TOM) {
          // Entire UMA region is in the low DCT
          UmaInfoPtr->UmaAttributes = (DctSelHi == 1) ? UMA_ATTRIBUTE_ON_DCT0 : UMA_ATTRIBUTE_ON_DCT1;
        } else {
          // UMA region is in both DCT0 and DCT1
          UmaInfoPtr->UmaAttributes = UMA_ATTRIBUTE_ON_DCT0 | UMA_ATTRIBUTE_ON_DCT1;
        }
      }
    }
    UmaInfoPtr->UmaSize = (RefPtr->UmaSize) << 16;
    IDS_HDT_CONSOLE (MEM_FLOW, "UMA is allocated:\n\tBase: %x0000\n\tSize: %x0000\n", RefPtr->UmaBase, RefPtr->UmaSize);
  }

  return TRUE;
}

