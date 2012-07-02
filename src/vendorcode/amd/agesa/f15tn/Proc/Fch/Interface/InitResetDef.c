/* $NoKeywords:$ */
/**
 * @file
 *
 * Fch Init during POWER-ON
 *
 * Prepare Fch environment during power on stage.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
;*********************************************************************************/

#include    "FchPlatform.h"
#include    "Ids.h"
#include    "heapManager.h"
#include    "Filecode.h"
#define FILECODE PROC_FCH_INTERFACE_INITRESETDEF_FILECODE

extern FCH_RESET_DATA_BLOCK  InitResetCfgDefault;

FCH_RESET_DATA_BLOCK*
FchInitResetLoadPrivateDefault (
  IN       AMD_RESET_PARAMS      *ResetParams
  );

FCH_RESET_DATA_BLOCK*
FchInitResetLoadPrivateDefault (
  IN       AMD_RESET_PARAMS      *ResetParams
  )
{
  FCH_RESET_DATA_BLOCK      *FchParams;
  ALLOCATE_HEAP_PARAMS      AllocHeapParams;
  AGESA_STATUS              AgesaStatus;

  // First allocate internal data block via heap manager
  AllocHeapParams.RequestedBufferSize = sizeof (FCH_RESET_DATA_BLOCK);
  AllocHeapParams.Persist = HEAP_TEMP_MEM + 1;
  AllocHeapParams.BufferHandle = AMD_FCH_RESET_DATA_BLOCK_HANDLE;
  AgesaStatus = HeapAllocateBuffer (&AllocHeapParams, &ResetParams->StdHeader);
  ASSERT (!AgesaStatus);

  FchParams = (FCH_RESET_DATA_BLOCK *) AllocHeapParams.BufferPtr;
  ASSERT (FchParams != NULL);
  IDS_HDT_CONSOLE (FCH_TRACE, "    FCH Reset Data Block Allocation: [0x%x], Ptr = 0x%08x\n", AgesaStatus, FchParams);

  *FchParams = InitResetCfgDefault;

  FchParams->Gpp.GppLinkConfig           = UserOptions.FchBldCfg->CfgFchGppLinkConfig;
  FchParams->Gpp.PortCfg[0].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort0Present;
  FchParams->Gpp.PortCfg[1].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort1Present;
  FchParams->Gpp.PortCfg[2].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort2Present;
  FchParams->Gpp.PortCfg[3].PortPresent  = UserOptions.FchBldCfg->CfgFchGppPort3Present;
  FchParams->Gpp.PortCfg[0].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort0HotPlug;
  FchParams->Gpp.PortCfg[1].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort1HotPlug;
  FchParams->Gpp.PortCfg[2].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort2HotPlug;
  FchParams->Gpp.PortCfg[3].PortHotPlug  = UserOptions.FchBldCfg->CfgFchGppPort3HotPlug;

  return FchParams;
}

