/* $NoKeywords:$ */
/**
 * @file
 *
 * mmOnlineSpare.c
 *
 * Main Memory Feature implementation file for Node Interleaving
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_MAIN_MMONLINESPARE_FILECODE
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
MemMOnlineSpare (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Check and enable online spare on all nodes.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMOnlineSpare (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8   Node;
  BOOLEAN IsEnabled;
  UINT8   FirstEnabledNode;
  UINT32  BottomIO;
  BOOLEAN RetVal;
  MEM_NB_BLOCK  *NBPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  AGESA_TESTPOINT (TpProcMemOnlineSpareInit, &(MemMainPtr->MemPtr->StdHeader));
  FirstEnabledNode = 0;
  IsEnabled = FALSE;
  RetVal = TRUE;
  NBPtr = MemMainPtr->NBPtr;
  RefPtr = NBPtr[BSP_DIE].RefPtr;

  for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
    if (NBPtr[Node].FeatPtr->OnlineSpare (&NBPtr[Node])) {
      if (!IsEnabled) {
        // Record the first node that has spared dimm enabled
        FirstEnabledNode = Node;
        IsEnabled = TRUE;
      }
    }
  }

  if (IsEnabled) {
    NBPtr[BSP_DIE].SharedPtr->CurrentNodeSysBase = 0;
    BottomIO = (NBPtr[BSP_DIE].RefPtr->BottomIo & 0xF8) << 8;
    // If the first node that has spared dimms does not have a system base smaller
    // than bottomIO, then we don't need to reset the GStatus, as we don't need to
    // remap memory hole.
    if (NBPtr[FirstEnabledNode].MCTPtr->NodeSysBase < BottomIO) {
      RefPtr->GStatus[GsbHWHole] = FALSE;
      RefPtr->GStatus[GsbSpIntRemapHole] = FALSE;
      RefPtr->GStatus[GsbSoftHole] = FALSE;
      RefPtr->HoleBase = 0;
    }

    for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
      if (Node >= FirstEnabledNode) {
        // Remap memory on nodes with node number larger than the first node that has spared dimms.
        NBPtr[Node].MCTPtr->Status[SbHWHole] = FALSE;
        NBPtr[Node].MCTPtr->Status[SbSWNodeHole] = FALSE;
        NBPtr[Node].SetBitField (&NBPtr[Node], BFDctSelBaseAddr, 0);
        NBPtr[Node].SetBitField (&NBPtr[Node], BFDctSelHiRngEn, 0);
        NBPtr[Node].SetBitField (&NBPtr[Node], BFDctSelHi, 0);
        NBPtr[Node].SetBitField (&NBPtr[Node], BFDctSelBaseOffset, 0);
        NBPtr[Node].SetBitField (&NBPtr[Node], BFDramHoleAddrReg, 0);
        NBPtr[Node].HtMemMapInit (&NBPtr[Node]);
      } else {
        // No change is needed in the memory map of this node.
        // Need to adjust the current system base for other nodes processed later.
        NBPtr[Node].SharedPtr->CurrentNodeSysBase = (NBPtr[Node].MCTPtr->NodeSysLimit + 1) & 0xFFFFFFF0;
        // If the current node does not have the memory hole, then set DramHoleAddrReg to be 0.
        // If memory hoisting is enabled later by other node, SyncAddrMapToAllNodes will set the base
        // and DramMemHoistValid.
        // Otherwise, do not change the register value, as we need to keep DramHoleOffset unchanged, as well
        // DramHoleValid.
        if (!NBPtr[Node].MCTPtr->Status[SbHWHole]) {
          NBPtr[Node].SetBitField (&NBPtr[Node], BFDramHoleAddrReg, 0);
        }
      }
    }

    for (Node = 0; Node < MemMainPtr->DieCount; Node++) {
      NBPtr[Node].SyncAddrMapToAllNodes (&NBPtr[Node]);
      RetVal &= (BOOLEAN) (NBPtr[Node].MCTPtr->ErrCode < AGESA_FATAL);
    }
    NBPtr[BSP_DIE].CpuMemTyping (&NBPtr[BSP_DIE]);
  }
  return RetVal;
}
