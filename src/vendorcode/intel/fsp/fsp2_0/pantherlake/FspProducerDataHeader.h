/** @file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution. The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

#ifndef _FSP_PRODUCER_DATA_HEADER_H_
#define _FSP_PRODUCER_DATA_HEADER_H_

#include <Guid/FspHeaderFile.h>

#define BUILD_TIME_STAMP_SIZE 12

//
// FSP Header Data structure from FspHeader driver.
//
#pragma pack(1)
///
/// FSP Producer Data Subtype - 1
///
typedef struct {
  ///
  /// Byte 0x00: Length of this FSP producer data type record.
  ///
  UINT16  Length;
  ///
  /// Byte 0x02: FSP producer data type.
  ///
  UINT8   Type;
  ///
  /// Byte 0x03: Revision of this FSP producer data type.
  ///
  UINT8   Revision;
  ///
  /// Byte 0x04: 4 byte field of RC version which is used to build this FSP image.
  ///
  UINT32  RcVersion;
  ///
  /// Byte 0x08: Represents the build time stamp "YYYYMMDDHHMM".
  ///
  UINT8  BuildTimeStamp[BUILD_TIME_STAMP_SIZE];
} FSP_PRODUCER_DATA_TYPE1;

///
/// FSP Producer Data Subtype - 2
///
typedef struct {
  ///
  /// Byte 0x00: Length of this FSP producer data type record.
  ///
  UINT16  Length;
  ///
  /// Byte 0x02: FSP producer data type.
  ///
  UINT8   Type;
  ///
  /// Byte 0x03: Revision of this FSP producer data type.
  ///
  UINT8   Revision;
  ///
  /// Byte 0x04: 4 byte field of Mrc version which is used to build this FSP image.
  ///
  UINT8  MrcVersion [4];
} FSP_PRODUCER_DATA_TYPE2;


typedef struct {
  FSP_INFO_HEADER          FspInfoHeader;
  FSP_INFO_EXTENDED_HEADER FspInfoExtendedHeader;
  FSP_PRODUCER_DATA_TYPE1  FspProduceDataType1;
  FSP_PRODUCER_DATA_TYPE2  FspProduceDataType2;
  FSP_PATCH_TABLE          FspPatchTable;
} FSP_PRODUCER_DATA_TABLES;
#pragma pack()

#endif // _FSP_PRODUCER_DATA_HEADER_H
