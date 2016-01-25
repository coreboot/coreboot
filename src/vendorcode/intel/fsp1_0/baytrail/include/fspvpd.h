/**

Copyright (C) 2013-2015 Intel Corporation

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

#ifndef __FSPVPD_H__
#define __FSPVPD_H__

#pragma pack(1)

typedef struct {
  UINT8    EnableMemoryDown;
  UINT8    DRAMSpeed;      /* DRAM Speed  */
  UINT8    DRAMType;       /* DRAM Type */
  UINT8    DIMM0Enable;    /* DIMM 0 Enable */
  UINT8    DIMM1Enable;    /* DIMM 1 Enable */
  UINT8    DIMMDWidth;     /* DRAM device data width */
  UINT8    DIMMDensity;    /* DRAM device data density */
  UINT8    DIMMBusWidth;   /* DIMM Bus Width */
  UINT8    DIMMSides;      /* Ranks Per DIMM */
  UINT8    DIMMtCL;        /* tCL */
  UINT8    DIMMtRPtRCD;    /* tRP and tRCD in DRAM clk - 5:12.5ns, 6:15ns, etc. */
  UINT8    DIMMtWR;        /* tWR in DRAM clk  */
  UINT8    DIMMtWTR;       /* tWTR in DRAM clk */
  UINT8    DIMMtRRD;       /* tRRD in DRAM clk */
  UINT8    DIMMtRTP;       /* tRTP in DRAM clk */
  UINT8    DIMMtFAW;       /* tFAW in DRAM clk */
} MEMORY_DOWN_DATA;


typedef struct _UPD_DATA_REGION {
  UINT64                      Signature;                     /* Offset 0x0000 */
  UINT32                      RESERVED1;                     /* Offset 0x0008 */
  UINT8                       Padding0[20];                  /* Offset 0x000C */
  UINT16                      PcdMrcInitTsegSize;            /* Offset 0x0020 */
  UINT16                      PcdMrcInitMmioSize;            /* Offset 0x0022 */
  UINT8                       PcdMrcInitSPDAddr1;            /* Offset 0x0024 */
  UINT8                       PcdMrcInitSPDAddr2;            /* Offset 0x0025 */
  UINT8                       PcdeMMCBootMode;               /* Offset 0x0026 */
  UINT8                       PcdEnableSdio;                 /* Offset 0x0027 */
  UINT8                       PcdEnableSdcard;               /* Offset 0x0028 */
  UINT8                       PcdEnableHsuart0;              /* Offset 0x0029 */
  UINT8                       PcdEnableHsuart1;              /* Offset 0x002A */
  UINT8                       PcdEnableSpi;                  /* Offset 0x002B */
  UINT8                       ReservedUpdSpace1;             /* Offset 0x002C */
  UINT8                       PcdEnableSata;                 /* Offset 0x002D */
  UINT8                       PcdSataMode;                   /* Offset 0x002E */
  UINT8                       PcdEnableAzalia;               /* Offset 0x002F */
  UINT32                      AzaliaConfigPtr;               /* Offset 0x0030 */
  UINT8                       PcdEnableXhci;                 /* Offset 0x0034 */
  UINT8                       PcdEnableLpe;                  /* Offset 0x0035 */
  UINT8                       PcdLpssSioEnablePciMode;       /* Offset 0x0036 */
  UINT8                       PcdEnableDma0;                 /* Offset 0x0037 */
  UINT8                       PcdEnableDma1;                 /* Offset 0x0038 */
  UINT8                       PcdEnableI2C0;                 /* Offset 0x0039 */
  UINT8                       PcdEnableI2C1;                 /* Offset 0x003A */
  UINT8                       PcdEnableI2C2;                 /* Offset 0x003B */
  UINT8                       PcdEnableI2C3;                 /* Offset 0x003C */
  UINT8                       PcdEnableI2C4;                 /* Offset 0x003D */
  UINT8                       PcdEnableI2C5;                 /* Offset 0x003E */
  UINT8                       PcdEnableI2C6;                 /* Offset 0x003F */
  UINT8                       PcdEnablePwm0;                 /* Offset 0x0040 */
  UINT8                       PcdEnablePwm1;                 /* Offset 0x0041 */
  UINT8                       PcdEnableHsi;                  /* Offset 0x0042 */
  UINT8                       PcdIgdDvmt50PreAlloc;          /* Offset 0x0043 */
  UINT8                       PcdApertureSize;               /* Offset 0x0044 */
  UINT8                       PcdGttSize;                    /* Offset 0x0045 */
  UINT32                      SerialDebugPortAddress;        /* Offset 0x0046 */
  UINT8                       SerialDebugPortType;           /* Offset 0x004A */
  UINT8                       PcdMrcDebugMsg;                /* Offset 0x004B */
  UINT8                       ISPEnable;                     /* Offset 0x004C */
  UINT8                       PcdSccEnablePciMode;           /* Offset 0x004D */
  UINT8                       IgdRenderStandby;              /* Offset 0x004E */
  UINT8                       TxeUmaEnable;                  /* Offset 0x004F */
  UINT8                       PcdOsSelection;                /* Offset 0x0050 */
  UINT8                       PcdEMMC45DDR50Enabled;         /* Offset 0x0051 */
  UINT8                       PcdEMMC45HS200Enabled;         /* Offset 0x0052 */
  UINT8                       PcdEMMC45RetuneTimerValue;     /* Offset 0x0053 */
  UINT8                       PcdEnableIgd;                  /* Offset 0x0054 */
  UINT8                       UnusedUpdSpace1[155];          /* Offset 0x0055 */
  MEMORY_DOWN_DATA            PcdMemoryParameters;           /* Offset 0x00F0 */
  UINT16                      PcdRegionTerminator;           /* Offset 0x0100 */
} UPD_DATA_REGION;


typedef struct _VPD_DATA_REGION {
  UINT64   PcdVpdRegionSign;                      /* Offset 0x0000 */
  UINT32   PcdImageRevision;                      /* Offset 0x0008 */
  UINT32   PcdUpdRegionOffset;                    /* Offset 0x000C */
  UINT8    Padding0[16];                          /* Offset 0x0010 */
  UINT32   PcdFspReservedMemoryLength;            /* Offset 0x0020 */
  UINT8    PcdPlatformType;                       /* Offset 0x0024 */
  UINT8    PcdEnableSecureBoot;                   /* Offset 0x0025 */
  UINT8    PcdMemoryParameters[16];               /* Offset 0x0026 */
} VPD_DATA_REGION;

#pragma pack()

#endif
