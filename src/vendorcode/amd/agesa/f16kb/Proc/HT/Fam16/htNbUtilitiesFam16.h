/* $NoKeywords:$ */
/**
 * @file
 *
 * Northbridge utility routines.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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

#ifndef _HT_NB_UTILITIES_FAM16_H_
#define _HT_NB_UTILITIES_FAM16_H_

/**
 * Return the number of cores (1 based count) on Node.
 *
 */
UINT8
Fam16GetNumCoresOnNode (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

UINT8
Fam16GetNodeCount (
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the enable compute unit status for this node.
 */
UINT8
Fam16GetEnabledComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the dual core compute unit status for this node.
 */
UINT8
Fam16GetDualcoreComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the triple core compute unit status for this node.
 */
UINT8
Fam16GetTriplecoreComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Get the quad core compute unit status for this node.
 */
UINT8
Fam16GetQuadcoreComputeUnits (
  IN       UINT8       Node,
  IN       NORTHBRIDGE *Nb
  );

/**
 * Write the total number of cores to the Node
 *
 */
VOID
Fam16SetTotalCores (
  IN       UINT8       Node,
  IN       UINT8       TotalNodes,
  IN       UINT8       TotalCores,
  IN       NORTHBRIDGE *Nb
  );

#endif // _HT_NB_UTILITIES_FAM15MOD1X_H_
