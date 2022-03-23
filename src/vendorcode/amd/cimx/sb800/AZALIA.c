/**
 * @file
 *
 * Config Southbridge HD Audio Controller
 *
 *
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

VOID configureAzaliaPinCmd (IN AMDSBCFG* pConfig, IN UINT32 ddBAR0, IN UINT8 dbChannelNum);
VOID configureAzaliaSetConfigD4Dword (IN CODECENTRY* tempAzaliaCodecEntryPtr, IN UINT32 ddChannelNum, IN UINT32 ddBAR0);

/**
 * Pin Config for ALC880, ALC882 and ALC883.
 *
 *
 *
 */
static const CODECENTRY AzaliaCodecAlc882Table[] =
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
static const CODECENTRY AzaliaCodecAlc262Table[] =
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
static const CODECENTRY AzaliaCodecAlc269Table[] =
{
  {0x12, 0x99A30960},
  {0x14, 0x99130110},
  {0x15, 0x0221401F},
  {0x16, 0x99130120},
  {0x18, 0x01A19850},
  {0x19, 0x02A15951},
  {0x1a, 0x01813052},
  {0x1b, 0x0181405F},
  {0x1d, 0x40134601},
  {0x1e, 0x01441130},
  {0x11, 0x18567140},
  {0x20, 0x0030FFFF},
  {0xff, 0xffffffff}
};

/**
 * Pin Config for ALC0861.
 *
 *
 *
 */
static const CODECENTRY AzaliaCodecAlc861Table[] =
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
static const CODECENTRY AzaliaCodecAlc889Table[] =
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
static const CODECENTRY AzaliaCodecAd1984Table[] =
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
static const CODECENTRY FrontPanelAzaliaCodecTableList[] =
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
static const CODECTBLLIST azaliaCodecTableList[] =
{
  {0x010ec0880, (CODECENTRY*)&AzaliaCodecAlc882Table[0]},
  {0x010ec0882, (CODECENTRY*)&AzaliaCodecAlc882Table[0]},
  {0x010ec0883, (CODECENTRY*)&AzaliaCodecAlc882Table[0]},
  {0x010ec0885, (CODECENTRY*)&AzaliaCodecAlc882Table[0]},
  {0x010ec0889, (CODECENTRY*)&AzaliaCodecAlc889Table[0]},
  {0x010ec0262, (CODECENTRY*)&AzaliaCodecAlc262Table[0]},
  {0x010ec0269, (CODECENTRY*)&AzaliaCodecAlc269Table[0]},
  {0x010ec0861, (CODECENTRY*)&AzaliaCodecAlc861Table[0]},
  {0x011d41984, (CODECENTRY*)&AzaliaCodecAd1984Table[0]},
  { (UINT32) 0x0FFFFFFFF, (CODECENTRY*) (UINTN)0x0FFFFFFFF}
};

/**
 * azaliaInitBeforePciEnum - Config HD Audio Before PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
azaliaInitBeforePciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  if ( pConfig->AzaliaController == 1 ) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEB, AccWidthUint8, ~BIT0, 0);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEB, AccWidthUint8, ~BIT0, BIT0);
    if ( pConfig->BuildParameters.HdAudioMsi) {
      RWPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG44, AccWidthUint32 | S3_SAVE, ~BIT8, BIT8);
      RWPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG60, AccWidthUint32 | S3_SAVE, ~BIT16, BIT16);
    }
  }
}

/**
 * azaliaInitAfterPciEnum - Config HD Audio after PCI emulation
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
azaliaInitAfterPciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8   Data;
  UINT8   i;
  UINT8   dbEnableAzalia;
  UINT8   dbPinRouting;
  UINT8   dbChannelNum;
  UINT8   dbTempVariable;
  UINT16   dwTempVariable;
  UINT32   ddBAR0;
  dbEnableAzalia = 0;
  dbChannelNum = 0;
  dbTempVariable = 0;
  dwTempVariable = 0;
  ddBAR0 = 0;

  if ( pConfig->AzaliaController == 1 ) {
    return;
  }

  if ( pConfig->AzaliaController != 1 ) {
    RWPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG04, AccWidthUint8 | S3_SAVE, ~BIT1, BIT1);
    if ( pConfig->BuildParameters.AzaliaSsid != 0 ) {
      RWPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.AzaliaSsid);
    }
    ReadPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG10, AccWidthUint32, &ddBAR0);
    if ( ddBAR0 != 0 ) {
      if ( ddBAR0 != 0xFFFFFFFF ) {
        ddBAR0 &=  ~(0x03FFF);
        dbEnableAzalia = 1;
      }
    }
  }

  if ( dbEnableAzalia ) {
    // Get SDIN Configuration
    if ( pConfig->AZALIACONFIG.AzaliaConfig.AzaliaSdin0 == 2 ) {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG167, AccWidthUint8, 0, 0x3E);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG167, AccWidthUint8, 0, 0x00);
    } else {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG167, AccWidthUint8, 0, 0x0);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG167, AccWidthUint8, 0, 0x01);
    }
    if ( pConfig->AZALIACONFIG.AzaliaConfig.AzaliaSdin1 == 2 ) {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG168, AccWidthUint8, 0, 0x3E);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG168, AccWidthUint8, 0, 0x00);
    } else {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG168, AccWidthUint8, 0, 0x0);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG168, AccWidthUint8, 0, 0x01);
    }
    if ( pConfig->AZALIACONFIG.AzaliaConfig.AzaliaSdin2 == 2 ) {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG169, AccWidthUint8, 0, 0x3E);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG169, AccWidthUint8, 0, 0x00);
    } else {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG169, AccWidthUint8, 0, 0x0);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG169, AccWidthUint8, 0, 0x01);
    }
    if ( pConfig->AZALIACONFIG.AzaliaConfig.AzaliaSdin3 == 2 ) {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG170, AccWidthUint8, 0, 0x3E);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG170, AccWidthUint8, 0, 0x00);
    } else {
      RWMEM (ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG170, AccWidthUint8, 0, 0x0);
      RWMEM (ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG170, AccWidthUint8, 0, 0x01);
    }
    // INT#A Azalia resource
    Data = 0x93; // Azalia APIC index
    WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &Data);
    Data = 0x10; // IRQ16 (INTA#)
    WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &Data);

    i = 11;
    do {
      ReadMEM ( ddBAR0 + SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
      dbTempVariable |= BIT0;
      WriteMEM (ddBAR0 + SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
      SbStall (1000);
      ReadMEM (ddBAR0 + SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
      i--;
    }  while ((! (dbTempVariable & BIT0)) && (i > 0) );

    if ( i == 0 ) {
      return;
    }

    SbStall (1000);
    ReadMEM ( ddBAR0 + SB_AZ_BAR_REG0E, AccWidthUint16, &dwTempVariable);
    if ( dwTempVariable & 0x0F ) {

      //atleast one azalia codec found
      // ?? E0 is not real register what we expect. we have change to GPIO/and program GPIO Mux
      //ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGE0, AccWidthUint8, &dbPinRouting);
      dbPinRouting = pConfig->AZALIACONFIG.AzaliaSdinPin;
      do {
        if ( ( ! (dbPinRouting & BIT0) ) && (dbPinRouting & BIT1) ) {
//          dbChannelNum = 3;
          configureAzaliaPinCmd (pConfig, ddBAR0, dbChannelNum);
        }
        dbPinRouting >>= 2;
        dbChannelNum++;
      }  while ( dbChannelNum != 4 );
    } else {
      //No Azalia codec found
      if ( pConfig->AzaliaController != 2 ) {
        dbEnableAzalia = 0;     //set flag to disable Azalia
      }
    }
  }

  if ( dbEnableAzalia ) {
    //redo clear reset
    do {
      dwTempVariable = 0;
      WriteMEM ( ddBAR0 + SB_AZ_BAR_REG0C, AccWidthUint16 | S3_SAVE, &dwTempVariable);
      ReadMEM (ddBAR0 + SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
      dbTempVariable &= ~(BIT0);
      WriteMEM (ddBAR0 + SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
      ReadMEM (ddBAR0 + SB_AZ_BAR_REG08, AccWidthUint8 | S3_SAVE, &dbTempVariable);
    }  while ( dbTempVariable & BIT0 );

    if ( pConfig->AzaliaSnoop == 1 ) {
      RWPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG42, AccWidthUint8 | S3_SAVE, 0xFF, BIT1 + BIT0);
    }
  } else {
    //disable Azalia controller
    RWPCI ((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG04, AccWidthUint16 | S3_SAVE, 0, 0);
    // RWPMIO (SB_PMIO_REG59, AccWidthUint8 | S3_SAVE, ~BIT3, 0);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEB, AccWidthUint8, ~BIT0, 0);
    // RWPCI ((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGFC, AccWidthUint8 | S3_SAVE, 0, 0x55);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEB, AccWidthUint8, ~BIT0, 0);
  }
}

/**
 * configureAzaliaPinCmd - Configuration HD Audio PIN Command
 *
 *
 * @param[in] pConfig              Southbridge configuration structure pointer.
 * @param[in] ddBAR0               HD Audio BAR0 base address.
 * @param[in] dbChannelNum         Channel Number.
 *
 */
VOID
configureAzaliaPinCmd (
  IN       AMDSBCFG* pConfig,
  IN       UINT32 ddBAR0,
  IN       UINT8 dbChannelNum
  )
{
  UINT32   ddTempVariable;
  UINT32   ddChannelNum;
  CODECTBLLIST*  ptempAzaliaOemCodecTablePtr;
  CODECENTRY* tempAzaliaCodecEntryPtr;

  if ( (pConfig->AzaliaPinCfg) != 1 ) {
    return;
  }

  ddChannelNum = dbChannelNum << 28;
  ddTempVariable = 0xF0000;
  ddTempVariable |= ddChannelNum;

  WriteMEM (ddBAR0 + SB_AZ_BAR_REG60, AccWidthUint32 | S3_SAVE, &ddTempVariable);
  SbStall (600);
  ReadMEM (ddBAR0 + SB_AZ_BAR_REG64, AccWidthUint32 | S3_SAVE, &ddTempVariable);

  if ( ((pConfig->AZOEMTBL.pAzaliaOemCodecTablePtr) == NULL) || ((pConfig->AZOEMTBL.pAzaliaOemCodecTablePtr) == ((CODECTBLLIST*) (UINTN)0xFFFFFFFF))) {
    ptempAzaliaOemCodecTablePtr = (CODECTBLLIST*) FIXUP_PTR (&azaliaCodecTableList[0]);
  } else {
    ptempAzaliaOemCodecTablePtr = (CODECTBLLIST*) pConfig->AZOEMTBL.pAzaliaOemCodecTablePtr;
  }

  while ( ptempAzaliaOemCodecTablePtr->CodecID != 0xFFFFFFFF ) {
    if ( ptempAzaliaOemCodecTablePtr->CodecID == ddTempVariable ) {
      break;
    } else {
      ++ptempAzaliaOemCodecTablePtr;
    }
  }

  if ( ptempAzaliaOemCodecTablePtr->CodecID != 0xFFFFFFFF ) {
    tempAzaliaCodecEntryPtr = (CODECENTRY*) ptempAzaliaOemCodecTablePtr->CodecTablePtr;

    if ( ((pConfig->AZOEMTBL.pAzaliaOemCodecTablePtr) == NULL) || ((pConfig->AZOEMTBL.pAzaliaOemCodecTablePtr) == ((CODECTBLLIST*) (UINTN)0xFFFFFFFF)) ) {
      tempAzaliaCodecEntryPtr = (CODECENTRY*) FIXUP_PTR (tempAzaliaCodecEntryPtr);
    }
    configureAzaliaSetConfigD4Dword (tempAzaliaCodecEntryPtr, ddChannelNum, ddBAR0);
    if ( pConfig->AzaliaFrontPanel != 1 ) {
      if ( (pConfig->AzaliaFrontPanel == 2) || (pConfig->FrontPanelDetected == 1) ) {
        if ( ((pConfig->AZOEMFPTBL.pAzaliaOemFpCodecTablePtr) == NULL) || ((pConfig->AZOEMFPTBL.pAzaliaOemFpCodecTablePtr) == (VOID*) (UINTN)0xFFFFFFFF) ) {
          tempAzaliaCodecEntryPtr = (CODECENTRY*) FIXUP_PTR (&FrontPanelAzaliaCodecTableList[0]);
        } else {
          tempAzaliaCodecEntryPtr = (CODECENTRY*) pConfig->AZOEMFPTBL.pAzaliaOemFpCodecTablePtr;
        }
        configureAzaliaSetConfigD4Dword (tempAzaliaCodecEntryPtr, ddChannelNum, ddBAR0);
      }
    }
  }
}

/**
 * configureAzaliaSetConfigD4Dword - Configuration HD Audio Codec table
 *
 *
 * @param[in] tempAzaliaCodecEntryPtr    HD Audio Codec table structure pointer.
 * @param[in] ddChannelNum               HD Audio Channel Number.
 * @param[in] ddBAR0                     HD Audio BAR0 base address.
 *
 */
VOID
configureAzaliaSetConfigD4Dword (
  IN       CODECENTRY* tempAzaliaCodecEntryPtr,
  IN       UINT32 ddChannelNum,
  IN       UINT32 ddBAR0
  )
{
  UINT8 dbtemp1;
  UINT8 dbtemp2;
  UINT8 i;
  UINT32 ddtemp;
  UINT32 ddtemp2;
  ddtemp = 0;
  ddtemp2 = 0;
  while ( (tempAzaliaCodecEntryPtr->Nid) != 0xFF ) {
    dbtemp1 = 0x20;
    if ( (tempAzaliaCodecEntryPtr->Nid) == 0x1 ) {
      dbtemp1 = 0x24;
    }

    ddtemp =  tempAzaliaCodecEntryPtr->Nid;
    ddtemp &= 0xff;
    ddtemp <<= 20;
    ddtemp |= ddChannelNum;

    ddtemp |= (0x700 << 8);
    for ( i = 4; i > 0; i-- ) {
      do {
        ReadMEM (ddBAR0 + SB_AZ_BAR_REG68, AccWidthUint32, &ddtemp2);
      } while ( ddtemp2 & BIT0 );

      dbtemp2 = (UINT8) (( (tempAzaliaCodecEntryPtr->Byte40) >> ((4 - i) * 8 ) ) & 0xff);
      ddtemp =  (ddtemp & 0xFFFF0000) + ((dbtemp1 - i) << 8) + dbtemp2;
      WriteMEM (ddBAR0 + SB_AZ_BAR_REG60, AccWidthUint32 | S3_SAVE, &ddtemp);
      SbStall (60);
    }
    ++tempAzaliaCodecEntryPtr;
  }
}
