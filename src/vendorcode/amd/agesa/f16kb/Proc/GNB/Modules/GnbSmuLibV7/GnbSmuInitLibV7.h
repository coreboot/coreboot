/* $NoKeywords:$ */
/**
 * @file
 *
 * NB services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 85506 $   @e \$Date: 2013-01-08 15:38:33 -0600 (Tue, 08 Jan 2013) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#ifndef _GNBNBINITLIBV7_H_
#define _GNBNBINITLIBV7_H_

#pragma pack (push, 1)
#include "GnbUraServices.h"

/// Firmware header
typedef struct {
  UINT32  Digest[5];                ///< Digest
  UINT32  Version;                  ///< Version
  UINT32  HeaderSize;               ///< Header length
  UINT32  Flags;                    ///< Flags
  UINT32  EntryPoint;               ///< Entry Point
  UINT32  CodeSize;                 ///< Code Size
  UINT32  ImageSize;                ///< Image Size
  UINT32  Rtos;                     ///< Rtos
  UINT32  SoftRegisters;            ///< Soft Registers
  UINT32  DpmTable;                 ///< Dpm Table
  UINT32  FanTable;                 ///< Fan Table
  UINT32  CacConfigTable;           ///< Cac Configuration Table
  UINT32  CacStatusTable;           ///< Cac Status Table
  UINT32  mcRegisterTable;          ///< mc Register Table
  UINT32  mcArbDramTimingTable;     ///< mc Arb Dram Timing Table
  UINT32  Globals;                  ///< Globals
  UINT32  Signature;                ///< Signature
  UINT32  Reserved[44];             ///< Reserved space
} FIRMWARE_HEADER_V7;

/// SMU service request contect
typedef struct {
  PCI_ADDR          GnbPciAddress;   ///< PCIe address of GNB
  UINT8             RequestId;       ///< Request/Msg ID
  UINT32            RequestArgument; ///< Request/Msg Argument
} SMU_MSG_CONTEXTV7;


#define SIZE_OF_APMWEIGHTS      (sizeof (UINT32))
typedef struct {
  UINT64        EnergyCountConstant;
  UINT64        BoostTimeConstant;
  UINT32        CountApmWeights;      ///< length of APMWeights array
//  UINT32        ApmWeights[1];        ///< ApmWeights
} SMU_RAM_CPU_INFO;


#pragma pack (pop)

VOID
GnbSmuServiceRequestV7 (
  IN       DEV_OBJECT               *DevObject,
  IN       UINT8                    RequestId,
  IN       UINT32                   RequestArgument,
  IN       UINT32                   AccessFlags
  );

AGESA_STATUS
GnbSmuFirmwareLoadV7 (
  IN       GNB_HANDLE               *GnbHandle,
  IN       FIRMWARE_HEADER_V7       *Firmware,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  );

#define GnbSmuInitLibV7136_macro0 STRING_TO_UINT32 ('!', 'S', 'C', 'S')

AGESA_STATUS
GnbSmuInitLibV7139_fun0(
  IN       GNB_HANDLE           *GnbHandle,
  CONST IN       UINT8                *ScsDataPtr,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

AGESA_STATUS
GnbSmuInitLibV7139_fun1(
  IN       GNB_HANDLE           *GnbHandle,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );


#endif
