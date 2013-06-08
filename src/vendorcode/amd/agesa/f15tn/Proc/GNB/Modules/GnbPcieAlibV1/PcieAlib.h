/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ALIB
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _PCIEALIB_H_
#define _PCIEALIB_H_

#pragma pack (push, 1)
///Port info asl buffer
typedef struct {
  UINT8                   BufferOp;           ///< Opcode
  UINT8                   PkgLength;          ///< Package length
  UINT8                   BufferSize;         ///< Buffer size
  UINT8                   ByteList;           ///< Byte lisy
  UINT8                   StartPhyLane;       ///< Port Start PHY lane
  UINT8                   EndPhyLane;         ///< Port End PHY lane
  UINT8                   StartCoreLane;      ///< Port Start Core lane
  UINT8                   EndCoreLane;        ///< Port End Core lane
  UINT8                   PortId;             ///< Port ID
  UINT16                  WrapperId;          ///< Wrapper ID
  UINT8                   LinkHotplug;        ///< Link hotplug type
  UINT8                   MaxSpeedCap;        ///< Max port speed capability
  UINT8                   ClkPmSupport;       ///< ClkPmSupport
} ALIB_PORT_INFO_BUFFER;
///Ports info asl package
typedef struct {
  UINT8                   PackageOp;          ///< Opcode
  UINT8                   PkgLength;          ///< Package length
  UINT8                   NumElements;        ///< number of elements
  UINT8                   PackageElementList; ///< package element list
  ALIB_PORT_INFO_BUFFER   PortInfo[7];        ///< Array of port info buffers
} ALIB_PORT_INFO_PACKAGE;

#pragma pack (pop)

AGESA_STATUS
PcieAlibFeature (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif
