/**
 * @file
 *
 * ASPM support.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
 *
 ***************************************************************************/
#ifndef _NBPCIEASPM_H_
#define _NBPCIEASPM_H_

VOID
PcieAsmpEnableOnPort (
  IN      PORT            PortId,
  IN      PCIE_LINK_ASPM  AsmpState,
  IN      AMD_NB_CONFIG   *pConfig
  );

UINT8
PcieAspmGetPmCapability (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  );


VOID
PcieAspmEnableOnDevice (
  IN      PCI_ADDR        Device,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  );

VOID
PcieAspmEnableOnFunction (
  IN      PCI_ADDR        Function,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  );

VOID
PcieAspmEnableOnLink (
  IN      PCI_ADDR        Downstream,
  IN      PCI_ADDR        Upstream,
  IN      UINT8           Lx,
  IN      AMD_NB_CONFIG   *pConfig
  );

VOID
PcieAspmEnableCommonClock (
  IN      PCI_ADDR        Downstream,
  IN      PCI_ADDR        Upstream,
  IN      AMD_NB_CONFIG   *pConfig
  );


VOID
PcieAspmCommonClockOnDevice (
  IN      PCI_ADDR        Device,
  IN      AMD_NB_CONFIG   *pConfig
  );

VOID
PcieAspmCommonClockOnFunction (
  IN      PCI_ADDR        Function,
  IN      AMD_NB_CONFIG   *pConfig
  );

#pragma pack (push, 1)

/// Framework for ASPM enable
typedef struct  {
  PCI_SCAN_PROTOCOL ScanPciePort;   ///< PCI scan protocol
  PCI_ADDR          DownstreamPort; ///< Downstream port to enable ASPM
  UINT8             MaxL0sLatency;  ///< TBD
  UINT8             MaxL1Latency;   ///< TBD
  UINT8             LinkCount;      ///< TBD
  UINT8             Lx;             ///< ASPM state to enable
} ASPM_WORKSPACE;

#define ASPM_UPSTREAM_L0s   BIT2
#define ASPM_DOWNSTREAM_L0s BIT3
#define ASPM_L1             BIT1
#define ASPM_L0s            BIT0

/// Framework for callback ASPM capability callback
typedef struct  {
  PCI_ADDR          DownstreamPort; ///< Downstream port PCI address to enable ASPM
  PCI_ADDR          UpstreamPort;   ///< Upstream port PCI address to enable ASPM
  UINT8             DownstreamLxCap; ///< Downstream port ASPM capability
  UINT8             UpstreamLxCap;  ///< Upstream port ASPM capability
  UINT8             DownstreamLx;   ///< Downstream port ASPM setting
  UINT8             UpstreamLx;     ///< Upstream port ASPM setting
  UINT8             RequestedLx;    ///< Requested port ASPM setting
} ASPM_LINK_INFO;

#pragma pack (pop)
#endif
