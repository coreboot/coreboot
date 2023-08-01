/** @file
  Header file for Firmware Version Information
 @copyright
  Copyright (c) 2015 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _FIRMWARE_VERSION_INFO_HOB_H_
#define _FIRMWARE_VERSION_INFO_HOB_H_

#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>

#pragma pack(1)
///
/// Firmware Version Structure
///
typedef struct {
  UINT8                          MajorVersion;
  UINT8                          MinorVersion;
  UINT8                          Revision;
  UINT16                         BuildNumber;
} FIRMWARE_VERSION;

///
/// Firmware Version Information Structure
///
typedef struct {
  UINT8                          ComponentNameIndex;        ///< Offset 0   Index of Component Name
  UINT8                          VersionStringIndex;        ///< Offset 1   Index of Version String
  FIRMWARE_VERSION               Version;                   ///< Offset 2-6 Firmware version
} FIRMWARE_VERSION_INFO;

#ifndef __SMBIOS_STANDARD_H__
///
/// The Smbios structure header.
///
typedef struct {
  UINT8                          Type;
  UINT8                          Length;
  UINT16                         Handle;
} SMBIOS_STRUCTURE;
#endif

///
/// Firmware Version Information HOB Structure
///
typedef struct {
  EFI_HOB_GUID_TYPE              Header;                    ///< Offset 0-23  The header of FVI HOB
  SMBIOS_STRUCTURE               SmbiosData;                ///< Offset 24-27  The SMBIOS header of FVI HOB
  UINT8                          Count;                     ///< Offset 28    Number of FVI elements included.
///
/// FIRMWARE_VERSION_INFO structures followed by the null terminated string buffer
///
} FIRMWARE_VERSION_INFO_HOB;
#pragma pack()

#endif // _FIRMWARE_VERSION_INFO_HOB_H_