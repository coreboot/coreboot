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
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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

  for (Index = 0; Index < (sizeof (SataPhyTable) / sizeof (SATA_PHY_SETTING)); Index++) {
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

