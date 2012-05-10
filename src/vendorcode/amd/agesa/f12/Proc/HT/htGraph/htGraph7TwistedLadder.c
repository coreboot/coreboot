/* $NoKeywords:$ */
/**
 * @file
 *
 * Seven node pop order twisted ladder Topology.
 *
 * The population order fall back to Seven nodes on a twisted ladder.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 44324 $   @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
 * ***************************************************************************
 *
 */

#include "Porting.h"
#include "htTopologies.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)


/*  6
 *  |
 *  4   5
 *  |\ /|
 *  |/ \|
 *  2   3
 *  |   |
 *  0---1
 */
/**
 * 7 node twisted ladder
 */
/**
 * @dot
   strict graph twl7 {
     node [shape="plaintext"];
     {rank=same; 0; 1}
     {rank=same; 2; 3}
     {rank=same; 4; 5}
     {rank=same; 6}
     0 -- 1 ;
     0 -- 2 ;
     1 -- 3 ;
     2 -- 4 ;
     2 -- 5 ;
     3 -- 4 ;
     3 -- 5 ;
     4 -- 6 ;
   }
 @enddot
 *
 */
CONST UINT8 ROMDATA amdHtTopologySevenTwistedLadder[] =
{
  0x07,
  0x06, 0xFF,  0x00, 0x11,  0x02, 0x22,  0x00, 0x12,  0x00, 0x22,  0x00, 0x22,  0x00, 0x22, // Node0
  0x00, 0x00,  0x09, 0xFF,  0x00, 0x03,  0x01, 0x33,  0x00, 0x33,  0x00, 0x33,  0x00, 0x33, // Node1
  0x30, 0x00,  0x00, 0x50,  0x31, 0xFF,  0x00, 0x54,  0x21, 0x44,  0x01, 0x55,  0x21, 0x44, // Node2
  0x00, 0x41,  0x30, 0x11,  0x00, 0x45,  0x32, 0xFF,  0x02, 0x44,  0x12, 0x55,  0x02, 0x44, // Node3
  0x48, 0x22,  0x40, 0x33,  0x48, 0x22,  0x40, 0x33,  0x4C, 0xFF,  0x40, 0x32,  0x0C, 0x66, // Node4
  0x00, 0x22,  0x04, 0x33,  0x00, 0x22,  0x04, 0x33,  0x00, 0x23,  0x0C, 0xFF,  0x00, 0x23, // Node5
  0x00, 0x44,  0x00, 0x44,  0x00, 0x44,  0x00, 0x44,  0x00, 0x44,  0x00, 0x44,  0x10, 0xFF  // Node6
};
