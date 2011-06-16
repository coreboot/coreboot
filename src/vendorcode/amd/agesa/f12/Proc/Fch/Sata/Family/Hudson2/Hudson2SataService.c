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
 * @e \$Revision: 48230 $   @e \$Date: 2011-03-05 06:55:12 +0800 (Sat, 05 Mar 2011) $
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
UINT8  NumOfSataPorts = 8;

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
  ReadMem (Bar5 + FCH_SATA_BAR5_REG1C , AccessWidth32, &FchSataBarRegDword);
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
    RwMem ((Bar5 + FCH_SATA_BAR5_REG20), AccessWidth16, ~(BIT8), BIT8);

    do {
      ReadMem (Bar5 + FCH_SATA_BAR5_REG20 , AccessWidth32, &FchSataBarRegDword);
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
    RwMem ((Bar5 + FCH_SATA_BAR5_REG20), AccessWidth16, ~(BIT8), BIT8);

    do {
      ReadMem (Bar5 + FCH_SATA_BAR5_REG20 , AccessWidth32, &FchSataBarRegDword);
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

  //
  // Enabled SATA MSI capability
  // SATA MSI and D3 Power State Capability
  //
  if ( FchSataMsiCapability ) {
    RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG34), AccessWidth8, 0, 0x50, StdHeader);
  } else {
    RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG34), AccessWidth8, 0, 0x70, StdHeader);
  }

  //
  // Disable SATA FLR Capability
  //
  RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG70), AccessWidth16, 0x00FF, 0x00, StdHeader);

  //
  // Sata Target Support 8 devices function
  //
  if ( FchSataTargetSupport8Device ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDA, AccessWidth16, ~BIT12, BIT12);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDA, AccessWidth16, ~BIT12, 0x00);
  }

  //
  // Sata Generic Mode setting
  //
  if ( FchSataDisableGenericMode ) {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDA, AccessWidth16, ~BIT13, BIT13);
  } else {
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGDA, AccessWidth16, ~BIT13, 0x00);
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
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x87), AccessWidth8, ~(BIT6), BIT6, StdHeader);
  } else {
    RwPci (((SATA_BUS_DEV_FUN << 16) + 0x87), AccessWidth8, ~(BIT6), 0x00, StdHeader);
  }

  RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG4C), AccessWidth32, (UINT32) (~ (0x3f << 26)), (UINT32) (0x3f << 26), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG48), AccessWidth32, (UINT32) (~ (0x01 << 11)), (UINT32) (0x01 << 11), StdHeader);
  RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG84), AccessWidth32, (UINT32) (~ (0x01 << 31)), (UINT32) (0x00 << 31), StdHeader);
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
  RwMem ((Bar5 + FCH_SATA_BAR5_REG430), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);
  RwMem ((Bar5 + FCH_SATA_BAR5_REG4B0), AccessWidth32, 0xFFFFFFFF, 0xFFFFFFFF);

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

  RwMem ((Bar5 + FCH_SATA_BAR5_REGFC), AccessWidth32, ~AndMaskDword, OrMaskDword);

  //
  // SATA ESP port setting
  // These config bits are set for SATA driver to identify which ports are external SATA ports and need to
  // support hotplug. If a port is set as an external SATA port and need to support hotplug, then driver will
  // not enable power management (HIPM & DIPM) for these ports.
  //
  if ( LocalCfgPtr->Sata.SataEspPort.SataPortReg != 0 ) {
    RwMem ((Bar5 + FCH_SATA_BAR5_REGF8), AccessWidth32, ~(LocalCfgPtr->Sata.SataEspPort.SataPortReg), 0);
    RwMem ((Bar5 + FCH_SATA_BAR5_REGF8), AccessWidth32, 0xFF00FF00, (LocalCfgPtr->Sata.SataEspPort.SataPortReg << 16));
    //
    // External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set
    //
    RwMem ((Bar5 + FCH_SATA_BAR5_REGFC), AccessWidth32, ~(BIT20), BIT20);
  } else {
    //
    // External SATA Port Indication Registers
    // If any of the ports was programmed as an external port, HCAP.SXS should also be set (Clear for no ESP port)
    //
    RwMem ((Bar5 + FCH_SATA_BAR5_REGF8), AccessWidth32, 0xFF00FF00, 0x00);
    RwMem ((Bar5 + FCH_SATA_BAR5_REGFC), AccessWidth32, ~(BIT20), 0x00);
  }

  if ( FchSataFisBasedSwitching ) {
    if (EfuseByte & BIT1) {
      RwMem ((Bar5 + FCH_SATA_BAR5_REGF8), AccessWidth32, 0x00FFFFFF, 0x00);
    } else {
      RwMem ((Bar5 + FCH_SATA_BAR5_REGF8), AccessWidth32, 0x00FFFFFF, 0xFF000000);
    }
  } else {
    RwMem ((Bar5 + FCH_SATA_BAR5_REGF8), AccessWidth32, 0x00FFFFFF, 0x00);
  }

  if ( LocalCfgPtr->Sata.BiosOsHandOff == 1 ) {
    RwMem ((Bar5 + FCH_SATA_BAR5_REG24), AccessWidth8, ~BIT0, BIT0);
  } else {
    RwMem ((Bar5 + FCH_SATA_BAR5_REG24), AccessWidth8, ~BIT0, 0x00);
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
      RwMem ((Bar5 + FCH_SATA_BAR5_REG0C), AccessWidth8, 0x00, 0x3F);
      RwMem ((Bar5 + FCH_SATA_BAR5_REG00), AccessWidth8, ~(BIT2 + BIT1 + BIT0), BIT2 + BIT0);
      RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG50 + 2), AccessWidth8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1, StdHeader);
      RwMem ((Bar5 + FCH_SATA_BAR5_REGFC), AccessWidth8, 0x07, 0x30);
    } else {
      RwMem ((Bar5 + FCH_SATA_BAR5_REG0C), AccessWidth8, 0x00, 0x0F);
      if ( LocalCfgPtr->Sata.SataCccSupport ) {
        RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG50 + 2), AccessWidth8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1, StdHeader);
        RwMem ((Bar5 + FCH_SATA_BAR5_REGFC), AccessWidth8, 0x07, 0x20);
      } else {
        RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG50 + 2), AccessWidth8, ~(BIT3 + BIT2 + BIT1), BIT2, StdHeader);
      }
    }
  } else {
    //
    // IDE2 Controller is disabled
    //
    RwMem ((Bar5 + FCH_SATA_BAR5_REG00), AccessWidth8, ~(BIT2 + BIT1 + BIT0), BIT2 + BIT1 + BIT0);
    RwMem ((Bar5 + FCH_SATA_BAR5_REG0C), AccessWidth8, 0x00, 0xFF);
    if ( LocalCfgPtr->Sata.SataCccSupport ) {
      RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG50 + 2), AccessWidth8, ~(BIT3 + BIT2 + BIT1), BIT3, StdHeader);
      RwMem ((Bar5 + FCH_SATA_BAR5_REGFC), AccessWidth8, 0x07, 0x40);
    } else {
      RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG50 + 2), AccessWidth8, ~(BIT3 + BIT2 + BIT1), BIT2 + BIT1, StdHeader);
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
  UINT16       SataLoopVarWord;
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
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG18), AccessWidth16, &IoBaseWord, StdHeader);
      } else {
        //
        //this port belongs to primary channel
        //
        ReadPci (((UINT32) (SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG10), AccessWidth16, &IoBaseWord, StdHeader);
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
      for ( SataLoopVarWord = 0; SataLoopVarWord < 300000; SataLoopVarWord++ ) {
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
  UINT8                  NumOfPorts;
  UINT8                  FchSataClkAutoOff;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  FchSataClkAutoOff = (UINT8) LocalCfgPtr->Sata.SataClkAutoOff;

  NumOfPorts = 0;
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
      RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG40 + 2), AccessWidth8, 0xFF, (1 << PortNumByte), StdHeader);
      RwMem (Bar5 + FCH_SATA_BAR5_REG0C, AccessWidth8, ~(1 << PortNumByte), 00);
    }
  }                                            ///end of for (PortNumByte=0;PortNumByte<6;PortNumByte++)

  ReadMem (Bar5 + FCH_SATA_BAR5_REG0C, AccessWidth8, &PortSataStatusByte);

  //
  //if all ports are in disabled state, report at least one port
  //
  if ( (PortSataStatusByte & 0xFF) == 0) {
    RwMem (Bar5 + FCH_SATA_BAR5_REG0C, AccessWidth8, (UINT32) ~(0xFF), 01);
  }

  ReadMem (Bar5 + FCH_SATA_BAR5_REG0C, AccessWidth8, &PortSataStatusByte);

  for (PortNumByte = 0; PortNumByte < MAX_SATA_PORTS; PortNumByte ++) {
    if (PortSataStatusByte & (1 << PortNumByte)) {
      NumOfPorts++;
    }
  }

  if ( NumOfPorts == 0) {
    NumOfPorts = 0x01;
  }

  RwMem (Bar5 + FCH_SATA_BAR5_REG00, AccessWidth8, 0xE0, NumOfPorts - 1);
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
    RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG80), AccessWidth16, ~BIT8, BIT8, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REGA8), AccessWidth16, ~BIT0, BIT0, StdHeader);
    RwPci (((SATA_BUS_DEV_FUN << 16) + FCH_SATA_REG80), AccessWidth16, ~BIT8, 0, StdHeader);
  }
}

