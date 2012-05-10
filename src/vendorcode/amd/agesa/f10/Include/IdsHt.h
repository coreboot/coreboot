/**
 * @file
 *
 * AMD IDS HyperTransport Definitions
 *
 * Contains AMD AGESA Integrated Debug HT related items.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 */
/*****************************************************************************
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
 * 
 *
 ***************************************************************************/

#ifndef _IDS_HT_H_
#define _IDS_HT_H_

// Frequency equates for call backs which take an actual frequency setting
#define HT_FREQUENCY_200M            0
#define HT_FREQUENCY_400M            2
#define HT_FREQUENCY_600M            4
#define HT_FREQUENCY_800M            5
#define HT_FREQUENCY_1000M           6
#define HT_FREQUENCY_1200M           7
#define HT_FREQUENCY_1400M           8
#define HT_FREQUENCY_1600M           9
#define HT_FREQUENCY_1800M           10
#define HT_FREQUENCY_2000M           11
#define HT_FREQUENCY_2200M           12
#define HT_FREQUENCY_2400M           13
#define HT_FREQUENCY_2600M           14
#define HT_FREQUENCY_2800M           17
#define HT_FREQUENCY_3000M           18
#define HT_FREQUENCY_3200M           19
#define HT_FREQUENCY_3600M           20

/**
 * HT IDS: HT Link Port Override params.
 *
 * Provide an absolute override of HT Link Port settings.  No checking is done that
 * the settings obey limits or capabilities, this responsibility rests with the user.
 *
 * Rules for values of structure items:
 * - Socket
 *   - HT_LIST_TERMINAL == end of port override list, rest of item is not accessed
 *   - HT_LIST_MATCH_ANY == Match Any Socket
 *   - 0 .. 7 == The matching socket
 * - Link
 *   - HT_LIST_MATCH_ANY == Match Any package link (that is not the internal links)
 *   - HT_LIST_MATCH_INTERNAL_LINK == Match the internal links
 *   - 0 .. 7 == The matching package link.  0 .. 3 are the ganged links or sublink 0's, 4 .. 7 are the sublink1's.
 * - Frequency
 *   - HT_LIST_TERMINAL == Do not override the frequency, AUTO setting
 *   - HT_FREQUENCY_200M .. HT_FREQUENCY_3600M = The frequency value to use
 * - Widthin
 *   - HT_LIST_TERMINAL == Do not override the width, AUTO setting
 *   - 2, 4, 8, 16, 32 == The width value to use
 * - Widthout
 *   - HT_LIST_TERMINAL == Do not override the width, AUTO setting
 *   - 2, 4, 8, 16, 32 == The width value to use
 */
typedef struct {
  // Match Fields
  UINT8       Socket;      ///< The Socket which this port is on.
  UINT8       Link;        ///< The port for this package link on that socket.
  // Override fields
  UINT8       Frequency;   ///< Absolutely override the port's frequency.
  UINT8       WidthIn;     ///< Absolutely override the port's width.
  UINT8       WidthOut;    ///< Absolutely override the port's width.
} HTIDS_PORT_OVERRIDE;

/**
 * A list of port overrides to search.
 */
typedef HTIDS_PORT_OVERRIDE *HTIDS_PORT_OVERRIDE_LIST;
VOID
HtIdsGetPortOverride (
  IN       BOOLEAN                    IsSourcePort,
  IN OUT   PORT_DESCRIPTOR            *Port0,
  IN OUT   PORT_DESCRIPTOR            *Port1,
  IN OUT   HTIDS_PORT_OVERRIDE_LIST   *PortOverrideList,
  IN       STATE_DATA                 *State
  );

typedef
VOID
F_HtIdsGetPortOverride (
  IN       BOOLEAN                    IsSourcePort,
  IN OUT   PORT_DESCRIPTOR            *Port0,
  IN OUT   PORT_DESCRIPTOR            *Port1,
  IN OUT   HTIDS_PORT_OVERRIDE_LIST   *PortOverrideList,
  IN       STATE_DATA                 *State
  );
typedef F_HtIdsGetPortOverride* PF_HtIdsGetPortOverride;
#endif // _IDS_HT_H
