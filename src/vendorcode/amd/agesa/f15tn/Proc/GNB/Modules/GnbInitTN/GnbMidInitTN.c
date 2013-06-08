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
 * @e \$Revision: 64352 $   @e \$Date: 2012-01-19 03:54:04 -0600 (Thu, 19 Jan 2012) $
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
#include  "GnbPcie.h"
#include  "GnbFuseTable.h"
#include  "heapManager.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbNbInitLibV1.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbGfxInitLibV1.h"
#include  "GnbGfxConfig.h"
#include  "GnbTable.h"
#include  "GnbRegisterAccTN.h"
#include  "GnbRegistersTN.h"
#include  "OptionGnb.h"
#include  "GfxLibTN.h"
#include  "GnbFamServices.h"
#include  "GnbGfxFamServices.h"
#include  "GnbBapmCoeffCalcTN.h"
#include  "PcieComplexDataTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_GNBMIDINITTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

extern GNB_BUILD_OPTIONS      GnbBuildOptions;
extern GNB_TABLE ROMDATA      GnbMidInitTableTN[];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
#define NUM_DPM_STATES 8

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
GnbMidInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Registers needs to be set if no GFX PCIe ports beeing us
 *
 *
 *
 * @param[in]  Pcie                Pointer to PCIe_PLATFORM_CONFIG
 */

VOID
STATIC
GnbIommuMidInitCheckGfxPciePorts (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_WRAPPER_CONFIG   *WrapperList;
  BOOLEAN               GfxPciePortUsed;
  D0F2xF4_x57_STRUCT    D0F2xF4_x57;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuMidInitCheckGfxPciePorts Enter\n");
  GfxPciePortUsed = FALSE;

  WrapperList = PcieConfigGetChildWrapper (Pcie);
  ASSERT (WrapperList != NULL);
  if (WrapperList->WrapId == GFX_WRAP_ID) {
    PCIe_ENGINE_CONFIG   *EngineList;
    EngineList = PcieConfigGetChildEngine (WrapperList);
    while (EngineList != NULL) {
      if (PcieConfigIsPcieEngine (EngineList)) {
        IDS_HDT_CONSOLE (GNB_TRACE, "Checking Gfx ports device number %x\n", EngineList->Type.Port.NativeDevNumber);
        if (PcieConfigCheckPortStatus (EngineList, INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
           ((EngineList->Type.Port.PortData.LinkHotplug != HotplugDisabled) && (EngineList->Type.Port.PortData.LinkHotplug != HotplugInboard))) {
          // GFX PCIe ports beeing used
          GfxPciePortUsed = TRUE;
          IDS_HDT_CONSOLE (GNB_TRACE, "GFX PCIe ports beeing used\n");
          break;
        }
      }
      EngineList = PcieLibGetNextDescriptor (EngineList);
    }
  }

  if (!GfxPciePortUsed) {
    //D0F2xF4_x57.Field.L1ImuPcieGfxDis needs to be set
    GnbRegisterReadTN (D0F2xF4_x57_TYPE, D0F2xF4_x57_ADDRESS, &D0F2xF4_x57.Value, 0, GnbLibGetHeader (Pcie));
    D0F2xF4_x57.Field.L1ImuPcieGfxDis = 1;
    GnbRegisterWriteTN (D0F2xF4_x57_TYPE, D0F2xF4_x57_ADDRESS, &D0F2xF4_x57.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuMidInitCheckGfxPciePorts Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to for each PCIe port
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */

VOID
STATIC
GnbIommuMidInitOnPortCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_TOPOLOGY_INFO           TopologyInfo;
  D0F2xFC_x07_L1_STRUCT       D0F2xFC_x07_L1;
  D0F2xFC_x0D_L1_STRUCT       D0F2xFC_x0D_L1;
  UINT8                       L1cfgSel;
  TopologyInfo.PhantomFunction = FALSE;
  TopologyInfo.PcieToPciexBridge = FALSE;
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled) {
    TopologyInfo.PhantomFunction = TRUE;
    TopologyInfo.PcieToPciexBridge = TRUE;
  } else {
    if (PcieConfigIsSbPcieEngine (Engine)) {
      PCI_ADDR  StartSbPcieDev;
      PCI_ADDR  EndSbPcieDev;
      StartSbPcieDev.AddressValue = MAKE_SBDFO (0, 0, 0x15, 0, 0);
      EndSbPcieDev.AddressValue = MAKE_SBDFO (0, 0, 0x15, 7, 0);
      GnbGetTopologyInfoV4 (StartSbPcieDev, EndSbPcieDev, &TopologyInfo, GnbLibGetHeader (Pcie));
    } else {
      GnbGetTopologyInfoV4 (Engine->Type.Port.Address, Engine->Type.Port.Address, &TopologyInfo, GnbLibGetHeader (Pcie));
    }
  }
  L1cfgSel = (Engine->Type.Port.CoreId == 1) ? 1 : 0;
  if (TopologyInfo.PhantomFunction) {
    GnbRegisterReadTN (
      D0F2xFC_x07_L1_TYPE,
      D0F2xFC_x07_L1_ADDRESS (L1cfgSel),
      &D0F2xFC_x07_L1.Value,
      0,
      GnbLibGetHeader (Pcie)
      );
    D0F2xFC_x07_L1.Value |= BIT0;
    GnbRegisterWriteTN (
      D0F2xFC_x07_L1_TYPE,
      D0F2xFC_x07_L1_ADDRESS (L1cfgSel),
      &D0F2xFC_x07_L1.Value,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
  }
  if (TopologyInfo.PcieToPciexBridge) {
    GnbRegisterReadTN (
      D0F2xFC_x0D_L1_TYPE,
      D0F2xFC_x0D_L1_ADDRESS (L1cfgSel),
      &D0F2xFC_x0D_L1.Value,
      0,
      GnbLibGetHeader (Pcie)
      );
    D0F2xFC_x0D_L1.Field.VOQPortBits = 0x7;
    GnbRegisterWriteTN (
      D0F2xFC_x0D_L1_TYPE,
      D0F2xFC_x0D_L1_ADDRESS (L1cfgSel),
      &D0F2xFC_x0D_L1.Value,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Orb/Ioc Cgtt Override setting
 *
 *
 * @param[in]  Property   Property
 * @param[in]  StdHeader  Standard configuration header
 */

VOID
STATIC
GnbCgttOverrideTN (
  IN      UINT32                          Property,
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  UINT32                          CGINDx0_Value;
  UINT32                          CGINDx1_Value;
  GFX_PLATFORM_CONFIG             *Gfx;
  AGESA_STATUS                    Status;
  D0F0x64_x23_STRUCT              D0F0x64_x23;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbCgttOverrideTN Enter\n");

  CGINDx0_Value = 0xFFFFFFFF;
  //When orb clock gating is enabled in the BIOS clear CG_ORB_cgtt_lclk_override - bit 13
  CGINDx1_Value = 0xFFFFFFFF;
  if ((Property & TABLE_PROPERTY_ORB_CLK_GATING) == TABLE_PROPERTY_ORB_CLK_GATING) {
    CGINDx1_Value &= 0xFFFFDFFF;
  }
  //When ioc clock gating is enabled in the BIOS clear CG_IOC_cgtt_lclk_override - bit 15
  if ((Property & TABLE_PROPERTY_IOC_LCLK_CLOCK_GATING) == TABLE_PROPERTY_IOC_LCLK_CLOCK_GATING) {
    CGINDx1_Value &= 0xFFFF7FFF;
    if ((Property & TABLE_PROPERTY_IOMMU_DISABLED) != TABLE_PROPERTY_IOMMU_DISABLED) {
      //only IOMMU enabled and IOC clock gating enable
      GnbRegisterReadTN (D0F0x64_x23_TYPE, D0F0x64_x23_ADDRESS, &D0F0x64_x23.Value, 0, StdHeader);
      D0F0x64_x23.Field.SoftOverrideClk0 = 1;
      D0F0x64_x23.Field.SoftOverrideClk1 = 1;
      D0F0x64_x23.Field.SoftOverrideClk3 = 1;
      D0F0x64_x23.Field.SoftOverrideClk4 = 1;
      GnbRegisterWriteTN (D0F0x64_x23_TYPE, D0F0x64_x23_ADDRESS, &D0F0x64_x23.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
    }
  }
  //When smu sclk clock gating is enabled in the BIOS clear CG_IOC_cgtt_lclk_override - bit 18
  if ((Property & TABLE_PROPERTY_SMU_SCLK_CLOCK_GATING) == TABLE_PROPERTY_SMU_SCLK_CLOCK_GATING) {
    CGINDx1_Value &= 0xFFFBFFFF;
  }

  Status = GfxLocateConfigData (StdHeader, &Gfx);
  if (Status != AGESA_FATAL) {
    if (Gfx->GmcClockGating) {
      //In addition to above registers it is necessary to reset override bits for VMC, MCB, and MCD blocks
      // CGINDx0, clear bit 27, bit 28
      CGINDx0_Value &= 0xE7FFFFFF;
      GnbRegisterWriteTN (TYPE_CGIND, 0x0, &CGINDx0_Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
      // CGINDx1, clear bit 11
      CGINDx1_Value &= 0xFFFFF7FF;
    }

  }

  if (CGINDx1_Value != 0xFFFFFFFF) {
    GnbRegisterWriteTN (TYPE_CGIND, 0x1, &CGINDx1_Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbCgttOverrideTN Exit\n");

}

/*----------------------------------------------------------------------------------------*/
/**
 * IOMMU Mid Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

STATIC AGESA_STATUS
GnbIommuMidInit (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS          Status;
  PCIe_PLATFORM_CONFIG  *Pcie;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuMidInit Enter\n");
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  if (Status == AGESA_SUCCESS) {
    PcieConfigRunProcForAllEngines (
      DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
      GnbIommuMidInitOnPortCallback,
      NULL,
      Pcie
      );
  }

  GnbIommuMidInitCheckGfxPciePorts (Pcie);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbIommuMidInit Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * IOMMU Mid Init
 *
 *
 *
 * @param[in]  StdHeader  Standard configuration header
 * @retval     AGESA_STATUS
 */

STATIC AGESA_STATUS
GnbLclkDpmInitTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS              Status;
  PCIe_PLATFORM_CONFIG      *Pcie;
  PP_FUSE_ARRAY             *PpFuseArray;
  PCI_ADDR                  GnbPciAddress;
  UINT32                    Index;
  UINT8                     LclkDpmMode;
  D0F0xBC_x1F200_STRUCT     D0F0xBC_x1F200[NUM_DPM_STATES];
  D0F0xBC_x1F208_STRUCT     D0F0xBC_x1F208[NUM_DPM_STATES];
  D0F0xBC_x1F210_STRUCT     D0F0xBC_x1F210[NUM_DPM_STATES];
  D0F0xBC_x1F300_STRUCT     D0F0xBC_x1F300;
  ex1003_STRUCT      ex1003 [NUM_DPM_STATES];
  DOUBLE                    PcieCacLut;
  ex1072_STRUCT      ex1072 ;
  D0F0xBC_x1FE00_STRUCT     D0F0xBC_x1FE00;
  D0F0xBC_x1F30C_STRUCT     D0F0xBC_x1F30C;
  D18F3x64_STRUCT           D18F3x64;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLclkDpmInitTN Enter\n");
  Status = AGESA_SUCCESS;
  LclkDpmMode = GnbBuildOptions.LclkDpmEn ? LclkDpmRcActivity : LclkDpmDisabled;
  IDS_OPTION_HOOK (IDS_GNB_LCLK_DPM_EN, &LclkDpmMode, StdHeader);
  if (LclkDpmMode == LclkDpmRcActivity) {
    PpFuseArray = GnbLocateHeapBuffer (AMD_PP_FUSE_TABLE_HANDLE, StdHeader);
    if (PpFuseArray != NULL) {
      Status = PcieLocateConfigurationData (StdHeader, &Pcie);
      if (Status == AGESA_SUCCESS) {
        GnbPciAddress.AddressValue = MAKE_SBDFO (0, 0, 0, 0, 0);
        //Clear DPM_EN bit in LCLK_DPM_CNTL register
        //Call BIOS service SMC_MSG_CONFIG_LCLK_DPM to disable LCLK DPM
        GnbRegisterReadTN (D0F0xBC_x1F300_TYPE, D0F0xBC_x1F300_ADDRESS, &D0F0xBC_x1F300.Value, 0, StdHeader);
        D0F0xBC_x1F300.Field.LclkDpmEn = 0x0;
        GnbRegisterWriteTN (D0F0xBC_x1F300_TYPE, D0F0xBC_x1F300_ADDRESS, &D0F0xBC_x1F300.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
        GnbSmuServiceRequestV4 (
          GnbPciAddress,
          SMC_MSG_CONFIG_LCLK_DPM,
          GNB_REG_ACC_FLAG_S3SAVE,
          StdHeader
          );

        //Initialize LCLK states
        LibAmdMemFill (D0F0xBC_x1F200, 0x00, sizeof (D0F0xBC_x1F200), StdHeader);
        LibAmdMemFill (D0F0xBC_x1F208, 0x00, sizeof (D0F0xBC_x1F208), StdHeader);
        LibAmdMemFill (ex1003, 0x00, sizeof (D0F0xBC_x1F208), StdHeader);

        D0F0xBC_x1F200[0].Field.LclkDivider = PpFuseArray->LclkDpmDid[0];
        D0F0xBC_x1F200[0].Field.VID = PpFuseArray->SclkVid[PpFuseArray->LclkDpmVid[0]];
        D0F0xBC_x1F200[0].Field.LowVoltageReqThreshold = 0xa;
        D0F0xBC_x1F210[0].Field.ActivityThreshold = 0xf;

        D0F0xBC_x1F200[5].Field.LclkDivider = PpFuseArray->LclkDpmDid[1];
        D0F0xBC_x1F200[5].Field.VID = PpFuseArray->SclkVid[PpFuseArray->LclkDpmVid[1]];
        D0F0xBC_x1F200[5].Field.LowVoltageReqThreshold = 0xa;
        D0F0xBC_x1F210[5].Field.ActivityThreshold = 0x32;
        D0F0xBC_x1F200[5].Field.StateValid = 0x1;

        D0F0xBC_x1F200[6].Field.LclkDivider = PpFuseArray->LclkDpmDid[2];
        D0F0xBC_x1F200[6].Field.VID = PpFuseArray->SclkVid[PpFuseArray->LclkDpmVid[2]];
        D0F0xBC_x1F200[6].Field.LowVoltageReqThreshold = 0xa;
        D0F0xBC_x1F210[6].Field.ActivityThreshold = 0x32;
        D0F0xBC_x1F200[6].Field.StateValid = 0x1;

        GnbRegisterReadTN (TYPE_D0F0xBC , 0x1f920 , &ex1072.Value, 0, StdHeader);
        PcieCacLut = 0.0000057028 * (1 << ex1072.Field.ex1072_0 );
        IDS_HDT_CONSOLE (GNB_TRACE, "LCLK DPM1 10khz %x (%d)\n", GfxFmCalculateClock (PpFuseArray->LclkDpmDid[1], StdHeader), GfxFmCalculateClock (PpFuseArray->LclkDpmDid[1], StdHeader));
        D0F0xBC_x1FE00.Field.Data = (UINT32) GnbFpLibDoubleToInt32 (PcieCacLut * GfxFmCalculateClock (PpFuseArray->LclkDpmDid[1], StdHeader));
        GnbRegisterWriteTN (D0F0xBC_x1FE00_TYPE, D0F0xBC_x1FE00_ADDRESS, &D0F0xBC_x1FE00.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);
        PcieCacLut = 0.00000540239329 * (1 << ex1072.Field.ex1072_0 );
        ex1003[6].Field.ex1003_0  = (UINT32) GnbFpLibDoubleToInt32 (PcieCacLut * GfxFmCalculateClock (PpFuseArray->LclkDpmDid[2], StdHeader));
        IDS_HDT_CONSOLE (GNB_TRACE, "LCLK DPM2 10khz %x (%d)\n", GfxFmCalculateClock (PpFuseArray->LclkDpmDid[2], StdHeader), GfxFmCalculateClock (PpFuseArray->LclkDpmDid[2], StdHeader));

        for (Index = 0; Index < NUM_DPM_STATES; ++Index) {
          GnbRegisterWriteTN (
            D0F0xBC_x1F200_TYPE,
            D0F0xBC_x1F200_ADDRESS + Index * 0x20,
            &D0F0xBC_x1F200[Index].Value,
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteTN (
            D0F0xBC_x1F208_TYPE,
            D0F0xBC_x1F208_ADDRESS + Index * 0x20,
            &D0F0xBC_x1F208[Index].Value,
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteTN (
            D0F0xBC_x1F210_TYPE,
            D0F0xBC_x1F210_ADDRESS + Index * 0x20,
            &D0F0xBC_x1F210[Index].Value,
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
          GnbRegisterWriteTN (
            TYPE_D0F0xBC ,
            0x1f940  + Index * 4,
            &ex1003[Index].Value,
            GNB_REG_ACC_FLAG_S3SAVE,
            StdHeader
            );
        }
        //Enable LCLK DPM Voltage Scaling
        GnbRegisterReadTN (D0F0xBC_x1F300_TYPE, D0F0xBC_x1F300_ADDRESS, &D0F0xBC_x1F300.Value, 0, StdHeader);
        D0F0xBC_x1F300.Field.VoltageChgEn = 0x1;
        D0F0xBC_x1F300.Field.LclkDpmEn = 0x1;
        D0F0xBC_x1F300.Field.LclkDpmBootState = 0x5;
        GnbRegisterWriteTN (D0F0xBC_x1F300_TYPE, D0F0xBC_x1F300_ADDRESS, &D0F0xBC_x1F300.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

        //Programming Lclk Thermal Throttling Threshold
        GnbRegisterReadTN (D18F3x64_TYPE, D18F3x64_ADDRESS, &D18F3x64.Value, 0, StdHeader);
        GnbRegisterReadTN (D0F0xBC_x1F30C_TYPE, D0F0xBC_x1F30C_ADDRESS, &D0F0xBC_x1F30C.Value, 0, StdHeader);
        D0F0xBC_x1F30C.Field.LowThreshold = (UINT16) (((D18F3x64.Field.HtcTmpLmt / 2 + 52) - 1 + 49) * 8);
        D0F0xBC_x1F30C.Field.HighThreshold = (UINT16) (((D18F3x64.Field.HtcTmpLmt / 2 + 52) + 49) * 8);
        GnbRegisterWriteTN (D0F0xBC_x1F30C_TYPE, D0F0xBC_x1F30C_ADDRESS, &D0F0xBC_x1F30C.Value, GNB_REG_ACC_FLAG_S3SAVE, StdHeader);

        GnbSmuServiceRequestV4 (
          GnbPciAddress,
          SMC_MSG_CONFIG_LCLK_DPM,
          GNB_REG_ACC_FLAG_S3SAVE,
          StdHeader
          );
      }
    } else {
      Status = AGESA_ERROR;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbLclkDpmInitTN Exit [0x%x]\n", Status);
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
GnbMidInterfaceTN (
  IN      AMD_CONFIG_PARAMS               *StdHeader
  )
{
  AGESA_STATUS      Status;
  UINT32            Property;
  AGESA_STATUS      AgesaStatus;
  GNB_HANDLE        *GnbHandle;
  UINT8             SclkDid;

  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "GnbMidInterfaceTN Enter\n");

  GnbHandle = GnbGetHandle (StdHeader);
  ASSERT (GnbHandle != NULL);

  Property = TABLE_PROPERTY_DEAFULT;
  Property |= GfxLibIsControllerPresent (StdHeader) ? 0 : TABLE_PROPERTY_IGFX_DISABLED;
  Property |= GnbBuildOptions.LclkDeepSleepEn ? TABLE_PROPERTY_LCLK_DEEP_SLEEP : 0;
  Property |= GnbBuildOptions.CfgOrbClockGatingEnable ? TABLE_PROPERTY_ORB_CLK_GATING : 0;
  Property |= GnbBuildOptions.CfgIocLclkClockGatingEnable ? TABLE_PROPERTY_IOC_LCLK_CLOCK_GATING : 0;
  Property |= GnbBuildOptions.CfgIocSclkClockGatingEnable ? TABLE_PROPERTY_IOC_SCLK_CLOCK_GATING : 0;
  Property |= GnbFmCheckIommuPresent (GnbHandle, StdHeader) ? 0: TABLE_PROPERTY_IOMMU_DISABLED;
  Property |= GnbBuildOptions.SmuSclkClockGatingEnable ? TABLE_PROPERTY_SMU_SCLK_CLOCK_GATING : 0;

  IDS_OPTION_HOOK (IDS_GNB_PROPERTY, &Property, StdHeader);

  if ((Property & TABLE_PROPERTY_IOMMU_DISABLED) == 0) {
    Status = GnbEnableIommuMmioV4 (GnbHandle, StdHeader);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    Status = GnbIommuMidInit (StdHeader);
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
  }
  //
  // Set sclk to 100Mhz
  //
  SclkDid = GfxRequestSclkTNS3Save (
              GfxLibCalculateDidTN (98 * 100, StdHeader),
              StdHeader
              );

  Status = GnbProcessTable (
             GnbHandle,
             GnbMidInitTableTN,
             Property,
             GNB_TABLE_FLAGS_FORCE_S3_SAVE,
             StdHeader
             );
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  //
  // Restore Sclk
  //
  GfxRequestSclkTNS3Save (
    SclkDid,
    StdHeader
    );

  GnbCgttOverrideTN (Property, StdHeader);

  Status = GnbLclkDpmInitTN (StdHeader);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbMidInterfaceTN Exit [0x%x]\n", AgesaStatus);
  return  AgesaStatus;
}
