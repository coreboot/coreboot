/* $NoKeywords:$ */
/**
 * @file
 *
 * Eight node Ladder Topology.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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

#include "Porting.h"
#include "htTopologies.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)


/*  6---7
 *  |   |
 *  4---5
 *  |   |
 *  2---3
 *  |   |
 *  0---1
 */
/**
 * 8 node ladder
 */
/**
 * @dot
   strict graph ladder8 {
     node [shape="plaintext"];
     {rank=same; 0; 1}
     {rank=same; 2; 3}
     {rank=same; 4; 5}
     {rank=same; 6; 7}
     0 -- 1 ;
     0 -- 2 ;
     1 -- 3 ;
     2 -- 4 ;
     2 -- 3 ;
     3 -- 5 ;
     4 -- 5 ;
     4 -- 6 ;
     5 -- 7 ;
     6 -- 7 ;
   }
 @enddot
 *
 */
CONST UINT8 ROMDATA amdHtTopologyEightStraightLadder[] =
{
  0x08,
  0x06, 0xFF,  0x00, 0x11,  0x02, 0x22,  0x00, 0x22,  0x02, 0x22,  0x00, 0x22,  0x02, 0x22,  0x00, 0x22, // Node0
  0x00, 0x00,  0x09, 0xFF,  0x00, 0x33,  0x01, 0x33,  0x00, 0x33,  0x01, 0x33,  0x00, 0x33,  0x01, 0x33, // Node1
  0x18, 0x00,  0x00, 0x00,  0x19, 0xFF,  0x00, 0x33,  0x09, 0x44,  0x00, 0x44,  0x09, 0x44,  0x00, 0x44, // Node2
  0x00, 0x11,  0x24, 0x11,  0x00, 0x22,  0x26, 0xFF,  0x00, 0x55,  0x06, 0x55,  0x00, 0x55,  0x06, 0x55, // Node3
  0x60, 0x22,  0x00, 0x22,  0x60, 0x22,  0x00, 0x22,  0x64, 0xFF,  0x00, 0x55,  0x24, 0x66,  0x00, 0x66, // Node4
  0x00, 0x33,  0x90, 0x33,  0x00, 0x33,  0x90, 0x33,  0x00, 0x44,  0x98, 0xFF,  0x00, 0x77,  0x18, 0x77, // Node5
  0x80, 0x44,  0x00, 0x44,  0x80, 0x44,  0x00, 0x44,  0x80, 0x44,  0x00, 0x44,  0x90, 0xFF,  0x00, 0x77, // Node6
  0x00, 0x55,  0x40, 0x55,  0x00, 0x55,  0x40, 0x55,  0x00, 0x55,  0x40, 0x55,  0x00, 0x66,  0x60, 0xFF  // Node7
};

