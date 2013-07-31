/* $NoKeywords:$ */
/**
 * @file
 *
 * GFX mid post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 88282 $   @e \$Date: 2013-02-19 11:20:56 -0600 (Tue, 19 Feb 2013) $
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
#include  "Ids.h"
#include  "amdlib.h"
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV5.h"
#include  "GnbGfxFamServices.h"
#include  "GfxGmcInitKB.h"
#include  "GfxLibKB.h"
#include  "GfxLibV3.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "PcieConfigData.h"
#include  "PcieConfigLib.h"
#include  "cpuFamilyTranslation.h"
#include  "GfxSamuInitKB.h"
#include  "GnbHandleLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXMIDINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GfxMidInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------*/
/**
 * Init GFX at Mid Post.
 *
 *
 *
 * @param[in] StdHeader       Standard configuration header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
GfxMidInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GFX_PLATFORM_CONFIG   *Gfx;
  UINT8                 AudioEPCount;
  UINT32                GMMx5F50;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxMidInterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Status =  GfxLocateConfigData (StdHeader, &Gfx);
  ASSERT (Status == AGESA_SUCCESS);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_FATAL) {
    GfxFmDisableController (StdHeader);
  } else {
    if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
      Status = GfxEnableGmmAccessV3 (Gfx);
      ASSERT (Status == AGESA_SUCCESS);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
      if (Status != AGESA_SUCCESS) {
        // Can not initialize GMM registers going to disable GFX controller
        IDS_HDT_CONSOLE (GNB_TRACE, "  Fail to establish GMM access\n");
        Gfx->UmaInfo.UmaMode = UMA_NONE;
        GfxFmDisableController (StdHeader);
      } else {
        Status = GfxGmcInitKB (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        Status = GfxSamuInit (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        Status = GfxInitSsid (Gfx);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        AudioEPCount = 0;
        Status = GfxIntAudioEPEnumV3 (Gfx, &AudioEPCount);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);

        if (AudioEPCount > 2) {
          AudioEPCount = 2;
        }

        AudioEPCount = 7 - AudioEPCount;
        GnbRegisterReadKB (GnbGetHandle (StdHeader), 0x12, 0x5F50, &GMMx5F50, 0, StdHeader);
        GMMx5F50 &= ~7; GMMx5F50 |= AudioEPCount & 7;
        GMMx5F50 |= 1 << 4;
        GnbRegisterWriteKB (GnbGetHandle (StdHeader), 0x12, 0x5F50, &GMMx5F50, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxMidInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}

