/* $NoKeywords:$ */
/**
 * @file
 *
 * merrhdl.c
 *
 * Memory error handling
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
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "heapManager.h"
#include "merrhdl.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MERRHDL_FILECODE

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
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
 *  This function handle errors occur in memory code.
 *
 *
 *     @param[in,out]   *MCTPtr - pointer to DIE_STRUCT.
 *     @param[in,out]   DCT - DCT that needs to be handled.
 *     @param[in,out]   ChipSelMask - Chip select mask that needs to be handled
 *     @param[in,out]   *StdHeader - pointer to AMD_CONFIG_PARAMS
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemErrHandle (
  IN       DIE_STRUCT *MCTPtr,
  IN       UINT8  DCT,
  IN       UINT16  ChipSelMask,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN ErrorRecovery;
  BOOLEAN IgnoreErr;
  DCT_STRUCT *DCTPtr;
  UINT8 CurrentDCT;
  LOCATE_HEAP_PTR LocHeap;
  MEM_NB_BLOCK *NBPtr;
  MEM_MAIN_DATA_BLOCK mmData;

  DCTPtr = MCTPtr->DctData;
  ErrorRecovery = TRUE;
  IgnoreErr = FALSE;
  IDS_OPTION_HOOK (IDS_MEM_ERROR_RECOVERY, &ErrorRecovery, StdHeader);

  if (ErrorRecovery) {
    if (DCT == EXCLUDE_ALL_DCT) {
      // Exclude all DCTs on a node
      for (CurrentDCT = 0; CurrentDCT < MCTPtr->DctCount; CurrentDCT++) {
        DCTPtr[CurrentDCT].Timings.CsTestFail = DCTPtr[CurrentDCT].Timings.CsPresent;
      }
    } else if (ChipSelMask == EXCLUDE_ALL_CHIPSEL) {
      // Exclude the specified DCT
      DCTPtr[DCT].Timings.CsTestFail = DCTPtr[DCT].Timings.CsPresent;
    } else {
      // Exclude the chip select that has been marked out
      DCTPtr[DCT].Timings.CsTestFail |= ChipSelMask & DCTPtr[DCT].Timings.CsPresent;
      IDS_OPTION_HOOK (IDS_LOADCARD_ERROR_RECOVERY, &DCTPtr[DCT], StdHeader);
    }

    // Exclude the failed dimm to recovery from error
    if (MCTPtr->NodeMemSize != 0) {
      LocHeap.BufferHandle = AMD_MEM_AUTO_HANDLE;
      if (HeapLocateBuffer (&LocHeap, StdHeader) == AGESA_SUCCESS) {
        // NB block has already been constructed by main block.
        // No need to construct it here.
        NBPtr = (MEM_NB_BLOCK *)LocHeap.BufferPtr;
        if (!NBPtr->SharedPtr->NodeMap[MCTPtr->NodeId].IsValid) {
          // Memory map has not been calculated, no need to remap memory across node here.
          // Only need to remap memory within the node.
          NBPtr = &NBPtr[MCTPtr->NodeId];
          NBPtr->FeatPtr->ExcludeDIMM (NBPtr);
        } else {
          // Need to remap memory across the whole system.
          mmData.MemPtr = NBPtr->MemPtr;
          mmData.mmSharedPtr = NBPtr->SharedPtr;
          mmData.NBPtr = NBPtr;
          mmData.TechPtr = (MEM_TECH_BLOCK *) (&NBPtr[NBPtr->MemPtr->DieCount]);
          mmData.DieCount = NBPtr->MemPtr->DieCount;
          if (!MemFeatMain.ExcludeDIMM (&mmData)) {
            return FALSE;
          }
        }
      }
      // If allocation fails, that means the code is not running at BSP.
      // Parallel training is in process.
      // Remap for parallel training will be done when control returns to BSP.
    }
    return TRUE;
  } else {
    IDS_OPTION_HOOK (IDS_MEM_IGNORE_ERROR, &IgnoreErr, StdHeader);
    if (IgnoreErr) {
      return TRUE;
    }
    SetMemError (AGESA_FATAL, MCTPtr);
    // ErrorRecovery is FALSE
    return FALSE;
  }
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

