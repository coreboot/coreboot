/* $NoKeywords:$ */
/**
 * @file
 *
 * Internal access to HT Interface, for non-coherent features.
 *
 * This file provides definitions used by HT internal modules.  The
 * external HT interface (in agesa.h) is accessed using these methods.
 * This keeps the HT Feature implementations abstracted from the HT
 * interface.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $        @e  \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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

#ifndef _HT_INTERFACE_NONCOHERENT_H_
#define _HT_INTERFACE_NONCOHERENT_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         Prototypes to Interface from Feature Code
 *
 *----------------------------------------------------------------------------
 */

/**
 * Manually control bus number assignment.
 *
 */
BOOLEAN
GetOverrideBusNumbers (
  IN       UINT8        Node,
  IN       UINT8        Link,
     OUT   UINT8        *SecBus,
     OUT   UINT8        *SubBus,
  IN       STATE_DATA   *State
  );

/**
 * Get Manual BUID assignment list.
 *
 */
BOOLEAN
GetManualBuidSwapList (
  IN       UINT8            Node,
  IN       UINT8            Link,
     OUT   BUID_SWAP_LIST   **List,
  IN       STATE_DATA       *State
  );

/**
 * Override capabilities of a device.
 *
 */

VOID
GetDeviceCapOverride (
  IN       UINT8       HostNode,
  IN       UINT8       HostLink,
  IN       UINT8       Depth,
  IN       PCI_ADDR    PciAddress,
  IN       UINT32      DevVenId,
  IN       UINT8       Revision,
  IN       UINT8       Link,
  IN OUT   UINT8       *LinkWidthIn,
  IN OUT   UINT8       *LinkWidthOut,
  IN OUT   UINT32      *FreqCap,
  IN OUT   UINT32      *Clumping,
  IN       STATE_DATA  *State
  );

/**
 * Get limits for non-coherent Links.
 *
 */
VOID
GetIoPcbLimits (
  IN       UINT8        HostNode,
  IN       UINT8        HostLink,
  IN       UINT8        Depth,
  IN OUT   UINT8        *DownstreamLinkWidthLimit,
  IN OUT   UINT8        *UpstreamLinkWidthLimit,
  IN OUT   UINT32       *PcbFreqCap,
  IN       STATE_DATA   *State
  );

#endif  /* _HT_INTERFACE_NONCOHERENT_H_ */
