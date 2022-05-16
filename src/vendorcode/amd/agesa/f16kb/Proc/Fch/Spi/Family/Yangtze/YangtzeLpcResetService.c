/* $NoKeywords:$ */
/**
 * @file
 *
 * Config Fch LPC controller
 *
 * Init LPC Controller features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision: 87493 $   @e \$Date: 2013-02-04 11:56:12 -0600 (Mon, 04 Feb 2013) $
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
****************************************************************************
*/
#include "FchPlatform.h"
#include "Filecode.h"

#define FILECODE PROC_FCH_SPI_FAMILY_YANGTZE_YANGTZELPCRESETSERVICE_FILECODE
#define SPI_BASE UserOptions.FchBldCfg->CfgSpiRomBaseAddress

CONST SPI_CONTROLLER_PROFILE SpiControllerProfile[4] = {
  {128, 100, 100, 100, 100},
  {128,  66,  66,  66,  66},
  {128,  33,  33,  33,  33},
  {128,  16,  16,  16,  16},
  };
CONST SPI_DEVICE_PROFILE DefaultSpiDeviceTable[] = {
  //JEDEC_ID,RomSize,SecSize;MaxNormal;MaxFast;MaxDual;MaxQuad;QeReadReg;QeWriteReg;QeRegSize;QeLocation;
  {0x001524C2, 2 << 20, 4096,  33, 108, 150, 300, 0x05, 0x01, 0x1, 0x0040}, //Macronix_MX25L1635D
  {0x001525C2, 2 << 20, 4096,  33, 108, 160, 432, 0x05, 0x01, 0x1, 0x0040}, //Macronix_MX25L1635E
  {0x00165EC2, 4 << 20, 4096,  33, 104, 208, 400, 0x05, 0x01, 0x1, 0x0040}, //Macronix_MX25L3235D
//  {0x003625C2, 4 << 20, 4096,  33, 104, 168, 432, 0x05, 0x01, 0x1, 0x0040}, //Macronix_MX25U3235F
  {0x001720C2, 8 << 20, 4096,  33, 104, 208, 400, 0x05, 0x01, 0x1, 0x0040}, //Macronix_MX25L6436E
  {0x003725C2, 8 << 20, 4096,  33, 104, 168, 432, 0x05, 0x01, 0x1, 0x0040}, //Macronix_MX25U6435F

  {0x0046159D, 4 << 20, 4096,  33, 104, 208, 400, 0x05, 0x01, 0x1, 0x0040}, //PFLASH Pm25LQ032C

  {0x001540EF, 2 << 20, 4096,  33, 104, 208, 416, 0x35, 0x01, 0x2, 0x0200}, //Wnbond_W25Q16CV
  {0x001640EF, 4 << 20, 4096,  33, 104, 208, 320, 0x35, 0x01, 0x2, 0x0200}, //Wnbond_W25Q32BV
  {0x001740EF, 8 << 20, 4096, 104, 104, 208, 416, 0x35, 0x01, 0x2, 0x0200}, //Wnbond_W25Q64

  {0x004326BF, 8 << 20, 4096,  40, 104, 160, 416, 0x35, 0x01, 0x2, 0x0200}, //SST26VF064BA

  {0x001640C8, 4 << 20, 4096,  33, 100, 160, 320, 0x35, 0x01, 0x2, 0x0200}, //GigaDecice GD25Q32BSIGR

  {0x00164037, 4 << 20, 4096,  33, 100, 200, 400, 0x35, 0x01, 0x2, 0x0200}, //AMIC A25LQ32B

  {0x00000000, 4 << 20, 4096,  33,  33,  33,  33, 0x05, 0x01, 0x1, 0x0040}
};


/**
 * FchInitYangtzeResetLpcPciTable - Lpc (Spi) device registers
 * initial during the power on stage.
 *
 *
 *
 *
 */
CONST REG8_MASK FchInitYangtzeResetLpcPciTable[] =
{
  //
  // LPC Device (Bus 0, Dev 20, Func 3)
  //
  {0x00, LPC_BUS_DEV_FUN, 0},

  {FCH_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {FCH_LPC_REG7C, 0x00, BIT0 + BIT2},
  {0x78, 0xF0, BIT2 + BIT3},                       /// Enable LDRQ pin
  {FCH_LPC_REGBA, 0x9F, BIT5 + BIT6},
  // Force EC_PortActive to 1 to fix possible IR non function issue when NO_EC_SUPPORT is defined
  {FCH_LPC_REGA4, (UINT8)~ BIT0, BIT0},
  {0xFF, 0xFF, 0xFF},
};

/**
 * FchInitResetLpcProgram - Config Lpc controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetLpcProgram (
  IN       VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;
  //
  // enable prefetch on Host, set LPC cfg 0xBB bit 0 to 1
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT8, StdHeader);

  ProgramPciByteTable ( (REG8_MASK*) (&FchInitYangtzeResetLpcPciTable[0]),
                       ARRAY_SIZE(FchInitYangtzeResetLpcPciTable), StdHeader);

  if ( LocalCfgPtr->Spi.LpcClk0 ) {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0 + 1, AccessWidth8, 0xDF, 0x20, StdHeader);
  } else {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0 + 1, AccessWidth8, 0xDF, 0, StdHeader);
  }
  if ( LocalCfgPtr->Spi.LpcClk1 ) {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0 + 1, AccessWidth8, 0xBF, 0x40, StdHeader);
  } else {
    RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGD0 + 1, AccessWidth8, 0xBF, 0, StdHeader);
  }
  if ( LocalCfgPtr->LegacyFree ) {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0x0003C000, StdHeader);
  } else {
    RwPci (((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REG44), AccessWidth32, 00, 0xFF03FFD5, StdHeader);
  }
}

/**
 * FchWriteSpiExtReg - Write SPI Extension Register
 *
 *
 *
 * @param[in] SpiExtRegIndex - Extension Register Index.
 * @param[in] SpiExtRegData - Extension Register Data.
 *
 */
STATIC VOID
FchWriteSpiExtReg (
  IN       UINT8      SpiExtRegIndex,
  IN       UINT8      SpiExtRegData
  )
{
  ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG1E) = SpiExtRegIndex;
  ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG1F) = SpiExtRegData;
}
/**
 * FchSetSpiCounter - Set SPI RX/TX Counters
 *
 *
 *
 * @param[in] TxCounter - Transfer Counter.
 * @param[in] RxCounter - Receive Counter.
 *
 */
STATIC VOID
FchSetSpiCounter (
  IN       UINT8      TxCounter,
  IN       UINT8      RxCounter
  )
{
  FchWriteSpiExtReg (FCH_SPI_MMIO_REG1F_X05_TX_BYTE_COUNT, TxCounter);
  FchWriteSpiExtReg (FCH_SPI_MMIO_REG1F_X06_RX_BYTE_COUNT, RxCounter);
}
/**
 * FchSpiControllerNotBusy - SPI Conroller Not Busy
 *
 *
 *
 *
 */
STATIC VOID
FchSpiControllerNotBusy (
  VOID)
{
  UINT32 SpiReg00;
  SpiReg00 = FCH_SPI_BUSY + FCH_SPI_EXEC_OPCODE;
  do {
    SpiReg00 = ACPIMMIO32 (SPI_BASE + FCH_SPI_MMIO_REG00);
  } while ((SpiReg00 & (FCH_SPI_BUSY + FCH_SPI_EXEC_OPCODE)));
}
/**
 * FchResetFifo - Reset SPI FIFO
 *
 *
 *
 *
 */
STATIC VOID
FchResetFifo (
  VOID)
{
  ACPIMMIO32 (SPI_BASE + FCH_SPI_MMIO_REG00) |= BIT20;
}
/**
 * WaitForSpiDeviceWriteEnabled -
 *
 *
 *
 *
 */
STATIC BOOLEAN
WaitForSpiDeviceWriteEnabled (
  VOID)
{
  UINT8 bStatus;
  bStatus = 0;
  do
  {
    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      0x05,//IN       UINT8    Opcode,
      &bStatus,//IN       OUT UINT8    *DataPtr,
      NULL,//IN       UINT8    *AddressPtr,
      0,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      TRUE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
  } while ((bStatus & 2) == 0);
  return TRUE;
}
/**
 * WaitForSpiDeviceComplete -
 *
 *
 *
 *
 */
STATIC BOOLEAN
WaitForSpiDeviceComplete (
  VOID)
{
  UINT8 bStatus;
  bStatus = 1;
  do
  {
    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      0x05,//IN       UINT8    Opcode,
      &bStatus,//IN       OUT UINT8    *DataPtr,
      NULL,//IN       UINT8    *AddressPtr,
      0,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      TRUE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
  } while (bStatus & 1);
  return TRUE;
}
/**
 * FchSpiTransfer - FCH Spi Transfer
 *
 *
 *
 * @param[in] PrefixCode   - Prefix code.
 * @param[in] Opcode       - Opcode.
 * @param[in] DataPtr      - Data Pointer.
 * @param[in] AddressPtr   - Address Pointer.
 * @param[in] Length       - Read/Write Length.
 * @param[in] WriteFlag    - Write Flag.
 * @param[in] AddressFlag  - Address Flag.
 * @param[in] DataFlag     - Data Flag.
 * @param[in] FinishedFlag - Finished Flag.
 *
 */
//static
AGESA_STATUS
FchSpiTransfer (
  IN       UINT8    PrefixCode,
  IN       UINT8    Opcode,
  IN OUT   UINT8    *DataPtr,
  IN       UINT8    *AddressPtr,
  IN       UINT8    Length,
  IN       BOOLEAN  WriteFlag,
  IN       BOOLEAN  AddressFlag,
  IN       BOOLEAN  DataFlag,
  IN       BOOLEAN  FinishedFlag
  )
{
  UINTN  Addr;
  UINTN  Retry;
  UINTN  i;
  UINTN  index;
  UINT8  WriteCount;
  UINT8  ReadCount;
  //UINT8  Dummy;
  //UINT8  CurrFifoIndex;

  if (!((Opcode == 0x9f) && (!DataFlag))) {
    if (PrefixCode) {
      Retry = 0;
      do {
        ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG00 + 0) = PrefixCode;
        //ACPIMMIO8(SPI_BASE + FCH_SPI_MMIO_REG00 + 1) = 0;
        FchSetSpiCounter (0, 0);
        ACPIMMIO32 (SPI_BASE + FCH_SPI_MMIO_REG00) |= FCH_SPI_EXEC_OPCODE;
        FchSpiControllerNotBusy ();

        if (FinishedFlag) {
          if (WaitForSpiDeviceWriteEnabled ()) {
            Retry = 0;
          } else {
            Retry ++;
            if (Retry >= FCH_SPI_RETRY_TIMES) {
              return AGESA_ERROR;
            }
          }
        }
      } while (Retry);
    }
    Retry = 0;
    do {
      WriteCount = 0;
      ReadCount = 0;
      //
      // Reset Fifo Ptr
      //
      FchResetFifo ();
      //
      // Check Address Mode
      //
      index = 0;
      Addr = (UINTN) AddressPtr;
      if (AddressFlag) {
        //for (i = 16, Addr = (UINTN) AddressPtr; i >= 0; i -= 8) {
        for (i = 0; i < 3; i ++) {
          //ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG0C + 0) = (UINT8) ((Addr >> i) & 0xff);
          ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG80_FIFO + index) = (UINT8) ((Addr >> (16 - i * 8)) & 0xff);
          index ++;
        }
        WriteCount += 3;
      }
      if (DataFlag) {
        //
        // Check Read/Write Mode
        //
        if (WriteFlag) {
          WriteCount += Length + 1;
          for (i = 0; i < (UINTN) (Length + 1); i ++) {
            //ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG0C + 0) = DataPtr[i];
            ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG80_FIFO + index) = DataPtr[i];
            index ++;
          }
        } else {
          //
          // Read operation must plus extra 1 byte
          //
          ReadCount += Length + 2;
        }
      }
      ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG00 + 0) = Opcode;
      //ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG00 + 1) = (ReadCount << 4) + WriteCount;
      FchSetSpiCounter (WriteCount, ReadCount);
      ACPIMMIO32 (SPI_BASE + FCH_SPI_MMIO_REG00) |= FCH_SPI_EXEC_OPCODE;
      FchSpiControllerNotBusy ();

      if (FinishedFlag) {
        if (WaitForSpiDeviceComplete ()) {
          Retry = 0;
        } else {
          Retry ++;
          if (Retry >= FCH_SPI_RETRY_TIMES) {
            return AGESA_ERROR;
          }
        }
      }
    } while (Retry);
    if (DataFlag && ReadCount) {
      //
      // Reset Fifo Ptr
      //
      FchResetFifo ();
      //while (DataFlag && ReadCount) {
      //  CurrFifoIndex = ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG4E + 1) & 0x07;
      //  if (CurrFifoIndex != WriteCount) {
      //    Dummy = ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG0C + 0);
      //  } else break;
      //}
      for (i = 0; i < (UINTN) (Length + 1); i ++) {
        //DataPtr[i] = ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG0C + 0);
        DataPtr[i] = ACPIMMIO8 (SPI_BASE + FCH_SPI_MMIO_REG80_FIFO + WriteCount + i);
      }
    }
  }
  return AGESA_SUCCESS;
}

/**
 *
 *
 *
 * @param[in] SpiQualMode- Spi Qual Mode.
 * @param[in] StdHeader  - Standard Header.
 *
 */
STATIC VOID
FchSetQualMode (
  IN       UINT32      SpiQualMode,
  IN       AMD_CONFIG_PARAMS         *StdHeader
  )
{

  RwMem (ACPI_MMIO_BASE + GPIO_BASE + 0x69, AccessWidth8, (UINT32)~BIT3, BIT3);
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBB, AccessWidth8, (UINT32)~ BIT0, BIT0, StdHeader);
  RwMem (SPI_BASE + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~( BIT18 + BIT29 + BIT30), ((SpiQualMode & 1) << 18) + ((SpiQualMode & 6) << 28));

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
  IN       VOID     *FchDataPtr
  )
{
  UINT32                    SpiModeByte;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  StdHeader = LocalCfgPtr->StdHeader;

  //
  // Set Spi ROM Base Address
  //
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGA0, AccessWidth32, 0x001F, SPI_BASE, StdHeader);

  RwMem (SPI_BASE + FCH_SPI_MMIO_REG00, AccessWidth32, 0xFFFFFFFF, (BIT19 + BIT24 + BIT25 + BIT26));
  RwMem (SPI_BASE + FCH_SPI_MMIO_REG0C, AccessWidth32, 0xFFC0FFFF, 0 );

  RwMem (SPI_BASE + FCH_SPI_MMIO_REG20, AccessWidth8, 0xFE, (UINT8) ((LocalCfgPtr->SPI100_Enable) << 0));

  if (LocalCfgPtr->SpiSpeed) {
    RwMem (SPI_BASE + FCH_SPI_MMIO_REG22, AccessWidth32, ~((UINT32) (0xF << 12)), ((LocalCfgPtr->SpiSpeed - 1 ) << 12));
  }

  if (LocalCfgPtr->FastSpeed) {
    RwMem (SPI_BASE + FCH_SPI_MMIO_REG22, AccessWidth32, ~((UINT32) (0xF << 8)), ((LocalCfgPtr->FastSpeed - 1 ) << 8));
  }

  RwMem (SPI_BASE + FCH_SPI_MMIO_REG1C, AccessWidth32, (UINT32)~(BIT10), ((LocalCfgPtr->BurstWrite) << 10));

  SpiModeByte = LocalCfgPtr->Mode;
  if (LocalCfgPtr->Mode) {
    if ((SpiModeByte == FCH_SPI_MODE_QUAL_114) || (SpiModeByte == FCH_SPI_MODE_QUAL_144) || (SpiModeByte == FCH_SPI_MODE_QUAL_112) || (SpiModeByte == FCH_SPI_MODE_QUAL_122) || (SpiModeByte == FCH_SPI_MODE_FAST)) {
      if (FchPlatformSpiQe (FchDataPtr)) {
        FchSetQualMode (SpiModeByte, StdHeader);
      }
    } else {
      RwMem (SPI_BASE + FCH_SPI_MMIO_REG00, AccessWidth32, (UINT32)~( BIT18 + BIT29 + BIT30), ((LocalCfgPtr->Mode & 1) << 18) + ((LocalCfgPtr->Mode & 6) << 28));
    }
  } else {
    if (FchPlatformSpiQe (FchDataPtr)) {
      SpiModeByte = FCH_SPI_MODE_QUAL_144;
    }
  }
  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT8, StdHeader);

  RwPci ((LPC_BUS_DEV_FUN << 16) + FCH_LPC_REGBA, AccessWidth16, 0xFFFF, BIT7, StdHeader);
}



STATIC UINT32
FchReadSpiId (
  IN       BOOLEAN    Flag
  )
{
  UINT32 DeviceID;
  UINT8 *DeviceIdPtr;
  DeviceID = 0;
  DeviceIdPtr = (UINT8 *) (((UINTN) (&DeviceID)));
  if (Flag) {
    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      0x9F,//IN       UINT8    Opcode,
      DeviceIdPtr,//IN  OUT   UINT8    *DataPtr,
      (UINT8 *) (NULL),//IN       UINT8    *AddressPtr,
      2,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      TRUE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
  } else {
    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      0x9F,//IN       UINT8    Opcode,
      DeviceIdPtr,//IN  OUT   UINT8    *DataPtr,
      (UINT8 *) (NULL),//IN       UINT8    *AddressPtr,
      2,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      FALSE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
  }
  return DeviceID;
}
/**
 * FchReadSpiQe - Read SPI Qual Enable
 *
 *
 *
 * @param[in] SpiDeviceProfilePtr - Spi Device Profile Pointer
 * @param[in] SpiQeRegValue   - Spi QuadEnable Register Value
 *
 */
STATIC BOOLEAN
FchReadSpiQe (
  IN OUT   SPI_DEVICE_PROFILE    *SpiDeviceProfilePtr,
  IN       UINT16     SpiQeRegValue
  )
{
  UINT16     Value16;
  SpiQeRegValue = 0;
  Value16 = 0;

  FchSpiTransfer (
    0, //IN       UINT8    PrefixCode,
    SpiDeviceProfilePtr->QeReadRegister,//IN       UINT8    Opcode,
    (UINT8 *)(&SpiQeRegValue),//IN  OUT   UINT8    *DataPtr,
    NULL,//IN       UINT8    *AddressPtr,
    0,//IN       UINT8    Length,
    FALSE,//IN       BOOLEAN  WriteFlag,
    FALSE,//IN       BOOLEAN  AddressFlag,
    TRUE,//IN       BOOLEAN  DataFlag,
    FALSE //IN       BOOLEAN  FinishedFlag
  );
  if (SpiDeviceProfilePtr->QeOperateSize == 2) {
    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      0x05,//IN       UINT8    Opcode,
      (UINT8 *)(&SpiQeRegValue),//IN  OUT   UINT8    *DataPtr,
      NULL,//IN       UINT8    *AddressPtr,
      0,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      TRUE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      SpiDeviceProfilePtr->QeReadRegister,//IN       UINT8    Opcode,
      (UINT8 *)(&Value16),//IN  OUT   UINT8    *DataPtr,
      NULL,//IN       UINT8    *AddressPtr,
      0,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      TRUE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
    SpiQeRegValue |= (Value16 << 8);
  }

  if (SpiDeviceProfilePtr->QeLocation & SpiQeRegValue) {
    return TRUE;
  }
  SpiQeRegValue |= SpiDeviceProfilePtr->QeLocation;
  return FALSE;
}
/**
 * FchWriteSpiQe - Write SPI Qual Enable
 *
 *
 *
 * @param[in] SpiDeviceProfilePtr - Spi Device Profile Pointer
 * @param[in] SpiQeRegValue   - Spi QuadEnable Register Value
 *
 */
STATIC VOID
FchWriteSpiQe (
  IN OUT   SPI_DEVICE_PROFILE    *SpiDeviceProfilePtr,
  IN       UINT16     SpiQeRegValue
  )
{

  SpiQeRegValue |= SpiDeviceProfilePtr->QeLocation;
  FchSpiTransfer (
    0x06, //IN       UINT8    PrefixCode,
    SpiDeviceProfilePtr->QeWriteRegister,//IN       UINT8    Opcode,
    (UINT8 *)(&SpiQeRegValue),//IN  OUT   UINT8    *DataPtr,
    NULL,//IN       UINT8    *AddressPtr,
    SpiDeviceProfilePtr->QeOperateSize - 1,//IN       UINT8    Length,
    TRUE,//IN       BOOLEAN  WriteFlag,
    FALSE,//IN       BOOLEAN  AddressFlag,
    TRUE,//IN       BOOLEAN  DataFlag,
    TRUE //IN       BOOLEAN  FinishedFlag
  );
}

/**
 * FchFindSpiDeviceProfile - Find SPI Device Profile
 *
 *
 *
 * @param[in] DeviceID   - Device ID
 * @param[in] SpiDeviceProfilePtr - Spi Device Profile Pointer
 *
 */
STATIC BOOLEAN
FchFindSpiDeviceProfile (
  IN       UINT32     DeviceID,
  IN OUT   SPI_DEVICE_PROFILE    *SpiDeviceProfilePtr
  )
{
  SPI_DEVICE_PROFILE   *CurrentSpiDeviceProfilePtr;
  UINT16     SpiQeRegValue;
  SpiQeRegValue = 0;
  CurrentSpiDeviceProfilePtr = SpiDeviceProfilePtr;
  do {
    if (CurrentSpiDeviceProfilePtr->JEDEC_ID == DeviceID) {
      SpiDeviceProfilePtr = CurrentSpiDeviceProfilePtr;
      if (!(FchReadSpiQe (SpiDeviceProfilePtr, SpiQeRegValue))) {
        FchWriteSpiQe (SpiDeviceProfilePtr, SpiQeRegValue);
        if (!(FchReadSpiQe (SpiDeviceProfilePtr, SpiQeRegValue))) {
          return FALSE;
        }
      }
      return TRUE;
    }
    CurrentSpiDeviceProfilePtr++;
  } while (CurrentSpiDeviceProfilePtr->JEDEC_ID != 0);
  return FALSE;
}

/**
 * FchConfigureSpiDeviceDummyCycle - Configure Spi Device Dummy
 * Cycle
 *
 *
 *
 * @param[in] DeviceID   - Device ID
 * @param[in] FchDataPtr  - FchData Pointer.
 *
 */
STATIC BOOLEAN
FchConfigureSpiDeviceDummyCycle (
  IN       UINT32     DeviceID,
  IN OUT   FCH_RESET_DATA_BLOCK    *FchDataPtr
  )
{
  UINT16     Mode16;
  UINT16     Value16;
  UINT8      Value8;
  UINT16     DummyValue16;
  UINT16     CurrentDummyValue16;
  UINT16     CurrentMode16;
  AMD_CONFIG_PARAMS         *StdHeader;

  StdHeader = FchDataPtr->StdHeader;
  Value16 = 0;
  DummyValue16 = 8;

  switch (DeviceID) {
  case 0x17BA20://N25Q064
  case 0x16BA20://N25Q032

    FchSpiTransfer (
      0, //IN       UINT8    PrefixCode,
      0xB5,//IN       UINT8    Opcode,
      (UINT8 *)(&Value16),//IN  OUT   UINT8    *DataPtr,
      NULL,//IN       UINT8    *AddressPtr,
      1,//IN       UINT8    Length,
      FALSE,//IN       BOOLEAN  WriteFlag,
      FALSE,//IN       BOOLEAN  AddressFlag,
      TRUE,//IN       BOOLEAN  DataFlag,
      FALSE //IN       BOOLEAN  FinishedFlag
    );
    CurrentDummyValue16 = Value16 >> 12;
    CurrentMode16 = (Value16 >> 9) & 7;

    switch (FchDataPtr->Mode) {
    case FCH_SPI_MODE_QUAL_144:
      DummyValue16 = 6;
      Mode16 = FCH_SPI_DEVICE_MODE_144;
      break;
    case FCH_SPI_MODE_QUAL_114:
      DummyValue16 = 8;
      Mode16 = FCH_SPI_DEVICE_MODE_114;
      Mode16 = 7;
      break;
    case FCH_SPI_MODE_QUAL_122:
      DummyValue16 = 4;
      Mode16 = FCH_SPI_DEVICE_MODE_122;
      break;
    case FCH_SPI_MODE_QUAL_112:
      DummyValue16 = 8;
      Mode16 = FCH_SPI_DEVICE_MODE_112;
      break;
    case FCH_SPI_MODE_FAST:
      DummyValue16 = 8;
      Mode16 = FCH_SPI_DEVICE_MODE_FAST;
      break;
    default:
      DummyValue16 = 15;
      Mode16 = 7;
      break;
    }
    if ((CurrentDummyValue16 != DummyValue16) || (CurrentMode16 != Mode16)) {
      //FCH_DEADLOOP();
      Value16 &= ~ (0x7f << 9);
      Value16 |= (DummyValue16 << 12);
      Value16 |= (Mode16 << 9);
      FchSpiTransfer (
        0x06, //IN       UINT8    PrefixCode,
        0xB1,//IN       UINT8    Opcode,
        (UINT8 *)(&Value16),//IN  OUT   UINT8    *DataPtr,
        NULL,//IN       UINT8    *AddressPtr,
        1,//IN       UINT8    Length,
        TRUE,//IN       BOOLEAN  WriteFlag,
        FALSE,//IN       BOOLEAN  AddressFlag,
        TRUE,//IN       BOOLEAN  DataFlag,
        TRUE //IN       BOOLEAN  FinishedFlag
      );

      FchSpiTransfer (
        0, //IN       UINT8    PrefixCode,
        0x85,//IN       UINT8    Opcode,
        (UINT8 *)(&Value8),//IN  OUT   UINT8    *DataPtr,
        NULL,//IN       UINT8    *AddressPtr,
        0,//IN       UINT8    Length,
        FALSE,//IN       BOOLEAN  WriteFlag,
        FALSE,//IN       BOOLEAN  AddressFlag,
        TRUE,//IN       BOOLEAN  DataFlag,
        FALSE //IN       BOOLEAN  FinishedFlag
      );

      Value8 &= ~ (0xf << 4);
      Value8 |= (UINT8) (DummyValue16 << 4);
      FchSpiTransfer (
        0x06, //IN       UINT8    PrefixCode,
        0x81,//IN       UINT8    Opcode,
        (UINT8 *)(&Value8),//IN  OUT   UINT8    *DataPtr,
        NULL,//IN       UINT8    *AddressPtr,
        0,//IN       UINT8    Length,
        TRUE,//IN       BOOLEAN  WriteFlag,
        FALSE,//IN       BOOLEAN  AddressFlag,
        TRUE,//IN       BOOLEAN  DataFlag,
        TRUE //IN       BOOLEAN  FinishedFlag
      );
      //      FchStall (1000, StdHeader);
//      WriteIo8 ((UINT16) (0xCF9), 0x0E);
    }
    return TRUE;
  default:
    return FALSE;
  }
}
/**
 * FchPlatformSpiQe - Platform SPI Qual Enable
 *
 *
 *
 * @param[in] FchDataPtr  - FchData Pointer.
 *
 */
BOOLEAN
FchPlatformSpiQe (
  IN       VOID     *FchDataPtr
  )
{
  UINT32 DeviceID;
  SPI_DEVICE_PROFILE        *LocalSpiDeviceProfilePtr;
  FCH_RESET_DATA_BLOCK      *LocalCfgPtr;
  AMD_CONFIG_PARAMS         *StdHeader;
  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *) FchDataPtr;
  if (LocalCfgPtr->QeEnabled) {
    return TRUE;
  }
  StdHeader = LocalCfgPtr->StdHeader;
  FchReadSpiId (FALSE);
  DeviceID = FchReadSpiId (TRUE);
//  if (LocalCfgPtr->OemSpiDeviceTable != NULL) {
//    LocalSpiDeviceProfilePtr = LocalCfgPtr->OemSpiDeviceTable;
//    if (FchFindSpiDeviceProfile (DeviceID, LocalSpiDeviceProfilePtr)) {
//      return TRUE;
//    }
//  }
  LocalSpiDeviceProfilePtr = (SPI_DEVICE_PROFILE *) (&DefaultSpiDeviceTable);
  if (FchConfigureSpiDeviceDummyCycle (DeviceID, LocalCfgPtr)) {
    return TRUE;
  }
  if (FchFindSpiDeviceProfile (DeviceID, LocalSpiDeviceProfilePtr)) {
    return TRUE;
  }
  return FALSE;
}
