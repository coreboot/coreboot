/* $NoKeywords:$ */
/**
 * @file
 *
 * Eight node hydra Topology using "Doubloon/Drachma".
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
 * 8 node hydra doubloon, 4 MCM processors.
 */
/**
 * @dot
   strict graph doubloon8 {
     node [shape="plaintext"];
     0 -- 1 ;    2 -- 3 ;   4 -- 5 ;   6 -- 7 ;
     0 -- 2 ;    1 -- 2 ;   2 -- 4 ;   3 -- 5 ;   4 -- 6 ; 5 -- 6 ;
     0 -- 4 ;    1 -- 5 ;   2 -- 6 ;   3 -- 7 ;
     0 -- 6 ;    1 -- 7 ;   2 -- 7 ;   3 -- 6
   }
 @enddot
 *
 */
CONST UINT8 ROMDATA amdHtTopologyEightDoubloon[] =
{
  0x08,
  0x56, 0xFF,  0x00, 0x11,  0x00, 0x22,  0x00, 0x22,  0x02, 0x44,  0x00, 0x44,  0x02, 0x66,  0x00, 0x66, // Node 0
  0x00, 0x00,  0xA5, 0xFF,  0x00, 0x22,  0x00, 0x22,  0x00, 0x55,  0x01, 0x55,  0x00, 0x77,  0x01, 0x77, // Node 1
  0x08, 0x00,  0x08, 0x11,  0xDB, 0xFF,  0x03, 0x33,  0x08, 0x44,  0x00, 0x44,  0x00, 0x66,  0x00, 0x77, // Node 2
  0x00, 0x22,  0x00, 0x22,  0x00, 0x22,  0xE4, 0xFF,  0x00, 0x55,  0x04, 0x55,  0x00, 0x66,  0x00, 0x77, // Node 3
  0x20, 0x00,  0x00, 0x00,  0x20, 0x22,  0x00, 0x22,  0x65, 0xFF,  0x00, 0x55,  0x00, 0x66,  0x00, 0x66, // Node 4
  0x00, 0x11,  0x10, 0x11,  0x00, 0x33,  0x10, 0x33,  0x00, 0x44,  0x5A, 0xFF,  0x00, 0x66,  0x00, 0x66, // Node 5
  0x80, 0x00,  0x00, 0x00,  0x00, 0x22,  0x00, 0x33,  0x80, 0x44,  0x80, 0x55,  0xBD, 0xFF,  0x30, 0x77, // Node 6
  0x00, 0x11,  0x40, 0x11,  0x00, 0x22,  0x00, 0x33,  0x00, 0x66,  0x00, 0x66,  0x00, 0x66,  0x4E, 0xFF  // Node 7
};
