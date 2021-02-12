/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics Controller family specific service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 63460 $   @e \$Date: 2011-12-22 19:04:22 -0600 (Thu, 22 Dec 2011) $
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
*/


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "FchPlatform.h"
#include  "Filecode.h"
#define FILECODE PROC_FCH_SATA_FAMILY_HUDSON2_HUDSON2SATAENVSERVICE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
//
// Local Routine
//
VOID FchSataCombineControlDataByte (IN UINT8  *ControlReg);
VOID FchSataCombineControlDataWord (IN UINT16  *ControlReg);

SATA_PHY_SETTING SataPhyTable[] =
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
 * FchInitEnvProgramSataPciRegs - Sata Pci Configuration Space
 * register setting
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvProgramSataPciRegs (
  IN  VOID     *FchDataPtr
  )
{
  UINT8                  *PortRegByte;
  UINT16                 *PortRegWord;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //
  // Caculate SataPortReg for SATA_ESP_PORT
  //
  PortRegByte = &(LocalCfgPtr->Sata.SataEspPort.SataPortReg);
  FchSataCombineControlDataByte (PortRegByte);
  PortRegByte = &(LocalCfgPtr->Sata.SataPortPower.SataPortReg);
  FchSataCombineControlDataByte (PortRegByte);
  PortRegWord = &(LocalCfgPtr->Sata.SataPortMd.SataPortMode);
  FchSataCombineControlDataWord (PortRegWord);
  PortRegByte = &(LocalCfgPtr->Sata.SataHotRemovalEnhPort.SataPortReg);
  FchSataCombineControlDataByte (PortRegByte);

  //
  // Set Sata PCI Configuration Space Write enable
  //
  SataEnableWriteAccess (StdHeader);

  //
  // Enables the SATA watchdog timer register prior to the SATA BIOS post
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x44), AccessWidth8, 0xff, BIT0, StdHeader);

  //
  // SATA PCI Watchdog timer setting
  //  Set timer out to 0x20 to fix IDE to SATA Bridge dropping drive issue.
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x44 + 2), AccessWidth8, 0, 0x20, StdHeader);

  //
  // BIT4:disable fast boot
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040 ), AccessWidth8, 0xff, BIT4, StdHeader);

  //
  // Enable IDE DMA read enhancement
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48 + 3), AccessWidth8, 0xff, BIT7, StdHeader);

  //
  // Unused SATA Ports Disabled
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040  + 2), AccessWidth8, 0, LocalCfgPtr->Sata.SataPortPower.SataPortReg, StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48), AccessWidth32, (UINT32) (~ (0x01 << 11)), (UINT32) (0x01 << 11), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 ), AccessWidth32, (UINT32) (~ (0x01 << 31)), (UINT32) (0x00 << 31), StdHeader);
  //RPR 9.22 Design Enhancement
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x1 << 18)), (UINT32) (0x1 << 18), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x1 << 20)), (UINT32) (0x1 << 20), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x1 << 21)), (UINT32) (0x1 << 21), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x7 << 26)), (UINT32) (0x7 << 26), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x3 << 30)), (UINT32) (0x3 << 30), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48), AccessWidth32, (UINT32) (~ (0x1 << 30)), (UINT32) (0x1 << 30), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x1 << 29)), (UINT32) (0x1 << 29), StdHeader);
}

/**
 * FchSataCombineControlDataByte - Combine port control options
 * to one control byte.
 *
 *
 * @param[in] *ControlReg - Data pointer for control byte.
 *
 */
VOID
FchSataCombineControlDataByte (
  IN  UINT8    *ControlReg
  )
{
  UINT8   Index;
  UINT8   PortControl;

  *ControlReg = 0;
  for ( Index = 0; Index < 8; Index++ ) {
    PortControl = *( ControlReg + 1 + Index );
    *ControlReg |= PortControl << Index;
  }
}
/**
 * FchSataCombineControlDataWord - Combine port control options
 * to one control Word.
 *
 *
 * @param[in] *ControlReg - Data pointer for control byte.
 *
 */
VOID
FchSataCombineControlDataWord (
  IN  UINT16    *ControlReg
  )
{
  UINT8   Index;
  UINT8   PortControl;

  *ControlReg = 0;
  for ( Index = 0; Index < 8; Index++ ) {
    PortControl = *( (UINT8 *)ControlReg + 2 + Index );
    *ControlReg |= PortControl << (Index * 2);
  }
}


VOID
FchProgramSataPhy (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  SATA_PHY_SETTING         *PhyTablePtr;
  UINT16                   Index;

  PhyTablePtr = &SataPhyTable[0];

  for (Index = 0; Index < ARRAY_SIZE(SataPhyTable); Index++) {
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x80, AccessWidth16, 0x00, PhyTablePtr->PhyCoreControlWord, StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x98, AccessWidth32, 0x00, PhyTablePtr->PhyFineTuneDword, StdHeader);
    ++PhyTablePtr;
  }


  RwPci ((SATA_BUS_DEV_FUN << 16) + 0x80, AccessWidth16, 0x00, 0x110, StdHeader);
  RwPci ((SATA_BUS_DEV_FUN << 16) + 0x09C , AccessWidth32, (UINT32) (~(0x7 << 4)), (UINT32) (0x2 << 4), StdHeader);
  RwPci ((SATA_BUS_DEV_FUN << 16) + 0x80, AccessWidth16, 0x00, 0x10, StdHeader);
}

/**
 * FchInitEnvSataRaidProgram - Configuration SATA Raid
 * controller
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSataRaidProgram (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       SataSSIDValue;
  UINT32       DeviceId;
  UINT8        EfuseValue;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Class code
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x08), AccessWidth32, 0, 0x01040040, StdHeader);
  //
  // Device ID
  //
  SataSSIDValue = 0;
  if (LocalCfgPtr->Sata.SataRaid5Ssid != 0 ) {
    SataSSIDValue = LocalCfgPtr->Sata.SataRaid5Ssid;
  }

  DeviceId = FCH_SATA_RAID5_DID;
  EfuseValue = SATA_EFUSE_LOCATION;
  GetEfuseStatus (&EfuseValue, StdHeader);

  if (( EfuseValue & SATA_EFUSE_BIT ) || ( LocalCfgPtr->Sata.SataForceRaid == 1 )) {
    DeviceId = FCH_SATA_RAID_DID;
    if (LocalCfgPtr->Sata.SataRaidSsid != 0 ) {
      SataSSIDValue = LocalCfgPtr->Sata.SataRaidSsid;
    }
  }

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x02), AccessWidth16, 0, DeviceId, StdHeader);
  //
  // SSID
  //
  if (SataSSIDValue != 0 ) {
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x2C, AccessWidth32, 0, SataSSIDValue, StdHeader);
  }
}

