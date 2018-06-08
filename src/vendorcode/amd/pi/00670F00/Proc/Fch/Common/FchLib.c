/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH IO access common routine
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: FCH
 * @e \$Revision$   @e \$Date$
 *
 */
 /*****************************************************************************
 *
 * Copyright (c) 2008 - 2016, Advanced Micro Devices, Inc.
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
#include "FchPlatform.h"
#define FILECODE PROC_FCH_COMMON_FCHLIB_FILECODE

/**< FchStall - Reserved  */
VOID
FchStall (
  IN       UINT32              uSec,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16 timerAddr;
  UINT32 startTime;
  UINT32 elapsedTime;

  LibAmdMemRead (AccessWidth16, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG64), &timerAddr, StdHeader);
  if ( timerAddr == 0 ) {
    uSec = uSec / 2;
    while ( uSec != 0 ) {
      LibAmdIoRead (AccessWidth8, FCHOEM_IO_DELAY_PORT, (UINT8 *) (&startTime), StdHeader);
      uSec--;
    }
  } else {
    LibAmdIoRead (AccessWidth32, timerAddr, &startTime, StdHeader);
    for ( ;; ) {
      LibAmdIoRead (AccessWidth32, timerAddr, &elapsedTime, StdHeader);
      if ( elapsedTime < startTime ) {
        elapsedTime = elapsedTime + FCH_MAX_TIMER - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }
      if ( (elapsedTime * FCHOEM_ELAPSED_TIME_UNIT / FCHOEM_ELAPSED_TIME_DIVIDER) > uSec ) {
        break;
      }
    }
  }
}

#if IS_ENABLED(CONFIG_VENDORCODE_FULL_SUPPORT)
/**< cimFchStall - Reserved  */
VOID
CimFchStall (
  IN       UINT32              uSec,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16 timerAddr;
  UINT32 startTime;
  UINT32 elapsedTime;

  LibAmdMemRead (AccessWidth16, (UINT64) (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG64), &timerAddr, StdHeader);
  if ( timerAddr == 0 ) {
    uSec = uSec / 2;
    while ( uSec != 0 ) {
      LibAmdIoRead (AccessWidth8, FCHOEM_IO_DELAY_PORT, (UINT8*)&elapsedTime, StdHeader);
      uSec--;
    }
  } else {
    LibAmdIoRead (AccessWidth32, timerAddr, &startTime, StdHeader);
    for ( ;; ) {
      LibAmdIoRead (AccessWidth32, timerAddr, &elapsedTime, StdHeader);
      if ( elapsedTime < startTime ) {
        elapsedTime = elapsedTime + FCH_MAX_TIMER - startTime;
      } else {
        elapsedTime = elapsedTime - startTime;
      }
      if ( (elapsedTime * FCHOEM_ELAPSED_TIME_UNIT / FCHOEM_ELAPSED_TIME_DIVIDER) > uSec ) {
        break;
      }
    }
  }
}

/**< FchReset - Reserved  */
VOID
FchPciReset (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8   PciRstValue;

  PciRstValue = 0x06;
  LibAmdIoWrite (AccessWidth8, FCH_PCIRST_BASE_IO, &PciRstValue, StdHeader);
}

/**< outPort80 - Reserved  */
VOID
OutPort80 (
  IN       UINT32              pcode,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth8, FCHOEM_OUTPUT_DEBUG_PORT, &pcode, StdHeader);
  return;
}

/**< outPort1080 - Reserved  */
VOID
OutPort1080 (
  IN       UINT32              pcode,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth32, 0x1080, &pcode, StdHeader);
  return;
}

/**< FchCopyMem - Reserved  */
VOID
FchCopyMem (
  IN      VOID*   pDest,
  IN      VOID*   pSource,
  IN      UINTN   Length
  )
{
  UINTN  i;
  UINT8  *Ptr;
  UINT8  *Source;
  Ptr = (UINT8*)pDest;
  Source = (UINT8*)pSource;
  for (i = 0; i < Length; i++) {
    *Ptr = *Source;
    Source++;
    Ptr++;
  }
}

/** GetRomSigPtr - Reserved **/
VOID*
GetRomSigPtr (
  IN      UINTN               *RomSigPtr,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32   RomPtr;
  UINT32   RomSig;
  UINT16   MswAddr;

  *RomSigPtr = 0;
  MswAddr = 0xFF00;
  do {
    RomPtr = (MswAddr << 16) + FCH_ROMSIG_BASE_IO;
    LibAmdMemRead (AccessWidth32, (UINT64) RomPtr, &RomSig, StdHeader);
    if (RomSig == FCH_ROMSIG_SIGNATURE) {
      *RomSigPtr = RomPtr;
      break;
    }
    MswAddr >>= 1;
    MswAddr |= BIT15;
  } while (MswAddr != 0xFFF8);
  return RomSigPtr;
}

/** RwXhciIndReg - Reserved **/
VOID
RwXhciIndReg (
  IN       UINT32              Index,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RevReg;
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = (USB_XHCI_BUS_DEV_FUN << 12) + 0x48;
  LibAmdPciWrite (AccessWidth32, PciAddress, &Index, StdHeader);
  PciAddress.AddressValue = (USB_XHCI_BUS_DEV_FUN << 12) + 0x4C;
  RevReg = ~AndMask;
  LibAmdPciRMW (AccessWidth32, PciAddress, &OrMask, &RevReg, StdHeader);

  PciAddress.AddressValue = (USB_XHCI1_BUS_DEV_FUN << 12) + 0x48;
  LibAmdPciWrite (AccessWidth32, PciAddress, &Index, StdHeader);
  PciAddress.AddressValue = (USB_XHCI1_BUS_DEV_FUN << 12) + 0x4C;
  RevReg = ~AndMask;
  LibAmdPciRMW (AccessWidth32, PciAddress, &OrMask, &RevReg, StdHeader);
}

/** RwXhci0IndReg - Reserved **/
VOID
RwXhci0IndReg (
  IN       UINT32              Index,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RevReg;
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = (USB_XHCI_BUS_DEV_FUN << 12) + 0x48;
  LibAmdPciWrite (AccessWidth32, PciAddress, &Index, StdHeader);
  PciAddress.AddressValue = (USB_XHCI_BUS_DEV_FUN << 12) + 0x4C;
  RevReg = ~AndMask;
  LibAmdPciRMW (AccessWidth32, PciAddress, &OrMask, &RevReg, StdHeader);
}

/** RwXhci1IndReg - Reserved **/
VOID
RwXhci1IndReg (
  IN       UINT32              Index,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RevReg;
  PCI_ADDR  PciAddress;

  PciAddress.AddressValue = (USB_XHCI1_BUS_DEV_FUN << 12) + 0x48;
  LibAmdPciWrite (AccessWidth32, PciAddress, &Index, StdHeader);
  PciAddress.AddressValue = (USB_XHCI1_BUS_DEV_FUN << 12) + 0x4C;
  RevReg = ~AndMask;
  LibAmdPciRMW (AccessWidth32, PciAddress, &OrMask, &RevReg, StdHeader);
}

/** ReadXhci0Phy - Reserved **/
VOID
ReadXhci0Phy (
  IN       UINT32              Port,
  IN       UINT32              Address,
  IN       UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegIndex;
  UINT32    RegValue;
  PCI_ADDR  PciAddress;

  RwXhci0IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + Address, StdHeader);

  RegIndex = FCH_XHCI_IND60_REG04;
  PciAddress.AddressValue = (USB_XHCI_BUS_DEV_FUN << 12) + 0x48;
  LibAmdPciWrite (AccessWidth32, PciAddress, &RegIndex, StdHeader);
  PciAddress.AddressValue = (USB_XHCI_BUS_DEV_FUN << 12) + 0x4C;
  LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);

  (*Value) = ( RegValue >> (Port * 8)) & 0x000000FF;
}

/** ReadXhci1Phy - Reserved **/
VOID
ReadXhci1Phy (
  IN       UINT32              Port,
  IN       UINT32              Address,
  IN       UINT32              *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32    RegIndex;
  UINT32    RegValue;
  PCI_ADDR  PciAddress;

  RwXhci1IndReg ( FCH_XHCI_IND60_REG00, 0xFFFFC000, (Port << 13) + BIT12 + Address, StdHeader);

  RegIndex = FCH_XHCI_IND60_REG04;
  PciAddress.AddressValue = (USB_XHCI1_BUS_DEV_FUN << 12) + 0x48;
  LibAmdPciWrite (AccessWidth32, PciAddress, &RegIndex, StdHeader);
  PciAddress.AddressValue = (USB_XHCI1_BUS_DEV_FUN << 12) + 0x4C;
  LibAmdPciRead (AccessWidth32, PciAddress, &RegValue, StdHeader);

  (*Value) = ( RegValue >> (Port * 8)) & 0x000000FF;
}

/** AcLossControl - Reserved **/
VOID
AcLossControl (
  IN       UINT8 AcLossControlValue
  )
{
  AcLossControlValue &= 0x03;
  AcLossControlValue |= BIT2;
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG5B, AccessWidth8, 0xF0, AcLossControlValue);
}

/** RecordFchConfigPtr - Reserved **/
VOID
RecordFchConfigPtr (
  IN       UINT32 FchConfigPtr
  )
{
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x08, AccessWidth8, 0, (UINT8) ((FchConfigPtr >> 0) & 0xFF) );
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x09, AccessWidth8, 0, (UINT8) ((FchConfigPtr >> 8) & 0xFF) );
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0A, AccessWidth8, 0, (UINT8) ((FchConfigPtr >> 16) & 0xFF) );
  RwMem (ACPI_MMIO_BASE + CMOS_RAM_BASE + 0x0B, AccessWidth8, 0, (UINT8) ((FchConfigPtr >> 24) & 0xFF) );
}

/** ReadAlink - Reserved **/
UINT32
ReadAlink (
  IN       UINT32              Index,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 Data;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
  LibAmdIoRead (AccessWidth32, ALINK_ACCESS_DATA, &Data, StdHeader);
  //Clear Index
  Index = 0;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
  return Data;
}

/** WriteAlink - Reserved **/
VOID
WriteAlink (
  IN       UINT32              Index,
  IN       UINT32              Data,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_DATA, &Data, StdHeader);
  //Clear Index
  Index = 0;
  LibAmdIoWrite (AccessWidth32, ALINK_ACCESS_INDEX, &Index, StdHeader);
}

/** RwAlink - Reserved **/
VOID
RwAlink (
  IN       UINT32              Index,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 AccessType;

  AccessType = Index & 0xE0000000;
  if (AccessType == (AXINDC << 29)) {
    WriteAlink ((FCH_AX_INDXC_REG30 | AccessType), Index & 0x1FFFFFFF, StdHeader);
    Index = FCH_AX_DATAC_REG34 | AccessType;
  } else if (AccessType == (AXINDP << 29)) {
    WriteAlink ((FCH_AX_INDXP_REG38 | AccessType), Index & 0x1FFFFFFF, StdHeader);
    Index = FCH_AX_DATAP_REG3C | AccessType;
  }
  WriteAlink (Index, (ReadAlink (Index, StdHeader) & AndMask) | OrMask, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read PMIO
 *
 *
 *
 * @param[in] Address  - PMIO Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Read Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
ReadPmio (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    i;

  OpFlag = OpFlag & 0x7f;
  OpFlag = 1 << (OpFlag - 1);
  for (i = 0; i < OpFlag; i++) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD6, &Address, StdHeader);
    Address++;
    LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGCD7, (UINT8 *)Value + i, StdHeader);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PMIO
 *
 *
 *
 * @param[in] Address  - PMIO Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Write Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
WritePmio (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    i;

  OpFlag = OpFlag & 0x7f;
  OpFlag = 1 << (OpFlag - 1);
  for (i = 0; i < OpFlag; i++) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD6, &Address, StdHeader);
    Address++;
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD7, (UINT8 *)Value + i, StdHeader);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * RwPmio - Read/Write PMIO
 *
 *
 *
 * @param[in] Address    - PMIO Offset value
 * @param[in] OpFlag     - Access sizes
 * @param[in] AndMask    - Data And Mask 32 bits
 * @param[in] OrMask     - Data OR Mask 32 bits
 * @param[in] StdHeader
 *
 */
VOID
RwPmio (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 Result;

  ReadPmio (Address, OpFlag, &Result, StdHeader);
  Result = (Result & AndMask) | OrMask;
  WritePmio (Address, OpFlag, &Result, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read PMIO2
 *
 *
 *
 * @param[in] Address  - PMIO2 Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Read Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
ReadPmio2 (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8 i;

  OpFlag = OpFlag & 0x7f;
  OpFlag = 1 << (OpFlag - 1);
  for ( i = 0; i < OpFlag; i++ ) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD0, &Address, StdHeader);
    Address++;
    LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGCD1, (UINT8 *) Value + i, StdHeader);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Write PMIO 2
 *
 *
 *
 * @param[in] Address  - PMIO2 Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Write Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
WritePmio2 (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8 i;

  OpFlag = OpFlag & 0x7f;
  OpFlag = 1 << (OpFlag - 1);

  for ( i = 0; i < OpFlag; i++ ) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD0, &Address, StdHeader);
    Address++;
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD1, (UINT8 *) Value + i, StdHeader);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * RwPmio2 - Read/Write PMIO2
 *
 *
 *
 * @param[in] Address    - PMIO2 Offset value
 * @param[in] OpFlag     - Access sizes
 * @param[in] AndMask    - Data And Mask 32 bits
 * @param[in] OrMask     - Data OR Mask 32 bits
 * @param[in] StdHeader
 *
 */
VOID
RwPmio2 (
  IN       UINT8 Address,
  IN       UINT8  OpFlag,
  IN       UINT32 AndMask,
  IN       UINT32 OrMask,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32 Result;

  ReadPmio2 (Address, OpFlag, &Result, StdHeader);
  Result = (Result & AndMask) | OrMask;
  WritePmio2 (Address, OpFlag, &Result, StdHeader);
}


/*----------------------------------------------------------------------------------------*/
/**
 * Read BIOSRAM
 *
 *
 *
 * @param[in] Address  - BIOSRAM Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Read Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
ReadBiosram (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    i;

  OpFlag = OpFlag & 0x7f;
  OpFlag = 1 << (OpFlag - 1);
  for (i = 0; i < OpFlag; i++) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD4, &Address, StdHeader);
    Address++;
    LibAmdIoRead (AccessWidth8, FCH_IOMAP_REGCD5, (UINT8 *)Value + i, StdHeader);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write BIOSRAM
 *
 *
 *
 * @param[in] Address  - BIOSRAM Offset value
 * @param[in] OpFlag   - Access sizes
 * @param[in] Value    - Write Data Buffer
 * @param[in] StdHeader
 *
 */
VOID
WriteBiosram (
  IN       UINT8               Address,
  IN       UINT8               OpFlag,
  IN       VOID                *Value,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8    i;

  OpFlag = OpFlag & 0x7f;
  OpFlag = 1 << (OpFlag - 1);
  for (i = 0; i < OpFlag; i++) {
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD4, &Address, StdHeader);
    Address++;
    LibAmdIoWrite (AccessWidth8, FCH_IOMAP_REGCD5, (UINT8 *)Value + i, StdHeader);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Record SMI Status
 *
 *
 * @param[in] StdHeader
 *
 */
VOID
RecordSmiStatus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN   Index;
  UINT8   SwSmiValue;

  ACPIMMIO8 (0xfed80320) |= 0x01;
  for ( Index = 0; Index < 20; Index++ ) {
    ACPIMMIO8 (0xfed10020 + Index) = ACPIMMIO8 (0xfed80280 + Index);
  }
  LibAmdIoRead (AccessWidth8, 0xB0, &SwSmiValue, StdHeader);
  ACPIMMIO8 (0xfed10040) = SwSmiValue;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Clear All SMI Status
 *
 *
 * @param[in] StdHeader
 *
 */
VOID
ClearAllSmiStatus (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINTN   Index;

  for ( Index = 0; Index < 20; Index++ ) {
    ACPIMMIO8 (0xfed80280 + Index) |= 0;
  }
}
#endif /* IS_ENABLED(CONFIG_VENDORCODE_FULL_SUPPORT) */
