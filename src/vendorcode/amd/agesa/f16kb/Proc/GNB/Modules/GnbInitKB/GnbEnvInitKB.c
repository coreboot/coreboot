/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB env post initialization.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 85506 $   @e \$Date: 2013-01-08 15:38:33 -0600 (Tue, 08 Jan 2013) $
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
#include  "heapManager.h"
#include  "Gnb.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbPcieConfig.h"
#include  "GnbSmuInitLibV7.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbNbInitLibV5.h"
#include  "GnbF1TableKB.h"
#include  "GnbF1Table.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBENVINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern GNB_BUILD_OPTIONS      GnbBuildOptions;
extern GNB_TABLE ROMDATA      GnbEnvInitTableKB [];
extern GNB_TABLE ROMDATA      GnbIommuInitTableKB [];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbEnvInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * NB Dynamic Wake
 * ORB_CNB_Wake signal is used to inform the CNB NCLK controller and GNB LCLK controller
 *   that ORB is (or will soon) push data into the synchronizer FIFO (i.e. wake is high).
 *
 * @param[in] NbPciAddress    Gnb PCI address
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
STATIC
GnbOrbDynamicWakeKB (
  IN      PCI_ADDR            NbPciAddress,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{

  D0F0x98_x2C_STRUCT D0F0x98_x2C;

  GnbLibPciIndirectRead (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x2C_ADDRESS,
    AccessWidth32,
    &D0F0x98_x2C.Value,
    StdHeader
    );

  // Enable Dynamic wake
  // Wake Hysteresis timer value.  Specifies the number of SMU pulses to count.
  if (GnbBuildOptions.CfgOrbDynWakeEnable) {
    D0F0x98_x2C.Field.DynWakeEn = 1;
  } else {
    D0F0x98_x2C.Field.DynWakeEn = 0;
  }
  D0F0x98_x2C.Field.WakeHysteresis = 0x19;

  IDS_OPTION_HOOK (IDS_GNB_ORBDYNAMIC_WAKE, &D0F0x98_x2C, StdHeader);

  GnbLibPciIndirectWrite (
    NbPciAddress.AddressValue | D0F0x94_ADDRESS,
    D0F0x98_x2C_ADDRESS,
    AccessS3SaveWidth32,
    &D0F0x98_x2C.Value,
    StdHeader
    );
}
/*----------------------------------------------------------------------------------------*/
/**
 * HTC Data
 *
 * @param[in] GnbHandle       Gnb Header
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
STATIC
GnbFillHtcData (
  IN      GNB_HANDLE          *GnbHandle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{

  UINT32           D0F0xBC_x3FDD4;
  D18F3x64_STRUCT  D18F3x64;
  UINT16           *p = (UINT16 *)&D0F0xBC_x3FDD4;
  GnbRegisterReadKB (GnbHandle, D18F3x64_TYPE, D18F3x64_ADDRESS,
    &D18F3x64.Value, 0, StdHeader);

  GnbRegisterReadKB (GnbHandle, 0x4, 0x3fdd4,
    &D0F0xBC_x3FDD4, 0, StdHeader);

  if (D18F3x64.Field.HtcEn == 1) {

    p[1] = D18F3x64.Field.HtcTmpLmt / 2 + 52;
    p[0] =
        p[1] - (D18F3x64.Field.HtcHystLmt / 2);

    p[1] =
        (p[1] + 49) << 3;
    p[0] =
        (p[0] + 49) << 3;

  } else {

    p[0] = 0;
    p[1] = 0;

  }


  GnbRegisterWriteKB (GnbHandle, 0x4,
        0x3fdd4, &D0F0xBC_x3FDD4, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

}

/*----------------------------------------------------------------------------------------*/
/**
 * LHTC Data
 *
 * @param[in] GnbHandle       Gnb Header
 * @param[in] StdHeader       Standard Configuration Header
 */

VOID
STATIC
GnbUpdateLhtcData (
  IN      GNB_HANDLE          *GnbHandle,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AMD_ENV_PARAMS            *EnvParams;
  UINT32                    LhtcParameter;
  D18F3x64_STRUCT           D18F3x64;
  UINT32                    D0F0xBC_xC0104090;
  DEV_OBJECT                DevObject;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbUpdateLhtcData Enter\n");
  EnvParams = (AMD_ENV_PARAMS *) StdHeader;

  // Make sure both config values are valid
  if ((EnvParams->PlatformConfig.LhtcTemperatureLimit >= 520) && (EnvParams->PlatformConfig.HtcTemperatureLimit >= 520)) {
    // Platform config value is in .1 degrees
    LhtcParameter = (EnvParams->PlatformConfig.LhtcTemperatureLimit - 520) / 5;
    // Read HtcTmpLmt field from D18F3
    GnbRegisterReadKB (GnbGetHandle (StdHeader), D18F3x64_TYPE, D18F3x64_ADDRESS, &D18F3x64.Value, 0, StdHeader);
    GnbRegisterReadKB (GnbGetHandle (StdHeader), 0x4, 0xC0104090, &D0F0xBC_xC0104090, 0, StdHeader);
    if ((D18F3x64.Field.HtcEn == 1) && (LhtcParameter < D18F3x64.Field.HtcTmpLmt) && (LhtcParameter < ((D0F0xBC_xC0104090 >> 23) & 0xFF)))  {
      // All values are valid, so we can send the service to the SMU
      DevObject.StdHeader = StdHeader;
      DevObject.GnbHandle = GnbGetHandle (StdHeader);
      DevObject.DevPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
      GnbSmuServiceRequestV7 (
        &DevObject,
        SMC_MSG_LHTC_LIMIT_SetLimit,
        LhtcParameter,
        GNB_REG_ACC_FLAG_S3SAVE
        );
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbUpdateLhtcData Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * GNB init at env
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbEnvInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS      Status;
  AGESA_STATUS      AgesaStatus;
  UINT32            Property;
  GNB_HANDLE        *GnbHandle;
  PCI_ADDR          GnbPciAddress;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnvInterfaceKB Enter\n");
  Property = TABLE_PROPERTY_DEFAULT;
  Property |= GnbBuildOptions.CfgBapmSupport ? TABLE_PROPERTY_BAPM : 0;
  Property |= GnbBuildOptions.CfgLhtcSupport ? TABLE_PROPERTY_LHTC : 0;
  Property |= GnbBuildOptions.CfgTdcSupport ? 0x00040000ul : 0;
  Property |= 0x00080000ul;
  AgesaStatus = AGESA_SUCCESS;
  GnbLoadF1TableKB (StdHeader);
  GnbHandle = GnbGetHandle (StdHeader);
  while (GnbHandle != NULL) {
    GnbPciAddress = GnbGetHostPciAddress (GnbHandle);
    Status = GnbSetTomV5 (GnbPciAddress, StdHeader);
    GnbOrbDynamicWakeKB (GnbPciAddress, StdHeader);
    GnbFillHtcData (GnbHandle, StdHeader);
    if (GnbIsGnbConnectedToSb (GnbHandle)) {
      GnbLpcDmaDeadlockPreventionV4 (GnbHandle, StdHeader);
    }

    IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);

    Status = GnbProcessTable (
               GnbHandle,
               GnbEnvInitTableKB,
               Property,
               GNB_TABLE_FLAGS_FORCE_S3_SAVE,
               StdHeader
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    if ((Property & TABLE_PROPERTY_LHTC) != 0) {
      GnbUpdateLhtcData (GnbHandle, StdHeader);
    }

    Status = GnbProcessTable (
               GnbHandle,
               GnbIommuInitTableKB,
               Property,
               GNB_TABLE_FLAGS_FORCE_S3_SAVE,
               StdHeader
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbEnvInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
