/**
 * @file
 *
 * Southbridge Initial routine
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

VOID abcfgTbl (IN ABTBLENTRY* pABTbl);

/**
 * sbUsbPhySetting - USB Phy Calibration Adjustment
 *
 *
 * @param[in] Value  Controller PCI config address (bus# + device# + function#)
 *
 */
VOID sbUsbPhySetting (IN  UINT32 Value);


/*--------------------------- Documentation Pages ---------------------------*/
/**
 *  @page LegacyInterfaceCalls  Legacy Interface Calls
 *  <TD>@subpage SB_POWERON_INIT_Page "SB_POWERON_INIT"</TD><TD></TD>
 *  <TD>@subpage SB_BEFORE_PCI_INIT_Page "SB_BEFORE_PCI_INIT"</TD><TD></TD>
 *  <TD>@subpage SB_AFTER_PCI_INIT_Page "SB_AFTER_PCI_INIT"</TD><TD></TD>
 *  <TD>@subpage SB_LATE_POST_INIT_Page "SB_LATE_POST_INIT"</TD><TD></TD>
 *  <TD>@subpage SB_BEFORE_PCI_RESTORE_INIT_Page "SB_BEFORE_PCI_RESTORE_INIT"</TD><TD></TD>
 *  <TD>@subpage SB_AFTER_PCI_RESTORE_INIT_Page "SB_AFTER_PCI_RESTORE_INIT"</TD><TD></TD>
 *  <TD>@subpage SB_SMM_SERVICE_Page "SB_SMM_SERVICE"</TD><TD></TD>
 *  <TD>@subpage SB_SMM_ACPION_Page "SB_SMM_ACPION"</TD><TD></TD>
 *
 *  @page LegacyInterfaceCallOuts  Legacy Interface CallOuts
 *  <TD>@subpage CB_SBGPP_RESET_ASSERT_Page CB_SBGPP_RESET_ASSERT
 *  <TD>@subpage CB_SBGPP_RESET_DEASSERT_Page CB_SBGPP_RESET_DEASSERT
 *
*/

/**
 * sbEarlyPostByteInitTable - PCI device registers initial during early POST.
 *
 */
static const REG8MASK sbEarlyPostByteInitTable[] =
{
  // SMBUS Device (Bus 0, Dev 20, Func 0)
  {0x00, SMBUS_BUS_DEV_FUN, 0},
  {SB_CFG_REG10, 0X00, (SBCIMx_Version & 0xFF)}, //Program the version information
  {SB_CFG_REG11, 0X00, (SBCIMx_Version >> 8)},
  {0xFF, 0xFF, 0xFF},

  // IDE Device (Bus 0, Dev 20, Func 1)
  {0x00, IDE_BUS_DEV_FUN, 0},
  {SB_IDE_REG62 + 1, ~BIT0, BIT5},  // Enabling IDE Explicit Pre-Fetch  IDE PCI Config 0x62[8]=0
  // Allow MSI capability of IDE controller to be visible. IDE PCI Config 0x62[13]=1
  {0xFF, 0xFF, 0xFF},

  // Azalia Device (Bus 0, Dev 20, Func 2)
  {0x00, AZALIA_BUS_DEV_FUN, 0},
  {SB_AZ_REG4C, ~BIT0, BIT0},
  {0xFF, 0xFF, 0xFF},

  // LPC Device (Bus 0, Dev 20, Func 3)
  {0x00, LPC_BUS_DEV_FUN, 0},
  {SB_LPC_REG40, ~BIT2, BIT2},     // RPR 1.1 Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b
  {SB_LPC_REG78, ~BIT0, 00},       // RPR 1.1 Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b
  {SB_LPC_REG78, ~BIT1, 00},       // Disables MSI capability
  {SB_LPC_REGBB, ~BIT0, BIT0 + BIT3 + BIT4 + BIT5},     // Enabled SPI Prefetch from HOST.
  {0xFF, 0xFF, 0xFF},

  // PCIB Bridge (Bus 0, Dev 20, Func 4)
  {0x00, PCIB_BUS_DEV_FUN, 0},
  {SB_PCIB_REG40, 0xFF, BIT5},      // RPR PCI-bridge Subtractive Decode
  {SB_PCIB_REG4B, 0xFF, BIT7},      //
  {SB_PCIB_REG66, 0xFF, BIT4},      // RPR Enabling One-Prefetch-Channel Mode, PCIB_PCI_config 0x64 [20]
  {SB_PCIB_REG65, 0xFF, BIT7},      // RPR proper operation of CLKRUN#.
  {SB_PCIB_REG0D, 0x00, 0x40},      // Setting Latency Timers to 0x40, Enables the PCIB to retain ownership
  {SB_PCIB_REG1B, 0x00, 0x40},      // of the bus on the Primary side and on the Secondary side when GNT# is deasserted.
  {SB_PCIB_REG66 + 1, 0xFF, BIT1},  // RPR Enable PCI bus GNT3#..
  {0xFF, 0xFF, 0xFF},

  // SATA Device (Bus 0, Dev 17, Func 0)
  {0x00, SATA_BUS_DEV_FUN, 0},
  {SB_SATA_REG44, 0xff, BIT0},       // Enables the SATA watchdog timer register prior to the SATA BIOS post
  {SB_SATA_REG44 + 2, 0, 0x20},      // RPR 8.12 SATA PCI Watchdog timer setting
                                     // [SB01923] Set timer out to 0x20 to fix IDE to SATA Bridge dropping drive issue.
  {0xFF, 0xFF, 0xFF},
};


/**
 * sbPmioEPostInitTable - Southbridge ACPI MMIO initial during POST.
 *
 */
static const AcpiRegWrite sbPmioEPostInitTable[] =
{
  // HPET workaround
  {PMIO_BASE >> 8,  SB_PMIOA_REG54 + 3, 0xFC, BIT0 + BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REG54 + 2, 0x7F, BIT7},
  {PMIO_BASE >> 8,  SB_PMIOA_REG54 + 2, 0x7F, 0x00},
  // End of HPET workaround
  // Enable SB800 A12 ACPI bits at PMIO 0xC0 [30,10:3]
  // ClrAllStsInThermalEvent 3 Set to 1 to allow ASF remote power down/power cycle, Thermal event, Fan slow event to clear all the Gevent status and enabled bits. The bit should be set to 1 all the time.
  // UsbGoodClkDlyEn         4 Set to 1 to delay de-assertion of Usb clk by 6 Osc clk. The bit should be set to 1 all the time.
  // ForceNBCPUPwr           5 Set to 1 to force CPU pwrGood to be toggled along with NB pwrGood.
  // MergeUsbPerReq          6 Set to 1 to merge usb perdical traffic into usb request as one of break event.
  // IMCWatchDogRstEn        7 Set to 1 to allow IMC watchdog timer to reset entire acpi block. The bit should be set to 1 when IMC is enabled.
  // GeventStsFixEn          8 1: Gevent status is not reset by its enable bit. 0: Gevent status is reset by its enable bit.
  // PmeTimerFixEn           9 Set to 1 to reset Pme Timer when going to sleep state.
  // UserRst2EcEn           10 Set to 1 to route user reset event to Ec. The bit should be set to 1 when IMC is enabled.
  // Smbus0ClkSEn           30 Set to 1 to enable SMBus0 controller clock stretch support.
  {PMIO_BASE >> 8, SB_PMIOA_REGC4, ~(BIT2 + BIT4), BIT2 + BIT4},
  {PMIO_BASE >> 8, SB_PMIOA_REGC0, 0, 0xF9},
  // PM_reg xC1 [3] = 1b, per RPR 2.7 CPU PwrGood Setting
  {PMIO_BASE >> 8, SB_PMIOA_REGC0 + 1, 0x04, 0x0B},
  // RtcSts              19-17 RTC_STS set only in Sleep State.
  // GppPme                 20 Set to 1 to enable PME request from SB GPP.
  // Pcireset               22 Set to 1 to allow SW to reset PCIe.
  {PMIO_BASE >> 8, SB_PMIOA_REGC2, 0x20, 0x58},
  {PMIO_BASE >> 8, SB_PMIOA_REGC2 + 1, 0, 0x40},

  //Item Id:      SB02037: RTC_STS should be set in S state
  //set PMIO 0xC0 [19:16] Set to 1110 to allow RTC_STS to be set only in non_G0 state.
  //{PMIO_BASE >> 8, SB_PMIOA_REGC2, (UINT8)~(0x0F), 0x0E},

  //Item Id:      SB02034
  //Title:        SB GPP NIC auto wake at second time sleep
  //set PMIO 0xC4 bit 2 to 1 then set PMIO 0xC0 bit 20 to 1 to enable fix for SB02034

  {PMIO_BASE >> 8, SB_PMIOA_REGC2, ~(BIT4), BIT4},

  //{GPIO_BASE >> 8, SB_GPIO_REG62 , 0x00, 0x4E},
  {PMIO_BASE >> 8, SB_PMIOA_REG74, 0x00, BIT0 + BIT1 + BIT2 + BIT4},
  {PMIO_BASE >> 8, SB_PMIOA_REGDE + 1, ~(BIT0 + BIT1), BIT0 + BIT1},
  {PMIO_BASE >> 8, SB_PMIOA_REGDE, ~BIT4, BIT4},
  {PMIO_BASE >> 8, SB_PMIOA_REGBA, ~BIT3, BIT3},
  {PMIO_BASE >> 8, SB_PMIOA_REGBA + 1, ~BIT6, BIT6},
  {PMIO_BASE >> 8, SB_PMIOA_REGBC, ~BIT1, BIT1},
  {PMIO_BASE >> 8, SB_PMIOA_REGED, ~(BIT0 + BIT1), 0},
  //RPR Hiding Flash Controller PM_IO 0xDC[7] = 0x0 & PM_IO 0xDC [1:0]=0x01
  {PMIO_BASE >> 8, SB_PMIOA_REGDC, 0x7C, BIT0},
  // RPR Turning off FC clock
  {MISC_BASE >> 8, SB_MISC_REG40 + 1, ~(BIT3 + BIT2), BIT3 + BIT2},
  {MISC_BASE >> 8, SB_MISC_REG40 + 2, ~BIT0, BIT0},
  {SMI_BASE >> 8, SB_SMI_Gevent0, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent1, 0, 1},
  {SMI_BASE >> 8, SB_SMI_Gevent2, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent3, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent4, 0, 4},
  {SMI_BASE >> 8, SB_SMI_Gevent5, 0, 5},
  {SMI_BASE >> 8, SB_SMI_Gevent6, 0, 6},
  {SMI_BASE >> 8, SB_SMI_Gevent7, 0, 29},

  {SMI_BASE >> 8, SB_SMI_Gevent9, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent10, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent11, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent12, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent13, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent14, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent15, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent16, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent17, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent18, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent19, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent20, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent21, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent22, 0, 29},
  {SMI_BASE >> 8, SB_SMI_Gevent23, 0, 29},
//
  {SMI_BASE >> 8, SB_SMI_Usbwakup0, 0, 11},
  {SMI_BASE >> 8, SB_SMI_Usbwakup1, 0, 11},
  {SMI_BASE >> 8, SB_SMI_Usbwakup2, 0, 11},
  {SMI_BASE >> 8, SB_SMI_Usbwakup3, 0, 11},
  {SMI_BASE >> 8, SB_SMI_IMCGevent0, 0, 12},
  {SMI_BASE >> 8, SB_SMI_IMCGevent1, 0, 29},
  {SMI_BASE >> 8, SB_SMI_FanThGevent, 0, 13},
  {SMI_BASE >> 8, SB_SMI_SBGppPme0, 0, 15},
  {SMI_BASE >> 8, SB_SMI_SBGppPme1, 0, 16},
  {SMI_BASE >> 8, SB_SMI_SBGppPme2, 0, 17},
  {SMI_BASE >> 8, SB_SMI_SBGppPme3, 0, 18},
  {SMI_BASE >> 8, SB_SMI_SBGppHp0, 0, 29},
  {SMI_BASE >> 8, SB_SMI_SBGppHp1, 0, 29},
  {SMI_BASE >> 8, SB_SMI_SBGppHp2, 0, 29},
  {SMI_BASE >> 8, SB_SMI_SBGppHp3, 0, 29},
  {SMI_BASE >> 8, SB_SMI_GecPme, 0, 19},
  {SMI_BASE >> 8, SB_SMI_CIRPme, 0, 23},
  {SMI_BASE >> 8, SB_SMI_Gevent8, 0, 26},
  {SMI_BASE >> 8, SB_SMI_AzaliaPme, 0, 27},
  {SMI_BASE >> 8, SB_SMI_SataGevent0, 0, 30},
  {SMI_BASE >> 8, SB_SMI_SataGevent1, 0, 31},

  {SMI_BASE >> 8, SB_SMI_WakePinGevent, 0, 29},
  {SMI_BASE >> 8, SB_SMI_ASFMasterIntr, 0, 29},
  {SMI_BASE >> 8, SB_SMI_ASFSlaveIntr, 0, 29},

//  {SMI_BASE >> 8, SB_SMI_REG04, ~BIT4, BIT4},
//  {SMI_BASE >> 8, SB_SMI_REG04 + 1, ~BIT0, BIT0},
//  {SMI_BASE >> 8, SB_SMI_REG04 + 2, ~BIT3, BIT3},
  {SMI_BASE >> 8, SB_SMI_REG08,  ~BIT4, 0},
  {SMI_BASE >> 8, SB_SMI_REG08+3,  ~BIT2, 0},
//  {SMI_BASE >> 8, SB_SMI_REG0C,  ~BIT4, BIT4},
  {SMI_BASE >> 8, SB_SMI_REG0C + 2, ~BIT3, BIT3},
  {SMI_BASE >> 8, SB_SMI_TWARN, 0, 9},
  {SMI_BASE >> 8, SB_SMI_TMI, 0, 29},
  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * abTblEntry800 - AB-Link Configuration Table for SB800
 *
 */
static const ABTBLENTRY abTblEntry800[] =
{
  // RPR Enable downstream posted transactions to pass non-posted transactions.
  {ABCFG, SB_ABCFG_REG10090, BIT8 + BIT16, BIT8 + BIT16},

  // RPR Enable SB800 to issue memory read/write requests in the upstream direction.
  {AXCFG, SB_AB_REG04, BIT2, BIT2},

  // RPR Enabling IDE/PCIB Prefetch for Performance Enhancement
  // PCIB prefetch   ABCFG 0x10060 [20] = 1   ABCFG 0x10064 [20] = 1
  {ABCFG, SB_ABCFG_REG10060, BIT20, BIT20}, //  PCIB prefetch enable
  {ABCFG, SB_ABCFG_REG10064, BIT20, BIT20}, //  PCIB prefetch enable

  // RPR Controls the USB OHCI controller prefetch used for enhancing performance of ISO out devices.
  // RPR Setting B-Link Prefetch Mode (ABCFG 0x80 [18:17] = 11)
  {ABCFG, SB_ABCFG_REG80, BIT0 + BIT17 + BIT18, BIT0 + BIT17 + BIT18},

  // RPR Enabled SMI ordering enhancement. ABCFG 0x90[21]
  // RPR USB Delay A-Link Express L1 State. ABCFG 0x90[17]
  {ABCFG, SB_ABCFG_REG90, BIT21 + BIT17, BIT21 + BIT17},

  // RPR Disable the credit variable in the downstream arbitration equation
  // RPR Register bit to qualify additional address bits into downstream register programming. (A12 BIT1 default is set)
  {ABCFG, SB_ABCFG_REG9C, BIT0, BIT0},

  // RPR Enabling Detection of Upstream Interrupts ABCFG 0x94 [20] = 1
  // ABCFG 0x94 [19:0] = cpu interrupt delivery address [39:20]
  {ABCFG, SB_ABCFG_REG94, BIT20, BIT20 + 0x00FEE},

  // RPR Programming cycle delay for AB and BIF clock gating
  // RPR Enable the AB and BIF clock-gating logic.
  // RPR Enable the A-Link int_arbiter enhancement to allow the A-Link bandwidth to be used more efficiently
  // RPR Enable the requester ID for upstream traffic. [16]: SB/NB link [17]: GPP
  {ABCFG, SB_ABCFG_REG10054,  0x00FFFFFF, 0x010407FF},
  {ABCFG, SB_ABCFG_REG98,  0xFFFF00FF, 0x00034700},
  {ABCFG, SB_ABCFG_REG54,  0x00FF0000, 0x00040000},
  // RPR Non-Posted Memory Write Support
  {AX_INDXC, SB_AX_INDXC_REG10, BIT9, BIT9},
  {ABCFG, 0, 0, (UINT8) 0xFF},  // This dummy entry is to clear ab index
  { (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF},
};

/**
 * SbPcieOrderRule - AB-Link Configuration Table for ablink Post Pass Np Downstream/Upstream Feature
 *
 */
static const ABTBLENTRY SbPcieOrderRule[] =
{
// abPostPassNpDownStreamTbl
  {ABCFG, SB_ABCFG_REG10060, BIT31, BIT31},
  {ABCFG, SB_ABCFG_REG1009C, BIT4 + BIT5, BIT4 + BIT5},
  {ABCFG, SB_ABCFG_REG9C, BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7, BIT2 + BIT3 + BIT4 + BIT5 + BIT6 + BIT7},
  {ABCFG, SB_ABCFG_REG90, BIT21 + BIT22 + BIT23, BIT21 + BIT22 + BIT23},
  {ABCFG, SB_ABCFG_REGF0, BIT6 + BIT5, BIT6 + BIT5},
  {AXINDC, SB_AX_INDXC_REG02, BIT9, BIT9},
  {ABCFG, SB_ABCFG_REG10090, BIT9 + BIT10 + BIT11 + BIT12, BIT9 + BIT10 + BIT11 + BIT12},
// abPostPassNpUpStreamTbl
  {ABCFG, SB_ABCFG_REG58, BIT10, BIT10},
  {ABCFG, SB_ABCFG_REGF0, BIT3 + BIT4, BIT3 + BIT4},
  {ABCFG, SB_ABCFG_REG54, BIT1, BIT1},
  { (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF},
};

/**
 * commonInitEarlyBoot - Config Southbridge SMBUS/ACPI/IDE/LPC/PCIB.
 *
 *    This settings should be done during S3 resume also
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
commonInitEarlyBoot (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32   abValue;
  UINT16   dwTempVar;
  CPUID_DATA  CpuId;
  UINT8   cimIrConfig;
  UINT8   Data;

  cimIrConfig = (UINT8) pConfig->IrConfig;
#if  SB_CIMx_PARAMETER == 0
  cimIrConfig = cimIrConfigDefault;
#endif

  //IR init Logical device 0x05
  if ( cimIrConfig ) {
    // Enable EC_PortActive
    RWPCI (((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGA4), AccWidthUint16 | S3_SAVE, 0xFFFE, BIT0);
    EnterEcConfig ();
    RWEC8 (0x07, 0x00, 0x05);         //Select logical device 05, IR controller
    RWEC8 (0x60, 0x00, 0x05);         //Set Base Address to 550h
    RWEC8 (0x61, 0x00, 0x50);
    RWEC8 (0x70, 0xF0, 0x05);         //Set IRQ to 05h
    RWEC8 (0x30, 0x00, 0x01);         //Enable logical device 5, IR controller
    Data = 0xAB;
    WriteIO (0x550, AccWidthUint8, &Data);
    ReadIO (0x551, AccWidthUint8, &Data);
    Data = ((Data & 0xFC ) | cimIrConfig);
    WriteIO (0x551, AccWidthUint8, &Data);
    ExitEcConfig ();
    Data = 0xA0;                      // EC APIC index
    WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &Data);
    Data = 0x05;                      // IRQ5
    WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &Data);
  } else {
    EnterEcConfig ();
    RWEC8 (0x07, 0x00, 0x05);         //Select logical device 05, IR controller
    RWEC8 (0x30, 0x00, 0x00);         //Disable logical device 5, IR controller
    ExitEcConfig ();
  }


  CpuidRead (0x01, &CpuId);

  //
  // SB CFG programming
  //
  //Make BAR registers of smbus visible.
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8 + 1, AccWidthUint8, ~BIT6, 0);
  //Early post initialization of pci config space
  programPciByteTable ((REG8MASK*) FIXUP_PTR (&sbEarlyPostByteInitTable[0]), sizeof (sbEarlyPostByteInitTable) / sizeof (REG8MASK) );
  if ( pConfig->BuildParameters.SmbusSsid != 0 ) {
    RWPCI ((SMBUS_BUS_DEV_FUN << 16) + SB_CFG_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.SmbusSsid);
  }
  //Make BAR registers of smbus invisible.
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8 + 1, AccWidthUint8, ~BIT6, BIT6);

  //
  // LPC CFG programming
  //
  // SSID for LPC Controller
  if (pConfig->BuildParameters.LpcSsid != 0 ) {
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG2C, AccWidthUint32 | S3_SAVE, 0x00, pConfig->BuildParameters.LpcSsid);
  }
  // LPC MSI
  if ( pConfig->BuildParameters.LpcMsi) {
    RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REG78, AccWidthUint32 | S3_SAVE, ~BIT1, BIT1);
  }

  //
  // PCIB CFG programming
  //
  //Disable or Enable PCI Clks based on input
  RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG42, AccWidthUint8 | S3_SAVE, ~(BIT5 + BIT4 + BIT3 + BIT2), ((pConfig->PciClks) & 0x0F) << 2 );
  RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG4A, AccWidthUint8 | S3_SAVE, ~(BIT1 + BIT0), (pConfig->PciClks) >> 4  );
  // PCIB MSI
  if ( pConfig->BuildParameters.PcibMsi) {
    RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG40, AccWidthUint8 | S3_SAVE, ~BIT3, BIT3);
  }

  //
  // AB CFG programming
  //
  // Read Arbiter address, Arbiter address is in PMIO 6Ch
  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG6C, AccWidthUint16, &dwTempVar);
  RWIO (dwTempVar, AccWidthUint8, 0, 0);        // Write 0 to enable the arbiter

  abLinkInitBeforePciEnum (pConfig);            // Set ABCFG registers
  // AB MSI
  if ( pConfig->BuildParameters.AbMsi) {
    abValue = readAlink (SB_ABCFG_REG94 | ((UINT32) ABCFG << 29));
    abValue = abValue | BIT20;
    writeAlink (SB_ABCFG_REG94 | ((UINT32) ABCFG << 29), abValue);
  }


  //
  // SB Specific Function programming
  //

  // PCIE Native setting
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBA + 1, AccWidthUint8, ~BIT14, 0);
  if ( pConfig->NativePcieSupport == 1) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG74 + 3, AccWidthUint8, ~(BIT3 + BIT1 + BIT0), BIT2 + BIT0);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG74 + 3, AccWidthUint8, ~(BIT3 + BIT1 + BIT0), BIT2);
  }

#ifdef ACPI_SLEEP_TRAP
  // Set SLP_TYPE as SMI event
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REGB0, AccWidthUint8, ~(BIT2 + BIT3), BIT2);
  // Disabled SLP function for S1/S3/S4/S5
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBE, AccWidthUint8, ~BIT5, 0x00);
  // Set S state transition disabled (BIT0) force ACPI to send SMI message when writing to SLP_TYP Acpi register. (BIT1)
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG08 + 3, AccWidthUint8, ~(BIT0 + BIT1), BIT1);
  // Enabled Global Smi ( BIT7 clear as 0 to enable )
  RWMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REG98 + 3 , AccWidthUint8, ~BIT7, 0x00);
#endif
  if ( pConfig->SbUsbPll == 0) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF3, AccWidthUint8, 0, 0x20);
  }
  // Set Stutter timer settings
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 1, AccWidthUint8, ~(BIT3 + BIT4), BIT3 + BIT4);
  // Set LDTSTP# duration to 10us for HydraD CPU, or when HT link is 200MHz
  if ((pConfig->AnyHT200MhzLink) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x100080) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x100090) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x1000A0)) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94, AccWidthUint8, 0, 0x0A);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 3, AccWidthUint8 | S3_SAVE, 0xFE, 0x28);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94, AccWidthUint8, 0, 0x01);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 3, AccWidthUint8 | S3_SAVE, 0xFE, 0x20);
  }

  //PM_Reg 0x7A[15] (CountHaltMsgEn) should be set when C1e option is enabled
  //PM_Reg 0x7A[3:0] (NumOfCpu) should be set to 1h when C1e option is enabled
  //PM_Reg 0x80[13] has to set to 1 to enable Message C scheme.
  if (pConfig->MTC1e) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7A, AccWidthUint16 | S3_SAVE, 0x7FF0, BIT15 + 1);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 1, AccWidthUint8 | S3_SAVE, ~BIT5, BIT5);
  }

  programSbAcpiMmioTbl ((AcpiRegWrite *) (pConfig->OEMPROGTBL.OemProgrammingTablePtr_Ptr));
}

/**
 * abSpecialSetBeforePciEnum - Special setting ABCFG registers before PCI emulation.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
abSpecialSetBeforePciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32   abValue;
  abValue = readAlink (SB_ABCFG_REGC0 | ((UINT32) ABCFG << 29));
  abValue &= 0xf0;
  if ( pConfig->SbPcieOrderRule && abValue ) {
    abValue = readAlink (SB_RCINDXC_REG02 | ((UINT32) RCINDXC << 29));
    abValue = abValue | BIT9;
    writeAlink (SB_RCINDXC_REG02 | ((UINT32) RCINDXC << 29), abValue);
  }
}

VOID
usbDesertPll (
  IN       AMDSBCFG* pConfig
  )
{
  if ( pConfig->SbUsbPll == 0) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGF3, AccWidthUint8, 0, 0x00);
  }
}

/**
 * commonInitEarlyPost - Config Southbridge SMBUS/ACPI/IDE/LPC/PCIB.
 *
 *    This settings might not program during S3 resume
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
commonInitEarlyPost (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8  dbPortStatus;
  UINT8  cimSpreadSpectrum;
  UINT32 cimSpreadSpectrumType;

  cimSpreadSpectrum = pConfig->SpreadSpectrum;
  cimSpreadSpectrumType = pConfig->BuildParameters.SpreadSpectrumType;
#if  SB_CIMx_PARAMETER == 0
  cimSpreadSpectrum = cimSpreadSpectrumDefault;
  cimSpreadSpectrumType = cimSpreadSpectrumTypeDefault;
#endif
  programSbAcpiMmioTbl ((AcpiRegWrite*) FIXUP_PTR (&sbPmioEPostInitTable[0]));

  // CallBackToOEM (PULL_UP_PULL_DOWN_SETTINGS, NULL, pConfig);

  if ( cimSpreadSpectrum ) {
  // Misc_Reg_40[25]=1 -> allow to change spread profile
  // Misc_Reg19=83 -> new spread profile
  // Misc_Reg[12:10]=9975be
  // Misc_Reg0B=91
  // Misc_Reg09=21
  // Misc_Misc_Reg_08[0]=1 -> enable spread
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x43, AccWidthUint8, ~BIT1, BIT1);
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x19, AccWidthUint8, 0, 0x83);
    getChipSysMode (&dbPortStatus);
    if ( ((dbPortStatus & ChipSysIntClkGen) != ChipSysIntClkGen) ) {
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x1A, AccWidthUint8, ~(BIT5 + BIT6 + BIT7), 0x80);
    }

    if ( cimSpreadSpectrumType == 0 ) {
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x12, AccWidthUint8, 0, 0x99);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x11, AccWidthUint8, 0, 0x75);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccWidthUint8, 0, 0xBE);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x0B, AccWidthUint8, 0, 0x91);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x09, AccWidthUint8, 0, 0x21);
    } else { // Spread profile for Ontario CPU related platform
      // This spread profile setting is for Ontario HDMI & DVI output from DP with -0.425%
      // Misc_Reg[12:10]=828FA8
      // Misc_Reg0B=11
      // Misc_Reg09=21
      // Misc_Reg10[25:24]=01b
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x12, AccWidthUint8, 0, 0x82);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x11, AccWidthUint8, 0, 0x8F);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccWidthUint8, 0, 0xA8);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x0B, AccWidthUint8, 0, 0x11);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x09, AccWidthUint8, 0, 0x21);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x13, AccWidthUint8, 0xFC, 0x1);
    }

    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG08, AccWidthUint8, 0xFE, 0x01);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG08, AccWidthUint8, 0xFE, 0x00);
  }

  // RPR PLL 100Mhz Reference Clock Buffer setting for internal clock generator mode
  getChipSysMode (&dbPortStatus);
  if ( ((dbPortStatus & ChipSysIntClkGen) == ChipSysIntClkGen) ) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE +  SB_MISC_REG04 + 1, AccWidthUint8, ~BIT5, BIT5);
  }

  // Set ASF SMBUS master function enabled here (temporary)
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG28, AccWidthUint16 | S3_SAVE, ~(BIT0 + BIT2), BIT0 + BIT2);

  programSbAcpiMmioTbl ((AcpiRegWrite *) (pConfig->OEMPROGTBL.OemProgrammingTablePtr_Ptr));
#ifndef NO_EC_SUPPORT
  // Software IMC enable
  if (((pConfig->BuildParameters.ImcEnableOverWrite == 1) && ((dbPortStatus & ChipSysEcEnable) == 0)) || ((pConfig->BuildParameters.ImcEnableOverWrite == 2) && ((dbPortStatus & ChipSysEcEnable) == ChipSysEcEnable))) {
    if (validateImcFirmware (pConfig)) {
      softwareToggleImcStrapping (pConfig);
    } else {
      CallBackToOEM (IMC_FIRMWARE_FAIL, 0, pConfig);
    }
  }
#endif

}
/**
 * abLinkInitBeforePciEnum - Set ABCFG registers before PCI emulation.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
abLinkInitBeforePciEnum (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  cimResetCpuOnSyncFlood;
  ABTBLENTRY  *pAbTblPtr;

  cimResetCpuOnSyncFlood = pConfig->ResetCpuOnSyncFlood;
#if  SB_CIMx_PARAMETER == 0
  cimResetCpuOnSyncFlood = cimResetCpuOnSyncFloodDefault;
#endif
  if ( pConfig->SbPcieOrderRule ) {
    pAbTblPtr = (ABTBLENTRY *) FIXUP_PTR (&SbPcieOrderRule[0]);
    abcfgTbl (pAbTblPtr);
  }
  pAbTblPtr = (ABTBLENTRY *) FIXUP_PTR (&abTblEntry800[0]);
  abcfgTbl (pAbTblPtr);
  if ( cimResetCpuOnSyncFlood ) {
    rwAlink (SB_ABCFG_REG10050 | ((UINT32) ABCFG << 29), ~BIT2, BIT2);
  }
}

/**
 * abcfgTbl - Program ABCFG by input table.
 *
 *
 * @param[in] pABTbl  ABCFG config table.
 *
 */
VOID
abcfgTbl (
  IN       ABTBLENTRY* pABTbl
  )
{
  UINT32   ddValue;

  while ( (pABTbl->regType) != 0xFF ) {
    if ( pABTbl->regType > AXINDC ) {
      ddValue = pABTbl->regIndex | ((UINT32) pABTbl->regType << 29);
      writeAlink (ddValue, ((readAlink (ddValue)) & (0xFFFFFFFF^ (pABTbl->regMask))) | pABTbl->regData);
    } else {
      ddValue = 0x30 | ((UINT32) pABTbl->regType << 29);
      writeAlink (ddValue, pABTbl->regIndex);
      ddValue = 0x34 | ((UINT32) pABTbl->regType << 29);
      writeAlink (ddValue, ((readAlink (ddValue)) & (0xFFFFFFFF^ (pABTbl->regMask))) | pABTbl->regData);
    }
    ++pABTbl;
  }

  //Clear ALink Access Index
  ddValue = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &ddValue);
}

/**
 * commonInitLateBoot - Prepare Southbridge register setting to boot to OS.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
commonInitLateBoot (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8  dbValue;
  UINT32  ddVar;
  // We need to do the following setting in late post also because some bios core pci enumeration changes these values
  // programmed during early post.
  // RPR 4.5 Master Latency Timer

  dbValue = 0x40;
  WritePCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG0D, AccWidthUint8, &dbValue);
  WritePCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG1B, AccWidthUint8, &dbValue);

  //SB P2P AutoClock control settings.
  ddVar = (pConfig->PcibAutoClkCtrlHigh << 16) | (pConfig->PcibAutoClkCtrlLow);
  WritePCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG4C, AccWidthUint32, &ddVar);
  ddVar = (pConfig->PcibClkStopOverride);
  RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG50, AccWidthUint16, 0x3F, (UINT16) (ddVar << 6));

  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGBB, AccWidthUint8, 0xBF | S3_SAVE, BIT3 + BIT4 + BIT5);

  // USB Phy Calibration Adjustment
  ddVar = (USB1_EHCI_BUS_DEV_FUN << 16);
  sbUsbPhySetting (ddVar);
  ddVar = (USB2_EHCI_BUS_DEV_FUN << 16);
  sbUsbPhySetting (ddVar);
  ddVar = (USB3_EHCI_BUS_DEV_FUN << 16);
  sbUsbPhySetting (ddVar);

  c3PopupSetting (pConfig);
  FusionRelatedSetting (pConfig);
}

/**
 * sbUsbPhySetting - USB Phy Calibration Adjustment
 *
 *
 * @param[in] Value  Controller PCI config address (bus# + device# + function#)
 *
 */
VOID
sbUsbPhySetting (
  IN       UINT32 Value
  )
{
  UINT32  ddBarAddress;
  UINT32  ddPhyStatus03;
  UINT32  ddPhyStatus4;
  UINT8   dbRevId;
  //Get BAR address
  ReadPCI ((UINT32) Value + SB_EHCI_REG10, AccWidthUint32, &ddBarAddress);
  if ( (ddBarAddress != - 1) && (ddBarAddress != 0) ) {
    ReadMEM ( ddBarAddress + SB_EHCI_BAR_REGA8, AccWidthUint32, &ddPhyStatus03);
    ReadMEM ( ddBarAddress + SB_EHCI_BAR_REGAC, AccWidthUint32, &ddPhyStatus4);
    ddPhyStatus03 &= 0x07070707;
    ddPhyStatus4 &= 0x00000007;
    if ( (ddPhyStatus03 != 0x00) | (ddPhyStatus4 != 0x00) ) {
      // RPR 7.7 USB 2.0 Ports Driving Strength step 1
      //Make BAR registers of smbus visible.
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8 + 1, AccWidthUint8, ~BIT6, 0);
      ReadPCI ((SMBUS_BUS_DEV_FUN << 16) + SB_CFG_REG08, AccWidthUint8, &dbRevId);
      //Make BAR registers of smbus invisible.
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8 + 1, AccWidthUint8, ~BIT6, BIT6);
      if (dbRevId == 0x41) { // A12
        RWMEM (ddBarAddress + SB_EHCI_BAR_REGC0, AccWidthUint32, 0xFFFF00FF, 0x1500);
        RWMEM (ddBarAddress + SB_EHCI_BAR_REGC4, AccWidthUint32, 0xFFFFF0FF, 0);
      } else if (dbRevId == 0x42) { // A13
        RWMEM (ddBarAddress + SB_EHCI_BAR_REGC0, AccWidthUint32, 0xFFFF00FF, 0x0F00);
        RWMEM (ddBarAddress + SB_EHCI_BAR_REGC4, AccWidthUint32, 0xFFFFF0FF, 0x0100);
      }
    }
  }
}

/**
 * hpetInit - Program Southbridge HPET function
 *
 *
 *
 * @param[in] pConfig         Southbridge configuration structure pointer.
 * @param[in] pStaticOptions  Platform build configuration table.
 *
 */
VOID
hpetInit (
  IN       AMDSBCFG* pConfig,
  IN       BUILDPARAM *pStaticOptions
  )
{
  DESCRIPTION_HEADER* pHpetTable;
  UINT8 cimHpetTimer;
  UINT8 cimHpetMsiDis;

  cimHpetTimer = (UINT8) pConfig->HpetTimer;
  cimHpetMsiDis = (UINT8) pConfig->HpetMsiDis;
#if  SB_CIMx_PARAMETER == 0
  cimHpetTimer = cimHpetTimerDefault;
  cimHpetMsiDis = cimHpetMsiDisDefault;
#endif
  pHpetTable = NULL;
  if ( cimHpetTimer == TRUE ) {
    //Program the HPET BAR address
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFF800, pStaticOptions->HpetBase);
    //Enabling decoding of HPET MMIO
    //Enable HPET MSI support
    //Enable High Precision Event Timer (also called Multimedia Timer) interrupt
    if ( cimHpetMsiDis == FALSE ) {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFF800, BIT0 + BIT1 + BIT2 + BIT3 + BIT4);
    } else {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFF800, BIT0 + BIT1);
    }

  } else {
    if ( ! (pConfig->S3Resume) ) {
      pHpetTable = (DESCRIPTION_HEADER*) ACPI_LocateTable (Int32FromChar('H', 'P', 'E', 'T'));
    }
    if ( pHpetTable != NULL ) {
      pHpetTable->Signature = Int32FromChar('T', 'E', 'P', 'H');
    }
  }
}

/**
 * c3PopupSetting - Program Southbridge C state function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID
c3PopupSetting (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8  dbValue;
  //RPR C-State and VID/FID Change
  dbValue = getNumberOfCpuCores  ();
  if (dbValue > 1) {
    //PM 0x80[2]=1, For system with dual core CPU, set this bit to 1 to automatically clear BM_STS when the C3 state is being initiated.
    //PM 0x80[1]=1, For system with dual core CPU, set this bit to 1 and BM_STS will cause C3 to wakeup regardless of BM_RLD
    //PM 0x7E[6]=1, Enable pop-up for C3. For internal bus mastering or BmReq# from the NB, the SB will de-assert
    //LDTSTP# (pop-up) to allow DMA traffic, then assert LDTSTP# again after some idle time.
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint8 | S3_SAVE, ~(BIT1 + BIT2), (BIT1 + BIT2));
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7E, AccWidthUint8 | S3_SAVE, ~BIT6, BIT6);
  }
  //SB800 needs to changed for RD790 support
  //PM 0x80 [8] = 0 for system with RS780
  //Note: RS690 north bridge has AllowLdtStop built for both display and PCIE traffic to wake up the HT link.
  //BmReq# needs to be ignored otherwise may cause LDTSTP# not to toggle.
  //PM_IO 0x80[3]=1, Ignore BM_STS_SET message from NB
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT9 + BIT8 + BIT7 + BIT4 + BIT3 + BIT2 + BIT1 + BIT0), 0x21F);
  //LdtStartTime = 10h for minimum LDTSTP# de-assertion duration of 16us in StutterMode. This is to guarantee that
  //the HT link has been safely reconnected before it can be disconnected again. If C3 pop-up is enabled, the 16us also
  //serves as the minimum idle time before LDTSTP# can be asserted again. This allows DMA to finish before the HT
  //link is disconnected.
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94 + 2, AccWidthUint8, 0, 0x10);

  //This setting provides 16us delay before the assertion of LDTSTOP# when C3 is entered. The
  //delay will allow USB DMA to go on in a continuous manner
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG98 + 1, AccWidthUint8, 0, 0x10);
  // Not in the RPR so far, it's hand writing from ASIC
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7C, AccWidthUint8 | S3_SAVE, 0, 0x85);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7C + 1, AccWidthUint8 | S3_SAVE, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7E + 1, AccWidthUint8 | S3_SAVE, ~(BIT7 + BIT5), BIT7 + BIT5);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88 + 1, AccWidthUint8 | S3_SAVE, ~BIT4, BIT4);
  // RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94, AccWidthUint8, 0, 0x10);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG98 + 3, AccWidthUint8, 0, 0x10);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGB4 + 1, AccWidthUint8, 0, 0x0B);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88, AccWidthUint8 | S3_SAVE, 0xFF, BIT4);
  if (pConfig->LdtStpDisable) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88, AccWidthUint8 | S3_SAVE, ~BIT5, 0);
  }
}

/**
 * FusionRelatedSetting - Program Fusion C related function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID
FusionRelatedSetting (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8 cimAcDcMsg;
  UINT8 cimTimerTickTrack;
  UINT8 cimClockInterruptTag;
  UINT8 cimOhciTrafficHanding;
  UINT8 cimEhciTrafficHanding;
  UINT8 cimFusionMsgCMultiCore;
  UINT8 cimFusionMsgCStage;
  UINT32 ddValue;

  cimAcDcMsg = (UINT8) pConfig->AcDcMsg;
  cimTimerTickTrack = (UINT8) pConfig->TimerTickTrack;
  cimClockInterruptTag = (UINT8) pConfig->ClockInterruptTag;
  cimOhciTrafficHanding = (UINT8) pConfig->OhciTrafficHanding;
  cimEhciTrafficHanding = (UINT8) pConfig->EhciTrafficHanding;
  cimFusionMsgCMultiCore = (UINT8) pConfig->FusionMsgCMultiCore;
  cimFusionMsgCStage = (UINT8) pConfig->FusionMsgCStage;
#if  SB_CIMx_PARAMETER == 0
  cimAcDcMsg = cimAcDcMsgDefault;
  cimTimerTickTrack = cimTimerTickTrackDefault;
  cimClockInterruptTag = cimClockInterruptTagDefault;
  cimOhciTrafficHanding = cimOhciTrafficHandingDefault;
  cimEhciTrafficHanding = cimEhciTrafficHandingDefault;
  cimFusionMsgCMultiCore = cimFusionMsgCMultiCoreDefault;
  cimFusionMsgCStage = cimFusionMsgCStageDefault;
#endif
  ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGA0, AccWidthUint32 | S3_SAVE, &ddValue);
  ddValue = ddValue & 0xC07F00A0;
  if ( cimAcDcMsg ) {
    ddValue = ddValue | BIT0;
  }
  if ( cimTimerTickTrack ) {
    ddValue = ddValue | BIT1;
  }
  if ( cimClockInterruptTag ) {
    ddValue = ddValue | BIT10;
  }
  if ( cimOhciTrafficHanding ) {
    ddValue = ddValue | BIT13;
  }
  if ( cimEhciTrafficHanding ) {
    ddValue = ddValue | BIT15;
  }
  if ( cimFusionMsgCMultiCore ) {
    ddValue = ddValue | BIT23;
  }
  if ( cimFusionMsgCStage ) {
    ddValue = (ddValue | (BIT6 + BIT4 + BIT3 + BIT2));
  }
  WriteMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGA0, AccWidthUint32 | S3_SAVE, &ddValue);
}
#ifndef NO_EC_SUPPORT
/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 * @retval        TRUE   Pass
 * @retval        FALSE  Failed
 */
BOOLEAN
validateImcFirmware (
  IN       AMDSBCFG* pConfig
  )
{
  UINT32  ImcSig;
  UINT32  ImcSigAddr;
  UINT32  ImcAddr;
  UINT32  CurAddr;
  UINT32  ImcBinSig0;
  UINT32  ImcBinSig1;
  UINT16  ImcBinSig2;
  UINT8  dbIMCChecksume;
  UINT8  dbIMC;
  ImcAddr = 0;

  // Software IMC enable
  ImcSigAddr = 0x80000; // start from 512k to 64M
  ImcSig = 0x0; //
  while ( ( ImcSig != 0x55aa55aa ) && ( ImcSigAddr <= 0x4000000 ) ) {
    CurAddr = 0xffffffff - ImcSigAddr + 0x20001;
    ReadMEM (CurAddr, AccWidthUint32, &ImcSig);
    ReadMEM ((CurAddr + 4), AccWidthUint32, &ImcAddr);
    ImcSigAddr <<= 1;
  }

  dbIMCChecksume = 0xff;
  if ( ImcSig == 0x55aa55aa ) {
    // "_AMD_IMC_C" at offset 0x2000 of the binary
    ReadMEM ((ImcAddr + 0x2000), AccWidthUint32, &ImcBinSig0);
    ReadMEM ((ImcAddr + 0x2004), AccWidthUint32, &ImcBinSig1);
    ReadMEM ((ImcAddr + 0x2008), AccWidthUint16, &ImcBinSig2);
    if ((ImcBinSig0 == 0x444D415F) && (ImcBinSig1 == 0x434D495F) && (ImcBinSig2 == 0x435F) ) {
      dbIMCChecksume = 0;
      for ( CurAddr = ImcAddr; CurAddr < ImcAddr + 0x10000; CurAddr++ ) {
        ReadMEM (CurAddr, AccWidthUint8, &dbIMC);
        dbIMCChecksume = dbIMCChecksume + dbIMC;
      }
    }
  }
  if ( dbIMCChecksume ) {
    return  FALSE;
  } else {
    return  TRUE;
  }
}

/**
 * softwareToggleImcStrapping - Software Toggle IMC Firmware Strapping.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
softwareToggleImcStrapping (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8  dbValue;
  UINT8  dbPortStatus;
  UINT32  abValue;
  UINT32  abValue1;

  getChipSysMode (&dbPortStatus);

  ReadPMIO (SB_PMIOA_REGBF, AccWidthUint8, &dbValue);
  //if ( (dbValue & (BIT6 + BIT7)) != 0xC0 ) {  // PwrGoodOut =1, PwrGoodEnB=1
  //The strapStatus register is not mapped into StrapOveride not in the same bit position. The following is difference.

  //StrapStatus                               StrapOverride
  //   bit4                                            bit17
  //   bit6                                            bit12
  //   bit12                                           bit15
  //   bit15                                           bit16
  //   bit16                                           bit18
  ReadMEM ((ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG80), AccWidthUint32, &abValue);
  abValue1 = abValue;
  if (abValue & BIT4) {
    abValue1 = (abValue1 & ~BIT4) | BIT17;
  }
  if (abValue & BIT6) {
    abValue1 = (abValue1 & ~BIT6) | BIT12;
  }
  if (abValue & BIT12) {
    abValue1 = (abValue1 & ~BIT12) | BIT15;
  }
  if (abValue & BIT15) {
    abValue1 = (abValue1 & ~BIT15) | BIT16;
  }
  if (abValue & BIT16) {
    abValue1 = (abValue1 & ~BIT16) | BIT18;
  }
  abValue1 |= BIT31;             // Overwrite enable
  if ((dbPortStatus & ChipSysEcEnable) == 0) {
    abValue1 |= BIT2;            // bit2- EcEnableStrap
  } else {
    abValue1 &= ~BIT2;           // bit2=0 EcEnableStrap
  }
  WriteMEM ((ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG84), AccWidthUint32, &abValue1);
  dbValue |= BIT6;               // PwrGoodOut =1
  dbValue &= ~BIT7;              // PwrGoodEnB =0
  WritePMIO (SB_PMIOA_REGBF, AccWidthUint8, &dbValue);

  dbValue = 06;
  WriteIO (0xcf9, AccWidthUint8, &dbValue);
  SbStall (0xffffffff);
}
#endif

#ifndef NO_HWM_SUPPORT
/**
 * validateImcFirmware - Validate IMC Firmware.
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
hwmInit (
  IN       AMDSBCFG* pConfig
  )
{
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xB2, AccWidthUint8 | S3_SAVE, 0, 0x55);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xB3, AccWidthUint8 | S3_SAVE, 0, 0x55);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x91, AccWidthUint8 | S3_SAVE, 0, 0x55);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x92, AccWidthUint8 | S3_SAVE, 0, 0x55);

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x00, AccWidthUint8 | S3_SAVE, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x10, AccWidthUint8 | S3_SAVE, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x20, AccWidthUint8 | S3_SAVE, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x30, AccWidthUint8 | S3_SAVE, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x40, AccWidthUint8 | S3_SAVE, 0, 0x06);

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x66, AccWidthUint8 | S3_SAVE, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x6B, AccWidthUint8 | S3_SAVE, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x70, AccWidthUint8 | S3_SAVE, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x75, AccWidthUint8 | S3_SAVE, 0, 0x01);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0x7A, AccWidthUint8 | S3_SAVE, 0, 0x01);

  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xF8, AccWidthUint8 | S3_SAVE, 0, 0x05);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xF9, AccWidthUint8 | S3_SAVE, 0, 0x06);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xFF, AccWidthUint8 | S3_SAVE, 0, 0x42);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xE9, AccWidthUint8 | S3_SAVE, 0, 0xFF);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xEB, AccWidthUint8 | S3_SAVE, 0, 0x1F);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xEF, AccWidthUint8 | S3_SAVE, 0, 0x04);
  RWMEM (ACPI_MMIO_BASE + PMIO2_BASE + 0xFB, AccWidthUint8 | S3_SAVE, 0, 0x00);
}
#endif
