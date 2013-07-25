/* $NoKeywords:$ */
/**
 * @file
 *
 * GFX env post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 86586 $   @e \$Date: 2013-01-23 12:45:26 -0600 (Wed, 23 Jan 2013) $
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
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbCommonLib.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbGfxConfig.h"
#include  "GnbGfxFamServices.h"
#include  "GfxLibKB.h"
#include  "GfxLibV3.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbHandleLib.h"
#include  "cpuFamilyTranslation.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXENVINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_TABLE ROMDATA      GfxEnvInitTableKB[];
extern GNB_BUILD_OPTIONS      GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
GfxEnvInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize GFX straps.
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxStrapsEnvInitKB (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D0F0xD4_x010914E1_STRUCT    D0F0xD4_x10914E1;
  D0F0xD4_x010914E2_STRUCT    D0F0xD4_x10914E2;
  D0F0xD4_x01091507_STRUCT    D0F0xD4_x1091507;
  D0F0x64_x1D_STRUCT          D0F0x64_x1D;
  UINT32                      D0F0xD4_x010914C3;
  GNB_HANDLE                 *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxStrapsEnvInitKB Enter\n");

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));

  GnbRegisterReadKB (GnbHandle, D0F0xD4_x010914E1_TYPE, D0F0xD4_x010914E1_ADDRESS, &D0F0xD4_x10914E1.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadKB (GnbHandle, D0F0xD4_x010914E2_TYPE, D0F0xD4_x010914E2_ADDRESS, &D0F0xD4_x10914E2.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadKB (GnbHandle, D0F0xD4_x01091507_TYPE, D0F0xD4_x01091507_ADDRESS, &D0F0xD4_x1091507.Value, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadKB (GnbHandle, D0F0xD4_x010914C3_TYPE, D0F0xD4_x010914C3_ADDRESS, &D0F0xD4_x010914C3, 0, GnbLibGetHeader (Gfx));

  GnbLibPciIndirectRead (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0x64_x1D.Field.VgaEn = 0x1;

  D0F0xD4_x10914E2.Field.bita = 0x0;
  D0F0xD4_x10914E2.Field.bita = 0x0;
  D0F0xD4_x1091507.Field.bit16 = Gfx->GnbHdAudio;
  D0F0xD4_x10914E2.Field.bit13 = Gfx->GnbHdAudio;

  D0F0xD4_x10914E1.Field.StrapBifRegApSize = 0x2;
  D0F0xD4_x10914E1.Field.StrapBifDoorbellBarDis = 0x0;
  D0F0xD4_x10914E1.Field.Bitfield_13_13 = 0x0;
  D0F0xD4_x10914E1.Field.Bitfield_15_14 = 0x3;

  if (Gfx->UmaInfo.UmaSize > 128 * 0x100000) {
    D0F0xD4_x10914E1.Field.StrapBifMemApSize = 0x1;
    D0F0xD4_x1091507.Field.StrapBifMemApSizePin = 0x1;
  } else if (Gfx->UmaInfo.UmaSize > 64 * 0x100000) {
    D0F0xD4_x10914E1.Field.StrapBifMemApSize = 0x0;
    D0F0xD4_x1091507.Field.StrapBifMemApSizePin = 0x0;
  } else {
    D0F0xD4_x10914E1.Field.StrapBifMemApSize = 0x2;
    D0F0xD4_x1091507.Field.StrapBifMemApSizePin = 0x2;
  }

  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0xD4_x010914C3 |= BIT0;
  GnbRegisterWriteKB (GnbHandle, D0F0xD4_x010914E1_TYPE, D0F0xD4_x010914E1_ADDRESS, &D0F0xD4_x10914E1.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, D0F0xD4_x010914E2_TYPE, D0F0xD4_x010914E2_ADDRESS, &D0F0xD4_x10914E2.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, D0F0xD4_x01091507_TYPE, D0F0xD4_x01091507_ADDRESS, &D0F0xD4_x1091507.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, D0F0xD4_x010914C3_TYPE, D0F0xD4_x010914C3_ADDRESS, &D0F0xD4_x010914C3, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxStrapsEnvInitKB Exit\n");
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init GFX at Env Post.
 *
 *
 *
 * @param[in] StdHeader     Standard configuration header
 * @retval    AGESA_STATUS
  */


AGESA_STATUS
GfxEnvInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GFX_PLATFORM_CONFIG   *Gfx;
  GNB_HANDLE            *GnbHandle;
  UINT32                Property;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInterfaceKB Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Property = TABLE_PROPERTY_DEFAULT;

  Status = GfxLocateConfigData (StdHeader, &Gfx);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
     // Power up iGPU
      GfxRequestGPUPowerV3 (Gfx, 1);
      Status = GfxStrapsEnvInitKB (Gfx);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
      ASSERT (Status == AGESA_SUCCESS);
    } else {
      GfxFmDisableController (StdHeader);
      Property |= TABLE_PROPERTY_IGFX_DISABLED;
    }
  } else {
    GfxFmDisableController (StdHeader);
    Property |= TABLE_PROPERTY_IGFX_DISABLED;
  }

  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  Status = GnbProcessTable (
             GnbHandle,
             GfxEnvInitTableKB,
             Property,
             GNB_TABLE_FLAGS_FORCE_S3_SAVE,
             StdHeader
             );
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  Status;
}
