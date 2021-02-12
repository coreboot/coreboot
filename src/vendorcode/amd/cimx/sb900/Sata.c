
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
****************************************************************************
*/
#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

//
// Declaration of local functions
//
VOID  sataSetIrqIntResource (IN AMDSBCFG* pConfig);
VOID  sataBar5setting (IN AMDSBCFG* pConfig, IN UINT32 *pBar5);
#ifdef SATA_BUS_DEV_FUN_FPGA
  VOID  sataBar5settingFpga (IN AMDSBCFG* pConfig, IN UINT32 *pBar5);
#endif
VOID  shutdownUnconnectedSataPortClock (IN AMDSBCFG* pConfig, IN UINT32 ddBar5);
VOID  CaculateAhciPortNumber (IN AMDSBCFG* pConfig, IN UINT32 ddBar5);
VOID  sataDriveDetection (IN AMDSBCFG* pConfig, IN UINT32 *pBar5);
#ifdef SATA_BUS_DEV_FUN_FPGA
  VOID  sataDriveDetectionFpga (IN AMDSBCFG* pConfig, IN UINT32 *pBar5);
#endif
VOID  sataGpioPorcedure (IN AMDSBCFG* pConfig);

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
#ifdef SATA_BUS_DEV_FUN_FPGA
VOID
sataBar5settingFpga (
  IN       AMDSBCFG*   pConfig,
  IN       UINT32      *pBar5
  )
{
  UINT8  dbValue;
  //Get BAR5 value
  ReadPCI (((SATA_BUS_DEV_FUN_FPGA << 16) + SB_SATA_REG24), AccWidthUint32, pBar5);
  //Assign temporary BAR if is not already assigned
  if ( (*pBar5 == 0) || (*pBar5 == - 1) ) {
    //assign temporary BAR5
    if ( (pConfig->TempMMIO == 0) || (pConfig->TempMMIO == - 1) ) {
      *pBar5 = 0xFEC01000;
    } else {
      *pBar5 = pConfig->TempMMIO;
    }
    WritePCI (((SATA_BUS_DEV_FUN_FPGA << 16) + SB_SATA_REG24), AccWidthUint32, pBar5);
  }
  //Clear Bits 9:0
  *pBar5 = *pBar5 & 0xFFFFFC00;
  dbValue = 0x07;
  WritePCI (((SATA_BUS_DEV_FUN_FPGA << 16) + 0x04), AccWidthUint8, &dbValue);
  WritePCI (((PCIB_BUS_DEV_FUN << 16) + 0x04), AccWidthUint8, &dbValue);
}
#endif
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
  UINT32  ddPortSataStatus;
  UINT8  cimSataClkAutoOff;

  cimSataClkAutoOff = (UINT8) pConfig->SataClkAutoOff;
#if  SB_CIMx_PARAMETER == 0
  cimSataClkAutoOff = cimSataClkAutoOffDefault;
#endif
  if ( cimSataClkAutoOff == TRUE ) {
    //ENH225976 Enable SATA auto clock control by default ( (pConfig->SataClass == NATIVE_IDE_MODE) || (pConfig->SataClass == LEGACY_IDE_MODE) || (pConfig->SataClass == IDE_TO_AHCI_MODE) ) {
    for ( dbPortNum = 0; dbPortNum < 8; dbPortNum++ ) {
      ReadMEM (ddBar5 + SB_SATA_BAR5_REG128 + (dbPortNum * 0x80), AccWidthUint32, &ddPortSataStatus);
      // Shutdown the clock for the port and do the necessary port reporting changes.
      // ?? Error port status should be 1 not 3
      ddPortSataStatus &= 0x00000F0F;
      if ( (!((ddPortSataStatus == 0x601) || (ddPortSataStatus == 0x201) || (ddPortSataStatus == 0x103))) && (! ((pConfig->SATAESPPORT.SataPortReg) & (1 << dbPortNum))) ) {
        TRACE ((DMSG_SB_TRACE, "Shutting down clock for SATA port %X \n", dbPortNum));
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, 0xFF, (1 << dbPortNum));
      }
    }     //end of for (dbPortNum=0;dbPortNum<6;dbPortNum++)
  }  //end of SataClkAuto Off option
}

/**
 * CaculateAhciPortNumber - Caculat AHCI Port Number
 *
 *   - Private function
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 * @param[in] ddBar5 Sata BAR5 base address.
 *
 */
VOID
CaculateAhciPortNumber (
  IN       AMDSBCFG* pConfig,
  IN       UINT32 ddBar5
  )
{
  UINT8  dbPortNum;
  UINT8  dbPortSataStatus;
  UINT8  NumOfPorts;
  UINT8  MaxPortNum;
  NumOfPorts = 0;
  MaxPortNum = 4;
  if ( (pConfig->SATAMODE.SataMode.SataIdeCombMdPriSecOpt) == ENABLED ) {
    MaxPortNum = 6;
  } else if ( (pConfig->SATAMODE.SataMode.SataIdeCombinedMode) == ENABLED ) {
    MaxPortNum = 8;
  }
  ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8, &dbPortSataStatus);
  for ( dbPortNum = 0; dbPortNum < MaxPortNum; dbPortNum++ ) {
    if ( dbPortSataStatus & (1 << dbPortNum) ) {
      RWMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, ~(1 << dbPortNum), 00);
    }
  }
  ReadMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, &dbPortSataStatus);
  //if all ports are in disabled state, report atleast one port
  if ( (dbPortSataStatus & 0xFF) == 0) {
    RWMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, (UINT32) ~(0xFF), 01);
  }
  ReadMEM (ddBar5 + SB_SATA_BAR5_REG0C, AccWidthUint8, &dbPortSataStatus);
  for (dbPortNum = 0; dbPortNum < MaxPortNum; dbPortNum ++) {
    if (dbPortSataStatus & (1 << dbPortNum)) {
      NumOfPorts++;
    }
  }
  if ( NumOfPorts == 0) {
    NumOfPorts = 0x01;
  }
  RWMEM (ddBar5 + SB_SATA_BAR5_REG00, AccWidthUint8, 0xE0, NumOfPorts - 1);
}

/**
 * sataGpioPorcedure - Sata GPIO function Procedure
 *
 *   - Private function
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
sataGpioPorcedure (
  IN  AMDSBCFG* pConfig
  )
{
  UINT32  ddBar5;
  UINT32  ddData;
  UINT32  eMb;
  UINT32  ddTempVariable;
  UINT8  cimSataSgpio0;

  ddBar5 = 0;
  eMb = 0;
  cimSataSgpio0 = (UINT8) pConfig->SataSgpio0;

#if  SB_CIMx_PARAMETER == 0
  cimSataSgpio0 = cimSataSgpio0Default;
#endif

  sataBar5setting (pConfig, &ddBar5);
  ReadMEM (ddBar5 + SB_SATA_BAR5_REG1C , AccWidthUint32 | S3_SAVE, &ddData);
  eMb = (ddBar5 + (( ddData & 0xFFFF0000) >> 14));
  if ( eMb ) {
    ddTempVariable = 0x03040C00;
    WriteMEM ( ddBar5 + eMb, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    ddTempVariable = 0x00C08240;
    WriteMEM ( ddBar5 + eMb + 4, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    ddTempVariable = 0x00000001;
    WriteMEM ( ddBar5 + eMb + 8, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    if ( cimSataSgpio0 ) {
      ddTempVariable = 0x00000060;
    } else {
      ddTempVariable = 0x00000061;
    }
    WriteMEM ( ddBar5 + eMb + 0x0C, AccWidthUint32 | S3_SAVE, &ddTempVariable);

    RWMEM ((ddBar5 + SB_SATA_BAR5_REG20), AccWidthUint16 | S3_SAVE, ~(BIT8), BIT8);
    do {
      ReadMEM (ddBar5 + SB_SATA_BAR5_REG20 , AccWidthUint32 | S3_SAVE, &ddData);
      ddData = ddData & BIT8;
    } while ( ddData != 0 );

    ddTempVariable = 0x03040F00;
    WriteMEM ( ddBar5 + eMb, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    ddTempVariable = 0x00008240;
    WriteMEM ( ddBar5 + eMb + 4, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    ddTempVariable = 0x00000002;
    WriteMEM ( ddBar5 + eMb + 8, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    ddTempVariable = 0x00800000;
    WriteMEM ( ddBar5 + eMb + 0x0C, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    ddTempVariable = 0x0F003700;
    WriteMEM ( ddBar5 + eMb + 0x0C, AccWidthUint32 | S3_SAVE, &ddTempVariable);
    RWMEM ((ddBar5 + SB_SATA_BAR5_REG20), AccWidthUint16 | S3_SAVE, ~(BIT8), BIT8);
    do {
      ReadMEM (ddBar5 + SB_SATA_BAR5_REG20 , AccWidthUint32 | S3_SAVE, &ddData);
      ddData = ddData & BIT8;
    } while ( ddData != 0 );
  }
}


/**
 * Table for class code of SATA Controller in different modes
 *
 *
 *
 *
 */
UINT32 sataIfCodeTable[] =
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

UINT16 sataDeviceIDTable[] =
{
  SB_SATA_DID,  //sata device ID of IDE
  SB_SATA_RAID_DID,  //sata device ID of RAID
  SB_SATA_AHCI_DID,  //sata class ID of AHCI
  SB_SATA_DID,  //sata device ID of Legacy IDE
  SB_SATA_DID,  //sata device ID of IDE->AHCI mode
};

/**
 * Table for Sata Phy Fine Setting
 *
 *
 *
 *
 */
SATAPHYSETTING sataPhyTable[] =
{
  //Gen3
  {0x0030, 0x0057A607},
  {0x0031, 0x0057A607},
  {0x0032, 0x0057A407},
  {0x0033, 0x0057A407},
  {0x0034, 0x0057A607},
  {0x0035, 0x0057A607},
  {0x0036, 0x0057A403},
  {0x0037, 0x0057A403},

  //Gen2
  {0x0120, 0x00071302},

  //Gen1
  {0x0110, 0x00174101}
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
  UINT8   dbChannel;
  UINT16    i;
  SATAPHYSETTING  *pPhyTable;

  pConfig->SATAPORTPOWER.SataPortPowerReg = \
    + pConfig->SATAPORTPOWER.SataPortPower.PORT0 \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT1 << 1) \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT2 << 2) \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT3 << 3) \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT4 << 4) \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT5 << 5) \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT6 << 6) \
    + (pConfig->SATAPORTPOWER.SataPortPower.PORT7 << 7);
  // BIT0 Enable write access to PCI header (reg 08h-0Bh) by setting SATA PCI register 40h
  // BIT4:disable fast boot //?
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT0 + BIT4);
  // BIT0 Enable write access to PCI header (reg 08h-0Bh) by setting IDE PCI register 40h
  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG40), AccWidthUint8 | S3_SAVE, 0xff, BIT0);
  // RPR Enable IDE DMA read enhancement
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48 + 3), AccWidthUint8 | S3_SAVE, 0xff, BIT7);
  // RPR Unused SATA Ports Disabled
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 2), AccWidthUint8 | S3_SAVE, 0, pConfig->SATAPORTPOWER.SataPortPowerReg);
  dbValue = (UINT8)pConfig->SataClass;
  if (dbValue == AHCI_MODE_7804) {
    dbValue = AHCI_MODE;
  }
  if (dbValue == IDE_TO_AHCI_MODE_7804) {
    dbValue = IDE_TO_AHCI_MODE;
  }
  // Disable PATA MSI
  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG34), AccWidthUint8 | S3_SAVE, 0x00, 0x00);
  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG06), AccWidthUint8 | S3_SAVE, 0xEF, 0x00);

  if ( (pConfig->SataClass == 3) | (pConfig->SataClass == 0)) {
    dbChannel = 0x00;
    ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48 + 3), AccWidthUint8, &dbChannel);
    dbChannel &= 0xCF;
    if ( pConfig->SataDisUnusedIdePChannel ) {
      dbChannel |= 0x10;
    }
    if ( pConfig->SataDisUnusedIdeSChannel ) {
      dbChannel |= 0x20;
    }
    WritePCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48 + 3), AccWidthUint8, &dbChannel);
  }

  if ((pConfig->SATAMODE.SataMode.SataIdeCombinedMode) == DISABLED ) {
    ReadPCI (((IDE_BUS_DEV_FUN << 16) + SB_SATA_REG40 + 11), AccWidthUint8, &dbChannel);
    dbChannel &= 0xCF;
    if ( pConfig->IdeDisUnusedIdePChannel ) {
      dbChannel |= 0x10;
    }
    if ( pConfig->IdeDisUnusedIdeSChannel ) {
      dbChannel |= 0x20;
    }
    WritePCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG40 + 11), AccWidthUint8, &dbChannel);
  }
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
  if (pConfig->BuildParameters.IdeSsid != 0 ) {
    RWPCI ((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.IdeSsid);
  }
  // SATA Controller Class ID & SSID
  pDeviceIdptr = (UINT16 *) FIXUP_PTR (&sataDeviceIDTable[0]);
  ddTempVar = pConfig->BuildParameters.SataIDESsid;
  dwDeviceId = pDeviceIdptr[dbValue];
  if ( pConfig->SataClass == RAID_MODE) {
    ddTempVar = pConfig->BuildParameters.SataRAID5Ssid;
    dwDeviceId = SB_SATA_RAID5_DID;
    pValue = SATA_EFUSE_LOCATION;
    getEfuseStatus (&pValue);
    if (( pValue & SATA_EFUSE_BIT ) || ( pConfig->SataForceRaid == 1 )) {
      dwDeviceId = SB_SATA_RAID_DID;
      ddTempVar = pConfig->BuildParameters.SataRAIDSsid;
    }
  }
  if ( ((pConfig->SataClass) == AHCI_MODE) || ((pConfig->SataClass) == IDE_TO_AHCI_MODE) ||
    ((pConfig->SataClass) == AHCI_MODE_7804) || ((pConfig->SataClass) == IDE_TO_AHCI_MODE_7804) ) {
    ddTempVar = pConfig->BuildParameters.SataAHCISsid;
  }
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, dwDeviceId);
  RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG2C, AccWidthUint32 | S3_SAVE, 0x00, ddTempVar);

  // SATA IRQ Resource
  sataSetIrqIntResource (pConfig);

  // RPR 9.5 SATA PHY Programming Sequence
  pPhyTable = (SATAPHYSETTING*)FIXUP_PTR (&sataPhyTable[0]);
  for (i = 0; i < ARRAY_SIZE(sataPhyTable); i++) {
    RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG80, AccWidthUint16 | S3_SAVE, 0x00, pPhyTable->wPhyCoreControl);
    RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG98, AccWidthUint32 | S3_SAVE, 0x00, pPhyTable->dwPhyFineTune);
    ++pPhyTable;
  }
  RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG80, AccWidthUint16 | S3_SAVE, 0x00, 0x110);
  RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG9C, AccWidthUint32 | S3_SAVE, (UINT32) (~(0x7 << 4)), (UINT32) (0x2 << 4));
  RWPCI ((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG80, AccWidthUint16 | S3_SAVE, 0x00, 0x10);

  RWPCI (((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
  // Disable write access to PCI header
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
  if ( IsSbA12Plus () ) {
    //SATA PCI Config 0x4C[31:26] program 111111b (six 1's)
    //SATA PCI Config 0x48[11] program 1
    //SATA PCI Config 0x84[31] program 0
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG4C), AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x3f << 26)), (UINT32) (0x3f << 26));
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48), AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x01 << 11)), (UINT32) (0x01 << 11));
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG84), AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x01 << 31)), (UINT32) (0x00 << 31));
  }

  //SATA PCI config register 0x4C [20] =1
  //SATA PCI config register 0x4C [21] =1
  //SATA PCI config register 0x4C [18] =1
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG4C), AccWidthUint32 | S3_SAVE, ~ (BIT18 + BIT20 + BIT21), (BIT18 + BIT20 + BIT21));
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
  UINT8  cimSataMsiCapability;
  UINT8  cimSataTargetSupport8Device;
  UINT8  cimSataDisableGenericMode;
  UINT8  cimSataAhciEnclosureManagement;
  UINT8  cimSataSgpio0;
  UINT8  cimSataSgpio1;
  UINT8  cimSataHotRemovelEnh;
  UINT8  cimSataPhyPllShutDown;
  UINT8  dbCccInt;

  cimSataAggrLinkPmCap = (UINT8) pConfig->SataAggrLinkPmCap;
  cimSataPortMultCap = (UINT8) pConfig->SataPortMultCap;
  cimSataPscCap = (UINT8) pConfig->SataPscCap;
  cimSataSscCap = (UINT8) pConfig->SataSscCap;
  cimSataFisBasedSwitching = (UINT8) pConfig->SataFisBasedSwitching;
  cimSataCccSupport = (UINT8) pConfig->SataCccSupport;
  cimSataMsiCapability = (UINT8) pConfig->SataMsiCapability;
  cimSataTargetSupport8Device = (UINT8) pConfig->SataTargetSupport8Device;
  cimSataDisableGenericMode = (UINT8) pConfig->SataDisableGenericMode;
  cimSataAhciEnclosureManagement = (UINT8) pConfig->SataAhciEnclosureManagement;
  cimSataSgpio0 = (UINT8) pConfig->SataSgpio0;
  cimSataSgpio1 = (UINT8) pConfig->SataSgpio1;
  cimSataHotRemovelEnh = (UINT8) pConfig->SataHotRemoveEnh;
  cimSataPhyPllShutDown = (UINT8) pConfig->SataPhyPllShutDown;

#if  SB_CIMx_PARAMETER == 0
  cimSataAggrLinkPmCap = cimSataAggrLinkPmCapDefault;
  cimSataPortMultCap = cimSataPortMultCapDefault;
  cimSataPscCap = cimSataPscCapDefault;
  cimSataSscCap = cimSataSscCapDefault;
  cimSataFisBasedSwitching = cimSataFisBasedSwitchingDefault;
  cimSataCccSupport = cimSataCccSupportDefault;
  cimSataMsiCapability = cimSataMsiCapabilityDefault;
  cimSataTargetSupport8Device = cimSataTargetSupport8DeviceDefault;
  cimSataDisableGenericMode = cimSataDisableGenericModeDefault;
  cimSataAhciEnclosureManagement = cimSataAhciEnclosureManagementDefault;
  cimSataSgpio0 = cimSataSgpio0Default;
  cimSataSgpio1 = cimSataSgpio1Default;
  cimSataHotRemovelEnh = cimSataHotRemoveEnhDefault;
  cimSataPhyPllShutDown = cimSataPhyPllShutDownDefault;
#endif

  ddAndMask = 0;
  ddOrMask = 0;
  ddBar5 = 0;
  if ( pConfig->SATAMODE.SataMode.SataController == 0 ) {
    return;   //return if SATA controller is disabled.
  }

  //Enable write access to pci header, pm capabilities
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, 0xFF, BIT0);

  sataBar5setting (pConfig, &ddBar5);

  ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, 0xFF, 0x03);   //memory and io access enable
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
  } else {
    ddAndMask |= BIT1;
  }
  if ( cimSataSscCap ) {
    ddOrMask |= BIT26;
  } else {
    ddAndMask |= BIT26;
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
  if ( cimSataAhciEnclosureManagement ) {
    ddOrMask |= BIT27;
  } else {
    ddAndMask |= BIT27;
  }
  RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint32 | S3_SAVE, ~ddAndMask, ddOrMask);


  // SATA ESP port setting
  // These config bits are set for SATA driver to identify which ports are external SATA ports and need to
  // support hotplug. If a port is set as an external SATA port and need to support hotplug, then driver will
  // not enable power management (HIPM & DIPM) for these ports.
  pConfig->SATAESPPORT.SataPortReg = \
    + pConfig->SATAESPPORT.SataEspPort.PORT0 \
    + (pConfig->SATAESPPORT.SataEspPort.PORT1 << 1) \
    + (pConfig->SATAESPPORT.SataEspPort.PORT2 << 2) \
    + (pConfig->SATAESPPORT.SataEspPort.PORT3 << 3) \
    + (pConfig->SATAESPPORT.SataEspPort.PORT4 << 4) \
    + (pConfig->SATAESPPORT.SataEspPort.PORT5 << 5) \
    + (pConfig->SATAESPPORT.SataEspPort.PORT6 << 6) \
    + (pConfig->SATAESPPORT.SataEspPort.PORT7 << 7);
  if ( pConfig->SATAESPPORT.SataPortReg != 0 ) {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, ~(pConfig->SATAESPPORT.SataPortReg), 0);
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, 0xFF00FF00, (pConfig->SATAESPPORT.SataPortReg << 16));
    // RPR 8.7 External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint32 | S3_SAVE, ~(BIT20), BIT20);
  } else {
    // RPR 8.7 External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set (Clear for no ESP port)
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, 0xFF00FF00, 0x00);
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint32 | S3_SAVE, ~(BIT20), 0x00);
  }

  if ( cimSataFisBasedSwitching ) {
    if (dbEfuse & BIT1) {
      RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, 0x00FFFFFF, 0x00);
    } else {
      RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, 0x00FFFFFF, 0xFF000000);
    }
  } else {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REGF8), AccWidthUint32 | S3_SAVE, 0x00FFFFFF, 0x00);
  }

  //SB02712 Turn off MSI for SATA IDE mode.
  if ((pConfig->SataClass == NATIVE_IDE_MODE) || (pConfig->SataClass == LEGACY_IDE_MODE)) {
    cimSataMsiCapability = 0;
  }
  //Enabled SATA MSI capability
  // RPR 8.11 SATA MSI and D3 Power State Capability
  if (IsSbA12Plus ()) {
    if ( cimSataMsiCapability ) {
      RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG34), AccWidthUint8 | S3_SAVE, 0, 0x50);
    } else {
      RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG34), AccWidthUint8 | S3_SAVE, 0, 0x70);
    }
  } else {
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG34), AccWidthUint8 | S3_SAVE, 0, 0x70);
  }

  if (((pConfig->SataClass) != NATIVE_IDE_MODE) && ((pConfig->SataClass) != LEGACY_IDE_MODE)) {
    // RAID or AHCI
    if ((pConfig->SATAMODE.SataMode.SataIdeCombinedMode) == DISABLED) {
      // IDE2 Controller is enabled
      if ((pConfig->SATAMODE.SataMode.SataIdeCombMdPriSecOpt) == ENABLED) {
        // 6 AHCI mode
        RWMEM ((ddBar5 + SB_SATA_BAR5_REG0C), AccWidthUint8 | S3_SAVE, 0x00, 0x3F);
        RWMEM ((ddBar5 + SB_SATA_BAR5_REG00), AccWidthUint8 | S3_SAVE, ~(BIT2 + BIT1 + BIT0), BIT2 + BIT0);
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1);
        RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint8, 0x07, 0x30);
      } else {
        RWMEM ((ddBar5 + SB_SATA_BAR5_REG0C), AccWidthUint8 | S3_SAVE, 0x00, 0x0F);
        if ( pConfig->SataCccSupport ) {
          RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1);
        } else {
          RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2);
        }
      }
    } else {
      // IDE2 Controller is disabled
      RWMEM ((ddBar5 + SB_SATA_BAR5_REG00), AccWidthUint8 | S3_SAVE, ~(BIT2 + BIT1 + BIT0), BIT2 + BIT1 + BIT0);
      RWMEM ((ddBar5 + SB_SATA_BAR5_REG0C), AccWidthUint8 | S3_SAVE, 0x00, 0xFF);
      if ( pConfig->SataCccSupport ) {
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT3);
      } else {
        RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG50 + 2), AccWidthUint8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1);
      }
    }
  }
  if ( pConfig->BIOSOSHandoff == 1 ) {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REG24), AccWidthUint8 | S3_SAVE, ~BIT0, BIT0);
  } else {
    RWMEM ((ddBar5 + SB_SATA_BAR5_REG24), AccWidthUint8 | S3_SAVE, ~BIT0, 0x00);
  }
  pConfig->SATAPORTMODE.SataPortMode = \
    pConfig->SATAPORTMODE.SataPortMd.PORT0 \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT1 << 2) \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT2 << 4) \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT3 << 6) \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT4 << 8) \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT5 << 10) \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT6 << 12) \
    + (pConfig->SATAPORTMODE.SataPortMd.PORT7 << 14);
  SataPortMode = (UINT16)pConfig->SATAPORTMODE.SataPortMode;
  dbPortNum = 0;

  while ( dbPortNum < 8 ) {
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
      cimSbStall (1000);
      RWMEM (ddBar5 + SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFE, 0x00);
    }
    SataPortMode >>= 2;
    dbPortNum ++;
  }
  SbStall (1000);
  SataPortMode = (UINT16)pConfig->SATAPORTMODE.SataPortMode;
  dbPortNum = 0;

  while ( dbPortNum < 8 ) {
    dbPortMode = (UINT8) (SataPortMode & 3);
    if ( (dbPortMode == BIT0) || (dbPortMode == BIT1) ) {
      RWMEM (ddBar5 + SB_SATA_BAR5_REG12C + dbPortNum * 0x80, AccWidthUint8, 0xFE, 0x00);
    }
    dbPortNum ++;
    SataPortMode >>= 2;
  }

  if ( cimSataTargetSupport8Device ) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint16, ~BIT12, BIT12);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint16, ~BIT12, 0x00);
  }

  if ( cimSataDisableGenericMode ) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint16, ~BIT13, BIT13);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint16, ~BIT13, 0x00);
  }
  // 9.19 Optionally Disable Hot-removal Detection Enhancement
  if ( cimSataHotRemovelEnh ) {
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG80), AccWidthUint16 | S3_SAVE, ~BIT8, BIT8 );
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REGA8), AccWidthUint16 | S3_SAVE, ~BIT0, BIT0);
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG80), AccWidthUint16 | S3_SAVE, ~BIT8, 0 );
  }
  if ( cimSataSgpio0 ) {
    sataGpioPorcedure (pConfig);
  }
  if ( cimSataSgpio1 ) {
    sataGpioPorcedure (pConfig);
  }
  if ( IsSbA11 () ) {
    if ( cimSataPhyPllShutDown ) {
      RWPCI (((SATA_BUS_DEV_FUN << 16) + 0x87), AccWidthUint8 | S3_SAVE, ~(BIT6 + BIT7), BIT6 + BIT7);
    } else {
      RWPCI (((SATA_BUS_DEV_FUN << 16) + 0x87), AccWidthUint8 | S3_SAVE, ~(BIT6 + BIT7), 0x00);
    }
  } else {
    if ( cimSataPhyPllShutDown ) {
      RWPCI (((SATA_BUS_DEV_FUN << 16) + 0x87), AccWidthUint8 | S3_SAVE, ~(BIT6), BIT6);
    } else {
      RWPCI (((SATA_BUS_DEV_FUN << 16) + 0x87), AccWidthUint8 | S3_SAVE, ~(BIT6), 0x00);
    }
  }
  if ( IsSbA12Plus () ) {
    //SATA PCI Config 0x4C[31:26] program 111111b (six 1's)
    //SATA PCI Config 0x48[11] program 1
    //SATA PCI Config 0x84[31] program 0
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG4C), AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x3f << 26)), (UINT32) (0x3f << 26));
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG48), AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x01 << 11)), (UINT32) (0x01 << 11));
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG84), AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x01 << 31)), (UINT32) (0x00 << 31));
  }
  // RPR 9.18 CCC Interrupt
  dbCccInt = 4;
  if ((pConfig->SATAMODE.SataMode.SataIdeCombinedMode) == DISABLED) {
    if ((pConfig->SATAMODE.SataMode.SataIdeCombMdPriSecOpt) == ENABLED) {
      dbCccInt = 6;
    }
  } else {
    dbCccInt = 8;
  }
  RWMEM ((ddBar5 + SB_SATA_BAR5_REGFC), AccWidthUint8, 0x07, (dbCccInt << 3));

  shutdownUnconnectedSataPortClock (pConfig, ddBar5);

  WritePCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);

  // RPR 9.13 Disable SATA FLR Capability
  // SATA_PCI_config 0x70 [15:8] = 0x00
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG70), AccWidthUint32 | S3_SAVE, 0xFFFF00FF, 0x00);
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
#ifdef SATA_BUS_DEV_FUN_FPGA
  sataBar5settingFpga (pConfig, &ddBar5);
  sataDriveDetectionFpga (pConfig, &ddBar5);
#endif
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
  UINT32   ddVar1;
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sata drive detection procedure\n\n"));
  TRACE ((DMSG_SB_TRACE, "SATA BAR5 is %X \n", *pBar5));
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
        for ( ddVar1 = 0; ddVar1 < 300000; ddVar1++ ) {
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

#ifdef SATA_BUS_DEV_FUN_FPGA
VOID
sataDriveDetectionFpga (
  IN       AMDSBCFG* pConfig,
  IN       UINT32 *pBar5
  )
{
  UINT32   ddVar0;
  UINT8   dbPortNum;
  UINT8   dbVar0;
  UINT16   dwIoBase;
  UINT32   ddVar1;
  TRACE ((DMSG_SB_TRACE, "CIMx - Entering sata drive detection procedure\n\n"));
  TRACE ((DMSG_SB_TRACE, "SATA BAR5 is %X \n", *pBar5));
  for ( dbPortNum = 0; dbPortNum < 4; dbPortNum++ ) {
    ReadMEM (*pBar5 + SB_SATA_BAR5_REG128 + dbPortNum * 0x80, AccWidthUint32, &ddVar0);
    if ( ( ddVar0 & 0x0F ) == 0x03 ) {
      if ( dbPortNum & BIT0 ) {
        //this port belongs to secondary channel
        ReadPCI (((UINT32) (SATA_BUS_DEV_FUN_FPGA << 16) + SB_SATA_REG18), AccWidthUint16, &dwIoBase);
      } else {
        //this port belongs to primary channel
        ReadPCI (((UINT32) (SATA_BUS_DEV_FUN_FPGA << 16) + SB_SATA_REG10), AccWidthUint16, &dwIoBase);
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
      for ( ddVar1 = 0; ddVar1 < 300000; ddVar1++ ) {
        ReadIO (dwIoBase + 7, AccWidthUint8, &dbVar0);
        if ( (dbVar0 & 0x88) == 0 ) {
          break;
        }
        SbStall (100);
      }
    } //end of if ( ( ddVar0 & 0x0F ) == 0x03)
  } //for (dbPortNum = 0; dbPortNum < 4; dbPortNum++)
}
#endif

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

  sataBar5setting (pConfig, &ddBar5);

  ReadPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar);
  //Enable memory and io access
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, 0xFF, 0x03);

  if (( pConfig->SataClass == IDE_TO_AHCI_MODE) || ( pConfig->SataClass == IDE_TO_AHCI_MODE_7804 )) {
    //program the AHCI class code
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG08), AccWidthUint32 | S3_SAVE, 0, 0x01060100);
    //Set interrupt enable bit
    RWMEM ((ddBar5 + 0x04), AccWidthUint8, (UINT32)~0, BIT1);
    //program the correct device id for AHCI mode
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, SB_SATA_AHCI_DID);
  }

  if (( pConfig->SataClass == AHCI_MODE_7804 ) || ( pConfig->SataClass == IDE_TO_AHCI_MODE_7804 )) {
    //program the correct device id for AHCI 7804 mode
    RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, 0, SB_SATA_AMDAHCI_DID);
  }
  // OBS236459 IDE controller not shown in device manager when Os installed on IDE mode port
  //if ( pConfig->SataClass == IDE_TO_AHCI_MODE_7804 ) {
    //Disable IDE2 and Enable 8 channel for IDE-AHCI mode
    //RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGDA, AccWidthUint8, ~BIT1, BIT3);
  //}

  //Clear error status
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG130), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG1B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG230), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG2B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG330), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG3B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG430), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  RWMEM ((ddBar5 + SB_SATA_BAR5_REG4B0), AccWidthUint32 | S3_SAVE, 0xFFFFFFFF, 0xFFFFFFFF);
  CaculateAhciPortNumber (pConfig, ddBar5);
  //Restore memory and io access bits
  WritePCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG04), AccWidthUint8, &dbVar );
  //Disable write access to pci header and pm capabilities
  RWPCI (((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG40), AccWidthUint8 | S3_SAVE, ~BIT0, 0);
  for ( dbPortNum = 0; dbPortNum < 6; dbPortNum++ ) {
    RWMEM ((ddBar5 + 0x110 + (dbPortNum * 0x80)), AccWidthUint32, 0xFFFFFFFF, 0x00);
  }
}


