/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe wrapper configuration services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44325 $   @e \$Date: 2010-12-22 03:29:53 -0700 (Wed, 22 Dec 2010) $
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
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "PcieFamilyServices.h"
#include  GNB_MODULE_DEFINITIONS (GnbCommonLib)
#include  GNB_MODULE_DEFINITIONS (GnbPcieInitLibV1)
#include  "PcieMiscLib.h"
#include  "GnbPcieFamServices.h"
#include  "OntarioDefinitions.h"
#include  "GnbRegistersON.h"
#include  "NbSmuLib.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_PCIE_FAMILY_0X14_F14PCIEWRAPPERSERVICES_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
STATIC
PcieOnConfigureGppEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
STATIC
PcieOnConfigureDdiEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  );

VOID
PcieFmExecuteNativeGen1Reconfig (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

AGESA_STATUS
PcieOnGetGppConfigurationValue (
  IN       UINT64    ConfigurationSignature,
     OUT   UINT8     *ConfigurationValue
  );

/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */
CONST PCIE_HOST_REGISTER_ENTRY PcieInitTable [] = {
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6440_ADDRESS),
    D0F0xE4_PHY_6440_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6440_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6480_ADDRESS),
    D0F0xE4_PHY_6480_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6480_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6500_ADDRESS),
    D0F0xE4_PHY_6500_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6500_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6600_ADDRESS),
    D0F0xE4_PHY_6600_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6600_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6840_ADDRESS),
    D0F0xE4_PHY_6840_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6840_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6880_ADDRESS),
    D0F0xE4_PHY_6880_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6880_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6900_ADDRESS),
    D0F0xE4_PHY_6900_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6900_RxInCalForce_OFFSET
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_6A00_ADDRESS),
    D0F0xE4_PHY_6A00_RxInCalForce_MASK,
    0x1 << D0F0xE4_PHY_6A00_RxInCalForce_OFFSET
  },
  {
    WRAP_SPACE (0, D0F0xE4_WRAP_8016_ADDRESS),
    D0F0xE4_WRAP_8016_CalibAckLatency_MASK,
    0
  },
  {
    PHY_SPACE (0, 0, D0F0xE4_PHY_4004_ADDRESS),
    D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdVal_MASK | D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdEn_MASK,
    (0x1 << D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdVal_OFFSET) | (0x1 << D0F0xE4_PHY_4004_PllBiasGenPdnbOvrdEn_OFFSET)
  },
  {
    D0F0xE4_x0108_8071_ADDRESS,
    D0F0xE4_x0108_8071_RxAdjust_MASK,
    0x3 << D0F0xE4_x0108_8071_RxAdjust_OFFSET
  },
  {
    D0F0xE4_x0108_8072_ADDRESS,
    D0F0xE4_x0108_8072_TxAdjust_MASK,
    0x3 << D0F0xE4_x0108_8072_TxAdjust_OFFSET
  },
};

/*----------------------------------------------------------------------------------------*/
/**
 * Configure engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  EngineType          Engine Type
 * @param[in]  ConfigurationId     Configuration ID
 * @retval     AGESA_SUCCESS       Configuration successfully applied
 * @retval     AGESA_UNSUPPORTED   No more configuration available for given engine type
 * @retval     AGESA_ERROR         Requested configuration not supported
 */
AGESA_STATUS
PcieFmConfigureEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_ERROR;
  switch (Wrapper->WrapId) {
  case GPP_WRAP_ID:
    if (EngineType != PciePortEngine) {
      return AGESA_UNSUPPORTED;
    }
    Status = PcieOnConfigureGppEnginesLaneAllocation (Wrapper, ConfigurationId);
    break;
  case DDI_WRAP_ID:
    if (EngineType != PcieDdiEngine) {
      return AGESA_UNSUPPORTED;
    }
    Status = PcieOnConfigureDdiEnginesLaneAllocation (Wrapper, ConfigurationId);
    break;
  default:
    ASSERT (FALSE);

  }
  return Status;
}

CONST UINT8 GppLaneConfigurationTable [][NUMBER_OF_GPP_PORTS * 2] = {
//4     5                                6                               7                                8 (SB)
  {4, 7, UNUSED_LANE_ID, UNUSED_LANE_ID,  UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID,  0, 3},
  {4, 5, 6,              7,               UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID, UNUSED_LANE_ID,  0, 3},
  {4, 5, UNUSED_LANE_ID, UNUSED_LANE_ID,  6,              7,              UNUSED_LANE_ID, UNUSED_LANE_ID,  0, 3},
  {4, 5, 6,              6,               7,              7,              UNUSED_LANE_ID, UNUSED_LANE_ID,  0, 3},
  {4, 5, UNUSED_LANE_ID, UNUSED_LANE_ID,  6,              6,              7,              7,               0, 3},
  {4, 4, 5,              5,               6,              6,              7,              7,               0, 3}
};

CONST UINT8 GppPortIdConfigurationTable [][NUMBER_OF_GPP_PORTS] = {
//4  5  6  7  8 (SB)
  {1, 2, 3, 4, 0},
  {1, 2, 3, 4, 0},
  {1, 3, 2, 4, 0},
  {1, 2, 3, 4, 0},
  {1, 4, 2, 3, 0},
  {1, 2, 3, 4, 0}
};

/*----------------------------------------------------------------------------------------*/
/**
 * Configure GFX engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  ConfigurationId     Configuration ID
 * @retval     AGESA_SUCCESS       Configuration successfully applied
 * @retval     AGESA_ERROR         Requested configuration not supported
 */


AGESA_STATUS
STATIC
PcieOnConfigureGppEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  )
{
  PCIe_ENGINE_CONFIG  *EnginesList;
  UINTN               CoreLaneIndex;
  UINTN               PortIdIndex;
  if (ConfigurationId > ((sizeof (GppLaneConfigurationTable) / (NUMBER_OF_GPP_PORTS * 2)) - 1)) {
    return AGESA_ERROR;
  }
  EnginesList = PcieWrapperGetEngineList (Wrapper);
  CoreLaneIndex = 0;
  PortIdIndex = 0;
  do {
    if (PortIdIndex > 0) EnginesList++;
    EnginesList->Flags &= ~DESCRIPTOR_ALLOCATED;
    EnginesList->Type.Port.PortId = GppPortIdConfigurationTable [ConfigurationId][PortIdIndex++];
    EnginesList->Type.Port.StartCoreLane = GppLaneConfigurationTable [ConfigurationId][CoreLaneIndex++];
    EnginesList->Type.Port.EndCoreLane = GppLaneConfigurationTable [ConfigurationId][CoreLaneIndex++];

  } while (IS_LAST_DESCRIPTOR (EnginesList));
  return  AGESA_SUCCESS;
}


CONST UINT8 DdiLaneConfigurationTable [][NUMBER_OF_DDIS * 2] = {
  {0,  3,   4,   7,  8,  11}
};

/*----------------------------------------------------------------------------------------*/
/**
 * Configure DDI engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  ConfigurationId     Configuration ID
 * @retval     AGESA_SUCCESS       Configuration successfully applied
 * @retval     AGESA_ERROR         Requested configuration not supported
 */


AGESA_STATUS
STATIC
PcieOnConfigureDdiEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  )
{
  PCIe_ENGINE_CONFIG  *EnginesList;
  UINTN               LaneIndex;
  EnginesList = PcieWrapperGetEngineList (Wrapper);
  if (ConfigurationId > ((sizeof (DdiLaneConfigurationTable) / (NUMBER_OF_DDIS * 2)) - 1)) {
    return AGESA_ERROR;
  }
  LaneIndex = 0;
  do {
    if (LaneIndex > 0) EnginesList++;
    EnginesList->Flags &= ~DESCRIPTOR_ALLOCATED;
    EnginesList->EngineData.StartLane = DdiLaneConfigurationTable [ConfigurationId][LaneIndex++] +
                                        Wrapper->StartPhyLane;
    EnginesList->EngineData.EndLane = DdiLaneConfigurationTable [ConfigurationId][LaneIndex++] +
                                      Wrapper->StartPhyLane;
  } while (IS_LAST_DESCRIPTOR (EnginesList));
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure clock to run out of the wrapper at specific speed
 *
 *
 * @param[in]  LinkSpeedCapability  Link Speed capability
 * @param[in]  Wrapper              Pointer to wrapper config descriptor
 * @param[in]  Pcie                 Pointer to global PCIe configuration
 */
VOID
PcieFmConfigureClock (
  IN      PCIE_LINK_SPEED_CAP           LinkSpeedCapability,
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  )
{
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get configuration Value for GPP wrapper
 *
 *
 *
 * @param[in]   ConfigurationSignature  Configuration signature
 * @param[out]  ConfigurationValue      Configuration value
 * @retval      AGESA_SUCCESS           Correct core configuration value returned by in *ConfigurationValue
 * @retval      AGESA_ERROR             ConfigurationSignature is incorrect
 */
AGESA_STATUS
PcieOnGetGppConfigurationValue (
  IN       UINT64    ConfigurationSignature,
     OUT   UINT8     *ConfigurationValue
  )
{
  switch (ConfigurationSignature) {
  case GPP_CORE_x4x1x1x1x1:
    *ConfigurationValue = 0x4;
    break;
  case GPP_CORE_x4x2x1x1:
  case GPP_CORE_x4x2x1x1_ST:
    //Configuration  2:1:1 - Device Numbers 4:5:6
    //Configuration  2:1:1 - Device Numbers 4:6:7
    *ConfigurationValue = 0x3;
    break;
  case GPP_CORE_x4x2x2:
  case GPP_CORE_x4x2x2_ST:
    //Configuration  2:2 - Device Numbers 4:5
    //Configuration  2:2 - Device Numbers 4:6
    *ConfigurationValue = 0x2;
    break;
  case GPP_CORE_x4x4:
    *ConfigurationValue = 0x1;
    break;
  default:
    ASSERT (FALSE);
    return  AGESA_ERROR;
  }
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get core configuration value
 *
 *
 *
 * @param[in]  Wrapper                Pointer to internal configuration data area
 * @param[in]  CoreId                 Core ID
 * @param[in]  ConfigurationSignature Configuration signature
 * @param[out] ConfigurationValue     Configuration value (for core configuration)
 * @retval     AGESA_SUCCESS          Configuration successfully applied
 * @retval     AGESA_ERROR            Core configuration value can not be determined
 */
AGESA_STATUS
PcieFmGetCoreConfigurationValue (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 CoreId,
  IN      UINT64                ConfigurationSignature,
  IN      UINT8                 *ConfigurationValue
  )
{
  AGESA_STATUS  Status;

  if (Wrapper->WrapId == GPP_WRAP_ID) {
    Status = PcieOnGetGppConfigurationValue (ConfigurationSignature, ConfigurationValue);
  } else {
    Status = AGESA_ERROR;
  }
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get max link speed capability supported by this port
 *
 *
 *
 * @param[in]  Flags              See Flags PCIE_PORT_GEN_CAP_BOOT / PCIE_PORT_GEN_CAP_MAX
 * @param[in]  Engine             Pointer to engine config descriptor
 * @param[in]  Pcie               Pointer to global PCIe configuration
 * @retval     PcieGen1/PcieGen2  Max supported link gen capability
 */
PCIE_LINK_SPEED_CAP
PcieFmGetLinkSpeedCap (
  IN       UINT32                Flags,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIE_LINK_SPEED_CAP LinkSpeedCapability;
  ASSERT (Engine->Type.Port.PortData.LinkSpeedCapability < MaxPcieGen);
  LinkSpeedCapability = PcieGen2;
  if (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGenMaxSupported) {
    Engine->Type.Port.PortData.LinkSpeedCapability = (UINT8) LinkSpeedCapability;
  }
  if (Pcie->PsppPolicy == PsppPowerSaving) {
    LinkSpeedCapability = PcieGen1;
  }
  if (Engine->Type.Port.PortData.LinkSpeedCapability < LinkSpeedCapability) {
    LinkSpeedCapability = Engine->Type.Port.PortData.LinkSpeedCapability;
  }
  if ((Flags & PCIE_PORT_GEN_CAP_BOOT) != 0) {
    if (Pcie->PsppPolicy == PsppBalanceLow || Engine->Type.Port.PortData.MiscControls.LinkSafeMode == PcieGen1) {
      LinkSpeedCapability = PcieGen1;
    }
  }
  return LinkSpeedCapability;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Various initialization  needed prior topology and configuration initialization
 *
 *
 *
 * @param[in]  Pcie            Pointer to global PCIe configuration
 *
 */
VOID
PcieFmPreInit (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32                    Index;
  PCIe_SILICON_CONFIG       *Silicon;
  PCIE_LINK_SPEED_CAP       GlobalCapability;
  F14_PCIe_WRAPPER_CONFIG   *F14PcieWrapper;

  Silicon = PcieComplexGetSiliconList (&Pcie->ComplexList[0]);
  F14PcieWrapper = &((F14_COMPLEX_CONFIG*) Silicon)->FmGppWrapper ;
  GlobalCapability = PcieUtilGlobalGenCapability (
                       PCIE_PORT_GEN_CAP_MAX | PCIE_GLOBAL_GEN_CAP_ALL_PORTS,
                       Pcie
                       );
  if ((GlobalCapability == PcieGen1)  && (F14PcieWrapper->NativeGen1Support == TRUE)) {
    PcieFmExecuteNativeGen1Reconfig (Pcie);
  }
  Silicon = PcieComplexGetSiliconList (&Pcie->ComplexList[0]);
  for (Index = 0; Index < ARRAY_SIZE(PcieInitTable); Index++) {
    PcieSiliconRegisterRMW (
      Silicon,
      PcieInitTable[Index].Reg,
      PcieInitTable[Index].Mask,
      PcieInitTable[Index].Data,
      FALSE,
      Pcie
      );
  }

  // Set PCIe SSID.
  PcieSiliconRegisterRMW (
    Silicon,
    WRAP_SPACE (0, D0F0xE4_WRAP_8002_ADDRESS),
    D0F0xE4_WRAP_8002_SubsystemVendorID_MASK | D0F0xE4_WRAP_8002_SubsystemID_MASK,
    UserOptions.CfgGnbPcieSSID,
    FALSE,
    Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if engine can be remapped to Device/function number requested by user
 * defined engine descriptor
 *
 *   Function only called if requested device/function does not much native device/function
 *
 * @param[in]  PortDescriptor      Pointer to user defined engine descriptor
 * @param[in]  Engine              Pointer engine configuration
 * @retval     TRUE                Descriptor can be mapped to engine
 * @retval     FALSE               Descriptor can NOT be mapped to engine
 */

BOOLEAN
PcieFmCheckPortPciDeviceMapping (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  return FALSE;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get core configuration string
 *
 *   Debug function for logging configuration
 *
 * @param[in] Wrapper              Pointer to internal configuration data area
 * @param[in] ConfigurationValue   Configuration value
 * @retval                         Configuration string
 */

CONST CHAR8*
PcieFmDebugGetCoreConfigurationString (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 ConfigurationValue
  )
{
  switch (ConfigurationValue) {
  case 4:
    return (CONST CHAR8*)"1x4, 4x1";
  case 3:
    return (CONST CHAR8*)"1x4, 1x2, 2x1";
  case 2:
    return (CONST CHAR8*)"1x4, 2x2";
  case 1:
    return (CONST CHAR8*)"1x4, 1x4";
  default:
    break;
  }
  return (CONST CHAR8*)" !!! Something Wrong !!!";
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get wrapper name
 *
 *  Debug function for logging wrapper name
 *
 * @param[in] Wrapper              Pointer to internal configuration data area
 * @retval                         Wrapper Name string
 */

CONST CHAR8*
PcieFmDebugGetWrapperNameString (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  )
{
  switch (Wrapper->WrapId) {
  case  GPP_WRAP_ID:
    return (CONST CHAR8*)"GPPSB";
  case  DDI_WRAP_ID:
    return (CONST CHAR8*)"Virtual DDI";
  default:
    break;
  }
  return (CONST CHAR8*)" !!! Something Wrong !!!";
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get register address name
 *
 *  Debug function for logging register trace
 *
 * @param[in] AddressFrame         Address Frame
 * @retval                         Register address name
 */
CONST CHAR8*
PcieFmDebugGetHostRegAddressSpaceString (
  IN      UINT16                 AddressFrame
  )
{
  switch (AddressFrame) {
  case  0x130:
    return (CONST CHAR8*)"GPP WRAP";
  case  0x110:
    return (CONST CHAR8*)"GPP PIF0";
  case  0x120:
    return (CONST CHAR8*)"GPP PHY0";
  case  0x101:
    return (CONST CHAR8*)"GPP CORE";
  default:
    break;
  }
  return (CONST CHAR8*)" !!! Something Wrong !!!";
}


/*----------------------------------------------------------------------------------------*/
/**
 * Execute/clean up reconfiguration for Gen 1 native mode
 *
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieFmExecuteNativeGen1Reconfig (
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmExecuteNativeGen1Reconfig Enter\n");
  NbSmuServiceRequest (19, FALSE, GnbLibGetHeader (Pcie));
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieFmExecuteNativeGen1Reconfig Enter\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check if the lane can be muxed by link width requested by user
 * defined engine descriptor
 *
 *    Check Engine StartCoreLane could be aligned by user requested link width(x1, x2, x4, x8, x16).
 *    Check Engine StartCoreLane could be aligned by user requested link width x2.
 *
 * @param[in]  PortDescriptor      Pointer to user defined engine descriptor
 * @param[in]  Engine              Pointer engine configuration
 * @retval     TRUE                Lane can be muxed
 * @retval     FALSE               LAne can NOT be muxed
 */

BOOLEAN
PcieFmCheckPortPcieLaneCanBeMuxed (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  UINT16  DescriptorHiLane;
  UINT16  DescriptorLoLane;
  UINT16  DescriptorNumberOfLanes;
  PCIe_WRAPPER_CONFIG         *Wrapper;
  UINT16                      NormalizedLoPhyLane;
  BOOLEAN                     Result;

  Result  = FALSE;
  Wrapper = (PCIe_WRAPPER_CONFIG *)Engine->Wrapper;
  DescriptorLoLane = MIN (PortDescriptor->EngineData.StartLane, PortDescriptor->EngineData.EndLane);
  DescriptorHiLane = MAX (PortDescriptor->EngineData.StartLane, PortDescriptor->EngineData.EndLane);
  DescriptorNumberOfLanes = DescriptorHiLane - DescriptorLoLane + 1;

  NormalizedLoPhyLane = DescriptorLoLane - Wrapper->StartPhyLane;

  if (NormalizedLoPhyLane == Engine->Type.Port.StartCoreLane) {
    Result = TRUE;
  } else {
    if (((Engine->Type.Port.StartCoreLane % 2) == 0) || (Engine->Type.Port.StartCoreLane == 0)) {
      if (NormalizedLoPhyLane == 0) {
        Result = TRUE;
      } else {
        if (((NormalizedLoPhyLane % 2) == 0) && ((NormalizedLoPhyLane % DescriptorNumberOfLanes) == 0)) {
          Result = TRUE;
        }
      }
    }
  }

  return Result;
}
