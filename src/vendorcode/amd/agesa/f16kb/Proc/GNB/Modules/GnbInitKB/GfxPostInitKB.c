/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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
#include  "GnbPcie.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbGfx.h"
#include  "GnbGfxConfig.h"
#include  "GnbF1Table.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "GnbHandleLib.h"
#include  "GfxLibV3.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXPOSTINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GfxPostInterfaceKB (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Init GFX at Post.
 *
 *
 *
 * @param[in]     StdHeader     Standard configuration header
 * @retval        AGESA_STATUS
 */


AGESA_STATUS
GfxPostInterfaceKB (
  IN       AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AMD_POST_PARAMS           *PostParamsPtr;
  GFX_CARD_CARD_INFO        GfxDiscreteCardInfo;
  AGESA_STATUS              Status;
  GFX_PLATFORM_CONFIG       *Gfx;
  UINT32                    D0F0xBC_xC00C0000;
  GnbRegistersKB7269_STRUCT  var5;
  GnbRegistersKB7314_STRUCT  var6;
  GNB_HANDLE                *GnbHandle;

  PostParamsPtr = (AMD_POST_PARAMS *)StdHeader;
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxPostInterfaceKB Enter\n");
  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  GnbRegisterReadKB (GnbHandle, 0x4,
      0xc00c0000, &D0F0xBC_xC00C0000, 0, StdHeader);
  Status = GfxLocateConfigData (StdHeader, &Gfx);
  ASSERT (Status == AGESA_SUCCESS);
  if (Status == AGESA_SUCCESS) {
    if (((D0F0xBC_xC00C0000 >> 1) & 1) != 1) {
      if (PostParamsPtr->MemConfig.UmaMode != UMA_NONE) {
        LibAmdMemFill (&GfxDiscreteCardInfo, 0x0, sizeof (GfxDiscreteCardInfo), StdHeader);
        GfxGetDiscreteCardInfo (&GfxDiscreteCardInfo, StdHeader);
        if (((GfxDiscreteCardInfo.PciGfxCardBitmap != 0) ||
          (GfxDiscreteCardInfo.AmdPcieGfxCardBitmap != GfxDiscreteCardInfo.PcieGfxCardBitmap)) ||
          ((PostParamsPtr->GnbPostConfig.IgpuEnableDisablePolicy == IGPU_DISABLE_ANY_PCIE)  &&
          ((GfxDiscreteCardInfo.PciGfxCardBitmap != 0) || (GfxDiscreteCardInfo.PcieGfxCardBitmap != 0)))) {
          PostParamsPtr->MemConfig.UmaMode = UMA_NONE;
          IDS_HDT_CONSOLE (GFX_MISC, "  GfxDisabled due dGPU policy\n");
        }
      }

      if (PostParamsPtr->MemConfig.UmaMode == UMA_NONE) {
        GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x13014AC, &var5.Value, 0, StdHeader);
        GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x13014B6, &var6.Value, 0, StdHeader);
        var5.Field.bit16 = FALSE;
        var6.Field.bit13 = FALSE;
        GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x13014AC, &var5.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
        GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x13014B6, &var6.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
      }

      // Power down iGPU
      GfxRequestGPUPowerV3 (Gfx, 0);
    } else {
      PostParamsPtr->MemConfig.UmaMode = UMA_NONE;
    }
  } else {
    PostParamsPtr->MemConfig.UmaMode = UMA_NONE;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxPostInterfaceKB Exit [0x%x]\n", Status);
  return  Status;
}
