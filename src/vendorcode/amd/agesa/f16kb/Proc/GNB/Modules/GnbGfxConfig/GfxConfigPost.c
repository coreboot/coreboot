/* $NoKeywords:$ */
/**
 * @file
 *
 * Initialize GFX configuration data structure.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 86709 $   @e \$Date: 2013-01-24 17:39:09 -0600 (Thu, 24 Jan 2013) $
 *
 */
/*
*****************************************************************************
*
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "amdlib.h"
#include  "Ids.h"
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbCommonLib.h"
#include  "GfxConfigLib.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXCONFIG_GFXCONFIGPOST_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG      UserOptions;
extern GNB_BUILD_OPTIONS  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GfxConfigPostInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Allocate UMA configuration data
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @retval        AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GfxConfigPostInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  GFX_PLATFORM_CONFIG       *Gfx;
  AMD_POST_PARAMS           *PostParamsPtr;
  AGESA_STATUS              Status;
  PostParamsPtr = (AMD_POST_PARAMS *)StdHeader;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxConfigPostInterface Enter\n");
  Gfx = GnbAllocateHeapBuffer (AMD_GFX_PLATFORM_CONFIG_HANDLE, sizeof (GFX_PLATFORM_CONFIG), StdHeader);
  ASSERT (Gfx != NULL);
  if (Gfx != NULL) {
    LibAmdMemFill (Gfx, 0x00, sizeof (GFX_PLATFORM_CONFIG), StdHeader);
    if (GnbBuildOptions.IgfxModeAsPcieEp) {
      Gfx->GfxControllerMode = GfxControllerPcieEndpointMode;
      Gfx->GfxPciAddress.AddressValue = MAKE_SBDFO (0, 0, 1, 0, 0);
    } else {
      Gfx->GfxControllerMode = GfxControllerLegacyBridgeMode;
      Gfx->GfxPciAddress.AddressValue = MAKE_SBDFO (0, 1, 5, 0, 0);
    }
    Gfx->StdHeader = (PVOID) StdHeader;
    Gfx->GnbHdAudio = PostParamsPtr->PlatformConfig.GnbHdAudio;
    Gfx->AbmSupport = PostParamsPtr->PlatformConfig.AbmSupport;
    Gfx->DynamicRefreshRate = PostParamsPtr->PlatformConfig.DynamicRefreshRate;
    Gfx->LcdBackLightControl = PostParamsPtr->PlatformConfig.LcdBackLightControl;
    Gfx->AmdPlatformType = UserOptions.CfgAmdPlatformType;
    Gfx->GmcPowerGating = GnbBuildOptions.GmcPowerGating;
    Gfx->UmaSteering = GnbBuildOptions.CfgUmaSteering;
    GNB_DEBUG_CODE (
      GfxConfigDebugDump (Gfx);
      );
  } else {
    Status = AGESA_ERROR;
  }
  IDS_OPTION_HOOK (IDS_GNB_PLATFORMCFG_OVERRIDE, Gfx, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxConfigPostInterface Exit [0x%x]\n", Status);
  return  Status;
}


