/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch Spi controller
 *
 * Init Spi Controller features (PEI phase).
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 64064 $   @e \$Date: 2012-01-15 22:36:53 -0600 (Sun, 15 Jan 2012) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE PROC_FCH_SPI_SPIRESET_FILECODE

/**
 * FchDummy2 - Dummy2
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 *
 */
VOID
FchDummy2 (
  IN  UINT32     SpiRomBase
  );

VOID
FchDummy (
  IN  UINT32     SpiRomBase
  );

VOID
FchSpiExecute (
  IN  UINT32     SpiRomBase
  );

VOID
FchResetFifo (
  IN  UINT32     SpiRomBase
  );

VOID
FchResetFifoToLast (
  IN  UINT32     SpiRomBase
  );

UINT32
FchReadSpiId (
  IN  UINT32     SpiRomBase,
  IN  BOOLEAN    Flag
  );

BOOLEAN
FchReadSpiQe (
  IN  UINT32     SpiRomBase,
  IN  UINT32     DeviceID
  );

VOID
FchWriteSpiQe (
  IN  UINT32     SpiRomBase,
  IN  UINT32     DeviceID
  );

VOID
FchSetQualMode (
  IN  UINT32      SpiQualMode,
  IN  UINT32     SpiRomBase,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  );

VOID
FchClearQualMode (
  IN  AMD_CONFIG_PARAMS         *StdHeader
  );

BOOLEAN
FchCheckSpiQe (
  IN  UINT32     SpiRomBase,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  );

/**
 * FchDummy - Dummy
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 *
 */
VOID
FchDummy (
  IN  UINT32     SpiRomBase
  )
{
  ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00);
}
/**
 * FchSpiExecute - SPI Execute
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 *
 */
VOID
FchSpiExecute (
  IN  UINT32     SpiRomBase
  )
{
  UINT32 SpiReg00;
  SpiReg00 = BIT31 + BIT16;
  ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00) |= BIT16;
  do {
    SpiReg00 = ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00);
  } while ((SpiReg00 & (BIT31 + BIT16)));
}
/**
 * FchResetFifo - Reset SPI FIFO
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 *
 */
VOID
FchResetFifo (
  IN  UINT32     SpiRomBase
  )
{
  ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00) |= BIT20;
  //ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00) |= BIT21;
}
/**
 * FchResetFifoToLast - Reset SPI FIFO to last
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 *
 */
VOID
FchResetFifoToLast (
  IN  UINT32     SpiRomBase
  )
{
  ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00) |= BIT20;
}

/**
 * FchReadSpiId - Read SPI ID
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 * @param[in] Flag       - Read Flag.
 *
 */
UINT32
FchReadSpiId (
  IN  UINT32     SpiRomBase,
  IN  BOOLEAN    Flag
  )
{
  UINT32 DeviceID;
  UINT8 i;
  DeviceID = 0;
  if (Flag) {
    ACPIMMIO16 (SpiRomBase + FCH_SPI_MMIO_REG00) = 0x409F;
    FchSpiExecute (SpiRomBase);
    FchResetFifo (SpiRomBase);
    for (i = 0; i < 3; i++) {
      DeviceID |= ((UINT32) (ACPIMMIO8 (SpiRomBase + FCH_SPI_MMIO_REG0C)) << (i*8));
    }
  } else {
    FchDummy (SpiRomBase);
    FchDummy2 (SpiRomBase);
  }
  return DeviceID;
}
/**
 * FchReadSpiQe - Read SPI Qual Enable
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 * @param[in] DeviceID   - Devic ID.
 *
 */
BOOLEAN
FchReadSpiQe (
  IN  UINT32     SpiRomBase,
  IN  UINT32     DeviceID
  )
{
  UINT8 StatusLen;
  UINT8 dbStatus;
  UINT8 dbStatus1;
  UINT8 StatusRead;
  UINT8 StatusWrite;
  UINT8 QualEnable;
  UINT8 QualEnable1;

  StatusLen = 0x02;
  StatusRead = 0x05;
  StatusWrite = 0x01;
  QualEnable = 0;
  QualEnable1 = 0;

  switch (DeviceID) {
  case 0x0024C2: //Macronix_MX25L1633E
  case 0x0025C2: //Macronix_MX25L1636E
  case 0x005EC2: //Macronix_MX25L3235D
  case 0x165EC2: //Macronix_MX25L3235D:tested
    QualEnable = 0x40;
    break;

  case 0x0014ef: //Wnbond_W25X16= S25FL016K
  case 0x004015: //Wnbond_W25Q16CV
    StatusLen = 0x02;
    StatusRead = 0x35;
    QualEnable1 = 0x02;
    break;

  case 0x00861F: //Atmel_AT25DQ161
    StatusRead = 0x3F;
    StatusRead = 0x3E;
    QualEnable = 0x80;
    break;

  default:
    return FALSE;
  }
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth16, 0, (StatusLen << 12) | StatusRead);
  FchSpiExecute (SpiRomBase);
  FchResetFifo (SpiRomBase);
  ReadMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus);
  ReadMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus);
  ReadMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus1);
  if ((dbStatus & QualEnable) || (dbStatus1 & QualEnable1) ) {
    return TRUE;
  }
  return FALSE;
}
/**
 * FchWriteSpiQe - Write SPI Qual Enable
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 * @param[in] DeviceID   - Devic ID.
 *
 */
VOID
FchWriteSpiQe (
  IN  UINT32     SpiRomBase,
  IN  UINT32     DeviceID
  )
{
  UINT8 StatusLen;
  UINT8 StatusWriteLen;
  UINT8 dbStatus;
  UINT8 dbStatus1;
  UINT8 StatusRead;
  UINT8 StatusWrite;
  UINT8 QualEnable;
  UINT8 QualEnable1;

  StatusLen = 0x02;
  StatusRead = 0x05;
  StatusWrite = 0x01;
  StatusWriteLen = 0x01;
  QualEnable = 0;
  QualEnable1 = 0;

  switch (DeviceID) {
  case 0x0024C2: //Macronix_MX25L1633E
  case 0x0025C2: //Macronix_MX25L1636E
  case 0x005EC2: //Macronix_MX25L3235D
  case 0x165EC2: //Macronix_MX25L3235D:tested
    QualEnable = 0x40;
    break;

  case 0x0014ef: //Wnbond_W25X16= S25FL016K
  case 0x004015: //Wnbond_W25Q16CV
    StatusLen = 0x02;
    StatusRead = 0x35;
    QualEnable1 = 0x02;
    break;

  case 0x00861F: //Atmel_AT25DQ161
    StatusRead = 0x3F;
    StatusRead = 0x3E;
    QualEnable = 0x80;
    break;

  default:
    return ;
  }
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth16, 0, (StatusLen << 12) | StatusRead);
  FchSpiExecute (SpiRomBase);
  FchResetFifo (SpiRomBase);
  ReadMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus);
  ReadMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus1);

  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth16, 0, 0x0006);
  FchSpiExecute (SpiRomBase);

  FchResetFifo (SpiRomBase);
  dbStatus |= QualEnable;
  dbStatus1 |= QualEnable1;
  WriteMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus);
  WriteMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth8, &dbStatus1);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth16, 0, (StatusWriteLen << 8) | StatusWrite);
  FchSpiExecute (SpiRomBase);
}
/**
 * FchSetQualMode - Set SPI Qual Mode
 *
 *
 *
 * @param[in] SpiQualMode- Spi Qual Mode.
 * @param[in] SpiRomBase - Spi Rom Base.
 * @param[in] StdHeader  - Standard Header.
 *
 */
VOID
FchSetQualMode (
  IN  UINT32      SpiQualMode,
  IN  UINT32     SpiRomBase,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  )
{
  RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GEVENT_REG09, AccessWidth8, (UINT32)~BIT3, (UINT32)BIT3);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBB, AccessWidth8, (UINT32)~BIT0, (UINT32)BIT0, StdHeader);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~( BIT18 + BIT29 + BIT30), ((SpiQualMode & 1) << 18) + ((SpiQualMode & 6) << 28));
}
/**
 * FchClearQualMode - Clear SPI Qual Mode
 *
 *
 *
 * @param[in] StdHeader  - Standard Header.
 *
 */
VOID
FchClearQualMode (
  IN  AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT32                    SpiRomBase;
  //
  // Get Spi ROM Base Address
  //
  ReadPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA0, AccessWidth32, &SpiRomBase, StdHeader);
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~( BIT18 + BIT29 + BIT30), 0);
  RwMem (ACPI_MMIO_BASE + GPIO_BASE + FCH_GEVENT_REG09, AccessWidth8, (UINT32)~BIT3, 0);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBB, AccessWidth8, (UINT32)~ BIT0, 0, StdHeader);
}
/**
 * FchCheckSpiQe - Check SPI Qual Enable
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 * @param[in] StdHeader  - Standard Header.
 *
 */
BOOLEAN
FchCheckSpiQe (
  IN  UINT32     SpiRomBase,
  IN  AMD_CONFIG_PARAMS         *StdHeader
  )
{
  UINT32 DeviceID;
  //FchClearQualMode (SpiRomBase, StdHeader);
  FchReadSpiId (SpiRomBase, FALSE);
  DeviceID = FchReadSpiId (SpiRomBase, TRUE);
  //if (DeviceID != 0x165EC2) FCH_DEADLOOP();
  switch (DeviceID) {
  case 0x0024C2: //Macronix_MX25L1633E
  case 0x0025C2: //Macronix_MX25L1636E
  case 0x005EC2: //Macronix_MX25L3235D
  case 0x165EC2: //Macronix_MX25L3235D:tested
  case 0x0014ef: //Wnbond_W25X16= S25FL016K
  case 0x004015: //Wnbond_W25Q16CV
  case 0x00861F: //Atmel_AT25DQ161
    if (FchReadSpiQe (SpiRomBase, DeviceID)) {
      return TRUE;
    } else {
      do {
        FchWriteSpiQe (SpiRomBase, DeviceID);
        if (FchReadSpiQe (SpiRomBase, DeviceID)) {
          return TRUE;
        }
      } while (!FchReadSpiQe (SpiRomBase, DeviceID));
    }
    break;
  }
  return FALSE;
}
/**
 * FchInitResetSpi - Config Spi controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSpi (
  IN  VOID     *FchDataPtr
  )
{
  UINT32                    SpiModeByte;
  UINT32                    SpiRomBase;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  SpiRomBase = UserOptions.FchBldCfg->CfgSpiRomBaseAddress;

  //
  // Set Spi ROM Base Address
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA0, AccessWidth32, 0x001F, SpiRomBase, StdHeader);

  //
  //  Spi Mode Initial
  //
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, 0xFFFFFFFF, (BIT19 + BIT24 + BIT25 + BIT26));
  RwMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth32, 0xFFC0FFFF, 0 );

  if (LocalCfgPtr->SpiSpeed) {
    RwMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth32, (UINT32)~(BIT13 + BIT12), ((LocalCfgPtr->SpiSpeed - 1 ) << 12));
  }

  if (LocalCfgPtr->FastSpeed) {
    RwMem (SpiRomBase + FCH_SPI_MMIO_REG0C, AccessWidth32, (UINT32)~(BIT15 + BIT14), ((LocalCfgPtr->FastSpeed - 1 ) << 14));
  }

  RwMem (SpiRomBase + FCH_SPI_MMIO_REG1C, AccessWidth32, (UINT32)~(BIT10), ((LocalCfgPtr->BurstWrite) << 10));

  SpiModeByte = LocalCfgPtr->Mode;
  if (LocalCfgPtr->Mode) {
    if ((SpiModeByte == FCH_SPI_MODE_QUAL_114) || (SpiModeByte == FCH_SPI_MODE_QUAL_144)) {
      if (FchCheckSpiQe (SpiRomBase, StdHeader)) {
        FchSetQualMode (SpiModeByte, SpiRomBase, StdHeader);
      }
    } else {
      RwMem (SpiRomBase + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~( BIT18 + BIT29 + BIT30), ((LocalCfgPtr->Mode & 1) << 18) + ((LocalCfgPtr->Mode & 6) << 28));
    }
  } else {
    if (FchCheckSpiQe (SpiRomBase, StdHeader)) {
      SpiModeByte = FCH_SPI_MODE_QUAL_144;
      //FchSetQualMode (SpiModeByte, SpiRomBase, StdHeader);
    }
  }
  // Enabling SPI ROM Prefetch
  // Set LPC cfg 0xBA bit 8
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT8, StdHeader);

  // Enable SPI Prefetch for USB, set LPC cfg 0xBA bit 7 to 1.
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT7, StdHeader);
}
/**
 * FchDummy2 - Dummy2
 *
 *
 *
 * @param[in] SpiRomBase - Spi Rom Base.
 *
 */
VOID
FchDummy2 (
  IN  UINT32     SpiRomBase
  )
{
  ACPIMMIO32 (SpiRomBase + FCH_SPI_MMIO_REG00);
}


