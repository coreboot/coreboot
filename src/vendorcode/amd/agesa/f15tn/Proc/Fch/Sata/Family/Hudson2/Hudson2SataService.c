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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
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
#define FILECODE PROC_FCH_SATA_FAMILY_HUDSON2_HUDSON2SATASERVICE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/**
 * FchSataGpioInitial - Sata GPIO function Procedure
 *
 *   - Private function
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSataGpioInitial (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       Bar5;
  UINT32       FchSataBarRegDword;
  UINT32       EMb;
  UINT32       SataGpioVariableDword;
  UINT8        FchSataSgpio0;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  Bar5 = 0;
  EMb = 0;
  FchSataSgpio0 = (UINT8) LocalCfgPtr->Sata.SataSgpio0;

  SataBar5setting (LocalCfgPtr, &Bar5);
  ReadMem (Bar5 + 0x1C , AccessWidth32, &FchSataBarRegDword);
  EMb = (Bar5 + (( FchSataBarRegDword & 0xFFFF0000) >> 14));

  if ( EMb ) {
    SataGpioVariableDword = 0x03040C00;
    WriteMem ( Bar5 + EMb, AccessWidth32, &SataGpioVariableDword);
    SataGpioVariableDword = 0x00C08240;
    WriteMem ( Bar5 + EMb + 4, AccessWidth32, &SataGpioVariableDword);
    SataGpioVariableDword = 0x00000001;
    WriteMem ( Bar5 + EMb + 8, AccessWidth32, &SataGpioVariableDword);

    if ( FchSataSgpio0 ) {
      SataGpioVariableDword = 0x00000060;
    } else {
      SataGpioVariableDword = 0x00000061;
    }

    WriteMem ( Bar5 + EMb + 0x0C, AccessWidth32, &SataGpioVariableDword);
    RwMem ((Bar5 + 0x20), AccessWidth16, (UINT32)~(BIT8), BIT8);

    do {
      ReadMem (Bar5 + 0x20 , AccessWidth32, &FchSataBarRegDword);
      FchSataBarRegDword = FchSataBarRegDword & BIT8;
    } while ( FchSataBarRegDword != 0 );

    SataGpioVariableDword = 0x03040F00;
    WriteMem ( Bar5 + EMb, AccessWidth32, &SataGpioVariableDword);
    SataGpioVariableDword = 0x00008240;
    WriteMem ( Bar5 + EMb + 4, AccessWidth32, &SataGpioVariableDword);
    SataGpioVariableDword = 0x00000002;
    WriteMem ( Bar5 + EMb + 8, AccessWidth32, &SataGpioVariableDword);
    SataGpioVariableDword = 0x00800000;
    WriteMem ( Bar5 + EMb + 0x0C, AccessWidth32, &SataGpioVariableDword);
    SataGpioVariableDword = 0x0F003700;
    WriteMem ( Bar5 + EMb + 0x0C, AccessWidth32, &SataGpioVariableDword);
    RwMem ((Bar5 + 0x20), AccessWidth16, (UINT32)~(BIT8), BIT8);

    do {
      ReadMem (Bar5 + 0x20 , AccessWidth32, &FchSataBarRegDword);
      FchSataBarRegDword = FchSataBarRegDword & BIT8;
    } while ( FchSataBarRegDword != 0 );
  }
}

/**
 * FchInitMidProgramSataRegs - Sata Pci Configuration Space
 * register setting
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidProgramSataRegs (
  IN VOID  *FchDataPtr
  )
{
  UINT8        FchSataMsiCapability;
  UINT8        FchSataTargetSupport8Device;
  UINT8        FchSataDisableGenericMode;
  UINT8        FchSataSgpio0;
  UINT8        FchSataSgpio1;
  UINT8        FchSataPhyPllShutDown;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  FchSataMsiCapability = (UINT8) LocalCfgPtr->Sata.SataMsiCapability;
  FchSataTargetSupport8Device = (UINT8) LocalCfgPtr->Sata.SataTargetSupport8Device;
  FchSataDisableGenericMode = (UINT8) LocalCfgPtr->Sata.SataDisableGenericMode;
  FchSataSgpio0 = (UINT8) LocalCfgPtr->Sata.SataSgpio0;
  FchSataSgpio1 = (UINT8) LocalCfgPtr->Sata.SataSgpio1;
  FchSataPhyPllShutDown = (UINT8) LocalCfgPtr->Sata.SataPhyPllShutDown;

  if ((LocalCfgPtr->Sata.SataClass == SataNativeIde) || (LocalCfgPtr->Sata.SataClass == SataLegacyIde)) {
    FchSataMsiCapability = 0;
  }
  //
  // Enabled SATA MSI capability
  // SATA MSI and D3 Power State Capability
  //
  if ( FchSataMsiCapability ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x34), AccessWidth8, 0, 0x50, StdHeader);
  } else {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x34), AccessWidth8, 0, 0x70, StdHeader);
  }

  //
  // Disable SATA FLR Capability
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x70), AccessWidth16, 0x00FF, 0x00, StdHeader);

  //
  // Sata Target Support 8 devices function
  //
  if ( FchSataTargetSupport8Device ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth16, (UINT32)~BIT12, BIT12);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth16, (UINT32)~BIT12, 0x00);
  }

  //
  // Sata Generic Mode setting
  //
  if ( FchSataDisableGenericMode ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth16, (UINT32)~BIT13, BIT13);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xDA , AccessWidth16, (UINT32)~BIT13, 0x00);
  }

  //
  // Sata GPIO Initial
  //
  if ( FchSataSgpio0 ) {
    FchSataGpioInitial ( LocalCfgPtr );
  }

  if ( FchSataSgpio1 ) {
    FchSataGpioInitial ( LocalCfgPtr );
  }

  //
  // Sata Phy Pll Shutdown setting
  //
  if ( FchSataPhyPllShutDown ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x87), AccessWidth8, (UINT32)~(BIT6), BIT6, StdHeader);
  } else {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x87), AccessWidth8, (UINT32)~(BIT6), 0x00, StdHeader);
  }

  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x04C ), AccessWidth32, (UINT32) (~ (0x3f << 26)), (UINT32) (0x3f << 26), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x48), AccessWidth32, (UINT32) (~ (0x01 << 11)), (UINT32) (0x01 << 11), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + 0x084 ), AccessWidth32, (UINT32) (~ (0x01 << 31)), (UINT32) (0x00 << 31), StdHeader);
}


/**
 * FchInitLateProgramSataRegs - Sata Pci Configuration Space
 * register setting
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitLateProgramSataRegs (
  IN VOID  *FchDataPtr
  )
{
  UINT8        PortNumByte;
  UINT32       Bar5;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  SataBar5setting (LocalCfgPtr, &Bar5);
  //
  //Clear error status
  //
  RwMem ((Bar5 + FCH_SATA_BAR5_REG130), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + FCH_SATA_BAR5_REG1B0), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + FCH_SATA_BAR5_REG230), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + FCH_SATA_BAR5_REG2B0), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + FCH_SATA_BAR5_REG330), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + FCH_SATA_BAR5_REG3B0), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + 0x0430 ), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + 0x04B0 ), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);

  for ( PortNumByte = 0; PortNumByte < MAX_SATA_PORTS; PortNumByte++ ) {
    RwMem ((Bar5 + 0x110 + (PortNumByte * 0x80)), AccessWidth32, 0xFFFFFFFF, 0x00);
  }
}

/**
 * sataBar5RegSet - Sata Bar5 register setting
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
SataBar5RegSet (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       AndMaskDword;
  UINT32       OrMaskDword;
  UINT32       Bar5;
  UINT8        EfuseByte;
  UINT8        FchSataAggrLinkPmCap;
  UINT8        FchSataPortMultCap;
  UINT8        FchSataPscCap;
  UINT8        FchSataSscCap;
  UINT8        FchSataFisBasedSwitching;
  UINT8        FchSataCccSupport;
  UINT8        FchSataAhciEnclosureManagement;
  FCH_DATA_BLOCK         *LocalCfgPtr;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;

  FchSataAggrLinkPmCap = (UINT8) LocalCfgPtr->Sata.SataAggrLinkPmCap;
  FchSataPortMultCap = (UINT8) LocalCfgPtr->Sata.SataPortMultCap;
  FchSataPscCap = (UINT8) LocalCfgPtr->Sata.SataPscCap;
  FchSataSscCap = (UINT8) LocalCfgPtr->Sata.SataSscCap;
  FchSataFisBasedSwitching = (UINT8) LocalCfgPtr->Sata.SataFisBasedSwitching;
  FchSataCccSupport = (UINT8) LocalCfgPtr->Sata.SataCccSupport;
  FchSataAhciEnclosureManagement = (UINT8) LocalCfgPtr->Sata.SataAhciEnclosureManagement;

  AndMaskDword = 0;
  OrMaskDword = 0;
  Bar5 = 0;

  SataBar5setting (LocalCfgPtr, &Bar5);
  EfuseByte = SATA_FIS_BASE_EFUSE_LOC;
  GetEfuseStatus (&EfuseByte, LocalCfgPtr->StdHeader);

  if ( !FchSataPortMultCap ) {
    AndMaskDword |= BIT12;
  }

  if ( FchSataAggrLinkPmCap ) {
    OrMaskDword |= BIT11;
  } else {
    AndMaskDword |= BIT11;
  }

  if ( FchSataPscCap ) {
    OrMaskDword |= BIT1;
  } else {
    AndMaskDword |= BIT1;
  }

  if ( FchSataSscCap ) {
    OrMaskDword |= BIT26;
  } else {
    AndMaskDword |= BIT26;
  }

  if ( FchSataFisBasedSwitching ) {
    if (EfuseByte & BIT1) {
      AndMaskDword |= BIT10;
    } else {
      OrMaskDword |= BIT10;
    }
  } else {
    AndMaskDword |= BIT10;
  }

  //
  // Disabling CCC (Command Completion Coalescing) support.
  //
  if ( FchSataCccSupport ) {
    OrMaskDword |= BIT19;
  } else {
    AndMaskDword |= BIT19;
  }

  if ( FchSataAhciEnclosureManagement ) {
    OrMaskDword |= BIT27;
  } else {
    AndMaskDword |= BIT27;
  }

  RwMem ((Bar5 + 0xFC), AccessWidth32, ~AndMaskDword, OrMaskDword);

  //
  // SATA ESP port setting
  // These config bits are set for SATA driver to identify which ports are external SATA ports and need to
  // support hotplug. If a port is set as an external SATA port and need to support hotplug, then driver will
  // not enable power management (HIPM & DIPM) for these ports.
  //
  if ( LocalCfgPtr->Sata.SataEspPort.SataPortReg != 0 ) {
    RwMem ((Bar5 + 0xF8), AccessWidth32, ~(LocalCfgPtr->Sata.SataEspPort.SataPortReg), 0);
    RwMem ((Bar5 + 0xF8), AccessWidth32, 0xFF00FF00, (LocalCfgPtr->Sata.SataEspPort.SataPortReg << 16));
    //
    // External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set
    //
    RwMem ((Bar5 + 0xFC), AccessWidth32, (UINT32)~(BIT20), BIT20);
  } else {
    //
    // External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set (Clear for no ESP port)
    //
    RwMem ((Bar5 + 0xF8), AccessWidth32, 0xFF00FF00, 0x00);
    RwMem ((Bar5 + 0xFC), AccessWidth32, (UINT32)~(BIT20), 0x00);
  }

  if ( FchSataFisBasedSwitching ) {
    if (EfuseByte & BIT1) {
      RwMem ((Bar5 + 0xF8), AccessWidth32, 0x00FFFFFF, 0x00);
    } else {
      RwMem ((Bar5 + 0xF8), AccessWidth32, 0x00FFFFFF, 0xFF000000);
    }
  } else {
    RwMem ((Bar5 + 0xF8), AccessWidth32, 0x00FFFFFF, 0x00);
  }

  if ( LocalCfgPtr->Sata.BiosOsHandOff == 1 ) {
    RwMem ((Bar5 + 0x24), AccessWidth8, (UINT32)~BIT0, BIT0);
  } else {
    RwMem ((Bar5 + 0x24), AccessWidth8, (UINT32)~BIT0, 0x00);
  }
}

/**
 * FchSataSetDeviceNumMsi - Program Sata controller support
 * device number cap & MSI cap
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSataSetDeviceNumMsi (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                 Bar5;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  Bar5 = 0;
  SataBar5setting (LocalCfgPtr, &Bar5);
  //
  // RAID or AHCI
  //
  if (LocalCfgPtr->Sata.SataMode.IdeEnable == DISABLED) {
    //
    // IDE2 Controller is enabled
    //
    if (LocalCfgPtr->Sata.SataMode.Sata6AhciCap == ENABLED) {
      //
      // 6 AHCI mode
      //
      RwMem ((Bar5 + 0x0C), AccessWidth8, 0x00, 0x3F);
      RwMem ((Bar5 + 0x00), AccessWidth8, (UINT32)~(BIT2 + BIT1 + BIT0), BIT2 + BIT0);
      RwPci (((SATA_BUS_DEV_FUN << 16) + 0x50 + 2), AccessWidth8, (UINT32)~(BIT3 + BIT2 + BIT1), BIT2 + BIT1, StdHeader);
      RwMem ((Bar5 + 0xFC), AccessWidth8, 0x07, 0x30);
    } else {
      RwMem ((Bar5 + 0x0C), AccessWidth8, 0x00, 0x0F);
      if ( LocalCfgPtr->Sata.SataCccSupport ) {
        RwPci (((SATA_BUS_DEV_FUN << 16) + 0x50 + 2), AccessWidth8, (UINT32)~(BIT3 + BIT2 + BIT1), BIT2 + BIT1, StdHeader);
        RwMem ((Bar5 + 0xFC), AccessWidth8, 0x07, 0x20);
      } else {
        RwPci (((SATA_BUS_DEV_FUN << 16) + 0x50 + 2), AccessWidth8, (UINT32)~(BIT3 + BIT2 + BIT1), BIT2, StdHeader);
      }
    }
  } else {
    //
    // IDE2 Controller is disabled
    //
    RwMem ((Bar5 + 0x00), AccessWidth8, (UINT32)~(BIT2 + BIT1 + BIT0), BIT2 + BIT1 + BIT0);
    RwMem ((Bar5 + 0x0C), AccessWidth8, 0x00, 0xFF);
    if ( LocalCfgPtr->Sata.SataCccSupport ) {
      RwPci (((SATA_BUS_DEV_FUN << 16) + 0x50 + 2), AccessWidth8, (UINT32)~(BIT3 + BIT2 + BIT1), BIT3, StdHeader);
      RwMem ((Bar5 + 0xFC), AccessWidth8, 0x07, 0x40);
    } else {
      RwPci (((SATA_BUS_DEV_FUN << 16) + 0x50 + 2), AccessWidth8, (UINT32)~(BIT3 + BIT2 + BIT1), BIT2 + BIT1, StdHeader);
    }
  }
}


/**
 * FchSataDriveDetection - Sata drive detection
 *
 *   - Sata Ide & Sata Ide to Ahci only
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] *Bar5Ptr Sata BAR5 base address.
 *
 */
VOID
FchSataDriveDetection (
  IN  VOID       *FchDataPtr,
  IN  UINT32     *Bar5Ptr
  )
{
  UINT32       SataBarInfo;
  UINT8        PortNumByte;
  UINT8        SataPortType;
  UINT16       IoBaseWord;
  UINT32       SataLoopVarDWord;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  for ( PortNumByte = 0; PortNumByte < 4; PortNumByte++ ) {

    ReadMem (*Bar5Ptr + FCH_SATA_BAR5_REG128 + PortNumByte * 0x80, AccessWidth32, &SataBarInfo);

    if ( ( SataBarInfo & 0x0F ) == 0x03 ) {
      if ( PortNumByte & BIT0 ) {
        //
        //this port belongs to secondary channel
        //
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN << 16) + 0x18), AccessWidth16, &IoBaseWord, StdHeader);
      } else {
        //
        //this port belongs to primary channel
        //
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN << 16) + 0x10), AccessWidth16, &IoBaseWord, StdHeader);
      }

      //
      //if legacy ide mode, then the bar registers don't contain the correct values. So we need to hardcode them
      //
      if ( LocalCfgPtr->Sata.SataClass == SataLegacyIde ) {
        IoBaseWord = ( (0x170) | ((UINT16) ( (~((UINT8) (PortNumByte & BIT0) << 7)) & 0x80 )) );
      }

      if ( PortNumByte & BIT1 ) {
        //
        //this port is slave
        //
        SataPortType = 0xB0;
      } else {
        //
        //this port is master
        //
        SataPortType = 0xA0;
      }

      IoBaseWord &= 0xFFF8;
      LibAmdIoWrite (AccessWidth8, IoBaseWord + 6, &SataPortType, StdHeader);

      //
      //Wait in loop for 30s for the drive to become ready
      //
      for ( SataLoopVarDWord = 0; SataLoopVarDWord < 300000; SataLoopVarDWord++ ) {
        LibAmdIoRead (AccessWidth8, IoBaseWord + 7, &SataPortType, StdHeader);
        if ( (SataPortType & 0x88) == 0 ) {
          break;
        }
        FchStall (100, StdHeader);
      }
    }
  }
}

/**
 * FchShutdownUnconnectedSataPortClock - Shutdown unconnected
 * Sata port clock
 *
 *   - Sata Ide & Sata Ide to Ahci only
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 * @param[in] Bar5 Sata BAR5 base address.
 *
 */
VOID
FchShutdownUnconnectedSataPortClock (
  IN  VOID       *FchDataPtr,
  IN  UINT32     Bar5
  )
{
  UINT8                  PortNumByte;
  UINT8                  PortSataStatusByte;
  UINT8                  FchSataClkAutoOff;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  FchSataClkAutoOff = (UINT8) LocalCfgPtr->Sata.SataClkAutoOff;

  //
  // Enable SATA auto clock control by default
  //
  for ( PortNumByte = 0; PortNumByte < MAX_SATA_PORTS; PortNumByte++ ) {
    ReadMem (Bar5 + FCH_SATA_BAR5_REG128 + (PortNumByte * 0x80), AccessWidth8, &PortSataStatusByte);
    //
    // Shutdown the clock for the port and do the necessary port reporting changes.
    // Error port status should be 1 not 3
    //
    if ( ((PortSataStatusByte & 0x0F) != 0x03) && (! ((LocalCfgPtr->Sata.SataEspPort.SataPortReg) & (1 << PortNumByte))) ) {
      RwPci (((SATA_BUS_DEV_FUN << 16) + 0x040  + 2), AccessWidth8, 0xFF, (1 << PortNumByte), StdHeader);
      RwMem (Bar5 + 0x0C, AccessWidth8, ~(1 << PortNumByte), 00);
    }
  }                                            ///end of for (PortNumByte=0;PortNumByte<6;PortNumByte++)

  //
  //Set the Ports Implemented register
  //if all ports are in disabled state, report at least one port
  //
  ReadMem (Bar5 + 0x0C, AccessWidth8, &PortSataStatusByte);
  if ( (PortSataStatusByte & 0xFF) == 0) {
    RwMem (Bar5 + 0x0C, AccessWidth8, (UINT32) ~(0xFF), 01);
  }

  //
  // Number of Ports (NP): 0’s based value indicating the maximum number
  //  of ports supported by the HBA silicon.
  //
  RwMem (Bar5 + 0x00, AccessWidth8, 0xE0, MAX_SATA_PORTS - 1);
}

/**
 * FchSataSetPortGenMode - Set Sata port mode (each) for
 * Gen1/Gen2/Gen3
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSataSetPortGenMode (
  IN  VOID     *FchDataPtr
  )
{
  UINT32       Bar5;
  UINT8        PortNumByte;
  UINT8        PortModeByte;
  UINT16       SataPortMode;
  BOOLEAN      FchSataHotRemovalEnh;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  FchSataHotRemovalEnh = LocalCfgPtr->Sata.SataHotRemovalEnh;

  SataBar5setting (LocalCfgPtr, &Bar5);
  SataPortMode = (UINT16)LocalCfgPtr->Sata.SataPortMd.SataPortMode;
  PortNumByte = 0;

  while ( PortNumByte < 8 ) {
    PortModeByte = (UINT8) (SataPortMode & 3);
    if ( (PortModeByte == BIT0) || (PortModeByte == BIT1) ) {
      if ( PortModeByte == BIT0 ) {
        //
        // set GEN 1
        //
        RwMem (Bar5 + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80, AccessWidth8, 0x0F, 0x10);
      }

      if ( PortModeByte == BIT1 ) {
        //
        // set GEN2 (default is GEN3)
        //
        RwMem (Bar5 + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80, AccessWidth8, 0x0F, 0x20);
      }

      RwMem (Bar5 + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80, AccessWidth8, 0xFF, 0x01);
    }

    SataPortMode >>= 2;
    PortNumByte ++;
  }

  FchStall (1000, StdHeader);
  SataPortMode = (UINT16)LocalCfgPtr->Sata.SataPortMd.SataPortMode;
  PortNumByte = 0;

  while ( PortNumByte < 8 ) {
    PortModeByte = (UINT8) (SataPortMode & 3);

    if ( (PortModeByte == BIT0) || (PortModeByte == BIT1) ) {
      RwMem (Bar5 + FCH_SATA_BAR5_REG12C + PortNumByte * 0x80, AccessWidth8, 0xFE, 0x00);
    }

    PortNumByte ++;
    SataPortMode >>= 2;
  }

  //
  // Sata Hot Removal Enhance setting
  //
  if ( FchSataHotRemovalEnh ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x80), AccessWidth16, (UINT32)~BIT8, BIT8, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x0A8 ), AccessWidth16, (UINT32)~BIT0, BIT0, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x80), AccessWidth16, (UINT32)~BIT8, 0, StdHeader);
  }
}

