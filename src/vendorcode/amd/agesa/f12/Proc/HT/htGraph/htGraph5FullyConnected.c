/* $NoKeywords:$ */
/**
 * @file
 *
 * Five node Fully Connected Topology.
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


/**
 * Five node fully connected
 */
/**
 * @dot
   strict graph full5 {
     node [shape="plaintext"];
     0 -- 1 ;
     0 -- 2 ;
     0 -- 3 ;
     0 -- 4 ;
     1 -- 2 ;
     1 -- 3 ;
     1 -- 4 ;
     2 -- 3 ;
     2 -- 4 ;
     3 -- 4 ;
   }
 @enddot
 *
 */
CONST UINT8 ROMDATA amdHtTopologyFiveFully[] =
{
  0x05,
  0x1E, 0xFF,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44, // Node 0
  0x00, 0x00,  0x1D, 0xFF,  0x00, 0x22,  0x00, 0x33,  0x00, 0x44, // Node 1
  0x00, 0x00,  0x00, 0x11,  0x1B, 0xFF,  0x00, 0x33,  0x00, 0x44, // Node 2
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0x17, 0xFF,  0x00, 0x44, // Node 3
  0x00, 0x00,  0x00, 0x11,  0x00, 0x22,  0x00, 0x33,  0x0F, 0xFF  // Node 4
};
