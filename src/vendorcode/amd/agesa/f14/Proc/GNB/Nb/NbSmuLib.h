/* $NoKeywords:$ */
/**
 * @file
 *
 * Various NB initialization services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 39275 $   @e \$Date: 2010-10-09 08:22:05 +0800 (Sat, 09 Oct 2010) $
 *
 */
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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
 * ***************************************************************************
 *
 */
#ifndef _NBSMULIB_H_
#define _NBSMULIB_H_


#define SMU_EXT_SERVICE_FLAGS_POLL_ACK    0x1
#define SMU_EXT_SERVICE_FLAGS_POLL_DONE   0x2
#define SMU_GMM_TO_FCR(GmmReg)            ((GmmReg >> 2) | 0xFF300000)

#pragma pack (push, 1)
/// SMU Register Entry
typedef struct {
  UINT16  Reg;                       ///< Register address
  UINT32  Value;                     ///< Register data
} SMU_REGISTER_ENTRY;

/// SMU Firmware revision
typedef struct {
  UINT16              MajorRev;     ///< Major revision
  UINT16              MinorRev;     ///< Minor revision
} SMU_FIRMWARE_REV;

/// Firmware block
typedef struct {
  UINT16              Address;      ///< Block Address
  UINT16              Length;       ///< Block length in DWORD
  CONST UINT32              *Data;        ///< Pointer to data array
} SMU_FIRMWARE_BLOCK;

/// Firmware header
typedef struct {
  SMU_FIRMWARE_REV    Revision;       ///< Revision info
  UINT16              NumberOfBlock;  ///< Number of blocks
  CONST SMU_FIRMWARE_BLOCK  *BlockArray;    ///< Pointer to block definition array
} SMU_FIRMWARE_HEADER;

/// SMU indirect register write data context
typedef  struct {
  UINT8               Address;        ///< SMU indirect register address
  ACCESS_WIDTH        Width;          ///< SMU indirect register width
  UINT32              Value;          ///< Value
} SMU_INDIRECT_WRITE_DATA;
#pragma pack (pop)

VOID
NbSmuIndirectRead (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
     OUT  VOID                *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuIndirectPoll (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
  IN      UINT32              Mask,
  IN      UINT32              CompateData,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuIndirectWriteEx (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
  CONST IN      VOID                *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuIndirectWrite (
  IN      UINT8               Address,
  IN      ACCESS_WIDTH        Width,
  CONST IN      VOID                *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuIndirectWriteS3Script (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              ContextLength,
  IN      VOID*               Context
  );

VOID
NbSmuRcuRegisterWrite (
  IN      UINT16              Address,
  CONST IN      UINT32              *Value,
  IN      UINT32              Count,
  IN      BOOLEAN             S3Save,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuRcuRegisterRead (
  IN      UINT16              Address,
  OUT     UINT32              *Value,
  IN      UINT32              Count,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuSrbmRegisterRead (
  IN      UINT32              Address,
  OUT     UINT32              *Value,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuSrbmRegisterWrite (
  IN      UINT32              Address,
  IN      UINT32              *Value,
  IN      BOOLEAN             S3Save,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuServiceRequestEx (
  IN      UINT8               RequestId,
  IN      UINT8               Flags,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuServiceRequest (
  IN      UINT8               RequestId,
  IN      BOOLEAN             S3Save,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuServiceRequestS3Script (
  IN      AMD_CONFIG_PARAMS   *StdHeader,
  IN      UINT16              ContextLength,
  IN      VOID*               Context
  );

UINT32
NbSmuReadEfuse (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

UINT32
NbSmuReadEfuseField (
  IN      UINT8               Chain,
  IN      UINT16              Offset,
  IN      UINT8               Length,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
NbSmuFirmwareDownload (
  CONST IN      SMU_FIRMWARE_HEADER *Firmware,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

SMU_FIRMWARE_REV
NbSmuFirmwareRevision (
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#define SMI_FIRMWARE_REVISION(x) ((x.MajorRev << 16) | x.MinorRev)
#endif
