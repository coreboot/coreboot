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
/*;********************************************************************************
;
; Copyright (c) 2011, Advanced Micro Devices, Inc.
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * Neither the name of Advanced Micro Devices, Inc. nor the names of
;       its contributors may be used to endorse or promote products derived
;       from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
; ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
;*********************************************************************************/

#include "SbPlatform.h"
#include "cbtypes.h"
#include "AmdSbLib.h"

//
// Declaration of local functions
//

VOID abcfgTbl (IN ABTBLENTRY* pABTbl);
VOID A13ResumeResetTwoSecondRtcWakeup (void);


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
REG8MASK sbEarlyPostByteInitTable[] =
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
  {SB_LPC_REG40, ~BIT2, BIT2},                          // RPR Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b
  {SB_LPC_REG48, 0x00, BIT0 + BIT1 + BIT2},
  {SB_LPC_REG78, 0xFC, 00},                             // RPR Enabling LPC DMA Function  0x40[2]=1b 0x78[0]=0b / Disables MSI capability
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
  {SB_SATA_REG44, 0xff, BIT0},       // RPR Enables the SATA watchdog timer register prior to the SATA BIOS post
  {SB_SATA_REG44 + 2, 0, 0x20},      // RPR SATA PCI Watchdog timer setting
                                     // [SB01923] Set timer out to 0x20 to fix IDE to SATA Bridge dropping drive issue.
  {0xFF, 0xFF, 0xFF},
};


/**
 * sbPmioEPostInitTable - Southbridge ACPI MMIO initial during POST.
 *
 */
AcpiRegWrite sbPmioEPostInitTable[] =
{
  {00, 00, 0xB0, 0xAC}, // Signature
  // HPET workaround
  {PMIO_BASE >> 8,  SB_PMIOA_REG54 + 3, 0xFC, BIT0 + BIT1},
  {PMIO_BASE >> 8,  SB_PMIOA_REG54 + 2, 0x7F, BIT7},
  {PMIO_BASE >> 8,  SB_PMIOA_REG54 + 2, 0x7F, 0x00},
  // End of HPET workaround
  // Enable Hudson-2 A12 ACPI bits at PMIO 0xC0 [30, 10:3]
  // ClrAllStsInThermalEvent 3 Set to 1 to allow ASF remote power down/power cycle, Thermal event, Fan slow event to clear all the Gevent status and enabled bits. The bit should be set to 1 all the time.
  // UsbGoodClkDlyEn         4 Set to 1 to delay de-assertion of Usb clk by 6 Osc clk. The bit should be set to 1 all the time.
  // ForceNBCPUPwr           5 Set to 1 to force CPU pwrGood to be toggled along with NB pwrGood.
  // MergeUsbPerReq          6 Set to 1 to merge usb perdical traffic into usb request as one of break event.
  // IMCWatchDogRstEn        7 Set to 1 to allow IMC watchdog timer to reset entire acpi block. The bit should be set to 1 when IMC is enabled.
  // GeventStsFixEn          8 1: Gevent status is not reset by its enable bit. 0: Gevent status is reset by its enable bit.
  // PmeTimerFixEn           9 Set to 1 to reset Pme Timer when going to sleep state.
  // UserRst2EcEn           10 Set to 1 to route user reset event to Ec. The bit should be set to 1 when IMC is enabled.
  // Smbus0ClkSEn           30 Set to 1 to enable SMBus0 controller clock stretch support.
  {PMIO_BASE >> 8, SB_PMIOA_REGC4, ~BIT2, BIT2},
  {PMIO_BASE >> 8, SB_PMIOA_REGC0, 0, 0xF9},
  {PMIO_BASE >> 8, SB_PMIOA_REGC0 + 1, 0x04, 0x03},
  // RtcSts              19-17 RTC_STS set only in Sleep State.
  // GppPme                 20 Set to 1 to enable PME request from SB GPP.
  // Pcireset               22 Set to 1 to allow SW to reset PCIe.
  {PMIO_BASE >> 8, SB_PMIOA_REGC2, 0x20, 0x58},
  {PMIO_BASE >> 8, SB_PMIOA_REGC2 + 1, 0, 0x40},
  {PMIO_BASE >> 8, SB_PMIOA_REGC2, ~(BIT4), BIT4},
  {PMIO_BASE >> 8, SB_PMIOA_REGCC, 0xF8, 0x01},
  {PMIO_BASE >> 8, SB_PMIOA_REG74, 0x00, BIT0 + BIT1 + BIT2 + BIT4},
  {PMIO_BASE >> 8, SB_PMIOA_REG74 + 3, ~BIT5, 0},
  {PMIO_BASE >> 8, SB_PMIOA_REGDE + 1, ~(BIT0 + BIT1), BIT0 + BIT1},
  {PMIO_BASE >> 8, SB_PMIOA_REGDE, ~BIT4, BIT4},
  {PMIO_BASE >> 8, SB_PMIOA_REGBA, ~BIT3, BIT3},
  {PMIO_BASE >> 8, SB_PMIOA_REGBA + 1, ~BIT6, BIT6},
  {PMIO_BASE >> 8, SB_PMIOA_REGBC, ~BIT1, BIT1},
  {PMIO_BASE >> 8, SB_PMIOA_REGED, ~(BIT4 + BIT0 + BIT1), 0},
  //RPR Hiding Flash Controller PM_IO 0xDC[7] = 0x0 & PM_IO 0xDC [1:0]=0x01
  {PMIO_BASE >> 8, SB_PMIOA_REGDC, 0x7C, BIT0},
  {SMI_BASE >> 8, SB_SMI_Gevent1, 0, 1},
  {SMI_BASE >> 8, SB_SMI_Gevent3, 0, 3},
  {SMI_BASE >> 8, SB_SMI_Gevent4, 0, 4},
  {SMI_BASE >> 8, SB_SMI_Gevent5, 0, 5},
  {SMI_BASE >> 8, SB_SMI_Gevent6, 0, 6},
  {SMI_BASE >> 8, SB_SMI_Gevent23, 0, 23},
  {SMI_BASE >> 8, SB_SMI_xHC0Pme, 0, 11},
  {SMI_BASE >> 8, SB_SMI_xHC1Pme, 0, 11},
  {SMI_BASE >> 8, SB_SMI_Usbwakup0, 0, 11},
  {SMI_BASE >> 8, SB_SMI_Usbwakup1, 0, 11},
#ifndef USB_LOGO_SUPPORT
  {SMI_BASE >> 8, SB_SMI_Usbwakup2, 0, 11},
  {SMI_BASE >> 8, SB_SMI_Usbwakup3, 0, 11},
#endif
  {SMI_BASE >> 8, SB_SMI_IMCGevent0, 0, 12},
  {SMI_BASE >> 8, SB_SMI_FanThGevent, 0, 13},
  {SMI_BASE >> 8, SB_SMI_SBGppPme0, 0, 15},
  {SMI_BASE >> 8, SB_SMI_SBGppPme1, 0, 16},
  {SMI_BASE >> 8, SB_SMI_SBGppPme2, 0, 17},
  {SMI_BASE >> 8, SB_SMI_SBGppPme3, 0, 18},
  {SMI_BASE >> 8, SB_SMI_GecPme, 0, 19},
  {SMI_BASE >> 8, SB_SMI_CIRPme, 0, 28},
  {SMI_BASE >> 8, SB_SMI_Gevent8, 0, 24},
  {SMI_BASE >> 8, SB_SMI_AzaliaPme, 0, 27},
  {SMI_BASE >> 8, SB_SMI_SataGevent0, 0, 30},
  {SMI_BASE >> 8, SB_SMI_SataGevent1, 0, 31},
  {SMI_BASE >> 8, SB_SMI_REG08,  0xE7, 0},
  {SMI_BASE >> 8, SB_SMI_REG0C + 2, ~BIT3, BIT3},
  {SMI_BASE >> 8, SB_SMI_TWARN, 0, 9},
// RPR CG PLL CMOX Clock Driver Setting for power saving
  {MISC_BASE >> 8, SB_MISC_REG18 + 0x06, 0, 0xE0},
  {MISC_BASE >> 8, SB_MISC_REG18 + 0x07, 0, 0x1F},
  //{SERIAL_DEBUG_BASE >> 8, SB_SDB_REG74, 0, 0},
  {0xFF, 0xFF, 0xFF, 0xFF},
};

/**
 * abTblEntry800 - AB-Link Configuration Table for Hudson-2
 *
 */
ABTBLENTRY abTblEntry800[] =
{
  // RPR Enable downstream posted transactions to pass non-posted transactions.
  {ABCFG, SB_ABCFG_REG10090, BIT8 + BIT16, BIT8 + BIT16},

  // RPR Enable Hudson-2 to issue memory read/write requests in the upstream direction.
  {AXCFG, SB_AB_REG04, BIT2, BIT2},

  // RPR Enabling IDE/PCIB Prefetch for Performance Enhancement
  // PCIB prefetch   ABCFG 0x10060 [20] = 1   ABCFG 0x10064 [20] = 1
  {ABCFG, SB_ABCFG_REG10060, BIT20, BIT20}, //  PCIB prefetch enable
  {ABCFG, SB_ABCFG_REG10064, BIT20, BIT20}, //  PCIB prefetch enable

  // RPR Controls the USB OHCI controller prefetch used for enhancing performance of ISO out devices.
  // RPR Setting B-Link Prefetch Mode (ABCFG 0x80 [18:17] = 11)
  {ABCFG, SB_ABCFG_REG80, BIT0 + BIT17 + BIT18, BIT0 + BIT17 + BIT18},

  // RPR Enabled SMI ordering enhancement. ABCFG 0x90[21]
  // RPR 7.7 USB Delay A-Link Express L1 State. ABCFG 0x90[16]
  {ABCFG, SB_ABCFG_REG90, BIT21 + BIT16, BIT21 + BIT16},

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
  {ABCFG, SB_ABCFG_REG98,  0xFFFC00FF, 0x00034700},
  {ABCFG, SB_ABCFG_REG54,  0x00FF0000, 0x00040000},
  // RPR Non-Posted Memory Write Support
  {AXINDC, SB_AX_INDXC_REG10, BIT9, BIT9},
  // RPR 4.18 UMI L1 Configuration
  //Step 1: AXINDC_Reg 0x02[0] = 0x1 Set REGS_DLP_IGNORE_IN_L1_EN to ignore DLLPs during L1 so that txclk can be turned off.
  //Step 2: AXINDP_Reg 0x02[15] = 0x1 Sets REGS_LC_ALLOW_TX_L1_CONTROL to allow TX to prevent LC from going to L1 when there are outstanding completions.
  {AXINDC, SB_AX_INDXC_REG02, BIT0, BIT0},
  {AXINDP, SB_AX_INDXP_REG02, BIT15, BIT15},
  {ABCFG, 0, 0, (UINT8) 0xFF},  // This dummy entry is to clear ab index
  { (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF, (UINT8)0xFF},
};

/**
 * SbPcieOrderRule - AB-Link Configuration Table for ablink Post Pass Np Downstream/Upstream Feature
 *
 */
ABTBLENTRY SbPcieOrderRule[] =
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
 * Table for SD controller capability register
 *
 *
 *
 *
 */
UINT32 sdCap[] =
{
  0x00000000, //
  0x039FD972, //
  0x839ED972, //
  0x839AD972, //
  0x839DD972, //
  0x839CD972, //
  0x8398D972, //
};

/**
 * Table for Spread Spectrum
 *
 * RPR 12.9 Internal Clock Generator Spread Profile - to set default value to 0.363%.
 *
 *
 */
SB_SPREAD_SPECTRUM_ENTRY SpreadParameterTable[] =
{
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},               //  Disabled
  {1, 0x318, 0, 0x6F83, 0x90, 0, 0x07, 0, 1, 1},// -3630 ppm :1:Default
  {1, 0x318, 0, 0x7AE1, 0x9F, 0, 0x0D, 0, 1, 1},// -4000 ppm :2
  {1, 0x318, 0, 0x77CF, 0x9B, 0, 0x0B, 0, 1, 1},// -3900 ppm :3
  {1, 0x318, 0, 0x74BC, 0x94, 0, 0x0A, 0, 1, 1},// -3800 ppm :4
  {1, 0x318, 0, 0x71AA, 0x93, 0, 0x08, 0, 1, 1},// -3700 ppm :5
  {1, 0x318, 0, 0x6FD2, 0x90, 0, 0x07, 0, 1, 1},// -3640 ppm :6
  {1, 0x318, 0, 0x6F83, 0x90, 0, 0x07, 0, 1, 1},// -3630 ppm :7
  {1, 0x318, 0, 0x6F35, 0x8F, 0, 0x07, 0, 1, 1},// -3620 ppm :8
  {1, 0x318, 0, 0x6EE6, 0x8F, 0, 0x07, 0, 1, 1},// -3610 ppm :9
  {1, 0x318, 0, 0x6E98, 0x8F, 0, 0x07, 0, 1, 1},// -3600 ppm :10
  {1, 0x318, 0, 0x6E49, 0x8E, 0, 0x07, 0, 1, 1},// -3590 ppm :11
  {1, 0x018, 0, 0x6666, 0x83, 0, 0x00, 0, 0, 1} // -3500 ppm :12
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
  SB_CPUID_DATA  CpuId;
  UINT8   cimNativepciesupport;
  UINT8   cimIrConfig;
  UINT8   Data;
  UINT8   cimALinkClkGateOff;
  UINT8   cimBLinkClkGateOff;

  cimNativepciesupport = (UINT8) pConfig->NativePcieSupport;
  cimIrConfig = (UINT8) pConfig->IrConfig;
  cimALinkClkGateOff = (UINT8) pConfig->ALinkClkGateOff;
  cimBLinkClkGateOff = (UINT8) pConfig->BLinkClkGateOff;

#if  SB_CIMx_PARAMETER == 0
  cimNativepciesupport = cimNativepciesupportDefault;
  cimIrConfig = cimIrConfigDefault;
#endif
  //Clear hwmSbtsiAutoPollStarted
  pConfig->hwm.hwmSbtsiAutoPollStarted = FALSE;
  //Ac Loss Control
  AcLossControl (pConfig->PwrFailShadow);

  //SB VGA Init
  // OBS194249 Cobia_Nutmeg_DP-VGA Electrical SI validation_Lower RGB Luminance level BGADJ=0x1F & DACADJ=0x1B
  // Removed for OBS194249 causes display issue in Windows
  // SbVgaInit ();

  //IR init Logical device 0x05
  if ( cimIrConfig ) {
    EnterEcConfig ();
    RWEC8 (0x07, 0x00, 0x05);         //Select logical device 05, IR controller
    RWEC8 (0x60, 0x00, 0x05);         //Set Base Address to 550h
    RWEC8 (0x61, 0x00, 0x50);
    RWEC8 (0x70, 0xF0, 0x05);         //Set IRQ to 05h
    RWEC8 (0x30, 0x00, 0x01);         //Enable logical device 5, IR controller
    Data = 0xAB;
    WriteIO (0x550, AccWidthUint8, &Data);
    ReadIO (0x551, AccWidthUint8, &Data);
    Data = (((Data & 0xFC ) | 0x20) | cimIrConfig);
    WriteIO (0x551, AccWidthUint8, &Data);
//    Data = 0xCA;
//    WriteIO (0x550, AccWidthUint8, &Data);
//    Data = 0x81;
//    WriteIO (0x551, AccWidthUint8, &Data);

    ExitEcConfig ();
    Data = 0xA0;                      // EC APIC index
    WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &Data);
    Data = 0x05;                      // IRQ5
    WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &Data);
  } else {
    EnterEcConfig ();
    //if (pConfig->Sdb != 1 ) {
      //RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE + 0x04, AccWidthUint8, 0, 0);
    //}
    RWEC8 (0x07, 0x00, 0x05);         //Select logical device 05, IR controller
    RWEC8 (0x30, 0x00, 0x00);         //Disable logical device 5, IR controller
    ExitEcConfig ();
  }


  TRACE ((DMSG_SB_TRACE, "CIMx - Entering commonInitEarlyBoot \n"));
  CpuidRead (0x01, &CpuId);

  //
  // SB CFG programming
  //
  //Make BAR registers of smbus visible.
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8 + 1, AccWidthUint8, ~BIT6, 0);
  //Early post initialization of pci config space
  programPciByteTable ((REG8MASK*) FIXUP_PTR (&sbEarlyPostByteInitTable[0]),
                       ARRAY_SIZE(sbEarlyPostByteInitTable));
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
  Data = ~(pConfig->PciClks);
  RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG42, AccWidthUint8 | S3_SAVE, ~(BIT5 + BIT4 + BIT3 + BIT2), (Data & 0x0F) << 2 );
  RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG4A, AccWidthUint8 | S3_SAVE, ~(BIT3 + BIT2 + BIT1 + BIT0), Data >> 4 );
  // PCIB MSI
  if ( pConfig->BuildParameters.PcibMsi) {
    RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG40, AccWidthUint8 | S3_SAVE, ~BIT3, BIT3);
  }

  if (  pConfig->SlowSpeedABlinkClock ) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG40, AccWidthUint8, ~BIT1, BIT1);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG40, AccWidthUint8, ~BIT1, 0);
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
    abValue = readAlink (SB_ABCFG_REG94 | (UINT32) (ABCFG << 29));
    abValue = abValue | BIT20;
    writeAlink (SB_ABCFG_REG94 | (UINT32) (ABCFG << 29), abValue);
  }


  //
  // SB Specific Function programming
  //

  // PCIE Native setting
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBA + 1, AccWidthUint8, ~BIT14, 0);
  if ( pConfig->NativePcieSupport == 1) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG74 + 3, AccWidthUint8, ~(BIT3 + BIT1 + BIT0), BIT3 + BIT2 + BIT0);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG74 + 3, AccWidthUint8, ~(BIT3 + BIT1 + BIT0), BIT3 + BIT2);
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
  // Set Stutter timer settings
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 1, AccWidthUint8, ~(BIT3 + BIT4), BIT3 + BIT4);
  // Set LDTSTP# duration to 10us for HydraD CPU, or when HT link is 200MHz
  if ((pConfig->AnyHT200MhzLink) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x100080) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x100090) || ((CpuId.EAX_Reg & 0x00ff00f0) == 0x1000A0)) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94, AccWidthUint8, 0, 0x0A);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 3, AccWidthUint8, 0xFE, 0x28);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94, AccWidthUint8, 0, 0x01);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80 + 3, AccWidthUint8, 0xFE, 0x20);
  }
  // A/B Clock Gate-OFF
  if ( (IsSbA12Plus ()) && ( cimALinkClkGateOff || cimBLinkClkGateOff )) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG04 + 2, AccWidthUint8, ~(BIT0), BIT0);
  }
  if ( cimALinkClkGateOff ) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccWidthUint8, 0xFE, BIT0);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccWidthUint8, 0xFE, 0x00);
  }
  if ( cimBLinkClkGateOff ) {
    if ( IsSbA11 () ) {
      // RPR13.7 B-Link Clock Gating Threshold for A11 only
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x2D, AccWidthUint8, 0xEF, 0x10); //A11 Only
    }
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccWidthUint8, 0xFD, BIT1);
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x2E, AccWidthUint8, 0xFD, 0x00);
  }
  // RPR SSC will provide better jitter margin
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x13, AccWidthUint8, 0xFC, 0x01);
  // Set ACPIMMIO by OEM Input table
  programSbAcpiMmioTbl ((AcpiRegWrite *) (pConfig->OEMPROGTBL.OemProgrammingTablePtr_Ptr));
  // 2.9 NB Power Good Control on System Reset
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBF, AccWidthUint8, ~ (BIT0), 0);
  // 2.10 Extend SerIrq request
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50, AccWidthUint32, ~ (BIT29), (BIT29));
  // 2.13 Clear status of SATA PERR
  Data = BIT6;
  WriteMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REG3C, AccWidthUint8, &Data);
  Data = BIT7;
  WriteMEM (ACPI_MMIO_BASE + SMI_BASE + SB_SMI_REG84 + 2, AccWidthUint8, &Data);

  if (IsSbA13Plus ()) {
    // 2.14 Enable Delayed SLP_S3/S5 to the Board
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC0 + 1, AccWidthUint8, 0xFF, BIT2);
    // 2.15 Enable C-State Wake-up before Warm Reset
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGBE, AccWidthUint8, ~ (BIT0), BIT0);
    // 3.16 Disable LPC A-Link Cycle Bypass
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG50, AccWidthUint32, ~ (BIT19), (BIT19));
  }
  //RPR 3.5 Enabling  LPCCLK0 Power-down Function
  //OBS261463 Torpedo-IMC Fan Control-System stops somewhere when running S3
  if ((!isImcEnabled ()) && (IsSbA13Plus ())) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD2, AccWidthUint8, ~ (BIT3), BIT3);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD2, AccWidthUint8, ~ (BIT3), 0);
  }
  TRACE ((DMSG_SB_TRACE, "CIMx - Exiting commonInitEarlyBoot \n"));
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
  abValue = readAlink (SB_ABCFG_REGC0 | (UINT32) (ABCFG << 29));
  abValue &= 0xf0;
  if ( pConfig->SbPcieOrderRule && abValue ) {
    abValue = readAlink (SB_RCINDXC_REG02 | (UINT32) (RCINDXC << 29));
    abValue = abValue | BIT9;
    writeAlink (SB_RCINDXC_REG02 | (UINT32) (RCINDXC << 29), abValue);
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
  UINT8  Data;
  UINT32  Data32;
  UINT8  cimSpreadSpectrum;
  UINT8  maxSpreadEntry;
  AMDSBCFG*   pTmp;
  pTmp = pConfig;

  maxSpreadEntry = (sizeof SpreadParameterTable) / (sizeof (SB_SPREAD_SPECTRUM_ENTRY));

  cimSpreadSpectrum = pConfig->SpreadSpectrum;
#if  SB_CIMx_PARAMETER == 0
  cimSpreadSpectrum = cimSpreadSpectrumDefault;
#endif
  programSbAcpiMmioTbl ((AcpiRegWrite*) FIXUP_PTR (&sbPmioEPostInitTable[0]));

  // Turn on and configure LPC clock (48MHz)
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x28, AccWidthUint32, ~(BIT21 + BIT20 + BIT19), 2 << 19);
  RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG40, AccWidthUint8, ~BIT7, 0);

  if ( cimSpreadSpectrum ) {
    if ( IsSbA11 () ) {
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
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x12, AccWidthUint8, 0, 0x99);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x11, AccWidthUint8, 0, 0x75);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccWidthUint8, 0, 0xBE);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x0B, AccWidthUint8, 0, 0x91);
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x09, AccWidthUint8, 0, 0x21);
    }
    if ( cimSpreadSpectrum >= maxSpreadEntry ) {
      cimSpreadSpectrum = 1;
    }
    if ( IsSbA12Plus () ) {

      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x40, AccWidthUint32, (UINT32) (~(0x1 << 25)), ( SpreadParameterTable[cimSpreadSpectrum].P_40_25 << 25));

      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccWidthUint32, (UINT32) (~(0x1 << 0)), (0x0 << 0));

      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x18, AccWidthUint32, (UINT32) (~(0x7FF << 5)), (SpreadParameterTable[cimSpreadSpectrum].P_18_15_5 << 5));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x18, AccWidthUint32, (UINT32) (~(0xF << 16)), (SpreadParameterTable[cimSpreadSpectrum].P_18_19_16 << 16));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccWidthUint32, (UINT32) (~(0xFFFF << 8)), (SpreadParameterTable[cimSpreadSpectrum].P_10_23_8 << 8));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccWidthUint32, (UINT32) (~(0xFF << 0)), (SpreadParameterTable[cimSpreadSpectrum].P_10_7_0 << 0));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x1C, AccWidthUint32, (UINT32) (~(0x3F << 0)), (SpreadParameterTable[cimSpreadSpectrum].P_1C_5_0 << 0));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccWidthUint32, (UINT32) (~(0xF << 28)), (SpreadParameterTable[cimSpreadSpectrum].P_08_31_28 << 28));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccWidthUint32, (UINT32) (~(0x1 << 7)), (SpreadParameterTable[cimSpreadSpectrum].P_08_7 << 7));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x08, AccWidthUint32, (UINT32) (~(0x1 << 8)), (SpreadParameterTable[cimSpreadSpectrum].P_08_8 << 8));
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + 0x10, AccWidthUint32, (UINT32) (~(0x3 << 24)), (SpreadParameterTable[cimSpreadSpectrum].P_10_25_24 << 24));
    }
    if ( IsExternalClockMode () ) {
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG08, AccWidthUint8, 0xFE, 0x00);
    } else {
      RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG08, AccWidthUint8, 0xFE, 0x01);
    }
  } else {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE + SB_MISC_REG08, AccWidthUint8, 0xFE, 0x00);
  }

  // SD Configuration
  if ( pConfig->sdConfig ) {
    //OBS263741 TTP1000D: SD Host Controller can't be enabled after disabling it in BIOS setup.
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE +  SB_PMIOA_REGD3, AccWidthUint8, 0xBF, 0x40);
    RWMEM (ACPI_MMIO_BASE + MISC_BASE +  SB_MISC_REG41, AccWidthUint8, 0xF1, 0x48);
    RWMEM (ACPI_MMIO_BASE + MISC_BASE +  SB_MISC_REG42, AccWidthUint8, 0xFE, 0x00);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE +  SB_PMIOA_REGE7, AccWidthUint8, 0x00, 0x12);
    // INT#A SD resource
    Data = 0x97; // Azalia APIC index
    WriteIO (SB_IOMAP_REGC00, AccWidthUint8, &Data);
    Data = 0x10; // IRQ16 (INTA#)
    WriteIO (SB_IOMAP_REGC01, AccWidthUint8, &Data);

    ReadPCI ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccWidthUint32, &Data32);
    Data32 |= BIT31 + BIT24 + BIT18 + BIT16; //ADMA
    if ( pConfig->sdConfig == 2) {
      Data32 &=  ~(BIT16 + BIT24); //DMA
    } else if ( pConfig->sdConfig == 3) {
      Data32 &=  ~(BIT16 + BIT18 + BIT24); //PIO
    }
    Data32 &= ~(BIT17 + BIT23); //clear bitwidth
    Data32 |= (pConfig->sdSpeed << 17) + (pConfig->sdBitwidth << 23);
    RWPCI ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGA4, AccWidthUint32 | S3_SAVE, 0, Data32);
    //SB02544: SD: Some SD cards cannot be detected in HIGH speed mode
    if ( IsSbA12Plus () ) {
      RWPCI ((SD_BUS_DEV_FUN << 16) + SD_PCI_REGB0, AccWidthUint32 | S3_SAVE, (UINT32) (~ (0x03 << 10)), (UINT32) (0x03 << 10));
    }
    //BUG260949  There isn't code that set SSID of SD Controller in Hudson CIMX
    ReadPCI ((SD_BUS_DEV_FUN << 16) + SD_PCI_REG00, AccWidthUint32, &Data32);
    RWPCI ((SD_BUS_DEV_FUN << 16) + SD_PCI_REG2C, AccWidthUint32 | S3_SAVE, 0, Data32);
  } else {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE +  SB_PMIOA_REGD3, AccWidthUint8, 0xBF, 0x00);
  }

  // RPR PLL 100Mhz Reference Clock Buffer setting for internal clock generator mode (BIT5)
  // RPR OSC Clock setting for  internal clock generator mode (BIT6)
  getChipSysMode (&dbPortStatus);
  if ( ((dbPortStatus & ChipSysIntClkGen) == ChipSysIntClkGen) ) {
    RWMEM (ACPI_MMIO_BASE + MISC_BASE +  SB_MISC_REG04 + 1, AccWidthUint8, ~(BIT5 + BIT6), BIT5 + BIT6);
  }

  // Set ASF SMBUS master function enabled here (temporary)
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG28, AccWidthUint16 | S3_SAVE, ~(BIT0 + BIT2), BIT0 + BIT2);

  programSbAcpiMmioTbl ((AcpiRegWrite *) (pConfig->OEMPROGTBL.OemProgrammingTablePtr_Ptr));

#ifndef NO_EC_SUPPORT
  // Software IMC enable
  if (((pConfig->BuildParameters.ImcEnableOverWrite == 1) && ((dbPortStatus & ChipSysEcEnable) == 0)) || ((pConfig->BuildParameters.ImcEnableOverWrite == 2) && ((dbPortStatus & ChipSysEcEnable) == ChipSysEcEnable))) {
    if (validateImcFirmware (pConfig)) {
      softwareToggleImcStrapping (pConfig);
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
  AMDSBCFG* Temp;

  cimResetCpuOnSyncFlood = pConfig->ResetCpuOnSyncFlood;
#if  SB_CIMx_PARAMETER == 0
  cimResetCpuOnSyncFlood = cimResetCpuOnSyncFloodDefault;
#endif
  Temp = pConfig;
  if ( pConfig->SbPcieOrderRule == 1 ) {
    pAbTblPtr = (ABTBLENTRY *) FIXUP_PTR (&SbPcieOrderRule[0]);
    abcfgTbl (pAbTblPtr);
  }
  if ( pConfig->SbPcieOrderRule == 2 ) {
    rwAlink (SB_ABCFG_REG10090 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x7 << 10), (UINT32) (0x7 << 10));
    rwAlink (SB_ABCFG_REG58 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1F << 11), (UINT32) (0x1C << 11));
    rwAlink (SB_ABCFG_REGB4 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x3 << 0), (UINT32) (0x3 << 0));
  }
  pAbTblPtr = (ABTBLENTRY *) FIXUP_PTR (&abTblEntry800[0]);
  abcfgTbl (pAbTblPtr);
  if ( cimResetCpuOnSyncFlood ) {
    rwAlink (SB_ABCFG_REG10050 | (UINT32) (ABCFG << 29), ~BIT2, BIT2);
  }

  if ( pConfig->AbClockGating ) {
    rwAlink (SB_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
    rwAlink (SB_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x1 << 24));
    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x1 << 24));
  } else {
    rwAlink (SB_ABCFG_REG10054 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x0 << 24));
    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 24), (UINT32) (0x0 << 24));
  }


  if ( pConfig->GppClockGating ) {
    rwAlink (SB_ABCFG_REG98 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xF << 12), (UINT32) (0x4 << 12));
    rwAlink (SB_ABCFG_REG98 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xF << 8), (UINT32) (0x7 << 8));
    rwAlink (SB_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x1 << 0));
  } else {
    rwAlink (SB_ABCFG_REG98 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xF << 8), (UINT32) (0x0 << 8));
    rwAlink (SB_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 0), (UINT32) (0x0 << 0));
  }

  if ( pConfig->L1TimerOverwrite ) {
    rwAlink (SB_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x7 << 12), (UINT32) (pConfig->L1TimerOverwrite  << 12));
    rwAlink (SB_ABCFG_REG90 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 15), (UINT32) (0x1  << 15));
  }

  if ( pConfig->UmiLinkWidth ) {
//    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }
  if ( pConfig->UmiDynamicSpeedChange ) {
    rwAlink ((UINT32) SB_AX_INDXP_REGA4, ~ (UINT32) (0x1 << 0), (UINT32) (0x1 << 0));
    rwAlink ((UINT32) SB_AX_CFG_REG88, ~ (UINT32) (0xF << 0), (UINT32) (0x2 << 0));
    rwAlink ((UINT32) SB_AX_INDXP_REGA4, ~ (UINT32) (0x1 << 18), (UINT32) (0x1 << 18));
  }
  if ( pConfig->PcieRefClockOverclocking ) {
//    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }
  if ( pConfig->SbAlinkGppTxDriverStrength  ) {
    rwAlink (SB_ABCFG_REGA8 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x3 << 18), (UINT32) ((pConfig->SbAlinkGppTxDriverStrength - 1) << 18));
    rwAlink (SB_ABCFG_REGA0 | (UINT32) (ABCFG << 29), ~ (UINT32) (0x1 << 8), (UINT32) (0x1 << 8));
  }
  if ( pConfig->PcieAER ) {
//    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
  }
  if ( pConfig->PcieRAS ) {
//    rwAlink (SB_ABCFG_REG54 | (UINT32) (ABCFG << 29), ~ (UINT32) (0xFF << 16), (UINT32) (0x4 << 16));
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
    TRACE ((DMSG_SB_TRACE, "RegType: %X, RegNumber: %X, AndMask = %X, OrMask = %X \n", pABTbl->regType, pABTbl->regIndex, pABTbl->regMask, pABTbl->regData));
    if ( pABTbl->regType == AXINDC ) {
      ddValue = 0x30 | (pABTbl->regType << 29);
      writeAlink (ddValue, (pABTbl->regIndex & 0x00FFFFFF));
      ddValue = 0x34 | (pABTbl->regType << 29);
      writeAlink (ddValue, ((readAlink (ddValue)) & (0xFFFFFFFF^ (pABTbl->regMask))) | pABTbl->regData);
    } else if ( pABTbl->regType == AXINDP ) {
      ddValue = 0x38 | (pABTbl->regType << 29);
      writeAlink (ddValue, (pABTbl->regIndex & 0x00FFFFFF));
      ddValue = 0x3C | (pABTbl->regType << 29);
      writeAlink (ddValue, ((readAlink (ddValue)) & (0xFFFFFFFF^ (pABTbl->regMask))) | pABTbl->regData);
    } else {
      ddValue = pABTbl->regIndex | (pABTbl->regType << 29);
      writeAlink (ddValue, ((readAlink (ddValue)) & (0xFFFFFFFF^ (pABTbl->regMask))) | pABTbl->regData);
    }
    ++pABTbl;
  }

  //Clear ALink Access Index
  ddValue = 0;
  WriteIO (ALINK_ACCESS_INDEX, AccWidthUint32 | S3_SAVE, &ddValue);
  TRACE ((DMSG_SB_TRACE, "Exiting abcfgTbl\n"));
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
  UINT8  NStBit;
  UINT8  NSBit;
  UINT8  indexValue;

  // We need to do the following setting in late post also because some bios core pci enumeration changes these values
  // programmed during early post.
  // RPR 4.5 Master Latency Timer

  dbValue = 0x40;
  WritePCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG0D, AccWidthUint8, &dbValue);
  WritePCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG1B, AccWidthUint8, &dbValue);

  //RPR 6.4 CLKRUN#
  // SB P2P AutoClock control settings.
  // ddVar = (pConfig->PcibAutoClkCtrlLow) | (pConfig->PcibAutoClkCtrlLow);
  if ( pConfig->ClockRun ) {
    ReadMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG54, AccWidthUint8, &dbValue);
    NStBit = dbValue & 0x03;
    NSBit = (dbValue & 0x3F ) >> 2;
    ddVar = (4 + (NStBit * 2) + (( 17 + NSBit) * 3) + 4) | 0x01;
    if ( IsSbA12Plus () ) {
      ddVar = 9; //4 clocks
    }
    WritePCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG4C, AccWidthUint32, &ddVar);
  }



  ddVar = (pConfig->PcibClkStopOverride);
  RWPCI ((PCIB_BUS_DEV_FUN << 16) + SB_PCIB_REG50, AccWidthUint16, 0x3F, (UINT16) (ddVar << 6));

  RWPCI ((LPC_BUS_DEV_FUN << 16) + SB_LPC_REGBB, AccWidthUint8, 0xBF | S3_SAVE, BIT3 + BIT4 + BIT5);

  if ( IsGCPU () ) {
    GcpuRelatedSetting (pConfig);
  } else {
    c3PopupSetting (pConfig);
  }
  //[RPR 2.12] Mt C1E Enable
  MtC1eEnable (pConfig);

  RWPCI ((0xC1 << 16) + 0xBC, AccWidthUint8, 0x7f, 0x80);

  //if (pConfig->Sdb == 1 ) {
    //RWMEM (ACPI_MMIO_BASE + SERIAL_DEBUG_BASE +  SB_SDB_REG00, AccWidthUint8, 0xFF, 0x05);
  //}

  if ( pConfig->XhciSwitch == 1 ) {
    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x10, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_BAR00;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x11, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_BAR01;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x12, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_BAR02;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x13, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_BAR03;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x04, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_04H;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x0C, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_0CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI_BUS_DEV_FUN << 16) + 0x3C, AccWidthUint8, &dbValue);
    indexValue = XHCI_REGISTER_3CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x10, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_BAR00;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x11, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_BAR01;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x12, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_BAR02;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x13, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_BAR03;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x04, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_04H;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x0C, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_0CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);

    ReadPCI ((USB_XHCI1_BUS_DEV_FUN << 16) + 0x3C, AccWidthUint8, &dbValue);
    indexValue = XHCI1_REGISTER_3CH;
    WriteIO (SB_IOMAP_REGCD4, AccWidthUint8, &indexValue);
    WriteIO (SB_IOMAP_REGCD5, AccWidthUint8, &dbValue);
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
#ifdef SB_TIMER_TICK_INTERVAL_WA
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFF800, BIT0 + BIT1);
#endif
    } else {
      RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG50, AccWidthUint32 | S3_SAVE, 0xFFFFF800, BIT0 + BIT1);
    }

  } else {
    if ( ! (pConfig->S3Resume) ) {
      pHpetTable = (DESCRIPTION_HEADER*) ACPI_LocateTable (Int32FromChar('T', 'E', 'P', 'H'));
    }
    if ( pHpetTable != NULL ) {
      pHpetTable->Signature = Int32FromChar('H', 'P', 'E', 'T');
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
  AMDSBCFG* Temp;
  UINT8  dbValue;
  Temp = pConfig;
  dbValue = getNumberOfCpuCores  ();
#define NON_SUPPORT_PREVIOUS_C3 TRUE
#ifndef NON_SUPPORT_PREVIOUS_C3
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
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88, AccWidthUint8 | S3_SAVE, ~(BIT4 + BIT5), BIT4 + BIT5);

#else
  //RPR2.4 C-State and VID/FID Change
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88, AccWidthUint8 | S3_SAVE, ~(BIT5), BIT5);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT2), BIT2);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT1), BIT1);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7E, AccWidthUint8 | S3_SAVE, ~(BIT6), BIT6);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG94, AccWidthUint8 | S3_SAVE, 0, 0x01);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG89, AccWidthUint8 | S3_SAVE, ~BIT4, BIT4);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG88, AccWidthUint8 | S3_SAVE, ~BIT4, BIT4);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG9B, AccWidthUint8 | S3_SAVE, ~(BIT6 + BIT5 + BIT4), BIT4);

  //RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT10), BIT10);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG9B, AccWidthUint8 | S3_SAVE, ~(BIT1 + BIT0), 0);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG96, AccWidthUint8 | S3_SAVE, 0, 0x10);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG99, AccWidthUint8 | S3_SAVE, 0, 0x10);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG8E, AccWidthUint8 | S3_SAVE, 0, 0x80);
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG97, AccWidthUint8 | S3_SAVE, ~(BIT1 + BIT0), 0);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT4), BIT4);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT9), BIT9);

  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~(BIT7), 0);

#endif
}

/**
 * GcpuRelatedSetting - Program GCPU C related function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID
GcpuRelatedSetting (
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
  if ( cimFusionMsgCMultiCore ) {
    ddValue = (ddValue | (BIT6 + BIT4 + BIT3 + BIT2));
  }

  WriteMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGA0, AccWidthUint32 | S3_SAVE, &ddValue);
}

/**
 * MtC1eEnable - Program Mt C1E Enable Function
 *
 *
 *
 * @param[in] pConfig   Southbridge configuration structure pointer.
 *
 */
VOID
MtC1eEnable (
  IN       AMDSBCFG* pConfig
  )
{
  if ( pConfig->MtC1eEnable ) {
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7A, AccWidthUint16 | S3_SAVE, ~ BIT15, BIT15);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG7A, AccWidthUint16 | S3_SAVE, ~ (BIT3 + BIT2 + BIT1 + BIT0), 0x01);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~ BIT13, BIT13);
    RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG80, AccWidthUint16 | S3_SAVE, ~ BIT7, BIT7);
  }
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
  abValue1 = abValue & (~ (BIT4 + BIT6 + BIT17 + BIT12 + BIT15 + BIT16 + BIT18));
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
  dbValue |= (BIT6 + BIT7);                                             // PwrGoodOut =1, PwrGoodEnB=1
  WritePMIO (SB_PMIOA_REGBF, AccWidthUint8, &dbValue);

  dbValue = 06;
  WriteIO (0xcf9, AccWidthUint8, &dbValue);
  SbStall (0xffffffff);
}
#endif

/**
 * A13ResumeResetTwoSecondRtcWakeup - A13 Resume Reset 2 Seconds
 * RTC Wakeup
 *
 *
 *
 * @retval  Nothing
 *
 */
VOID
A13ResumeResetTwoSecondRtcWakeup (
  void
  )
{
  if ( IsSbA13Plus () ) {
    //Configure RTC clocks and power failure to "off"
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG57) |= 0x03;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG56) |= 0x80;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG56) &= 0x7f;
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REG5B) = 0x04;

    //set 2 seconds RTC wake up
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x00) = 0;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x01) = 2;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x02) = 0;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x03) = 0;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x04) = 0;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x05) = 0;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x0d) = 0x80;
    ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_BASE + 0x0b) |= 0x22;

    //Do ResumeReset by SB A13 ECO
    WriteIo8 (0x80, 0xef);
    ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD6) |= 0x40;
    do {
      WriteIo8 (0x80, 0xfe);
    } while ( ACPIMMIO8 (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGD6) );
  }
}

/**
 * StressResetModeLate - Stress Reset Mode
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
StressResetModeLate (
  IN       AMDSBCFG* pConfig
  )
{
  switch ( pConfig->StressResetMode ) {
  case 1:
    WriteIo8 ((UINT16) (0x64), 0xFE);
    break;
  case 2:
    WriteIo8 ((UINT16) (0xCF9), 0x06);
    break;
  case 3:
    WriteIo8 ((UINT16) (0xCF9), 0x0E);
    break;
  case 4:
    A13ResumeResetTwoSecondRtcWakeup ();
    return;
  default:
    return;
  }
  while (pConfig->StressResetMode) {
  }
}

/**
 * CheckEfuse - Check Efuse
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
CheckEfuse (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8     EfuseIndex;
  UINT8     EfuseFailureCount;
  pConfig->EfuseRemainder = 0;
  pConfig->EfuseSum = 0;
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, ~BIT5, BIT5);
  for ( EfuseIndex = 0; EfuseIndex < 0x20; EfuseIndex ++ ) {
    pConfig->EfuseByte [EfuseIndex] = getEfuseByte (EfuseIndex);
  }
  RWMEM (ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGC8, AccWidthUint8, ~BIT5, 0);
  for ( EfuseIndex = 0x10; EfuseIndex < 0x20; EfuseIndex ++ ) {
    pConfig->EfuseSum = pConfig->EfuseSum + pConfig->EfuseByte [EfuseIndex];
    pConfig->EfuseRemainder = (((pConfig->EfuseRemainder) << 8) + pConfig->EfuseByte [0x2f - EfuseIndex]) % 0xc1;
  }
  pConfig->EfuseSum = (UINT8) (0x100 - pConfig->EfuseSum);
  if (!(( pConfig->EfuseByte [0x0e] == 0 ) && ( pConfig->EfuseByte [0x0f] == 0 ))) {
    if (( pConfig->EfuseRemainder != pConfig->EfuseByte [0x0e] ) || ( pConfig->EfuseSum != pConfig->EfuseByte [0x0f])) {
      EfuseFailureCount = ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D);
      if ( EfuseFailureCount == 0xff ) {
        EfuseFailureCount = 0;
        ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D) = 0;
      }
      if ( EfuseFailureCount < 2 ) {
        EfuseFailureCount++;
        ACPIMMIO8 (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0D) = EfuseFailureCount;
        A13ResumeResetTwoSecondRtcWakeup ();
      }
    }
  }
}

/**
 * ValidateFchVariant - Validate FCH Variant
 *
 *
 *
 * @param[in] pConfig Southbridge configuration structure pointer.
 *
 */
VOID
ValidateFchVariant (
  IN       AMDSBCFG* pConfig
  )
{
  UINT8     XhciEfuse;
  UINT8     PcieEfuse;
  UINT8     FchVariantEfuse;

  switch ( pConfig->FchVariant ) {
  case FCH_M3T:
    //Disable Devices for M3T
    pConfig->GecConfig = 1;
    pConfig->hwm.hwmEnable = 0;
    pConfig->sdConfig = 0;
    pConfig->IrConfig = 0;
//    pConfig->USBMODE.UsbMode.Ohci3 = 0;
//    pConfig->USBMODE.UsbMode.Ehci3 = 0;
    break;
  default:
    break;
  }

  // add Efuse checking for Xhci enable/disable
  XhciEfuse = XHCI_EFUSE_LOCATION;
  getEfuseStatus (&XhciEfuse);
  if ((XhciEfuse & (BIT0 + BIT1)) == (BIT0 + BIT1)) {
    pConfig->XhciSwitch = 0;
  }

  // add Efuse checking for PCIE Gen2 enable
  PcieEfuse = PCIE_FORCE_GEN1_EFUSE_LOCATION;
  getEfuseStatus (&PcieEfuse);
  if ( PcieEfuse & BIT0 ) {
    pConfig->NbSbGen2 = 0;
    pConfig->GppGen2 = 0;
  }

  FchVariantEfuse = FCH_Variant_EFUSE_LOCATION;
  getEfuseStatus (&FchVariantEfuse);
  if ((FchVariantEfuse == 0x07) || (FchVariantEfuse == 0x08)) {
    pConfig->NbSbGen2 = 0;
  }
}

/**
 * Is GCPU?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsGCPU (
  OUT VOID
  )
{
  UINT8 ExtendedFamily;
  UINT8 ExtendedModel;
  UINT8 BaseFamily;
  UINT8 BaseModel;
  UINT8 Stepping;
  UINT8 Family;
  UINT8 Model;
  SB_CPUID_DATA  CpuId;

  CpuidRead (0x01, &CpuId);
  ExtendedFamily = (UINT8) ((CpuId.EAX_Reg >> 20) & 0xff);
  ExtendedModel = (UINT8) ((CpuId.EAX_Reg >> 16) & 0xf);
  BaseFamily = (UINT8) ((CpuId.EAX_Reg >> 8) & 0xf);
  BaseModel = (UINT8) ((CpuId.EAX_Reg >> 4) & 0xf);
  Stepping = (UINT8) ((CpuId.EAX_Reg >> 0) & 0xf);
  Family = BaseFamily + ExtendedFamily;
  Model = (ExtendedModel >> 4) + BaseModel;
  if ( (Family == 0x12) || \
       (Family == 0x14) || \
       (Family == 0x16) || \
       ((Family == 0x15) && ((Model == 0x10) || (Model == 0x30))) ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 * Is UMI One Lane GEN1 Mode?
 *
 *
 * @retval  TRUE or FALSE
 *
 */
BOOLEAN
IsUmiOneLaneGen1Mode (
  OUT VOID
  )
{
  UINT32   abValue;
  abValue = readAlink ((UINT32) (SB_AX_CFG_REG68));
  abValue >>= 16;
  if (((abValue & 0x0f) == 1) && ((abValue & 0x03f0) == 0x0010)) {
    return (TRUE);
  } else {
    return (FALSE);
  }
}

/**
 * Record SMI Status
 *
 *
 * @retval  Nothing
 *
 */
VOID
RecordSmiStatus (
  OUT VOID
  )
{
  UINTN   i;
  UINT8   SwSmiValue;
  ACPIMMIO8 (0xfed80320) |= 0x01;
  for ( i = 0; i < 20; i++ ) {
    ACPIMMIO8 (0xfed10020 + i) = ACPIMMIO8 (0xfed80280 + i);
  }
  SwSmiValue = ReadIo8 (0xb0);
  ACPIMMIO8 (0xfed10040) = SwSmiValue;
}

