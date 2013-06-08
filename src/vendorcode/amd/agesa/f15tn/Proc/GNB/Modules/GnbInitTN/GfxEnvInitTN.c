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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include  "GfxLibTN.h"
#include  "GnbRegistersTN.h"
#include  "GnbInitTN.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbHandleLib.h"
#include  "GnbTimerLib.h"
#include  "cpuFamilyTranslation.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GFXENVINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_TABLE ROMDATA      GfxEnvInitTableTN[];
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
GfxEnvInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Shut Down Disabled SIMDs
 *
 * @param[in] Property        GNB property
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxShutDownDisabledSimdsTN (
  IN      UINT32                Property,
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  ex1006_STRUCT  ex1006 ;
  ex1009_STRUCT   ex1009;
  D0F0xBC_xE03002F8_STRUCT  D0F0xBC_xE03002F8;
  D0F0xBC_xE03002FC_STRUCT  D0F0xBC_xE03002FC;
  D0F0xBC_xE0300054_STRUCT  GfxChainPgfsmConfig;
  UINT8                     n;
  UINT32                    Mask;
  CPU_LOGICAL_ID            LogicalId;
  GNB_HANDLE                *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxShutDownDisabledSimdsTN Enter\n");

  GnbHandle = GnbGetHandle (GnbLibGetHeader (Gfx));
  ASSERT (GnbHandle != NULL);
  GetLogicalIdOfSocket (GnbGetSocketId (GnbHandle), &LogicalId, GnbLibGetHeader (Gfx));

  GfxChainPgfsmConfig.Value = 0;
  GfxChainPgfsmConfig.Field.PowerDown = 1;
  GfxChainPgfsmConfig.Field.P2Select = 1;
  GfxChainPgfsmConfig.Field.FsmAddr = 0xFF;

  //Step 1: Read fuse to see which SIMD(s) have been disabled
  GnbRegisterReadTN (TYPE_D0F0xBC , 0xe000101c , &ex1006.Value, 0, GnbLibGetHeader (Gfx));

  //Step 2: Check which SIMD has been disabled
  for (n = 0; n < 6; n++) {
    if (((Property & TABLE_PROPERTY_IGFX_DISABLED) != 0) || ((ex1006.Field.ex1006_0  >> n) & 0x1)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "Disable SIMD %d\n", n);
      //Step 3: Make sure PGFSM has been programmed in GFX Power Island.
      //Step 4: Make sure SCLK frequency is below 400Mhz
      //Step 5: Enable PGFSM clock
      GnbRegisterReadTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, 0, GnbLibGetHeader (Gfx));
      ex1009.Value |= (0x1 << (0  + n));
      GnbRegisterWriteTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
      //Step 6
      GnbRegisterWriteTN (TYPE_D0F0xBC, (D0F0xBC_xE0300054_ADDRESS + (n * 0x1C)), &GfxChainPgfsmConfig.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
      //Step 7
      Mask = (UINT32) (0x1F << (n * 5));
      do {
        GnbRegisterReadTN (D0F0xBC_xE03002F8_TYPE, D0F0xBC_xE03002F8_ADDRESS, &D0F0xBC_xE03002F8.Value, 0, GnbLibGetHeader (Gfx));
      } while ((D0F0xBC_xE03002F8.Value & Mask )!= 0);
      //Step 8: Restore previous SCLK divider
      if ((LogicalId.Revision & 0x0000000000000100ull ) != 0x0000000000000100ull ) {
        do {
          GnbRegisterReadTN (D0F0xBC_xE03002FC_TYPE, D0F0xBC_xE03002FC_ADDRESS, &D0F0xBC_xE03002FC.Value, 0, GnbLibGetHeader (Gfx));
        } while ((D0F0xBC_xE03002FC.Value & Mask )!= Mask);
      } else {
      }
      //Step 10: Turn off PGFSM clock
      GnbRegisterReadTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, 0, GnbLibGetHeader (Gfx));
      ex1009.Value &= (~ (UINT64) (0x1 << (0  + n)));
      GnbRegisterWriteTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxShutDownDisabledSimdsTN Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Shut Down Disabled SIMDs
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxShutDownDisabledRbsTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  ex1006_STRUCT  ex1006 ;
  D0F0xBC_xE0003024_STRUCT  D0F0xBC_xE0003024;
  D0F0xBC_xE03000FC_STRUCT  D0F0xBC_xE03000FC;
  D0F0xBC_xE0300100_STRUCT  D0F0xBC_xE0300100;
  ex1009_STRUCT   ex1009 ;
  D0F0xBC_xE03002F4_STRUCT  D0F0xBC_xE03002F4;
  D0F0xBC_xE03002E4_STRUCT  D0F0xBC_xE03002E4;
  UINT8                     i;
  UINT8                     RbNumber;
  UINT32                    Mask1;
  UINT32                    Mask2;

  D0F0xBC_xE03000FC.Value = 0;
  D0F0xBC_xE03000FC.Field.WriteOp = 1;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxShutDownDisabledRbsTN Enter\n");

  //Step 1: Read fuse to see which SIMD(s) have been disabled
  GnbRegisterReadTN (TYPE_D0F0xBC , 0xe000101c , &ex1006.Value, 0, GnbLibGetHeader (Gfx));

  //Step 2: Power down disabled RB
  if (ex1006.Field.ex1006_1  == 0x1) {
    RbNumber = 0;
    Mask1 = 0x3FFFA;
    Mask2 = 0x5;
  } else if (ex1006.Field.ex1006_1  == 0x2) {
    RbNumber = 1;
    Mask1 = 0x3FFF5;
    Mask2 = 0xA;
  } else {
    return AGESA_SUCCESS;
  }
  //Step 3: Enable PGFSM commands during reset
  GnbRegisterReadTN (D0F0xBC_xE0003024_TYPE, D0F0xBC_xE0003024_ADDRESS, &D0F0xBC_xE0003024.Value, 0, GnbLibGetHeader (Gfx));
  D0F0xBC_xE0003024.Value |= 0x1;
  GnbRegisterWriteTN (D0F0xBC_xE0003024_TYPE, D0F0xBC_xE0003024_ADDRESS, &D0F0xBC_xE0003024.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  //Step 4: Make sure PGFSM has been programmed before sending power down command.
  //CB0 = 0, DB0 = 2, CB1 = 1, DB1 = 3
  for (i = RbNumber; i < 4; i += 2) {
    D0F0xBC_xE0300100.Value = (5 << 16 ) | (4 << 8 ) | (10 << 0 ); //reg0
    GnbRegisterWriteTN (D0F0xBC_xE0300100_TYPE, D0F0xBC_xE0300100_ADDRESS, &D0F0xBC_xE0300100.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    D0F0xBC_xE03000FC.Field.FsmAddr = i;
    D0F0xBC_xE03000FC.Field.RegAddr = 2 ;
    GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));

    D0F0xBC_xE0300100.Value = (50 << 0 ) | (50 << 12 ); //reg1
    GnbRegisterWriteTN (D0F0xBC_xE0300100_TYPE, D0F0xBC_xE0300100_ADDRESS, &D0F0xBC_xE0300100.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    D0F0xBC_xE03000FC.Field.RegAddr = 3 ;
    GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));

    D0F0xBC_xE0300100.Value = 0; //control
    GnbRegisterWriteTN (D0F0xBC_xE0300100_TYPE, D0F0xBC_xE0300100_ADDRESS, &D0F0xBC_xE0300100.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    D0F0xBC_xE03000FC.Field.RegAddr = 1 ;
    GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
    GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));
  }
  //Step 5: Make sure SCLK frequency is below 400Mhz
  //Step 6: Enable PGFSM clock
  GnbRegisterReadTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, 0, GnbLibGetHeader (Gfx));
  ex1009.Field.ex1009_1  = 1;
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  //Step 7
  D0F0xBC_xE03000FC.Value = 0;
  D0F0xBC_xE03000FC.Field.PowerDown = 1;
  D0F0xBC_xE03000FC.Field.P1Select = 1;
  D0F0xBC_xE03000FC.Field.P2Select = 1;
  D0F0xBC_xE03000FC.Field.FsmAddr = RbNumber;
  GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));

  //Step 8
  D0F0xBC_xE03000FC.Field.FsmAddr = RbNumber + 1;
  GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));

  //Step 9: Wait for isolation to be asserted for RB0/RB1
  do {
    GnbRegisterReadTN (D0F0xBC_xE03002F4_TYPE, D0F0xBC_xE03002F4_ADDRESS, &D0F0xBC_xE03002F4.Value, 0, GnbLibGetHeader (Gfx));
  } while ((D0F0xBC_xE03002F4.Value & Mask1 )!= 0);
  //Step 10: Restore previous SCLK divider
  //Step 11: Wait for PSO daughter to be asserted for RB0/RB1
  do {
    GnbRegisterReadTN (D0F0xBC_xE03002E4_TYPE, D0F0xBC_xE03002E4_ADDRESS, &D0F0xBC_xE03002E4.Value, 0, GnbLibGetHeader (Gfx));
  } while ((D0F0xBC_xE03002E4.Value & Mask2 )!= Mask2);

  //Step 12: Set PGFSM power up override bits so SMU will not power up disabled RB
  D0F0xBC_xE0300100.Value = 0x3 << 11;
  D0F0xBC_xE03000FC.Value = 0;
  D0F0xBC_xE03000FC.Field.RegAddr = 1 ;
  GnbRegisterWriteTN (D0F0xBC_xE0300100_TYPE, D0F0xBC_xE0300100_ADDRESS, &D0F0xBC_xE0300100.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  D0F0xBC_xE03000FC.Field.FsmAddr = RbNumber;
  GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));
  D0F0xBC_xE03000FC.Field.FsmAddr = RbNumber + 1;
  GnbRegisterWriteTN (D0F0xBC_xE03000FC_TYPE, D0F0xBC_xE03000FC_ADDRESS, &D0F0xBC_xE03000FC.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));
  GnbLibStallS3Save (1, GnbLibGetHeader (Gfx));

  //Step 13: Turn off PGFSM clock
  GnbRegisterReadTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, 0, GnbLibGetHeader (Gfx));
  ex1009.Field.ex1009_1  = 1;
  GnbRegisterWriteTN (TYPE_D0F0xBC , 0xe0300328 , &ex1009.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  //Step 14: Disable PGFSM commands during reset
  GnbRegisterReadTN (D0F0xBC_xE0003024_TYPE, D0F0xBC_xE0003024_ADDRESS, &D0F0xBC_xE0003024.Value, 0, GnbLibGetHeader (Gfx));
  D0F0xBC_xE0003024.Value &= 0xFFFFFFFE;
  GnbRegisterWriteTN (D0F0xBC_xE0003024_TYPE, D0F0xBC_xE0003024_ADDRESS, &D0F0xBC_xE0003024.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Gfx));

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxShutDownDisabledRbsTN Exit\n");

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize GFX straps.
 *
 *
 * @param[in] Gfx             Pointer to global GFX configuration
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxEnvInitTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  )
{
  D0F0x64_x1C_STRUCT  D0F0x64_x1C;
  D0F0x64_x1D_STRUCT  D0F0x64_x1D;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInitTN Enter\n");

  GnbLibPciIndirectRead (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1C_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x1C.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciIndirectRead (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0x64_x1C.Field.AudioNonlegacyDeviceTypeEn = 0x0;
  D0F0x64_x1C.Field.F0NonlegacyDeviceTypeEn = 0x0;

  D0F0x64_x1D.Field.IntGfxAsPcieEn = 0x1;
  D0F0x64_x1C.Field.RcieEn = 0x1;

  D0F0x64_x1D.Field.VgaEn = 0x1;

  D0F0x64_x1C.Field.AudioEn = Gfx->GnbHdAudio;
  D0F0x64_x1C.Field.F0En = 0x1;
  D0F0x64_x1C.Field.RegApSize = 0x1;

  if (Gfx->UmaInfo.UmaSize > 128 * 0x100000) {
    D0F0x64_x1C.Field.MemApSize = 0x1;
  } else if (Gfx->UmaInfo.UmaSize > 64 * 0x100000) {
    D0F0x64_x1C.Field.MemApSize = 0x0;
  } else if (Gfx->UmaInfo.UmaSize > 32 * 0x100000) {
    D0F0x64_x1C.Field.MemApSize = 0x2;
  } else {
    D0F0x64_x1C.Field.MemApSize = 0x3;
  }
  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1D_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1D.Value,
    GnbLibGetHeader (Gfx)
    );

  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1C_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1C.Value,
    GnbLibGetHeader (Gfx)
    );

  D0F0x64_x1C.Field.WriteDis = 0x1;

  GnbLibPciIndirectWrite (
    GNB_SBDFO | D0F0x60_ADDRESS,
    D0F0x64_x1C_ADDRESS | IOC_WRITE_ENABLE,
    AccessS3SaveWidth32,
    &D0F0x64_x1C.Value,
    GnbLibGetHeader (Gfx)
    );

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInitTN Exit\n");
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
GfxEnvInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  GFX_PLATFORM_CONFIG   *Gfx;
  GNB_HANDLE            *GnbHandle;
  UINT32                Property;
  BOOLEAN               ShutDownDisabledSimd;
  BOOLEAN               ShutDownDisabledRb;
  UINT8                 SclkDid;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInterfaceTN Enter\n");
  AgesaStatus = AGESA_SUCCESS;
  Property = TABLE_PROPERTY_DEAFULT;
  ShutDownDisabledSimd = GnbBuildOptions.CfgUnusedSimdPowerGatingEnable;
  ShutDownDisabledRb = GnbBuildOptions.CfgUnusedRbPowerGatingEnable;

  Status = GfxLocateConfigData (StdHeader, &Gfx);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    if (Gfx->UmaInfo.UmaMode != UMA_NONE) {
      Status = GfxEnvInitTN (Gfx);
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
  //
  // Set sclk to 100Mhz
  //
  SclkDid = GfxRequestSclkTNS3Save (
              GfxLibCalculateDidTN (98 * 100, StdHeader),
              StdHeader
              );

  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);
  Status = GnbProcessTable (
             GnbHandle,
             GfxEnvInitTableTN,
             Property,
             GNB_TABLE_FLAGS_FORCE_S3_SAVE,
             StdHeader
             );
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  if (ShutDownDisabledSimd == TRUE) {
    GfxShutDownDisabledSimdsTN (Property, Gfx);
  }

  if ((Property & TABLE_PROPERTY_IGFX_DISABLED) != 0) {
    if (ShutDownDisabledRb == TRUE) {
      GfxShutDownDisabledRbsTN (Gfx);
    }
  }
  //
  // Restore Sclk
  //
  GfxRequestSclkTNS3Save (
    SclkDid,
    StdHeader
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxEnvInterfaceTN Exit [0x%x]\n", AgesaStatus);
  return  Status;
}
