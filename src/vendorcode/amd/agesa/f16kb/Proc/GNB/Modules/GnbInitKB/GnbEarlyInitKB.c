/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB early post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84838 $   @e \$Date: 2012-12-20 10:04:21 -0600 (Thu, 20 Dec 2012) $
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
#include  "OptionGnb.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbTable.h"
#include  "GnbSmuInitLibV7.h"
#include  "GnbSmuFirmwareKB.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "cpuF16PowerMgmt.h"
#include  "excel925.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBEARLYINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_TABLE ROMDATA GnbEarlierInitTableBeforeSmuKB [];
extern GNB_TABLE ROMDATA GnbEarlyInitTableKB [];
extern GNB_BUILD_OPTIONS GnbBuildOptions;
extern BUILD_OPT_CFG     UserOptions;

#define CFG_DISP_PHY_TDP_LIMIT                      0
#define CFG_FCH_PWR_CREDIT                          0


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbEarlyInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
GnbEarlierInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
GnbScsInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

AGESA_STATUS
OptionGnbInstall581 (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

STATIC VOID
GnbEarlyInitKB129_fun (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AMD_EARLY_PARAMS        *EarlyParams;
  UINT32                  v0;
  UINT32                  v1;
  D18F4x110_STRUCT        D18F4x110;
  D18F5xE0_STRUCT         D18F5xE0;
  UINT32                  v4;
  UINT32                  Data;

  EarlyParams = (AMD_EARLY_PARAMS *) StdHeader;

  v0 = ((EarlyParams->PlatformConfig.VrmProperties[CoreVrm].CurrentLimit / 10) & 0xFFFF) |
	  (((EarlyParams->PlatformConfig.VrmProperties[NbVrm].CurrentLimit / 10) & 0xFFFF) << 16);
  GnbRegisterWriteKB (GnbGetHandle (StdHeader), 0x4, 0x3FA04, &v0, 0, StdHeader);

  v1 = ((EarlyParams->PlatformConfig.VrmProperties[CoreVrm].SviOcpLevel / 10) & 0xFFFF) |
	  (((EarlyParams->PlatformConfig.VrmProperties[NbVrm].SviOcpLevel / 10) & 0xFFFF) << 16);
  GnbRegisterWriteKB (GnbGetHandle (StdHeader), 0x4, 0x3f994, &v1, 0, StdHeader);

  GnbRegisterReadKB (GnbGetHandle (StdHeader),
      D18F4x110_TYPE, D18F4x110_ADDRESS, &D18F4x110, 0, StdHeader);
  GnbRegisterReadKB (GnbGetHandle (StdHeader),
      D18F5xE0_TYPE, D18F5xE0_ADDRESS, &D18F5xE0, 0, StdHeader);

  // Period = (2^(RunAvgRange + 1)*CSampleTimer*5.12)us - round down to nearest 10uS
  Data = D18F5xE0.Field.RunAvgRange + 1;
  Data = 1 << Data;
  Data *= D18F4x110.Field.CSampleTimer;
  Data = ((Data * 512) / 1000) * 10;

  IDS_HDT_CONSOLE (GNB_TRACE, "VPC period = %d\n", Data);

  v4 = Data;
  GnbRegisterWriteKB (GnbGetHandle (StdHeader),
      0x4, 0x3F830, &v4, 0, StdHeader);

}

/*----------------------------------------------------------------------------------------*/
STATIC VOID
GnbEarlyInitKB180_fun (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D18F3xA8_STRUCT         D18F3xA8;
  UINT32                  D0F0xBC_x3F9D8;
  UINT64                  MsrData;

  GnbRegisterReadKB (GnbGetHandle (StdHeader), D18F3xA8_TYPE, D18F3xA8_ADDRESS, &D18F3xA8, 0, StdHeader);
  GnbRegisterReadKB (GnbGetHandle (StdHeader), 0x4, 0x3F9D8, &D0F0xBC_x3F9D8, 0, StdHeader);
  D0F0xBC_x3F9D8 &= ~(0xF << 24);
  D0F0xBC_x3F9D8 |= (D18F3xA8.Field.PopDownPstate & 0x7) << 24;
  LibAmdMsrRead ((PS_REG_BASE + D18F3xA8.Field.PopDownPstate), &MsrData, StdHeader);
  D0F0xBC_x3F9D8 |= (((MsrData & (1 << 22)) == 0) ? (1 << 27) :0);
  GnbRegisterWriteKB (GnbGetHandle (StdHeader), 0x4, 0x3F9D8, &D0F0xBC_x3F9D8, 0, StdHeader);
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init Package Power Support
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 */

STATIC VOID
GnbInitPkgPowerKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  UINT32                  D0F0xBC_x3F834;
  UINT32                  D0F0xBC_x3F838;
  UINT32                  D0F0xBC_x3F844;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbInitPkgPowerKB Enter\n");

  D0F0xBC_x3F834 = CFG_DISP_PHY_TDP_LIMIT;
  GnbRegisterWriteKB (GnbGetHandle (StdHeader), TYPE_D0F0xBC, 0x3F834,
      &D0F0xBC_x3F834, 0, StdHeader);

  D0F0xBC_x3F838 = CFG_FCH_PWR_CREDIT;
  GnbRegisterWriteKB (GnbGetHandle (StdHeader), TYPE_D0F0xBC, 0x3F838,
      &D0F0xBC_x3F838, 0, StdHeader);

  GnbRegisterReadKB (GnbGetHandle (StdHeader), 0x4, 0x3f844,
      &D0F0xBC_x3F844, 0, StdHeader);
  D0F0xBC_x3F844 &= 0xe0000000;
  GnbRegisterWriteKB (GnbGetHandle (StdHeader), 0x4, 0x3f844,
      &D0F0xBC_x3F844, 0, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbInitPkgPowerKB Exit\n");
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Request VDDB Pmin
 *
 *
 *
 * @param[in]  GnbHandle  GNB_HANDLE
 * @param[in]  StdHeader  Standard configuration header
 */

STATIC VOID
GnbRequestVddNbPminKB (
  IN      GNB_HANDLE                      *GnbHandle,
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  D18F5x170_STRUCT        D18F5x170;
  D18F5x160_STRUCT        D18F5x160;
  UINT32                  Millivolt;
  UINT32                  NbVid;
  DEV_OBJECT              DevObject;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbRequestVddNbPminKB Enter\n");
  // 1) Read F5x170[NbPstateMaxVal] to find NB Pmin.
  GnbRegisterReadKB (GnbHandle, D18F5x170_TYPE, D18F5x170_ADDRESS, &D18F5x170, 0, StdHeader);
  // 2) Determine voltage associated with NB Pmin.  Read F5x(160+4*NbPstateMaxVal)[NbVid].  (For example, NBP0 is F5x160, NBP1 is F5x164, etc)
  GnbRegisterReadKB (GnbHandle, D18F5x160_TYPE, (D18F5x160_ADDRESS + (4 * D18F5x170.Field.NbPstateMaxVal)), &D18F5x160, 0, StdHeader);
  NbVid = (D18F5x160.Field.NbVid_7_ << 7) | (D18F5x160.Field.NbVid_6_0_);
  // 3) Make voltage request (via BIOSSMC_MSG_VDDNB_REQUEST) for voltage determined by Step 2.  Note that message requires encoding in voltage, not VID.
  //    Use the following equation to decode SVI2 VIDs:
  //      Voltage = 1.55 - (0.00625*VID)
  //    Furthermore, VDDNB requests to SMU should be encoded in 0.25mV steps.
  //    Therefore, voltage should be translated into mV and then multiplied by 4, to be encoded as 0.25mV steps.
  //    Or... ( 1.55 * 1000 * 4) - (.00625 * 1000 * 4) * VID) = (1550 * 4) - (25 * VID)
  Millivolt = (1550 * 4) - (25 * NbVid);

  IDS_HDT_CONSOLE (GNB_TRACE, "  Set Voltage for NbPstateMaxVal = 0x%x, Vid code 0x%x = %d mV\n", D18F5x170.Field.NbPstateMaxVal, NbVid, Millivolt);
  DevObject.StdHeader = StdHeader;
  DevObject.GnbHandle = GnbHandle;
  DevObject.DevPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  GnbSmuServiceRequestV7 (
    &DevObject,
    SMC_MSG_VDDNB_REQUEST,
    Millivolt,
    0
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbRequestVddNbPminKB Exit\n");
  return;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Initialize GFX straps.
 *
 *
 * @param[in] StdHeader  Standard configuration header
 */
STATIC VOID
GfxStrapsEarlyInitKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  GnbRegistersKB7236_STRUCT    v0;
  GnbRegistersKB7269_STRUCT    v1;
  GnbRegistersKB7314_STRUCT    v2;
  GnbRegistersKB7341_STRUCT    v3;
  GNB_HANDLE                 *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxStrapsEarlyInitKB Enter\n");

  GnbHandle = GnbGetHandle (StdHeader);

  GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x13014AB, &v0.Value, 0, StdHeader);
  GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x13014AC, &v1.Value, 0, StdHeader);
  GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x13014B6, &v2.Value, 0, StdHeader);
  GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x013014BE, &v3.Value, 0, StdHeader);

  v0.Field.bit17 = 1;

  v2.Field.bita = 0x0;
  v2.Field.StrapBifF0LegacyDeviceTypeDis = 0x0;

  v1.Field.bit16 = UserOptions.CfgGnbHdAudio;
  v2.Field.bit13 = UserOptions.CfgGnbHdAudio;

  // Enable PCI Vendor Specific Capabilities
  v3.Field.bit_20 = 1;

  GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x13014AB, &v0.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x13014AC, &v1.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x13014B6, &v2.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x013014BE, &v3.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxStrapsEarlyInitKB Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable Gfx gBIF
 *
 *
 * @param[in] StdHeader  Standard configuration header
 * @retval    AGESA_STATUS
 */

STATIC AGESA_STATUS
GfxGBifEnableKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  GnbRegistersKB7208_STRUCT    var0;
  GnbRegistersKB7514_STRUCT    var1;
  UINT32                      D0F0xBC_xC00C0000;
  GNB_HANDLE                  *GnbHandle;

  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGBifEnableKB Enter\n");
  GnbHandle = GnbGetHandle (StdHeader);

  GnbRegisterReadKB (GnbHandle, 0x4, 0xc00c0000, &D0F0xBC_xC00C0000, 0, StdHeader);
  if (((D0F0xBC_xC00C0000 >> 1) & 1 )!= 1) {
    GfxStrapsEarlyInitKB (StdHeader);
  }

  GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x1301486, &var0.Value, 0, StdHeader);
  GnbRegisterReadKB (GnbHandle, TYPE_D0F0xD4, 0x1091518, &var1.Value, 0, StdHeader);
  var0.Field.bit2 = 0x1;
  var1.Field.bit0 = 0x1;
  GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x1091518, &var1.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  GnbRegisterWriteKB (GnbHandle, TYPE_D0F0xD4, 0x1301486, &var0.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "GfxGBifEnableKB Exit\n");

  return  AGESA_SUCCESS;
}
/*----------------------------------------------------------------------------------------*/
/**
 * GNB init at early post
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbEarlyInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS     Status;
  AGESA_STATUS     AgesaStatus;
  GNB_HANDLE       *GnbHandle;
  UINT32           Property;

  AgesaStatus = AGESA_SUCCESS;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlyInterfaceKB Enter\n");
  GnbHandle = GnbGetHandle (StdHeader);

  Property = TABLE_PROPERTY_DEFAULT;
  Property |= UserOptions.CfgGnbSyncFloodPinAsNmi ? 0x00000400ul : 0;
  Property |= GnbBuildOptions.CfgSviRevision == 2 ? TABLE_PROPERTY_SVI2 : 0;

  IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);

  while (GnbHandle != NULL) {
    GnbEarlyInitKB129_fun (StdHeader);
    GnbEarlyInitKB180_fun (StdHeader);
    GnbInitPkgPowerKB (StdHeader);
    Status = GnbProcessTable (
               GnbHandle,
               GnbEarlyInitTableKB,
               Property,
               0,
               StdHeader
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    GnbRequestVddNbPminKB (GnbHandle, StdHeader);
    Status = GfxGBifEnableKB (StdHeader);
    ASSERT (Status == AGESA_SUCCESS);
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlyInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * GNB init at earlier post
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbEarlierInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS      Status;
  AGESA_STATUS     AgesaStatus;
  GNB_HANDLE        *GnbHandle;

  AgesaStatus = AGESA_SUCCESS;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlierInterfaceKB Enter\n");
  GnbHandle = GnbGetHandle (StdHeader);
  while (GnbHandle != NULL) {
    Status = GnbProcessTable (
               GnbHandle,
               GnbEarlierInitTableBeforeSmuKB,
               0,
               0,
               StdHeader
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    Status = GnbSmuFirmwareLoadV7 (GnbHandle, (FIRMWARE_HEADER_V7*) &FirmwareKB[0], StdHeader);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEarlierInterfaceKB Exit [0x%x]\n", Status);
  return  AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
OptionGnbInstall581 (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS      Status;
  AGESA_STATUS      AgesaStatus;
  GNB_HANDLE        *GnbHandle;

  AgesaStatus = AGESA_SUCCESS;
  Status = AGESA_SUCCESS;
  GnbHandle = GnbGetHandle (StdHeader);

  while (GnbHandle != NULL) {
    Status = GnbSmuInitLibV7139_fun0 (GnbHandle, &excel925[0], StdHeader);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  return  AgesaStatus;
}
