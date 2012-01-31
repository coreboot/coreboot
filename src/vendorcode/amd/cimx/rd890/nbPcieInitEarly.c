/**
 * @file
 *
 *  PCIe Early initialization.
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
 * PCIE Init for all NB.
 *    Basic programming / EP training. After this call EP are fully operational.
 *
 *
 *
 * @param[in] ConfigPtr   Northbridges configuration block pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
AmdPcieEarlyInit (
  IN OUT   AMD_NB_CONFIG_BLOCK *ConfigPtr
  )
{
  AGESA_STATUS Status;

  Status = LibNbApiCall (PcieEarlyInit, ConfigPtr);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Northbridge PCIE Init.
 *    Basic programming / EP training. After this call EP are fully operational on particular NB.
 *
 *
 *
 * @param[in] NbConfigPtr   Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieEarlyInit (
  IN OUT   AMD_NB_CONFIG    *NbConfigPtr
  )
{
  AGESA_STATUS Status;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieEarlyInit Enter\n"));
  Status = PcieLibInitValidateInput (NbConfigPtr);
  if (Status == AGESA_FATAL) {
    REPORT_EVENT (AGESA_FATAL, GENERAL_ERROR_BAD_CONFIGURATION, 0 , 0, 0, 0, NbConfigPtr);
    CIMX_ASSERT (FALSE);
    return  Status;
  }
  Status = PciePreTrainingInit (NbConfigPtr);
  Status = PcieInitPorts (NbConfigPtr);
  Status = PcieAfterTrainingInit (NbConfigPtr);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (NbConfigPtr), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieEarlyInit Exit [0x%x]\n", Status));
  return  Status;
}



/*----------------------------------------------------------------------------------------*/
/**
 * Misc initialization prior port link training started
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PciePreTrainingInit (
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  CORE        CoreId ;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PciePreTrainingInit Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  //Unhide all ports
  PcieLibUnHidePorts (pConfig);
  if (pPcieConfig->PcieMmioBaseAddress != 0 && pPcieConfig->PcieMmioSize != 0) {
    PcieLibSetPcieMmioBase (pPcieConfig->PcieMmioBaseAddress, pPcieConfig->PcieMmioSize, pConfig);
  }
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    CoreId %d  CoreSetting = 0x%x\n", CoreId, *((UINT32*)&pConfig->pPcieConfig->CoreSetting[CoreId])));
    //if (pPcieConfig->CoreSetting[CoreId].CoreDisabled == OFF) {
      //Configure cores
    if (pPcieConfig->CoreSetting[CoreId].SkipConfiguration == OFF) {
      PcieLibSetCoreConfiguration (CoreId, pConfig);
    }
      //Init core registers
    PcieLibCommonCoreInit (CoreId, pConfig);
    //}
  }
  PcieLibPreTrainingInit (pConfig);
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    CoreId %d  CoreSetting = 0x%x\n", CoreId, *((UINT32*)&pConfig->pPcieConfig->CoreSetting[CoreId])));
    //Init CPL buffer allocation
    //if (pPcieConfig->CoreSetting[CoreId].CoreDisabled == OFF && pPcieConfig->CoreSetting[CoreId].CplBufferAllocation == ON) {
    if (pPcieConfig->CoreSetting[CoreId].CplBufferAllocation == ON) {
      PcieLibCplBufferAllocation (CoreId, pConfig);
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PciePreTrainingInit Exit\n"));
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Misc initialization after port training complete
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieAfterTrainingInit (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  PCIE_CONFIG *pPcieConfig;
  CORE        CoreId;

  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  for (CoreId = 0; CoreId <= MAX_CORE_ID; CoreId++) {
//    if (pPcieConfig->CoreSetting[CoreId].CoreDisabled == OFF) {
      //Configure cores
    PcieLibCoreAfterTrainingInit (CoreId, pConfig);
//    }
  }
  //Hide all Ports
  PcieLibHidePorts (pConfig);
  return  AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Train PCIE Ports
 *
 *
 *
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieInitPorts (
  IN OUT   AMD_NB_CONFIG     *pConfig
  )
{
  AGESA_STATUS  Status;
  PCIE_CONFIG   *pPcieConfig;

  Status = AGESA_SUCCESS;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  if (pPcieConfig->DeviceInitMaskS1 != 0) {
    Status = PcieInitSelectedPorts (pPcieConfig->DeviceInitMaskS1, pConfig);
  }
  if (pPcieConfig->DeviceInitMaskS2 != 0) {
    Status = PcieInitSelectedPorts (pPcieConfig->DeviceInitMaskS2, pConfig);
  }
  return  Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Train PCIE Ports selected for this stage
 *
 *
 *  @param[in] SelectedPortMask Bitmap of port ID selected for training.
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieInitSelectedPorts (
  IN       UINT16          SelectedPortMask,
  IN OUT   AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  PCIE_CONFIG   *pPcieConfig;
  PORT          PortId;
  BOOLEAN       RequestResetDelay;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitSelectedPorts (Ports = 0x%x) Enter\n", SelectedPortMask));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  Status = AGESA_SUCCESS;
  RequestResetDelay = FALSE;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    PortId %d  PortConfiguration = 0x%x ExtPortConfiguration = 0x%x\n", PortId, *((UINT32*)&pPcieConfig->PortConfiguration[PortId]), *((UINT32*)&pPcieConfig->ExtPortConfiguration[PortId])));
    if ((SelectedPortMask & (1 <<  PortId)) != 0) {
      if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON && PcieLibIsValidPortId (PortId, pConfig)) {
        PCIE_LINK_MODE    LinkMode;
        //Deassert slot reset. Bring EP out of reset
        Status = LibNbCallBack (PHCB_AmdPortResetDeassert, 1 << PortId, pConfig);
        if (Status == AGESA_SUCCESS) {
          //STALL (GET_BLOCK_CONFIG_PTR (pConfig), pPcieConfig->ResetToTrainingDelay * 1000, 0);
          RequestResetDelay = TRUE;
        }
        //Init common registers
        PcieLibCommonPortInit (PortId, pConfig);
        //Check if we already have device failure to go to Gen2 before
        if (PcieLibCheckGen2Disabled (PortId, pConfig)) {
          pPcieConfig->PortConfiguration[PortId].PortLinkMode = PcieLinkModeGen1;
          pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis = PcieTxDeemphasis6dB; // this is to workaround Gen2
        }
        //@todo  Add handling for scratch register for PCIE Gen
        switch (pPcieConfig->PortConfiguration[PortId].PortLinkMode) {
        case  PcieLinkModeGen2:
        case  PcieLinkModeGen2AdvertizeOnly:
        case  PcieLinkModeGen1:
          LinkMode = pPcieConfig->PortConfiguration[PortId].PortLinkMode;
          break;
        default:
          LinkMode = PcieLinkModeGen1;
        }
        PcieLibSetLinkMode (PortId, LinkMode, pConfig);
        //Enable Compliance Mode
        if (pPcieConfig->PortConfiguration[PortId].PortCompliance == ON) {
          PcieLibSetLinkCompliance (PortId, pConfig);
        }
      } else {
        //Port disabled
        SelectedPortMask  &= (~(1 <<  PortId));
      }
    }
  }
  if (RequestResetDelay) {
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), pPcieConfig->ResetToTrainingDelay * 1000, 0);
  }
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if ((SelectedPortMask & (1 <<  PortId)) != 0) {
        //Release Port Training
      PcieLibPortTrainingControl (PortId, PcieLinkTrainingRelease, pConfig);
    }
  }
  STALL (GET_BLOCK_CONFIG_PTR (pConfig), pPcieConfig->TrainingToLinkTestDelay * 1000, 0);
  Status = PcieCheckSelectedPorts (SelectedPortMask, pConfig);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitSelectedPorts Exit\n"));
  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Check link state on selected ports.
 *
 *
 *
 *  @param[in] SelectedPortMask Bitmap of port ID selected for training.
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieCheckSelectedPorts (
  IN    UINT16          SelectedPortMask,
  IN    AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS      Status;
  PCIE_CONFIG       *pPcieConfig;
  PORT              PortId;
  UINT16            PortMask;
  UINT16            CurrentPortMask;
  PCIE_LINK_STATUS  PortsLinkStatus[MAX_PORT_ID + 1];

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieCheckSelectedPorts (Ports = 0x%x) Enter\n", SelectedPortMask));
  Status = AGESA_SUCCESS;
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  PortMask = SelectedPortMask;
  // Clear up link state storage
  LibAmdMemFill (PortsLinkStatus, 0, sizeof (PortsLinkStatus), (AMD_CONFIG_PARAMS *)&(pPcieConfig->sHeader));
  // Initial check for link status on all ports
  if (PortMask != 0) {
    PcieGetPortsLinkStatus (PortMask, &PortsLinkStatus[0], pPcieConfig->ReceiverDetectionPooling, pConfig);
  }
  // Check if training on any ports in progress
  PortMask = PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusTrainingInProgress);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    #1 PortMask  = 0x%x\n", PortMask));
  if (PortMask != 0) {
    // Try to recover ports in case of broken lane
    if (PcieBrokenLaneWorkaround (PortMask, pConfig) != AGESA_UNSUPPORTED) {
      // Update port status array
      PortMask |= PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusConnected);
      PcieGetPortsLinkStatus (PortMask, &PortsLinkStatus[0], pPcieConfig->ReceiverDetectionPooling, pConfig);
    }
  }
  // Check if training on any ports still in progress
  CurrentPortMask = PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusTrainingInProgress);
  if (PortMask != CurrentPortMask) {
    REPORT_EVENT (AGESA_WARNING, PCIE_ERROR_BROKEN_LINE, (PortMask^CurrentPortMask)&PortMask, 0, 0, 0, pConfig);
  }
  PortMask = CurrentPortMask;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    #2 PortMask  = 0x%x\n", PortMask));
  if (PortMask != 0) {
    // Try to recover port training by downgrading link speed to Gen1
    if (PcieGen2Workaround (PortMask, pConfig) != AGESA_UNSUPPORTED) {
      PortMask |= PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusConnected);
      PcieGetPortsLinkStatus (PortMask, &PortsLinkStatus[0], pPcieConfig->ReceiverDetectionPooling, pConfig);
    }
  }
  // Check if training on any ports still in progress
  CurrentPortMask = PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusTrainingInProgress);
  if (PortMask != CurrentPortMask) {
    REPORT_EVENT (AGESA_WARNING, PCIE_ERROR_GEN2_FAIL, (PortMask^CurrentPortMask)&PortMask, 0, 0, 0, pConfig);
  }
  PortMask = CurrentPortMask;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    #3 PortMask  = 0x%x\n", PortMask));
  if (PortMask != 0) {
    REPORT_EVENT (AGESA_WARNING, PCIE_ERROR_TRAINING_FAIL, PortMask, 0, 0, 0, pConfig);
    PcieMiscWorkaround (&PortsLinkStatus[0], pConfig);
  }
  //Get bitmap of successfully trained ports
  PortMask = PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusConnected);
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    #4 PortMask  = 0x%x\n", PortMask));
  if (PortMask != 0) {
    // Check if VCO negotiation is completed
    PcieCheckVco (PortMask, &PortsLinkStatus[0], pConfig);
  }
  CurrentPortMask = PcieFindPortsWithLinkStatus (&PortsLinkStatus[0], PcieLinkStatusConnected);
  if (PortMask != CurrentPortMask) {
    REPORT_EVENT (AGESA_WARNING, PCIE_ERROR_VCO_NEGOTIATON, (PortMask^CurrentPortMask)&PortMask, 0, 0, 0, pConfig);
  }
  PortMask = CurrentPortMask;
  //Update status port status info
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if ((SelectedPortMask & (1 <<  PortId)) != 0) {
      PCI_ADDR Port;
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      if (PortsLinkStatus[PortId] == PcieLinkStatusInCompliance) {
        pPcieConfig->PortConfiguration[PortId].PortCompliance = ON;
      } else {
        if (PortsLinkStatus[PortId] == PcieLinkStatusConnected) {
          if (LibNbCallBack (PHCB_AmdPortTrainingCompleted, Port.AddressValue, pConfig) == AGESA_ERROR) {
            PortsLinkStatus[PortId] = 0;
          }
          if (PortsLinkStatus[PortId] == PcieLinkStatusConnected &&
            pPcieConfig->PcieConfiguration.DisableGfxWorkaround == OFF &&
            PcieLibGetPortLinkInfo (PortId, pConfig).MaxLinkWidth >= PcieLinkWidth_x8 &&
            PcieGfxWorkarounds (PortId, pConfig) != AGESA_SUCCESS) {
            //CIMX_ASSERT (FALSE);
            PortsLinkStatus[PortId] = 0;
          }
          if (PortsLinkStatus[PortId] == PcieLinkStatusConnected) {
            pPcieConfig->PortConfiguration[PortId].PortDetected = ON;
            PcieLibSetLinkWidth (PortId, pPcieConfig->ExtPortConfiguration[PortId].PortLinkWidth, pConfig);
          }
        }
        if (pPcieConfig->PortConfiguration[PortId].PortDetected == OFF &&
          pPcieConfig->PortConfiguration[PortId].PortHotplug == OFF) {
            //Port training on Hold if Link in not connected and not in compliance
          PcieLibPortTrainingControl (PortId, PcieLinkTrainingHold, pConfig);
        }
      }
      if (pPcieConfig->PortConfiguration[PortId].PortDetected == OFF ||
          pPcieConfig->PortConfiguration[PortId].PortHotplug == ON) {
        // For all port without devices and hotplug ports
        LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG70, AccessWidth32, (UINT32)~BIT19, BIT19, pConfig);
      }
      LibNbPciIndexWrite (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG01,  AccessWidth32, (UINT32*)&pPcieConfig->PortConfiguration[PortId], pConfig);
      LibNbPciWrite (Port.AddressValue | NB_PCIP_REG108, AccessWidth32, (UINT32*)&pPcieConfig->ExtPortConfiguration[PortId], pConfig);
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieCheckSelectedPorts Exit\n"));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Workaround for broken TX line.
 *
 *
 *
 *  @param[in] SelectedPortMask Bitmap of port ID selected for training.
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieBrokenLaneWorkaround (
  IN    UINT16          SelectedPortMask,
  IN    AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  PORT          PortId;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieBrokenLaneWorkaround Enter\n"));
  Status = AGESA_UNSUPPORTED;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if ((SelectedPortMask & (1 <<  PortId)) != 0) {
      LINK_INFO LinkInfo = PcieLibGetPortLinkInfo (PortId, pConfig);
      if (LinkInfo.MaxLinkWidth > PcieLinkWidth_x1 && LinkInfo.LinkWidth < LinkInfo.MaxLinkWidth) {
        PcieLibPowerOffPortLanes (PortId, LinkInfo.LinkWidth, pConfig);
        if (PcieLibResetSlot (PortId, pConfig) == AGESA_SUCCESS) {
          Status = AGESA_SUCCESS;
        }
      }
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieBrokenLaneWorkaround Exit\n"));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Workaround for device violating Gen2 spec.
 *      Downgrade link speed to Gen1.
 *
 *
 *
 *  @param[in] SelectedPortMask   Bitmap of port ID selected for training.
 *  @param[in] pConfig            Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieGen2Workaround (
  IN    UINT16          SelectedPortMask,
  IN    AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  PCIE_CONFIG   *pPcieConfig;
  PORT          PortId;
  BOOLEAN       RequestPciReset;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieGen2Workaround Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  RequestPciReset = FALSE;
  Status = AGESA_UNSUPPORTED;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if ((SelectedPortMask & (1 <<  PortId)) != 0) {
      if (pPcieConfig->PortConfiguration[PortId].PortLinkMode == PcieLinkModeGen2 ||
          pPcieConfig->PortConfiguration[PortId].PortLinkMode == PcieLinkModeGen2AdvertizeOnly ||
          pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis == PcieTxDeemphasis3p5dB) {
        //Degrade link speed to Gen1
        pPcieConfig->ExtPortConfiguration[PortId].PortDeemphasis = PcieTxDeemphasis6dB;
        PcieLibSetLinkMode (PortId, PcieLinkModeGen1, pConfig);
        PcieLibSetGen2Disabled (PortId, pConfig);
        if (PcieLibResetSlot (PortId, pConfig) != AGESA_SUCCESS) {
          //Slot reset  logic not supported request PCI reset.
          RequestPciReset = TRUE;
        }
        //Report back to caller that potential downgrade case is detected.
        Status = AGESA_SUCCESS;
      }
    }
    if (RequestPciReset) {
      PcieLibRequestPciReset (pConfig);
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieGen2Workaround Enter\n"));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Try to recover system by issuing system wide PCI reset.
 *
 *
 *
 *  @param[in] PortsLinkStatus  Array of link status for every Port
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieMiscWorkaround (
  IN      PCIE_LINK_STATUS  *PortsLinkStatus,
  IN      AMD_NB_CONFIG     *pConfig
  )
{
  AGESA_STATUS  Status;
  PORT          PortId;
  UINT16        PortMask;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieMiscWorkaround Enter\n"));
  Status = AGESA_UNSUPPORTED;
  PortMask = PcieFindPortsWithLinkStatus (PortsLinkStatus, PcieLinkStatusTrainingInProgress);
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if ((PortMask & (1 <<  PortId)) != 0) {
      if (PcieLibRequestPciReset (pConfig)!= AGESA_SUCCESS) {
        break;
      }
      PortMask = PcieFindPortsWithLinkStatus (PortsLinkStatus, PcieLinkStatusTrainingInProgress);
      if (PortMask == 0) {
        break;
      }
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieMiscWorkaround Exit\n"));
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Check VCO negotiation complete.
 *    Routine will retry retrain device infinitely if VCO negotiation is failing.
 *
 *
 *  @param[in] SelectedPortMask Bitmap of port ID selected for training.
 *  @param[in] PortsLinkStatus  Array of link status for every Port
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
AGESA_STATUS
PcieCheckVco (
  IN       UINT16             SelectedPortMask,
  IN       PCIE_LINK_STATUS   *PortsLinkStatus,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  AGESA_STATUS  Status;
  UINT16        VcoNegotiationInProgressPortMask;
  PORT          PortId;
  UINT16        VcoStatus;
  UINT32        LinkRetrainCount;
  UINT32        VcoPoll;
  UINT32        Value;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]VcoNegotiationInProgress Enter\n"));
  Status = AGESA_SUCCESS;
  VcoNegotiationInProgressPortMask = SelectedPortMask;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (VcoNegotiationInProgressPortMask & (1 <<  PortId)) {
      // For each port where VCO needs to be checked
      PCI_ADDR  Port;
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      PortsLinkStatus[PortId] = PcieLinkStatusVcoNegotiationInProgress;
      for (LinkRetrainCount = 0; LinkRetrainCount < 10; LinkRetrainCount++) {
        // Poll for 200 ms for VC0 negotioation completion
        for (VcoPoll = 0; VcoPoll < 200; VcoPoll++) {
          LibNbPciRead (Port.AddressValue | NB_PCIP_REG12A, AccessWidth16, &VcoStatus, pConfig);
          if ((VcoStatus & BIT1) != 0) {
            STALL (GET_BLOCK_CONFIG_PTR (pConfig), 1000, 0);
          } else {
            PortsLinkStatus[PortId] = PcieLinkStatusConnected;
            break;
          }
        }  //For each VcoPoll
        if (PortsLinkStatus[PortId] == PcieLinkStatusVcoNegotiationInProgress) {
          CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_MISC), "   Vco Not Completed. Retrain link on PortId %d\n", PortId));
          LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, pConfig);
          Value = (Value & 0xfffffe80) | ((Value & 0x70) >> 4) | BIT8;
          LibNbPciIndexWrite (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA2, AccessWidth32, &Value, pConfig);
        } else {
          break;    //Vco negotiations complete
        }
      }  //For each LinkRetrainCount
      if (PortsLinkStatus[PortId] == PcieLinkStatusVcoNegotiationInProgress) {
        PortsLinkStatus[PortId] = PcieLinkStatusNotConnected;
      }
    } // Vco negotiations required
  }  //For each port
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]VcoNegotiationInProgress Exit\n"));
  return  Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Get bit map of ports with particular link status
 *
 *
 *
 *  @param[in] PortLinkStatus   Pointer to array of link status for every Port
 *  @param[in] LinkStatus       LinkStatus to search for.
 *
 */
/*----------------------------------------------------------------------------------------*/
UINT16
PcieFindPortsWithLinkStatus (
  IN       PCIE_LINK_STATUS  *PortLinkStatus,
  IN       PCIE_LINK_STATUS   LinkStatus
  )
{
  UINT16  PortMask;
  PORT    PortId;

  PortMask = 0;
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    if (PortLinkStatus[PortId] == LinkStatus) PortMask |= (1 << PortId);
  }
  return PortMask;
}
/*----------------------------------------------------------------------------------------*/
/**
 * Gather link state for selected ports.
 *
 *
 *  @param[in] SelectedPortMask Bitmap of port ID selected for training.
 *  @param[in] PortLinkStatus   Pointer to array of link status for every Port
 *  @param[in] Pooling          Time in MS to pool for link status change.
 *  @param[in] pConfig          Northbridge configuration structure pointer.
 *
 */
/*----------------------------------------------------------------------------------------*/
PCIE_LINK_STATUS
PcieGetPortsLinkStatus (
  IN       UINT16             SelectedPortMask,
  IN OUT   PCIE_LINK_STATUS   *PortLinkStatus,
  IN       UINT32             Pooling,
  IN       AMD_NB_CONFIG      *pConfig
  )
{
  PCIE_LINK_STATUS  Status;
  PORT              PortId;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieGetPortsLinkStatus Enter\n"));
  Status = PcieLinkStatusNotConnected;
  Pooling *= 10;
  while (Pooling-- != 0 && Status != PcieLinkStatusConnected) {
    Status = PcieLinkStatusConnected;                                            //Set up initial overall state as connected
    for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
      //Work only on selected ports
      if ((SelectedPortMask & (1 <<  PortId)) != 0) {
        PCI_ADDR  Port;
        UINT32    LinkState;
        Port = PcieLibGetPortPciAddress (PortId, pConfig);    //Get PCI address of this port
        //Get link state
        LibNbPciIndexRead (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REGA5, AccessWidth32, &LinkState, pConfig);
        LinkState &= 0x3F;
        //CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_MISC), "    PortId %d LinkState = 0x%x \n", PortId, LinkState));
        printk(BIOS_INFO, "[NBPCIE]  PortId %02d LinkState = 0x%x \n", PortId, LinkState);
        //Check if link in L0 state

        if (LinkState == 0x10) {
          PortLinkStatus[PortId] = PcieLinkStatusConnected;
        } else {
          Status = PcieLinkStatusNotConnected;
          //Check if link in compliance mode
          if (LinkState == 0x7) {
            PortLinkStatus[PortId] = PcieLinkStatusInCompliance;
          } else {
            //Check if we passed receiver detection. It will indicate that device present.
            if (LinkState > 0x4) {
              PortLinkStatus[PortId] = PcieLinkStatusTrainingInProgress;
            }
          }
        }
      }
    }
    STALL (GET_BLOCK_CONFIG_PTR (pConfig), 100, 0);
  }
  return Status;
}
