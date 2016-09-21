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
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
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
  AMD_PUBLIC_KEY                  = 0,    ///< PSP entry pointer to AMD public key
  PSP_FW_BOOT_LOADER              = 1,    ///< PSP entry points to PSP boot loader in SPI space
  PSP_FW_TRUSTED_OS               = 2,    ///< PSP entry points to PSP Firmware region in SPI space
  PSP_FW_RECOVERY_BOOT_LOADER     = 3,    ///< PSP entry point to PSP recovery region.
  PSP_NV_DATA                     = 4,    ///< PSP entry points to PSP data region in SPI space
  BIOS_PUBLIC_KEY                 = 5,    ///< PSP entry points to BIOS public key stored in SPI space
  BIOS_RTM_FIRMWARE               = 6,    ///< PSP entry points to BIOS RTM code (PEI volume) in SPI space
  BIOS_RTM_SIGNATURE              = 7,    ///< PSP entry points to signed BIOS RTM hash stored  in SPI space
  SMU_OFFCHIP_FW                  = 8     ///< PSP entry points to SMU image
};
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
