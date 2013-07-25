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
 * @e \$Revision: 84514 $   @e \$Date: 2012-12-17 10:44:17 -0600 (Mon, 17 Dec 2012) $
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

#ifndef _PCIEALIBV2_H_
#define _PCIEALIBV2_H_

#pragma pack (push, 1)

/// _ALIB_PORT_DATA structure
typedef struct {
  UINT8                   PciePortMaxSpeed;                 ///< Maximum speed for PCIe Port
  UINT8                   PciePortAcSpeed;                  ///< AC power speed for PCIe Port
  UINT8                   PciePortDcSpeed;                  ///< DC power speed for PCIe Port
  UINT8                   PciePortCurSpeed;                 ///< Current speed for PCIe Port
  UINT8                   PcieSbPort;                       ///< Sb Port
  UINT8                   PcieLinkSafeMode;                 ///< Link Safe Mode
  UINT8                   PcieLocalOverrideSpeed;           ///< Local Override Speed
  UINT8                   StartPhyLane;                     ///< Start PHY lane
  UINT8                   EndPhyLane;                       ///< End PHY lane
  UINT8                   StartCoreLane;                    ///< Start CORE lane
  UINT8                   EndCoreLane;                      ///< End Core lane
  UINT8                   PortId;                           ///< Logical Port ID
  UINT8                   LinkHotplug;                      ///< Hotplug Flags
  UINT8                   PciDev;                           ///< PCI Device Number
  UINT8                   PciFun;                           ///< PCI Function Number
} _ALIB_PORT_DATA;

/// Set size for Port Data
typedef union {
  _ALIB_PORT_DATA         PortData;                         ///< Port data for ALIB
  UINT8                   Padding[20];                      ///< Padding size
} ALIB_PORT_DATA;

/// _ALIB_GLOBAL_DATA structure
typedef  struct {
  UINT8                 PsppPolicy;                       ///< PSPP Policy
  UINT8                 PcieVidGen1;                      ///< VID for Gen1
  UINT8                 PcieVidGen2;                      ///< VID for Gen2
  UINT8                 PcieVidGen3;                      ///< VID for Gen3
  UINT8                 DpmMask;                          ///< DPM Mask
  UINT8                 NumBoostStates;                   ///< DPM States
  UINT8                 DockedTdpHeadroom;                ///< Docked TDP Headroom
} _ALIB_GLOBAL_DATA;

/// Set size for Global Data
typedef union {
  _ALIB_GLOBAL_DATA       Data;                             ///< Global data struct for ALIB
  UINT8                   Padding[32];                      ///< Padding size
} ALIB_GLOBAL_DATA;

/// ALIB_GLOBAL_DATA structure
typedef struct {
  ALIB_GLOBAL_DATA        Data;                             ///< Global data struct for ALIB
  ALIB_PORT_DATA          PortData[12];                     ///< Padding size
} ALIB_DATA;


#pragma pack (pop)

AGESA_STATUS
PcieAlibV2Feature (
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

#endif
