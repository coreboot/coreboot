/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe power gate
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * 
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
#include  "PcieInit.h"
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  GNB_MODULE_DEFINITIONS (GnbPcieConfig)
#include  "PciePowerGate.h"
#include  "GnbRegistersON.h"
#include  "NbSmuLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_FEATURE_PCIEPOWERGATE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FORCE_PCIE_POWERGATING_DISABLE       (1 << 2)
#define FORCE_PCIE_PHY_POWERGATING_DISABLE   (1 << 1)

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

POWER_GATE_DATA PciePowerGatingData = {
   113, 50, 50, 50, 50, 50
};


/// PCIe power gating
UINT32  PciePowerGatingTable_1[] = {
//  SMUx0B_x8408_ADDRESS
  0,
//  SMUx0B_x840C_ADDRESS
  0,
//  SMUx0B_x8410_ADDRESS
  (0x0 << SMUx0B_x8410_PwrGatingEn_OFFSET) |
  (0x1 << SMUx0B_x8410_PsoControlValidNum_OFFSET) |
  (0x3 << SMUx0B_x8410_PwrGaterSel_OFFSET)
};

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe Power Gating
 *
 *
 *
 * @param[in] StdHeader     Standard Configuration Header
 * @param[in] Flags         Force Powergating disable or Phy disable flag.
 * @param[in] PowerGateData Power Gate data
 */


VOID
STATIC
PcieSmuPowerGatingInit (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT8               Flags,
  IN      POWER_GATE_DATA     *PowerGateData
  )
{

  NbSmuRcuRegisterWrite (
    SMUx0B_x8408_ADDRESS,
    &PciePowerGatingTable_1[0],
    sizeof (PciePowerGatingTable_1) / sizeof (UINT32),
    TRUE,
    StdHeader
    );

  NbSmuRcuRegisterWrite (
    SMUx0B_x84A0_ADDRESS,
    (UINT32 *) PowerGateData,
    sizeof (POWER_GATE_DATA) / sizeof (UINT32),
    TRUE,
    StdHeader
    );
  if (Flags != 0) {
    UINT32  Value;
    ASSERT ((Flags & (~(BIT1 |  BIT2))) == 0);
    NbSmuRcuRegisterRead (SMUx0B_x8410_ADDRESS, &Value, 1, StdHeader);
    Value |= (Flags & (BIT1 | BIT2));
    NbSmuRcuRegisterWrite (SMUx0B_x8410_ADDRESS, &Value, 1, TRUE, StdHeader);
  }
  NbSmuServiceRequest (0x01, TRUE, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * PCIe PowerGate PHY lanes
 *
 *
 * @param[in] WrapperLaneBitMap     Lane bitmap on  wrapper
 * @param[in] WrapperStartlaneId    Start Line Id of the wrapper
 * @param[in] Service               Power gate service
 * @param[in] Core                  Core power gate request
 * @param[in] Tx                    Tx power gate request
 * @param[in] Rx                    Rx power gate request
 * @param[in] Pcie                  PCIe configuration data
 */

VOID
STATIC
PcieSmuPowerGateLanes (
  IN      UINT32                        WrapperLaneBitMap,
  IN      UINT16                        WrapperStartlaneId,
  IN      UINT8                         Service,
  IN      UINT8                         Core,
  IN      UINT8                         Tx,
  IN      UINT8                         Rx,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PCIe_PHY_POWER_GATE LaneSegment;
  UINT8               NumberOfLanes;
  UINT8               Index;
  LaneSegment.Tx = Tx;
  LaneSegment.Rx = Rx;
  LaneSegment.Core = Core;
  NumberOfLanes = 0;
  for (Index = 0; Index <= 32; Index++) {
    if ((WrapperLaneBitMap & 1) != 0) {
      NumberOfLanes++;
    } else {
      if (NumberOfLanes != 0) {
        LaneSegment.LowerLaneId = Index - NumberOfLanes + WrapperStartlaneId;
        LaneSegment.UpperLaneId = Index - 1 + WrapperStartlaneId;
        IDS_HDT_CONSOLE (PCIE_MISC, "  Powergate Phy Lanes %d - %d (Service = 0x%x, Core = 0x%x, Tx = 0x%x, Rx = 0x%x)\n",
          LaneSegment.LowerLaneId, LaneSegment.UpperLaneId, Service, Core, Tx, Rx
          );
        NbSmuRcuRegisterWrite (
          0x858C,
          (UINT32*) &LaneSegment,
          1,
          TRUE,
          GnbLibGetHeader (Pcie)
        );
        NbSmuServiceRequest (Service, TRUE, GnbLibGetHeader (Pcie));
        NumberOfLanes = 0;
      }
    }
    WrapperLaneBitMap >>= 1;
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Pll access required
 *
 * @param[in]       PllId           Pll ID
 * @param[in]       AccessRequired  Access required
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */
VOID
STATIC
PciePowerGatePllControl (
  IN      UINT8                         PllId,
  IN      BOOLEAN                       AccessRequired,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINT32 Value;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePllControl Enter\n");
  NbSmuRcuRegisterRead (0x859C, &Value, 1, GnbLibGetHeader (Pcie));
  Value =  (Value & 0xFFFFFF00) | PllId;
  NbSmuRcuRegisterWrite (0x859C, &Value, 1, TRUE, GnbLibGetHeader (Pcie));
  NbSmuServiceRequest (AccessRequired ? 0x18 : 0x17, TRUE, GnbLibGetHeader (Pcie));
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePllControl Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Report used lanes to SMU.
 *
 *
 * @param[in]       Wrapper         Wrapper configuration descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 * @retval          AGESA_STATUS
 */

AGESA_STATUS
STATIC
PciePowerGateReportUsedLanesCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINT32  LaneBitmap;
  LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_ACTIVE | LANE_TYPE_DDI_ACTIVE | LANE_TYPE_PCIE_HOTPLUG, 0, Wrapper, Pcie);
  if (LaneBitmap != 0) {
    PcieSmuPowerGateLanes (LaneBitmap, Wrapper->StartPhyLane, 0x14, 0x1, 0x0, 0x0, Pcie);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe PowerGate PHY lanes
 *
 *
 * @param[in]       Wrapper         Wrapper configuration descriptor
 * @param[out]      Buffer          Pointer to Boolean to report if DDI lanes present
 * @param[in]       Pcie            Pointer to global PCIe configuration
 * @retval          AGESA_STATUS
 */

AGESA_STATUS
STATIC
PciePowerGatePhyLaneCallback (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  UINT32    LaneBitmap;
  BOOLEAN   *IsDdiPresent;
  IsDdiPresent = (BOOLEAN*) Buffer;
  LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_ALL, LANE_TYPE_PCIE_ACTIVE | LANE_TYPE_DDI_ACTIVE | LANE_TYPE_PCIE_HOTPLUG, Wrapper, Pcie);
  if (LaneBitmap != 0) {
    PcieSmuPowerGateLanes (LaneBitmap,  Wrapper->StartPhyLane, 0x13, 0x1, 0x1, 0x1, Pcie);
  }
  // Powergate inactive hotplug lanes
  LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_PCIE_HOTPLUG, LANE_TYPE_PCIE_ACTIVE, Wrapper, Pcie);
  if (LaneBitmap != 0) {
    PcieSmuPowerGateLanes (LaneBitmap,  Wrapper->StartPhyLane, 0x13, 0x0, 0x1, 0x1, Pcie);
  }
  // Powergate DDI lanes
  LaneBitmap = PcieUtilGetWrapperLaneBitMap (LANE_TYPE_DDI_ACTIVE, 0, Wrapper, Pcie);
  if (LaneBitmap != 0) {
    *IsDdiPresent = TRUE;
    PcieSmuPowerGateLanes (LaneBitmap,  Wrapper->StartPhyLane, 0x13, 0x0, 0x0, 0x1, Pcie);
  }
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PCIe PowerGate PHY lanes
 *
 *
 *
 * @param[in] StdHeader     Standard Configuration Header
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
STATIC
PciePowerGatePhyLane (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AGESA_STATUS          Status;
  AGESA_STATUS          AgesaStatus;
  BOOLEAN               IsDdiPresent;
  PCIe_PLATFORM_CONFIG  *Pcie;
  AgesaStatus = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePhyLane Enter\n");
  Status = PcieLocateConfigurationData (StdHeader, &Pcie);
  ASSERT (Status == AGESA_SUCCESS);
  AGESA_STATUS_UPDATE (Status, AgesaStatus);
  if (Status == AGESA_SUCCESS) {
    PciePortsVisibilityControl (UnhidePorts, Pcie);
    IsDdiPresent = FALSE;
    Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PciePowerGateReportUsedLanesCallback, NULL, Pcie );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    PciePowerGatePllControl (0x1, TRUE, Pcie);
    Status = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PciePowerGatePhyLaneCallback, &IsDdiPresent, Pcie );
    AGESA_STATUS_UPDATE (Status, AgesaStatus);
    if (!IsDdiPresent) {
      PciePowerGatePllControl (0x1, FALSE, Pcie);
    }
    PciePortsVisibilityControl (HidePorts, Pcie);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGatePhyLane Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Power PCIe block
 *
 *
 *
 * @param[in] StdHeader        Pointer to Standard configuration
 * @retval    AGESA_STATUS
 */

AGESA_STATUS
PciePowerGateFeature (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PCIE_POWERGATE_CONFIG PciePowerGate;
  AGESA_STATUS          Status;
  UINT8                 Flags;
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateFeature Enter\n");
  Status = AGESA_SUCCESS;
  PciePowerGate.Services.PciePowerGate = 0x1;
  PciePowerGate.Services.PciePhyLanePowerGate = 0x1;
  LibAmdMemCopy (&PciePowerGate.Pcie, &PciePowerGatingData, sizeof (POWER_GATE_DATA), StdHeader);
  IDS_OPTION_CALLOUT (IDS_CALLOUT_GNB_PCIE_POWERGATE_CONFIG, &PciePowerGate, StdHeader);
  Flags = 0;
  if (PciePowerGate.Services.PciePowerGate == 0x0) {
    IDS_HDT_CONSOLE (PCIE_MISC, " Pcie Power Gating - Disabled\n");
    Flags |= FORCE_PCIE_POWERGATING_DISABLE;
  }
  if (PciePowerGate.Services.PciePhyLanePowerGate == 0x0) {
    IDS_HDT_CONSOLE (PCIE_MISC, " Pcie Phy Power Gating - Disabled\n");
    Flags |= FORCE_PCIE_PHY_POWERGATING_DISABLE;
  }
  PcieSmuPowerGatingInit (StdHeader, Flags, &PciePowerGate.Pcie);
  Status = PciePowerGatePhyLane (StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "PciePowerGateFeature Exit [0x%x]\n", Status);
  return Status;
}
