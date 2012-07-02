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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*;********************************************************************************
;
; Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
;
; AMD is granting you permission to use this software (the Materials)
; pursuant to the terms and conditions of your Software License Agreement
; with AMD.  This header does *NOT* give you permission to use the Materials
; or any rights under AMD's intellectual property.  Your use of any portion
; of these Materials shall constitute your acceptance of those terms and
; conditions.  If you do not agree to the terms and conditions of the Software
; License Agreement, please do not use any portion of these Materials.
;
; CONFIDENTIALITY:  The Materials and all other information, identified as
; confidential and provided to you by AMD shall be kept confidential in
; accordance with the terms and conditions of the Software License Agreement.
;
; LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
; PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
; WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
; OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
; IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
; (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
; INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
; GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
; RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
; THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
; EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
; THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
;
; AMD does not assume any responsibility for any errors which may appear in
; the Materials or any other related information provided to you by AMD, or
; result from use of the Materials or any related information.
;
; You agree that you will not reverse engineer or decompile the Materials.
;
; NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
; further information, software, technical information, know-how, or show-how
; available to you.  Additionally, AMD retains the right to modify the
; Materials at any time, without notice, and is not obligated to provide such
; modified Materials to you.
;
; U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
; "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
; subject to the restrictions as set forth in FAR 52.227-14 and
; DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
; Government constitutes acknowledgement of AMD's proprietary rights in them.
;
; EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
; direct product thereof will be exported directly or indirectly, into any
; country prohibited by the United States Export Administration Act and the
; regulations thereunder, without the required authorization from the U.S.
; government nor will be used for any purpose prohibited by the same.
;*********************************************************************************/
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
  MswAddr = 0xFFF0;
  do {
    RomPtr = (MswAddr << 16) + FCH_ROMSIG_BASE_IO;
    LibAmdMemRead (AccessWidth32, (UINT64) RomPtr, &RomSig, StdHeader);
    if (RomSig == FCH_ROMSIG_SIGNATURE) {
      *RomSigPtr = RomPtr;
      break;
    }
    MswAddr <<= 1;
  } while (MswAddr != 0xFE00);
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
