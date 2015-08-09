/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2015, Advanced Micro Devices, Inc.
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
 ***************************************************************************/

#ifndef _PSP_BASE_LIB_H_
#define _PSP_BASE_LIB_H_

#include "AMD.h"
#include "amdlib.h"
#include "PspDirectory.h"

#define PSP_PCI_SEG        0x00    ///< PSP Seg address
#define PSP_PCI_BUS        0x00    ///< PSP Bus address
#define PSP_PCI_DEV        0x08    ///< PSP Device address
#define PSP_PCI_FN         0x00    ///< PSP Fn address
#define PSP_PCI_BDA        ((PSP_PCI_DEV << 11) + (PSP_PCI_FN << 8))
#define GET_PSP_PCI_ADDR (Offset)    MAKE_SBDFO (PSP_PCI_SEG, PSP_PCI_BUS, PSP_PCI_DEV, PSP_PCI_FN, Offset)

#define PSP_PCI_DEVID_REG           0x00    ///< DevId
#define PSP_PCI_CMD_REG             0x04    ///< CmdReg
#define PSP_PCI_BAR1_REG            0x18    ///< Pci Bar1
#define PSP_PCI_BAR3_REG            0x20    ///< Pci Bar3
#define PSP_PCI_MIRRORCTRL1_REG     0x44    ///< PSP Mirror Reg Ctrl 1
#define PSP_PCI_EXTRAPCIHDR_REG     0x48    ///< Extra PCI Header Ctr
#define PSP_PCI_HTMSICAP_REG        0x5C    ///<  HT MSI Capability

#define D8F0x44_PmNxtPtrW_MASK                                  0xff

#define PSP_MAILBOX_BASE            0x70    ///< Mailbox base offset on PCIe BAR
#define PSP_MAILBOX_STATUS_OFFSET   0x4     ///< Staus Offset

#define PMIO_INDEX_PORT        0xCD6    ///Pmio index port
#define PMIO_DATA_PORT         0xCD7    ///Pmio data port

#define PMIO_REG62             0x62    ///PMIOx62

//======================================================================================
//
// Define Mailbox Status field
//
//======================================================================================
//
/// MBox Status MMIO space
///
typedef struct {
  UINT32 MboxInitialized:1;   ///< Target will set this to 1 to indicate it is initialized  (for ex. PSP/TPM ready)
  UINT32 Error:1;             ///< Target in adddtion to Done bit will also set this bit to indicate success/error on last command
  UINT32 Terminated:1;        ///< Target will set this bit if it aborted the command due to abort request
  UINT32 Halt:1;              ///< Target will set this error if there is critical error that require reset etc
  UINT32 Recovery:1;          ///< Target will set this error if some PSP entry point by PSP directory has been corrupted.
  UINT32 Reserved:27;          ///< Reserved
} MBOX_STATUS;

//
// Above defined as bitmap
#define MBOX_STATUS_INITIALIZED       0x00000001ul    ///< Mailbox Status: Initialized
#define MBOX_STATUS_ERROR             0x00000002ul    ///< Mailbox Status: Error
#define MBOX_STATUS_ABORT             0x00000004ul    ///< Mailbox Status: Abort
#define MBOX_STATUS_HALT              0x00000008ul    ///< Mailbox Status: Halt
#define MBOX_STATUS_RECOVERY          0x00000010ul    ///< Mailbox Status: Recovery required



UINT32
PspLibPciReadConfig (
  IN  UINT32 Register
  );

VOID
PspLibPciWriteConfig (
  IN  UINT32 Register,
  IN  UINT32 Value
  );

UINT32
PspLibPciReadPspConfig (
  IN UINT16 Offset
  );

VOID
PspLibPciWritePspConfig (
  IN UINT16 Offset,
  IN  UINT32 Value
  );

BOOLEAN
GetPspDirBase (
  IN OUT   UINT32     *Address
);

BOOLEAN
PSPEntryInfo (
  IN      PSP_DIRECTORY_ENTRY_TYPE    EntryType,
  IN OUT  UINT64                      *EntryAddress,
  IN      UINT32                      *EntrySize
  );

BOOLEAN
CheckPspDevicePresent (
  VOID
  );

BOOLEAN
CheckPspPlatformSecureEnable (
  VOID
  );

BOOLEAN
CheckPspRecoveryFlag (
  VOID
  );

BOOLEAN
GetPspBar1Addr (
  IN OUT   UINT32 *PspMmio
  );

BOOLEAN
GetPspBar3Addr (
  IN OUT   UINT32 *PspMmio
  );


BOOLEAN
GetPspMboxStatus (
  IN OUT   MBOX_STATUS **MboxStatus
  );


BOOLEAN
PspBarInitEarly (VOID);

VOID
PspLibPciIndirectRead (
  IN       PCI_ADDR     Address,
  IN       UINT32       IndirectAddress,
  IN       ACCESS_WIDTH Width,
     OUT   VOID         *Value
  );

VOID
PspLibPciIndirectWrite (
  IN      PCI_ADDR      Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value
  );

UINT8
PspLibAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  );

BOOLEAN
IsS3Resume (VOID);

#endif // _AMD_LIB_H_
