/**
 * @file
 *
 * PCIE Late Initialization
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "NbPlatform.h"
#include "amdDebugOutLib.h"
#include "amdSbLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Amd PCIE Late Init for all NB.
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */
AGESA_STATUS
AmdPcieLateInit (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = LibNbApiCall (PcieLateInit, ConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Amd PCIE Late Init for all NB.
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */
AGESA_STATUS
AmdPcieLateInitWa (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = LibNbApiCall (PcieLateInitWa, ConfigPtr);
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Amd PCIE Special Init for all NB.
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */
AGESA_STATUS
AmdPcieValidatePortState (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = LibNbApiCall (PcieValidatePortState, ConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Amd PCIE S3 Init fro all NB.
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */
AGESA_STATUS
AmdPcieS3Init (
  IN      AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = LibNbApiCall (PcieLateInit, ConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NB PCIE Late Init.
 *    Extended  programming. Enable power management and misc capability.
 *    Prepare PCIE subsystem to boot to OS.
 *
 *
 * @param[in] NbConfigPtr  Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLateInit (
  IN      AMD_NB_CONFIG    *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  PcieLibUnHidePorts (NbConfigPtr);
  Status = PcieLateValidateConfiguration (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    PcieLibHidePorts (NbConfigPtr);
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return  Status;
  }
  PcieLibLateInit (NbConfigPtr);
  Status = PcieLateInitPorts (NbConfigPtr);
  Status = PcieLateInitCores (NbConfigPtr);
  PcieLibHidePorts (NbConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * NB PCIE Late Init.
 *    Extended  programming. Enable power management and misc capability.
 *    Prepare PCIE subsystem to boot to OS.
 *
 *
 * @param[in] NbConfigPtr  Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLateInitWa (
  IN      AMD_NB_CONFIG    *NbConfigPtr
  )
{
  UINT32        Value;
  BOOLEAN       SmuWa;
  LibNbPciIndexRead (NbConfigPtr->NbPciAddress.AddressValue | NB_MISC_INDEX, NB_MISC_REG4A, AccessWidth32, &Value, NbConfigPtr);
  SmuWa = ((Value & BIT21) != 0) ? TRUE : FALSE;
  if (SmuWa) {
    UINT32 SmuWaData;
    LibNbMcuControl (AssertReset, NbConfigPtr);
    SmuWaData = LibNbReadMcuRam (0xFE74, NbConfigPtr);
    SmuWaData &= 0x00ff;
    LibNbLoadMcuFirmwareBlock (0xFE74, 0x1, &SmuWaData, NbConfigPtr);
    LibNbMcuControl (DeAssertReset, NbConfigPtr);
  }
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Late init PCIE Ports
 *
 *
 *
*  @param[in] NbConfigPtr          Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLateInitPorts (
  IN     AMD_NB_CONFIG   *NbConfigPtr
  )
{
  AGESA_STATUS  Status;
  PCIE_CONFIG   *pPcieConfig;
  PORT          PortId;
  BOOLEAN       IsIommuEnabled;
  NB_INFO       NbInfo;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLateInitPorts Enter\n"));
  IsIommuEnabled = LibNbIsIommuEnabled (NbConfigPtr);
  NbInfo = LibNbGetRevisionInfo (NbConfigPtr);
  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
  Status = AGESA_SUCCESS;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    CORE  CoreId;
    CoreId = PcieLibGetCoreId (PortId, NbConfigPtr);
    if (!PcieLibIsValidCoreId (CoreId, NbConfigPtr)) {
      PcieLibPowerOffPortLanes (PortId, PcieLinkWidth_x0, NbConfigPtr);
    } else if (PcieLibIsValidPortId (PortId, NbConfigPtr)) {
      PCIE_LINK_WIDTH LinkWidth;
      PCI_ADDR        Port;
      LinkWidth = PcieLibGetLinkWidth (PortId, NbConfigPtr);
      CoreId = PcieLibGetCoreId (PortId, NbConfigPtr);
      Port = PcieLibGetPortPciAddress (PortId, NbConfigPtr);
      PcieLateCommonPortInit (PortId, NbConfigPtr);
      if (pPcieConfig->PortConfiguration[PortId].PortDetected == ON) {
        if (pPcieConfig->PortConfiguration[PortId].PortLinkMode == PcieLinkModeGen2SoftwareInitiated) {
          PcieInitiateSoftwareGen2 (PortId, NbConfigPtr);
        }
        PcieAsmpEnableOnPort (PortId, (UINT8)pPcieConfig->PortConfiguration[PortId].PortAspm, NbConfigPtr);
      }
      LibNbPciIndexRMW (Port.AddressValue  | NB_BIF_INDEX, NB_BIFNBP_REG70 , AccessS3SaveWidth32, (UINT32)~BIT12, 0, NbConfigPtr); //PCIE should not ignore malformed packet error or ATS request
      if (pPcieConfig->PortConfiguration[PortId].PortCompliance == OFF &&
          pPcieConfig->PortConfiguration[PortId].PortHotplug == OFF &&
          pPcieConfig->CoreSetting[CoreId].PowerOffUnusedLanes == ON) {
          PcieLibPowerOffPortLanes (PortId, LinkWidth, NbConfigPtr);
      }
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLateInitPorts Exit\n"));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Late init PCIE Cores. Core level feature/power management etc.
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLateInitCores (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  PCIE_CONFIG   *pPcieConfig;
  CORE          CoreId;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLateInitCores Enter\n"));
  Status = AGESA_SUCCESS;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "   Init CoreId [%d]\n", CoreId));
    if (pPcieConfig->CoreSetting[CoreId].PowerOffPllInL1 == ON) {
      PcieLibEnablePllPowerOffInL1 (CoreId, pConfig);
    }
    if (pPcieConfig->CoreSetting[CoreId].PowerOffPll == ON) {
      PcieLibPowerOffPll (CoreId, pConfig);
    }
    PcieLibMiscLateCoreSetting (CoreId, pConfig);
    PcieLibManageTxClock   (CoreId, pConfig);
    PcieLibManageLclkClock (CoreId, pConfig);
  }
#ifndef VC1_SUPPORT_DISABLE
  if (NB_SBDFO == 0 && pPcieConfig->PcieConfiguration.NbSbVc1 == ON) {
    PcieNbSbSetupVc (pConfig);
  }
#endif
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLateInitCores Exit\n"));
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/*
 * Set up NB-SB virtual channel for audio traffic
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
VOID
PcieNbSbSetupVc (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  UINT32    VCStatus;
  PCI_ADDR  Port;

  Port = PcieLibGetPortPciAddress (8, pConfig);
  if (PcieSbSetupVc (pConfig) == AGESA_SUCCESS) {
    LibNbPciRMW (Port.AddressValue | NB_PCIP_REG124, AccessS3SaveWidth8, 0x01, 0, pConfig);
    LibNbPciRMW (Port.AddressValue | NB_PCIP_REG130, AccessS3SaveWidth32, (UINT32)~(BIT24 + BIT25 + BIT26), 0xFE + BIT24, pConfig);
    LibNbPciRMW (Port.AddressValue | NB_PCIP_REG130, AccessS3SaveWidth32, 0xffffffff, BIT31, pConfig);
    do {
      STALL (GET_BLOCK_CONFIG_PTR (pConfig), 200, CIMX_S3_SAVE);
      LibNbPciRead (Port.AddressValue | NB_PCIP_REG134, AccessWidth32, &VCStatus, pConfig);
    } while (VCStatus & BIT17);
    PcieSbEnableVc (pConfig);
  }
}


/*----------------------------------------------------------------------------------------*/
/*
 * Late common Port Init
 *
 *
 *  @param[in] PortId           Port Id
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLateCommonPortInit (
  IN     PORT        PortId,
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  Status = AGESA_SUCCESS;

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/*
 * Initiate SW Gen2 switch
 *
 *
 *
 *  @param[in] PortId           Port Id.
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
VOID
PcieInitiateSoftwareGen2 (
  IN     PORT            PortId,
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  UINT8     LinkSpeedCap;
  UINT8     PcieCapPtr;
  UINT8     SecondaryBus;
  UINT32    Value;
  UINT32    Counter;
  PCI_ADDR  Ep;
  PCI_ADDR  Port;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitiateSoftwareGen2 PortId[%d] Enter\n", PortId));
  Counter = 5000;
  Port = PcieLibGetPortPciAddress (PortId, pConfig);
  LibNbPciRead (Port.AddressValue | NB_PCIP_REG19, AccessWidth8, &SecondaryBus, pConfig);
  Ep.AddressValue = 0;
  Ep.Address.Bus = SecondaryBus;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE] SecondaryBus = 0x%x \n", SecondaryBus));
  PcieCapPtr = LibNbFindPciCapability (Ep.AddressValue, PCIE_CAP_ID, pConfig);
  LibNbPciRead (Ep.AddressValue | (PcieCapPtr + 0xC), AccessWidth8, &LinkSpeedCap, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE] PcieCapPtr = 0x%x \n", PcieCapPtr));
  if ((LinkSpeedCap & 0xf) < 2) {
    return;
  }
  PcieLibSetLinkMode (PortId, PcieLinkModeGen2, pConfig);
  LibNbPciIndexRMW (Port.AddressValue  | NB_BIF_INDEX, NB_BIFNBP_REGA4 , AccessS3SaveWidth32, (UINT32)~(BIT18), BIT18 , pConfig);
  do {
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), 200, CIMX_S3_SAVE);
    LibNbPciIndexRead (Port.AddressValue | NB_PCIP_REGE0, NB_BIFNBP_REGA5, AccessWidth32, &Value, pConfig);
  } while ((UINT8)Value != 0x10 && Counter-- != 0);
  LibNbPciIndexRead (Port.AddressValue | NB_PCIP_REGE0, NB_BIFNBP_REGA4, AccessWidth32, &Value, pConfig);
  if ((Value & BIT24) != 0) {
  //Initiate link speed change
    LibNbPciIndexRMW (Port.AddressValue | NB_PCIP_REGE0, NB_BIFNBP_REGA4, AccessS3SaveWidth32, ((UINT32)~BIT7), BIT7, pConfig);
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitiateSoftwareGen2 Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * Validate input parameters configuration for PCie Late Init call.
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieLateValidateConfiguration (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  NB_INFO     NbInfo;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLateValidateConfiguration Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);
  if (pPcieConfig == NULL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0, 0, 0, 0, pConfig);
    CIMX_ASSERT (FALSE);
    return  AGESA_FATAL;
  }
  if (pPcieConfig->sHeader.InitializerID != INITIALIZED_BY_INITIALIZER) {
    PcieLibInitializer (pConfig);
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieLateValidateConfiguration Exit\n"));
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * PcieValidatePortState
 *    Port disable or port visibility control
 *
 *
 * @param[in] NbConfigPtr  Northbridge configuration structure pointer.
 *
 */
AGESA_STATUS
PcieValidatePortState (
  IN     AMD_NB_CONFIG   *NbConfigPtr
  )
{
  AGESA_STATUS  Status;

  Status = AGESA_SUCCESS;
  PcieLibUnHidePorts (NbConfigPtr);
  PcieLibValidatePortStateInit (NbConfigPtr);
  PcieForcePortsVisibleOrDisable (NbConfigPtr);
  PcieLibHidePorts (NbConfigPtr);
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * PciePortsVisibleOrDisable
 *    Set ports always visible or disable based on input parameter
 *
 *
 *
*  @param[in] NbConfigPtr          Northbridge configuration structure pointer.
 *
 */
VOID
PcieForcePortsVisibleOrDisable (
  IN     AMD_NB_CONFIG   *NbConfigPtr
  )
{
  PCIE_CONFIG   *pPcieConfig;
  PORT          PortId;
  PCI_ADDR      Port;

  pPcieConfig = GET_PCIE_CONFIG_PTR (NbConfigPtr);
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PcieLibIsValidPortId (PortId, NbConfigPtr)) {
      Port = PcieLibGetPortPciAddress (PortId, NbConfigPtr);
      if (pPcieConfig->PortConfiguration[PortId].ForcePortDisable == ON ) {
        pPcieConfig->PortConfiguration[PortId].PortPresent = OFF;
        pPcieConfig->PortConfiguration[PortId].PortDetected = OFF;
      }
      if (pPcieConfig->PortConfiguration[PortId].PortAlwaysVisible == ON) {
        LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG70, AccessWidth32, (UINT32)~BIT19, BIT19, NbConfigPtr);
        pPcieConfig->PortConfiguration[PortId].PortPresent = ON;
        pPcieConfig->PortConfiguration[PortId].PortDetected = ON;
      }
      LibNbPciIndexWrite (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG01,  AccessWidth32, (UINT32*)&pPcieConfig->PortConfiguration[PortId], NbConfigPtr);
    }
  }
}

