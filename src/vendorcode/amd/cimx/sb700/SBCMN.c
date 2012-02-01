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


#include        "Platform.h"


REG8MASK sbEarlyPostByteInitTable[]={
        // SMBUS Device(Bus 0, Dev 20, Func 0)
        {0x00, SMBUS_BUS_DEV_FUN, 0},
        {SB_SMBUS_REG43, ~(UINT8)BIT3, 0x00},                  //Make BAR registers of smbus visible.
        {SB_SMBUS_REG24, 0X00, (CIMx_Version & 0xFF)},  //Program the version information
        {SB_SMBUS_REG24+1, 0x00, (CIMx_Version >> 8)},
        {SB_SMBUS_REG24+2, 0x00, RC_Information},
        {SB_SMBUS_REG24+3, 0x00, Additional_Changes_Indicator},
        {SB_SMBUS_REG43, ~(UINT8)BIT3, BIT3},                  //Make BAR registers of smbus invisible.
        {SB_SMBUS_REGAE, ~(UINT8)(BIT6 + BIT5), BIT6 + BIT5},  //Disable Timer IRQ enhancement for proper operation of the 8254 timer.
                                                        // [6] - IoApicPicArbEn, set 1 to enable arbiter between APIC and PIC interrupts
        {SB_SMBUS_REGAD, ~(UINT8)(BIT0+BIT1+BIT2+BIT4), BIT0+BIT3},    // Initialize SATA to default values, SATA Enabled,
                                                                // Combined mode enabled, SATA as primary, power saving enable
        {SB_SMBUS_REGAF, 0xE3, 6 << 2},                 // Set SATA Interrupt to INTG#
        {SB_SMBUS_REG68, BIT3, 0 },                             //      First disable all usb controllers and then enable then according to setup selection
        {0xFF, 0xFF, 0xFF},

        // IDE Device(Bus 0, Dev 20, Func 1)
        {0x00, IDE_BUS_DEV_FUN, 0},
        {SB_IDE_REG62+1, ~(UINT8)BIT0, BIT5},                  // Enabling IDE Explicit Pre-Fetch  IDE PCI Config 0x62[8]=0
                                                        // Allow MSI capability of IDE controller to be visible. IDE PCI Config 0x62[13]=1
        {0xFF, 0xFF, 0xFF},

        // Azalia Device(Bus 0, Dev 20, Func 2)
        {0x00, AZALIA_BUS_DEV_FUN, 0},
        {SB_AZ_REG4C, ~(UINT8)BIT0, BIT0},
        {0xFF, 0xFF, 0xFF},

        // LPC Device(Bus 0, Dev 20, Func 3)
        {0x00, LPC_BUS_DEV_FUN, 0},

        {SB_LPC_REG40, ~(UINT8)BIT2, BIT2},            // Enabling LPC DMA Function  0x40[2]
        {SB_LPC_REG78, ~(UINT8)BIT1, 00},                      // Disables MSI capability
        {0xFF, 0xFF, 0xFF},

        // P2P Bridge(Bus 0, Dev 20, Func 4)
        {0x00, SBP2P_BUS_DEV_FUN, 0},

        {SB_P2P_REG64+1, 0xFF, BIT7+BIT6},      //Adjusting CLKRUN#, PCIB_PCI_Config 0x64[15]=01
                                                                                //Enabling arbiter fix, PCIB_PCI_Config 0x64[14]=01
        {SB_P2P_REG64+2, 0xFF, BIT4},   //Enabling One-Prefetch-Channel Mode, PCIB_PCI_config 0x64 [20]

        {SB_P2P_REG0D, 0x00, 0x40},             //Setting Latency Timers to 0x40, Enables the PCIB to retain ownership
        {SB_P2P_REG1B, 0x00, 0x40},             // of the bus on the Primary side and on the Secondary side when GNT# is deasserted.

        {0xFF, 0xFF, 0xFF},

        // SATA Device(Bus 0, Dev 17, Func 0)
        {0x00, SATA_BUS_DEV_FUN, 0},
        {SB_SATA_REG44, 0xff, BIT0},    // Enables the SATA watchdog timer register prior to the SATA BIOS post
        {SB_SATA_REG40+3, 0xff, BIT5},  // RPR setting: Disable the testing/enhancement mode    SATA_PCI_config 0x40 [29] = 1
        {SB_SATA_REG48+2, 0xff, BIT5},  // RPR setting: Disable the testing/enhancement mode    SATA_PCI_config 0x48 [24] = 1, [21] = 1
        {SB_SATA_REG48+3, 0xff, BIT0},
        {SB_SATA_REG44 + 2, 0, 0x10},   // Program watchdog timer with 16 retries before timer time-out.
        {0xFF, 0xFF, 0xFF},
};


REG8MASK sbEarlyPostPmioInitTbl[]={
        // index                andmask ormask
        {SB_PMIO_REG55, ~(UINT8)(BIT3+BIT4+BIT5), BIT5+BIT3}, //BIT3(PcieNative)=1b, BIT4(Pcie_Wak_Mask)=0b, BIT5(Pcie_WAK_Sci)=1b
        {SB_PMIO_REG01, 0xff, BIT1},
        {SB_PMIO_REG0E, 0xff, BIT2 + BIT3},
        {SB_PMIO_REG10, 0x3E, (BIT6+BIT5+BIT3+BIT1)},   // RTC_En_En + TMR_En_En + GLB_EN_EN and clear EOS_EN + PciExpWakeDisEn
        {SB_PMIO_REG61, 0xFF, 0x40},            // USB Device Support to Wakeup System from S3/S4 state, USB PME & PCI Act from NB
        {SB_PMIO_REG59, 0xFC, 0x00 },           // Clear the flash controller bits BIT1:0
        {SB_PMIO_REG01, 0xFF, 0x97 },           // Clear all the status
        {SB_PMIO_REG05, 0xFF, 0xFF },
        {SB_PMIO_REG06, 0xFF, 0xFF },
        {SB_PMIO_REG07, 0xFF, 0xFF },
        {SB_PMIO_REG0F, 0xFF, 0x1F },
        {SB_PMIO_REG1D, 0xFF, 0xFF },
        {SB_PMIO_REG39, 0xFF, 0xFF },
        {SB_PMIO_REG7C, ~(UINT8)(BIT5+BIT3+BIT2), BIT3+BIT2},          //Turn on BLink LED
        {SB_PMIO_REG67, 0xFF, 0x06},            // C State enable, must be set in order to exercise C state
        {SB_PMIO_REG68, 0x38, 0x84},
        {SB_PMIO_REG8D, 0xFF, 0x01},            // Set PM_Reg_0x8D[0] to enable PmeTurnOff/PmeMsgAck handshake to fix PCIE LAN S3/S4 wake failure
        {SB_PMIO_REG84, 0xFD, BIT3+BIT0},
        {SB_PMIO_REG53, 0xFF, BIT7+BIT6},       //ACPI System Clock setting, PMIO Reg 0x53[6]=1. Our reference clock
                                                                                //is either 25 or 100Mhz and so the default acpi clock is actually
                                                                                //running at 12.5Mhz and so the system time will run slow.  We have
                                                                                //generated another internal clock which runs at 14.318Mhz which is the
                                                                                //correct frequency.  We should set this bit to turn on this feature PMIO_REG53[6]=1
                                                                                //PCI Clock Period, PM_IO 0x53 [7] = 1. By setting this, PCI clock period
                                                                                //increase to 30.8 ns.
        {SB_PMIO_REG95, ~(UINT8)(BIT2+BIT1+BIT0), BIT2+BIT1},  //USB Advanced Sleep Control, Enables USB EHCI controller
                                                                                                        //to sleep for 6 uframes in stead of the standard 10us to
                                                                                                        //improve power saving.
        {SB_PMIO_REGD7, 0xFF, BIT6+BIT1},

};


// commonInitEarlyBoot - set /SMBUS/ACPI/IDE/LPC/PCIB. This settings should be done during S3 resume also
void    commonInitEarlyBoot(AMDSBCFG* pConfig) {
        UINT16       dwTempVar;
        CPUID_DATA   CpuId;
        CPUID_DATA   CpuId_Brand;
        UINT8        dbValue;
        UINT32       ddValue;
  UINT8        Family, Model, Stepping;

        TRACE((DMSG_SB_TRACE, "CIMx - Entering commonInitEarlyBoot \n"));
        CpuidRead (0x01, &CpuId);
        CpuidRead (0x80000001, &CpuId_Brand);           //BrandID

        //Early post initialization of pci config space
        programPciByteTable( (REG8MASK*)FIXUP_PTR(&sbEarlyPostByteInitTable[0]), sizeof(sbEarlyPostByteInitTable)/sizeof(REG8MASK) );

        // RPR 5.5 Clear PM_IO 0x65[4] UsbResetByPciRstEnable, Set this bit so that usb gets reset whenever there is PCIRST.
        RWPMIO(SB_PMIO_REG65, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT4, BIT4);


        #if 0 //KZ [083011]-It's used wrong BIOS SIZE for Coreboot.
        //For being compatible with earlier revision, check whether ROM decoding is changed already outside CIMx before
        //changing it.
        ReadPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG68, AccWidthUint16 | S3_SAVE, &dwTempVar);
        if ( (dwTempVar == 0x08) || (dwTempVar == 0x00))
                RWPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG68, AccWidthUint8 | S3_SAVE, 0, 0x0E);// Change the 1Mb below ROM decoding range to 0xE0000 to 0xFFFFF
        #endif

        if      (pConfig->AzaliaController == 1)
                RWPMIO(SB_PMIO_REG59, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT3, 0);
        else
                RWPMIO(SB_PMIO_REG59, AccWidthUint8 | S3_SAVE, 0xFF, BIT3);

        //Disable or Enable PCI Clks based on input
        RWPCI((SBP2P_BUS_DEV_FUN << 16) + SB_P2P_REG42, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT5+BIT4+BIT3+BIT2), ((pConfig->PciClks) & 0x0F) << 2 );
        RWPCI((SBP2P_BUS_DEV_FUN << 16) + SB_P2P_REG4A, AccWidthUint8 | S3_SAVE, ~(UINT32)(BIT1+BIT0), ((pConfig->PciClks) >> 4) | ((pConfig->PciClk5) << 1) );
        ReadPMIO(SB_PMIO_REG2C, AccWidthUint16, &dwTempVar);    // Read Arbiter address, Arbiter address is in PMIO 2Ch
        RWIO(dwTempVar, AccWidthUint8, 0, 0);                   // Write 0 to enable the arbiter

        abLinkInitBeforePciEnum(pConfig);                       // Set ABCFG registers
        // Set LDTSTP# duration to 10us for HydraD CPU model 8, 9 or A; or when HT link is 200MHz; or Family15 Orochi CPU C32/G34 package
        ddValue = CpuId.REG_EAX & 0x00FF00F0;
    dbValue = 1;

        if((CpuId.REG_EAX & 0x00F00F00) == 0x00600F00) {
                if(((CpuId_Brand.REG_EBX & 0xF0000000) == 0x30000000) || ((CpuId_Brand.REG_EBX & 0xF0000000) == 0x50000000)) {
                        //Orochi processor G34/C32, set to 10us
                        dbValue = 10;
                }
                else {
                        // Orochi processor AM3, set to 5us
                        dbValue = 5;
                }
        }

        if ((pConfig->AnyHT200MhzLink) || (ddValue == 0x100080) || (ddValue == 0x100090) || (ddValue == 0x1000A0)) {
        //any kind of CPU run HT at 200Mhz , or HydraD CPU model 8, 9 or A, set to 10us
          dbValue = 10;
        }


        RWPMIO(SB_PMIO_REG8B, AccWidthUint8 | S3_SAVE, 0x00, dbValue);

        // Enable/Disable watchdog timer
        RWPMIO(SB_PMIO_REG69, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT0, (UINT8)(!pConfig->WatchDogTimerEnable));

        // Per SB700/SP5100 RPR 2.5
        //
        // Enable C1e stutter timer for any system with chip revision >= A14
        // Set SMBUS:0x5c[22:16] = 16  -- Set amount of idle time to 16ms
        //

        if (getRevisionID() >= SB700_A14) {
          dwTempVar = 0x0010;

    // Set PMIO:0xcb[5] = 1  -- AutoStutterTimerEn, set 1 to enable
    // Set PMIO:0xcb[6] = 1  -- AutoStutterTimeSel, 1=1ms timer tick increment; 0=2us increment
          RWPMIO(SB_PMIO_REGCB, AccWidthUint8 | S3_SAVE, 0xff, BIT6 + BIT5);

      Family = (UINT8)((CpuId.REG_EAX & 0x00ff0000)>> 16);
      Model = (UINT8)((CpuId.REG_EAX & 0x000000f0)>> 4);
      Stepping = (UINT8) (CpuId.REG_EAX & 0x0000000f);

    // For Server system (SP5100) with CPU type = Family 10h with LS2 mode enabled:
    // Model=6 && Stepping=2 || Model=(4I5|6) && Stepping >=3 || Model=(8|9) && Stepping >= 1 || Model Ah
    // Set SMBUS:0x5c[22:16] = 20  -- Set amount of idle time to 20ms
          if (IsLs2Mode() && (Family == 0x10)) {
        switch( Model ){
          case 0x4:
          case 0x5:
                     if( Stepping >= 3 )  dwTempVar = 0x14;
                     break;
          case 0x6:
                     if( Stepping >= 2 )  dwTempVar = 0x14;
                     break;
          case 0x8:
                     if( Stepping >= 1 )  dwTempVar = 0x14;
                     break;
          case 0x9:
                     if( Stepping >= 1 )  dwTempVar = 0x14;
                     break;
          case 0xA:
                     dwTempVar = 0x14;
                     break;
        }
          }
          // Set SMBUS:0x5c[7] = 1       -- CheckC3, set 1 to check for C3 state
          RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG5C, AccWidthUint32 | S3_SAVE, ~(0x7F << 16), (dwTempVar << 16) + BIT7);
        }

        //Message-Triggered C1E is not supported in Family 10h G34r1 HY-D0 (0x00100F90) and Family 10h C32 HY-D0 (0x00100F80) processor.
        ddValue = CpuId.REG_EAX;
        if ((getRevisionID() == SB700_A15) && (pConfig->MTC1e == CIMX_OPTION_ENABLED) && (ddValue != 0x00100F90) && (ddValue != 0x00100F80)) {
          //
          // MTC1e: For A15 (server only) - The settings here borrow the existing legacy ACPI BM_STS and BM_RLD bits as a
          // mechanism to break out from C1e under a non-OS controlled C3 state. Under this scheme, the logic will automatically
          // clear the BM_STS bit whenever it enters C1e state. Whenever BM_REQ#/IDLE_EXIT# is detected, it will cause the
          // BM_STS bit to be set and therefore causing the C state logic to exit.
          //
          // Set BMReqEnable (SMBUS:0x64[5]=1) to enable the pin as BM_REQ#/IDLE_EXIT# to the C state logic
          // Set CheckOwnReq (SMBUS:0x64[4]=0) to force IDLE_EXIT# to set BM_STS and wake from C3
          RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG64, AccWidthUint8 | S3_SAVE, 0xEF, BIT5);

          // Set PCI_Active_enable (PMIO:0x61[2]=1), the secondary enable bit for SB to monitor BM_REQ#/IDLE_EXIT#
          RWPMIO(SB_PMIO_REG61, AccWidthUint8 | S3_SAVE, 0xff, BIT2);

          // Set auto_bm_rld (PMIO:0x9a[4]=1) so that assertion on BM_REQ#/IDLE_EXIT# pin will cause C state logic to break out from C1e
          // Set auto_clr_bm_sts (PMIO:0x9a[5]=1) will cause the C state logic to automatically clear the BM_STS bit whenever it sees a C1e entry
          RWPMIO(SB_PMIO_REG9A, AccWidthUint8 | S3_SAVE, 0xff, BIT5 + BIT4);


          // MTC1e: The logic basically counts the number of HALT_ENTER messages. When it has received the number of HALT_ENTER
          // messages equal to NumOfCpu (PMIO:0xc9[3:0]), it will generate an internal C1e command to the C state logic.
          // The count increments when it sees HALT_ENTER message after it has generated the C1e command, and it treats the
          // HALT_EXIT message as a break event.
          //
          // Set ServerCEn
          RWPMIO(SB_PMIO_REGBB, AccWidthUint8 | S3_SAVE, 0xFF, BIT7);

          // Enable counting HALT
          // PMIO:0xc9[4]   = CountHaltMsgEn
          // PMIO:0xc9[3:0] = NumOfCpu, set to 1 since CPU logic will coordinate among cores and only generate one HALT message
          RWPMIO(SB_PMIO_REGC9, AccWidthUint8 | S3_SAVE, 0xE0, BIT4 + 1);
        }

        c3PopupSetting(pConfig);

        TRACE((DMSG_SB_TRACE, "CIMx - Exiting commonInitEarlyBoot \n"));
}


void    commonInitEarlyPost(AMDSBCFG* pConfig){
        //early post initialization of pmio space
        programPmioByteTable( (REG8MASK *)FIXUP_PTR(&sbEarlyPostPmioInitTbl[0]), (sizeof(sbEarlyPostPmioInitTbl)/sizeof(REG8MASK)) );
        CallBackToOEM(PULL_UP_PULL_DOWN_SETTINGS, NULL, pConfig);
}


// AB-Link Configuration Table
ABTBLENTRY abTblEntry600[]={
        // Enabling Downstream Posted Transactions to Pass Non-Posted Transactions for the K8 Platform ABCFG 0x10090[8] = 1
        // ABCFG 0x10090 [16] = 1, ensures the SMI# message to be sent before the IO command is completed. The ordering of
        // SMI# and IO is important for the IO trap to work properly.
        {ABCFG,SB_AB_REG10090           ,BIT16+BIT8             ,BIT16+BIT8                     },
        // Enabling UpStream DMA Access AXCFG: 0x04[2]=1
        {AXCFG,SB_AB_REG04              ,BIT2                   ,BIT2                   },
        // Setting B-Link Prefetch Mode ABCFG 0x80 [17] = 1 ABCFG 0x80 [18] = 1
        {ABCFG,SB_AB_REG80              ,BIT17+BIT18            ,BIT17+BIT18    },
        // Disable B-Link client's credit variable in downstream arbitration equation (for All Revisions)
        // ABCFG 0x9C[0] = 1 Disable credit variable in downstream arbitration equation
        // Enabling Additional Address Bits Checking in Downstream Register Programming
        // ABCFG 0x9C[1] = 1
        {ABCFG,SB_AB_REG9C              ,BIT8+BIT1+BIT0                 ,BIT8+BIT1+BIT0 },
        // Enabling IDE/PCIB Prefetch for Performance Enhancement
        // IDE prefetch    ABCFG 0x10060 [17] = 1   ABCFG 0x10064 [17] = 1
        // PCIB prefetch   ABCFG 0x10060 [20] = 1   ABCFG 0x10064 [20] = 1
        {ABCFG,SB_AB_REG10060   ,BIT17+BIT20            ,BIT17+BIT20            },      //  IDE+PCIB prefetch enable
        {ABCFG,SB_AB_REG10064   ,BIT17+BIT20            ,BIT17+BIT20            },      //  IDE+PCIB prefetch enable
        // Enabling Detection of Upstream Interrupts ABCFG 0x94 [20] = 1
        // ABCFG 0x94 [19:0] = cpu interrupt delivery address [39:20]
        {ABCFG,SB_AB_REG94              ,BIT20                  ,BIT20+0x00FEE                  },
        // Programming cycle delay for AB and BIF clock gating
        // Enabling AB and BIF Clock Gating
        // Enabling AB Int_Arbiter Enhancement
        // Enabling Requester ID
        {ABCFG,SB_AB_REG10054,  0x00FFFFFF      , 0x010407FF    },
        {ABCFG,SB_AB_REG98      ,       0xFFFF00FF      , 0x00014700    },      // Enable the requestor ID for upstream traffic ABCFG 0x98[16]=1
//      {ABCFG,SB_AB_REG54      ,       0x00FF0000      , 0x01040000    },
        {ABCFG,SB_AB_REG54      ,       0x00FF0000      , 0x00040000    },

        {ABCFG,0,0,-1}, // This dummy entry is to clear ab index
        {-1, -1, -1, -1                                                                         },
};


// AB-Link Configuration Table
ABTBLENTRY abTblForA15[]={

        //SMI Reordering fix
        {ABCFG, SB_AB_REG90             ,BIT21                  , BIT21 },
        {ABCFG, SB_AB_REG9C             ,BIT15+BIT9+BIT5        ,BIT15+BIT9+BIT5},

        //Posted pass NP Downstream feature
        {AX_INDXC,      SB_AB_REG02,    BIT9            ,BIT9           },
        {ABCFG,         SB_AB_REG9C,    BIT14+BIT13+BIT12+BIT11+BIT10+BIT7+BIT6 , BIT14+BIT13+BIT12+BIT11+BIT10+BIT7+BIT6},
        {ABCFG,         SB_AB_REG1009C, BIT5+BIT4       , BIT5+BIT4},

        //Posted pass NP upstream feature
        {ABCFG,         SB_AB_REG58,    BIT15+BIT14+BIT13+BIT12+BIT11, BIT15+BIT14+BIT13+BIT11},

        //64 bit Non-posted memory write support
        {AX_INDXC,      SB_AB_REG02,    BIT10           ,BIT10          },

        {ABCFG,         SB_AB_REG10090, BIT12+BIT11+BIT10+BIT9  , BIT12+BIT11+BIT10+BIT9},

        {ABCFG,0,0,-1}, // This dummy entry is to clear ab index
        {-1, -1, -1, -1                                                                         },
};


// abLinkInitBeforePciEnum - Set ABCFG registers
void    abLinkInitBeforePciEnum(AMDSBCFG* pConfig){
        ABTBLENTRY      *pAbTblPtr;

        // disable PMIO decoding when AB is set
        RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG64, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT2, 0);

        pAbTblPtr = (ABTBLENTRY *)FIXUP_PTR(&abTblEntry600[0]);
        abcfgTbl(pAbTblPtr);

        if (getRevisionID() > SB700_A11){
                //Enable OHCI Prefetch
                writeAlink( (SB_AB_REG80 | (ABCFG << 30)), (readAlink((SB_AB_REG80 | (ABCFG << 30)))) | BIT0);
                //Register bit to maintain correct ordering of SMI and IO write completion
                writeAlink( (SB_AB_REG8C | (ABCFG << 30)), (readAlink((SB_AB_REG8C | (ABCFG << 30)))) | BIT8);
        }

        if (getRevisionID() >= SB700_A14){
                //Enable fix for TT SB01345
                writeAlink( (SB_AB_REG90 | (ABCFG << 30)), (readAlink((SB_AB_REG90 | (ABCFG << 30)))) | BIT17);
                //Disable IO Write and SMI ordering enhancement
                writeAlink( (SB_AB_REG9C | (ABCFG << 30)), (readAlink((SB_AB_REG9C | (ABCFG << 30)))) & (0xFFFFFEFF));
        }

        if (getRevisionID() >= SB700_A15) {
                pAbTblPtr = (ABTBLENTRY *)FIXUP_PTR(&abTblForA15[0]);
                abcfgTbl(pAbTblPtr);
        }


        // enable pmio decoding after ab is configured
        // or   BYTE PTR es:[ebp+SMBUS_BUS_DEV_FUN shl 12 + SB_SMBUS_REG64], BIT2
        RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG64, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT2, BIT2);
}


void    abcfgTbl(ABTBLENTRY* pABTbl){
        UINT32  ddValue;

        while ((pABTbl->regType) != 0xFF){
                TRACE((DMSG_SB_TRACE, "RegType: %X, RegNumber:%X, AndMask=%X, OrMask=%X \n",pABTbl->regType , pABTbl->regIndex, pABTbl->regMask, pABTbl->regData));
                if (pABTbl->regType > AX_INDXP){
                        ddValue = pABTbl->regIndex | (pABTbl->regType << 30);
                        writeAlink(ddValue, ((readAlink(ddValue)) & (0xFFFFFFFF^(pABTbl->regMask)))|pABTbl->regData);
                }
                else{
                        ddValue = 0x30 | (pABTbl->regType << 30);
                        writeAlink(ddValue, pABTbl->regIndex);
                        ddValue = 0x34 | (pABTbl->regType << 30);
                        writeAlink(ddValue, ((readAlink(ddValue)) & (0xFFFFFFFF^(pABTbl->regMask)))|pABTbl->regData);
                }
                ++pABTbl;
        }

        //Clear ALink Access Index
        ddValue = 0;
        WriteIO(ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &ddValue);
        TRACE((DMSG_SB_TRACE, "Exiting abcfgTbl\n"));
}


// programSubSystemIDs - Config Subsystem ID for all SB devices.
void    programSubSystemIDs(AMDSBCFG* pConfig, BUILDPARAM       *pStaticOptions){
        UINT32  ddTempVar;
        UINT16  dwDeviceId;

        RWPCI((USB1_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ohci0Ssid);
        RWPCI((USB1_OHCI1_BUS_DEV_FUN << 16) + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ohci1Ssid);
        RWPCI((USB2_OHCI0_BUS_DEV_FUN << 16) + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ohci2Ssid);
        RWPCI((USB2_OHCI1_BUS_DEV_FUN << 16) + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ohci3Ssid);
        RWPCI((USB3_OHCI_BUS_DEV_FUN << 16) + SB_OHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ohci4Ssid);

        RWPCI((USB1_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ehci0Ssid);
        RWPCI((USB2_EHCI_BUS_DEV_FUN << 16) + SB_EHCI_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->Ehci1Ssid);

        RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->SmbusSsid);
        RWPCI((IDE_BUS_DEV_FUN << 16) + SB_IDE_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->IdeSsid);
        RWPCI((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->LpcSsid);
        RWPCI((AZALIA_BUS_DEV_FUN << 16) + SB_AZ_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pStaticOptions->AzaliaSsid);

        ddTempVar = pStaticOptions->SataIDESsid;
        if ( ((pConfig->SataClass) == AHCI_MODE) || ((pConfig->SataClass)== IDE_TO_AHCI_MODE) )
                ddTempVar = pStaticOptions->SataAHCISsid;

        ReadPCI(((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG02), AccWidthUint16 | S3_SAVE, &dwDeviceId);
        if ((pConfig->SataClass) == RAID_MODE){
                ddTempVar = pStaticOptions->SataRAIDSsid;
                if (dwDeviceId==SB750_SATA_DEFAULT_DEVICE_ID)
                        ddTempVar = pStaticOptions->SataRAID5Ssid;
        }

        if ( ((pConfig->SataClass) == AMD_AHCI_MODE) || ((pConfig->SataClass) == IDE_TO_AMD_AHCI_MODE) ) {
                ddTempVar = pStaticOptions->SataAHCISsid;
        }
        RWPCI((SATA_BUS_DEV_FUN << 16) + SB_SATA_REG2C, AccWidthUint32 | S3_SAVE, 0x00, ddTempVar);
}

void    commonInitLateBoot(AMDSBCFG* pConfig){
        UINT8   dbValue;
        UINT32  ddVar;

        // We need to do the following setting in late post also because some bios core pci enumeration changes these values
        // programmed during early post.
        // RPR 4.5 Master Latency Timer
        // Master Latency Timer PCIB_PCI_config 0x0D/0x1B = 0x40
        // Enables the PCIB to retain ownership of the bus on the
        // Primary side and on the Secondary side when GNT# is deasserted.
        //mov   BYTE PTR es:[ebp+SBP2P_BUS_DEV_FUN shl 12 + SB_P2P_REG0D], 40h
        //mov   BYTE PTR es:[ebp+SBP2P_BUS_DEV_FUN shl 12 + SB_P2P_REG1B], 40h
        dbValue = 0x40;
        WritePCI((SBP2P_BUS_DEV_FUN << 16) + SB_P2P_REG0D, AccWidthUint8, &dbValue);
        WritePCI((SBP2P_BUS_DEV_FUN << 16) + SB_P2P_REG1B, AccWidthUint8, &dbValue);

        //SB P2P AutoClock control settings.
        ddVar = (pConfig->PcibAutoClkCtrlLow) | (pConfig->PcibAutoClkCtrlLow);
        WritePCI((SBP2P_BUS_DEV_FUN << 16) + SB_P2P_REG4C, AccWidthUint32, &ddVar);
        ddVar = (pConfig->PcibClkStopOverride);
        RWPCI((SBP2P_BUS_DEV_FUN << 16) + SB_P2P_REG50, AccWidthUint16, 0x3F, (UINT16) (ddVar << 6));

        if (pConfig->MobilePowerSavings){
                //If RTC clock is not driven to any chip, it should be shut-off. If system uses external RTC, then SB needs to
                //drive out RTC clk to external RTC chip. If system uses internal RTC, then this clk can be shut off.
                RWPMIO(SB_PMIO_REG68, AccWidthUint8, ~(UINT32)BIT4, (pConfig->ExternalRTCClock)<<4);
                if (!getClockMode()){
                        if  (!(pConfig->UsbIntClock) ){
                                //If the external clock is used, the second PLL should be shut down
                                RWPMIO(SB_PMIO_REGD0, AccWidthUint8, 0xFF, BIT0);
                                // If external clock mode is used, the 25Mhz oscillator buffer can be turned-off by setting  PMIO 0xD4[7]=1
                                RWPMIO(SB_PMIO_REGD4, AccWidthUint8, 0xFF, BIT7);
                                //Disable unused clocks
                                RWPMIO(SB_PMIO_REGCA, AccWidthUint8, 0xFF, 0x7E);
                        }
                }
                writeAlink(0x30, SB_AB_REG40);
                writeAlink(0x34, ((readAlink(0x34)) & 0xFFFF0000) | 0x008A);

        }
        else{
                //Don't shutoff RTC clock
                RWPMIO(SB_PMIO_REG68, AccWidthUint8, ~(UINT32)BIT4, 0);
                //Dont disable second PLL
                RWPMIO(SB_PMIO_REGD0, AccWidthUint8, ~(UINT32)BIT0, 0);
                //Enable the 25Mhz oscillator
                RWPMIO(SB_PMIO_REGD4, AccWidthUint8, ~(UINT32)BIT7, 0);
                RWPMIO(SB_PMIO_REGCA, AccWidthUint8, 0xFF, 0x00);
        }
}


void
hpetInit (AMDSBCFG* pConfig, BUILDPARAM *pStaticOptions)
{
  DESCRIPTION_HEADER*   pHpetTable;

  if (pConfig->HpetTimer == 1) {
    UINT8      dbTemp;

    RWPMIO(SB_PMIO_REG9A, AccWidthUint8, 0xFF, BIT7);
    // Program the HPET BAR address
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REGB4, AccWidthUint32 | S3_SAVE, 0, pStaticOptions->HpetBase);

    // Enable HPET MMIO decoding: SMBUS:0x43[4] = 1
    // Enable HPET MSI support only when HpetMsiDis == 0
    dbTemp = (pConfig->HpetMsiDis)? BIT4 : BIT7 + BIT6 + BIT5 + BIT4;
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, ~(UINT32)BIT3, dbTemp);
    // Program HPET default clock period
    if (getRevisionID() >= SB700_A13) {
      RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG34, AccWidthUint32 | S3_SAVE, 0x00, 0x429B17E);
    }
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + SB_SMBUS_REG43, AccWidthUint8 | S3_SAVE, 0xFF, BIT3);
    // Enable High Precision Event Timer (also called Multimedia Timer) interrupt
    RWPCI((SMBUS_BUS_DEV_FUN << 16) + (SB_SMBUS_REG64+1), AccWidthUint8 | S3_SAVE, ~(UINT32)BIT2, BIT2);
  }
  else {
    if (!(pConfig->S3Resume)) {
//      pHpetTable = (DESCRIPTION_HEADER*)ACPI_LocateTable('TEPH');
      pHpetTable = (DESCRIPTION_HEADER*)ACPI_LocateTable(Int32FromChar ('T', 'E', 'P', 'H'));
      if (pHpetTable != NULL) {
//        pHpetTable->Signature = 'HPET';
        pHpetTable->Signature = Int32FromChar ('T', 'E', 'P', 'H');
      }
    }
  }
}


void c3PopupSetting(AMDSBCFG* pConfig){
        UINT8   dbTemp;
        CPUID_DATA   CpuId;

        CpuidRead (0x01, &CpuId);
        //RPR 2.3 C-State and VID/FID Change
        dbTemp = GetNumberOfCpuCores();
        if (dbTemp > 1){
                //PM_IO 0x9A[5]=1, For system with dual core CPU, set this bit to 1 to automatically clear BM_STS when the C3 state is being initiated.
                //PM_IO 0x9A[4]=1, For system with dual core CPU, set this bit to 1 and BM_STS will cause C3 to wakeup regardless of BM_RLD
                //PM_IO 0x9A[2]=1, Enable pop-up for C3. For internal bus mastering or BmReq# from the NB, the SB will de-assert
                                                   //LDTSTP# (pop-up) to allow DMA traffic, then assert LDTSTP# again after some idle time.
                RWPMIO(SB_PMIO_REG9A, AccWidthUint8, 0xFF, BIT5+BIT4+BIT2);
        }

        //SB700 needs to changed for RD790 support
        //PM_IO 0x8F [4] = 0 for system with RS690
        //Note: RS690 north bridge has AllowLdtStop built for both display and PCIE traffic to wake up the HT link.
        //BmReq# needs to be ignored otherwise may cause LDTSTP# not to toggle.
        //PM_IO 0x8F[5]=1, Ignore BM_STS_SET message from NB
        RWPMIO(SB_PMIO_REG8F, AccWidthUint8, ~(UINT32)(BIT5+BIT4), BIT5);

        //LdtStartTime = 10h for minimum LDTSTP# de-assertion duration of 16us in StutterMode. This is to guarantee that
        //the HT link has been safely reconnected before it can be disconnected again. If C3 pop-up is enabled, the 16us also
        //serves as the minimum idle time before LDTSTP# can be asserted again. This allows DMA to finish before the HT
        //link is disconnected.
    //Increase LDTSTOP Deassertion time for SP5100 to 20us, SB700 remains the same
    dbTemp = (IsServer())? 0x14 : 0x10;
        RWPMIO(SB_PMIO_REG88, AccWidthUint8, 0x00, dbTemp);

        //This setting provides 16us delay before the assertion of LDTSTOP# when C3 is entered. The
        //delay will allow USB DMA to go on in a continous manner
        RWPMIO(SB_PMIO_REG89, AccWidthUint8, 0x00, 0x10);

        //Set this bit to allow pop-up request being latched during the minimum LDTSTP# assertion time
        RWPMIO(SB_PMIO_REG52, AccWidthUint8, 0xFF, BIT7);

}

