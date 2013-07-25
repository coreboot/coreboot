/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB mid post initialization.
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
#include  "GnbLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbF1Table.h"
#include  "heapManager.h"
#include  "GnbGfxFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV5.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbSmuInitLibV7.h"
#include  "GnbTable.h"
#include  "GnbRegisterAccKB.h"
#include  "GnbRegistersKB.h"
#include  "GnbFamServices.h"
#include  "OptionGnb.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_GNBMIDINITKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS      GnbBuildOptions;
extern GNB_TABLE ROMDATA      GnbMidInitTableKB[];


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbMidInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 *
 *
 * @param[out]      Index             State index
 * @param[in]       PpF1s
 * @retval          TRUE              State is valid
 */
STATIC BOOLEAN
GnbIsF1dStateValid (
  IN      UINT8                         Index,
  IN      PP_F1_ARRAY_V2              *PpF1s
  )
{
  BOOLEAN Result;
  Result = FALSE;

  if (Index >= 5) {
    Result = FALSE;
  } else if ((PpF1s->PP_FUSE_ARRAY_V2_fld37 & (1 << Index)) ||
             (PpF1s->PP_FUSE_ARRAY_V2_fld38 & (1 << Index))) {
    Result = TRUE;
  }
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * LCLK DPM Initialization
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

STATIC AGESA_STATUS
GnbLclkDpmInitKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS              Status;
  PCIe_PLATFORM_CONFIG      *Pcie;
  PP_F1_ARRAY_V2          *PpF1Array;
  PCI_ADDR                  GnbPciAddress;
  UINT8                     Index;
  UINT8                     LclkDpmMode;

  UINT32                    D0F0xBC_x3FD00[8];
  UINT32                    D0F0xBC_x3FD04[8];
  UINT32                    D0F0xBC_x3FD08[8];
  UINT32                    D0F0xBC_x3FD0C[8];
  UINT32                    D0F0xBC_x3FD10[8];
  UINT32                    D0F0xBC_x3FDC8;
  UINT32                    D0F0xBC_x3F820;
  UINT32                    D0F0xBC_x3FDC4;
  UINT32                    D0F0xBC_xC0200110;

  GNB_HANDLE                *GnbHandle;
  DEV_OBJECT                DevObject;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLclkDpmInitKB Enter\n");
  Status = AGESA_SUCCESS;

  GnbHandle = GnbGetHandle (StdHeader);
  DevObject.DevPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
  DevObject.GnbHandle = GnbHandle;
  DevObject.StdHeader = StdHeader;

  GnbRegisterReadKB (GnbHandle, 0x4,
      0x3f820, &D0F0xBC_x3F820, 0, StdHeader);
  D0F0xBC_x3F820 &= ~0xFF;
  GnbRegisterWriteKB (GnbHandle, 0x4,
      0x3f820, &D0F0xBC_x3F820, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

  LclkDpmMode = GnbBuildOptions.LclkDpmEn ? LclkDpmRcActivity : LclkDpmDisabled;
  IDS_OPTION_HOOK (IDS_GNB_LCLK_DPM_EN, &LclkDpmMode, StdHeader);
  if (LclkDpmMode == LclkDpmRcActivity) {
    PpF1Array = GnbLocateHeapBuffer (AMD_PP_F1_TABLE_HANDLE, StdHeader);
    if (PpF1Array != NULL) {
      Status = PcieLocateConfigurationData (StdHeader, &Pcie);
      if (Status == AGESA_SUCCESS) {
        GnbPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
        //Clear DPM_EN bit in LCLK_DPM_CNTL register
        //Call BIOS service SMC_MSG_CONFIG_LCLK_DPM to disable LCLK DPM
        GnbRegisterReadKB (GnbHandle, 0x4,
            0x3fdc8, &D0F0xBC_x3FDC8, 0, StdHeader);
        D0F0xBC_x3FDC8 &= 0x00FFFFFF;
        GnbRegisterWriteKB (GnbHandle, 0x4,
            0x3fdc8, &D0F0xBC_x3FDC8, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

        //Initialize LCLK states
        LibAmdMemFill (D0F0xBC_x3FD00, 0x00, sizeof (D0F0xBC_x3FD00), StdHeader);
        LibAmdMemFill (D0F0xBC_x3FD04, 0x00, sizeof (D0F0xBC_x3FD04), StdHeader);
        LibAmdMemFill (D0F0xBC_x3FD08, 0x00, sizeof (D0F0xBC_x3FD08), StdHeader);
        LibAmdMemFill (D0F0xBC_x3FD0C, 0x00, sizeof (D0F0xBC_x3FD0C), StdHeader);
        LibAmdMemFill (D0F0xBC_x3FD10, 0x00, sizeof (D0F0xBC_x3FD10), StdHeader);

        for (Index = 0; Index < 8; ++Index) {

          if (GnbIsF1dStateValid (Index, PpF1Array)) {

            ASSERT (Index < 5);
            IDS_HDT_CONSOLE (GNB_TRACE, "  Valid Lclk state: %d\n", Index);

            // CNTL0
            D0F0xBC_x3FD00[Index] = 0;
            D0F0xBC_x3FD00[Index] |= (PpF1Array->PP_FUSE_ARRAY_V2_fld5[Index] & 0xFF) << 16;
            D0F0xBC_x3FD00[Index] |= 1 << 24;

            // CNTL1
            D0F0xBC_x3FD04[Index] = GnbTranslateVidCodeToMillivoltV5 (
              PpF1Array->PP_FUSE_ARRAY_V2_fld32[Index], StdHeader
              ) * 4 / 100;

            // CNTL2
            // D0F0xBC_x3FD08 defined as zero and is already zeroed above
            D0F0xBC_x3FD08[Index] = 0;

            // CNTL3
            D0F0xBC_x3FD0C[Index] =
                GfxFmCalculateClock (PpF1Array->PP_FUSE_ARRAY_V2_fld5[Index], StdHeader);
            D0F0xBC_x3FD10[Index] &= ~(0xFF << 8);
            switch (D0F0xBC_x3FD0C[Index]) {
            case 20000:
              D0F0xBC_x3FD10[Index] |= 0x7 << 8;
              break;
            case 30000:
              D0F0xBC_x3FD10[Index] |= 0x2 << 8;
              break;
            case 40000:
              D0F0xBC_x3FD10[Index] |= 0x3 << 8;
              break;
            case 15000:
              D0F0xBC_x3FD10[Index] |= 0x6 << 8;
              break;
            case 10000:
              D0F0xBC_x3FD10[Index] |= 0x8 << 8;
              break;
            default:
              break;
            }

            // Activity Threshold
            D0F0xBC_x3FD10[Index] &= ~0xFFFF0000;
            D0F0xBC_x3FD10[Index] |= 50 << 24;
          } else {

            IDS_HDT_CONSOLE (GNB_TRACE, "  Invalid Lclk state: %d\n", Index);

            // CNTL0
            D0F0xBC_x3FD00[Index] = 0xF800;

            // CNTL1
            D0F0xBC_x3FD04[Index] = 0x0;

            // CNTL2
            // D0F0xBC_x3FD08 defined as zero and is already zeroed above
            D0F0xBC_x3FD08[Index] = 0;

            // CNTL3
            D0F0xBC_x3FD0C[Index] = 0;

            // Activity Threshold
            D0F0xBC_x3FD10[Index] &= ~0xFFFF0000;
            D0F0xBC_x3FD10[Index] |= 50 << 24;
          }

          GnbRegisterWriteKB (
            GnbHandle,
            0x4,
            0x3fd00 + Index * 0x14,
            &D0F0xBC_x3FD00[Index],
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteKB (
            GnbHandle,
            0x4,
            0x3fd04 + Index * 0x14,
            &D0F0xBC_x3FD04[Index],
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteKB (
            GnbHandle,
            0x4,
            0x3fd08 + Index * 0x14,
            &D0F0xBC_x3FD08[Index],
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteKB (
            GnbHandle,
            0x4,
            0x3fd0c + Index * 0x14,
            &D0F0xBC_x3FD0C[Index],
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteKB (
            GnbHandle,
            0x4,
            0x3fd10 + Index * 0x14,
            &D0F0xBC_x3FD10[Index],
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
        }
        //Enable LCLK DPM Voltage Scaling
        GnbRegisterReadKB (GnbHandle, 0x4,
            0x3fdc8, &D0F0xBC_x3FDC8, 0, StdHeader);
	D0F0xBC_x3FDC8 &= 0xFF;
        D0F0xBC_x3FDC8 |= 1 << 16 | 1 << 24;
        GnbRegisterWriteKB (GnbHandle, 0x4,
            0x3fdc8, &D0F0xBC_x3FDC8, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

        GnbRegisterReadKB (GnbHandle, 0x4,
            0x3fdc4, &D0F0xBC_x3FDC4, 0, StdHeader);
        D0F0xBC_x3FDC4 = 5;
        GnbRegisterWriteKB (GnbHandle, 0x4,
            0x3fdc4, &D0F0xBC_x3FDC4, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

        GnbRegisterReadKB (GnbHandle, 0x4,
            0x3f820, &D0F0xBC_x3F820, 0, StdHeader);
        D0F0xBC_x3F820 &= ~0xFF; D0F0xBC_x3F820 |= 1;
        GnbRegisterWriteKB (GnbHandle, 0x4,
            0x3f820, &D0F0xBC_x3F820, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

        GnbRegisterReadKB (GnbHandle, 0x4,
            0xc0200110, &D0F0xBC_xC0200110, 0, StdHeader);
	D0F0xBC_xC0200110 &= ~(1 << 2 | 7 << 5 | 0xfffff800);
        D0F0xBC_xC0200110 &= ~(1 << 0 | 1 << 1 | 1 << 8);
        D0F0xBC_xC0200110 |= 3<<3 | 1 << 9 | 1 << 10; //.Field.BusyCntSel = 3;
        GnbRegisterWriteKB (GnbHandle, 0x4,
            0xc0200110, &D0F0xBC_xC0200110, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);


        GnbSmuServiceRequestV7 (
          &DevObject,
          SMC_MSG_LCLK_DPM_ENABLE,
          0,
          GNB_REG_ACC_FLAG_S3SAVE
          );

      }
    } else {
      Status = AGESA_ERROR;
    }

    GnbSmuServiceRequestV7 (
      &DevObject,
      SMC_MSG_ENABLE_ALLCLK_MONITOR,
      0,
      GNB_REG_ACC_FLAG_S3SAVE
      );

  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLclkDpmInitKB Exit [0x%x]\n", Status);
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Mid Post Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

AGESA_STATUS
GnbMidInterfaceKB (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS      Status;
  UINT32            Property;
  AGESA_STATUS      AgesaStatus;
  GNB_HANDLE        *GnbHandle;
  PCI_ADDR            GnbPciAddress;

  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbMidInterfaceKB Enter\n");

  Property = TABLE_PROPERTY_DEFAULT;
  Property |= GfxLibIsControllerPresent (StdHeader) ? 0 : TABLE_PROPERTY_IGFX_DISABLED;
  Property |= GnbBuildOptions.LclkDeepSleepEn ? TABLE_PROPERTY_LCLK_DEEP_SLEEP : 0;
  Property |= GnbBuildOptions.CfgOrbClockGatingEnable ? 0x00000008ul : 0;
  Property |= 0x00000010ul;
  Property |= GnbBuildOptions.CfgIocSclkClockGatingEnable ? 0x00000020ul : 0;

  GnbHandle = GnbGetHandle (StdHeader);
  while (GnbHandle != NULL) {
    IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);
    Status = GnbProcessTable (
               GnbHandle,
               GnbMidInitTableKB,
               Property,
               GNB_TABLE_FLAGS_FORCE_S3_SAVE,
               StdHeader
               );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  GnbPciAddress = GnbGetHostPciAddress (GnbGetHandle (StdHeader));
  GnbLibPciIndirectWriteField (
    GnbPciAddress.AddressValue | D0F0xF8_ADDRESS,
    D0F0xFC_x0F_ADDRESS,
    D0F0xFC_x0F_GBIFExtIntrGrp_OFFSET,
    D0F0xFC_x0F_GBIFExtIntrGrp_WIDTH,
    5,
    AccessS3SaveWidth32,
    StdHeader
    );

  GnbLibPciIndirectWriteField (
    GnbPciAddress.AddressValue | D0F0xF8_ADDRESS,
    D0F0xFC_x0F_ADDRESS,
    D0F0xFC_x0F_GBIFExtIntrSwz_OFFSET,
    D0F0xFC_x0F_GBIFExtIntrSwz_WIDTH,
    0,
    AccessS3SaveWidth32,
    StdHeader
    );

  Status = GnbLclkDpmInitKB (StdHeader);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbMidInterfaceKB Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
