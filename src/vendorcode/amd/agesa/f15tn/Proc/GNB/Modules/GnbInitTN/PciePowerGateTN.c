/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe power gate initialization
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
#include  "amdlib.h"
#include  "Ids.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "OptionGnb.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieFamServices.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbNbInitLibV4.h"
#include  "GnbRegistersTN.h"
#include  "GnbRegisterAccTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIEPOWERGATETN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

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
PciePowerGateTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Report used lanes
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
PciePowerGateReportActiveLanesCallbackTN (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xBC_x1F39C_STRUCT   D0F0xBC_x1F39C;
  UINT32                  LaneBitmap;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateReportActiveLanesCallbackTN Enter\n");
  LaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE | LANE_TYPE_DDI_PHY_NATIVE_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG, 0, Engine);
  if (LaneBitmap != 0) {
    D0F0xBC_x1F39C.Value = 0;
    D0F0xBC_x1F39C.Field.Tx = 0;
    D0F0xBC_x1F39C.Field.Rx = 0;
    D0F0xBC_x1F39C.Field.Core = 0;
    D0F0xBC_x1F39C.Field.SkipPhy = 1;
    D0F0xBC_x1F39C.Field.SkipCore = 1;
    D0F0xBC_x1F39C.Field.UpperLaneID = LibAmdBitScanReverse (LaneBitmap) + PcieConfigGetParentWrapper (Engine)->StartPhyLane;
    D0F0xBC_x1F39C.Field.LowerLaneID = LibAmdBitScanForward (LaneBitmap) + PcieConfigGetParentWrapper (Engine)->StartPhyLane;
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "  LowerLaneID - %02d UpperLaneID - %02d Tx - %d  Rx - %d Core - %d Exit\n",
      D0F0xBC_x1F39C.Field.LowerLaneID,
      D0F0xBC_x1F39C.Field.UpperLaneID,
      D0F0xBC_x1F39C.Field.Tx,
      D0F0xBC_x1F39C.Field.Rx,
      D0F0xBC_x1F39C.Field.Core
      );
    if (PcieConfigIsPcieEngine (Engine) && PcieFmGetLinkSpeedCap (PCIE_PORT_GEN_CAP_BOOT, Engine) == PcieGen2) {
      D0F0xBC_x1F610_STRUCT  D0F0xBC_x1F610;
      UINT32                 Gen2LaneBitmap;
      Gen2LaneBitmap = ((1 << (D0F0xBC_x1F39C.Field.UpperLaneID - D0F0xBC_x1F39C.Field.LowerLaneID + 1)) - 1) << D0F0xBC_x1F39C.Field.LowerLaneID;
      GnbRegisterReadTN (D0F0xBC_x1F610_TYPE, D0F0xBC_x1F610_ADDRESS, &D0F0xBC_x1F610.Value, 0, GnbLibGetHeader (Pcie));
      D0F0xBC_x1F610.Field.GFXH |= (Gen2LaneBitmap >> 16) & 0xFF;
      D0F0xBC_x1F610.Field.GFXL |= (Gen2LaneBitmap >> 8) & 0xFF;
      D0F0xBC_x1F610.Field.GPPSB |= (Gen2LaneBitmap  & 0xFF );
      GnbRegisterWriteTN (D0F0xBC_x1F610_TYPE,  D0F0xBC_x1F610_ADDRESS, &D0F0xBC_x1F610.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
    }
    GnbRegisterWriteTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
    GnbSmuServiceRequestV4 (
      PcieConfigGetParentSilicon (Engine)->Address,
      SMC_MSG_PHY_LN_ON,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateReportActiveLanesCallbackTN Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power down unused lanes
 *
 *
 *
 *
 * @param[in]       Wrapper         Pointer to wrapper configuration
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 * @retval          AGESA_SUCCESS
 *
 */

AGESA_STATUS
STATIC
PciePowerGatePowerDownUnusedLanesCallbackTN (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   Index;
  UINTN                   State;
  UINT32                  LaneBitmap;
  UINT16                  StartLane;
  UINT16                  EndLane;
  D0F0xBC_x1F39C_STRUCT   D0F0xBC_x1F39C;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePowerDownUnusedLanesCallbackTN Enter\n");

  LaneBitmap = PcieUtilGetWrapperLaneBitMap (
                 LANE_TYPE_PHY_NATIVE_ALL,
                 LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE | LANE_TYPE_DDI_PHY_NATIVE_ACTIVE | LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG,
                 Wrapper
                 );
  IDS_HDT_CONSOLE (GNB_TRACE, " Lane Bitmap 0x%x\n", LaneBitmap);
  if (LaneBitmap != 0) {
    State = 0;
    StartLane = 0;
    EndLane = 0;
    for (Index = 0; Index <= (LibAmdBitScanReverse (LaneBitmap) + 1); Index++) {
      if ((State == 0) && ((LaneBitmap & (1 << Index)) != 0)) {
        StartLane = Index;
        State = 1;
      } else if ((State == 1) && ((LaneBitmap & (1 << Index)) == 0)) {
        EndLane = Index - 1;
        State = 0;
        GnbRegisterReadTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, 0, GnbLibGetHeader (Pcie));
        D0F0xBC_x1F39C.Field.Tx = 1;
        D0F0xBC_x1F39C.Field.Rx = 1;
        D0F0xBC_x1F39C.Field.Core = 1;
        D0F0xBC_x1F39C.Field.LowerLaneID = StartLane + Wrapper->StartPhyLane;
        D0F0xBC_x1F39C.Field.UpperLaneID = EndLane + Wrapper->StartPhyLane;
        IDS_HDT_CONSOLE (
          PCIE_MISC,
          "  LowerLaneID - %02d UpperLaneID - %02d Tx - %d  Rx - %d Core - %d Exit\n",
          D0F0xBC_x1F39C.Field.LowerLaneID,
          D0F0xBC_x1F39C.Field.UpperLaneID,
          D0F0xBC_x1F39C.Field.Tx,
          D0F0xBC_x1F39C.Field.Rx,
          D0F0xBC_x1F39C.Field.Core
          );
        GnbRegisterWriteTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
        GnbSmuServiceRequestV4 (
          PcieConfigGetParentSilicon (Wrapper)->Address,
          SMC_MSG_PHY_LN_OFF,
          GNB_REG_ACC_FLAG_S3SAVE,
          GnbLibGetHeader (Pcie)
          );
      }
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePowerDownUnusedLanesCallbackTN Exit\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power down unused lanes
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
PciePowerGatePowerDownLanesCallbackTN (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  D0F0xBC_x1F39C_STRUCT   D0F0xBC_x1F39C;
  UINT32                  LaneBitmap;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePowerDownLanesCallbackTN Enter\n");
  LaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_PCIE_PHY_NATIVE_HOTPLUG, LANE_TYPE_PCIE_PHY_NATIVE_ALLOC_ACTIVE, Engine);
  if (LaneBitmap != 0) {
    GnbRegisterReadTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, 0, GnbLibGetHeader (Pcie));
    D0F0xBC_x1F39C.Field.Tx = 1;
    D0F0xBC_x1F39C.Field.Rx = 1;
    D0F0xBC_x1F39C.Field.Core = 0;
    D0F0xBC_x1F39C.Field.UpperLaneID = LibAmdBitScanReverse (LaneBitmap) + PcieConfigGetParentWrapper (Engine)->StartPhyLane;
    D0F0xBC_x1F39C.Field.LowerLaneID = LibAmdBitScanForward (LaneBitmap) + PcieConfigGetParentWrapper (Engine)->StartPhyLane;
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "  PCIe Lanes LowerLaneID - %02d UpperLaneID - %02d Tx - %d  Rx - %d Core - %d Exit\n",
      D0F0xBC_x1F39C.Field.LowerLaneID,
      D0F0xBC_x1F39C.Field.UpperLaneID,
      D0F0xBC_x1F39C.Field.Tx,
      D0F0xBC_x1F39C.Field.Rx,
      D0F0xBC_x1F39C.Field.Core
      );
    GnbRegisterWriteTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
    GnbSmuServiceRequestV4 (
      PcieConfigGetParentSilicon (Engine)->Address,
      SMC_MSG_PHY_LN_OFF,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
  }
  LaneBitmap = PcieUtilGetEngineLaneBitMap (LANE_TYPE_DDI_PHY_NATIVE_ACTIVE, 0, Engine);
  if (LaneBitmap != 0) {
    GnbRegisterReadTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, 0, GnbLibGetHeader (Pcie));
    D0F0xBC_x1F39C.Field.Tx = 1;
    D0F0xBC_x1F39C.Field.Rx = 1;
    D0F0xBC_x1F39C.Field.Core = 1;
    D0F0xBC_x1F39C.Field.UpperLaneID = LibAmdBitScanReverse (LaneBitmap) + PcieConfigGetParentWrapper (Engine)->StartPhyLane;
    D0F0xBC_x1F39C.Field.LowerLaneID = LibAmdBitScanForward (LaneBitmap) + PcieConfigGetParentWrapper (Engine)->StartPhyLane;
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "  DDI Lanes LowerLaneID - %02d UpperLaneID - %02d Tx - %d  Rx - %d Core - %d Exit\n",
      D0F0xBC_x1F39C.Field.LowerLaneID,
      D0F0xBC_x1F39C.Field.UpperLaneID,
      D0F0xBC_x1F39C.Field.Tx,
      D0F0xBC_x1F39C.Field.Rx,
      D0F0xBC_x1F39C.Field.Core
      );
    GnbRegisterWriteTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
    GnbSmuServiceRequestV4 (
      PcieConfigGetParentSilicon (Engine)->Address,
      SMC_MSG_PHY_LN_OFF,
      GNB_REG_ACC_FLAG_S3SAVE,
      GnbLibGetHeader (Pcie)
      );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePowerDownLanesCallbackTN Exit\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Pcie Power gate init
 *
 *   Late PCIe initialization
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 * @retval     AGESA_SUCCESS       Topology successfully mapped
 */

AGESA_STATUS
PciePowerGateTN (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT8                   PowerGatingFlags;
  D0F0xBC_x1F39C_STRUCT   D0F0xBC_x1F39C;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateTN Enter\n");
  PowerGatingFlags = GnbBuildOptions.CfgPciePowerGatingFlags;
  // Report used lanes
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_DDI_ENGINE,
    PciePowerGateReportActiveLanesCallbackTN,
    NULL,
    Pcie
    );

  IDS_OPTION_HOOK (IDS_GNB_PCIE_POWER_GATING, &PowerGatingFlags, GnbLibGetHeader (Pcie));

  // Update flags
  GnbRegisterReadTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, 0, GnbLibGetHeader (Pcie));
  if ((PowerGatingFlags & PCIE_POWERGATING_SKIP_CORE) == 0) {
    D0F0xBC_x1F39C.Field.SkipCore = 0;
  }
  if ((PowerGatingFlags & PCIE_POWERGATING_SKIP_PHY) == 0) {
    D0F0xBC_x1F39C.Field.SkipPhy = 0;
  }
  GnbRegisterWriteTN (D0F0xBC_x1F39C_TYPE,  D0F0xBC_x1F39C_ADDRESS, &D0F0xBC_x1F39C.Value, GNB_REG_ACC_FLAG_S3SAVE, GnbLibGetHeader (Pcie));
  // Power down unused lanes
  PcieConfigRunProcForAllWrappers (
    DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_DDI_WRAPPER,
    PciePowerGatePowerDownUnusedLanesCallbackTN,
    NULL,
    Pcie
    );
  //Power down hotplug lanes
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_DDI_ENGINE,
    PciePowerGatePowerDownLanesCallbackTN,
    NULL,
    Pcie
    );
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateTN Exit\n");
  return  AGESA_SUCCESS;
}
