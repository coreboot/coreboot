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
 * @e \$Revision: 48409 $   @e \$Date: 2011-03-08 11:19:40 -0600 (Tue, 08 Mar 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (c) 2008 - 2011, Advanced Micro Devices, Inc.
 *               2013 - 2014, Sage Electronic Engineering, LLC
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
 ******************************************************************************
 */

#include <AGESA.h>
#include <cpuRegisters.h>
#include "amdlib.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#if !defined(AMDLIB_OPTIMIZE)
	#define AMDLIB_OPTIMIZE
#endif

BOOLEAN
STATIC
GetPciMmioAddress (
     OUT   UINT64            *MmioAddress,
     OUT   UINT32            *MmioSize,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
IdsOutPort (
  IN       UINT32 Addr,
  IN       UINT32 Value,
  IN       UINT32 Flag
  );

VOID
CpuidRead (
  IN        UINT32      CpuidFcnAddress,
  OUT       CPUID_DATA  *Value
  );

UINT8
ReadNumberOfCpuCores(
  void
  );

AMDLIB_OPTIMIZE
UINT8
ReadIo8 (
  IN       UINT16 Address
  )
{
  return __inbyte (Address);
}

AMDLIB_OPTIMIZE
UINT16
ReadIo16 (
  IN       UINT16 Address
  )
{
  return __inword (Address);
}

AMDLIB_OPTIMIZE
UINT32
ReadIo32 (
  IN       UINT16 Address
  )
{
  return __indword (Address);
}

AMDLIB_OPTIMIZE
VOID
WriteIo8 (
  IN       UINT16 Address,
  IN       UINT8 Data
  )
{
  __outbyte (Address, Data);
}

AMDLIB_OPTIMIZE
VOID
WriteIo16 (
  IN       UINT16 Address,
  IN       UINT16 Data
  )
{
  __outword (Address, Data);
}

AMDLIB_OPTIMIZE
VOID
WriteIo32 (
  IN       UINT16 Address,
  IN       UINT32 Data
  )
{
   __outdword (Address, Data);
}

AMDLIB_OPTIMIZE
STATIC
UINT64 SetFsBase (
  UINT64 address
  )
{
  UINT64 hwcr;
  hwcr = __readmsr (0xC0010015);
  __writemsr (0xC0010015, hwcr | 1 << 17);
  __writemsr (0xC0000100, address);
  return hwcr;
}

AMDLIB_OPTIMIZE
STATIC
VOID
RestoreHwcr (
  UINT64
  value
  )
{
  __writemsr (0xC0010015, value);
}

AMDLIB_OPTIMIZE
UINT8
Read64Mem8 (
  IN       UINT64 Address
  )
{
  UINT8 dataRead;
  UINT64 hwcrSave;
  if ((Address >> 32) == 0) {
    return *(volatile UINT8 *) (UINTN) Address;
  }
  hwcrSave = SetFsBase (Address);
  dataRead = __readfsbyte (0);
  RestoreHwcr (hwcrSave);
  return dataRead;
}

AMDLIB_OPTIMIZE
UINT16
Read64Mem16 (
  IN       UINT64 Address
  )
{
  UINT16 dataRead;
  UINT64 hwcrSave;
  if ((Address >> 32) == 0) {
    return *(volatile UINT16 *) (UINTN) Address;
  }
  hwcrSave = SetFsBase (Address);
  dataRead = __readfsword (0);
  RestoreHwcr (hwcrSave);
  return dataRead;
}

AMDLIB_OPTIMIZE
UINT32
Read64Mem32 (
  IN       UINT64 Address
  )
{
  UINT32 dataRead;
  UINT64 hwcrSave;
  if ((Address >> 32) == 0) {
    return *(volatile UINT32 *) (UINTN) Address;
  }
  hwcrSave = SetFsBase (Address);
  dataRead = __readfsdword (0);
  RestoreHwcr (hwcrSave);
  return dataRead;
  }

AMDLIB_OPTIMIZE
VOID
Write64Mem8 (
  IN       UINT64 Address,
  IN       UINT8 Data
  )
{
  if ((Address >> 32) == 0){
    *(volatile UINT8 *) (UINTN) Address = Data;
  }
  else {
    UINT64 hwcrSave;
    hwcrSave = SetFsBase (Address);
    __writefsbyte (0, Data);
    RestoreHwcr (hwcrSave);
  }
}

AMDLIB_OPTIMIZE
VOID
Write64Mem16 (
  IN       UINT64 Address,
  IN       UINT16 Data
  )
{
 if ((Address >> 32) == 0){
   *(volatile UINT16 *) (UINTN) Address = Data;
 }
 else {
   UINT64 hwcrSave;
   hwcrSave = SetFsBase (Address);
   __writefsword (0, Data);
   RestoreHwcr (hwcrSave);
 }
}

AMDLIB_OPTIMIZE
VOID
Write64Mem32 (
  IN       UINT64 Address,
  IN       UINT32 Data
  )
{
  if ((Address >> 32) == 0){
    *(volatile UINT32 *) (UINTN) Address = Data;
  }
  else {
    UINT64 hwcrSave;
    hwcrSave = SetFsBase (Address);
    __writefsdword (0, Data);
    RestoreHwcr (hwcrSave);
  }
}

AMDLIB_OPTIMIZE
VOID
LibAmdMsrRead (
  IN       UINT32 MsrAddress,
     OUT   UINT64 *Value,
  IN OUT   AMD_CONFIG_PARAMS *ConfigPtr
  )
{
  if ((MsrAddress == 0xFFFFFFFF) || (MsrAddress == 0x00000000)) {
	  IdsErrorStop(MsrAddress);
  }
  *Value = __readmsr (MsrAddress);
}

AMDLIB_OPTIMIZE
VOID
LibAmdMsrWrite (
  IN       UINT32 MsrAddress,
  IN       UINT64 *Value,
  IN OUT   AMD_CONFIG_PARAMS *ConfigPtr
  )
{
  __writemsr (MsrAddress, *Value);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Read IO port
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO port address
 * @param[in] Value         Pointer to save data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibAmdIoRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *(UINT8 *) Value = ReadIo8 (IoAddress);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *(UINT16 *) Value = ReadIo16 (IoAddress);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *(UINT32 *) Value = ReadIo32 (IoAddress);
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Write IO port
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] IoAddress     IO port address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibAmdIoWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       CONST VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    WriteIo8 (IoAddress, *(UINT8 *) Value);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    WriteIo16 (IoAddress, *(UINT16 *) Value);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    WriteIo32 (IoAddress, *(UINT32 *) Value);
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Read memory/MMIO
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibAmdMemRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    *(UINT8 *) Value = Read64Mem8 (MemAddress);
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    *(UINT16 *) Value = Read64Mem16 (MemAddress);
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    *(UINT32 *) Value = Read64Mem32 (MemAddress);
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Write memory/MMIO
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibAmdMemWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       CONST VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{

  switch (AccessWidth) {
  case AccessWidth8:
  case AccessS3SaveWidth8:
    Write64Mem8 (MemAddress, *((UINT8 *) Value));
    break;
  case AccessWidth16:
  case AccessS3SaveWidth16:
    Write64Mem16 (MemAddress, *((UINT16 *) Value));
    break;
  case AccessWidth32:
  case AccessS3SaveWidth32:
    Write64Mem32 (MemAddress, *((UINT32 *) Value));
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Read PCI config space
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] PciAddress    Pci address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibAmdPciRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
     OUT   VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 LegacyPciAccess;
  UINT32 MMIOSize;
  UINT64 RMWrite;
  UINT64 RMWritePrevious;
  UINT64 MMIOAddress;


  ASSERT (PciAddress.AddressValue != ILLEGAL_SBDFO);
  if (!GetPciMmioAddress (&MMIOAddress, &MMIOSize, NULL)) {
    // We need to convert our "portable" PCI address into a "real" PCI access
    LegacyPciAccess = ((1 << 31) + (PciAddress.Address.Register & 0xFC) + (PciAddress.Address.Function << 8) + (PciAddress.Address.Device << 11) + (PciAddress.Address.Bus << 16) + ((PciAddress.Address.Register & 0xF00) << (24 - 8)));
    if (PciAddress.Address.Register <= 0xFF) {
      LibAmdIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess, NULL);
      LibAmdIoRead (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value, NULL);
    } else {
      LibAmdMsrRead  (NB_CFG, &RMWritePrevious, NULL);
      RMWrite = RMWritePrevious | 0x0000400000000000;
      LibAmdMsrWrite (NB_CFG, &RMWrite, NULL);
      LibAmdIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess, NULL);
      LibAmdIoRead (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value, NULL);
      LibAmdMsrWrite (NB_CFG, &RMWritePrevious, NULL);
    }
    //IDS_HDT_CONSOLE (LIB_PCI_RD, "~PCI RD %08x = %08x\n", LegacyPciAccess, *(UINT32 *)Value);
  } else {
    // Setup the MMIO address
    ASSERT ((MMIOAddress + MMIOSize) > (MMIOAddress + (PciAddress.AddressValue & 0x0FFFFFFF)));
    MMIOAddress += (PciAddress.AddressValue & 0x0FFFFFFF);
    LibAmdMemRead (AccessWidth, MMIOAddress, Value, NULL);
    //IDS_HDT_CONSOLE (LIB_PCI_RD, "~MMIO RD %08x = %08x\n", (UINT32) MMIOAddress, *(UINT32 *)Value);
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Write PCI config space
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] PciAddress    Pci address
 * @param[in] Value         Pointer to data
 * @param[in] StdHeader     Standard configuration header
 *
 */
VOID
LibAmdPciWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       PCI_ADDR PciAddress,
  IN       CONST VOID *Value,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 LegacyPciAccess;
  UINT32 MMIOSize;
  UINT64 RMWrite;
  UINT64 RMWritePrevious;
  UINT64 MMIOAddress;


  ASSERT (PciAddress.AddressValue != ILLEGAL_SBDFO);
  if (!GetPciMmioAddress (&MMIOAddress, &MMIOSize, NULL)) {
    // We need to convert our "portable" PCI address into a "real" PCI access
    LegacyPciAccess = ((1 << 31) + (PciAddress.Address.Register & 0xFC) + (PciAddress.Address.Function << 8) + (PciAddress.Address.Device << 11) + (PciAddress.Address.Bus << 16) + ((PciAddress.Address.Register & 0xF00) << (24 - 8)));
    if (PciAddress.Address.Register <= 0xFF) {
      LibAmdIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess, NULL);
      LibAmdIoWrite (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value, NULL);
    } else {
      LibAmdMsrRead  (NB_CFG, &RMWritePrevious, NULL);
      RMWrite = RMWritePrevious | 0x0000400000000000;
      LibAmdMsrWrite (NB_CFG, &RMWrite, NULL);
      LibAmdIoWrite (AccessWidth32, IOCF8, &LegacyPciAccess, NULL);
      LibAmdIoWrite (AccessWidth, IOCFC + (UINT16) (PciAddress.Address.Register & 0x3), Value, NULL);
      LibAmdMsrWrite (NB_CFG, &RMWritePrevious, NULL);
    }
    //IDS_HDT_CONSOLE (LIB_PCI_WR, "~PCI WR %08x = %08x\n", LegacyPciAccess, *(UINT32 *)Value);
    //printk(BIOS_DEBUG, "~PCI WR %08x = %08x\n", LegacyPciAccess, *(UINT32 *)Value);
    //printk(BIOS_DEBUG, "LibAmdPciWrite\n");
  } else {
    // Setup the MMIO address
    ASSERT ((MMIOAddress + MMIOSize) > (MMIOAddress + (PciAddress.AddressValue & 0x0FFFFFFF)));
    MMIOAddress += (PciAddress.AddressValue & 0x0FFFFFFF);
    LibAmdMemWrite (AccessWidth, MMIOAddress, Value, NULL);
    //IDS_HDT_CONSOLE (LIB_PCI_WR, "~MMIO WR %08x = %08x\n", (UINT32) MMIOAddress, *(UINT32 *)Value);
    //printk(BIOS_DEBUG, "~MMIO WR %08x = %08x\n", (UINT32) MMIOAddress, *(UINT32 *)Value);
    //printk(BIOS_DEBUG, "LibAmdPciWrite mmio\n");
  }
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get MMIO base address for PCI accesses
 *
 * @param[out] MmioAddress   PCI MMIO base address
 * @param[out] MmioSize      Size of region in bytes
 * @param[in]  StdHeader     Standard configuration header
 *
 * @retval    TRUE          MmioAddress/MmioSize are valid
 */
BOOLEAN
STATIC
GetPciMmioAddress (
     OUT   UINT64            *MmioAddress,
     OUT   UINT32            *MmioSize,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN MmioIsEnabled;
  UINT32  EncodedSize;
  UINT64  LocalMsrRegister;

  MmioIsEnabled = FALSE;
  LibAmdMsrRead (MSR_MMIO_Cfg_Base, &LocalMsrRegister, NULL);
  if ((LocalMsrRegister & BIT0) != 0) {
    *MmioAddress = LocalMsrRegister & 0xFFFFFFFFFFF00000;
    EncodedSize = (UINT32) ((LocalMsrRegister & 0x3C) >> 2);
    *MmioSize = ((1 << EncodedSize) * 0x100000);
    MmioIsEnabled = TRUE;
  }
  return MmioIsEnabled;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Verify checksum of binary image (B1/B2/B3)
 *
 *
 * @param[in]   ImagePtr      Pointer to image  start
 * @retval      TRUE          Checksum valid
 * @retval      FALSE         Checksum invalid
 */
BOOLEAN
LibAmdVerifyImageChecksum (
  IN       CONST VOID *ImagePtr
  )
{
  // Assume ImagePtr points to the binary start ($AMD)
  // Checksum is on an even boundary in AMD_IMAGE_HEADER

  UINT16 Sum;
  UINT32 i;

  Sum = 0;

  i = ((AMD_IMAGE_HEADER*) ImagePtr)->ImageSize;

  while (i > 1) {
    Sum = Sum + *((UINT16 *)ImagePtr);
    ImagePtr = (VOID *) ((UINT8 *)ImagePtr + 2);
    i = i - 2;
  }
  if (i > 0) {
    Sum = Sum + *((UINT8 *) ImagePtr);
  }

  return (Sum == 0)?TRUE:FALSE;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Locate AMD binary image that contain specific module
 *
 *
 * @param[in]   StartAddress    Pointer to start range
 * @param[in]   EndAddress      Pointer to end range
 * @param[in]   Alignment       Image address alignment
 * @param[in]   ModuleSignature Module signature.
 * @retval     NULL  if image not found
 * @retval     pointer to image header
 */
CONST VOID *
LibAmdLocateImage (
  IN       CONST VOID *StartAddress,
  IN       CONST VOID *EndAddress,
  IN       UINT32 Alignment,
  IN       CONST CHAR8 ModuleSignature[8]
  )

{
  CONST UINT8 *CurrentPtr = StartAddress;
  AMD_MODULE_HEADER *ModuleHeaderPtr;
  CONST UINT64 SearchStr = *((UINT64*)ModuleSignature);

  // Search from start to end incrementing by alignment
  while ((CurrentPtr >= (UINT8 *) StartAddress) && (CurrentPtr < (UINT8 *) EndAddress)) {
    // First find a binary image
    if (IMAGE_SIGNATURE == *((UINT32 *) CurrentPtr)) {
      // TODO Figure out a way to fix the AGESA binary checksum
//    if (LibAmdVerifyImageChecksum (CurrentPtr)) {
        // If we have a valid image, search module linked list for a match
        ModuleHeaderPtr = (AMD_MODULE_HEADER*)(((AMD_IMAGE_HEADER *) CurrentPtr)->ModuleInfoOffset);
        while ((ModuleHeaderPtr != NULL) &&
            (MODULE_SIGNATURE == *((UINT32*)&(ModuleHeaderPtr->ModuleHeaderSignature)))) {
          if (SearchStr == *((UINT64*)&(ModuleHeaderPtr->ModuleIdentifier))) {
            return  CurrentPtr;
          }
          ModuleHeaderPtr = (AMD_MODULE_HEADER *)ModuleHeaderPtr->NextBlock;
        }
//      }
    }
    CurrentPtr += Alignment;
  }
  return NULL;
}

BOOLEAN
IdsErrorStop (
  IN      UINT32 FileCode
  )
{
	struct POST {
		UINT16 deadlo;
		UINT32 messagelo;
		UINT16 deadhi;
		UINT32 messagehi;
	} post = {0xDEAD, FileCode, 0xDEAD, FileCode};
	UINT16 offset = 0;
	UINT16 j;

	while(1) {
		offset %= sizeof(struct POST) / 2;
		WriteIo16(80, *((UINT16 *)&post)+offset);
		++offset;
		for (j=0; j<250; ++j) {
			ReadIo8(80);
		}
	}
}
