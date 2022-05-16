/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe utility. Various supporting functions.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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

#ifndef _PCIEUTILLIB_H_
#define _PCIEUTILLIB_H_

/// Core lanes
typedef enum {
  AllCoreLanes,           ///< All core lanes
  AllocatedCoreLanes,     ///< Allocated core lanes
  ActiveCoreLanes,        ///< Active core lanes
  HotplugCoreLanes,       ///< Hot plug core lanes
  SbCoreLanes,            ///< South bridge core lanes
} CORE_LANES;

/// DDI lanes
typedef enum {
  DdiAllLanes,            ///< All DDI Lanes
  DdiActiveLanes          ///< Active DDI Lanes
} DDI_LANES;

BOOLEAN
PcieUtilSearchArray (
  IN      UINT8                         *Buf1,
  IN      UINTN                         Buf1Length,
  CONST IN      UINT8                         *Buf2,
  IN      UINTN                         Buf2Length
  );

VOID
PcieUtilGetLinkHwStateHistory (
  IN       PCIe_ENGINE_CONFIG            *Engine,
     OUT   UINT8                         *History,
  IN       UINT8                         Length,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );


BOOLEAN
PcieUtilIsLinkReversed (
  IN      BOOLEAN                        HwLinkState,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );


UINT8
PcieUtilGetLinkWidth (
  IN      PCIe_ENGINE_CONFIG            *Engine,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  );


UINT32
PcieUtilGetEngineLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_ENGINE_CONFIG             *Engine
  );

UINT32
PcieUtilGetWrapperLaneBitMap (
  IN      UINT32                         IncludeLaneType,
  IN      UINT32                         ExcludeLaneType,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  );

VOID
PciePortProgramRegisterTable (
  CONST IN      PCIE_PORT_REGISTER_ENTRY       *Table,
  IN      UINTN                          Length,
  IN      PCIe_ENGINE_CONFIG             *Engine,
  IN      BOOLEAN                        S3Save,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

VOID
PcieLockRegisters (
  IN       PCIe_WRAPPER_CONFIG           *Wrapper,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  );

PCIE_LINK_SPEED_CAP
PcieUtilGlobalGenCapability (
  IN      UINT32                         Flags,
  IN      PCIe_PLATFORM_CONFIG           *Pcie
  );

#endif
