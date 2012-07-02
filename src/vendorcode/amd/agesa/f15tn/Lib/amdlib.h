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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
  VOID);

// MSR
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

VOID
LibAmdFinit (
  VOID);

VOID
StopHere (
  VOID
  );

#endif // _AMD_LIB_H_
