/* $NoKeywords:$ */
/**
 * @file
 *
 * Northbridge utility routines.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/**
 * Return the HT Host capability base PCI config address for a Link.
 *
 */
PCI_ADDR
MakeLinkBase (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Return the LinkFailed status AFTER an attempt is made to clear the bit.
 *
 */
BOOLEAN
ReadTrueLinkFailStatus (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Write the total number of cores and Nodes to the Node
 *
 */
VOID
SetTotalNodesAndCores (
  IN       UINT8       Node,
  IN       UINT8       TotalNodes,
  IN       UINT8       TotalCores,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the Count (1 based) of Nodes in the system.
 *
 */
UINT8
GetNodeCount (
  IN       NORTHBRIDGE *Nb
  );

/**
 * Limit coherent config accesses to cpus as indicated by Nodecnt.
 *
 */
VOID
LimitNodes (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the Package Link number, given the node and real link number.
 *
 */
UINT8
GetPackageLink (
  IN       UINT8       Node,
  IN       UINT8       Link,
  IN       NORTHBRIDGE *Nb
  );
