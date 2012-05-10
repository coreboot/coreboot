/**
 * @file
 *
 * Internal access to HT Interface, general purpose features.
 *
 * This file provides definitions used by HT internal modules.  The
 * external HT interface (in agesa.h) is accessed using these methods.
 * This keeps the HT Feature implementations abstracted from the HT
 * external interface.
 *
 * This file includes the interface support which is not removed with
 * various build options.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $        @e  \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
* 
* ***************************************************************************
*
*/

#ifndef _HT_INTERFACE_GENERAL_H_
#define _HT_INTERFACE_GENERAL_H_

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
 * Is PackageLink an Internal Link?
 */
BOOLEAN
IsPackageLinkInternal (
  IN       UINT8  PackageLink
  );

/**
 * Get the Socket number for a given Node number.
 *
 */
UINT8
GetSocketFromMap (
  IN       UINT8         Node,
  IN       STATE_DATA    *State
  );

/**
 * Ignore a Link.
 *
 */
FINAL_LINK_STATE
GetIgnoreLink (
  IN       UINT8        Node,
  IN       UINT8        Link,
  IN       IGNORE_LINK  *NbIgnoreLinkList,
  IN       STATE_DATA   *State
  );

/**
 * Get a new Socket Die to Node Map.
 *
 */
VOID
NewNodeAndSocketTables (
  IN OUT   STATE_DATA *State
  );

/**
 * Get the minimum Northbridge frequency for the system.
 *
 */
UINT32
GetMinNbCoreFreq (
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

/**
 * Fill in the socket's Node id when a processor is discovered in that socket.
 *
 */
VOID
SetNodeToSocketMap (
  IN       UINT8 Node,
  IN       UINT8 CurrentNodeModule,
  IN       UINT8 PackageLink,
  IN       UINT8 NewNode,
  IN       UINT8 HardwareSocket,
  IN       UINT8 Module,
  IN       STATE_DATA *State
  );

/**
 * Clean up the map structures after severe event has caused a fall back to 1 node.
 *
 */
VOID
CleanMapsAfterError (
  IN       STATE_DATA *State
  );

/**
 * Post Node id and other context info to AP cores via mailbox.
 *
 */
VOID
PostMapToAp (
  IN       STATE_DATA *State
  );

#endif  /* _HT_INTERFACE_GENERAL_H_ */
