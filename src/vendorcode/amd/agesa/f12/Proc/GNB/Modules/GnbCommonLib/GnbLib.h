/* $NoKeywords:$ */
/**
 * @file
 *
 * GNB register access services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 47656 $   @e \$Date: 2011-02-25 02:39:38 +0800 (Fri, 25 Feb 2011) $
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
* ***************************************************************************
*
*/
#ifndef _GNBLIB_H_
#define _GNBLIB_H_

#define IOC_WRITE_ENABLE 0x80

typedef AGESA_STATUS (F_GNB_REGISTER_ACCESS) (
  UINT8               RegisterSpaceType,
  UINT32              Address,
  VOID                *Value,
  UINT32              Flags,
  AMD_CONFIG_PARAMS   *StdHeader
);

typedef F_GNB_REGISTER_ACCESS *PF_GNB_REGISTER_ACCESS;

/// Register Read/Write protocol
typedef struct {
  PF_GNB_REGISTER_ACCESS Read;    ///< Read Register
  PF_GNB_REGISTER_ACCESS Write;   ///< Write Register
} GNB_REGISTER_PROTOCOL;

VOID
GnbLibPciIndirectWrite (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      VOID          *Value,
  IN      VOID          *Config
  );

VOID
GnbLibPciIndirectRead (
  IN       UINT32        Address,
  IN       UINT32        IndirectAddress,
  IN       ACCESS_WIDTH  Width,
     OUT   VOID          *Value,
  IN       VOID          *Config
  );

VOID
GnbLibPciIndirectRMW (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );

VOID
GnbLibPciIndirectWriteField (
  IN      UINT32        Address,
  IN      UINT32        IndirectAddress,
  IN      UINT8         FieldOffset,
  IN      UINT8         FieldWidth,
  IN      UINT32        Value,
  IN      BOOLEAN       S3Save,
  IN      VOID          *Config
  );


VOID
GnbLibPciRMW (
  IN      UINT32        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );

VOID
GnbLibIoRMW (
  IN      UINT16        Address,
  IN      ACCESS_WIDTH  Width,
  IN      UINT32        Mask,
  IN      UINT32        Value,
  IN      VOID          *Config
  );

UINT32
GnbGetNumberOfSockets (
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
GnbGetNumberOfSiliconsOnSocket (
  IN      UINT32            SiliconId,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GnbIsDevicePresentInSocket (
  IN      UINT32            SocketId,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

PCI_ADDR
GnbGetPciAddress (
  IN      UINT32            SocketId,
  IN      UINT32            SiliconId,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
GnbLibPowerOf (
  IN      UINT32            Value,
  IN      UINT32            Power
  );

VOID*
GnbLibFind (
  IN      UINT8             *Buf1,
  IN      UINTN             Buf1Length,
  IN      UINT8             *Buf2,
  IN      UINTN             Buf2Length
  );

VOID
GnbLibIndirectIoBlockRead (
  IN      UINT16            IndexPort,
  IN      UINT16            DataPort,
  IN      ACCESS_WIDTH      Width,
  IN      UINT32            IndexAddress,
  IN      UINT32            Count,
  IN      VOID              *Buffer,
  IN      VOID              *Config
  );

UINT8
GnbLiGetIoapicId (
  IN      UINT64            IoApicBaseAddress,
  IN      VOID              *Config
  );

VOID
GnbLibDebugDumpBuffer (
  IN       VOID             *Buffer,
  IN       UINT32           Count,
  IN       UINT8            DataWidth,
  IN       UINT8            LineWidth
  );

AGESA_STATUS
GnbLibLocateService (
  IN       GNB_SERVICE_ID       ServiceId,
  IN       UINT8                SocketId,
  IN       VOID                 **ServiceProtocol,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif
