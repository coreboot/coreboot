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
 * @e \$Revision: 39255 $   @e \$Date: 2010-10-08 11:27:41 -0700 (Fri, 08 Oct 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * 
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
#include  "GnbGfxConfig.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  "GfxConfigData.h"
#include  "GfxConfigPost.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBGFXCONFIG_GFXCONFIGENV_FILECODE
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
GfxConfigEnvInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );



/*----------------------------------------------------------------------------------------*/
/**
 * Get UMA info
 *
 * UMA info stored on heap by memory module
 *
 * @param[out] UmaInfo      Pointer to UMA info structure
 * @param[in]  StdHeader    Standard configuration header
 */

VOID
GfxGetUmaInfo (
     OUT   UMA_INFO          *UmaInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UMA_INFO *MemUmaInfo;

  MemUmaInfo = GnbLocateHeapBuffer (AMD_UMA_INFO_HANDLE, StdHeader);
  if (MemUmaInfo == NULL) {
    LibAmdMemFill (UmaInfo, 0x00, sizeof (UMA_INFO), StdHeader);
    UmaInfo->UmaMode = UMA_NONE;
  } else {
    LibAmdMemCopy (UmaInfo, MemUmaInfo, sizeof (UMA_INFO), StdHeader);
    if ((UmaInfo->UmaBase == 0) || (UmaInfo->UmaSize == 0)) {
      UmaInfo->UmaMode = UMA_NONE;
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate UMA configuration data
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @param[in,out] Gfx           Pointer to GFX configuration
 * @retval        AGESA_STATUS  Data located
 * @retval        AGESA_FATA    Data not found
 */

AGESA_STATUS
GfxLocateConfigData (
  IN       AMD_CONFIG_PARAMS     *StdHeader,
     OUT   GFX_PLATFORM_CONFIG   **Gfx
  )
{
  *Gfx = GnbLocateHeapBuffer (AMD_GFX_PLATFORM_CONFIG_HANDLE, StdHeader);
  if (*Gfx == NULL) {
    IDS_ERROR_TRAP;
    return  AGESA_FATAL;
  }
  (*Gfx)->StdHeader = StdHeader;
  return AGESA_SUCCESS;
}



/*----------------------------------------------------------------------------------------*/
/**
 * Update GFX config info at ENV
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @retval        AGESA_STATUS  Always succeeds
 */

AGESA_STATUS
GfxConfigEnvInterface (
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{

  AMD_ENV_PARAMS        *EnvParamsPtr;
  GFX_PLATFORM_CONFIG   *Gfx;
  AGESA_STATUS          Status;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxConfigEnvInterface Enter\n");
  Status = GfxLocateConfigData (StdHeader, &Gfx);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    EnvParamsPtr = (AMD_ENV_PARAMS *)StdHeader;
    Gfx->Gnb3dStereoPinIndex = EnvParamsPtr->GnbEnvConfiguration.Gnb3dStereoPinIndex;
    Gfx->LvdsSpreadSpectrum = EnvParamsPtr->GnbEnvConfiguration.LvdsSpreadSpectrum;
    Gfx->LvdsSpreadSpectrumRate = EnvParamsPtr->GnbEnvConfiguration.LvdsSpreadSpectrumRate;
    Gfx->LvdsMiscControl.Value = EnvParamsPtr->GnbEnvConfiguration.LvdsMiscControl.Value;
    Gfx->PcieRefClkSpreadSpectrum = EnvParamsPtr->GnbEnvConfiguration.PcieRefClkSpreadSpectrum;
    GfxGetUmaInfo (&Gfx->UmaInfo, StdHeader);
  }
  GNB_DEBUG_CODE (
    GfxConfigDebugDump (Gfx);
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxConfigEnvInterface Exit [0x%x]\n", Status);
  return  Status;
}
