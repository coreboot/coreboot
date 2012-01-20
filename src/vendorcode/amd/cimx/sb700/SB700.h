/*;********************************************************************************
;
; Copyright (C) 2012 Advanced Micro Devices, Inc.
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

#ifndef _AMD_SB700_H_
#define _AMD_SB700_H_

#pragma pack(push,1)

#define CIMx_Version                    0x0660
#define RC_Information                  0x00
#define Additional_Changes_Indicator    0x00

#define SB_POWERON_INIT                 0x001
#define OUTDEBUG_PORT                   0x002
#define SB_BEFORE_PCI_INIT              0x010
#define SB_AFTER_PCI_INIT               0x020
#define SB_LATE_POST_INIT               0x030
#define SB_BEFORE_PCI_RESTORE_INIT      0x040
#define SB_AFTER_PCI_RESTORE_INIT       0x050
#define SB_SMM_SERVICE                  0x060
#define SB_SMM_ACPION                   0x061

#ifndef OEM_CALLBACK_BASE
  #define       OEM_CALLBACK_BASE       0x100
#endif

//0x00 - 0x0F callback functions are reserved for bootblock
#define SATA_PHY_PROGRAMMING            OEM_CALLBACK_BASE + 0x10
#define PULL_UP_PULL_DOWN_SETTINGS      OEM_CALLBACK_BASE + 0x20

#define CFG_ADDR_PORT                   0xCF8
#define CFG_DATA_PORT                   0xCFC
#define ATI_AZALIA_ExtBlk_Addr          0x0F8
#define ATI_AZALIA_ExtBlk_DATA          0x0FC

#define ALINK_ACCESS_INDEX              0x0CD8
#define ALINK_ACCESS_DATA               ALINK_ACCESS_INDEX + 4

/*------------------------------------------------------------------
; I/O Base Address - Should be set by host BIOS
;------------------------------------------------------------------ */
#define DELAY_PORT                      0x0E0

/*------------------------------------------------------------------
; DEBUG_PORT = 8-bit I/O Port Address for POST Code Display
;------------------------------------------------------------------ */
#define SB7XX_DEVICE_ID                 0x4385

#define SB700_A11                       0x39
#define SB700_A12                       0x3A
#define SB700_A13                       0x3B
#define SB700_A14                       0x3C
#define SB700_A15                       0x3D

#define SATA_BUS_DEV_FUN                ((0x11 << 3) + 0)
#define FC_BUS_DEV_FUN                  ((0x11 << 3) + 1)
#define USB1_OHCI0_BUS_DEV_FUN          ((0x12 << 3) + 0)
#define USB1_OHCI1_BUS_DEV_FUN          ((0x12 << 3) + 1)
#define USB2_OHCI0_BUS_DEV_FUN          ((0x13 << 3) + 0)
#define USB2_OHCI1_BUS_DEV_FUN          ((0x13 << 3) + 1)
#define USB3_OHCI_BUS_DEV_FUN           ((0x14 << 3) + 5)
#define USB1_EHCI_BUS_DEV_FUN           ((0x12 << 3) + 2)
#define USB2_EHCI_BUS_DEV_FUN           ((0x13 << 3) + 2)

#define SMBUS_BUS_DEV_FUN               ((0x14 << 3) + 0)
#define IDE_BUS_DEV_FUN                 ((0x14 << 3) + 1)
#define AZALIA_BUS_DEV_FUN              ((0x14 << 3) + 2)
#define LPC_BUS_DEV_FUN                 ((0x14 << 3) + 3)
#define SBP2P_BUS_DEV_FUN               ((0x14 << 3) + 4)
#define NB_BDF                          ((0 << 3) + 0)
#define HT_LINK_BUS_DEV_FUN       ((0x18 << 3) + 0)
#define DCT1_BUS_DEV_FUN          ((0x18 << 3) + 2)
#define DCT2_BUS_DEV_FUN          ((0x19 << 3) + 2)
#define DCT3_BUS_DEV_FUN          ((0x1A << 3) + 2)
#define DCT4_BUS_DEV_FUN          ((0x1B << 3) + 2)


//Sata Controller Mode
#define NATIVE_IDE_MODE                 0
#define RAID_MODE                       1
#define AHCI_MODE                       2
#define LEGACY_IDE_MODE                 3
#define IDE_TO_AHCI_MODE                4
#define AMD_AHCI_MODE                   5
#define IDE_TO_AMD_AHCI_MODE            6

//Sata Port Configuration
#define SIX_PORTS                       0
#define FOUR_PORTS                      1

#define SB750_SATA_DEFAULT_DEVICE_ID    0x4393

#define SB_AX_INDXC_REG30               0x30
#define SB_AX_DATAC_REG34               0x34
#define SB_AX_INDXP_REG38               0x38
#define SB_AX_DATAP_REG3C               0x3C

#define AX_INDXC                        0
#define AX_INDXP                        1
#define AXCFG                           2
#define ABCFG                           3

#define SB_AB_REG02                     0x02
#define SB_AB_REG04                     0x04
#define SB_AB_REG40                     0x40            //
#define SB_AB_REG54                     0x54            //;miscCtr54
#define SB_AB_REG58                     0x58            //;RAB Control - RW - 32 bits - [RegAddr:58]
#define SB_AB_REG60                     0x60            //;DMA Prefetch Enable Port 0 - RW - 32 bits - [RegAddr:60]
#define SB_AB_REG64                     0x64            //;DMA Prefetch Flush Port 0 - RW - 32 bits - [RegAddr:64]
#define SB_AB_REG6C                     0x6C            //;DMA Prefetch Flush Port 0 - RW - 32 bits - [RegAddr:6C]
#define SB_AB_REG80                     0x80            //;DMA Prefetch Control Port 1 - RW - 32 bits - [RegAddr:80]
#define SB_AB_REG88                     0x88            //;DMA Prefetch Control Port 2 - RW - 32 bits - [RegAddr:88]
#define SB_AB_REG8C                     0x8C            //;AB Enhancement - RW - 16 bits - [RegAddr:88]
#define SB_AB_REG90                     0x90            //;BIF Control - RW - 32 bits - [RegAddr:90]
#define SB_AB_REG94                     0x94            //;MSI Control - RW - 32 bits
#define SB_AB_REG98                     0x98            //;BIF Control 1 - RW - 32 bits
#define SB_AB_REG9C                     0x9C            //;
#define SB_AB_REG10050                  BIT16+0x50
#define SB_AB_REG10054                  BIT16+0x54      //;AL_Arb_Ctl, AL_Clk_Ctl
#define SB_AB_REG10060                  BIT16+0x60      //;DMA Prefetch Enable Port 0 - RW - 32 bits - [RegAddr:10060]
#define SB_AB_REG10064                  BIT16+0x64      //;DMA Prefetch Flush Port 0 - RW - 32 bits - [RegAddr:64]
#define SB_AB_REG10090                  BIT16+0x90      //;
#define SB_AB_REG1009C                  BIT16+0x9C              //;


#define SB_PMIO_REG00                   0x000   // MiscControl
#define SB_PMIO_REG01                   0x001   // MiscStatus
#define SB_PMIO_REG02                   0x002   // SmiWakeUpEventEnable1
#define SB_PMIO_REG03                   0x003   // SmiWakeUpEventEnable2
#define SB_PMIO_REG04                   0x004   // SmiWakeUpEventEnable3
#define SB_PMIO_REG05                   0x005   // SmiWakeUpEventStatus1
#define SB_PMIO_REG06                   0x006   // SmiWakeUpEventStatus2
#define SB_PMIO_REG07                   0x007   // SmiWakeUpEventStatus3
#define SB_PMIO_REG08                   0x008   // InactiveTmrEventEnable1
#define SB_PMIO_REG09                   0x009   // InactiveTmrEventEnable2
#define SB_PMIO_REG0A                   0x00A   // InactiveTmrEventEnable3
#define SB_PMIO_REG0B                   0x00B   // PmTmr1InitValue
#define SB_PMIO_REG0C                   0x00C   // PmTmr1CurValue
#define SB_PMIO_REG0D                   0x00D   // PwrLedExtEvent
#define SB_PMIO_REG0E                   0x00E   // AcpiControl
#define SB_PMIO_REG0F                   0x00F   // AcpiStatus
#define SB_PMIO_REG10                   0x010   // AcpiEn
#define SB_PMIO_REG11                   0x011   // S1AgpStpEn
#define SB_PMIO_REG12                   0x012   // PmTmr2InitValue
#define SB_PMIO_REG13                   0x013   // PmTmr2CurValue
#define SB_PMIO_REG14                   0x014   // Programlo0RangeLo
#define SB_PMIO_REG15                   0x015   // ProgramIo0Rangei
#define SB_PMIO_REG16                   0x016   // ProgramIo1RangeLo
#define SB_PMIO_REG17                   0x017   // ProgramIo1Rangei
#define SB_PMIO_REG18                   0x018   // ProgramIo2RangeLo
#define SB_PMIO_REG19                   0x019   // ProgramIo2Rangei
#define SB_PMIO_REG1A                   0x01A   // ProgramIo3RangeLo
#define SB_PMIO_REG1B                   0x01B   // ProgramIo3Rangei
#define SB_PMIO_REG1C                   0x01C   // ProgramIoEnable
#define SB_PMIO_REG1D                   0x01D   // IOMonitorStatus
#define SB_PMIO_REG1E                   0x01E   // InactiveTmrEventEnable4
#define SB_PMIO_REG20                   0x020   // AcpiPm1EvtBlkLo
#define SB_PMIO_REG21                   0x021   // AcpiPm1EvtBlki
#define SB_PMIO_REG22                   0x022   // AcpiPm1CntBlkLo
#define SB_PMIO_REG23                   0x023   // AcpiPm1CntBlki
#define SB_PMIO_REG24                   0x024   // AcpiPmTmrBlkLo
#define SB_PMIO_REG25                   0x025   // AcpiPmTmrBlki
#define SB_PMIO_REG26                   0x026   // CpuControlLo
#define SB_PMIO_REG27                   0x027   // CpuControli
#define SB_PMIO_REG28                   0x028   // AcpiGpe0BlkLo
#define SB_PMIO_REG29                   0x029   // AcpiGpe0Blki
#define SB_PMIO_REG2A                   0x02A   // AcpiSmiCmdLo
#define SB_PMIO_REG2B                   0x02B   // AcpiSmiCmdi
#define SB_PMIO_REG2C                   0x02C   // AcpiPmaCntBlkLo
#define SB_PMIO_REG2D                   0x02D   // AcpiPmaCntBlki
#define SB_PMIO_REG2E                   0x02E   // AcpiSsCntBlkLo
#define SB_PMIO_REG2F                   0x02F   // AcpiSsCntBlki
#define SB_PMIO_REG30                   0x030   // GEvtConfig0
#define SB_PMIO_REG31                   0x031   // GEvtConfig1
#define SB_PMIO_REG32                   0x032   // GPMConfig0
#define SB_PMIO_REG33                   0x033   // GPMConfig1
#define SB_PMIO_REG34                   0x034   // GPMConfig2
#define SB_PMIO_REG35                   0x035   // GPMConfig3
#define SB_PMIO_REG36                   0x036   // GEvtLevelConfig
#define SB_PMIO_REG37                   0x037   // GPMLevelConfig0
#define SB_PMIO_REG38                   0x038   // GPMLevelConfig1
#define SB_PMIO_REG39                   0x039   // GEvtStatus
#define SB_PMIO_REG3A                   0x03A   // PMEStatus0
#define SB_PMIO_REG3B                   0x03B   // PMEStatus1
#define SB_PMIO_REG3C                   0x03C   // OtersConfig
#define SB_PMIO_REG3E                   0x03E   // VRT_T1
#define SB_PMIO_REG3F                   0x03F   // VRT_T2
#define SB_PMIO_REG40                   0x040   // Fan0DutyCycle
#define SB_PMIO_REG41                   0x041   // Fan0Control
#define SB_PMIO_REG42                   0x042   // Fan1DutyCycle
#define SB_PMIO_REG43                   0x043   // Reserved for internal use
#define SB_PMIO_REG50                   0x050   // PM_Enable
#define SB_PMIO_REG51                   0x051   // TPRESET1
#define SB_PMIO_REG52                   0x052   // TPRESET2
#define SB_PMIO_REG53                   0x053   // TESTENABLE
#define SB_PMIO_REG54                   0x054   // PWRBTTN_CLR
#define SB_PMIO_REG55                   0x055   // SoftPciRst
#define SB_PMIO_REG56                   0x056   // Reserved
#define SB_PMIO_REG59                   0x059   // Ac97Mask
#define SB_PMIO_REG60                   0x060   // Options_0
#define SB_PMIO_REG61                   0x061   // Options_1
#define SB_PMIO_REG62                   0x062   // Sadow_SCI
#define SB_PMIO_REG63                   0x063   // SwitcVoltageTime
#define SB_PMIO_REG64                   0x064   // SwitchGI_Time
#define SB_PMIO_REG65                   0x065   // UsbPMControl
#define SB_PMIO_REG66                   0x066   // MiscEnable66
#define SB_PMIO_REG67                   0x067   // MiscEnable67
#define SB_PMIO_REG68                   0x068   // MiscEnable68
#define SB_PMIO_REG69                   0x069   // WatcDogTimerControl
#define SB_PMIO_REG6C                   0x06C   // WatcDogTimerBase0
#define SB_PMIO_REG6D                   0x06D   // WatcDogTimerBase1
#define SB_PMIO_REG6E                   0x06E   // WatcDogTimerBase2
#define SB_PMIO_REG6F                   0x06F   // WatcDogTimerBase3
#define SB_PMIO_REG70                   0x070   //  S_LdtStartTime
#define SB_PMIO_REG71                   0x071   // FidVidOption
#define SB_PMIO_REG72                   0x072   // Spare4
#define SB_PMIO_REG73                   0x073   // Spare5
#define SB_PMIO_REG74                   0x074   // PwrFailSadow
#define SB_PMIO_REG75                   0x075   // Tpreset1b
#define SB_PMIO_REG76                   0x076   // S0S3ToS5Enable0
#define SB_PMIO_REG77                   0x077   // S0S3ToS5Enable1
#define SB_PMIO_REG78                   0x078   // S0S3ToS5Enable2
#define SB_PMIO_REG79                   0x079   // S0S3ToS5Enable3
#define SB_PMIO_REG7A                   0x07A   // NoStatusControl0
#define SB_PMIO_REG7B                   0x07B   // NoStatusControl1
#define SB_PMIO_REG7C                   0x07C   // MiscEnable7C
#define SB_PMIO_REG80                   0x080   // SMAF0
#define SB_PMIO_REG81                   0x081   // SMAF1
#define SB_PMIO_REG82                   0x082   // SMAF2
#define SB_PMIO_REG83                   0x083   // SMAF3
#define SB_PMIO_REG84                   0x084   // WakePinCntl
#define SB_PMIO_REG85                   0x085   // CF9Rst
#define SB_PMIO_REG86                   0x086   // ThermTrotCntl
#define SB_PMIO_REG87                   0x087   // LdtStpCmd
#define SB_PMIO_REG88                   0x088   // LdtStartTime
#define SB_PMIO_REG89                   0x089   // AgpStartTime
#define SB_PMIO_REG8A                   0x08A   // LdtAgpTimeCntl
#define SB_PMIO_REG8B                   0x08B   // StutterTime
#define SB_PMIO_REG8C                   0x08C   // StpClkDlyTime
#define SB_PMIO_REG8D                   0x08D   // AbPmeCntl
#define SB_PMIO_REG8E                   0x08E   // FakeAsr
#define SB_PMIO_REG8F                   0x08F   // FakeAsrEn
#define SB_PMIO_REG90                   0x090   // GEVENTOUT
#define SB_PMIO_REG91                   0x091   // GEVENTEnable
#define SB_PMIO_REG92                   0x092   // GEVENTIN
#define SB_PMIO_REG95                   0x095   // GPM98EN
#define SB_PMIO_REG9A                   0x09A   // EnanceControl
#define SB_PMIO_REG9E                   0x09E   // EnanceControl
#define SB_PMIO_REG9F                   0x09F   // EnanceControl
#define SB_PMIO_REGA0                   0x0A0   // Programlo4RangeLo
#define SB_PMIO_REGA1                   0x0A1   // ProgramIo4Rangei
#define SB_PMIO_REGA2                   0x0A2   // Programlo5RangeLo
#define SB_PMIO_REGA3                   0x0A3   // ProgramIo5Rangei
#define SB_PMIO_REGA4                   0x0A4   // Programlo6RangeLo
#define SB_PMIO_REGA5                   0x0A5   // ProgramIo6Rangei
#define SB_PMIO_REGA6                   0x0A6   // Programlo7RangeLo
#define SB_PMIO_REGA7                   0x0A7   // ProgramIo7Rangei
#define SB_PMIO_REGA8                   0x0A8   // PIO7654Enable
#define SB_PMIO_REGA9                   0x0A9   // PIO7654Status
#define SB_PMIO_REGB0                   0x0B0
#define SB_PMIO_REGB1                   0x0B1
#define SB_PMIO_REGB2                   0x0B2   // MiscControl3
#define SB_PMIO_REGB4                   0x0B4   // HPET BAR
#define SB_PMIO_REGB6                   0x0B6
#define SB_PMIO_REGB7                   0x0B7
#define SB_PMIO_REGBB                   0x0BB   // IMC_ACPI_Enable
#define SB_PMIO_REGBC                   0x0BC   //
#define SB_PMIO_REGBD                   0x0BD   //
#define SB_PMIO_REGC9                   0x0C9   // MultiK8Control
#define SB_PMIO_REGCA                   0x0CA   //
#define SB_PMIO_REGCB                   0x0CB   //
#define SB_PMIO_REGCC                   0x0CC   //
#define SB_PMIO_REGCD                   0x0CD   //
#define SB_PMIO_REGD0                   0x0D0   //
#define SB_PMIO_REGD2                   0x0D2   //
#define SB_PMIO_REGD4                   0x0D4   //
#define SB_PMIO_REGD7                   0x0D7   //


#define SB_RTC_REG00                    0x00            // Seconds - RW
#define SB_RTC_REG01                    0x01            // Seconds Alarm - RW
#define SB_RTC_REG02                    0x02            // Minutes - RW
#define SB_RTC_REG03                    0x03            // Minutes Alarm - RW
#define SB_RTC_REG04                    0x04            // ours - RW
#define SB_RTC_REG05                    0x05            // ours Alarm- RW
#define SB_RTC_REG06                    0x06            // Day of Week - RW
#define SB_RTC_REG07                    0x07            // Date of Mont - RW
#define SB_RTC_REG08                    0x08            // Mont - RW
#define SB_RTC_REG09                    0x09            // Year - RW
#define SB_RTC_REG0A                    0x0A            // Register A - RW
#define SB_RTC_REG0B                    0x0B            // Register B - RW
#define SB_RTC_REG0C                    0x0C            // Register C - R
#define SB_RTC_REG0D                    0x0D            // DateAlarm - RW
#define SB_RTC_REG32                    0x32            // AltCentury - RW
#define SB_RTC_REG48                    0x48            // Century - RW
#define SB_RTC_REG50                    0x50            // Extended RAM Address Port - RW
#define SB_RTC_REG53                    0x53            // Extended RAM Data Port - RW
#define SB_RTC_REG7E                    0x7E            // RTC Time Clear - RW
#define SB_RTC_REG7F                    0x7F            // RTC RAM Enable - RW

#define B_ECMOS_REG00                   0x00            // scratc-reg
                                                        //;BIT0=0       AsicDebug is enabled
                                                        //;BIT1=0       SLT S3 runs
#define SB_ECMOS_REG01                  0x01
#define SB_ECMOS_REG02                  0x02
#define SB_ECMOS_REG03                  0x03
#define SB_ECMOS_REG04                  0x04
#define SB_ECMOS_REG05                  0x05
#define SB_ECMOS_REG06                  0x06
#define SB_ECMOS_REG07                  0x07
#define SB_ECMOS_REG08                  0x08            // save 32BIT Pysical address of Config structure
#define SB_ECMOS_REG09                  0x09
#define SB_ECMOS_REG0A                  0x0A
#define SB_ECMOS_REG0B                  0x0B

#define SB_ECMOS_REG0C                  0x0C            //;save MODULE_ID
#define SB_ECMOS_REG0D                  0x0D            //;Reserve for NB

#define SB_IOMAP_REG00                  0x000   // Dma_C 0
#define SB_IOMAP_REG02                  0x002   // Dma_C 1
#define SB_IOMAP_REG04                  0x004   // Dma_C 2
#define SB_IOMAP_REG06                  0x006   // Dma_C 3
#define SB_IOMAP_REG08                  0x008   // Dma_Status
#define SB_IOMAP_REG09                  0x009   // Dma_WriteRest
#define SB_IOMAP_REG0A                  0x00A   // Dma_WriteMask
#define SB_IOMAP_REG0B                  0x00B   // Dma_WriteMode
#define SB_IOMAP_REG0C                  0x00C   // Dma_Clear
#define SB_IOMAP_REG0D                  0x00D   // Dma_MasterClr
#define SB_IOMAP_REG0E                  0x00E   // Dma_ClrMask
#define SB_IOMAP_REG0F                  0x00F   // Dma_AllMask
#define SB_IOMAP_REG20                  0x020   // IntrCntrlReg1
#define SB_IOMAP_REG21                  0x021   // IntrCntrlReg2
#define SB_IOMAP_REG40                  0x040   // TimerC0
#define SB_IOMAP_REG41                  0x041   // TimerC1
#define SB_IOMAP_REG42                  0x042   // TimerC2
#define SB_IOMAP_REG43                  0x043   // Tmr1CntrlWord
#define SB_IOMAP_REG61                  0x061   // Nmi_Status
#define SB_IOMAP_REG70                  0x070   // Nmi_Enable
#define SB_IOMAP_REG71                  0x071   // RtcDataPort
#define SB_IOMAP_REG72                  0x072   // AlternatRtcAddrPort
#define SB_IOMAP_REG73                  0x073   // AlternatRtcDataPort
#define SB_IOMAP_REG80                  0x080   // Dma_Page_Reserved0
#define SB_IOMAP_REG81                  0x081   // Dma_PageC2
#define SB_IOMAP_REG82                  0x082   // Dma_PageC3
#define SB_IOMAP_REG83                  0x083   // Dma_PageC1
#define SB_IOMAP_REG84                  0x084   // Dma_Page_Reserved1
#define SB_IOMAP_REG85                  0x085   // Dma_Page_Reserved2
#define SB_IOMAP_REG86                  0x086   // Dma_Page_Reserved3
#define SB_IOMAP_REG87                  0x087   // Dma_PageC0
#define SB_IOMAP_REG88                  0x088   // Dma_Page_Reserved4
#define SB_IOMAP_REG89                  0x089   // Dma_PageC6
#define SB_IOMAP_REG8A                  0x08A   // Dma_PageC7
#define SB_IOMAP_REG8B                  0x08B   // Dma_PageC5
#define SB_IOMAP_REG8C                  0x08C   // Dma_Page_Reserved5
#define SB_IOMAP_REG8D                  0x08D   // Dma_Page_Reserved6
#define SB_IOMAP_REG8E                  0x08E   // Dma_Page_Reserved7
#define SB_IOMAP_REG8F                  0x08F   // Dma_Refres
#define SB_IOMAP_REG92                  0x092   // FastInit
#define SB_IOMAP_REGA0                  0x0A0   // IntrCntrl2Reg1
#define SB_IOMAP_REGA1                  0x0A1   // IntrCntrl2Reg2
#define SB_IOMAP_REGC0                  0x0C0   // Dma2_C4Addr
#define SB_IOMAP_REGC2                  0x0C2   // Dma2_C4Cnt
#define SB_IOMAP_REGC4                  0x0C4   // Dma2_C5Addr
#define SB_IOMAP_REGC6                  0x0C6   // Dma2_C5Cnt
#define SB_IOMAP_REGC8                  0x0C8   // Dma2_C6Addr
#define SB_IOMAP_REGCA                  0x0CA   // Dma2_C6Cnt
#define SB_IOMAP_REGCC                  0x0CC   // Dma2_C7Addr
#define SB_IOMAP_REGCE                  0x0CE   // Dma2_C7Cnt
#define SB_IOMAP_REGD0                  0x0D0   // Dma_Status
#define SB_IOMAP_REGD2                  0x0D2   // Dma_WriteRest
#define SB_IOMAP_REGD4                  0x0D4   // Dma_WriteMask
#define SB_IOMAP_REGD6                  0x0D6   // Dma_WriteMode
#define SB_IOMAP_REGD8                  0x0D8   // Dma_Clear
#define SB_IOMAP_REGDA                  0x0DA   // Dma_Clear
#define SB_IOMAP_REGDC                  0x0DC   // Dma_ClrMask
#define SB_IOMAP_REGDE                  0x0DE   // Dma_ClrMask
#define SB_IOMAP_REGF0                  0x0F0   // NCP_Error
#define SB_IOMAP_REG40B                 0x040B  // DMA1_Extend
#define SB_IOMAP_REG4D0                 0x04D0  // IntrEdgeControl
#define SB_IOMAP_REG4D6                 0x04D6  // DMA2_Extend
#define SB_IOMAP_REGC00                 0x0C00  // Pci_Intr_Index
#define SB_IOMAP_REGC01                 0x0C01  // Pci_Intr_Data
#define SB_IOMAP_REGC14                 0x0C14  // Pci_Error
#define SB_IOMAP_REGC50                 0x0C50  // CMIndex
#define SB_IOMAP_REGC51                 0x0C51  // CMData
#define SB_IOMAP_REGC52                 0x0C52  // GpmPort
#define SB_IOMAP_REGC6F                 0x0C6F  // Isa_Misc
#define SB_IOMAP_REGCD0                 0x0CD0  // PMio2_Index
#define SB_IOMAP_REGCD1                 0x0CD1  // PMio2_Data
#define SB_IOMAP_REGCD4                 0x0CD4  // BIOSRAM_Index
#define SB_IOMAP_REGCD5                 0x0CD5  // BIOSRAM_Data
#define SB_IOMAP_REGCD6                 0x0CD6  // PM_Index
#define SB_IOMAP_REGCD7                 0x0CD7  // PM_Data
#define SB_IOMAP_REGCF9                 0x0CF9          // CF9Rst reg


#define SB_CM_REG02                     0x002   // TempStatus (via SB_IOMAP_REGC50)
#define SB_CM_REG03                     0x003   // TempInterrupt (via SB_IOMAP_REGC50)

#define SB_SATA_REG00                   0x000   // Vendor ID - R- 16 bits
#define SB_SATA_REG02                   0x002   // Device ID - RW -16 bits
#define SB_SATA_REG04                   0x004   // PCI Command - RW - 16 bits
#define SB_SATA_REG06                   0x006   // PCI Status - RW - 16 bits
#define SB_SATA_REG08                   0x008   // Revision ID/PCI Class Code - R - 32 bits - Offset: 08
#define SB_SATA_REG0C                   0x00C   // Cace Line Size - R/W - 8bits
#define SB_SATA_REG0D                   0x00D   // Latency Timer - RW - 8 bits
#define SB_SATA_REG0E                   0x00E   // eader Type - R - 8 bits
#define SB_SATA_REG0F                   0x00F   // BIST - R - 8 bits
#define SB_SATA_REG10                   0x010   // Base Address Register 0 - RW - 32 bits
#define SB_SATA_REG14                   0x014   // Base Address Register 1 - RW- 32 bits
#define SB_SATA_REG18                   0x018   // Base Address Register 2 - RW - 32 bits
#define SB_SATA_REG1C                   0x01C   // Base Address Register 3 - RW - 32 bits
#define SB_SATA_REG20                   0x020   // Base Address Register 4 - RW - 32 bits
#define SB_SATA_REG24                   0x024   // Base Address Register 5 - RW - 32 bits
#define SB_SATA_REG2C                   0x02C   // Subsystem Vendor ID - R - 16 bits
#define SB_SATA_REG2D                   0x02D   // Subsystem ID - R - 16 bits
#define SB_SATA_REG30                   0x030   // Expansion ROM Base Address - 32 bits
#define SB_SATA_REG34                   0x034   // Capabilities Pointer - R - 32 bits
#define SB_SATA_REG3C                   0x03C   // Interrupt Line - RW - 8 bits
#define SB_SATA_REG3D                   0x03D   // Interrupt Pin - R - 8 bits
#define SB_SATA_REG3E                   0x03E   // Min Grant - R - 8 bits
#define SB_SATA_REG3F                   0x03F   // Max Latency - R - 8 bits
#define SB_SATA_REG40                   0x040   // Configuration - RW - 32 bits
#define SB_SATA_REG44                   0x044   // Software Data Register - RW - 32 bits
#define SB_SATA_REG48                   0x048
#define SB_SATA_REG50                   0x050   // Message Capability - R - 16 bits
#define SB_SATA_REG52                   0x052   // Message Control - R/W - 16 bits
#define SB_SATA_REG54                   0x054   // Message Address - R/W - 32 bits
#define SB_SATA_REG58                   0x058   // Message Data - R/W - 16 bits
#define SB_SATA_REG5C                   0x05C   // RAMBIST Control Register - R/W - 8 bits
#define SB_SATA_REG5D                   0x05D   // RAMBIST Status0 Register - R - 8 bits
#define SB_SATA_REG5E                   0x05E   // RAMBIST Status1 Register - R - 8 bits
#define SB_SATA_REG60                   0x060   // Power Management Capabilities - R - 32 bits
#define SB_SATA_REG64                   0x064   // Power Management Control + Status - RW - 32 bits
#define SB_SATA_REG68                   0x068   // MSI Program Weigt - R/W - 8 bits
#define SB_SATA_REG69                   0x069   // PCI Burst Timer - R/W - 8 bits
#define SB_SATA_REG70                   0x070   // PCI Bus Master - IDE0 - RW - 32 bits
#define SB_SATA_REG74                   0x074   // PRD Table Address - IDE0 - RW - 32 bits
#define SB_SATA_REG78                   0x078   // PCI Bus Master - IDE1 - RW - 32 bits
#define SB_SATA_REG7C                   0x07C   // PRD Table Address - IDE1 - RW - 32 bits
#define SB_SATA_REG80                   0x080   // Data Transfer Mode - IDE0 - RW - 32 bits
#define SB_SATA_REG84                   0x084   // Data Transfer Mode - IDE1 - RW - 32 bits
#define SB_SATA_REG86                   0x086   // PY Global Control
#define SB_SATA_REG87                   0x087
#define SB_SATA_REG88                   0x088   // PHY Port0 Control - Port0 PY fine tune(0:23)
#define SB_SATA_REG8A                   0x08A
#define SB_SATA_REG8C                   0x08C   // PHY Port1 Control - Port0 PY fine tune(0:23)
#define SB_SATA_REG8E                   0x08E
#define SB_SATA_REG90                   0x090   // PHY Port2 Control - Port0 PY fine tune(0:23)
#define SB_SATA_REG92                   0x092
#define SB_SATA_REG94                   0x094   // PHY Port3 Control - Port0 PY fine tune(0:23)
#define SB_SATA_REG96                   0x096
#define SB_SATA_REG98                   0x098   // EEPROM Memory Address - Command + Status - RW - 32 bits
#define SB_SATA_REG9C                   0x09C   // EEPROM Memory Data - RW - 32 bits
#define SB_SATA_REGA0                   0x0A0   //
#define SB_SATA_REGA4                   0x0A4   //
#define SB_SATA_REGA5                   0x0A5   //;
#define SB_SATA_REGA8                   0x0A8   //
#define SB_SATA_REGAD                   0x0AD   //;
#define SB_SATA_REGB0                   0x0B0   // IDE1 Task File Configuration + Status - RW - 32 bits
#define SB_SATA_REGB5                   0x0B5   //;
#define SB_SATA_REGBD                   0x0BD   //;
#define SB_SATA_REGC0                   0x0C0   // BA5 Indirect Address - RW - 32 bits
#define SB_SATA_REGC4                   0x0C4   // BA5 Indirect Access - RW - 32 bits

#define SB_SATA_BAR5_REG00              0x000   // PCI Bus Master - IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG04              0x004   // PRD Table Address - IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG08              0x008   // PCI Bus Master - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REG0C              0x00C   // PRD Table Address - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REG10              0x010   // PCI Bus Master2 - IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG18              0x018   // PCI Bus Master2 - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REG20              0x020   // PRD Address - IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG24              0x024   // PCI Bus Master Byte Count - IDE0- RW - 32 bits
#define SB_SATA_BAR5_REG28              0x028   // PRD Address - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REG2C              0x02C   // PCI Bus Master Byte Count - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REG40              0x040   // FIFO Valid Byte Count and Control - IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG44              0x044   // FIFO Valid Byte Count and Control - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REG48              0x048   // System Configuration Status - Command - RW - 32 bits
#define SB_SATA_BAR5_REG4C              0x04C   // System Software Data Register - RW - 32 bits
#define SB_SATA_BAR5_REG50              0x050   // FLAS Memory Address - Command + Status - RW - 32 bits
#define SB_SATA_BAR5_REG54              0x054   // FLAS Memory Data - RW - 32 bits
#define SB_SATA_BAR5_REG58              0x058   // EEPROM Memory Address - Command + Status - RW - 32 bits
#define SB_SATA_BAR5_REG5C              0x05C   // EEPROM Memory Data - RW - 32 bits
#define SB_SATA_BAR5_REG60              0x060   // FIFO Port - IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG68              0x068   // FIFO Pointers1- IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG6C              0x06C   // FIFO Pointers2- IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REG70              0x070   // FIFO Port - IDE1- RW - 32 bits
#define SB_SATA_BAR5_REG78              0x078   // FIFO Pointers1- IDE1- RW - 32 bits
#define SB_SATA_BAR5_REG7C              0x07C   // FIFO Pointers2- IDE1- RW - 32 bits
#define SB_SATA_BAR5_REG80              0x080   // IDE0 Task File Register 0- RW - 32 bits
#define SB_SATA_BAR5_REG84              0x084   // IDE0 Task File Register 1- RW - 32 bits
#define SB_SATA_BAR5_REG88              0x088   // IDE0 Task File Register 2- RW - 32 bits
#define SB_SATA_BAR5_REG8C              0x08C   // IDE0 Read Aead Data - RW - 32 bits
#define SB_SATA_BAR5_REG90              0x090   // IDE0 Task File Register 0 - Command Buffering - RW - 32 bits
#define SB_SATA_BAR5_REG94              0x094   // IDE0 Task File Register 1 - Command Buffering - RW - 32 bits
#define SB_SATA_BAR5_REG9C              0x09C   // IDE0 Virtual DMA/PIO Read Aead Byte Count - RW - 32 bits
#define SB_SATA_BAR5_REGA0              0x0A0   // IDE0 Task File Configuration + Status - RW - 32 bits
#define SB_SATA_BAR5_REGB4              0x0B4   // Data Transfer Mode -IDE0 - RW - 32 bits
#define SB_SATA_BAR5_REGC0              0x0C0   // IDE1 Task File Register 0 - RW - 32 bits
#define SB_SATA_BAR5_REGC4              0x0C4   // IDE1 Task File Register 1 - RW - 32 bits
#define SB_SATA_BAR5_REGC8              0x0C8   // IDE1 Task File Register 2 - RW - 32 bits
#define SB_SATA_BAR5_REGCC              0x0CC   //  Read/Write Aead Data - RW - 32 bits
#define SB_SATA_BAR5_REGD0              0x0D0   // IDE1 Task File Register 0 - Command Buffering - RW - 32 bits
#define SB_SATA_BAR5_REGD4              0x0D4   // IDE1 Task File Register 1 - Command Buffering - RW - 32 bits
#define SB_SATA_BAR5_REGDC              0x0DC   // IDE1 Virtual DMA/PIO Read Aead Byte Count - RW - 32 bits
#define SB_SATA_BAR5_REGE0              0x0E0   // IDE1 Task File Configuration + Status - RW - 32 bits
#define SB_SATA_BAR5_REGF4              0x0F4   // Data Transfer Mode - IDE1 - RW - 32 bits
#define SB_SATA_BAR5_REGF8              0x0F8   // PORT Configuration
#define SB_SATA_BAR5_REGFC              0x0FC

#define SB_SATA_BAR5_REG100             0x0100  //;Serial ATA SControl - RW - 32 bits - [Offset: 100h (channel 1) / 180
#define SB_SATA_BAR5_REG104             0x0104  //;Serial ATA Sstatus - RW - 32 bits - [Offset: 104h (channel 1) / 184h (cannel
#define SB_SATA_BAR5_REG108             0x0108  //;Serial ATA Serror - RW - 32 bits - [Offset: 108h (channel 1) / 188h (cannel
#define SB_SATA_BAR5_REG10C             0x010C  //;Serial ATA Sdevice - RW - 32 bits - [Offset: 10Ch (channel 1) / 18Ch (cannel
#define SB_SATA_BAR5_REG110             0x0110  // Port-N Interrupt Status
#define SB_SATA_BAR5_REG144             0x0144  //;Serial ATA PY Configuration - RW - 32 bits
#define SB_SATA_BAR5_REG148             0x0148  //;SIEN - RW - 32 bits - [Offset: 148 (channel 1) / 1C8 (cannel 2)]
#define SB_SATA_BAR5_REG14C             0x014C  //;SFISCfg - RW - 32 bits - [Offset: 14C (channel 1) / 1CC (cannel 2)]
#define SB_SATA_BAR5_REG120             0x0120  // Port Task Fike Data
#define SB_SATA_BAR5_REG128             0x0128  // Port Serial ATA Status
#define SB_SATA_BAR5_REG12C             0x012C  // Port Serial ATA Control

#define SB_SATA_BAR5_REG130             0x0130
#define SB_SATA_BAR5_REG1B0             0x01B0
#define SB_SATA_BAR5_REG230             0x0230
#define SB_SATA_BAR5_REG2B0             0x02B0

#define SB_FC_REG00                     0x00            // Device/Vendor ID - R
#define SB_FC_REG04                     0x04            // Command - RW
#define SB_FC_REG10                     0x10            // BAR

#define SB_FC_MMIO_REG70                0x070
#define SB_FC_MMIO_REG200               0x200

#define SB_OHCI_REG00                   0x00            // Device/Vendor ID - R
#define SB_OHCI_REG04                   0x04            // Command - RW
#define SB_OHCI_REG06                   0x06            // Status - R
#define SB_OHCI_REG08                   0x08            // Revision ID/Class Code - R
#define SB_OHCI_REG0C                   0x0C            // Miscellaneous - RW
#define SB_OHCI_REG10                   0x10            // Bar_OCI - RW
#define SB_OHCI_REG2C                   0x2C            // Subsystem Vendor ID/ Subsystem ID - RW
#define SB_OHCI_REG34                   0x34            // Capability Pointer - R
#define SB_OHCI_REG3C                   0x3C            // Interrupt Line - RW
#define SB_OHCI_REG3D                   0x3D            // Interrupt Line - RW

#define SB_OHCI_REG40                   0x40            // Config Timers - RW
#define SB_OHCI_REG4C                   0x4C            // MSI Weigt - RW
#define SB_OHCI_REG50                   0x50            // ATI Misc Control - RW
#define SB_OHCI_REG51                   0x51
#define SB_OHCI_REG58                   0x58            // Over Current Control - RW
#define SB_OHCI_REG5C                   0x5C            // Over Current Control - RW
#define SB_OHCI_REG60                   0x60            // Serial Bus Release Number - R
#define SB_OHCI_REG68                   0x68            // Over Current Enable - RW
#define SB_OHCI_REGD0                   0x0D0   // MSI Control - RW
#define SB_OHCI_REGD4                   0x0D4   // MSI Address - RW
#define SB_OHCI_REGD8                   0x0D8   // MSI Data - RW
#define SB_OHCI_BAR_REG00               0x00            // cRevision - R
#define SB_OHCI_BAR_REG04               0x04            // cControl
#define SB_OHCI_BAR_REG08               0x08            // cCommandStatus
#define SB_OHCI_BAR_REG0C               0x0C            // cInterruptStatus  RW
#define SB_OHCI_BAR_REG10               0x10            // cInterruptEnable
#define SB_OHCI_BAR_REG14               0x14            // cInterruptDisable
#define SB_OHCI_BAR_REG18               0x18            // HcCCA
#define SB_OHCI_BAR_REG1C               0x1C            // cPeriodCurrentED
#define SB_OHCI_BAR_REG20               0x20            // HcControleadED
#define SB_OHCI_BAR_REG24               0x24            // cControlCurrentED  RW
#define SB_OHCI_BAR_REG28               0x28            // HcBulkeadED
#define SB_OHCI_BAR_REG2C               0x2C            // cBulkCurrentED- RW
#define SB_OHCI_BAR_REG30               0x30            // HcDoneead
#define SB_OHCI_BAR_REG34               0x34            // cFmInterval
#define SB_OHCI_BAR_REG38               0x38            // cFmRemaining
#define SB_OHCI_BAR_REG3C               0x3C            // cFmNumber
#define SB_OHCI_BAR_REG40               0x40            // cPeriodicStart
#define SB_OHCI_BAR_REG44               0x44            // HcLSThresold
#define SB_OHCI_BAR_REG48               0x48            // HcRDescriptorA
#define SB_OHCI_BAR_REG4C               0x4C            // HcRDescriptorB
#define SB_OHCI_BAR_REG50               0x50            // HcRStatus
#define SB_OHCI_BAR_REG160              0x160

#define SB_EHCI_REG00                   0x00            // DEVICE/VENDOR ID - R
#define SB_EHCI_REG04                   0x04            // Command - RW
#define SB_EHCI_REG06                   0x06            // Status - R
#define SB_EHCI_REG08                   0x08            // Revision ID/Class Code - R
#define SB_EHCI_REG0C                   0x0C            // Miscellaneous - RW
#define SB_EHCI_REG10                   0x10            // BAR - RW
#define SB_EHCI_REG2C                   0x2C            // Subsystem ID/Subsystem Vendor ID - RW
#define SB_EHCI_REG34                   0x34            // Capability Pointer - R
#define SB_EHCI_REG3C                   0x3C            // Interrupt Line - RW
#define SB_EHCI_REG3D                   0x3D            // Interrupt Line - RW
#define SB_EHCI_REG40                   0x40            // Config Timers - RW
#define SB_EHCI_REG4C                   0x4C            // MSI Weigt - RW
#define SB_EHCI_REG50                   0x50            // ATI Misc Control - RW
#define SB_EHCI_REG54                   0x54            // ATI Misc Control - RW
#define SB_EHCI_REG58                   0x58            // Over Current Control - R
#define SB_EHCI_REG60                   0x60            // SBRN - R
#define SB_EHCI_REG61                   0x61            // FLADJ - RW
#define SB_EHCI_REG62                   0x62            // PORTWAKECAP - RW
#define SB_EHCI_REGD0                   0x0D0   // MSI Control - RW
#define SB_EHCI_REGD4                   0x0D4   // MSI Address - RW
#define SB_EHCI_REGD8                   0x0D8   // MSI Data - RW
#define SB_EHCI_REGDC                   0x0DC   // PME Control - RW
#define SB_EHCI_REGE0                   0x0E0   // PME Data / Status - RW
#define SB_EHCI_BAR_REG00               0x00            // CAPLENGT - R
#define SB_EHCI_BAR_REG02               0x002           // CIVERSION- R
#define SB_EHCI_BAR_REG04               0x004           // CSPARAMS - R
#define SB_EHCI_BAR_REG08               0x008           // CCPARAMS - R
#define SB_EHCI_BAR_REG0C               0x00C           // CSP-PORTROUTE - R
#define SB_EHCI_BAR_REG20               0x020   // USBCMD - RW - 32 bits
#define SB_EHCI_BAR_REG24               0x024   // USBSTS - RW - 32 bits
#define SB_EHCI_BAR_REG28               0x028   // USBINTR -RW - 32 bits
#define SB_EHCI_BAR_REG2C               0x02C   // FRINDEX -RW - 32 bits
#define SB_EHCI_BAR_REG30               0x030   // CTRLDSSEGMENT -RW - 32 bits
#define SB_EHCI_BAR_REG34               0x034   // PERIODICLISTBASE -RW - 32 bits
#define SB_EHCI_BAR_REG38               0x038   // ASYNCLISTADDR -RW - 32 bits
#define SB_EHCI_BAR_REG60               0x060   // CONFIGFLAG -RW - 32 bits
#define SB_EHCI_BAR_REG64               0x064   // PORTSC(1-N_PORTS) -RW - 32 bits
#define SB_EHCI_BAR_REG84               0x084   // Packet Buffer Thresold Values - RW - 32 bits
#define SB_EHCI_BAR_REG88               0x088   // Packet Buffer Dept Value - RW - 32 bits
#define SB_EHCI_BAR_REG94               0x094   // UTMI Control and Status - RW - 32 bits
#define SB_EHCI_BAR_REG98               0x098   // Bist Control - RW - 32 bits
#define SB_EHCI_BAR_REG9C               0x09C   // ATI EOR Control - RW - 32 bits
#define SB_EHCI_BAR_REGA4               0x0A4   // USB IN/OUT FIFO Thresold Setting
#define SB_EHCI_BAR_REGBC               0x0BC   // ECI misc Setting
#define SB_EHCI_BAR_REGC0               0x0C0   // USB PHY Auto Calibration Setting

#define SB_SMBUS_REG00                  0x000   //;VendorID - R
#define SB_SMBUS_REG02                  0x002   //;DeviceID - R
#define SB_SMBUS_REG04                  0x004   // Command- RW
#define SB_SMBUS_REG05                  0x005   // Command- RW
#define SB_SMBUS_REG06                  0x006   // STATUS- RW
#define SB_SMBUS_REG08                  0x008   // Revision ID/Class Code- R
#define SB_SMBUS_REG0A                  0x00A   //;
#define SB_SMBUS_REG0B                  0x00B   //;
#define SB_SMBUS_REG0C                  0x00C   // Cace Line Size- R
#define SB_SMBUS_REG0D                  0x00D   // Latency Timer- R
#define SB_SMBUS_REG0E                  0x00E   // eader Type- R
#define SB_SMBUS_REG0F                  0x00F   // BIST- R
#define SB_SMBUS_REG10                  0x010   // Base Address 0- R
#define SB_SMBUS_REG11                  0x011   //;
#define SB_SMBUS_REG12                  0x012   //;
#define SB_SMBUS_REG13                  0x013   //;
#define SB_SMBUS_REG14                  0x014   // Base Address 1- R
#define SB_SMBUS_REG18                  0x018   // Base Address 2- R
#define SB_SMBUS_REG1C                  0x01C   // Base Address 3- R
#define SB_SMBUS_REG20                  0x020   // Base Address 4- R
#define SB_SMBUS_REG24                  0x024   // Base Address 5- R
#define SB_SMBUS_REG28                  0x028   // Cardbus CIS Pointer- R
#define SB_SMBUS_REG2C                  0x02C   // Subsystem Vendor ID- W
#define SB_SMBUS_REG2E                  0x02E   // Subsystem ID- W
#define SB_SMBUS_REG30                  0x030   // Expansion ROM Base Address - R
#define SB_SMBUS_REG34                  0x034   // Capability Pointer - R
#define SB_SMBUS_REG38                  0x038
#define SB_SMBUS_REG3C                  0x03C   // Interrupt Line - R
#define SB_SMBUS_REG3D                  0x03D   // Interrupt Pin - R
#define SB_SMBUS_REG3E                  0x03E   // Min_Gnt - R
#define SB_SMBUS_REG3F                  0x03F   // Max_Lat - R
#define SB_SMBUS_REG40                  0x040   // PCI Control- RW
#define SB_SMBUS_REG41                  0x041   // MiscFunction- RW
#define SB_SMBUS_REG42                  0x042   // DmaLimit- RW
#define SB_SMBUS_REG43                  0x043   // DmaEnanceEnable RW
#define SB_SMBUS_REG48                  0x048   // ISA Address Decode Control Register #1- RW
#define SB_SMBUS_REG49                  0x049   // ISA Address Decode Control Register #2- RW
#define SB_SMBUS_REG4A                  0x04A   // Scratc Pad- RW
#define SB_SMBUS_REG50                  0x050   // PciGpioOutControl- RW
#define SB_SMBUS_REG54                  0x054   // PciGpioConfig- RW
#define SB_SMBUS_REG58                  0x058   // ASFSMBusIoBase
#define SB_SMBUS_REG59                  0x059   //;
#define SB_SMBUS_REG5C                  0x05C   // Smart Power Control1
#define SB_SMBUS_REG60                  0x060   // MiscEnable- RW
#define SB_SMBUS_REG64                  0x064   // Features Enable- RW
#define SB_SMBUS_REG68                  0x068   // UsbEnable - RW
#define SB_SMBUS_REG6C                  0x06C   // TestMode- RW
#define SB_SMBUS_REG70                  0x070   // RunTimeTest- R
#define SB_SMBUS_REG74                  0x074   // IoApic_Conf- RW
#define SB_SMBUS_REG78                  0x078   // IoAddrEnable - R/W
#define SB_SMBUS_REG79                  0x079   //;
#define SB_SMBUS_REG7C                  0x07C   // RTC Control  ;VSJ-2005-06-16
#define SB_SMBUS_REG80                  0x080   // GPIO_Out_Cntrl - RW
#define SB_SMBUS_REG81                  0x081   // GPIO_Status - R
#define SB_SMBUS_REG90                  0x090   // Smbus Base Address - R
#define SB_SMBUS_REG94                  0x094   // Reserved - R
#define SB_SMBUS_REG98                  0x098   //
#define SB_SMBUS_REGA0                  0x0A0   // MoreGPIOIn +C R
#define SB_SMBUS_REGA4                  0x0A4   // MoreGPIOIn +C R
#define SB_SMBUS_REGA8                  0x0A8   // GPIOControl +C RW
#define SB_SMBUS_REGAC                  0x0AC   // MiscUsbEt - RW
#define SB_SMBUS_REGAD                  0x0AD   // MiscSata
#define SB_SMBUS_REGAE                  0x0AE
#define SB_SMBUS_REGAF                  0x0AF   // SataIntMap - RW
#define SB_SMBUS_REGB0                  0x0B0   // MSI Mapping Capability - R
#define SB_SMBUS_REGB4                  0x0B4   //HPET BASE Address
#define SB_SMBUS_REGBC                  0x0BC   // PciIntGpio - RW
#define SB_SMBUS_REGBE                  0x0BE   // UsbIntMap - RW
#define SB_SMBUS_REGC0                  0x0C0   // IokHiDrvSt - RW
#define SB_SMBUS_REGD0                  0x0D0   //
#define SB_SMBUS_REGD2                  0x0D2   // I2CbusConfig - RW
#define SB_SMBUS_REGD3                  0x0D3   // I2CCommand - RW
#define SB_SMBUS_REGD4                  0x0D4   // I2CSadow1- RW
#define SB_SMBUS_REGD5                  0x0D5   // I2Csadow2- RW
#define SB_SMBUS_REGD6                  0x0D6   // I2CBusRevision - RW
#define SB_SMBUS_REGE0                  0x0E0   // MSI_Weigt
#define SB_SMBUS_REGE1                  0x0E1   // MSI_Weigt
#define SB_SMBUS_REGF0                  0x0F0   // AB_REG_BAR - RW
#define SB_SMBUS_REGF1                  0x0F1
#define SB_SMBUS_REGF4                  0x0F4   // WakeIoAddr- RW
#define SB_SMBUS_REGF8                  0x0F8   // ExtendedAddrPort- RW
#define SB_SMBUS_REGFC                  0x0FC   // ExtendedDataPort- RW


#define SB_IDE_REG00                    0x00            // Vendor ID
#define SB_IDE_REG02                    0x02            // Device ID
#define SB_IDE_REG04                    0x04            // Command
#define SB_IDE_REG06                    0x06            // Status
#define SB_IDE_REG08                    0x08            // Revision ID/Class Code
#define SB_IDE_REG09                    0x09            // Class Code
#define SB_IDE_REG0A                    0x0A
#define SB_IDE_REG0C                    0x0C            // Cace Link Size
#define SB_IDE_REG0D                    0x0D            // Master Latency Timer
#define SB_IDE_REG0E                    0x0E            // eader Type
#define SB_IDE_REG0F                    0x0F            // BIST Mode Type
#define SB_IDE_REG10                    0x10            // Base Address 0
#define SB_IDE_REG14                    0x14            // Base Address 1
#define SB_IDE_REG18                    0x18            // Base Address 2
#define SB_IDE_REG1C                    0x1C            // Base Address 3
#define SB_IDE_REG20                    0x20            // Bus Master Interface Base Address
#define SB_IDE_REG2C                    0x2C            // Subsystem ID and Subsystem Vendor ID
#define SB_IDE_REG34                    0x34            // MSI Capabilities Pointer
#define SB_IDE_REG3C                    0x3C            // Interrupt Line
#define SB_IDE_REG3D                    0x3D            // Interrupt Pin
#define SB_IDE_REG3E                    0x3E            // Min_gnt
#define SB_IDE_REG3F                    0x3F            // Max_latency
#define SB_IDE_REG40                    0x40            // IDE PIO Timing
#define SB_IDE_REG44                    0x44            // IDE Legacy DMA (Multi-words DMA) Timing Modes
#define SB_IDE_REG48                    0x48            // IDE PIO Control
#define SB_IDE_REG4A                    0x4A            // IDE PIO Mode
#define SB_IDE_REG4C                    0x4C            // IDE Status
#define SB_IDE_REG54                    0x54            // IDE Ultra DMAControl
#define SB_IDE_REG55                    0x55            // IDE Ultra DMA Status
#define SB_IDE_REG56                    0x56            // IDE Ultra DMA Mode
#define SB_IDE_REG60                    0x60            // IDE PCI Retry Timing Counter
#define SB_IDE_REG61                    0x61            // PCI Error Control
#define SB_IDE_REG62                    0x62            // IDE Internal Control
#define SB_IDE_REG63                    0x63            // IDE Internal Control
#define SB_IDE_REG64                    0x64            // IDE PLL Control
#define SB_IDE_REG68                    0x68            // IDE MSI Programmable Weigt
#define SB_IDE_REG6C                    0x6C            // IDE Dynamic Clocking
#define SB_IDE_REG70                    0x70            // IDE MSI Control
#define SB_IDE_REG74                    0x74            // IDE MSI Address Register
#define SB_IDE_REG78                    0x78            // IDE MSI Data Register


#define SB_AZ_REG00                     0x00            // Vendor ID - R
#define SB_AZ_REG02                     0x02            // Device ID - R/W
#define SB_AZ_REG04                     0x04            // PCI Command
#define SB_AZ_REG06                     0x06            // PCI Status - R/W
#define SB_AZ_REG08                     0x08            // Revision ID
#define SB_AZ_REG09                     0x09            // Programming Interface
#define SB_AZ_REG0A                     0x0A            // Sub Class Code
#define SB_AZ_REG0B                     0x0B            // Base Class Code
#define SB_AZ_REG0C                     0x0C            // Cace Line Size - R/W
#define SB_AZ_REG0D                     0x0D            // Latency Timer
#define SB_AZ_REG0E                     0x0E            // eader Type
#define SB_AZ_REG0F                     0x0F            // BIST
#define SB_AZ_REG10                     0x10            // Lower Base Address Register
#define SB_AZ_REG14                     0x14            // Upper Base Address Register
#define SB_AZ_REG2C                     0x2C            // Subsystem Vendor ID
#define SB_AZ_REG2D                     0x2D            // Subsystem ID
#define SB_AZ_REG34                     0x34            // Capabilities Pointer
#define SB_AZ_REG3C                     0x3C            // Interrupt Line
#define SB_AZ_REG3D                     0x3D            // Interrupt Pin
#define SB_AZ_REG3E                     0x3E            // Minimum Grant
#define SB_AZ_REG3F                     0x3F            // Maximum Latency
#define SB_AZ_REG40                     0x40            // Misc Control 1
#define SB_AZ_REG42                     0x42            // Misc Control 2 Register
#define SB_AZ_REG43                     0x43            // Misc Control 3 Register
#define SB_AZ_REG44                     0x44            // Interrupt Pin Control Register
#define SB_AZ_REG46                     0x46            // Debug Control Register
#define SB_AZ_REG4C                     0x4C
#define SB_AZ_REG50                     0x50            // Power Management Capability ID
#define SB_AZ_REG52                     0x52            // Power Management Capabilities
#define SB_AZ_REG54                     0x54            // Power Management Control/Status
#define SB_AZ_REG60                     0x60            // MSI Capability ID
#define SB_AZ_REG62                     0x62            // MSI Message Control
#define SB_AZ_REG64                     0x64            // MSI Message Lower Address
#define SB_AZ_REG68                     0x68            // MSI Message Upper Address
#define SB_AZ_REG6C                     0x6C            // MSI Message Data

#define SB_AZ_BAR_REG00                 0x00            // Global Capabilities - R
#define SB_AZ_BAR_REG02                 0x02            // Minor Version - R
#define SB_AZ_BAR_REG03                 0x03            // Major Version - R
#define SB_AZ_BAR_REG04                 0x04            // Output Payload Capability - R
#define SB_AZ_BAR_REG06                 0x06            // Input Payload Capability - R
#define SB_AZ_BAR_REG08                 0x08            // Global Control - R/W
#define SB_AZ_BAR_REG0C                 0x0C            // Wake Enable - R/W
#define SB_AZ_BAR_REG0E                 0x0E            // State Cange Status - R/W
#define SB_AZ_BAR_REG10                 0x10            // Global Status - R/W
#define SB_AZ_BAR_REG18                 0x18            // Output Stream Payload Capability - R
#define SB_AZ_BAR_REG1A                 0x1A            // Input Stream Payload Capability - R
#define SB_AZ_BAR_REG20                 0x20            // Interrupt Control - R/W
#define SB_AZ_BAR_REG24                 0x24            // Interrupt Status - R/W
#define SB_AZ_BAR_REG30                 0x30            // Wall Clock Counter - R
#define SB_AZ_BAR_REG38                 0x38            // Stream Syncronization - R/W
#define SB_AZ_BAR_REG40                 0x40            // CORB Lower Base Address - R/W
#define SB_AZ_BAR_REG44                 0x44            // CORB Upper Base Address - RW
#define SB_AZ_BAR_REG48                 0x48            // CORB Write Pointer - R/W
#define SB_AZ_BAR_REG4A                 0x4A            // CORB Read Pointer - R/W
#define SB_AZ_BAR_REG4C                 0x4C            // CORB Control - R/W
#define SB_AZ_BAR_REG4D                 0x4D            // CORB Status - R/W
#define SB_AZ_BAR_REG4E                 0x4E            // CORB Size - R/W
#define SB_AZ_BAR_REG50                 0x50            // RIRB Lower Base Address - RW
#define SB_AZ_BAR_REG54                 0x54            // RIRB Upper Address - RW
#define SB_AZ_BAR_REG58                 0x58            // RIRB Write Pointer - RW
#define SB_AZ_BAR_REG5A                 0x5A            // RIRB Response Interrupt Count - R/W
#define SB_AZ_BAR_REG5C                 0x5C            // RIRB Control - R/W
#define SB_AZ_BAR_REG5D                 0x5D            // RIRB Status - R/W
#define SB_AZ_BAR_REG5E                 0x5E            // RIRB Size - R/W
#define SB_AZ_BAR_REG60                 0x60            // Immediate Command Output Interface - R/W
#define SB_AZ_BAR_REG64                 0x64            // Immediate Command Input Interface - R/W
#define SB_AZ_BAR_REG68                 0x68            // Immediate Command Input Interface - R/W
#define SB_AZ_BAR_REG70                 0x70            // DMA Position Lower Base Address - R/W
#define SB_AZ_BAR_REG74                 0x74            // DMA Position Upper Base Address - R/W
#define SB_AZ_BAR_REG2030               0x2030          // Wall Clock Counter Alias - R


#define SB_LPC_REG00                    0x00            // VID- R
#define SB_LPC_REG02                    0x02            // DID- R
#define SB_LPC_REG04                    0x04            // CMD- RW
#define SB_LPC_REG06                    0x06            // STATUS- RW
#define SB_LPC_REG08                    0x08            // Revision  ID/Class Code - R
#define SB_LPC_REG0C                    0x0C            // Cace Line Size - R
#define SB_LPC_REG0D                    0x0D            // Latency Timer - R
#define SB_LPC_REG0E                    0x0E            // eader Type - R
#define SB_LPC_REG0F                    0x0F            // BIST- R
#define SB_LPC_REG10                    0x10            // Base Address Reg 0- RW*
#define SB_LPC_REG2C                    0x2C            // Subsystem ID & Subsystem Vendor ID - Wo/Ro
#define SB_LPC_REG34                    0x34            // Capabilities Pointer - Ro
#define SB_LPC_REG40                    0x40            // PCI Control - RW
#define SB_LPC_REG44                    0x44            // IO Port Decode Enable Register 1- RW
#define SB_LPC_REG45                    0x45            // IO Port Decode Enable Register 2- RW
#define SB_LPC_REG46                    0x46            // IO Port Decode Enable Register 3- RW
#define SB_LPC_REG47                    0x47            // IO Port Decode Enable Register 4- RW
#define SB_LPC_REG48                    0x48            // IO/Mem Port Decode Enable Register 5- RW
#define SB_LPC_REG49                    0x49            // LPC Sync Timeout Count - RW
#define SB_LPC_REG4A                    0x4A            // IO/Mem Port Decode Enable Register 6- RW
#define SB_LPC_REG4C                    0x4C            // Memory Range Register - RW
#define SB_LPC_REG50                    0x50            // Rom Protect 0 - RW
#define SB_LPC_REG54                    0x54            // Rom Protect 1 - RW
#define SB_LPC_REG58                    0x58            // Rom Protect 2 - RW
#define SB_LPC_REG5C                    0x5C            // Rom Protect 3 - RW
#define SB_LPC_REG60                    0x60            // PCI Memory Start Address of LPC Target Cycles -
#define SB_LPC_REG62                    0x62            // PCI Memory End Address of LPC Target Cycles -
#define SB_LPC_REG64                    0x64            // PCI IO base Address of Wide Generic Port - RW
#define SB_LPC_REG65                    0x65
#define SB_LPC_REG66                    0x66
#define SB_LPC_REG67                    0x67
#define SB_LPC_REG68                    0x68            // LPC ROM Address Range 1 (Start Address) - RW
#define SB_LPC_REG69                    0x69
#define SB_LPC_REG6A                    0x6A            // LPC ROM Address Range 1 (End Address) - RW
#define SB_LPC_REG6B                    0x6B
#define SB_LPC_REG6C                    0x6C            // LPC ROM Address Range 2 (Start Address)- RW
#define SB_LPC_REG6D                    0x6D
#define SB_LPC_REG6E                    0x6E            // LPC ROM Address Range 2 (End Address) - RW
#define SB_LPC_REG6F                    0x6F
#define SB_LPC_REG70                    0x70            // Firmware ub Select - RW*
#define SB_LPC_REG71                    0x71
#define SB_LPC_REG72                    0x72
#define SB_LPC_REG73                    0x73
#define SB_LPC_REG74                    0x74            // Alternative Wide IO Range Enable- W/R
#define SB_LPC_REG78                    0x78            // Miscellaneous Control Bits- W/R
#define SB_LPC_REG7C                    0x7C            // TPM (trusted plant form module) reg- W/R
#define SB_LPC_REG9C                    0x9C
#define SB_LPC_REG80                    0x80            // MSI Capability Register- R
#define SB_LPC_REG8C                    0x8C
#define SB_LPC_REGA0                    0x0A0           // SPI base address
#define SB_LPC_REGA1                    0x0A1           // SPI base address
#define SB_LPC_REGA2                    0x0A2           // SPI base address
#define SB_LPC_REGA3                    0x0A3           // SPI base address
#define SB_LPC_REGA4                    0x0A4
#define SB_LPC_REGB8                    0x0B8
#define SB_LPC_REGBA                    0x0BA           // EcControl
#define SB_LPC_REGBB                    0x0BB           // HostControl


#define SB_P2P_REG00                    0x00            // VID - R
#define SB_P2P_REG02                    0x02            // DID - R
#define SB_P2P_REG04                    0x04            // CMD- RW
#define SB_P2P_REG06                    0x06            // STATUS- RW
#define SB_P2P_REG08                    0x08            // Revision ID/Class Code- R
#define SB_P2P_REG0C                    0x0C            // CSIZE- RW
#define SB_P2P_REG0D                    0x0D            // LTIMER- RW
#define SB_P2P_REG0E                    0x0E            // TYPE- R
#define SB_P2P_REG18                    0x18            // PBN- RW
#define SB_P2P_REG19                    0x19            // SBN- RW
#define SB_P2P_REG1A                    0x1A            // SUBBN- RW
#define SB_P2P_REG1B                    0x1B            // SLTIMER- RW
#define SB_P2P_REG1C                    0x1C            // IOBASE- RW
#define SB_P2P_REG1D                    0x1D            // IOLMT- RW
#define SB_P2P_REG1E                    0x1E            // SSTATUS- RW
#define SB_P2P_REG20                    0x20            // MBASE- RW
#define SB_P2P_REG21                    0x21
#define SB_P2P_REG22                    0x22            // MLMT- RW
#define SB_P2P_REG23                    0x23
#define SB_P2P_REG24                    0x24            // PMBASE- RW
#define SB_P2P_REG25                    0x25
#define SB_P2P_REG26                    0x26            // PMLMT- RW
#define SB_P2P_REG27                    0x27
#define SB_P2P_REG30                    0x30            // IOBU16- RW
#define SB_P2P_REG32                    0x32            // IOLU16- RW
#define SB_P2P_REG34                    0x34            // ECP_PTR- R
#define SB_P2P_REG3C                    0x3C            // INTLN- RW
#define SB_P2P_REG3D                    0x3D            // INTPN- R
#define SB_P2P_REG3E                    0x3E            // BCTRL- RW
#define SB_P2P_REG40                    0x40            // CPCTRL- R/W
#define SB_P2P_REG41                    0x41            // DCTRL- RW
#define SB_P2P_REG42                    0x42            // CLKCTRL- R/W
#define SB_P2P_REG43                    0x43            // ARCTRL- RW
#define SB_P2P_REG44                    0x44            // SMLT_PERF- RW
#define SB_P2P_REG46                    0x46            // PMLT_PERF- RW
#define SB_P2P_REG48                    0x48            // PCDMA- RW
#define SB_P2P_REG49                    0x49            // Additional Priority- Bits RW
#define SB_P2P_REG4A                    0x4A            // PCICLK Enable- Bits RW
#define SB_P2P_REG4B                    0x4B            // Misc Control RW
#define SB_P2P_REG4C                    0x4C            // AutoClockRun control RW
#define SB_P2P_REG50                    0x50            // Dual Address Cycle Enable and PCIB_CLK_Stop
#define SB_P2P_REG54                    0x54            // MSI Mapping Capability
#define SB_P2P_REG58                    0x58            // Signature Register of Microsoft Rework
#define SB_P2P_REG64                    0x64            // Misc Control Register
#define SB_P2P_REG65                    0x65            // Misc Control Register

#define SB_PMIO2_REG00                  0x00
#define SB_PMIO2_REG01                  0x01
#define SB_PMIO2_REG31                  0x31
#define SB_PMIO2_REG32                  0x32
#define SB_PMIO2_REG33                  0x33
#define SB_PMIO2_REG34                  0x34
#define SB_PMIO2_REG35                  0x35
#define SB_PMIO2_REG36                  0x36
#define SB_PMIO2_REG37                  0x37
#define SB_PMIO2_REG38                  0x38
#define SB_PMIO2_REG39                  0x39
#define SB_PMIO2_REG3A                  0x3A
#define SB_PMIO2_REG3B                  0x3B
#define SB_PMIO2_REG3C                  0x3C
#define SB_PMIO2_REG3D                  0x3D
#define SB_PMIO2_REG3E                  0x3E
#define SB_PMIO2_REG3F                  0x3F
#define SB_PMIO2_REG40                  0x40
#define SB_PMIO2_REG41                  0x41
#define SB_PMIO2_REG42                  0x42
#define SB_PMIO2_REG43                  0x43
#define SB_PMIO2_REG44                  0x44
#define SB_PMIO2_REG45                  0x45
#define SB_PMIO2_REG46                  0x46
#define SB_PMIO2_REG47                  0x47
#define SB_PMIO2_REG48                  0x48
#define SB_PMIO2_REG49                  0x49
#define SB_PMIO2_REG54                  0x54
#define SB_PMIO2_REG58                  0x58
#define SB_PMIO2_REG59                  0x59
#define SB_PMIO2_REG5A                  0x5A
#define SB_PMIO2_REG5B                  0x5B
#define SB_PMIO2_REG5C                  0x5C
#define SB_PMIO2_REG70                  0x70
#define SB_PMIO2_REGE5                  0xE5

#define SB_SPI_MMIO_REG0C               0x0C            //SPI_Cntrl1 Register


//Bus 0 Device 0x18 Function 0  HyperTransfer
//Link Frequency/Revision Register 0x88/0xA8/0xC8/0xE8 - 32 bits.
#define HT_LINK_REG89           0x89
#define HT_LINK_REGA9           0xA9
#define HT_LINK_REGC9           0xC9
#define HT_LINK_REGE9           0xE9

//Link Type Register 0x98/0xB8/0xD8/0xF8 - 32 bits.
#define HT_LINK_REG98           0x98
#define HT_LINK_REGB8           0xB8
#define HT_LINK_REGD8           0xD8
#define HT_LINK_REGF8           0xF8

//Link Frequency Extension Register 0x9C/0xBC/0xDC/0xFC - 32 bits.
#define HT_LINK_REG9C           0x9C
#define HT_LINK_REGBC           0xBC
#define HT_LINK_REGDC           0xDC
#define HT_LINK_REGFC           0xFC

//DRAM CS Base Address Register D18F2x40/x48/x50/x58
#define DCT_REG40           0x40
#define DCT_REG48           0x48
#define DCT_REG50           0x50
#define DCT_REG58           0x58

//DRAM Configuration Low Register D18F2x90/x91/x92/x93
#define DCT_REG90           0x90
#define DCT_REG91           0x91
#define DCT_REG92           0x92
#define DCT_REG93           0x93

#pragma pack(pop)

#endif          //#ifndef _AMD_SB700_H_
