/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Gpp controller
 *
 * Init Gpp Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Ids.h"
#include "Filecode.h"
#define FILECODE PROC_FCH_PCIE_GPPLATE_FILECODE

//
// Declaration of local functions
//


/**
 * FchGppSetAspm - Set GPP ASPM
 *
 *
 * @param[in] PciAddress PCI Address.
 * @param[in] LxState    Lane State.
 * @param[in] StdHeader
 *
 */
STATIC VOID
FchGppSetAspm (
  IN  UINT32                    PciAddress,
  IN  UINT8                     LxState,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT8      PcieCapOffset;
  UINT8      DeviceType;

  PcieCapOffset = FchFindPciCap (PciAddress, PCIE_CAP_ID, StdHeader);

  if (PcieCapOffset) {
    //
    // Read link capabilities register (0x0C[11:10] - ASPM support)
    //
    ReadPci (PciAddress + PcieCapOffset + 0x0D, AccessWidth8, &DeviceType, StdHeader);
    if (DeviceType & BIT2) {
      DeviceType = (DeviceType >> 2) & (BIT1 + BIT0);
      //
      // Set ASPM state in link control register
      //
      RwPci (PciAddress + PcieCapOffset + 0x10, AccessWidth8, 0xffffffff, LxState & DeviceType, StdHeader);
    }
  }
}

/**
 * FchGppSetEpAspm - Set EP ASPM
 *
 *
 * @param[in] PciAddress PCI Address.
 * @param[in] LxState    Lane State.
 * @param[in] StdHeader
 *
 */
STATIC VOID
FchGppSetEpAspm (
  IN  UINT32                    PciAddress,
  IN  UINT8                     LxState,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT8      DeviceType;
  UINT8      MaxFuncs;
  UINT32     DevBDF;

  MaxFuncs = 1;
  ReadPci (PciAddress + 0x0E, AccessWidth8, &DeviceType, StdHeader);

  if (DeviceType & BIT7) {
    MaxFuncs = 8;                                                                   /// multi-function device
  }

  while (MaxFuncs != 0) {
    DevBDF = PciAddress + (UINT32) ((MaxFuncs - 1) << 16);
    FchGppSetAspm (DevBDF, LxState, StdHeader);
    MaxFuncs--;
  }
}

/**
 * FchGppValidateAspm - Validate EndPoint support for GPP ASPM
 *
 *
 * @param[in] PciAddress PCI Address.
 * @param[in] LxState    Lane State.
 * @param[in] StdHeader
 *
 */
STATIC VOID
FchGppValidateAspm (
  IN  UINT32                    PciAddress,
  IN  UINT8                     *LxState,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT8      PcieCapOffset;
  UINT8      DeviceType;
  UINT8      MaxFuncs;
  UINT32     DevBDF;

  MaxFuncs = 1;
  ReadPci (PciAddress + 0x0E, AccessWidth8, &DeviceType, StdHeader);

  if (DeviceType & BIT7) {
    MaxFuncs = 8;                                                                   /// multi-function device
  }

  while (MaxFuncs != 0) {
    DevBDF = PciAddress + (UINT32) ((MaxFuncs - 1) << 16);
    PcieCapOffset = FchFindPciCap (DevBDF, PCIE_CAP_ID, StdHeader);

    if (PcieCapOffset) {
      //
      // Read link capabilities register (0x0C[11:10] - ASPM support)
      //
      ReadPci (DevBDF + PcieCapOffset + 0x0D, AccessWidth8, &DeviceType, StdHeader);
      if (DeviceType & BIT2) {
        DeviceType = (DeviceType >> 2) & (BIT1 + BIT0);
        //
        // Update ASPM state as what endpoint support
        //
        *LxState &= DeviceType;
      }
    }
    MaxFuncs--;
  }
}


/**
 * FchInitLateGpp - Prepare Gpp controller to boot to OS.
 *
 *  PcieGppLateInit
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateGpp (
  IN       VOID     *FchDataPtr
  )
{
  UINT8                  PortId;
  UINT8                  BusNum;
  UINT8                  PortAspmValue;
  UINT8                  AllowStrapControlByAB;
  UINT8                  GppS3Data;
  FCH_GPP_PORT_CONFIG    *PortCfg;
  UINT32                 PciAspmValue;
  UINT32                 AbValue;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Disable hidden register decode and serial number capability
  //
  AbValue = ReadAlink (FCH_ABCFG_REG330 | (UINT32) (ABCFG << 29), StdHeader);
  WriteAlink (FCH_ABCFG_REG330 | (UINT32) (ABCFG << 29), AbValue & ~(BIT26 + BIT10), StdHeader);
  //
  // Configure ASPM & Save GPP port status into CMOS
  //
  AllowStrapControlByAB = 0x01;
  GppS3Data = 0x00;

  for ( PortId = 0; PortId < MAX_GPP_PORTS; PortId++ ) {
    //
    // write pci_reg3d with 0x01 to fix yellow mark for GPP bridge under some OS
    // when native PCIE is enabled but MSI is not available
    // BIF/GPP allowing strap STRAP_BIF_INTERRUPT_PIN_SB controlled by AB reg
    //
    PortCfg = &LocalCfgPtr->Gpp.PortCfg[PortId];
    if (PortCfg->PortDetected) {
      GppS3Data |= 1 << (PortId + 4);
      if (PortCfg->PortIsGen2 == FALSE) {
        GppS3Data |= 1 << (PortId);
      }
    }
    if (PortCfg->PortHotPlug) {
      RwPci (PCI_ADDRESS (0, 21, PortId, 0x04), AccessWidth8, 0xFE, 0x00, StdHeader);         ///clear IO enable to fix possible hotplug hang
    }

    WritePci (PCI_ADDRESS (0, 21, PortId, 0x3d), AccessWidth8, &AllowStrapControlByAB, StdHeader);
    ReadPci (PCI_ADDRESS (0, 21, PortId, 0x19), AccessWidth8, &BusNum, StdHeader);

    if (BusNum != 0xFF) {
      ReadPci (PCI_ADDRESS (BusNum, 0, 0, 0x00), AccessWidth32, &PciAspmValue, StdHeader);
      if (PciAspmValue != 0xffffffff) {
        PortAspmValue = LocalCfgPtr->Gpp.GppPortAspm;
        //
        // Validate ASPM support on EP side
        //
        FchGppValidateAspm (PCI_ADDRESS (BusNum, 0, 0, 0), &PortAspmValue, StdHeader);
        //
        // Set ASPM on EP side
        //
        FchGppSetEpAspm (PCI_ADDRESS (BusNum, 0, 0, 0), PortAspmValue, StdHeader);
        //
        // Set ASPM on port side
        //
        FchGppSetAspm (PCI_ADDRESS (0, 21, PortId, 0), PortAspmValue, StdHeader);
      }
    }
    RwAlink ((FCH_RCINDXP_REG02 | (UINT32) (PortId << 24)), (UINT32)~BIT15, BIT15, StdHeader);
  }
  RwAlink (FCH_RCINDXC_REG02, (UINT32)~BIT0, BIT0, StdHeader);

  if ( LocalCfgPtr->Gpp.GppPhyPllPowerDown == TRUE ) {
    //
    // Power Saving Feature for GPP Lanes
    //
    // Set PCIE_P_CNTL in Alink PCIEIND space
    //
    AbValue = ReadAlink (FCH_RCINDXC_REG40, StdHeader);
    AbValue |= BIT12 + BIT0;
    AbValue &= ~(BIT9 + BIT4);
    WriteAlink (FCH_RCINDXC_REG40, AbValue, StdHeader);
    RwAlink (FCH_RCINDXC_REG02, (UINT32)~(BIT8 + BIT3), BIT8 + BIT3, StdHeader);
    GppGen2Workaround (&LocalCfgPtr->Gpp, StdHeader);
  }

  //
  // Configure Lock HWInit registers
  //
  AbValue = ReadAlink (FCH_ABCFG_REGC0 | (UINT32) (ABCFG << 29), StdHeader);
  if (AbValue & 0xF0) {
    AbValue = ReadAlink (FCH_RCINDXC_REG10, StdHeader);
    WriteAlink (FCH_RCINDXC_REG10, AbValue | BIT0, StdHeader);    /// Set HWINIT_WR_LOCK
  }

  //
  // Restore strap0 via override
  //
  if (LocalCfgPtr->Gpp.PcieAer) {
    RwAlink (0x310  | (UINT32) (ABCFG << 29), 0xFFFFFFFF, BIT7, StdHeader);
    RwAlink (FCH_RCINDXC_REGC0, 0xFFFFFFFF, BIT9, StdHeader);
  }
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D, AccessWidth8, 0, GppS3Data);
}

