/* $NoKeywords:$ */
/**
 * @file
 *
 * Link optimization generic support.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
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

/**
 * Get Link features into system data structure.
 *
 */
VOID
GatherLinkFeatures (
  IN OUT   PORT_DESCRIPTOR        *ThisPort,
  IN       HT_INTERFACE           *Interface,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       NORTHBRIDGE            *Nb
  );

/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 *
 */
VOID
SetLinkRegang (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Set the link's Unit Id Clumping enable.
 *
 */
VOID
SetLinkUnitIdClumping (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       UINT32      ClumpingEnables,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Change the hardware state for all Links according to the now optimized data in the
 * port list data structure.
 */
VOID
SetLinkFrequency (
  IN       UINT8          Node,
  IN       UINT8          Link,
  IN       UINT8          Frequency,
  IN       NORTHBRIDGE    *Nb
  );
