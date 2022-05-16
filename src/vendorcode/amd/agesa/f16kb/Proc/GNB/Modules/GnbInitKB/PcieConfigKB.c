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
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersKB.h"
#include  "GnbRegisterAccKB.h"
#include  "PcieComplexDataKB.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITKB_PCIECONFIGKB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define DEVFUNC(d, f) ((((UINT8) d) << 3) | ((UINT8) f))

extern PCIe_LANE_ALLOC_DESCRIPTOR ROMDATA   PcieLaneAllocConfigurationKB[];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8*
PcieDebugGetCoreConfigurationStringKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 ConfigurationValue
  );

CONST CHAR8*
PcieDebugGetHostRegAddressSpaceStringKB (
  IN      PCIe_SILICON_CONFIG    *Silicon,
  IN      UINT16                 AddressFrame
  );

BOOLEAN
PcieCheckPortPcieLaneCanBeMuxedKB (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  );

AGESA_STATUS
PcieMapPortPciAddressKB (
  IN      PCIe_ENGINE_CONFIG     *Engine
  );

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
CONST CHAR8*
PcieDebugGetWrapperNameStringKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  );

AGESA_STATUS
PcieConfigureEnginesLaneAllocationKB (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
PcieGetCoreConfigurationValueKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 CoreId,
  IN      UINT64                ConfigurationSignature,
  IN      UINT8                 *ConfigurationValue
  );

BOOLEAN
PcieCheckPortPciDeviceMappingKB (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  );

AGESA_STATUS
PcieGetSbConfigInfoKB (
  IN       UINT8                         SocketId,
     OUT   PCIe_PORT_DESCRIPTOR          *SbPort,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

//
// Default port dev map
//
CONST UINT8 ROMDATA DefaultPortDevMap [] = {
  DEVFUNC (2, 1),
  DEVFUNC (2, 2),
  DEVFUNC (2, 3),
  DEVFUNC (2, 4),
  DEVFUNC (2, 5)
};

//
// Default apic config
//
CONST APIC_DEVICE_INFO ROMDATA DefaultIoapicConfig [] = {
  {0, 0, 0x18},
  {1, 0, 0x19},
  {2, 0, 0x1A},
  {3, 0, 0x1B},
  {4, 0, 0x18}
};


/*----------------------------------------------------------------------------------------*/
/**
 * Configure engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  PcieLaneConfig       Lane configuration descriptor
 * @param[in]  ConfigurationId     Configuration ID
 * @retval     AGESA_SUCCESS       Configuration successfully applied
 * @retval     AGESA_ERROR         Requested configuration not supported
 */
STATIC AGESA_STATUS
PcieConfigurePcieEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIe_LANE_ALLOC_DESCRIPTOR    *PcieLaneConfig,
  IN      UINT8                         ConfigurationId
  )
{
  UINT8               CoreLaneIndex;
  PCIe_ENGINE_CONFIG  *EnginesList;

  if (ConfigurationId >= PcieLaneConfig->NumberOfConfigurations) {
    return AGESA_ERROR;
  }
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  CoreLaneIndex = ConfigurationId * PcieLaneConfig->NumberOfEngines * 2;

  while (EnginesList != NULL) {
    if (PcieLibIsPcieEngine (EnginesList)) {
      PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
      EnginesList->Type.Port.StartCoreLane = PcieLaneConfig->ConfigTable[CoreLaneIndex++];
      EnginesList->Type.Port.EndCoreLane = PcieLaneConfig->ConfigTable[CoreLaneIndex++];
    }
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  DdiLaneConfig       Lane configuration descriptor
 * @param[in]  ConfigurationId     Configuration ID
 * @retval     AGESA_SUCCESS       Configuration successfully applied
 * @retval     AGESA_ERROR         Requested configuration not supported
 */
STATIC AGESA_STATUS
PcieConfigureDdiEnginesLaneAllocation (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIe_LANE_ALLOC_DESCRIPTOR    *DdiLaneConfig,
  IN      UINT8                         ConfigurationId
  )
{
  UINTN               LaneIndex;
  PCIe_ENGINE_CONFIG  *EnginesList;
  if (ConfigurationId >= DdiLaneConfig->NumberOfConfigurations) {
    return AGESA_ERROR;
  }
  LaneIndex = ConfigurationId * DdiLaneConfig->NumberOfEngines * 2;
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  while (EnginesList != NULL) {
    if (PcieLibIsDdiEngine (EnginesList)) {
      PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
      EnginesList->EngineData.StartLane = DdiLaneConfig->ConfigTable[LaneIndex++] + Wrapper->StartPhyLane;
      EnginesList->EngineData.EndLane = DdiLaneConfig->ConfigTable[LaneIndex++] + Wrapper->StartPhyLane;
    }
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return  AGESA_SUCCESS;
}

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
 * @retval     AGESA_ERROR         Requested configuration not supported
 */
AGESA_STATUS
PcieConfigureEnginesLaneAllocationKB (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  )
{
  AGESA_STATUS                Status;
  PCIe_LANE_ALLOC_DESCRIPTOR  *LaneConfigDescriptor;

  Status = AGESA_ERROR;
  LaneConfigDescriptor = PcieLaneAllocConfigurationKB;
  while (LaneConfigDescriptor != NULL) {
    if (LaneConfigDescriptor->WrapId == Wrapper->WrapId && LaneConfigDescriptor->EngineType == EngineType) {
      switch (EngineType) {
      case PciePortEngine:
        Status = PcieConfigurePcieEnginesLaneAllocation (Wrapper, LaneConfigDescriptor, ConfigurationId);
        break;
      case PcieDdiEngine:
        Status = PcieConfigureDdiEnginesLaneAllocation (Wrapper, LaneConfigDescriptor, ConfigurationId);
        break;
      default:
        ASSERT (FALSE);
      }
      break;
    }
    LaneConfigDescriptor = PcieConfigGetNextDataDescriptor (LaneConfigDescriptor);
  }
  return Status;
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
PcieGetCoreConfigurationValueKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 CoreId,
  IN      UINT64                ConfigurationSignature,
  IN      UINT8                 *ConfigurationValue
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;
  switch (ConfigurationSignature) {
  case GPP_CORE_x4x1x1x1x1:
    *ConfigurationValue = 0x4;
    break;
  case GPP_CORE_x4x2x1x1:
    *ConfigurationValue = 0x3;
    break;
  case GPP_CORE_x4x2x2:
    *ConfigurationValue = 0x2;
    break;
  case GPP_CORE_x4x4:
    *ConfigurationValue = 0x1;
    break;
  default:
    IDS_HDT_CONSOLE (PCIE_MISC, "ERROR!!![%s Wrapper] Unknown core config signature 0x%08x%08x\n",
      PcieDebugGetWrapperNameStringKB (Wrapper),
      ((UINT32 *) &ConfigurationSignature)[1],
      ((UINT32 *) &ConfigurationSignature)[0]
    );
    ASSERT (FALSE);
    Status = AGESA_ERROR;
  }
  return  Status;
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
PcieCheckPortPciDeviceMappingKB (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  UINT8 DevFunc;
  UINT8 Index;
  DevFunc = DEVFUNC (PortDescriptor->Port.DeviceNumber, PortDescriptor->Port.FunctionNumber);
  if (DevFunc == 0) {
    return TRUE;
  }
  for (Index = 0; Index < ARRAY_SIZE(DefaultPortDevMap); Index++) {
    if (DefaultPortDevMap[Index] == DevFunc) {
      return TRUE;
    }
  }
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
PcieDebugGetCoreConfigurationStringKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 ConfigurationValue
  )
{
  switch (ConfigurationValue) {
  case 0x4:
    return "4:1:1:1:1";
  case 0x3:
    return "4:2:1:1";
  case 0x2:
    return "4:2:2";
  case 0x1:
    return "4:4";
  default:
    break;
  }
  return " !!! Something Wrong !!!";
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
PcieDebugGetWrapperNameStringKB (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  )
{
  switch (Wrapper->WrapId) {
  case  GPP_WRAP_ID:
    return "GPP";
  case  DDI_WRAP_ID:
    return "Virtual DDI";
  default:
    break;
  }
  return " !!! Something Wrong !!!";
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get register address name
 *
 *  Debug function for logging register trace
 *
 * @param[in] Silicon              Silicon config descriptor
 * @param[in] AddressFrame         Address Frame
 * @retval                         Register address name
 */
CONST CHAR8*
PcieDebugGetHostRegAddressSpaceStringKB (
  IN      PCIe_SILICON_CONFIG    *Silicon,
  IN      UINT16                 AddressFrame
  )
{
  switch (AddressFrame) {
  case  0x130:
    return "GPP WRAP";
  case  0x110:
    return "GPP PIF0";
  case  0x120:
    return "GPP PHY0";
  case  0x140:
    return "GPP CORE";
  default:
    break;
  }
  return " !!! Something Wrong !!!";
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
 * @retval     FALSE               Lane can NOT be muxed
 */

BOOLEAN
PcieCheckPortPcieLaneCanBeMuxedKB (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  UINT16                DescriptorHiLane;
  UINT16                DescriptorLoLane;
  UINT16                DescriptorNumberOfLanes;
  PCIe_WRAPPER_CONFIG   *Wrapper;
  UINT16                NormalizedLoPhyLane;
  BOOLEAN               Result;

  Result  = FALSE;
  Wrapper = PcieConfigGetParentWrapper (Engine);
  DescriptorLoLane = MIN (PortDescriptor->EngineData.StartLane, PortDescriptor->EngineData.EndLane);
  DescriptorHiLane = MAX (PortDescriptor->EngineData.StartLane, PortDescriptor->EngineData.EndLane);
  DescriptorNumberOfLanes = DescriptorHiLane - DescriptorLoLane + 1;

  NormalizedLoPhyLane = DescriptorLoLane - Wrapper->StartPhyLane;

  if (NormalizedLoPhyLane == Engine->Type.Port.StartCoreLane) {
    Result = TRUE;
  } else {
    if (NormalizedLoPhyLane == 0) {
      Result = TRUE;
    } else {
      if ((NormalizedLoPhyLane % DescriptorNumberOfLanes) == 0) {
        Result = TRUE;
      }
    }
  }
  return Result;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Map engine to specific PCI device address
 *
 *
 *
 * @param[in]  Engine              Pointer to engine configuration
 * @retval     AGESA_ERROR         Fail to map PCI device address
 * @retval     AGESA_SUCCESS       Successfully allocate PCI address
 */

AGESA_STATUS
PcieMapPortPciAddressKB (
  IN      PCIe_ENGINE_CONFIG     *Engine
  )
{
  AGESA_STATUS            Status;
  KB_COMPLEX_CONFIG       *ComplexConfig;
  PCIe_COMPLEX_CONFIG     *Complex;
  PCIe_PLATFORM_CONFIG    *Pcie;
  UINT8                   DevFunc;
  UINT8                   Index;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddressKB Enter\n");
  Complex = (PCIe_COMPLEX_CONFIG *) PcieConfigGetParent (DESCRIPTOR_COMPLEX, &Engine->Header);
  Pcie = (PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &Complex->Header);
  if (Engine->Type.Port.PortData.DeviceNumber == 0 && Engine->Type.Port.PortData.FunctionNumber == 0) {
    Engine->Type.Port.PortData.DeviceNumber = Engine->Type.Port.NativeDevNumber;
    Engine->Type.Port.PortData.FunctionNumber = Engine->Type.Port.NativeFunNumber;
  }
  ComplexConfig = (KB_COMPLEX_CONFIG *) PcieConfigGetParentSilicon (Engine);
  IDS_OPTION_HOOK (IDS_GNB_PCIE_PORT_REMAP, &Engine->Type.Port, GnbLibGetHeader (Pcie));
  DevFunc = (Engine->Type.Port.PortData.DeviceNumber << 3) | Engine->Type.Port.PortData.FunctionNumber;
  for (Index = 0; Index < sizeof (ComplexConfig->FmSilicon.PortDevMap); ++Index) {
    if (ComplexConfig->FmSilicon.PortDevMap[Index] == DevFunc) {
      Status = AGESA_ERROR;
      break;
    }
  }
  if (Status == AGESA_SUCCESS) {
    ComplexConfig->FmSilicon.PortDevMap[Engine->Type.Port.PcieBridgeId] = DevFunc;
  }
  for (Index = 0; Index < sizeof (DefaultPortDevMap); ++Index) {
    if (DevFunc == DefaultPortDevMap[Index]) {
      Engine->Type.Port.LogicalBridgeId = Index;
      // Get the configuration from the table or from "auto settings"
      if (Engine->Type.Port.PortData.ApicDeviceInfo.GroupMap == 0x00) {
        // If Group is 0, use "Auto" settings
        Engine->Type.Port.PortData.ApicDeviceInfo = DefaultIoapicConfig[Index];
      }
      break;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddressKB Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Map engine to specific PCI device address
 *
 *
 * @param[in]  Silicon             Silicon config descriptor
 */

VOID
PcieSetPortPciAddressMapKB (
  IN      PCIe_SILICON_CONFIG     *Silicon
  )
{
  UINT8                   Index;
  UINT8                   DevFuncIndex;
  UINT8                   PortDevMap [sizeof (DefaultPortDevMap)];
  PCIe_PLATFORM_CONFIG    *Pcie;
  D0F0x64_x30_STRUCT      D0F0x64_x30;

  Pcie = (PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &Silicon->Header);
  LibAmdMemCopy (&PortDevMap[0], &DefaultPortDevMap[0], sizeof (DefaultPortDevMap), GnbLibGetHeader (Pcie));
  for (Index = 0; Index < sizeof (((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap); ++Index) {
    if (((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap[Index] != 0) {
      for (DevFuncIndex = 0; DevFuncIndex < sizeof (((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap); ++DevFuncIndex) {
        if (PortDevMap[DevFuncIndex] == ((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap[Index]) {
          PortDevMap[DevFuncIndex] = 0;
          break;
        }
      }
    }
  }
  for (Index = 0; Index < sizeof (((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap); ++Index) {
    if (((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap[Index] == 0) {
      for (DevFuncIndex = 0; DevFuncIndex < sizeof (((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap); ++DevFuncIndex) {
        if (PortDevMap[DevFuncIndex] != 0) {
          ((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap[Index] = PortDevMap[DevFuncIndex];
          PortDevMap[DevFuncIndex] = 0;
          break;
        }
      }
    }
    GnbRegisterReadKB ((GNB_HANDLE *) Silicon, D0F0x64_x30_TYPE, D0F0x64_x30_ADDRESS + Index, &D0F0x64_x30.Value, 0, GnbLibGetHeader (Pcie));
    D0F0x64_x30.Field.DevFnMap = ((KB_COMPLEX_CONFIG *) Silicon)->FmSilicon.PortDevMap[Index];
    GnbRegisterWriteKB ((GNB_HANDLE *) Silicon, D0F0x64_x30_TYPE, D0F0x64_x30_ADDRESS + Index, &D0F0x64_x30.Value, 0, GnbLibGetHeader (Pcie));
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Build default SB configuration descriptor
 *
 *
 * @param[in]  SocketId         Socket Id
 * @param[out] SbPort           Pointer to SB configuration descriptor
 * @param[in]  StdHeader        Standard configuration header.
 * @retval     AGESA_SUCCESS    Configuration data build successfully
 */
AGESA_STATUS
PcieGetSbConfigInfoKB (
  IN       UINT8                         SocketId,
     OUT   PCIe_PORT_DESCRIPTOR          *SbPort,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  return AGESA_UNSUPPORTED;
}
