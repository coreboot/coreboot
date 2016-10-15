/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP directory related functions Prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
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

#ifndef _PSP_DIRECTORY_H_
#define _PSP_DIRECTORY_H_


#define FIRMWARE_TABLE_SIGNATURE  0x55AA55AAul
/// Define the structure OEM signature table
typedef struct _FIRMWARE_ENTRY_TABLE {
  UINT32  Signature;    ///< Signature should be 0x55AA55AAul
  UINT32  ImcRomBase;   ///< Base Address for Imc Firmware
  UINT32  GecRomBase;   ///< Base Address for Gmc Firmware
  UINT32  XHCRomBase;   ///< Base Address for XHCI Firmware
  UINT32  PspDirBase;   ///< Base Address for PSP directory
  UINT32  NewPspDirBase;   ///< Base Address of PSP directory from program start from ST
} FIRMWARE_ENTRY_TABLE;

/// Define structure for PSP directory
typedef struct {
  UINT32  PspCookie;      ///< "$PSP"
  UINT32  Checksum;       ///< 32 bit CRC of header items below and the entire table
  UINT32  TotalEntries;   ///< Number of PSP Entries
  UINT32  Reserved;       ///< Unused
} PSP_DIRECTORY_HEADER;


/// define various enum type for PSP entry type
enum _PSP_DIRECTORY_ENTRY_TYPE {
  AMD_PUBLIC_KEY                  = 0x00,           ///< PSP entry pointer to AMD public key
  PSP_FW_BOOT_LOADER              = 0x01,           ///< PSP entry points to PSP boot loader in SPI space
  PSP_FW_TRUSTED_OS               = 0x02,           ///< PSP entry points to PSP Firmware region in SPI space
  PSP_FW_RECOVERY_BOOT_LOADER     = 0x03,           ///< PSP entry point to PSP recovery region.
  PSP_NV_DATA                     = 0x04,           ///< PSP entry points to PSP data region in SPI space
  BIOS_PUBLIC_KEY                 = 0x05,           ///< PSP entry points to BIOS public key stored in SPI space
  BIOS_RTM_FIRMWARE               = 0x06,           ///< PSP entry points to BIOS RTM code (PEI volume) in SPI space
  BIOS_RTM_SIGNATURE              = 0x07,           ///< PSP entry points to signed BIOS RTM hash stored  in SPI space
  SMU_OFFCHIP_FW                  = 0x08,           ///< PSP entry points to SMU image
  AMD_SEC_DBG_PUBLIC_KEY          = 0x09,           ///< PSP entry pointer to Secure Unlock Public key
  OEM_PSP_FW_PUBLIC_KEY           = 0x0A,           ///< PSP entry pointer to an optional public part of the OEM PSP Firmware Signing Key Token
  AMD_SOFT_FUSE_CHAIN_01          = 0x0B,           ///< PSP entry pointer to 64bit PSP Soft Fuse Chain
  PSP_BOOT_TIME_TRUSTLETS         = 0x0C,           ///< PSP entry points to boot-loaded trustlet binaries
  PSP_BOOT_TIME_TRUSTLETS_KEY     = 0x0D,           ///< PSP entry points to key of the boot-loaded trustlet binaries
  PSP_AGESA_RESUME_FW             = 0x10,           ///< PSP Entry points to PSP Agesa-Resume-Firmware
  SMU_OFF_CHIP_FW_2               = 0x12,           ///< PSP entry points to secondary SMU image
  PSP_S3_NV_DATA                  = 0x1A,           ///< PSP entry pointer to S3 Data Blob
  AMD_SCS_BINARY                  = 0x5F,           ///< Software Configuration Settings Data Block
};

#define PSP_ENTRY_PROGRAM_ID_0             0                  ///< Program identifier 0, used when two programs share the same root key
#define PSP_ENTRY_PROGRAM_ID_1             1                  ///< Program identifier 1, used when two programs share the same root key

#define PSP_ENTRY_CZ_PROGRAM_ID            PSP_ENTRY_PROGRAM_ID_0       ///< CZ Program identifier
#define PSP_ENTRY_BR_PROGRAM_ID            PSP_ENTRY_PROGRAM_ID_1       ///< BR Program identifier

typedef UINT32 PSP_DIRECTORY_ENTRY_TYPE;

/// Structure for PSP Entry
typedef struct {
  PSP_DIRECTORY_ENTRY_TYPE    Type;       ///< Type of PSP entry; 32 bit long
  UINT32                      Size;       ///< Size of PSP Entry in bytes
  UINT64                      Location;   ///< Location of PSP Entry (byte offset from start of SPI-ROM)
} PSP_DIRECTORY_ENTRY;

/// Structure for PSP directory
typedef struct {
  PSP_DIRECTORY_HEADER  Header;           ///< PSP directory header
  PSP_DIRECTORY_ENTRY   PspEntry[1];      ///< Array of PSP entries each pointing to a binary in SPI flash
                                          ///< The actual size of this array comes from the
                                          ///< header (PSP_DIRECTORY.Header.TotalEntries)
} PSP_DIRECTORY;

#endif //_PSP_DIRECTORY_H_
