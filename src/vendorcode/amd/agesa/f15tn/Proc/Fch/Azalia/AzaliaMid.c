/* $NoKeywords:$ */
/**
 * @file
 *
 * Config FCH HD Audio Controller
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
****************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE (0xB003)
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

  if ( LocalCfgPtr->Azalia.AzaliaEnable == AzDisable) {
    return;
  } else {
    RwPci ((((0x14<<3)+2) << 16) + 0x04, AccessWidth8, (UINT32)~BIT1, (UINT32)BIT1, StdHeader);

    if ( LocalCfgPtr->Azalia.AzaliaSsid != 0 ) {
      RwPci ((((0x14<<3)+2) << 16) + 0x2C, AccessWidth32, 0x00, LocalCfgPtr->Azalia.AzaliaSsid, StdHeader);
    }

    ReadPci ((((0x14<<3)+2)  << 16) + 0x10, AccessWidth32, &BAR0, StdHeader);

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

    Index = 11;
    do {
      ReadMem ( BAR0 + 0x08, AccessWidth8, &AzaliaTempVariableByte);
      AzaliaTempVariableByte |= BIT0;
      WriteMem (BAR0 + 0x08, AccessWidth8, &AzaliaTempVariableByte);
      FchStall (1000, StdHeader);
      ReadMem (BAR0 + 0x08, AccessWidth8, &AzaliaTempVariableByte);
      Index--;
    }  while ((! (AzaliaTempVariableByte & BIT0)) && (Index > 0) );

    if ( Index == 0 ) {
      return;
    }

    FchStall (1000, StdHeader);
    ReadMem ( BAR0 + 0x0E, AccessWidth16, &AzaliaTempVariableWord);
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
    if ( LocalCfgPtr->Azalia.AzaliaSnoop == 1 ) {
      RwPci ((((0x14<<3)+2)  << 16) + 0x42, AccessWidth8, 0xFF, BIT1 + BIT0, StdHeader);
    }
  } else {
    //
    //disable Azalia controller
    //
    RwPci ((((0x14<<3)+2)  << 16) + 0x04, AccessWidth16, 0, 0, StdHeader);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xEB , AccessWidth8, (UINT32)~BIT0, 0);
    RwMem (ACPI_MMIO_BASE + PMIO_BASE + 0xEB , AccessWidth8, (UINT32)~BIT0, 0);
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

  WriteMem (BAR0 + 0x60, AccessWidth32, &AzaliaTempVariable);
  FchStall (600, FchDataPtr->StdHeader);
  ReadMem (BAR0 + 0x64, AccessWidth32, &AzaliaTempVariable);

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
        ReadMem (BAR0 + 0x68, AccessWidth32, &TempDword2);
      } while ( (TempDword2 & BIT0) != 0 );

      TempByte2 = (UINT8) (( (TempAzaliaCodecEntryPtr->Byte40) >> ((4 - Index) * 8 ) ) & 0xff);
      TempDword1 =  (TempDword1 & 0xFFFF0000) + ((TempByte1 - Index) << 8) + TempByte2;
      WriteMem (BAR0 + 0x60, AccessWidth32, &TempDword1);
      FchStall (60, StdHeader);
    }

    ++TempAzaliaCodecEntryPtr;
  }
}


