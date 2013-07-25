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
 * @e \$Revision: 87849 $   @e \$Date: 2013-02-11 15:37:58 -0600 (Mon, 11 Feb 2013) $
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
#include  "Gnb.h"
#include  "GnbGfx.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbRegisterAccKB.h"
#include  "cpuFamilyTranslation.h"
#include  "GnbRegistersKB.h"
#include  "GfxLibKB.h"
#include  "GfxGmcInitKB.h"
#include  "Filecode.h"

#define FILECODE PROC_GNB_MODULES_GNBINITKB_GFXGMCINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_TABLE ROMDATA GfxGmcFeature1DisableKB [];
extern GNB_TABLE ROMDATA GfxGmcInitTableKB [];
extern GNB_TABLE ROMDATA GfxGmcFeature1EnableKB [];


#define GNB_GFX_DRAM_CH_0_PRESENT 1
#define GNB_GFX_DRAM_CH_1_PRESENT 2

#define DRAMTYPE_DDR3             1

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize Fb location
 *
 *
 *
 * @param[in]   GnbHandle         Pointer to GNB_HANDLE
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcInitializeFbLocationKB (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32            GMMx2024;
  UINT32            GMMx2068;
  UINT32            GMMx2C04;
  UINT32            GMMx5428;
  UINT64            FBBase;
  UINT64            FBTop;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInitializeFbLocationKB Enter\n");

  FBBase = 0x0F00000000;
  FBTop = FBBase + Gfx->UmaInfo.UmaSize - 1;
  GMMx2024 = 0;
  GMMx2C04 = 0;
  GMMx2024 |= (UINT16) (FBBase >> 24);
  GMMx2024 |= (UINT32) ((FBTop >> 24) << 16);
  GMMx2068 = (UINT32) (Gfx->UmaInfo.UmaBase >> 22);
  GMMx2C04 = (UINT32) (FBBase >> 8);
  GMMx5428 = Gfx->UmaInfo.UmaSize >> 20;
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2024, &GMMx2024, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2068, &GMMx2068, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2C04, &GMMx2C04, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x5428, &GMMx5428, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInitializeFbLocationKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize sequencer model
 *
 *
 *
 * @param[in]   GnbHandle         Pointer to GNB_HANDLE
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcInitializeHubAndCitfSteeringKB (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32           GMMx2004;
  UINT32           GMMx2008;

  GMMx2004 = 0x2210;
  GMMx2008 = 0;

  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2004, &GMMx2004, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2008, &GMMx2008, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable Stutter Mode with/without power-gating
 *
 *
 *
 * @param[in]   GnbHandle         Pointer to GNB_HANDLE
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcEnableStutterModePowerGatingKB (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32            GMMx3508;
  UINT32            GMMx350C;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcEnableStutterModePowerGatingKB Enter\n");

  GnbRegisterReadKB (GnbHandle, 0x12, 0x3508, &GMMx3508, 0, GnbLibGetHeader (Gfx));
  GnbRegisterReadKB (GnbHandle, 0x12, 0x350C, &GMMx350C, 0, GnbLibGetHeader (Gfx));
  if (Gfx->GmcPowerGating != GmcPowerGatingDisabled) {
    // Enabling power gating
    if (Gfx->GmcPowerGating == GmcPowerGatingWithStutter) {
      GMMx3508 |= 1;
      GMMx350C |= 1 << 11;
      GMMx350C &= ~(1 << 16);
    } else {
      GMMx3508 &= ~1;
      GMMx350C &= ~(1 << 11);
      GMMx350C |= 1 << 16;
    }
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x3508, &GMMx3508, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x350C, &GMMx350C, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  } else {
    // Disabling power gating
    GMMx3508 &= ~1;
    GMMx350C &= ~(1 << 11);
    GMMx350C &= ~(1 << 16);
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x3508, &GMMx3508, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x350C, &GMMx350C, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcEnableStutterModePowerGatingKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 *
 *
 *
 *
 * @param[in]   GnbHandle         Pointer to GNB_HANDLE
 * @param[in] Gfx         Pointer to global GFX configuration
 */

STATIC VOID
GfxGmcSecureGarlicAccessKB (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32   v1;
  UINT32   v2;
  UINT32   v3;

  v1 = (UINT32) (Gfx->UmaInfo.UmaBase >> 20);
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2868, &v1, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  v2 = (UINT32) (((Gfx->UmaInfo.UmaBase + Gfx->UmaInfo.UmaSize) >> 20) - 1);
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x286C, &v2, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  // Area FB - 32K reserved by VBIOS for SBIOS to use
  v3 = (UINT32) ((Gfx->UmaInfo.UmaBase + Gfx->UmaInfo.UmaSize - 32 * 1024) >> 12);
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x2878, &v3, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize C6 aperture location
 *
 *
 *
 * @param[in]   GnbHandle         Pointer to GNB_HANDLE
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */
STATIC VOID
GfxGmcInitializeC6LocationKB (
  IN      GNB_HANDLE            *GnbHandle,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D18F2x118_STRUCT    D18F2x118;
  D18F1x44_STRUCT     D18F1x44;
  UINT32     GMMx2870;
  UINT32     GMMx2874;

  // Check C6 enable,  D18F2x118[CC6SaveEn]
  GnbRegisterReadKB (GnbHandle, D18F2x118_TYPE, D18F2x118_ADDRESS, &D18F2x118.Value, 0, GnbLibGetHeader (Gfx));

  if (D18F2x118.Field.CC6SaveEn) {
    // From D18F1x[144:140,44:40] DRAM Base/Limit,
    // {DramBase[47:24], 00_0000h} <= address[47:0] <= {DramLimit[47:24], FF_FFFFh}.
    GnbRegisterReadKB (GnbHandle, D18F1x44_TYPE, D18F1x44_ADDRESS, &D18F1x44.Value, 0, GnbLibGetHeader (Gfx));
    //
    // base 39:20, base = Dram Limit + 1
    // ex: system 256 MB on Node 0, D18F1x44.Field.DramLimit_39_24_ = 0xE (240MB -1)
    // Node DRAM     D18F1x[144:140,44:40]    CC6DRAMRange         D18F4x128   D18F1x120 D18F1x124
    //    0 256MB    0MB ~ 240 MB - 1        240 MB ~ 256 MB - 1     0           0 MB,    256 MB - 1
    //

    // base 39:20
    GMMx2870 = ((D18F1x44.Field.DramLimit_39_24 + 1) << 4);
    // top 39:20
    GMMx2874 = (((D18F1x44.Field.DramLimit_39_24 + 1) << 24) + (16 * 0x100000) - 1) >> 20;

    GnbRegisterWriteKB (GnbHandle, 0x12, 0x2870, &GMMx2870, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbRegisterWriteKB (GnbHandle, 0x12, 0x2874, &GMMx2874, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize GMC
 *
 *
 *
 * @param[in]   Gfx               Pointer to global GFX configuration
 *
 */

AGESA_STATUS
GfxGmcInitKB (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  UINT32            v1;
  UINT32            GMMx25E8;
  D18F3x44_STRUCT   D18F3x44;
  GNB_HANDLE        *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInitKB Enter\n");
//2.1 Disable clock-gating
  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  ASSERT (GnbHandle != NULL);
  GnbProcessTable (
    GnbHandle,
    GfxGmcFeature1DisableKB,
    0,
    GNB_TABLE_FLAGS_FORCE_S3_SAVE,
    GnbLibGetHeader (Gfx)
    );
//2.3 HUB & CITF channel steering
  GfxGmcInitializeHubAndCitfSteeringKB (GnbHandle, Gfx);
//2.6 Frame buffer location
  GfxGmcInitializeFbLocationKB (GnbHandle, Gfx);
//2.8 Securing GARLIC access
//2.8.1 GARLIC request disable
  GfxGmcSecureGarlicAccessKB (GnbHandle, Gfx);
//2.8.2 C6 save aperture
  GfxGmcInitializeC6LocationKB (GnbHandle, Gfx);
//2.2 System memory address translation
//2.4 Sequencer model programming
//2.5 Power Gating - PGFSM and RENG init
//2.7 Performance tuning
//2.10 Display latency
//2.11 Remove blackout
  GnbProcessTable (
    GnbHandle,
    GfxGmcInitTableKB,
    0,
    GNB_TABLE_FLAGS_FORCE_S3_SAVE,
    GnbLibGetHeader (Gfx)
    );
//4. Power management
    GnbProcessTable (
      GnbHandle,
      GfxGmcFeature1EnableKB,
      0,
      GNB_TABLE_FLAGS_FORCE_S3_SAVE,
      GnbLibGetHeader (Gfx)
      );

//   SETUP or BLDCFG may override.
  switch (Gfx->UmaSteering) {
  case 0:
    v1 = 0;
    GnbRegisterReadKB (GnbHandle, D18F3x44_TYPE, D18F3x44_ADDRESS, &D18F3x44.Value, 0, GnbLibGetHeader (Gfx));
    if (D18F3x44.Field.DramEccEn == 1) {
      v1 = SystemTrafficOnion;
    }
    break;
  case SystemTrafficOnion:
    v1 = SystemTrafficOnion;
    break;
  case Onion:
    v1 = Onion;
    break;
  case 3:
    v1 = 3;
    break;
  default:
    v1 = 0;
    ASSERT (FALSE);
    break;
  }
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x206C, &v1, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_GMM_REGISTER_OVERRIDE, Gfx, GnbLibGetHeader (Gfx));

  GnbRegisterReadKB (GnbHandle, 0x12, 0x25E8, &GMMx25E8, 0, GnbLibGetHeader (Gfx));
  GMMx25E8 |= BIT14;
  GnbRegisterWriteKB (GnbHandle, 0x12, 0x25E8, &GMMx25E8, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  if (Gfx->GmcPowerGating != GmcPowerGatingDisabled) {
    //4.2 Enabling stutter mode with or without power-gating
    GfxGmcEnableStutterModePowerGatingKB (GnbHandle, Gfx);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGmcInitKB Exit\n");
  return AGESA_SUCCESS;
}
