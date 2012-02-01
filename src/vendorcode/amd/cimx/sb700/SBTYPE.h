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

#ifndef _AMD_SBTYPE_H_
#define _AMD_SBTYPE_H_

#pragma pack(push,1)

typedef UINT32 (*CIM_HOOK_ENTRY)(UINT32 Param1, UINTN Param2, void* pConfig);
typedef void (*SMM_SERVICE_ROUTINE) (void);

typedef struct _STDCFG{
        UINT32                  pImageBase;
        UINT32                  pPcieBase;
        UINT8                   Func;
        CIM_HOOK_ENTRY          pCallBack;
        UINT32                  pB2ImageBase;
}STDCFG;                        //Size of stdcfg is 17 bytes

typedef struct _BUILDPARAM
{
        UINT16  BiosSize:3;             //0-1MB, 1-2MB, 2-4MB, 3-8MB, 7-512KB, all other values reserved
        UINT16  LegacyFree:1;
        UINT16  Dummy0:12;

        UINT16  EcKbd:1;
        UINT16  EcChannel0:1;
        UINT16  Dummy1:14;

        UINT32  Smbus0BaseAddress;
        UINT16  Smbus1BaseAddress;
        UINT32  SioPmeBaseAddress;
        UINT32  WatchDogTimerBase;
        UINT32  SpiRomBaseAddress;

        UINT16  AcpiPm1EvtBlkAddr;
        UINT16  AcpiPm1CntBlkAddr;
        UINT16  AcpiPmTmrBlkAddr;
        UINT16  CpuControlBlkAddr;
        UINT16  AcpiGpe0BlkAddr;
        UINT16  SmiCmdPortAddr;
        UINT16  AcpiPmaCntBlkAddr;

        UINT16  EcLdn5MailBoxAddr;
        UINT8   EcLdn5Irq;
        UINT16  EcLdn9MailBoxAddr;
        UINT32  ReservedDword0;
        UINT32  ReservedDword1;
        UINT32  ReservedDword2;
        UINT32  ReservedDword3;

        UINT32  HpetBase;                       //HPET Base address

        UINT32  SataIDESsid;
        UINT32  SataRAIDSsid;
        UINT32  SataRAID5Ssid;
        UINT32  SataAHCISsid;

        UINT32  Ohci0Ssid;
        UINT32  Ohci1Ssid;
        UINT32  Ehci0Ssid;
        UINT32  Ohci2Ssid;
        UINT32  Ohci3Ssid;
        UINT32  Ehci1Ssid;
        UINT32  Ohci4Ssid;
        UINT32  SmbusSsid;
        UINT32  IdeSsid;
        UINT32  AzaliaSsid;
        UINT32  LpcSsid;
        UINT32  P2PSsid;
}BUILDPARAM;

typedef struct _CODECENTRY{
        UINT8 Nid;
        UINT32 Byte40;
}CODECENTRY;

typedef struct _CODECTBLLIST{
        UINT32  CodecID;
        CODECENTRY* CodecTablePtr;
}CODECTBLLIST;

typedef struct _AMDSBCFG
{
        STDCFG  StdHeader;                              //offset 0:16 - 17 bytes
        //UINT32        MsgXchgBiosCimx;                //offset 17:20 - 4 bytes
        UINT32  S3Resume:1;
        UINT32  RebootRequired:1;
        UINT32  Spi33Mhz:1;
        UINT32  SpreadSpectrum:1;
        UINT32  UsbIntClock:1;                          //0:Use external clock, 1:Use internal clock
        UINT32  PciClk5:1;                              //0:disable, 1:enable
        UINT32  TimerClockSource:2;                     //0:100Mhz PCIE Reference clock (same as SB700-A12,
                                                        //1: 14Mhz using 25M_48M_66M_OSC pin, 2: Auto (100Mhz for SB700-A12, 14Mhz
                                                        //using 25M_48m_66m_0SC pin for SB700-A14, SB710, SP5100
        UINT32  ResetCpuOnSyncFlood:1;                  //0:Reset CPU on Sync Flood, 1:Do not reset CPU on sync flood
        UINT32  MsgXchgBiosCimxDummyBB:23;

        /** BuildParameters - The STATIC platform information for CIMx Module. */
        BUILDPARAM BuildParameters;

        //SATA Configuration
        UINT32  SataController  :1;                     //0, 0:disable  1:enable*       //offset 25:28 - 4 bytes
        UINT32  SataClass       :3;                     //1, 0:IDE* 1:RAID      2:AHCI  3:Legacy IDE    4:IDE->AHCI     5:AMD_AHCI, 6:IDE->AMD_AHCI
        UINT32  SataSmbus       :1;                     //4, 0:disable  1:enable*
        UINT32  SataAggrLinkPmCap:1;                    //5, 0:OFF   1:ON
        UINT32  SataPortMultCap :1;                     //6, 0:OFF   1:ON
        UINT32  SataReserved    :2;                     //8:7, Reserved
        UINT32  SataClkAutoOff  :1;                     //9, AutoClockOff for IDE modes 0:Disabled, 1:Enabled
        UINT32  SataIdeCombinedMode     :1;             //10, SataIDECombinedMode 0:Disabled, 1:Enabled
        UINT32  SataIdeCombMdPriSecOpt:1;               //11, Combined Mode, SATA as primary or secondary 0:primary 1:secondary
        UINT32  SataReserved1   :6;                     //17:12, Not used currently
        UINT32  SataEspPort     :6;                     //23:18 SATA port is external accessiable on a signal only connector (eSATA:)
        UINT32  SataClkAutoOffAhciMode:1;               //24: Sata Auto clock off for AHCI mode
        UINT32  SataHpcpButNonESP:6;                    //25:30 Hotplug capable but not e-sata port
        UINT32  SataHideUnusedPort:1;                   //31, 0:Disabled   1:Enabled

        //Flash Configuration                           //offset 29:30 - 2 bytes
        UINT16  FlashController :1;                     //0, 0:disable FC & enable IDE  1:enable FC & disable IDE
        UINT16  FlashControllerMode:1;                  //1, 0:Flash behind SATA        1:Flash as standalone
        UINT16  FlashHcCrc:1;                           //2,
        UINT16  FlashErrorMode:1;                       //3
        UINT16  FlashNumOfBankMode:1;                   //4
        UINT16  FlashDummy:11;                          //5:15

        //USB Configuration                             //offset 31:32 - 2 bytes
        UINT16  Usb1Ohci0       :1;                     //0, 0:disable  1:enable*       Bus 0 Dev 18 Func0
        UINT16  Usb1Ohci1       :1;                     //1, 0:disable  1:enable*       Bus 0 Dev 18 Func1
        UINT16  Usb1Ehci        :1;                     //2, 0:disable  1:enable*       Bus 0 Dev 18 Func2
        UINT16  Usb2Ohci0       :1;                     //3, 0:disable  1:enable*       Bus 0 Dev 19 Func0
        UINT16  Usb2Ohci1       :1;                     //4, 0:disable  1:enable*       Bus 0 Dev 19 Func1
        UINT16  Usb2Ehci        :1;                     //5, 0:disable  1:enable*       Bus 0 Dev 19 Func2
        UINT16  Usb3Ohci        :1;                     //6, 0:disable  1:enable*       Bus 0 Dev 20 Func5
        UINT16  UsbOhciLegacyEmulation:1;               //7, 0:Enabled, 1:Disabled
        UINT16  UsbDummy        :8;                     //8:15

        //Azalia Configuration                          //offset 33:36 - 4 bytes
        UINT32  AzaliaController:2;                     //0, 0:AUTO, 1:disable, 2:enable
        UINT32  AzaliaPinCfg    :1;                     //2, 0:disable, 1:enable
        UINT32  AzaliaFrontPanel:2;                     //3, 0:AUTO, 1:disable, 2:enable
        UINT32  FrontPanelDetected:1;                   //5, 0:Not detected, 1:detected
        UINT32  AzaliaSdin0     :2;                     //6
        UINT32  AzaliaSdin1     :2;                     //8
        UINT32  AzaliaSdin2     :2;                     //10
        UINT32  AzaliaSdin3     :2;                     //12
        UINT32  AzaliaDummy     :18;                    //14:31

        CODECTBLLIST*   pAzaliaOemCodecTablePtr;        //offset 37:40 - 4 bytes
        UINT32  pAzaliaOemFpCodecTableptr;              //offset 41:44 - 4 bytes

        //Miscellaneous Configuration                   //offset 45:48 - 4 bytes
        UINT32  MiscReserved0:1;                        //0
        UINT32  HpetTimer:1;                            //1, 0:disable  1:enable
        UINT32  PciClks:5;                              //2:6, 0:disable, 1:enable
        UINT32  MiscReserved1:3;                        //9:7, Reserved
        UINT32  IdeController:1;                        //10, 0:Enable, 1:Disabled
        UINT32  MobilePowerSavings:1;                   //11, 0:Disable, 1:Enable       Power saving features especially for Mobile platform
        UINT32  ExternalRTCClock:1;                     //12, 0:Don't Shut Off, 1:Shut Off, external RTC clock
        UINT32  AcpiS1Supported:1;                      //13, 0:S1 not supported, 1:S1 supported
        UINT32  AnyHT200MhzLink:1;                      //14, 0:No HT 200Mhz Link in platform, 1; There is 200MHz HT Link in platform
        UINT32  WatchDogTimerEnable:1;                  //15, [0]: WDT disabled; 1: WDT enabled
        UINT32  MTC1e:1;                                //16, Message Triggered C1e - 0:Disabled*, 1:Enabled
        UINT32  HpetMsiDis:1;                           //17, HPET MSI - 0:Enable HPET MSI, 1:Disable
                UINT32  EhciDataCacheDis:1;                     //18, 0:Date Cache Enabled, 1:Date Cache Disabled    /** EHCI Async Data Cache Disable */
        UINT32  MiscDummy:13;

        UINT32  AsmAslInfoExchange0;                    //offset 49:52 - 4 bytes
        UINT32  AsmAslInfoExchange1;                    //offset 53:56

        //DebugOptions_1                                //offset 57:60
        UINT32  FlashPinConfig  :1;                     //0, 0:desktop mode     1:mobile mode
        UINT32  UsbPhyPowerDown :1;                     //1
        UINT32  PcibClkStopOverride     :10;            //11:2
        UINT32  Debug1Reserved0:4;                      //15:11
        UINT32  AzaliaSnoop:1;                          //16 0:Disable, 1:Enable
        UINT32  SataSscPscCap:1;                        //17, 0:Enable SSC/PSC capability, 1:Disable SSC/PSC capability
        UINT32  SataPortMode:6;                         //23:18, 0: AUTO, 1:Force SATA port(6/5/4/3/2/1) to GEN1
        UINT32  SataPhyWorkaround:2;                    //25:24, 0:AUTO, 1:Enable, 2:Disable
        UINT32  Gen1DeviceShutdownDuringPhyWrknd:2;     //27:26, 0:AUTO, 1:YES, 2:NO
        UINT32  OhciIsoOutPrefetchDis:1;                //28, 0:Enable OHCI ISO OUT prefetch, 1:Disable
        UINT32  Debug1Dummy:3;                          //

        //DebugOptions_2
        UINT32  PcibAutoClkCtrlLow:16;
        UINT32  PcibAutoClkCtrlHigh:16;

        //TempMMIO
        UINT32  TempMMIO:32;

}AMDSBCFG;

typedef struct  _SMMSERVICESTRUC
{
        UINT8   enableRegNum;
        UINT8   enableBit;
        UINT8   statusRegNum;
        UINT8   statusBit;
        CHAR8   *debugMessage;
        SMM_SERVICE_ROUTINE     serviceRoutine;
}SMMSERVICESTRUC;

typedef struct _ABTblEntry
{
        UINT8   regType;
        UINT32  regIndex;
        UINT32  regMask;
        UINT32  regData;
}ABTBLENTRY;

#define PCI_ADDRESS(bus,dev,func,reg) \
(UINT32) ( (((UINT32)bus) << 24) + (((UINT32)dev) << 19) + (((UINT32)func) << 16) + ((UINT32)reg) )

typedef UINT32  CIM_STATUS;
#define CIM_SUCCESS 0x00000000
#define CIM_ERROR       0x80000000
#define CIM_UNSUPPORTED 0x80000001

#pragma pack(pop)

#define CIMX_OPTION_DISABLED	0
#define CIMX_OPTION_ENABLED	1

#endif // _AMD_SBTYPE_H_
