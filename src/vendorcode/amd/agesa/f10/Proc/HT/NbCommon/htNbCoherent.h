/**
 * @file
 *
 * Coherent Feature Northbridge common routines.
 *
 * Provide access to hardware for routing, coherent discovery.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */

/***************************************************************************
 ***               FAMILY/NORTHBRIDGE GENERIC FUNCTIONS                 ***
 ***************************************************************************/

/**
 * Establish a Temporary route from one Node to another.
 *
 */
VOID
WriteRoutingTable (
  IN       UINT8       Node,
  IN       UINT8       Target,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Modifies the NodeID register on the target Node
 *
 */
VOID
WriteNodeID (
  IN       UINT8       Node,
  IN       UINT8       NodeID,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Read the Default Link
 *
 */
UINT8
ReadDefaultLink (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Turns routing tables on for a given Node
 *
 */
VOID
EnableRoutingTables (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Turns routing tables off for a given Node
 *
 */
VOID
DisableRoutingTables (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Verify that the Link is coherent, connected, and ready
 *
*/
BOOLEAN
VerifyLinkIsCoherent (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Read the token stored in the scratchpad register field.
 *
 */
UINT8
ReadToken (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Write the token stored in the scratchpad register
 *
 */
VOID
WriteToken (
  IN       UINT8       Node,
  IN       UINT8       Value,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Full Routing Table Register initialization
 *
 */
VOID
WriteFullRoutingTable (
  IN       UINT8       Node,
  IN       UINT8       Target,
  IN       UINT8       ReqLink,
  IN       UINT8       RspLink,
  IN       UINT32      BroadcastLinks,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Determine whether a Node is compatible with the discovered configuration so far.
 *
 */
BOOLEAN
IsIllegalTypeMix (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Fix (hopefully) exceptional conditions.
 *
 */
BOOLEAN
HandleSpecialNodeCase (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );

