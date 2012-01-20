/**
 * @file
 *
 * Routines to support Hotplug.
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
#include "amdSbLib.h"

#ifndef  HOTPLUG_SUPPORT_DISABLED

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define ATTN_BUTTON_PRESENT     BIT0
#define PWR_CONTROLLER_PRESENT  BIT1
#define MRL_SENSOR_PRESENT      BIT2
#define ATTN_INDICATOR_PRESENT  BIT3
#define PWR_INDICATOR_PRESENT   BIT4
#define HOTPLUG_SURPRISE        BIT5
#define HOTPLUG_CAPABLE         BIT6

#define ATTN_BUTTON_PRESSED     BIT0
#define PWR_FAULT_DETECTED      BIT1
#define MRL_SENSOR_CHANGED      BIT2
#define PRESENCE_DETECT_CHANGED BIT3
#define COMMAND_COMPLETED       BIT4
#define MRL_SENSOR_STATE        BIT5
#define PRESENCE_DETECT_STATE   BIT6
#define DL_STATE_CHANGED        BIT8

#define PWR_CONTROLLER_CNTL     BIT10
#define NO_COMMAND_COMPLETED_SUPPORTED  BIT18
#define SERVER_HOTPLUG_CAPABILITY \
  (ATTN_BUTTON_PRESENT | PWR_CONTROLLER_PRESENT | ATTN_INDICATOR_PRESENT | PWR_INDICATOR_PRESENT | HOTPLUG_CAPABLE)
#define NATIVE_HOTPLUG_CAPABILITY \
  (HOTPLUG_SURPRISE | HOTPLUG_CAPABLE)

#define SERVER_HOTPLUG  1
#define NATIVE_HOTPLUG  2
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
 * Init Ports Hotplug capability.
 * Initialize hotplug controller init port hotplug capability
 *
 *
 * @param[in] pConfig  Northbridge configuration structure pointer.
 *
 * @retval    AGESA_SUCCESS Hotplug controller successfully initialized.
 * @retval    AGESA_FAIL    Failure during initialization of hotplug controller.
 */
/*----------------------------------------------------------------------------------------*/
UINT32
PcieInitHotplug (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  UINT32        ServerHotplugPortMask;
  PORT          PortId;
  PCI_ADDR      ClkPciAddress;
  NB_INFO       NbInfo;
  PCIE_CONFIG   *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitHotplug Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);
  ServerHotplugPortMask = 0;
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  if (NbInfo.Type == NB_SR5690) {
    for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
      if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON &&
        pPcieConfig->PortConfiguration[PortId].PortHotplug != OFF ) {
        PCI_ADDR  Port;
        Port = PcieLibGetPortPciAddress (PortId, pConfig);
        if (pPcieConfig->PortConfiguration[PortId].PortHotplug == SERVER_HOTPLUG) {
          UINT8  HpDescriptorRegister;
          UINT8  HpDescriptorOffset;
          ServerHotplugPortMask |= 1 << PortId;
          HpDescriptorOffset = (PcieLibGetStaticPortInfo (PcieLibNativePortId (PortId, pConfig), pConfig))->HotplugAddress;
          if (HpDescriptorOffset != 0xff) {
            ServerHotplugPortMask |= 1 << PortId;
            HpDescriptorRegister = (PcieLibGetCoreInfo (PcieLibGetCoreId (PortId, pConfig), pConfig))->HotplugRegister;
            //Enable CLK config
            LibNbEnableClkConfig (pConfig);
            //Setup descriptor
            LibNbPciRMW (
              ClkPciAddress.AddressValue | HpDescriptorRegister ,
              AccessWidth32,
              0xffffffff,
              ((1 << 3) | (pPcieConfig->ExtPortConfiguration[PortId].PortHotplugDevMap << 2) | pPcieConfig->ExtPortConfiguration[PortId].PortHotplugByteMap) << HpDescriptorOffset,
              pConfig
              );
            //Hide CLK config
            LibNbDisableClkConfig (pConfig);
            // Enable power fault
            LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG10, AccessWidth32, (UINT32)~BIT4, BIT4, pConfig);
            //Set up capability. Keep NO_COMMAND_COMPLETED_SUPPORTED (bit 18) to zero
            LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessWidth32, 0xfffbffff, SERVER_HOTPLUG_CAPABILITY, pConfig);
            //Clear Status
            LibNbPciRMW (Port.AddressValue | NB_PCIP_REG72, AccessWidth16, 0xffffffff, 0x11F, pConfig);
          }
        }
        if (pPcieConfig->PortConfiguration[PortId].PortHotplug == NATIVE_HOTPLUG) {
          LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessWidth32, 0xffffffff, NATIVE_HOTPLUG_CAPABILITY, pConfig);
        }
      }
    }
  }
  return ServerHotplugPortMask;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Init Ports Hotplug capability.
 * Initialize hotplug controller init port hotplug capability
 *
 * @param[in] ServerHotplugPortMask  ServerHotplugPortMask
 * @param[in] pConfig  Northbridge configuration structure pointer.
 *
 * @retval    AGESA_SUCCESS Hotplug controller successfully initialized.
 * @retval    AGESA_FAIL    Failure during initialization of hotplug controller.
 */
/*----------------------------------------------------------------------------------------*/
VOID
PcieCheckHotplug (
  IN     UINT32          ServerHotplugPortMask,
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  PORT          PortId;
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieCheckHotplug Enter\n"));
    //Check if Firmware loaded successfully
  for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
    //Check Firmware Loaded successfully
    if ((ServerHotplugPortMask & (1 << PortId)) != 0) {
      UINT32    Count;
      PCI_ADDR  Port;
      UINT16    SlotStatus;

      Count = 30;       //Setup counter for 30ms
      Port = PcieLibGetPortPciAddress (PortId, pConfig);
      do {
        STALL (GET_BLOCK_CONFIG_PTR (pConfig), 1000, 0);
        LibNbPciRead (Port.AddressValue | NB_PCIP_REG72, AccessWidth16, &SlotStatus, pConfig);
      } while ((SlotStatus & (ATTN_BUTTON_PRESSED | PWR_FAULT_DETECTED)) == 0 && --Count != 0);
      CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Hotplug Firmware Init PortId = %d SlotStatus = 0x%x Retry = %d\n", PortId, SlotStatus, Count));
      if ((SlotStatus & PWR_FAULT_DETECTED) != 0 || (SlotStatus & (PWR_FAULT_DETECTED | ATTN_BUTTON_PRESSED)) == 0) {
        REPORT_EVENT (AGESA_ERROR, PCIE_ERROR_HOTPLUG_INIT, PortId, 0, 0, 0, pConfig);
        CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    ERROR!!!Hotplug Firmware Init FAIL\n"));
        LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessWidth32, (UINT32)~SERVER_HOTPLUG_CAPABILITY, 0x0, pConfig);
      } else {
        //Clear Status
        LibNbPciRMW (Port.AddressValue | NB_PCIP_REG72, AccessWidth16, 0xffffffff, 0x11F, pConfig);
        if ((SlotStatus & PRESENCE_DETECT_CHANGED) != 0) {
          //Power on slot
          LibNbPciRMW (Port.AddressValue | NB_PCIP_REG70, AccessWidth16, (UINT32)~PWR_CONTROLLER_CNTL, 0x0, pConfig);
        }
      }
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieCheckHotplug Exit\n"));
}


/*----------------------------------------------------------------------------------------*/
/**
 * Init Ports Hotplug capability.
 * Initialize hotplug controller init port hotplug capability
 *
 *
 * @param[in] pConfig  Northbridge configuration structure pointer.
 *
 * @retval    AGESA_SUCCESS Hotplug controller successfully initialized.
 * @retval    AGESA_FAIL    Failure during initialization of hotplug controller.
 */
/*----------------------------------------------------------------------------------------*/
/*
AGESA_STATUS
PcieInitHotplug (
  IN     AMD_NB_CONFIG   *pConfig
  )
{
  AGESA_STATUS  Status;
  UINT32        ServerHotplugPortMask;
  PORT          PortId;
  PCI_ADDR      ClkPciAddress;
  NB_INFO       NbInfo;
  PCIE_CONFIG   *pPcieConfig;

  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitHotplug Enter\n"));
  pPcieConfig = GET_PCIE_CONFIG_PTR (pConfig);
  NbInfo = LibNbGetRevisionInfo (pConfig);
  ServerHotplugPortMask = 0;
  ClkPciAddress = pConfig->NbPciAddress;
  ClkPciAddress.Address.Function = 1;
  Status = AGESA_SUCCESS;
  if (NbInfo.Type == NB_SR5690) {
    for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
      if (pPcieConfig->PortConfiguration[PortId].PortPresent == ON &&
        pPcieConfig->PortConfiguration[PortId].PortHotplug != OFF ) {
        PCI_ADDR  Port;
        Port = PcieLibGetPortPciAddress (PortId, pConfig);
        if (pPcieConfig->PortConfiguration[PortId].PortHotplug == SERVER_HOTPLUG) {
          UINT8  HpDescriptorRegister;
          UINT8  HpDescriptorOffset;
          ServerHotplugPortMask |= 1 << PortId;
          HpDescriptorOffset = (PcieLibGetStaticPortInfo (PcieLibNativePortId (PortId, pConfig), pConfig))->HotplugAddress;
          if (HpDescriptorOffset != 0xff) {
            ServerHotplugPortMask |= 1 << PortId;
            HpDescriptorRegister = (PcieLibGetCoreInfo (PcieLibGetCoreId (PortId, pConfig), pConfig))->HotplugRegister;
            //Enable CLK config
            LibNbEnableClkConfig (pConfig);
            //Setup descriptor
            LibNbPciRMW (
              ClkPciAddress.AddressValue | HpDescriptorRegister ,
              AccessWidth32,
              0xffffffff,
              ((1 << 3) | (pPcieConfig->ExtPortConfiguration[PortId].PortHotplugDevMap << 2) | pPcieConfig->ExtPortConfiguration[PortId].PortHotplugByteMap) << HpDescriptorOffset,
              pConfig
              );
            //Hide CLK config
            LibNbDisableClkConfig (pConfig);
            // Enable power fault
            LibNbPciIndexRMW (Port.AddressValue | NB_BIF_INDEX, NB_BIFNBP_REG10, AccessWidth32, (UINT32)~BIT4, BIT4, pConfig);
            //Set up capability. Keep NO_COMMAND_COMPLETED_SUPPORTED (bit 18) to zero
            LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessWidth32, 0xfffbffff, SERVER_HOTPLUG_CAPABILITY, pConfig);
            //Clear Status
            LibNbPciRMW (Port.AddressValue | NB_PCIP_REG72, AccessWidth16, 0xffffffff, 0x11F, pConfig);
          }
        }
        if (pPcieConfig->PortConfiguration[PortId].PortHotplug == NATIVE_HOTPLUG) {
          LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessWidth32, 0xffffffff, NATIVE_HOTPLUG_CAPABILITY, pConfig);
        }
      }
    }
    if (ServerHotplugPortMask != 0) {
      UINT32  FirmwareLength;
      FirmwareLength = sizeof (Firmware);
      if (FirmwareLength > 0) {
        UINT32 *pFirmware;
        pFirmware = (UINT32*)FIX_PTR_ADDR (&Firmware[0], NULL);
        //Load firmware
        LibNbMcuControl (AssertReset, pConfig);
        LibNbLoadMcuFirmwareBlock (0x200, (sizeof (Firmware) -  64), pFirmware, pConfig);
        LibNbLoadMcuFirmwareBlock (0xFFC0, 64, &pFirmware[(sizeof (Firmware) -  64) / 4], pConfig);
        LibNbMcuControl (DeAssertReset, pConfig);
      }
    }
    //Check if Firmware loaded successfully
    for (PortId = MIN_PORT_ID; PortId <= MAX_PORT_ID; PortId++) {
      //Check Firmware Loaded successfully
      if ((ServerHotplugPortMask & (1 << PortId)) != 0) {
        UINT32    Count;
        PCI_ADDR  Port;
        UINT16    SlotStatus;

        Count = 30;       //Setup counter for 30ms
        Port = PcieLibGetPortPciAddress (PortId, pConfig);
        do {
          STALL (GET_BLOCK_CONFIG_PTR (pConfig), 1000, 0);
          LibNbPciRead (Port.AddressValue | NB_PCIP_REG72, AccessWidth16, &SlotStatus, pConfig);
        } while ((SlotStatus & (ATTN_BUTTON_PRESSED | PWR_FAULT_DETECTED)) == 0 && --Count != 0);
        CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    Hotplug Firmware Init PortId = %d SlotStatus = 0x%x Retry = %d\n", PortId, SlotStatus, Count));
        if ((SlotStatus & PWR_FAULT_DETECTED) != 0 || (SlotStatus & (PWR_FAULT_DETECTED | ATTN_BUTTON_PRESSED)) == 0) {
          Status = AGESA_ERROR;
          REPORT_EVENT (AGESA_ERROR, PCIE_ERROR_HOTPLUG_INIT, PortId, 0, 0, 0, pConfig);
          CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "    ERROR!!!Hotplug Firmware Init FAIL\n", PortId));
          LibNbPciRMW (Port.AddressValue | NB_PCIP_REG6C, AccessWidth32, ~SERVER_HOTPLUG_CAPABILITY, 0x0, pConfig);
        } else {
          //Clear Status
          LibNbPciRMW (Port.AddressValue | NB_PCIP_REG72, AccessWidth16, 0xffffffff, 0x11F, pConfig);
          if ((SlotStatus & PRESENCE_DETECT_CHANGED) != 0) {
            //Power on slot
            LibNbPciRMW (Port.AddressValue | NB_PCIP_REG70, AccessWidth16, ~PWR_CONTROLLER_CNTL, 0x0, pConfig);
          }
        }
      }
    }
  }
  CIMX_TRACE ((TRACE_DATA (GET_BLOCK_CONFIG_PTR (pConfig), CIMX_NBPCIE_TRACE), "[NBPCIE]PcieInitHotplug Exit. Status[0x%x]\n", Status));
  return  Status;
}
*/
#endif