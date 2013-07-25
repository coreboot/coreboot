/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
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
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "PcieMapTopology.h"
#include  "PcieInputParser.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBPCIECONFIG_PCIECONFIGLIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * get Master Lane of PCIe port engine
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Master Engine Lane Number
 */
UINT8
PcieConfigGetPcieEngineMasterLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT8                 MasterLane;
  PCIe_WRAPPER_CONFIG   *Wrapper;
  ASSERT (PcieConfigIsPcieEngine (Engine));

  Wrapper = PcieConfigGetParentWrapper (Engine);
  if (Engine->EngineData.StartLane <= Engine->EngineData.EndLane) {
    MasterLane = (UINT8) (Engine->EngineData.StartLane - Wrapper->StartPhyLane);
  } else {
    MasterLane = (UINT8) (Engine->EngineData.EndLane - Wrapper->StartPhyLane);
  }
  return MasterLane;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get number of core lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Number of core lane
 */
UINT8
PcieConfigGetNumberOfCoreLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (Engine->Type.Port.StartCoreLane >= UNUSED_LANE_ID || Engine->Type.Port.EndCoreLane >= UNUSED_LANE_ID) {
    return 0;
  }
  return (UINT8) (Engine->Type.Port.EndCoreLane - Engine->Type.Port.StartCoreLane + 1);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Disable engine
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
VOID
PcieConfigDisableEngine (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (PcieConfigIsSbPcieEngine (Engine)) {
    return;
  }
  PcieConfigResetDescriptorFlags (Engine, DESCRIPTOR_ALLOCATED);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Disable all engines on wrapper
 *
 *
 *
 * @param[in]  EngineTypeMask Engine type bitmap.
 * @param[in]  Wrapper        Pointer to wrapper config descriptor
 */
VOID
PcieConfigDisableAllEngines (
  IN      UINTN                          EngineTypeMask,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG        *EngineList;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if ((EngineList->EngineData.EngineType & EngineTypeMask) != 0) {
      PcieConfigDisableEngine (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get engine PHY lanes bitmap
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
UINT32
PcieConfigGetEnginePhyLaneBitMap (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT32  LaneBitMap;
  LaneBitMap = 0;
  if (PcieLibIsEngineAllocated (Engine)) {
    LaneBitMap = ((1 << PcieConfigGetNumberOfPhyLane (Engine)) - 1) <<  (PcieLibGetLoPhyLane (Engine) - PcieConfigGetParentWrapper (Engine)->StartPhyLane);
  }
  return LaneBitMap;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get number of phy lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine config descriptor
 * @retval                 Number of Phy lane
 */
UINT8
PcieConfigGetNumberOfPhyLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (Engine->EngineData.StartLane >= UNUSED_LANE_ID || Engine->EngineData.StartLane >= UNUSED_LANE_ID) {
    return 0;
  }
  if (Engine->EngineData.StartLane > Engine->EngineData.EndLane) {
    return (UINT8) (Engine->EngineData.StartLane - Engine->EngineData.EndLane + 1);
  } else {
    return (UINT8) (Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get port configuration signature for given wrapper and core
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @param[in]  CoreId      Core ID
 * @retval                 Configuration Signature
 */
UINT64
PcieConfigGetConfigurationSignature (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper,
  IN      UINT8                          CoreId
  )
{
  UINT64              ConfigurationSignature;
  PCIe_ENGINE_CONFIG  *EngineList;
  ConfigurationSignature = 0;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsPcieEngine (EngineList) && EngineList->Type.Port.CoreId == CoreId) {
      ConfigurationSignature = (ConfigurationSignature << 8) | PcieConfigGetNumberOfCoreLane (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  return ConfigurationSignature;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check Port Status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  PortStatus      Check if status asserted for port
 * @retval                     TRUE if status asserted
 */
BOOLEAN
PcieConfigCheckPortStatus (
  IN       PCIe_ENGINE_CONFIG            *Engine,
  IN       UINT32                        PortStatus
  )
{
  return (Engine->InitStatus & PortStatus) == 0 ? FALSE : TRUE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set/Reset port status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  SetStatus       SetStatus
 * @param[in]  ResetStatus     ResetStatus
 *
 */
UINT16
PcieConfigUpdatePortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       PCIe_ENGINE_INIT_STATUS          SetStatus,
  IN       PCIe_ENGINE_INIT_STATUS          ResetStatus
  )
{
  Engine->InitStatus |= SetStatus;
  Engine->InitStatus &= (~ResetStatus);
  return Engine->InitStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all descriptor of specific type
 *
 *
 * @param[in]       InDescriptorFlags    Include descriptor flags
 * @param[in]       OutDescriptorFlags   Exlude descriptor flags
 * @param[in]       TerminationFlags     Termination flags
 * @param[in]       Callback             Pointer to callback function
 * @param[in, out]  Buffer               Pointer to buffer to pass information to callback
 * @param[in]       Pcie                 Pointer to global PCIe configuration
 */

AGESA_STATUS
PcieConfigRunProcForAllDescriptors (
  IN       UINT32                           InDescriptorFlags,
  IN       UINT32                           OutDescriptorFlags,
  IN       UINT32                           TerminationFlags,
  IN       PCIe_RUN_ON_DESCRIPTOR_CALLBACK  Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  )
{
  AGESA_STATUS            AgesaStatus;
  AGESA_STATUS            Status;
  PCIe_DESCRIPTOR_HEADER  *Descriptor;

  AgesaStatus = AGESA_SUCCESS;
  Descriptor = PcieConfigGetChild (InDescriptorFlags & DESCRIPTOR_ALL_TYPES, &Pcie->Header);
  while (Descriptor != NULL) {
    if ((InDescriptorFlags & Descriptor->DescriptorFlags) != 0 && (OutDescriptorFlags && Descriptor->DescriptorFlags) == 0) {
      Status = Callback (Descriptor, Buffer, Pcie);
      AGESA_STATUS_UPDATE (Status, AgesaStatus);
    }
    Descriptor = (PCIe_DESCRIPTOR_HEADER *) PcieConfigGetNextTopologyDescriptor (Descriptor, TerminationFlags);
  }
  return AgesaStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all wrappers in topology
 *
 *
 * @param[in]       DescriptorFlags   Wrapper Flags
 * @param[in]       Callback          Pointer to callback function
 * @param[in, out]  Buffer            Pointer to buffer to pass information to callback
 * @param[in]       Pcie              Pointer to global PCIe configuration
 */

AGESA_STATUS
PcieConfigRunProcForAllWrappers (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  AGESA_STATUS          AgesaStatus;
  AGESA_STATUS          Status;
  PCIe_WRAPPER_CONFIG   *Wrapper;

  AgesaStatus = AGESA_SUCCESS;
  Wrapper = (PCIe_WRAPPER_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_WRAPPERS, &Pcie->Header);
  while (Wrapper != NULL) {
    if (!(PcieLibIsVirtualDesciptor (Wrapper) && (DescriptorFlags & DESCRIPTOR_VIRTUAL) == 0)) {
      if ((DescriptorFlags & DESCRIPTOR_ALL_WRAPPERS & Wrapper->Header.DescriptorFlags) != 0) {
        Status = Callback (Wrapper, Buffer, Pcie);
        AGESA_STATUS_UPDATE (Status, AgesaStatus);
      }
    }
    Wrapper = (PCIe_WRAPPER_CONFIG *) PcieConfigGetNextTopologyDescriptor (Wrapper, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
  return AgesaStatus;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Execute callback on all engine in topology
 *
 *
 * @param[in]       DescriptorFlags Engine flags.
 * @param[in]       Callback        Pointer to callback function
 * @param[in, out]  Buffer          Pointer to buffer to pass information to callback
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */

VOID
PcieConfigRunProcForAllEngines (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK   Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{

  PCIe_ENGINE_CONFIG  *Engine;
  Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetChild (DESCRIPTOR_ALL_ENGINES, &Pcie->Header);
  while (Engine != NULL) {
    if (!(PcieLibIsVirtualDesciptor (Engine) && (DescriptorFlags & DESCRIPTOR_VIRTUAL) == 0)) {
      if (!((DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0 && !PcieLibIsEngineAllocated (Engine))) {
        if ((Engine->Header.DescriptorFlags & DESCRIPTOR_ALL_ENGINES & DescriptorFlags) != 0) {
          Callback (Engine, Buffer, Pcie);
        }
      }
    }
    Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get parent descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetParent (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Parent != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor - Descriptor->Parent);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get child descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetChild (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Child != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor + Descriptor->Child);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get peer descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetPeer (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  ASSERT (Descriptor != NULL);
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Peer != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *) ((UINT8 *) Descriptor + Descriptor->Peer);
    } else {
      return NULL;
    }
  }
  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check is engine is active or potentially active
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 TRUE  - engine active
 * @retval                 FALSE - engine not active
 */
BOOLEAN
PcieConfigIsActivePcieEngine (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  BOOLEAN Result;
  ASSERT (PcieConfigIsPcieEngine (Engine));
  Result = FALSE;
  if (PcieConfigIsEngineAllocated (Engine)) {
    if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
      (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled && Engine->Type.Port.PortData.LinkHotplug != HotplugInboard)) {
      Result = TRUE;
    }
  }
  return Result;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Locate SB engine on wrapper
 *
 *
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @retval                 SB engine pointer or NULL
 */
PCIe_ENGINE_CONFIG *
PcieConfigLocateSbEngine (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsSbPcieEngine (EngineList)) {
      return EngineList;
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
  return NULL;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump engine configuration
 *
 *
 * @param[in]  EngineList           Engine Configuration
 */
VOID
PcieConfigEngineDebugDump (
  IN      PCIe_ENGINE_CONFIG        *EngineList
  )
{
  IDS_HDT_CONSOLE (PCIE_MISC, "        Descriptor Flags - 0x%08x\n", EngineList->Header.DescriptorFlags);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Engine Type - %s\n        Start Phy Lane - %d\n        End   Phy Lane - %d\n",
    ((EngineList->EngineData.EngineType == PciePortEngine) ? "PCIe Port" : "DDI Link"),
    EngineList->EngineData.StartLane,
    EngineList->EngineData.EndLane
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "        Scrath         - %d\n", EngineList->Scratch);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Init Status    - 0x%08x\n", EngineList->InitStatus);
  if (PcieLibIsPcieEngine (EngineList)) {
    IDS_HDT_CONSOLE (PCIE_MISC, "        PCIe port configuration:\n");
    IDS_HDT_CONSOLE (PCIE_MISC, "          Port Training - %s\n",
      (EngineList->Type.Port.PortData.PortPresent == PortDisabled) ? "Disable" : "Enabled"
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          Start Core Lane - %d\n", EngineList->Type.Port.StartCoreLane);
    IDS_HDT_CONSOLE (PCIE_MISC, "          End Core Lane   - %d\n", EngineList->Type.Port.EndCoreLane);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Requested PCI Dev Number   - %d\n",EngineList->Type.Port.PortData.DeviceNumber);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Requested PCI Func Number  - %d\n",EngineList->Type.Port.PortData.FunctionNumber);
    IDS_HDT_CONSOLE (PCIE_MISC, "          PCI Address - %d:%d:%d\n",
      EngineList->Type.Port.Address.Address.Bus,
      EngineList->Type.Port.Address.Address.Device,
      EngineList->Type.Port.Address.Address.Function
      );
    IDS_HDT_CONSOLE (PCIE_MISC, "          Misc Control  - 0x%02x\n", EngineList->Type.Port.PortData.MiscControls);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Native PCI Dev Number  - %d\n", EngineList->Type.Port.NativeDevNumber);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Native PCI Func Number - %d\n", EngineList->Type.Port.NativeFunNumber);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Hotplug - %s\n",
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugDisabled) ? "Disabled" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugBasic) ? "Basic" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugServer) ? "Server" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugEnhanced) ? "Enhanced" : (
      (EngineList->Type.Port.PortData.LinkHotplug == HotplugInboard) ? "Inboard" : "Unknown"))))
      );
    ASSERT (EngineList->Type.Port.PortData.LinkHotplug < MaxHotplug);
    IDS_HDT_CONSOLE (PCIE_MISC, "          ASPM    - %s\n",
      (EngineList->Type.Port.PortData.LinkAspm == AspmDisabled) ? "Disabled" : (
      (EngineList->Type.Port.PortData.LinkAspm == AspmL0s) ? "L0s" : (
      (EngineList->Type.Port.PortData.LinkAspm == AspmL1) ? "L1" :  (
      (EngineList->Type.Port.PortData.LinkAspm == AspmL0sL1) ? "L0s & L1" : "Unknown")))
      );
    ASSERT (EngineList->Type.Port.PortData.LinkAspm < MaxAspm);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Speed   - %d\n",
      EngineList->Type.Port.PortData.LinkSpeedCapability
      );
  } else {
    IDS_HDT_CONSOLE (PCIE_MISC, "        DDI configuration:\n");
    IDS_HDT_CONSOLE (PCIE_MISC, "          Connector - %s\n",
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDP) ? "DP" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDP) ? "eDP" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeSingleLinkDVI) ? "Single Link DVI" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDualLinkDVI) ? "Dual Link DVI" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeHDMI) ? "HDMI" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDpToVga) ? "DP-to-VGA" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeDpToLvds) ? "DP-to-LVDS" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeLvds) ? "LVDS" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeSingleLinkDviI) ? "Single Link DVI-I" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeCrt) ? "CRT" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToLvds) ? "eDP To Lvds - No SW Init Required" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeEDPToLvdsSwInit) ? "Third party eDP To Lvds - SW Init Required" : (
      (EngineList->Type.Ddi.DdiData.ConnectorType == ConnectorTypeAutoDetect) ? "Autodetect" : "Unknown"))))))))))))
      );
    ASSERT (EngineList->Type.Ddi.DdiData.ConnectorType < MaxConnectorType);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Aux - Aux%d\n", EngineList->Type.Ddi.DdiData.AuxIndex + 1);
    ASSERT (EngineList->Type.Ddi.DdiData.AuxIndex < MaxAux);
    IDS_HDT_CONSOLE (PCIE_MISC, "          Hdp - Hdp%d\n", EngineList->Type.Ddi.DdiData.HdpIndex + 1);
    ASSERT (EngineList->Type.Ddi.DdiData.HdpIndex < MaxHdp);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump wrapper configuration
 *
 *
 * @param[in]  WrapperList           Wrapper Configuration
 */
VOID
PcieConfigWrapperDebugDump (
  IN      PCIe_WRAPPER_CONFIG        *WrapperList
  )
{
  PCIe_ENGINE_CONFIG    *EngineList;
  IDS_HDT_CONSOLE (PCIE_MISC, "      <---------Wrapper - %s Config -------->\n",
    PcieFmDebugGetWrapperNameString (WrapperList)
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "        Start PHY lane - %02d\n", WrapperList->StartPhyLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        End   PHY lane - %02d\n", WrapperList->EndPhyLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Descriptor Flags - 0x%08x\n", WrapperList->Header.DescriptorFlags);
  IDS_HDT_CONSOLE (PCIE_MISC, "        PowerOffUnusedLanes - %x\n        PowerOffUnusedPlls - %x\n        ClkGating - %x\n"
                              "        LclkGating - %x\n        TxclkGatingPllPowerDown - %x\n        PllOffInL1 - %x\n",
    WrapperList->Features.PowerOffUnusedLanes,
    WrapperList->Features.PowerOffUnusedPlls,
    WrapperList->Features.ClkGating,
    WrapperList->Features.LclkGating,
    WrapperList->Features.TxclkGatingPllPowerDown,
    WrapperList->Features.PllOffInL1
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "      <---------Wrapper - %s Config End----->\n",
    PcieFmDebugGetWrapperNameString (WrapperList)
    );
  EngineList = PcieConfigGetChildEngine (WrapperList);
  while (EngineList != NULL) {
    if (PcieLibIsEngineAllocated (EngineList)) {
      PcieConfigEngineDebugDump (EngineList);
    }
    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump configuration to debug out
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  PCIe_SILICON_CONFIG   *SiliconList;
  PCIe_WRAPPER_CONFIG   *WrapperList;
  PCIe_COMPLEX_CONFIG   *ComplexList;
  ComplexList = (PCIe_COMPLEX_CONFIG *) PcieConfigGetChild (DESCRIPTOR_COMPLEX, &Pcie->Header);
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config Start------------>\n");
  IDS_HDT_CONSOLE (PCIE_MISC, "  PSPP Policy - %s\n",
   (Pcie->PsppPolicy == PsppPowerSaving) ? "Power Saving" :
   (Pcie->PsppPolicy == PsppBalanceHigh) ? "Balance-High" : (
   (Pcie->PsppPolicy == PsppBalanceLow) ? "Balance-Low" : (
   (Pcie->PsppPolicy == PsppPerformance) ? "Performance" : (
   (Pcie->PsppPolicy == PsppDisabled) ? "Disabled" : "Unknown")))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  GFX Workaround - %s\n",
   (Pcie->GfxCardWorkaround == 0) ? "Disabled" : "Enabled"
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  LinkL0Pooling - %dus\n",
   Pcie->LinkL0Pooling
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  LinkGpioResetAssertionTime - %dus\n",
   Pcie->LinkGpioResetAssertionTime
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  LinkReceiverDetectionPooling - %dus\n",
   Pcie->LinkReceiverDetectionPooling
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "  Training Algorythm - %s\n",
    (Pcie->TrainingAlgorithm == PcieTrainingStandard) ? "PcieTrainingStandard" : (
    (Pcie->TrainingAlgorithm == PcieTrainingDistributed) ? "PcieTrainingDistributed" : "Unknown")
    );
  while (ComplexList != NULL) {
    IDS_HDT_CONSOLE (PCIE_MISC, "  <---------- Complex Config Start ---------->\n");
    IDS_HDT_CONSOLE (PCIE_MISC, "    Descriptor Flags - 0x%08x\n", ComplexList->Header.DescriptorFlags);
    IDS_HDT_CONSOLE (PCIE_MISC, "    Socket ID - %d\n", ComplexList->SocketId);
    SiliconList = PcieConfigGetChildSilicon (ComplexList);
    while (SiliconList != NULL) {
      IDS_HDT_CONSOLE (PCIE_MISC, "    <---------- Silicon Config Start -------->\n");
      IDS_HDT_CONSOLE (PCIE_MISC, "      Descriptor Flags - 0x%08x\n", SiliconList->Header.DescriptorFlags);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Silicon ID - %d\n", SiliconList->SiliconId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Node ID - %d\n", SiliconList->NodeId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Host PCI Address - %d:%d:%d\n",
        SiliconList->Address.Address.Bus,
        SiliconList->Address.Address.Device,
        SiliconList->Address.Address.Function
        );
      WrapperList = PcieConfigGetChildWrapper (SiliconList);
      while (WrapperList != NULL) {
        PcieConfigWrapperDebugDump (WrapperList);
        WrapperList = PcieLibGetNextDescriptor (WrapperList);
      }
      IDS_HDT_CONSOLE (PCIE_MISC, "    <---------- Silicon Config End ---------->\n");
      SiliconList = PcieLibGetNextDescriptor (SiliconList);
    }
    IDS_HDT_CONSOLE (PCIE_MISC, "  <---------- Complex Config End ------------>\n");
    ComplexList = PcieLibGetNextDescriptor (ComplexList);
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config End-------------->\n");
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
PcieUserDescriptorConfigDump (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor
  )
{

  IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %s\n",
    (EngineDescriptor->EngineData.EngineType == PciePortEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) ? "DDI Link" : (
    (EngineDescriptor->EngineData.EngineType == PcieUnusedEngine) ? "Unused" : "Invalid"))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->EngineData.StartLane,
    EngineDescriptor->EngineData.EndLane
  );
  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    PortPresent - %d\n    ChannelType  - %d\n    DeviceNumber - %d\n    FunctionNumber - %d\n    LinkSpeedCapability - %d\n    LinkAspm - %d\n    LinkHotplug - %d\n    ResetId - %d\n    SB link - %d\n    MiscControls - 0x%02x\n" ,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.PortPresent,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.ChannelType,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.DeviceNumber,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.FunctionNumber,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkSpeedCapability,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkAspm,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkHotplug,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.ResetId,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls.SbLink,
      ((PCIe_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls
    );
  }
  if (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) {
    IDS_HDT_CONSOLE (PCIE_MISC, "    ConnectorType - %d\n    AuxIndex  - %d\n    HdpIndex - %d\n" ,
      ((PCIe_DDI_DESCRIPTOR *) EngineDescriptor)->Ddi.ConnectorType,
      ((PCIe_DDI_DESCRIPTOR *) EngineDescriptor)->Ddi.AuxIndex,
      ((PCIe_DDI_DESCRIPTOR *) EngineDescriptor)->Ddi.HdpIndex
    );
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to debug out
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
PcieUserConfigConfigDump (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor;
  PCIe_COMPLEX_DESCRIPTOR     *CurrentComplexDescriptor;
  UINTN                       ComplexIndex;
  UINTN                       Index;
  UINTN                       NumberOfEngines;
  UINTN                       NumberOfComplexes;

  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config Start------------->\n");

  NumberOfComplexes = PcieInputParserGetNumberOfComplexes (ComplexDescriptor);
  for (ComplexIndex = 0; ComplexIndex < NumberOfComplexes; ++ComplexIndex) {
    CurrentComplexDescriptor = PcieInputParserGetComplexDescriptor (ComplexDescriptor, ComplexIndex);
    NumberOfEngines = PcieInputParserGetNumberOfEngines (CurrentComplexDescriptor);
    IDS_HDT_CONSOLE (PCIE_MISC, "  ComplexDescriptor SocketId - %d\n  NumberOfEngines - %d\n",
        ComplexDescriptor->SocketId,
        NumberOfEngines
      );

    for (Index = 0; Index < NumberOfEngines; Index++) {
      EngineDescriptor = PcieInputParserGetEngineDescriptor (ComplexDescriptor, Index);
      PcieUserDescriptorConfigDump (EngineDescriptor);
    }
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config End-------------->\n");
}

