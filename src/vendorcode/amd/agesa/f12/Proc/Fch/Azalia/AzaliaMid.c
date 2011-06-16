/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH HD Audio Controller
 *
 * Init Azalia Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 47028 $   @e \$Date: 2011-02-15 03:00:55 +0800 (Tue, 15 Feb 2011) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE PROC_FCH_AZALIA_AZALIAMID_FILECODE
//
// Declaration of local functions
//
VOID
ConfigureAzaliaPinCmd (
  IN FCH_DATA_BLOCK  *FchDataPtr,
  IN UINT32 BAR0,
  IN UINT8 ChannelNum
  );

VOID
ConfigureAzaliaSetConfigD4Dword (
  IN CODEC_ENTRY  *TempAzaliaCodecEntryPtr,
  IN UINT32 ChannelNumDword,
  IN UINT32 BAR0,
  IN AMD_CONFIG_PARAMS *StdHeader
  );

/**
 * FchInitMidAzalia - Config Azalia controller after PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitMidAzalia (
  IN  VOID     *FchDataPtr
  )
{
  UINT8        Data;
  UINT8        Index;
  BOOLEAN      EnableAzalia;
  UINT32       PinRouting;
  UINT8        ChannelNum;
  UINT8        AzaliaTempVariableByte;
  UINT16       AzaliaTempVariableWord;
  UINT32       BAR0;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  AMD_CONFIG_PARAMS      *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  EnableAzalia = FALSE;
  ChannelNum = 0;
  AzaliaTempVariableByte = 0;
  AzaliaTempVariableWord = 0;
  BAR0 = 0;

  if ( LocalCfgPtr->Azalia.AzaliaEnable == AzDisable ) {
    return;
  } else {
    RwPci ((AZALIA_BUS_DEV_FUN << 16) + FCH_AZ_REG04, AccessWidth8, ~BIT1, BIT1, StdHeader);

    if ( LocalCfgPtr->Azalia.AzaliaSsid != NULL ) {
      RwPci ((AZALIA_BUS_DEV_FUN << 16) + FCH_AZ_REG2C, AccessWidth32, 0x00, LocalCfgPtr->Azalia.AzaliaSsid, StdHeader);
    }

    ReadPci ((AZALIA_BUS_DEV_FUN << 16) + FCH_AZ_REG10, AccessWidth32, &BAR0, StdHeader);

    if ( BAR0 != 0 ) {
      if ( BAR0 != 0xFFFFFFFF ) {
        BAR0 &=  ~(0x03FFF);
        EnableAzalia = TRUE;
      }
    }
  }

  if ( EnableAzalia ) {
    //
    // Get SDIN Configuration
    //
    if ( LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin0 == 2 ) {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG167, AccessWidth8, 0, 0x3E);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG167, AccessWidth8, 0, 0x00);
    } else {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG167, AccessWidth8, 0, 0x0);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG167, AccessWidth8, 0, 0x01);
    }

    if ( LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin1 == 2 ) {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG168, AccessWidth8, 0, 0x3E);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG168, AccessWidth8, 0, 0x00);
    } else {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG168, AccessWidth8, 0, 0x0);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG168, AccessWidth8, 0, 0x01);
    }

    if ( LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin2 == 2 ) {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG169, AccessWidth8, 0, 0x3E);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG169, AccessWidth8, 0, 0x00);
    } else {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG169, AccessWidth8, 0, 0x0);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG169, AccessWidth8, 0, 0x01);
    }

    if ( LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin3 == 2 ) {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG170, AccessWidth8, 0, 0x3E);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG170, AccessWidth8, 0, 0x00);
    } else {
      RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GPIO_REG170, AccessWidth8, 0, 0x0);
      RwMem (ACPI_MMIO_BASE + IOMUX_BASE + FCH_GPIO_REG170, AccessWidth8, 0, 0x01);
    }
    //
    // INT#A Azalia resource
    //
    Data = 0x93;                                                                         /// Azalia APIC index
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC00, &Data, StdHeader);
    Data = 0x10;                                                                         /// IRQ16 (INTA#)
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGC01, &Data, StdHeader);

    Index = 11;
    do {
      ReadMem ( BAR0 + FCH_AZ_BAR_REG08, AccessWidth8, &AzaliaTempVariableByte);
      AzaliaTempVariableByte |= BIT0;
      WriteMem (BAR0 + FCH_AZ_BAR_REG08, AccessWidth8, &AzaliaTempVariableByte);
      FchStall (1000, StdHeader);
      ReadMem (BAR0 + FCH_AZ_BAR_REG08, AccessWidth8, &AzaliaTempVariableByte);
      Index--;
    }  while ((! (AzaliaTempVariableByte & BIT0)) && (Index > 0) );

    if ( Index == 0 ) {
      return;
    }

    FchStall (1000, StdHeader);
    ReadMem ( BAR0 + FCH_AZ_BAR_REG0E, AccessWidth16, &AzaliaTempVariableWord);
    if ( AzaliaTempVariableWord & 0x0F ) {

      //
      //at least one azalia codec found
      //
      //PinRouting = LocalCfgPtr->Azalia.AZALIA_CONFIG.AzaliaSdinPin;
      //new structure need make up PinRouting
      //need adjust later!!!
      //
      PinRouting = 0;
      PinRouting = (UINT32 )LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin3;
      PinRouting <<= 8;
      PinRouting |= (UINT32 )LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin2;
      PinRouting <<= 8;
      PinRouting |= (UINT32 )LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin1;
      PinRouting <<= 8;
      PinRouting |= (UINT32 )LocalCfgPtr->Azalia.AzaliaConfig.AzaliaSdin0;

      do {
        if ( ( ! (PinRouting & BIT0) ) && (PinRouting & BIT1) ) {
          ConfigureAzaliaPinCmd (LocalCfgPtr, BAR0, ChannelNum);
        }
        PinRouting >>= 8;
        ChannelNum++;
      }  while ( ChannelNum != 4 );
    } else {
      //
      //No Azalia codec found
      //
      if ( LocalCfgPtr->Azalia.AzaliaEnable != AzEnable ) {
        EnableAzalia = FALSE;                      ///set flag to disable Azalia
      }
    }
  }

  if ( EnableAzalia ) {
    //
    //redo clear reset
    //
    do {
      AzaliaTempVariableWord = 0;
      WriteMem ( BAR0 + FCH_AZ_BAR_REG0C, AccessWidth16, &AzaliaTempVariableWord);
      ReadMem (BAR0 + FCH_AZ_BAR_REG08, AccessWidth8, &AzaliaTempVariableByte);
      AzaliaTempVariableByte &= ~(BIT0);
      WriteMem (BAR0 + FCH_AZ_BAR_REG08, AccessWidth8, &AzaliaTempVariableByte);
      ReadMem (BAR0 + FCH_AZ_BAR_REG08, AccessWidth8, &AzaliaTempVariableByte);
    }  while ( AzaliaTempVariableByte & BIT0 );

    if ( LocalCfgPtr->Azalia.AzaliaSnoop == 1 ) {
      RwPci ((AZALIA_BUS_DEV_FUN << 16) + FCH_AZ_REG42, AccessWidth8, 0xFF, BIT1 + BIT0, StdHeader);
    }
  } else {
    //
    //disable Azalia controller
    //
    RwPci ((AZALIA_BUS_DEV_FUN << 16) + FCH_AZ_REG04, AccessWidth16, 0, 0, StdHeader);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEB, AccessWidth8, ~BIT0, 0);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEB, AccessWidth8, ~BIT0, 0);
  }
}

/**
 * Pin Config for ALC880, ALC882 and ALC883.
 *
 *
 *
 */
CODEC_ENTRY AzaliaCodecAlc882Table[] =
{
  {0x14, 0x01014010},
  {0x15, 0x01011012},
  {0x16, 0x01016011},
  {0x17, 0x01012014},
  {0x18, 0x01A19030},
  {0x19, 0x411111F0},
  {0x1a, 0x01813080},
  {0x1b, 0x411111F0},
  {0x1C, 0x411111F0},
  {0x1d, 0x411111F0},
  {0x1e, 0x01441150},
  {0x1f, 0x01C46160},
  {0xff, 0xffffffff}
};

/**
 * Pin Config for ALC0262.
 *
 *
 *
 */
CODEC_ENTRY AzaliaCodecAlc262Table[] =
{
  {0x14, 0x01014010},
  {0x15, 0x411111F0},
  {0x16, 0x411111F0},
  {0x18, 0x01A19830},
  {0x19, 0x02A19C40},
  {0x1a, 0x01813031},
  {0x1b, 0x02014C20},
  {0x1c, 0x411111F0},
  {0x1d, 0x411111F0},
  {0x1e, 0x0144111E},
  {0x1f, 0x01C46150},
  {0xff, 0xffffffff}
};

/**
 * Pin Config for ALC0269.
 *
 *
 *
 */
CODEC_ENTRY AzaliaCodecAlc269Table[] =
{
  {0x12, 0x99A308F0},
  {0x14, 0x99130010},
  {0x15, 0x0121101F},
  {0x16, 0x99036120},
  {0x18, 0x01A19850},
  {0x19, 0x99A309F0},
  {0x1a, 0x01813051},
  {0x1b, 0x0181405F},
  {0x1d, 0x40134601},
  {0x1e, 0x01442130},
  {0x11, 0x99430140},
  {0x20, 0x0030FFFF},
  {0xff, 0xffffffff}
};

/**
 * Pin Config for ALC0861.
 *
 *
 *
 */
CODEC_ENTRY AzaliaCodecAlc861Table[] =
{
  {0x01, 0x8086C601},
  {0x0B, 0x01014110},
  {0x0C, 0x01813140},
  {0x0D, 0x01A19941},
  {0x0E, 0x411111F0},
  {0x0F, 0x02214420},
  {0x10, 0x02A1994E},
  {0x11, 0x99330142},
  {0x12, 0x01451130},
  {0x1F, 0x411111F0},
  {0x20, 0x411111F0},
  {0x23, 0x411111F0},
  {0xff, 0xffffffff}
};

/**
 * Pin Config for ALC0889.
 *
 *
 *
 */
CODEC_ENTRY AzaliaCodecAlc889Table[] =
{
  {0x11, 0x411111F0},
  {0x14, 0x01014010},
  {0x15, 0x01011012},
  {0x16, 0x01016011},
  {0x17, 0x01013014},
  {0x18, 0x01A19030},
  {0x19, 0x411111F0},
  {0x1a, 0x411111F0},
  {0x1b, 0x411111F0},
  {0x1C, 0x411111F0},
  {0x1d, 0x411111F0},
  {0x1e, 0x01442150},
  {0x1f, 0x01C42160},
  {0xff, 0xffffffff}
};

/**
 * Pin Config for ADI1984.
 *
 *
 *
 */
CODEC_ENTRY AzaliaCodecAd1984Table[] =
{
  {0x11, 0x0221401F},
  {0x12, 0x90170110},
  {0x13, 0x511301F0},
  {0x14, 0x02A15020},
  {0x15, 0x50A301F0},
  {0x16, 0x593301F0},
  {0x17, 0x55A601F0},
  {0x18, 0x55A601F0},
  {0x1A, 0x91F311F0},
  {0x1B, 0x014511A0},
  {0x1C, 0x599301F0},
  {0xff, 0xffffffff}
};

/**
 * FrontPanel Config table list
 *
 *
 *
 */
CODEC_ENTRY FrontPanelAzaliaCodecTableList[] =
{
  {0x19, 0x02A19040},
  {0x1b, 0x02214020},
  {0xff, 0xffffffff}
};

/**
 * Current HD Audio support codec list
 *
 *
 *
 */
CODEC_TBL_LIST AzaliaCodecTableList[] =
{
  {0x010ec0880, &AzaliaCodecAlc882Table[0]},
  {0x010ec0882, &AzaliaCodecAlc882Table[0]},
  {0x010ec0883, &AzaliaCodecAlc882Table[0]},
  {0x010ec0885, &AzaliaCodecAlc882Table[0]},
  {0x010ec0889, &AzaliaCodecAlc889Table[0]},
  {0x010ec0262, &AzaliaCodecAlc262Table[0]},
  {0x010ec0269, &AzaliaCodecAlc269Table[0]},
  {0x010ec0861, &AzaliaCodecAlc861Table[0]},
  {0x011d41984, &AzaliaCodecAd1984Table[0]},
  { (UINT32) 0x0FFFFFFFF, (CODEC_ENTRY*) (UINTN)0x0FFFFFFFF}
};

/**
 * ConfigureAzaliaPinCmd - Configuration HD Audio PIN Command
 *
 *
 * @param[in] FchDataPtr              Fch configuration structure pointer.
 * @param[in] BAR0               HD Audio BAR0 base address.
 * @param[in] ChannelNum         Channel Number.
 *
 */
VOID
ConfigureAzaliaPinCmd (
  IN  FCH_DATA_BLOCK     *FchDataPtr,
  IN  UINT32         BAR0,
  IN  UINT8          ChannelNum
  )
{
  UINT32           AzaliaTempVariable;
  UINT32           ChannelNumDword;
  CODEC_TBL_LIST   *TempAzaliaOemCodecTablePtr;
  CODEC_ENTRY      *TempAzaliaCodecEntryPtr;

  if ( (FchDataPtr->Azalia.AzaliaPinCfg) != 1 ) {
    return;
  }

  ChannelNumDword = ChannelNum << 28;
  AzaliaTempVariable = 0xF0000;
  AzaliaTempVariable |= ChannelNumDword;

  WriteMem (BAR0 + FCH_AZ_BAR_REG60, AccessWidth32, &AzaliaTempVariable);
  FchStall (600, FchDataPtr->StdHeader);
  ReadMem (BAR0 + FCH_AZ_BAR_REG64, AccessWidth32, &AzaliaTempVariable);

  if ( ((FchDataPtr->Azalia.AzaliaOemCodecTablePtr) == NULL) || ((FchDataPtr->Azalia.AzaliaOemCodecTablePtr) == ((CODEC_TBL_LIST*) (UINTN)0xFFFFFFFF))) {
    TempAzaliaOemCodecTablePtr = (CODEC_TBL_LIST*) (&AzaliaCodecTableList[0]);
  } else {
    TempAzaliaOemCodecTablePtr = (CODEC_TBL_LIST*) FchDataPtr->Azalia.AzaliaOemCodecTablePtr;
  }

  while ( TempAzaliaOemCodecTablePtr->CodecId != 0xFFFFFFFF ) {
    if ( TempAzaliaOemCodecTablePtr->CodecId == AzaliaTempVariable ) {
      break;
    } else {
      ++TempAzaliaOemCodecTablePtr;
    }
  }

  if ( TempAzaliaOemCodecTablePtr->CodecId != 0xFFFFFFFF ) {
    TempAzaliaCodecEntryPtr = (CODEC_ENTRY*) TempAzaliaOemCodecTablePtr->CodecTablePtr;
    if ( ((FchDataPtr->Azalia.AzaliaOemCodecTablePtr) == NULL) || ((FchDataPtr->Azalia.AzaliaOemCodecTablePtr) == ((CODEC_TBL_LIST*) (UINTN)0xFFFFFFFF)) ) {
      TempAzaliaCodecEntryPtr = (CODEC_ENTRY*) (TempAzaliaCodecEntryPtr);
    }

    ConfigureAzaliaSetConfigD4Dword (TempAzaliaCodecEntryPtr, ChannelNumDword, BAR0, FchDataPtr->StdHeader);

    if ( FchDataPtr->Azalia.AzaliaFrontPanel != 1 ) {
      if ( (FchDataPtr->Azalia.AzaliaFrontPanel == 2) || (FchDataPtr->Azalia.FrontPanelDetected == 1) ) {
        if ( ((FchDataPtr->Azalia.AzaliaOemFpCodecTablePtr) == NULL) || ((FchDataPtr->Azalia.AzaliaOemFpCodecTablePtr) == (VOID*) (UINTN)0xFFFFFFFF) ) {
          TempAzaliaCodecEntryPtr = (CODEC_ENTRY*) (&FrontPanelAzaliaCodecTableList[0]);
        } else {
          TempAzaliaCodecEntryPtr = (CODEC_ENTRY*) FchDataPtr->Azalia.AzaliaOemFpCodecTablePtr;
        }

        ConfigureAzaliaSetConfigD4Dword (TempAzaliaCodecEntryPtr, ChannelNumDword, BAR0, FchDataPtr->StdHeader);
      }
    }
  }
}

/**
 * ConfigureAzaliaSetConfigD4Dword - Configuration HD Audio Codec table
 *
 *
 * @param[in] TempAzaliaCodecEntryPtr    HD Audio Codec table structure pointer.
 * @param[in] ChannelNumDword               HD Audio Channel Number.
 * @param[in] BAR0                     HD Audio BAR0 base address.
 * @param[in] StdHeader
 *
 */
VOID
ConfigureAzaliaSetConfigD4Dword (
  IN  CODEC_ENTRY     *TempAzaliaCodecEntryPtr,
  IN  UINT32          ChannelNumDword,
  IN  UINT32          BAR0,
  IN  AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8    TempByte1;
  UINT8    TempByte2;
  UINT8    Index;
  UINT32   TempDword1;
  UINT32   TempDword2;

  TempDword1 = 0;
  TempDword2 = 0;

  while ( (TempAzaliaCodecEntryPtr->Nid) != 0xFF ) {
    TempByte1 = 0x20;
    if ( (TempAzaliaCodecEntryPtr->Nid) == 0x1 ) {
      TempByte1 = 0x24;
    }

    TempDword1 =  TempAzaliaCodecEntryPtr->Nid;
    TempDword1 &= 0xff;
    TempDword1 <<= 20;
    TempDword1 |= ChannelNumDword;
    TempDword1 |= (0x700 << 8);

    for ( Index = 4; Index > 0; Index-- ) {
      do {
        ReadMem (BAR0 + FCH_AZ_BAR_REG68, AccessWidth32, &TempDword2);
      } while ( (TempDword2 & BIT0) != 0 );

      TempByte2 = (UINT8) (( (TempAzaliaCodecEntryPtr->Byte40) >> ((4 - Index) * 8 ) ) & 0xff);
      TempDword1 =  (TempDword1 & 0xFFFF0000) + ((TempByte1 - Index) << 8) + TempByte2;
      WriteMem (BAR0 + FCH_AZ_BAR_REG60, AccessWidth32, &TempDword1);
      FchStall (60, StdHeader);
    }

    ++TempAzaliaCodecEntryPtr;
  }
}
