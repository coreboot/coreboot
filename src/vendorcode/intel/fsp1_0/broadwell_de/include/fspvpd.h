/**

Copyright (C) 2015-2016 Intel Corporation

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
  be used to endorse or promote products derived from this software without
  specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.

**/

#ifndef __FSP_VPD_H__
#define __FSP_VPD_H__

#pragma pack(1)

typedef struct _UPD_DATA_REGION {
  UINT64              Signature;                        /* Offset 0x0000 */
  UINT64              Reserved;                         /* Offset 0x0008 */
  UINT8               UnusedUpdSpace0[16];              /* Offset 0x0010 */
  UINT8               SerialPortType;                   /* Offset 0x0020 */
  UINT32              SerialPortAddress;                /* Offset 0x0021 */
  UINT8               SerialPortConfigure;              /* Offset 0x0025 */
  UINT8               SerialPortBaudRate;               /* Offset 0x0026 */
  UINT8               SerialPortControllerInit0;        /* Offset 0x0027 */
  UINT8               SerialPortControllerInit1;        /* Offset 0x0028 */
  UINT8               ConfigIOU1_PciPort3;              /* Offset 0x0029 */
  UINT8               ConfigIOU2_PciPort1;              /* Offset 0x002A */
  UINT8               PowerStateAfterG3;                /* Offset 0x002B */
  UINT8               PchPciPort1;                      /* Offset 0x002C */
  UINT8               PchPciPort2;                      /* Offset 0x002D */
  UINT8               PchPciPort3;                      /* Offset 0x002E */
  UINT8               PchPciPort4;                      /* Offset 0x002F */
  UINT8               PchPciPort5;                      /* Offset 0x0030 */
  UINT8               PchPciPort6;                      /* Offset 0x0031 */
  UINT8               PchPciPort7;                      /* Offset 0x0032 */
  UINT8               PchPciPort8;                      /* Offset 0x0033 */
  UINT8               HotPlug_PchPciPort1;              /* Offset 0x0034 */
  UINT8               HotPlug_PchPciPort2;              /* Offset 0x0035 */
  UINT8               HotPlug_PchPciPort3;              /* Offset 0x0036 */
  UINT8               HotPlug_PchPciPort4;              /* Offset 0x0037 */
  UINT8               HotPlug_PchPciPort5;              /* Offset 0x0038 */
  UINT8               HotPlug_PchPciPort6;              /* Offset 0x0039 */
  UINT8               HotPlug_PchPciPort7;              /* Offset 0x003A */
  UINT8               HotPlug_PchPciPort8;              /* Offset 0x003B */
  UINT8               Ehci1Enable;                      /* Offset 0x003C */
  UINT8               Ehci2Enable;                      /* Offset 0x003D */
  UINT8               HyperThreading;                   /* Offset 0x003E */
  UINT8               DebugOutputLevel;                 /* Offset 0x003F */
  UINT8               TcoTimerHaltLock;                 /* Offset 0x0040 */
  UINT8               TurboMode;                        /* Offset 0x0041 */
  UINT8               BootPerfMode;                     /* Offset 0x0042 */
  UINT8               PciePort1aAspm;                   /* Offset 0x0043 */
  UINT8               PciePort1bAspm;                   /* Offset 0x0044 */
  UINT8               PciePort3aAspm;                   /* Offset 0x0045 */
  UINT8               PciePort3bAspm;                   /* Offset 0x0046 */
  UINT8               PciePort3cAspm;                   /* Offset 0x0047 */
  UINT8               PciePort3dAspm;                   /* Offset 0x0048 */
  UINT8               PchPciePort1Aspm;                 /* Offset 0x0049 */
  UINT8               PchPciePort2Aspm;                 /* Offset 0x004A */
  UINT8               PchPciePort3Aspm;                 /* Offset 0x004B */
  UINT8               PchPciePort4Aspm;                 /* Offset 0x004C */
  UINT8               PchPciePort5Aspm;                 /* Offset 0x004D */
  UINT8               PchPciePort6Aspm;                 /* Offset 0x004E */
  UINT8               PchPciePort7Aspm;                 /* Offset 0x004F */
  UINT8               PchPciePort8Aspm;                 /* Offset 0x0050 */
  UINT8               DFXEnable;                        /* Offset 0x0051 */
  UINT8               ThermalDeviceEnable;              /* Offset 0x0052 */
  UINT8               UnusedUpdSpace1[88];              /* Offset 0x0053 */
  UINT8               MemEccSupport;                    /* Offset 0x00AB */
  UINT8               MemDdrMemoryType;                 /* Offset 0x00AC */
  UINT8               MemRankMultiplication;            /* Offset 0x00AD */
  UINT8               MemRankMarginTool;                /* Offset 0x00AE */
  UINT8               MemScrambling;                    /* Offset 0x00AF */
  UINT8               MemRefreshMode;                   /* Offset 0x00B0 */
  UINT8               MemMcOdtOverride;                 /* Offset 0x00B1 */
  UINT8               MemCAParity;                      /* Offset 0x00B2 */
  UINT8               MemThermalThrottling;             /* Offset 0x00B3 */
  UINT8               MemPowerSavingsMode;              /* Offset 0x00B4 */
  UINT8               MemElectricalThrottling;          /* Offset 0x00B5 */
  UINT8               MemPagePolicy;                    /* Offset 0x00B6 */
  UINT8               MemSocketInterleaveBelow4G;       /* Offset 0x00B7 */
  UINT8               MemChannelInterleave;             /* Offset 0x00B8 */
  UINT8               MemRankInterleave;                /* Offset 0x00B9 */
  UINT8               MemDownEnable;                    /* Offset 0x00BA */
  UINT32              MemDownCh0Dimm0SpdPtr;            /* Offset 0x00BB */
  UINT32              MemDownCh0Dimm1SpdPtr;            /* Offset 0x00BF */
  UINT32              MemDownCh1Dimm0SpdPtr;            /* Offset 0x00C3 */
  UINT32              MemDownCh1Dimm1SpdPtr;            /* Offset 0x00C7 */
  UINT8               MemFastBoot;                      /* Offset 0x00CB */
  UINT8               pam0_hienable;                    /* Offset 0x00CC */
  UINT8               pam1_loenable;                    /* Offset 0x00CD */
  UINT8               pam1_hienable;                    /* Offset 0x00CE */
  UINT8               pam2_loenable;                    /* Offset 0x00CF */
  UINT8               pam2_hienable;                    /* Offset 0x00D0 */
  UINT8               pam3_loenable;                    /* Offset 0x00D1 */
  UINT8               pam3_hienable;                    /* Offset 0x00D2 */
  UINT8               pam4_loenable;                    /* Offset 0x00D3 */
  UINT8               pam4_hienable;                    /* Offset 0x00D4 */
  UINT8               pam5_loenable;                    /* Offset 0x00D5 */
  UINT8               pam5_hienable;                    /* Offset 0x00D6 */
  UINT8               pam6_loenable;                    /* Offset 0x00D7 */
  UINT8               pam6_hienable;                    /* Offset 0x00D8 */
  UINT8               MemAdr;                           /* Offset 0x00D9 */
  UINT8               MemAdrResumePath;                 /* Offset 0x00DA */
  UINT8               MemBlockScTrafficOnAdr;           /* Offset 0x00DB */
  UINT16              MemPlatformReleaseAdrClampsPort;  /* Offset 0x00DC */
  UINT32              MemPlatformReleaseAdrClampsAnd;   /* Offset 0x00DE */
  UINT32              MemPlatformReleaseAdrClampsOr;    /* Offset 0x00E2 */
  UINT8               UnusedUpdSpace2[24];              /* Offset 0x00E6 */
  UINT16              PcdRegionTerminator;              /* Offset 0x00FE */
} UPD_DATA_REGION;

#define FSP_IMAGE_ID  0x5F45442D5844425F                /* '_BDX-DE_' */
#define FSP_IMAGE_REV 0x00000301

typedef struct _VPD_DATA_REGION {
  UINT64              PcdVpdRegionSign;                 /* Offset 0x0000 */
  UINT32              PcdImageRevision;                 /* Offset 0x0008 */
  UINT32              PcdUpdRegionOffset;               /* Offset 0x000C */
  UINT8               UnusedVpdSpace0[16];              /* Offset 0x0010 */
  UINT32              PcdFspReservedMemoryLength;       /* Offset 0x0020 */
} VPD_DATA_REGION;

#pragma pack()

#endif
