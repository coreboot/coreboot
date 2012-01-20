/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 **/

#ifndef _AMD_LIB_H_
#define _AMD_LIB_H_

#define IOCF8 0xCF8
#define IOCFC 0xCFC

// Reg Values for ReadCpuReg and WriteCpuReg
#define    CR4_REG  0x04
#define    DR0_REG  0x10
#define    DR1_REG  0x11
#define    DR2_REG  0x12
#define    DR3_REG  0x13
#define    DR7_REG  0x17

// PROTOTYPES FOR amdlib32.asm
UINT8
ReadIo8 (
  IN       UINT16 Address
  );

UINT16
ReadIo16 (
  IN       UINT16 Address
  );

UINT32
ReadIo32 (
  IN       UINT16 Address
  );

VOID
WriteIo8 (
  IN       UINT16 Address,
  IN       UINT8 Data
  );

VOID
WriteIo16 (
  IN       UINT16 Address,
  IN       UINT16 Data
  );

VOID
WriteIo32 (
  IN       UINT16 Address,
  IN       UINT32 Data
  );

UINT8
Read64Mem8 (
  IN       UINT64 Address
  );

UINT16
Read64Mem16 (
  IN       UINT64 Address
  );

UINT32
Read64Mem32 (
  IN       UINT64 Address
  );

VOID
Write64Mem8 (
  IN       UINT64 Address,
  IN       UINT8 Data
  );

VOID
Write64Mem16 (
  IN       UINT64 Address,
  IN       UINT16 Data
  );

VOID
Write64Mem32 (
  IN       UINT64 Address,
  IN       UINT32 Data
  );

UINT64
ReadTSC (
  VOID
  );

// MSR

UINT64
MsrRead (
  IN       UINT32 MsrAddress
  );

VOID
MsrWrite (
  IN       UINT32 MsrAddress,
  IN			 UINT64 Value
  );

VOID
LibAmdMsrRead (
  IN       UINT32 MsrAddress,
     OUT   UINT64 *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMsrWrite (
  IN       UINT32 MsrAddress,
  IN       UINT64 *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

// IO
VOID
LibAmdIoRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdIoWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdIoRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdIoPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

// Memory or MMIO
VOID
LibAmdMemRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

// PCI
VOID
LibAmdPciRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
  IN       VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciReadBits (
  IN       PCI_ADDR Address,
  IN       UINT8 Highbit,
  IN       UINT8 Lowbit,
     OUT   UINT32 *Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciWriteBits (
  IN       PCI_ADDR Address,
  IN       UINT8 Highbit,
  IN       UINT8 Lowbit,
  IN       UINT32 *Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdPciFindNextCap (
  IN OUT   PCI_ADDR *Address,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

// CPUID
VOID
LibAmdCpuidRead (
  IN       UINT32 CpuidFcnAddress,
     OUT   CPUID_DATA *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

// Utility Functions
VOID
LibAmdMemFill (
  IN       VOID *Destination,
  IN       UINT8 Value,
  IN       UINTN FillLength,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LibAmdMemCopy (
  IN       VOID *Destination,
  IN       VOID *Source,
  IN       UINTN CopyLength,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

VOID *
LibAmdLocateImage (
  IN       VOID *StartAddress,
  IN       VOID *EndAddress,
  IN       UINT32 Alignment,
  IN       CHAR8 ModuleSignature[8]
  );

UINT32
LibAmdGetPackageType (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
LibAmdVerifyImageChecksum (
  IN       VOID *ImagePtr
  );

UINT8
LibAmdBitScanReverse (
  IN       UINT32 value
  );
UINT8
LibAmdBitScanForward (
  IN       UINT32 value
  );

VOID
LibAmdReadCpuReg (
  IN       UINT8 RegNum,
     OUT   UINT32 *Value
  );
VOID
LibAmdWriteCpuReg (
  IN       UINT8 RegNum,
  IN       UINT32 Value
  );

VOID
LibAmdWriteBackInvalidateCache (
  IN       VOID
  );

VOID
LibAmdSimNowEnterDebugger (VOID);

VOID
LibAmdHDTBreakPoint (VOID);

UINT8
LibAmdAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  );

VOID
LibAmdCLFlush (
  IN       UINT64 Address,
  IN       UINT8  Count
  );

VOID F10RevDProbeFilterCritical (
  IN       PCI_ADDR PciAddress,
  IN       UINT32   PciRegister
  );
VOID
IdsOutPort (
  IN       UINT32 Addr,
  IN       UINT32 Value,
  IN       UINT32 Flag
  );

VOID
StopHere (
  VOID
  );

VOID
CpuidRead (
  IN        UINT32      CpuidFcnAddress,
  OUT       CPUID_DATA  *Value
  );

UINT8
ReadNumberOfCpuCores(
  VOID
  );

#endif // _AMD_LIB_H_
