/* $NoKeywords:$ */
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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
 *
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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
