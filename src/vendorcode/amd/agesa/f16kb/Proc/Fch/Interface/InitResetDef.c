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
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*;********************************************************************************
;
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
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

