/* $NoKeywords:$ */
/**
 * @file
 *
 * SATA Controller family specific service procedure
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 86583 $   @e \$Date: 2013-01-23 12:31:06 -0600 (Wed, 23 Jan 2013) $
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
*/


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "FchPlatform.h"
#include  "Filecode.h"
#define FILECODE PROC_FCH_SATA_FAMILY_YANGTZE_YANGTZESATAENVSERVICE_FILECODE


SATA_PHY_SETTING SataPhyTable[] =
{
  {0x0030, 0x0040F407},
  {0x0120, 0x00403204},
  {0x0110, 0x00403103},

  {0x0031, 0x0040F407},
  {0x0121, 0x00403204},
  {0x0111, 0x00403103},

};

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
//
// Local Routine
//
VOID FchSataCombineControlDataByte (IN UINT8  *ControlReg);
VOID FchSataCombineControlDataWord (IN UINT16  *ControlReg);


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
  //Caculate SataPortReg for SATA_ESP_PORT
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

  // *
  // Enables the SATA watchdog timer register prior to the SATA BIOS post
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x44), AccessWidth8, 0xff, BIT0, StdHeader);

  // *
  // SATA PCI Watchdog timer setting
  //  Set timer out to 0x20 to fix IDE to SATA Bridge dropping drive issue.
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x44 + 2), AccessWidth8, 0, 0x20, StdHeader);

  //
  // BIT4: Enable fast boot (SpeedupXPBoot)
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040), AccessWidth8, 0xef, 0, StdHeader);

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48 + 3), AccessWidth8, 0xff, BIT7, StdHeader);

  //
  // Unused SATA Ports Disabled
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040 + 2), AccessWidth8, 0, LocalCfgPtr->Sata.SataPortPower.SataPortReg, StdHeader);

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084), AccessWidth32, (UINT32) (~ (0x01 << 31)), (UINT32) (0x00 << 31), StdHeader);
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

/**
 * FchProgramSataPhy - Program Sata PHY registers
 *
 * @param[in]   StdHeader
 *
 */
VOID
FchProgramSataPhy (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  SATA_PHY_SETTING         *PhyTablePtr;
  UINT16                   Index;
  UINT32                   SquelchValue[2];
  UINT8                    PortNum;

  PhyTablePtr = &SataPhyTable[0];

  for (Index = 0; Index < ARRAY_SIZE(SataPhyTable); Index++) {
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x080, AccessWidth16, 0xFC00, PhyTablePtr->PhyCoreControlWord, StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x98, AccessWidth32, 0x00, PhyTablePtr->PhyFineTuneDword, StdHeader);
    ++PhyTablePtr;
  }
  SquelchValue[0] = (0x07 << 9);
  SquelchValue[1] = (0x07 << 9);
  for (PortNum = 0; PortNum < 2; PortNum ++) {
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x080, AccessWidth16, 0x00, ( 0x30 + PortNum), StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x09C, AccessWidth32, (UINT32) (~(0x7 << 9)), SquelchValue[PortNum], StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x09C, AccessWidth32, (UINT32) (~(0x7 << 13)), (UINT32) (0x0 << 13), StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x080, AccessWidth16, 0x00, ( 0x20 + PortNum), StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x09C, AccessWidth32, (UINT32) (~(0x7 << 9)), SquelchValue[PortNum], StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x080, AccessWidth16, 0x00, ( 0x10 + PortNum), StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x09C, AccessWidth32, (UINT32) (~(0x7 << 9)), SquelchValue[PortNum], StdHeader);
    RwPci ((SATA_BUS_DEV_FUN << 16) + 0x080, AccessWidth16, 0x00, 0x010, StdHeader);
  }
}


