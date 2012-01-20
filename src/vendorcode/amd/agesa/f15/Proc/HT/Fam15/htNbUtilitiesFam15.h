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
 * Return the number of cores (1 based count) on Node.
 *
 */
UINT8
Fam15GetNumCoresOnNode (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the next link for iterating over the links on a node in the correct order.
 *
 */
LINK_ITERATOR_STATUS
Fam15GetNextLink (
  IN       UINT8       Node,
  IN OUT   UINT8       *Link,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get Info about Module Type of this northbridge
 *
 */
VOID
Fam15GetModuleInfo (
  IN       UINT8       Node,
     OUT   UINT8       *ModuleType,
     OUT   UINT8       *Module,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Implement the hardware method of doing Socket Naming, by accessing this northbridge's Socket Id register.
 *
 */
UINT8
Fam15GetSocket (
  IN       UINT8       Node,
  IN       UINT8       TempNode,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Implement the hardware method of doing Socket Naming, by accessing this northbridge's Socket Id register.
 *
 */
UINT8
Fam15StrappedGetSocket (
  IN       UINT8       Node,
  IN       UINT8       TempNode,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the enable compute unit status for this node.
 */
UINT8
Fam15GetEnabledComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the dual core compute unit status for this node.
 */
UINT8
Fam15GetDualcoreComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Post info to AP cores via a mailbox.
 *
 */
VOID
Fam15PostMailbox (
  IN       UINT8 Node,
  IN       AP_MAILBOXES ApMailboxes,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Retrieve info from a node's mailbox.
 *
 */
AP_MAIL_INFO
Fam15RetrieveMailbox (
  IN       UINT8 Node,
  IN       NORTHBRIDGE *Nb
  );
