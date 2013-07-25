/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe Init Library
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 87271 $   @e \$Date: 2013-01-31 10:11:23 -0600 (Thu, 31 Jan 2013) $
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
#ifndef _GNBPCIEINITLIBV5_H_
#define _GNBPCIEINITLIBV5_H_

VOID
PciePifApplyGangingV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePifPllPowerDownV5 (
  IN      UINT32                LaneBitmap,
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieTopologyApplyLaneMuxV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

BOOLEAN
PcieTopologyIsGen3SupportedV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper
  );

VOID
PciePwrPowerDownUnusedLanesV5 (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePwrClockGatingV5 (
  IN       PCIe_WRAPPER_CONFIG    *Wrapper,
  IN       PCIe_PLATFORM_CONFIG   *Pcie
  );

VOID
PciePortsVisibilityControlV5 (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      PCIe_PLATFORM_CONFIG    *Pcie
  );

VOID
PcieLinkInitHotplugV5 (
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieEnableSlotPowerLimitV5 (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      PCIe_PLATFORM_CONFIG   *Pcie
  );

VOID
PciePhyApplyGangingV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePhyChannelCharacteristicV5 (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieTopologyLaneControlV5 (
  IN      LANE_CONTROL          Control,
  IN      UINT32                LaneBitMap,
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieTopologyReduceLinkWidthV5 (
  IN      UINT8                 LinkWidth,
  IN      PCIe_ENGINE_CONFIG    *Engine,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieTopologyExecuteReconfigV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PciePifSetLs2ExitTimeV5 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  );

#endif
