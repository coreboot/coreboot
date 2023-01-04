/** @file
  Interface header file for the Enhanced warning log library class.

  @copyright
  INTEL CONFIDENTIAL
  Copyright 2018 - 2021 Intel Corporation. <BR>

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Intel Corporation or its suppliers or
  licensors. Title to the Material remains with Intel Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary    and
  confidential information of Intel Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Intel's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Intel in writing.

  Unless otherwise agreed by Intel in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Intel or
  Intel's suppliers or licensors in any way.
**/

#ifndef _ENHANCED_WARNING_LOG_LIB_
#define _ENHANCED_WARNING_LOG_LIB_

#define FSP_RESERVED_LEN                 12

#pragma pack(1)

///
/// Enhanced Warning Log Header
///
typedef struct {
  EFI_GUID  EwlGuid;      /// GUID that uniquely identifies the EWL revision
  UINT32    Size;         /// Total size in bytes including the header and buffer
  UINT32    FreeOffset;   /// Offset of the beginning of the free space from byte 0
                          /// of the buffer immediately following this structure
                          /// Can be used to determine if buffer has sufficient space for next entry
  UINT32    Crc;          /// 32-bit CRC generated over the whole size minus this crc field
                          /// Note: UEFI 32-bit CRC implementation (CalculateCrc32) (References [7])
                          /// Consumers can ignore CRC check if not needed.
  UINT32    Reserved;     /// Reserved for future use, must be initialized to 0
} EWL_HEADER;

///
/// List of all entry types supported by this revision of EWL
///
typedef enum {
  EwlType0  = 0,
  EwlType1  = 1,
  EwlType2  = 2,
  EwlType3  = 3,
  EwlType4  = 4,
  EwlType5  = 5,
  EwlType6  = 6,
  EwlType7  = 7,
  EwlType8  = 8,
  EwlType9  = 9,
  EwlType10 = 10,
  EwlType11 = 11,
  EwlType12 = 12,
  EwlType13 = 13,
  EwlType14 = 14,
  EwlType15 = 15,
  EwlType16 = 16,
  EwlType17 = 17,
  EwlType18 = 18,
  EwlType19 = 19,
  EwlType20 = 20,
  EwlType21 = 21,
  EwlType22 = 22,
  EwlType23 = 23,
  EwlType24 = 24,
  EwlType25 = 25,
  EwlType26 = 26,
  EwlType27 = 27,
  EwlType28 = 28,
  EwlType29 = 29,
  EwlType30 = 30,
  EwlType31 = 31,
  EwlType32 = 32,
  EwlTypeMax,
  EwlTypeOem = 0x8000,
  EwlTypeDelim = MAX_INT32
} EWL_TYPE;

///
/// EWL severities
///
typedef enum {
  EwlSeverityInfo,
  EwlSeverityWarning,
  EwlSeverityFatal,
  EwlSeverityMax,
  EwlSeverityDelim = MAX_INT32
} EWL_SEVERITY;


///
/// Generic entry header for parsing the log
///
typedef struct {
  EWL_TYPE      Type;
  UINT16        Size;     /// Entries will be packed by byte in contiguous space
  EWL_SEVERITY  Severity; /// Warning, error, informational, this may be extended in the future
} EWL_ENTRY_HEADER;

///
/// Legacy content provides context of the warning
///
typedef struct {
  UINT8     MajorCheckpoint;  // EWL Spec - Appendix B
  UINT8     MinorCheckpoint;
  UINT8     MajorWarningCode; // EWL Spec - Appendix A
  UINT8     MinorWarningCode;
} EWL_ENTRY_CONTEXT;

///
/// Legacy content to specify memory location
///
typedef struct {
  UINT8     Socket;     /// 0xFF = n/a
  UINT8     Channel;    /// 0xFF = n/a
  UINT8     PseudoChannel; /// 0xFF = n/a
  UINT8     Dimm;       /// 0xFF = n/a
  UINT8     Rank;       /// 0xFF = n/a
} EWL_ENTRY_MEMORY_LOCATION;

///
/// Type 3 = Enhanced type for command, control IO errors
///
typedef struct {
  EWL_ENTRY_HEADER           Header;
  EWL_ENTRY_CONTEXT          Context;
  EWL_ENTRY_MEMORY_LOCATION  MemoryLocation;
  UINT8			     reserved1[FSP_RESERVED_LEN]; // MRC_LT Level; MRC_GT Group; GSM_CSN Signal;
  UINT8                      EyeSize;                     // 0xFF = n/a
} EWL_ENTRY_TYPE3;

#pragma pack()

///
/// Enhanced Warning Log Spec defined data log structure
///
typedef struct {
  EWL_HEADER Header;          /// The size will vary by implementation and should not be assumed
  UINT8      Buffer[4 * 1024];  /// The spec requirement is that the buffer follow the header
} EWL_PUBLIC_DATA;

///
/// EWL private data structure.  This is going to be implementation dependent
///   When we separate OEM hooks via a PPI, we can remove this
///
typedef struct {
  UINT32            bufSizeOverflow;  // Number of bytes that could not be added to buffer
  UINT32            numEntries;       // Number of entries currently logged
  EWL_PUBLIC_DATA   status;           // Spec defined EWL
} EWL_PRIVATE_DATA;


#endif // #ifndef _ENHANCED_WARNING_LOG_LIB_
