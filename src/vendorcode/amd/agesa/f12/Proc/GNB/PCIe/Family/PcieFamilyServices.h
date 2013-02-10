/* $NoKeywords:$ */

/**
 * @file
 *
 * Family specific PCIe services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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

#ifndef _PCIECOMPLEXCONFIG_H_
#define _PCIECOMPLEXCONFIG_H_


AGESA_STATUS
PcieFmForceDccRecalibrationCallback (
  IN       PCIe_WRAPPER_CONFIG   *Wrapper,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  );

VOID
PcieFmPhyApplyGanging (
  IN       PCIe_WRAPPER_CONFIG          *Wrapper,
  IN       PCIe_PLATFORM_CONFIG         *Pcie
 );

AGESA_STATUS
PcieFmPhyLetPllPersonalityInitCallback (
  IN       PCIe_WRAPPER_CONFIG          *Wrapper,
  IN       VOID                         *Buffer,
  IN       PCIe_PLATFORM_CONFIG         *Pcie
  );

VOID
PcieFmPhyChannelCharacteristic (
  IN       PCIe_ENGINE_CONFIG           *Engine,
  IN       PCIe_PLATFORM_CONFIG         *Pcie
  );

VOID
PcieFmPortVisabilityControl (
  IN       PCIE_PORT_VISIBILITY         Control,
  IN       PCIe_SILICON_CONFIG          *Silicon,
  IN       PCIe_PLATFORM_CONFIG         *Pcie
  );

VOID
PcieFmPreInit (
  IN       PCIe_PLATFORM_CONFIG         *Pcie
  );


VOID
PcieFmAvertClockPickers (
  IN       PCIe_WRAPPER_CONFIG          *Wrapper,
  IN       PCIe_PLATFORM_CONFIG         *Pcie
  );

VOID
PcieFmSetBootUpVoltage (
  IN      PCIE_LINK_SPEED_CAP           LinkCap,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  );

VOID
PcieFmEnableSlotPowerLimit (
  IN      PCIe_ENGINE_CONFIG            *Engine,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  );

VOID
PcieFmConfigureClock (
  IN      PCIE_LINK_SPEED_CAP           LinkSpeedCapability,
  IN      PCIe_WRAPPER_CONFIG           *Wrapper,
  IN      PCIe_PLATFORM_CONFIG          *Pcie
  );

VOID
PcieFmPifSetRxDetectPowerMode (
  IN      PCIe_WRAPPER_CONFIG          *Wrapper,
  IN      PCIe_PLATFORM_CONFIG         *Pcie
  );

VOID
PcieFmPifSetPllModeForL1 (
  IN      UINT32                      LaneBitmap,
  IN      PCIe_WRAPPER_CONFIG         *Wrapper,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  );

UINT8
PcieFmPifGetPllPowerUpLatency (
  IN      PCIe_WRAPPER_CONFIG         *Wrapper,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  );

AGESA_STATUS
PcieFmPhyLaneInitInitCallback (
  IN      PCIe_WRAPPER_CONFIG         *Wrapper,
  IN      VOID                        *Buffer,
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  );
#endif

