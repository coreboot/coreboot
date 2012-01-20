/* $NoKeywords:$ */
/**
 * @file
 *
 * System Tuning Family 15h specific routines
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 50215 $   @e \$Date: 2011-04-05 20:50:13 -0600 (Tue, 05 Apr 2011) $
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
 * Set the traffic distribution register for the Links provided.
 *
 */
VOID
Fam15WriteTrafficDistribution (
  IN       UINT32       Links01,
  IN       UINT32       Links10,
  IN       NORTHBRIDGE  *Nb
  );

/**
 * Write a link pair to the link pair distribution and fixups.
 *
 */
VOID
Fam15WriteLinkPairDistribution (
  IN       UINT8        Node,
  IN       UINT8        ConnectedNode,
  IN       UINT8        Pair,
  IN       BOOLEAN      Asymmetric,
  IN       UINT8        MasterLink,
  IN       UINT8        AlternateLink,
  IN       NORTHBRIDGE  *Nb
  );

/**
 * Family 15h specific tunings.
 *
 */
VOID
Fam15WriteVictimDistribution (
  IN       UINT8        NodeA,
  IN       UINT8        NodeB,
  IN       UINT32       LinksAB,
  IN       UINT32       LinksBA,
  IN       NORTHBRIDGE  *Nb
  );

/**
 * Family 15h specific tunings.
 *
 */
VOID
Fam15BufferOptimizations (
  IN       UINT8       Node,
  IN       STATE_DATA  *State,
  IN       NORTHBRIDGE *Nb
  );
