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
 * @e \$Revision: 63425 $        @e  \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
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
