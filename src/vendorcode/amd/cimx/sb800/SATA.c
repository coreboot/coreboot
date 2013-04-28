
/**
 * @file
 *
 * Config Southbridge SATA controller
 *
 * Init SATA features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-SB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
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

#include "SBPLATFORM.h"
#include "cbtypes.h"

//
// Declaration of local functions
//
VOID  sataSetIrqIntResource (IN AMDSBCFG* pConfig);
VOID  sataBar5setting (IN AMDSBCFG* pConfig, IN UINT32 *pBar5);
VOID  shutdownUnconnectedSataPortClock (IN AMDSBCFG* pConfig, IN UINT32 ddBar5);
VOID  sataDriveDetection (IN AMDSBCFG* pConfig, IN UINT32 *pBar5);

/**
 * sataSetIrqIntResource - Config SATA IRQ/INT# resource
 *
 *
 *   - Private function
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sataSetIrqIntResource (
  IN       AMDSBCFG*   pConfig
  )
{
  UINT8   dbValue;
  // IRQ14/IRQ15 come from IDE or SATA
  dbValue = 0x08;
  WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &dbValue);
  ReadIO (SB_IOMAP_REGC01, AccWidthUint8, &dbValue);
  dbValue = dbValue & 0x0F;
  if (pConfig->SataClass == 3) {
    dbValue = dbValue | 0x50;
  } else {
    if (pConfig->SataIdeMode == 1) {
      // Both IDE & SATA set to Native mode
      dbValue = dbValue | 0xF0;
    }
  }
  WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &dbValue);
}

/**
 * sataBar5setting - Config SATA BAR5
 *
 *   - Private function
 *
 * @param[in] pConfig - Southbridge configuration structure pointer.
 * @param[in] *pBar5   - SATA BAR5 buffer.
 *
 */
VOID
sataBar5setting (
  IN       AMDSBCFG*   pConfig,
  IN       UINT32      *pBar5
  )
{
  //Get BAR5 value
  ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, pBar5);
  //Assign temporary BAR if is not already assigned
  if ( (*pBar5 == 0) || (*pBar5 == - 1) ) {
    //assign temporary BAR5
    if ( (pConfig->TempMMIO == 0) || (pConfig->TempMMIO == - 1) ) {
      *pBar5 = 0xFEC01000;
    } else {
      *pBar5 = pConfig->TempMMIO;
    }
    WritePCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG24), AccWidthUint32, pBar5);
  }
  //Clear Bits 9:0
  *pBar5 = *pBar5 & 0xFFFFFC00;
}
/**
 * shutdownUnconnectedSataPortClock - Shutdown unconnected Sata port clock
 *
 *   - Private function
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 * @param[in] ddBar5 Sata BAR5 base address.
 *
 */
VOID
shutdownUnconnectedSataPortClock (
  IN       AMDSBCFG* pConfig,
  IN       UINT32 ddBar5
  )
{
  UINT8  dbPortNum;
  UINT8  dbPortSataStatus;
  UINT8  NumOfPorts;
  UINT8  cimSataClkAutoOff;

  cimSataClkAutoOff = (UINT8) pConfig->SataClkAutoOff;
#if  SB_CIMx_PARAMETER == 0
  cimSataClkAutoOff = cimSataClkAutoOffDefault;
#endif
  NumOfPorts = 0;
  if ( cimSataClkAutoOff == TRUE ) {
    for ( dbPortNum = 0; dbPortNum < 6; dbPortNum++ ) {
      ReadMEM (ddBar5 + SB_SATA_BAR5_REG128 + (dbPortNum * 0x80), AccWidthUint8, &dbPortSataStatus);
      // Shutdown the clock for the port and do the necessary port reporting changes.
      // ?? Error port status should be 1 not 3
      if ( ((dbPortSataStatus & 0x0F) != 0x03) && (! ((pConfig->SataEspPort) & (1 << dbPortNum))) ) {
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, 0xFF, (1 << dbPortNum));
        RWMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, ~(1 << dbPortNum), 00);
      }
    }     //end of for (dbPortNum=0;dbPortNum<6;dbPortNum++)
    ReadMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, &dbPortSataStatus);
    //if all ports are in disabled state, report atleast one port
    if ( (dbPortSataStatus & 0x3F) == 0) {
      RWMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, (UINT32) ~(0x3F), 01);
    }
    ReadMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, &dbPortSataStatus);
    for (dbPortNum = 0; dbPortNum < 6; dbPortNum ++) {
      if (dbPortSataStatus & (1 << dbPortNum)) {
        NumOfPorts++;
      }
    }
    if ( NumOfPorts == 0) {
      NumOfPorts = 0x01;
    }
    RWMEM (ddBar5 + SB_SATA_BAR5_REG00, AccWidthUint8, 0xE0, NumOfPorts - 1);
  }  //end of SataClkAuto Off option
}

/**
 * Table for class code of SATA Controller in different modes
 *
 *
 *
 *
 */
const static UINT32 sataIfCodeTable[] =
{
  0x01018F40, //sata class ID of IDE
  0x01040040, //sata class ID of RAID
  0x01060140, //sata class ID of AHCI
  0x01018A40, //sata class ID of Legacy IDE
  0x01018F40, //sata class ID of IDE to AHCI mode
};

/**
 * Table for device id of SATA Controller in different modes
 *
 *
 *
 *
 */
const static UINT16 sataDeviceIDTable[] =
{
  0x4390,  //sata device ID of IDE
  0x4392,  //sata device ID of RAID
  0x4391,  //sata class ID of AHCI
  0x4390,  //sata device ID of Legacy IDE
  0x4390,  //sata device ID of IDE->AHCI mode
};

/**
 * Table for Sata Phy Fine Setting
 *
 *
 *
 *
 */
const static SATAPHYSETTING sataPhyTable[] =
{
  {0x3006, 0x0056A607},
  {0x2006, 0x00061400},
  {0x1006, 0x00061302},

  {0x3206, 0x0056A607},
  {0x2206, 0x00061400},
  {0x1206, 0x00061302},

  {0x3406, 0x0056A607},
  {0x2406, 0x00061402},
  {0x1406, 0x00064300},

  {0x3606, 0x0056A607},
  {0x2606, 0x00061402},
  {0x1606, 0x00064300},

  {0x3806, 0x0056A700},
  {0x2806, 0x00061502},
  {0x1806, 0x00064302},

  {0x3A06, 0x0056A700},
  {0x2A06, 0x00061502},
  {0x1A06, 0x00064302}
};

/**
 * sataInitBeforePciEnum - Config SATA controller before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sataInitBeforePciEnum (
  IN       AMDSBCFG*   pConfig
  )
{
  UINT32  ddTempVar;
  UINT32  ddValue;
  UINT32  *tempptr;
  UINT16  *pDeviceIdptr;
  UINT32  dwDeviceId;
  UINT8   dbValue;
  UINT8   pValue;
  UINT16    i;
  SATAPHYSETTING  *pPhyTable;

  ddTempVar = NULL;
  // BIT0 Enable write access to PCI header (reg 08h-0Bh) by setting SATA PCI register 40h
  // BIT4: Disable fast boot
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT0 + BIT2 + BIT4);
  // BIT0 Enable write access to PCI header (reg 08h-0Bh) by setting IDE PCI register 40h
  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT0);
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8 | S3_SAVE, 0, pConfig->SataPortPower);
  dbValue = (UINT8)pConfig->SataClass;
  if (dbValue == AHCI_MODE_4394) {
    dbValue = AHCI_MODE;
  }
  if (dbValue == IDE_TO_AHCI_MODE_4394) {
    dbValue = IDE_TO_AHCI_MODE;
  }
  // Disable PATA MSI
  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG34), AccWidthUint8 | S3_SAVE, 0x00, 0x00);
  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG06), AccWidthUint8 | S3_SAVE, 0xEF, 0x00);

  // Get the appropriate class code from the table and write it to PCI register 08h-0Bh
  // Set the appropriate SATA class based on the input parameters
  // SATA IDE Controller Class ID & SSID
  tempptr = (UINT32 *) FIXUP_PTR (&sataIfCodeTable[0]);
  if ( (pConfig->SataIdeMode == 1) && (pConfig->SataClass != 3) ) {
    ddValue = tempptr[0];
    // Write the class code to IDE PCI register 08h-0Bh
    RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG08), AccWidthUint32 | S3_SAVE, 0, ddValue);
  }
  ddValue = tempptr[dbValue];
  // Write the class code to SATA PCI register 08h-0Bh
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG08), AccWidthUint32 | S3_SAVE, 0, ddValue);
  if ( pConfig->SataClass == LEGACY_IDE_MODE ) {
    //Set PATA controller to native mode
    RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG09), AccWidthUint8 | S3_SAVE, 0x00, 0x08F);
  }
  if (pConfig->BuildParameters.IdeSsid != NULL ) {
    RWPCI ((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.IdeSsid);
  }
  // SATA Controller Class ID & SSID
  pDeviceIdptr = (UINT16 *) FIXUP_PTR (&sataDeviceIDTable[0]);
  if ( pConfig->BuildParameters.SataIDESsid != NULL ) {
    ddTempVar = pConfig->BuildParameters.SataIDESsid;
  }
  dwDeviceId = pDeviceIdptr[dbValue];
  if ( pConfig->SataClass == RAID_MODE) {
    if ( pConfig->BuildParameters.SataRAID5Ssid != NULL ) {
      ddTempVar = pConfig->BuildParameters.SataRAID5Ssid;
    }
    dwDeviceId = V_SB_SATA_RAID5_DID;
    pValue = SATA_EFUSE_LOCATION;
    getEfuseStatus (&pValue);
    if (( pValue & SATA_EFUSE_BIT ) || ( pConfig->SataForceRaid == 1 )) {
      dwDeviceId = V_SB_SATA_RAID_DID;
      if ( pConfig->BuildParameters.SataRAIDSsid != NULL ) {
        ddTempVar = pConfig->BuildParameters.SataRAIDSsid;
      }
    }
  }
  if ( ((pConfig->SataClass) == AHCI_MODE) || ((pConfig->SataClass) == IDE_TO_AHCI_MODE) ||
    ((pConfig->SataClass) == AHCI_MODE_4394) || ((pConfig->SataClass) == IDE_TO_AHCI_MODE_4394) ) {
    if ( pConfig->BuildParameters.SataAHCISsid != NULL ) {
      ddTempVar = pConfig->BuildParameters.SataAHCISsid;
    }
  }
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, dwDeviceId);
  if ( ddTempVar != NULL ) {
    RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG2C, AccWidthUint32 | S3_SAVE, 0x00, ddTempVar);
  }
  // SATA IRQ Resource
  sataSetIrqIntResource (pConfig);

  // 8.4 SATA PHY Programming Sequence
  pPhyTable = (SATAPHYSETTING*)FIXUP_PTR (&sataPhyTable[0]);
  for (i = 0; i < (sizeof (sataPhyTable) / sizeof (SATAPHYSETTING)); i++) {
    RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG84, AccWidthUint16 | S3_SAVE, ~(BIT1 + BIT2 + BIT9 + BIT10 + BIT11 + BIT12 + BIT13 + BIT14), pPhyTable->wPhyCoreControl);
    RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG94, AccWidthUint32 | S3_SAVE, 0x00, pPhyTable->dwPhyFineTune);
    ++pPhyTable;
  }

//   CallBackToOEM (SATA_PHY_PROGRAMMING, NULL, pConfig);

  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
  // Disable write access to PCI header
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
}

/**
 * sataInitAfterPciEnum - Config SATA controller after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sataInitAfterPciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddAndMask;
  UINT32  ddOrMask;
  UINT32  ddBar5;
  UINT8  dbVar;
  UINT8  dbPortNum;
  UINT8  dbEfuse;
  UINT8  dbPortMode;
  UINT16  SataPortMode;
  UINT8  cimSataAggrLinkPmCap;
  UINT8  cimSataPortMultCap;
  UINT8  cimSataPscCap;
  UINT8  cimSataSscCap;
  UINT8  cimSataFisBasedSwitching;
  UINT8  cimSataCccSupport;

  cimSataAggrLinkPmCap = (UINT8) pConfig->SataAggrLinkPmCap;
  cimSataPortMultCap = (UINT8) pConfig->SataPortMultCap;
  cimSataPscCap = (UINT8) pConfig->SataPscCap;
  cimSataSscCap = (UINT8) pConfig->SataSscCap;
  cimSataFisBasedSwitching = (UINT8) pConfig->SataFisBasedSwitching;
  cimSataCccSupport = (UINT8) pConfig->SataCccSupport;

#if  SB_CIMx_PARAMETER == 0
  cimSataAggrLinkPmCap = cimSataAggrLinkPmCapDefault;
  cimSataPortMultCap = cimSataPortMultCapDefault;
  cimSataPscCap = cimSataPscCapDefault;
  cimSataSscCap = cimSataSscCapDefault;
  cimSataFisBasedSwitching = cimSataFisBasedSwitchingDefault;
  cimSataCccSupport = cimSataCccSupportDefault;
#endif

  ddAndMask = 0;
  ddOrMask = 0;
  ddBar5 = 0;
  if ( pConfig->SATAMODE.SataMode.SataController == 0 ) {
    return;   //return if SATA controller is disabled.
  }

  //Enable write access to pci header, pm capabilities
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xFF, BIT0);
  //Disable AHCI Prefetch function
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8 | S3_SAVE, 0x7F, BIT7);

  sataBar5setting (pConfig, &ddBar5);

  ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8,0xFF, 0x03);   //memory and io access enable
  dbEfuse = SATA_FIS_BASE_EFUSE_LOC;
  getEfuseStatus (&dbEfuse);

  if ( !cimSataPortMultCap ) {
    ddAndMask |= BIT12;
  }
  if ( cimSataAggrLinkPmCap ) {
    ddOrMask |= BIT11;
  } else {
    ddAndMask |= BIT11;
  }
  if ( cimSataPscCap ) {
    ddOrMask |= BIT1;
  }
  if ( cimSataSscCap ) {
    ddOrMask |= BIT26;
  }
  if ( cimSataFisBasedSwitching ) {
    if (dbEfuse & BIT1) {
      ddAndMask |= BIT10;
    } else {
      ddOrMask |= BIT10;
    }
  } else {
    ddAndMask |= BIT10;
  }
  // RPR 8.10 Disabling CCC (Command Completion Coalescing) support.
  if ( cimSataCccSupport ) {
    ddOrMask |= BIT19;
  } else {
    ddAndMask |= BIT19;
  }
  RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint32 | S3_SAVE, ~ddAndMask, ddOrMask);


  // SATA ESP port setting
  // These config bits are set for SATA driver to identify which ports are external SATA ports and need to
  // support hotplug. If a port is set as an external SATA port and need to support hotplug, then driver will
  // not enable power management (HIPM & DIPM) for these ports.
  if ( pConfig->SataEspPort != 0 ) {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(pConfig->SataEspPort), 0);
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(BIT12 + BIT13 + BIT14 + BIT15 + BIT16 + BIT17 + BIT5 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0), (pConfig->SataEspPort << 12));
    // RPR 8.7 External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint32 | S3_SAVE, ~(BIT20), BIT20);
  } else {
    // RPR 8.7 External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set (Clear for no ESP port)
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(BIT5 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0), 0x00);
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint32 | S3_SAVE, ~(BIT20), 0x00);
  }
  if ( cimSataFisBasedSwitching ) {
    if (dbEfuse & BIT1) {
      RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(BIT22 + BIT23 + BIT24 + BIT25 + BIT26 + BIT27), 0x00);
    } else {
      RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(BIT22 + BIT23 + BIT24 + BIT25 + BIT26 + BIT27), (BIT22 + BIT23 + BIT24 + BIT25 + BIT26 + BIT27));
    }
  } else {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(BIT22 + BIT23 + BIT24 + BIT25 + BIT26 + BIT27), 0x00);
  }

  // Disabled SATA MSI and D3 Power State capability
  // RPR 8.13 SATA MSI and D3 Power State Capability
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG34), AccWidthUint8 | S3_SAVE, 0, 0x70);

  if (((pConfig->SataClass) != NATIVE_IDE_MODE) && ((pConfig->SataClass) != LEGACY_IDE_MODE)) {
    // RAID or AHCI
    if ((pConfig->SATAMODE.SataMode.SataIdeCombinedMode) == SATA_IDE_COMBINE_DISABLE) {
      RWMEM ((ddBar5 + SB_SATA_BAR5_REG00), AccWidthUint8 | S3_SAVE, ~(BIT2 + BIT1 + BIT0), BIT2 + BIT0);
      RWMEM ((ddBar5 + SB_SATA_BAR5_REG0C), AccWidthUint8 | S3_SAVE, 0xC0, 0x3F);
      // RPR 8.10 Disabling CCC (Command Completion Coalescing) support.
      // 8 messages
      RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1);
    } else {
      // RPR 8.10 Disabling CCC (Command Completion Coalescing) support.
      if ( pConfig->SataCccSupport ) {
          // 8 messages
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1);
      } else {
          // 4 messages
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2);
      }
    }
  }

  if ( pConfig->BIOSOSHandoff == 1 ) {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REG24), AccWidthUint8 | S3_SAVE, ~BIT0, BIT0);
  } else {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REG24), AccWidthUint8 | S3_SAVE, ~BIT0, 0x00);
  }

  SataPortMode = (UINT16)pConfig->SataPortMode;
  dbPortNum = 0;
  while ( dbPortNum < 6 ) {
    dbPortMode = (UINT8) (SataPortMode & 3);
    if ( (dbPortMode == BIT0) || (dbPortMode == BIT1) ) {
      if ( dbPortMode == BIT0 ) {
        // set GEN 1
        RWMEM (ddBar5 + SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0x0F, 0x10);
      }
      if ( dbPortMode == BIT1 ) {
        // set GEN2 (default is GEN3)
        RWMEM (ddBar5 + SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0x0F, 0x20);
      }
      RWMEM (ddBar5 + SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFF, 0x01);
    }
    SataPortMode >>= 2;
    dbPortNum ++;
  }
  SbStall (1000);
  SataPortMode = (UINT16)pConfig->SataPortMode;
  dbPortNum = 0;
  while ( dbPortNum < 6 ) {
    dbPortMode = (UINT8) (SataPortMode & 3);
    if ( (dbPortMode == BIT0) || (dbPortMode == BIT1) ) {
      RWMEM (ddBar5 + SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFE, 0x00);
    }
    dbPortNum ++;
    SataPortMode >>= 2;
  }
  WritePCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
  //Disable write access to pci header, pm capabilities
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
}


/**
 * sataInitMidPost - Config SATA controller in Middle POST.
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sataInitMidPost (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32   ddBar5;
  sataBar5setting (pConfig, &ddBar5);
  //If this is not S3 resume and also if SATA set to one of IDE mode, then implement drive detection workaround.
  if ( ! (pConfig->S3Resume) && ( ((pConfig->SataClass) != AHCI_MODE)  && ((pConfig->SataClass) != RAID_MODE) ) ) {
    sataDriveDetection (pConfig, &ddBar5);
  }
}

/**
 * sataDriveDetection - Sata drive detection
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 * @param[in] *pBar5 Sata BAR5 base address.
 *
 */
VOID
sataDriveDetection (
  IN       AMDSBCFG* pConfig,
  IN       UINT32 *pBar5
  )
{
  UINT32   ddVar0;
  UINT8   dbPortNum;
  UINT8   dbVar0;
  UINT16   dwIoBase;
  UINT16   dwVar0;
  if ( (pConfig->SataClass == NATIVE_IDE_MODE) || (pConfig->SataClass == LEGACY_IDE_MODE) || (pConfig->SataClass == IDE_TO_AHCI_MODE) ) {
    for ( dbPortNum = 0; dbPortNum < 4; dbPortNum++ ) {
      ReadMEM (*pBar5 + SB_SATA_BAR5_REG128 + dbPortNum * 0x80, AccWidthUint32, &ddVar0);
      if ( ( ddVar0 & 0x0F ) == 0x03 ) {
        if ( dbPortNum & BIT0 ) {
          //this port belongs to secondary channel
          ReadPCI (((UINT32) (SATA_BUS_DEV_FUN << 16) + SB_SATA_REG18), AccWidthUint16, &dwIoBase);
        } else {
          //this port belongs to primary channel
          ReadPCI (((UINT32) (SATA_BUS_DEV_FUN << 16) + SB_SATA_REG10), AccWidthUint16, &dwIoBase);
        }
        //if legacy ide mode, then the bar registers don't contain the correct values. So we need to hardcode them
        if ( pConfig->SataClass == LEGACY_IDE_MODE ) {
          dwIoBase = ( (0x170) | ((UINT16) ( (~((UINT8) (dbPortNum & BIT0) << 7)) & 0x80 )) );
        }
        if ( dbPortNum & BIT1 ) {
          //this port is slave
          dbVar0 = 0xB0;
        } else {
          //this port is master
          dbVar0 = 0xA0;
        }
        dwIoBase &= 0xFFF8;
        WriteIO (dwIoBase + 6, AccWidthUint8, &dbVar0);
        //Wait in loop for 30s for the drive to become ready
        for ( dwVar0 = 0; dwVar0 < 300000; dwVar0++ ) {
          ReadIO (dwIoBase + 7, AccWidthUint8, &dbVar0);
          if ( (dbVar0 & 0x88) == 0 ) {
            break;
          }
          SbStall (100);
        }
      } //end of if ( ( ddVar0 & 0x0F ) == 0x03)
    } //for (dbPortNum = 0; dbPortNum < 4; dbPortNum++)
  } //if ( (pConfig->SataClass == NATIVE_IDE_MODE) || (pConfig->SataClass == LEGACY_IDE_MODE) || (pConfig->SataClass == IDE_TO_AHCI_MODE))
}

/**
 * sataInitLatePost - Prepare SATA controller to boot to OS.
 *
 *              - Set class ID to AHCI (if set to AHCI * Mode)
 *              - Enable AHCI interrupt
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sataInitLatePost (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ddBar5;
  UINT8  dbVar;
  UINT8  dbPortNum;

  //Return immediately is sata controller is not enabled
  if ( pConfig->SATAMODE.SataMode.SataController == 0 ) {
    return;
  }
  //Enable write access to pci header, pm capabilities
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT0);

//  if ((pConfig->SATAMODE.SataMode.SataIdeCombinedMode) == SATA_IDE_COMBINE_DISABLE) {
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 1), AccWidthUint8 | S3_SAVE, ~BIT7, BIT7);
//  }
  sataBar5setting (pConfig, &ddBar5);

  ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
  //Enable memory and io access
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, 0xFF, 0x03);

  shutdownUnconnectedSataPortClock (pConfig, ddBar5);

  if (( pConfig->SataClass == IDE_TO_AHCI_MODE) || ( pConfig->SataClass == IDE_TO_AHCI_MODE_4394 )) {
    //program the AHCI class code
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG08), AccWidthUint32 | S3_SAVE, 0, 0x01060100);
    //Set interrupt enable bit
    RWMEM ((ddBar5 + 0x04), AccWidthUint8, (UINT32)~0, BIT1);
    //program the correct device id for AHCI mode
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, 0x4391);
  }

  if (( pConfig->SataClass == AHCI_MODE_4394 ) || ( pConfig->SataClass == IDE_TO_AHCI_MODE_4394 )) {
    //program the correct device id for AHCI 4394 mode
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, 0x4394);
  }

  //Clear error status ?? only 4 port
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG130), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG1B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG230), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG2B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG330), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG3B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  //Restore memory and io access bits
  WritePCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar );
  //Disable write access to pci header and pm capabilities
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
  for ( dbPortNum = 0; dbPortNum < 6; dbPortNum++ ) {
    RWMEM ((ddBar5 + 0x110 + (dbPortNum * 0x80)), AccWidthUint32, 0xFFFFFFFF, 0x00);
  }
}


