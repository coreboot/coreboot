/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/


#include	"Platform.h"

REG8MASK sbPorInitPciTable[] = {
  // SMBUS Device(Bus 0, Dev 20, Func 0)
  {0x00, SMBUS_BUS_DEV_FUN, 0},
  {SB_SMBUS_REGD0+2, 0x00, 0x01},
  {SB_SMBUS_REG40, 0x00, 0x44},
  {SB_SMBUS_REG40+1, 0xFF, 0xE9},   //Set smbus pci config 0x40[14]=1, This bit is used for internal bus flow control.
  {SB_SMBUS_REG64, 0x00, 0xBF},   //SB_SMBUS_REG64[13]=1, delays back to back interrupts to the CPU
  {SB_SMBUS_REG64+1, 0x00, 0x78},
  {SB_SMBUS_REG64+2, ~(UINT8)BIT6, 0x9E},
  {SB_SMBUS_REG64+3, 0x0F, 0x02},
  {SB_SMBUS_REG68+1, 0x00, 0x90},
  {SB_SMBUS_REG6C, 0x00, 0x20},
  {SB_SMBUS_REG78, 0x00, 0xFF},
  {SB_SMBUS_REG04, 0x00, 0x07},
  {SB_SMBUS_REG04+1, 0x00, 0x04},
  {SB_SMBUS_REGE1, 0x00, 0x99},   //RPR recommended setting, Sections "SMBUS Pci Config" & "IMC Access Control"
  {SB_SMBUS_REGAC, ~(UINT8)BIT4, BIT1},
  {SB_SMBUS_REG60+2, ~(UINT8)(BIT1+BIT0) , 0x24},  // Disabling Legacy USB Fast SMI# Smbus_PCI_config 0x62 [5] = 1. Legacy USB
                  // can request SMI# to be sent out early before IO completion.
                  // Some applications may have problems with this feature. The BIOS should set this bit
                  // to 1 to disable the feature. Enabling Legacy Interrupt Smbus_PCI_Config 0x62[2]=1.
  {0xFF, 0xFF, 0xFF},

  // LPC Device(Bus 0, Dev 20, Func 3)
  {0x00, LPC_BUS_DEV_FUN, 0},
  {SB_LPC_REG40, 0x00, 0x04},
  {SB_LPC_REG48, 0x00, 0x07},
  {SB_LPC_REG4A, 0x00, 0x20},	// Port Enable for IO Port 80h.
  {SB_LPC_REG78, ~(UINT8)BIT0, 0x00},
  {SB_LPC_REG7C, 0x00, 0x05},
  {SB_LPC_REGB8+3, ~(UINT8)BIT0, BIT7+BIT6+BIT5+BIT3+BIT0},  //RPR recommended setting,Section "IO / Mem Decoding" & "SPI bus"
  {0xFF, 0xFF, 0xFF},

  // P2P Bridge(Bus 0, Dev 20, Func 4)
  {0x00, SBP2P_BUS_DEV_FUN, 0},
  {SB_P2P_REG40, 0x00, 0x26},       // Enabling PCI-bridge subtractive decoding & PCI Bus 64-byte DMA Read Access
  {SB_P2P_REG4B, 0xFF, BIT6+BIT7+BIT4},
  {SB_P2P_REG1C, 0x00, 0x11},
  {SB_P2P_REG1D, 0x00, 0x11},
  {SB_P2P_REG04, 0x00, 0x21},
  {SB_P2P_REG50, 0x02, 0x01},       // PCI Bridge upstream dual address window
  {0xFF, 0xFF, 0xFF},
};


REG8MASK sbA13PorInitPciTable[] = {
  // SMBUS Device(Bus 0, Dev 20, Func 0)
  {0x00, SMBUS_BUS_DEV_FUN, 0},
  {SB_SMBUS_REG43, ~(UINT8)BIT3, 0x00},      //Make some hidden registers of smbus visible.
  {SB_SMBUS_REG38, (UINT8)~BIT7, 00},
  {SB_SMBUS_REGAC+1, ~(UINT8)BIT5, 0},     //Enable SATA test/enhancement mode
  {SB_SMBUS_REG43, 0xFF, BIT3},     //Make some hidden registers of smbus invisible.
  {0xFF, 0xFF, 0xFF},
};


REG8MASK sbA14PorInitPciTable[] = {
  // LPC Device(Bus 0, Dev 20, Func 3)
  {0x00, LPC_BUS_DEV_FUN, 0},
  {SB_LPC_REG8C+2, ~(UINT8)BIT1, 00},
  {0xFF, 0xFF, 0xFF},
};

REG8MASK sbPorPmioInitTbl[] = {
  // index    andmask ormask
  {SB_PMIO_REG67, 0xFF, 0x02},
  {SB_PMIO_REG37, 0xFF, 0x04},    // Configure pciepme as rising edge
  {SB_PMIO_REG50, 0x00, 0xE0},    // Enable CPU_STP (except S5) & PCI_STP
  {SB_PMIO_REG60, 0xFF, 0x20},    // Enable Speaker
  {SB_PMIO_REG65, (UINT8)~(BIT4+BIT7), 0x00},// Clear PM_IO 0x65[4] UsbResetByPciRstEnable to avoid S3 reset to reset USB
  {SB_PMIO_REG55, ~(UINT8)BIT6, 0x07},   // Select CIR wake event to ACPI.GEVENT[23] & Clear BIT6 SoftPciRst for safety
  {SB_PMIO_REG66, 0xFF, BIT5},    // Configure keyboard reset to generate pci reset
  {SB_PMIO_REGB2, 0xFF, BIT7},
  {SB_PMIO_REG0E, 0xFF, BIT3},    // Enable ACPI IO decoding
  {SB_PMIO_REGD7, 0xF6, 0x80},
  {SB_PMIO_REG7C, 0xFF, BIT4},    // enable RTC AltCentury register

  {SB_PMIO_REG75, 0xC0, 0x05},    // PME_TURN_OFF_MSG during ASF shutdown
  {SB_PMIO_REG52, 0xC0, 0x08},

  {SB_PMIO_REG8B, 0x00, 0x10},
  {SB_PMIO_REG69, 0xF9, 0x01 << 1}, // [Updated RPR] Set default WDT resolution to 10ms
};

REG8MASK sbA13PorPmioInitTbl[]={
  // index    andmask   ormask
  {SB_PMIO_REGD7, 0xFF, BIT5+BIT0}, //Fixes for TT SB00068 & SB01054 (BIT5 & BIT0 correspondingly)
  {SB_PMIO_REGBB, (UINT8)~BIT7, BIT6+BIT5},  //Fixes for TT SB00866 & SB00696 (BIT6 & BIT5 correspondingly)
                                          // Always clear [7] to begin with SP5100 C1e disabled

//  {SB_PMIO_REG65, 0xFF, BIT7},
//  {SB_PMIO_REG75, 0xC0, 0x01},    // PME_TURN_OFF_MSG during ASF shutdown
//  {SB_PMIO_REG52, 0xC0, 0x02},

};


void  sbPowerOnInit (AMDSBCFG *pConfig){
  UINT8   dbVar0, dbVar1, dbValue;
  UINT16    dwTempVar;
  BUILDPARAM  *pBuildOptPtr;

  TRACE((DMSG_SB_TRACE, "CIMx - Entering sbPowerOnInit \n"));

  setRevisionID();
  ReadPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, &dwTempVar);
  if (dwTempVar == SB750_SATA_DEFAULT_DEVICE_ID)
    RWPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG9C, AccWidthUint8 | S3_SAVE, 0xFF, 0x01);

  // Set A-Link bridge access address. This address is set at device 14h, function 0,
  // register 0f0h.   This is an I/O address. The I/O address must be on 16-byte boundry.
  RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGF0, AccWidthUint32, 00, ALINK_ACCESS_INDEX);

  writeAlink(0x80000004, 0x04);   // RPR 3.3 Enabling upstream DMA Access
  writeAlink(0x30, 0x10);       //AXINDC 0x10[9]=1, Enabling Non-Posted memory write for K8 platform.
  writeAlink(0x34, readAlink(0x34) | BIT9);

  if (!(pConfig->ResetCpuOnSyncFlood)){
    //Enable reset on sync flood
    writeAlink( (UINT32)( ((UINT32)SB_AB_REG10050) | ((UINT32)ABCFG << 30)),
				(UINT32)( readAlink((((UINT32)SB_AB_REG10050) | ((UINT32)ABCFG << 30))) | ((UINT32)BIT2) ));
  }

  pBuildOptPtr = &(pConfig->BuildParameters);

  WritePCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG90, AccWidthUint32 | S3_SAVE, &(pBuildOptPtr->Smbus0BaseAddress) );

        dwTempVar = pBuildOptPtr->Smbus1BaseAddress & (UINT16)~BIT0;
        if( dwTempVar != 0 ){
            RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG58, AccWidthUint16 | S3_SAVE, 00, (dwTempVar|BIT0));
            // Disable ASF Slave controller on SB700 rev A15.
            if (getRevisionID() == SB700_A15) {
                RWIO((dwTempVar+0x0D), AccWidthUint8, (UINT8)~BIT6, BIT6);
            }
        }

  WritePCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG64, AccWidthUint16 | S3_SAVE, &(pBuildOptPtr->SioPmeBaseAddress));
  RWPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA0, AccWidthUint32 | S3_SAVE, 0x001F,(pBuildOptPtr->SpiRomBaseAddress));

  WritePMIO(SB_PMIO_REG20, AccWidthUint16, &(pBuildOptPtr->AcpiPm1EvtBlkAddr));
  WritePMIO(SB_PMIO_REG22, AccWidthUint16, &(pBuildOptPtr->AcpiPm1CntBlkAddr));
  WritePMIO(SB_PMIO_REG24, AccWidthUint16, &(pBuildOptPtr->AcpiPmTmrBlkAddr));
  WritePMIO(SB_PMIO_REG26, AccWidthUint16, &(pBuildOptPtr->CpuControlBlkAddr));
  WritePMIO(SB_PMIO_REG28, AccWidthUint16, &(pBuildOptPtr->AcpiGpe0BlkAddr));
  WritePMIO(SB_PMIO_REG2A, AccWidthUint16, &(pBuildOptPtr->SmiCmdPortAddr));
  WritePMIO(SB_PMIO_REG2C, AccWidthUint16, &(pBuildOptPtr->AcpiPmaCntBlkAddr));
  RWPMIO(SB_PMIO_REG2E, AccWidthUint16, 0x00,(pBuildOptPtr->SmiCmdPortAddr)+8);
  WritePMIO(SB_PMIO_REG6C, AccWidthUint32, &(pBuildOptPtr->WatchDogTimerBase));

  //Program power on pci init table
  programPciByteTable( (REG8MASK*)FIXUP_PTR(&sbPorInitPciTable[0]), sizeof(sbPorInitPciTable)/sizeof(REG8MASK) );
  //Program power on pmio init table
  programPmioByteTable( (REG8MASK *)FIXUP_PTR(&sbPorPmioInitTbl[0]), (sizeof(sbPorPmioInitTbl)/sizeof(REG8MASK)) );

        dbValue = 0x00;
        ReadIO (SB_IOMAP_REGC14, AccWidthUint8, &dbValue);
        dbValue &= 0xF3;
        WriteIO (SB_IOMAP_REGC14, AccWidthUint8, &dbValue);

        dbValue = 0x0A;
        WriteIO (SB_IOMAP_REG70, AccWidthUint8, &dbValue);
        ReadIO (SB_IOMAP_REG71, AccWidthUint8, &dbValue);
        dbValue &= 0xEF;
        WriteIO (SB_IOMAP_REG71, AccWidthUint8, &dbValue);


  if (getRevisionID() >= SB700_A13){
    programPciByteTable( (REG8MASK*)FIXUP_PTR(&sbA13PorInitPciTable[0]), sizeof(sbA13PorInitPciTable)/sizeof(REG8MASK) );
    programPmioByteTable( (REG8MASK *)FIXUP_PTR(&sbA13PorPmioInitTbl[0]), (sizeof(sbA13PorPmioInitTbl)/sizeof(REG8MASK)) );
  }

  if ((getRevisionID() >= SB700_A14) )
    programPciByteTable( (REG8MASK*)FIXUP_PTR(&sbA14PorInitPciTable[0]), sizeof(sbA14PorInitPciTable)/sizeof(REG8MASK) );

  if ( (getRevisionID() >= SB700_A14) && ( (pConfig->TimerClockSource == 1) || (pConfig->TimerClockSource == 2) )){
    ReadPMIO(SB_PMIO_REGD4, AccWidthUint8, &dbVar1);
    if (!(dbVar1 & BIT6)){
      RWPMIO(SB_PMIO_REGD4, AccWidthUint8, 0xFF, BIT6);
      pConfig->RebootRequired=1;
    }
  }

  if (getRevisionID() > SB700_A11) {
    if (pConfig->PciClk5 == 1)
      RWPMIO(SB_PMIO_REG41, AccWidthUint8, ~(UINT32)BIT1, BIT1);    // Enabled PCICLK5 for A12
  }

  dbVar0 = (pBuildOptPtr->BiosSize + 1) & 7;
  if (dbVar0 > 4) {
    dbVar0 = 0;
  }
  //KZ [061811]-It's used wrong BIOS SIZE for Coreboot.  RWPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG6C, AccWidthUint8 | S3_SAVE, 0x00, 0xF8 << dbVar0);

  if (pConfig->Spi33Mhz)
    //spi reg0c[13:12] to 01h  to run spi 33Mhz in system bios
    RWMEM((pBuildOptPtr->SpiRomBaseAddress)+SB_SPI_MMIO_REG0C,AccWidthUint16 | S3_SAVE, ~(UINT32)(BIT13+BIT12), BIT12);

  //SB internal spread spectrum settings. A reboot is required if the spread spectrum settings have to be changed
  //from the existing value.
  ReadPMIO(SB_PMIO_REG42, AccWidthUint8, &dbVar0);
  if (pConfig->SpreadSpectrum != (dbVar0 >> 7) )
    pConfig->RebootRequired = 1;
  if (pConfig->SpreadSpectrum)
    RWPMIO(SB_PMIO_REG42, AccWidthUint8, ~(UINT32)BIT7, BIT7);
  else
    RWPMIO(SB_PMIO_REG42, AccWidthUint8, ~(UINT32)BIT7, 0);

  if  ( !(pConfig->S3Resume) ){
    //To detect whether internal clock chip is used, do the following procedure
    //set PMIO_B2[7]=1, then read PMIO_B0[4]; if it is 1, we are strapped to CLKGEN mode.
    //if it is 0, we are using clock chip on board.
    RWPMIO(SB_PMIO_REGB2, AccWidthUint8, 0xFF, BIT7);

    //Do the following programming only for SB700-A11.
    //1.  Set PMIO_B2 [7]=1 and read B0 and B1 and save those values.
    //2.  Set PMIO_B2 [7]=0
    //3.  Write the saved values from step 1, back to B0 and B1.
    //4.  Set PMIO_B2 [6]=1.
    ReadPMIO(SB_PMIO_REGB0, AccWidthUint16, &dwTempVar);
    if (getRevisionID() == SB700_A11){
      RWPMIO(SB_PMIO_REGB2, AccWidthUint8, ~(UINT32)BIT7, 00);
      WritePMIO(SB_PMIO_REGB0, AccWidthUint16, &dwTempVar);
      RWPMIO(SB_PMIO_REGB2, AccWidthUint8, 0xFF, BIT6);
    }

    if  (!(dwTempVar & BIT4)){
      RWPMIO(SB_PMIO_REGD0, AccWidthUint8, ~(UINT32)BIT0, 0); //Enable PLL2

      //we are in external clock chip on the board
      if (pConfig->UsbIntClock == CIMX_OPTION_ENABLED){
        //Configure usb clock to come from internal PLL
        RWPMIO(SB_PMIO_REGD2, AccWidthUint8, 0xFF, BIT3); //Enable 48Mhz clock from PLL2
        RWPMIO(SB_PMIO_REGBD, AccWidthUint8, ~(UINT32)BIT4, BIT4);  //Tell USB PHY to use internal 48Mhz clock from PLL2
      }
      else{
        //Configure usb clock to come from external clock
        RWPMIO(SB_PMIO_REGBD, AccWidthUint8, ~(UINT32)BIT4, 0);   //Tell USB PHY to use external 48Mhz clock from PLL2
        RWPMIO(SB_PMIO_REGD2, AccWidthUint8, ~(UINT32)BIT3, 00);  //Disable 48Mhz clock from PLL2
      }
    }
    else{
      //we are using internal clock chip on this board
      if (pConfig->UsbIntClock == CIMX_OPTION_ENABLED){
        //Configure usb clock to come from internal PLL
        RWPMIO(SB_PMIO_REGD2, AccWidthUint8, ~(UINT32)BIT3, 0);   //Enable 48Mhz clock from PLL2
        RWPMIO(SB_PMIO_REGBD, AccWidthUint8, ~(UINT32)BIT4, BIT4);  //Tell USB PHY to use internal 48Mhz clock from PLL2
      }
      else{
        //Configure usb clock to come from external clock
        RWPMIO(SB_PMIO_REGBD, AccWidthUint8, ~(UINT32)BIT4, 0);   //Tell USB PHY to use external 48Mhz clock from PLL2
        RWPMIO(SB_PMIO_REGD2, AccWidthUint8, ~(UINT32)BIT3, BIT3);  //Disable 48Mhz clock from PLL2
      }
    }

    ReadPMIO(SB_PMIO_REG43, AccWidthUint8, &dbVar0);
    RWPMIO(SB_PMIO_REG43, AccWidthUint8, ~(UINT32)(BIT6+BIT5+BIT0), (pConfig->UsbIntClock << 5));
    //Check whether our usb clock settings changed compared to previous boot, if yes then we need to reboot.
    if  ( (dbVar0 & BIT0) || ( (pConfig->UsbIntClock) != ((dbVar0 & (BIT6+BIT5)) >> 5)) )  pConfig->RebootRequired = 1;
  }

  if  (pBuildOptPtr->LegacyFree)        //if LEGACY FREE system
    RWPCI(((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG44), AccWidthUint32 | S3_SAVE, 00, 0x0003C000);
  else
    RWPCI(((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG44), AccWidthUint32 | S3_SAVE, 00, 0xFF03FFD5);

  if ( (getRevisionID() == SB700_A14) || (getRevisionID() == SB700_A13)){
    RWPMIO(SB_PMIO_REG65, AccWidthUint8, 0xFF, BIT7);
    RWPMIO(SB_PMIO_REG75, AccWidthUint8, 0xC0, BIT0);
    RWPMIO(SB_PMIO_REG52, AccWidthUint8, 0xC0, BIT1);
  }

  if (getRevisionID() >= SB700_A15) {
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG40+3, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT3), 0);
    //Enable unconditional shutdown fix in A15
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG38+1, AccWidthUint8 | S3_SAVE, 0xFF, BIT4);
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG40+3, AccWidthUint8 | S3_SAVE, 0xFF, BIT3);
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG06+1, AccWidthUint8 | S3_SAVE, 0xFF, 0xD0);
  }

  // [Updated RPR] Set ImcHostSmArbEn(SMBUS:0xE1[5]) only when IMC is enabled
  if (isEcPresent()) {
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGE1, AccWidthUint8 | S3_SAVE, 0xFF, BIT5);
  }

  //According to AMD Family 15h Models 00h-0fh processor BKDG section 2.12.8 LDTSTOP requirement
  // to program VID/FID LDTSTP# duration selection register
  AMDFamily15CpuLdtStopReq();

#ifndef NO_EC_SUPPORT
  ecPowerOnInit(pBuildOptPtr, pConfig);
#endif
}


void  setRevisionID(void){
  UINT8 dbVar0, dbVar1;

  ReadPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG08), AccWidthUint8, &dbVar0);
  ReadPMIO(SB_PMIO_REG53, AccWidthUint8, &dbVar1);
  if ( (dbVar0 == 0x39) && (dbVar1 & BIT6) && !(dbVar1 & BIT7)){
    RWPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG40), AccWidthUint8, ~(UINT32)BIT0, BIT0);
    RWPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG08), AccWidthUint8, 00, SB700_A12);
    RWPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG40), AccWidthUint8, ~(UINT32)BIT0, 00);
  }
  ReadPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG08), AccWidthUint8, &dbVar0);
}


UINT8 getRevisionID(void){
  UINT8 dbVar0;

  ReadPCI(((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG08), AccWidthUint8, &dbVar0);
  return  dbVar0;
}


void AMDFamily15CpuLdtStopReq(void) {
  CPUID_DATA   CpuId;
  CPUID_DATA   CpuId_Brand;
  UINT8   dbVar0, dbVar1, dbVar2;

  //According to AMD Family 15h Models 00h-0fh processor BKDG section 2.12.8 LDTSTOP requirement
  //to program VID/FID LDTSTP# duration selection register
  //If any of the following system configuration properties are true LDTSTP# assertion time required by the processor is 10us:
  // 1. Any link in the system operating at a Gen 1 Frequency.
  // 2. Also for server platform (G34/C32) set PM_REG8A[6:4]=100b (16us)

  CpuidRead (0x01, &CpuId);
  CpuidRead (0x80000001, &CpuId_Brand);   //BrandID, to read socket type
  if ((CpuId.REG_EAX & 0xFFFFFF00) == 0x00600F00) {

    //Program to Gen 3 default value - 001b
    RWPMIO(SB_PMIO_REG8A, AccWidthUint8, 0x8F, 0x10);   //set [6:4]=001b

    //Any link in the system operating at a Gen 1 Frequency.
    //Check Link 0 - Link connected regsister
    ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REG98), AccWidthUint8, &dbVar2);
    dbVar2 = dbVar2 & 0x01;

    if(dbVar2 == 0x01) {
      //Check Link 0 - Link Frequency Freq[4:0]
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REG89), AccWidthUint8, &dbVar0);
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REG9C), AccWidthUint8, &dbVar1);
      dbVar0 = dbVar0 & 0x0F;       //Freq[3:0]
      dbVar1 = dbVar1 & 0x01;       //Freq[4]
      dbVar0 = (dbVar1 << 4) | dbVar0;  //Freq[4:0]
      //Value 6 or less indicate Gen1
      if(dbVar0 <= 0x6) {
        RWPMIO(SB_PMIO_REG8A, AccWidthUint8, 0x8F, 0x40);   //set [6:4]=100b
      }
    }

    //Check Link 1 - Link connected regsister
    ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGB8), AccWidthUint8, &dbVar2);
    dbVar2 = dbVar2 & 0x01;
    if(dbVar2 == 0x01) {
      //Check Link 1 - Link Frequency Freq[4:0]
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGA9), AccWidthUint8, &dbVar0);
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGBC), AccWidthUint8, &dbVar1);
      dbVar0 = dbVar0 & 0x0F;       //Freq[3:0]
      dbVar1 = dbVar1 & 0x01;       //Freq[4]
      dbVar0 = (dbVar1 << 4) | dbVar0;  //Freq[4:0]
      //Value 6 or less indicate Gen1
      if(dbVar0 <= 0x6) {
        RWPMIO(SB_PMIO_REG8A, AccWidthUint8, 0x8F, 0x40);   //set [6:4]=100b
      }
    }

    //Check Link 2 - Link connected regsister
    ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGD8), AccWidthUint8, &dbVar2);
    dbVar2 = dbVar2 & 0x01;
    if(dbVar2 == 0x01) {
      //Check Link 2 - Link Frequency Freq[4:0]
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGC9), AccWidthUint8, &dbVar0);
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGDC), AccWidthUint8, &dbVar1);
      dbVar0 = dbVar0 & 0x0F;       //Freq[3:0]
      dbVar1 = dbVar1 & 0x01;       //Freq[4]
      dbVar0 = (dbVar1 << 4) | dbVar0;  //Freq[4:0]
      //Value 6 or less indicate Gen1
      if(dbVar0 <= 0x6) {
        RWPMIO(SB_PMIO_REG8A, AccWidthUint8, 0x8F, 0x40);   //set [6:4]=100b
      }
    }

    //Check Link 3 - Link connected regsister
    ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGF8), AccWidthUint8, &dbVar2);
    dbVar2 = dbVar2 & 0x01;
    if(dbVar2 == 0x01) {
      //Check Link 3 - Link Frequency Freq[4:0]
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGE9), AccWidthUint8, &dbVar0);
      ReadPCI(((HT_LINK_BUS_DEV_FUN << 16) + HT_LINK_REGFC), AccWidthUint8, &dbVar1);
      dbVar0 = dbVar0 & 0x0F;       //Freq[3:0]
      dbVar1 = dbVar1 & 0x01;       //Freq[4]
      dbVar0 = ((dbVar1 << 4) | dbVar0);  //Freq[4:0]
      //Value 6 or less indicate Gen1
      if(dbVar0 <= 0x6) {
        RWPMIO(SB_PMIO_REG8A, AccWidthUint8, 0x8F, 0x40);   //set [6:4]=100b
      }
    }

    // Server platform (G34/C32) set PM_REG8A[6:4]=100b (16us)
    if(((CpuId_Brand.REG_EBX & 0xF0000000) == 0x30000000) || ((CpuId_Brand.REG_EBX & 0xF0000000) == 0x50000000)) {
      RWPMIO(SB_PMIO_REG8A, AccWidthUint8, 0x8F, 0x40);   //set [6:4]=100b
    }
  }

}

