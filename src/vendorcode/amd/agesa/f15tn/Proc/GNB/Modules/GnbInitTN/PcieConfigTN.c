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
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLibV1.h"
#include  "GnbRegistersTN.h"
#include  "GnbRegisterAccTN.h"
#include  "PcieComplexDataTN.h"
#include  "Filecode.h"
#define FILECODE PROC_GNB_MODULES_GNBINITTN_PCIECONFIGTN_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern TN_COMPLEX_CONFIG      ComplexDataTN;
extern PCIe_PORT_DESCRIPTOR   DefaultSbPort;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

AGESA_STATUS
PcieConfigureEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
STATIC
PcieConfigureGfxEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
STATIC
PcieConfigureGppEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
STATIC
PcieConfigureDdiEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
STATIC
PcieConfigureDdi2EnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  );

AGESA_STATUS
PcieGetCoreConfigurationValueTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 CoreId,
  IN      UINT64                ConfigurationSignature,
  IN      UINT8                 *ConfigurationValue
  );

BOOLEAN
PcieCheckPortPciDeviceMappingTN (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  );

CONST CHAR8*
PcieDebugGetCoreConfigurationStringTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 ConfigurationValue
  );

CONST CHAR8*
PcieDebugGetWrapperNameStringTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  );

CONST CHAR8*
PcieDebugGetHostRegAddressSpaceStringTN (
  IN      PCIe_SILICON_CONFIG    *Silicon,
  IN      UINT16                 AddressFrame
  );

BOOLEAN
PcieCheckPortPcieLaneCanBeMuxedTN (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  );

AGESA_STATUS
PcieMapPortPciAddressTN (
  IN      PCIe_ENGINE_CONFIG     *Engine
  );

AGESA_STATUS
PcieGetComplexDataLengthTN (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

AGESA_STATUS
PcieBuildComplexConfigurationTN (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

UINT32
PcieGetNativePhyLaneBitmapTN (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  );

AGESA_STATUS
PcieGetSbConfigInfoTN (
  IN       UINT8                         SocketId,
     OUT   PCIe_PORT_DESCRIPTOR          *SbPort,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  );

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
PcieConfigureEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_ERROR;
  switch (Wrapper->WrapId) {
  case GFX_WRAP_ID:
    Status = PcieConfigureGfxEnginesLaneAllocationTN (Wrapper, EngineType, ConfigurationId);
    break;
  case GPP_WRAP_ID:
    if (EngineType != PciePortEngine) {
      return AGESA_UNSUPPORTED;
    }
    Status = PcieConfigureGppEnginesLaneAllocationTN (Wrapper, ConfigurationId);
    break;
  case DDI_WRAP_ID:
    if (EngineType != PcieDdiEngine) {
      return AGESA_UNSUPPORTED;
    }
    Status = PcieConfigureDdiEnginesLaneAllocationTN (Wrapper, ConfigurationId);
    break;
  case DDI2_WRAP_ID:
    if (EngineType != PcieDdiEngine) {
      return AGESA_UNSUPPORTED;
    }
    Status = PcieConfigureDdi2EnginesLaneAllocationTN (Wrapper, ConfigurationId);
    break;
  default:
    ASSERT (FALSE);
  }
  return Status;
}

CONST UINT8 GfxPortLaneConfigurationTable [][NUMBER_OF_GFX_PORTS * 2] = {
{0,  15, UNUSED_LANE_ID, UNUSED_LANE_ID},
{0,   7, 8,              15}
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
PcieConfigureGfxPortEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  )
{
  UINTN               CoreLaneIndex;
  PCIe_ENGINE_CONFIG  *EnginesList;
  if (ConfigurationId > ((sizeof (GfxPortLaneConfigurationTable) / (NUMBER_OF_GFX_PORTS * 2)) - 1)) {
    return AGESA_ERROR;
  }
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  CoreLaneIndex = 0;
  while (EnginesList != NULL) {
    if (PcieLibIsPcieEngine (EnginesList)) {
      PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
      EnginesList->Type.Port.StartCoreLane = GfxPortLaneConfigurationTable [ConfigurationId][CoreLaneIndex++];
      EnginesList->Type.Port.EndCoreLane = GfxPortLaneConfigurationTable [ConfigurationId][CoreLaneIndex++];
    }
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return  AGESA_SUCCESS;
}

CONST UINT8 GfxDdiLaneConfigurationTable [][NUMBER_OF_GFX_DDIS * 2] = {
  {0,   7, 8,  15, UNUSED_LANE_ID, UNUSED_LANE_ID},
  {4,   7, 8,  15, UNUSED_LANE_ID, UNUSED_LANE_ID},
  {0,   7, 8,  11, 12,             15},
  {4,   7, 8,  11, 12,             15}
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
PcieConfigureGfxDdiEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  )
{
  UINTN               LaneIndex;
  PCIe_ENGINE_CONFIG  *EnginesList;
  if (ConfigurationId > ((sizeof (GfxDdiLaneConfigurationTable) / (NUMBER_OF_GFX_DDIS * 2)) - 1)) {
    return AGESA_ERROR;
  }
  LaneIndex = 0;
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  while (EnginesList != NULL) {
    if (PcieLibIsDdiEngine (EnginesList)) {
      PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
      EnginesList->EngineData.StartLane = GfxDdiLaneConfigurationTable [ConfigurationId][LaneIndex++] + Wrapper->StartPhyLane;
      EnginesList->EngineData.EndLane = GfxDdiLaneConfigurationTable [ConfigurationId][LaneIndex++] + Wrapper->StartPhyLane;
    }
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Configure GFX engine list to support lane allocation according to configuration ID.
 *
 *
 *
 * @param[in]  Wrapper             Pointer to wrapper config descriptor
 * @param[in]  EngineType          Engine Type
 * @param[in]  ConfigurationId     Configuration ID
 * @retval     AGESA_SUCCESS       Configuration successfully applied
 * @retval     AGESA_UNSUPPORTED   Configuration not applicable
 * @retval     AGESA_ERROR         Requested configuration not supported
 */

AGESA_STATUS
STATIC
PcieConfigureGfxEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIE_ENGINE_TYPE              EngineType,
  IN      UINT8                         ConfigurationId
  )
{
  AGESA_STATUS  Status;

  switch (EngineType) {
  case PciePortEngine:
    Status = PcieConfigureGfxPortEnginesLaneAllocationTN (Wrapper, ConfigurationId);
    break;
  case PcieDdiEngine:
    Status = PcieConfigureGfxDdiEnginesLaneAllocationTN (Wrapper, ConfigurationId);
    break;
  default:
    Status = AGESA_UNSUPPORTED;
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
PcieConfigureGppEnginesLaneAllocationTN (
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
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  CoreLaneIndex = 0;
  PortIdIndex = 0;
  while (EnginesList != NULL) {
    PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
    EnginesList->Type.Port.StartCoreLane = GppLaneConfigurationTable [ConfigurationId][CoreLaneIndex++];
    EnginesList->Type.Port.EndCoreLane = GppLaneConfigurationTable [ConfigurationId][CoreLaneIndex++];
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return  AGESA_SUCCESS;
}


CONST UINT8 DdiLaneConfigurationTable [][NUMBER_OF_DDIS * 2] = {
  {0,  3,   4,              7},
  {0,  7,   UNUSED_LANE_ID, UNUSED_LANE_ID}
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
PcieConfigureDdiEnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  )
{
  PCIe_ENGINE_CONFIG  *EnginesList;
  UINTN               LaneIndex;
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  if (ConfigurationId > ((sizeof (DdiLaneConfigurationTable) / (NUMBER_OF_DDIS * 2)) - 1)) {
    return AGESA_ERROR;
  }
  LaneIndex = 0;
  while (EnginesList != NULL) {
    PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
    EnginesList->EngineData.StartLane = DdiLaneConfigurationTable [ConfigurationId][LaneIndex++] + Wrapper->StartPhyLane;
    EnginesList->EngineData.EndLane = DdiLaneConfigurationTable [ConfigurationId][LaneIndex++] + Wrapper->StartPhyLane;
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return  AGESA_SUCCESS;
}


CONST UINT8 Ddi2LaneConfigurationTable [][NUMBER_OF_DDIS2 * 2] = {
  {0,  6},
  {0,  3}
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
PcieConfigureDdi2EnginesLaneAllocationTN (
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      UINT8                         ConfigurationId
  )
{
  PCIe_ENGINE_CONFIG  *EnginesList;
  UINTN               LaneIndex;
  EnginesList = PcieConfigGetChildEngine (Wrapper);
  if (ConfigurationId > ((sizeof (Ddi2LaneConfigurationTable) / (NUMBER_OF_DDIS2 * 2)) - 1)) {
    return AGESA_ERROR;
  }
  LaneIndex = 0;
  while (EnginesList != NULL) {
    PcieConfigResetDescriptorFlags (EnginesList, DESCRIPTOR_ALLOCATED);
    EnginesList->EngineData.StartLane = Ddi2LaneConfigurationTable [ConfigurationId][LaneIndex++] + Wrapper->StartPhyLane;
    EnginesList->EngineData.EndLane = Ddi2LaneConfigurationTable [ConfigurationId][LaneIndex++] + Wrapper->StartPhyLane;
    EnginesList = PcieLibGetNextDescriptor (EnginesList);
  }
  return  AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Get configuration Value for GFX wrapper
 *
 *
 *
 * @param[in]   ConfigurationSignature  Configuration signature
 * @param[out]  ConfigurationValue      Configuration value
 * @retval     AGESA_SUCCESS            Correct core configuration value returned by in *ConfigurationValue
 * @retval     AGESA_ERROR              ConfigurationSignature is incorrect.
 */
STATIC AGESA_STATUS
PcieGetGfxConfigurationValueTN (
  IN       UINT64    ConfigurationSignature,
     OUT   UINT8     *ConfigurationValue
  )
{
  switch (ConfigurationSignature) {
  case GFX_CORE_x16:
    *ConfigurationValue = 0;
    break;
  case GFX_CORE_x8x8:
    *ConfigurationValue = 0x5;
    break;
  default:
    ASSERT (FALSE);
    return  AGESA_ERROR;
  }
  return  AGESA_SUCCESS;
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
STATIC AGESA_STATUS
PcieGetGppConfigurationValueTN (
  IN       UINT64    ConfigurationSignature,
     OUT   UINT8     *ConfigurationValue
  )
{
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
PcieGetCoreConfigurationValueTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 CoreId,
  IN      UINT64                ConfigurationSignature,
  IN      UINT8                 *ConfigurationValue
  )
{
  AGESA_STATUS  Status;

  if (Wrapper->WrapId == GFX_WRAP_ID) {
    Status = PcieGetGfxConfigurationValueTN (ConfigurationSignature, ConfigurationValue);
  } else if (Wrapper->WrapId == GPP_WRAP_ID) {
    Status = PcieGetGppConfigurationValueTN (ConfigurationSignature, ConfigurationValue);
  } else {
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
PcieCheckPortPciDeviceMappingTN (
  IN      PCIe_PORT_DESCRIPTOR  *PortDescriptor,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  BOOLEAN   Result;
  if (PortDescriptor->Port.DeviceNumber >= 2 && PortDescriptor->Port.DeviceNumber <= 7 && PortDescriptor->Port.FunctionNumber == 0 && !PcieConfigIsSbPcieEngine (Engine)) {
    Result = TRUE;
  } else {
    Result = FALSE;
  }
  return Result;
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
PcieDebugGetCoreConfigurationStringTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT8                 ConfigurationValue
  )
{
  switch (ConfigurationValue) {
  case 0:
    return "1x16";
  case 5:
    return "2x8";
  case 4:
    return "1x4, 4x1";
  case 3:
    return "1x4, 1x2, 2x1";
  case 2:
    return "1x4, 2x2";
  case 1:
    return "1x4, 1x4";
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
PcieDebugGetWrapperNameStringTN (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  )
{
  switch (Wrapper->WrapId) {
  case  GPP_WRAP_ID:
    return "GPPSB";
  case  GFX_WRAP_ID:
    return "GFX";
  case  DDI_WRAP_ID:
    return "DDI";
  case  DDI2_WRAP_ID:
    return "DDI2";
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
PcieDebugGetHostRegAddressSpaceStringTN (
  IN      PCIe_SILICON_CONFIG    *Silicon,
  IN      UINT16                 AddressFrame
  )
{
  switch (AddressFrame) {
  case  0x130:
    return "GPP  WRAP";
  case  0x131:
    return "GFX  WRAP";
  case  0x132:
    return "DDI  WRAP";
  case  0x133:
    return "DDI2 WRAP";
  case  0x110:
    return "GPP  PIF0";
  case  0x111:
    return "GFX  PIF0";
  case  0x211:
    return "GFX  PIF1";
  case  0x112:
    return "DDI  PIF0";
  case  0x113:
    return "DDI2 PIF0";
  case  0x120:
    return "GPP  PHY0";
  case  0x121:
    return "GFX  PHY0";
  case  0x221:
    return "GFX  PHY1";
  case  0x122:
    return "DDI  PHY0";
  case  0x123:
    return "DDI2 PHY0";
  case  0x101:
    return "GPP  CORE";
  case  0x201:
    return "GFX  CORE";
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
PcieCheckPortPcieLaneCanBeMuxedTN (
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
    if (((PortDescriptor->Port.MiscControls.SbLink == 0x0) && ((Engine->Type.Port.StartCoreLane % 2) == 0)) || (Engine->Type.Port.StartCoreLane == 0)) {
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
PcieMapPortPciAddressTN (
  IN      PCIe_ENGINE_CONFIG     *Engine
  )
{
  AGESA_STATUS            Status;
  TN_COMPLEX_CONFIG       *ComplexConfig;
  PCIe_PLATFORM_CONFIG    *Pcie;
  UINT8                   PortDevMap[6];
  UINT8                   FreeDevMap[6];
  UINT8                   PortIndex;
  UINT8                   EnginePortIndex;
  UINT8                   FreeIndex;
  D0F0x64_x20_STRUCT      D0F0x64_x20;
  D0F0x64_x21_STRUCT      D0F0x64_x21;
  Status = AGESA_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddressTN Enter\n");
  if (Engine->Type.Port.PortData.DeviceNumber == 0 && Engine->Type.Port.PortData.FunctionNumber == 0) {
    Engine->Type.Port.PortData.DeviceNumber = Engine->Type.Port.NativeDevNumber;
    Engine->Type.Port.PortData.FunctionNumber = Engine->Type.Port.NativeFunNumber;
  }
  if (!PcieConfigIsSbPcieEngine (Engine)) {
    ComplexConfig = (TN_COMPLEX_CONFIG *) PcieConfigGetParent (DESCRIPTOR_SILICON, &Engine->Header);
    Pcie = (PCIe_PLATFORM_CONFIG *) PcieConfigGetParent (DESCRIPTOR_PLATFORM, &Engine->Header);
    LibAmdMemFill (&FreeDevMap[0], 0x0, sizeof (FreeDevMap), GnbLibGetHeader (Pcie));
    LibAmdMemCopy (&PortDevMap[0], &ComplexConfig->FmSilicon.PortDevMap, sizeof (PortDevMap), GnbLibGetHeader (Pcie));
    for (PortIndex = 0; PortIndex < sizeof (PortDevMap); PortIndex++) {
      if (PortDevMap[PortIndex] != 0) {
        FreeDevMap[PortDevMap[PortIndex] - 2] = 1;
      }
    }
    EnginePortIndex = Engine->Type.Port.PortData.DeviceNumber - 2;
    if (FreeDevMap[EnginePortIndex] == 0) {
      // Dev number not yet allocated
      ComplexConfig->FmSilicon.PortDevMap[Engine->Type.Port.NativeDevNumber - 2] = Engine->Type.Port.PortData.DeviceNumber;
      FreeDevMap[EnginePortIndex] = 1;
      PortDevMap[Engine->Type.Port.NativeDevNumber - 2] = Engine->Type.Port.PortData.DeviceNumber;
      for (PortIndex = 0; PortIndex < sizeof (PortDevMap); PortIndex++) {
        if (PortDevMap[PortIndex] == 0) {
          for (FreeIndex = 0; FreeIndex < sizeof (FreeDevMap); FreeIndex++) {
            if (FreeDevMap[FreeIndex] == 0) {
              FreeDevMap[FreeIndex] = 1;
              break;
            }
          }
          PortDevMap[PortIndex] = FreeIndex + 2;
        }
      }

      GnbRegisterReadTN (D0F0x64_x20_TYPE, D0F0x64_x20_ADDRESS, &D0F0x64_x20, 0, GnbLibGetHeader (Pcie));
      D0F0x64_x20.Field.ProgDevMapEn = 0;
      GnbRegisterWriteTN (D0F0x64_x20_TYPE, D0F0x64_x20_ADDRESS, &D0F0x64_x20, 0, GnbLibGetHeader (Pcie));
      GnbRegisterReadTN (D0F0x64_x21_TYPE, D0F0x64_x21_ADDRESS, &D0F0x64_x21, 0, GnbLibGetHeader (Pcie));
      D0F0x64_x21.Field.GfxPortADevmap = PortDevMap[2 - 2];
      D0F0x64_x21.Field.GfxPortBDevmap = PortDevMap[3 - 2];
      D0F0x64_x20.Field.GppPortBDevmap = PortDevMap[4 - 2];
      D0F0x64_x20.Field.GppPortCDevmap = PortDevMap[5 - 2];
      D0F0x64_x20.Field.GppPortDDevmap = PortDevMap[6 - 2];
      D0F0x64_x20.Field.GppPortEDevmap = PortDevMap[7 - 2];
      D0F0x64_x20.Field.ProgDevMapEn = 0x1;
      GnbRegisterWriteTN (D0F0x64_x20_TYPE, D0F0x64_x20_ADDRESS, &D0F0x64_x20, 0, GnbLibGetHeader (Pcie));
      GnbRegisterWriteTN (D0F0x64_x21_TYPE, D0F0x64_x21_ADDRESS, &D0F0x64_x21, 0, GnbLibGetHeader (Pcie));
      D0F0x64_x20.Field.ProgDevMapEn = 1;
      GnbRegisterWriteTN (D0F0x64_x20_TYPE, D0F0x64_x20_ADDRESS, &D0F0x64_x20, 0, GnbLibGetHeader (Pcie));
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Fail device %d to port %d\n", Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.NativeDevNumber);
      Status = AGESA_ERROR;
    }
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMapPortPciAddressTN Exit [0x%x]\n", Status);
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get total number of silicons/wrappers/engines for this complex
 *
 *
 *
 * @param[in]  SocketId              Socket ID.
 * @param[out] Length                Length of configuration info block
 * @param[out] StdHeader             Standard configuration header
 * @retval     AGESA_SUCCESS         Configuration data length is correct
 */
AGESA_STATUS
PcieGetComplexDataLengthTN (
  IN       UINT8                         SocketId,
     OUT   UINTN                         *Length,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  *Length = sizeof (TN_COMPLEX_CONFIG);
  return  AGESA_SUCCESS;
}




/*----------------------------------------------------------------------------------------*/
/**
 * Build configuration
 *
 *
 *
 * @param[in]  SocketId         Socket ID.
 * @param[out] Buffer           Pointer to buffer to build internal complex data structure
 * @param[out] StdHeader        Standard configuration header.
 * @retval     AGESA_SUCCESS    Configuration data build successfully
 */
AGESA_STATUS
PcieBuildComplexConfigurationTN (
  IN       UINT8                         SocketId,
     OUT   VOID                          *Buffer,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  LibAmdMemCopy (Buffer, &ComplexDataTN, sizeof (TN_COMPLEX_CONFIG), StdHeader);
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 *  get native PHY lane bitmap
 *
 *
 * @param[in]  PhyLaneBitmap  Package PHY lane bitmap
 * @param[in]  Engine         Standard configuration header.
 * @retval     Native PHY lane bitmap
 */
UINT32
PcieGetNativePhyLaneBitmapTN (
  IN       UINT32                        PhyLaneBitmap,
  IN       PCIe_ENGINE_CONFIG            *Engine
  )
{
  return PhyLaneBitmap;
}

STATIC CONST PCIe_PORT_DESCRIPTOR DefaultSbPortTN = {
  0,
  PCIE_ENGINE_DATA_INITIALIZER (PciePortEngine, 0, 3),
  PCIE_PORT_DATA_INITIALIZER (PortEnabled, ChannelTypeLowLoss, 8, HotplugDisabled, PcieGenMaxSupported, PcieGenMaxSupported, AspmL0sL1, 0)
};

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
PcieGetSbConfigInfoTN (
  IN       UINT8                         SocketId,
     OUT   PCIe_PORT_DESCRIPTOR          *SbPort,
  IN       AMD_CONFIG_PARAMS             *StdHeader
  )
{
  LibAmdMemCopy (SbPort, &DefaultSbPortTN, sizeof (DefaultSbPortTN), StdHeader);
  return AGESA_SUCCESS;
}
