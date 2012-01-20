/* $NoKeywords:$ */
/**
 * @file
 *
 * Eight node Fully Connected Topology.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  HyperTransport
 * @e \$Revision: 56279 $   @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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

#include "Porting.h"
#include "htTopologies.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

/**
 * 8 node fully connected
 */
/**
 * @dot
   strict graph full8 {
     node [shape="plaintext"];
     0 -- 1 ;    1 -- 2 ;   2 -- 3 ;   3 -- 4 ;   4 -- 5 ; 5 -- 6 ;  6 -- 7 ;
     0 -- 2 ;    1 -- 3 ;   2 -- 4 ;   3 -- 5 ;   4 -- 6 ; 5 -- 7 ;
     0 -- 3 ;    1 -- 4 ;   2 -- 5 ;   3 -- 6 ;   4 -- 7 ;
     0 -- 4 ;    1 -- 5 ;   2 -- 6 ;   3 -- 7 ;
     0 -- 5 ;    1 -- 6 ;   2 -- 7 ;
     0 -- 6 ;    1 -- 7 ;
     0 -- 7 ;
   }
 @enddot
 *
 */
CONST UINT8 ROMDATA amdHtTopologyEightFully[] =
{
  0x08,
  0xFE, 0xFF,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44,  0x00, 0x55,  0x00, 0x66,  0x00, 0x77, // Node 0
  0x00, 0x00,  0xFD, 0xFF,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44,  0x00, 0x55,  0x00, 0x66,  0x00, 0x77, // Node 1
  0x00, 0x00,  0x00, 0x11,  0xFB, 0xFF,  0x00, 0x33,  0x00, 0x44,  0x00, 0x55,  0x00, 0x66,  0x00, 0x77, // Node 2
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0xF7, 0xFF,  0x00, 0x44,  0x00, 0x55,  0x00, 0x66,  0x00, 0x77, // Node 3
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0xEF, 0xFF,  0x00, 0x55,  0x00, 0x66,  0x00, 0x77, // Node 4
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44,  0xDF, 0xFF,  0x00, 0x66,  0x00, 0x77, // Node 5
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44,  0x00, 0x55,  0xBF, 0xFF,  0x00, 0x77, // Node 6
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44,  0x00, 0x55,  0x00, 0x66,  0x7F, 0xFF  // Node 7
};
