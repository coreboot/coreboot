/* $NoKeywords:$ */
/**
 * @file
 *
 * various service procedures
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
*****************************************************************************
*
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#ifndef _GFXGMCINITTN_H_
#define _GFXGMCINITTN_H_

#include  "GnbRegistersTN.h"

AGESA_STATUS
GfxGmcInitTN (
  IN      GFX_PLATFORM_CONFIG   *Gfx
  );

#pragma pack (push, 1)

/// DCT channel information
typedef struct {
  D18F2x94_dct0_STRUCT      D18F2x94_dct0;       ///< Register 0x94
  D18F2x94_dct1_STRUCT      D18F2x94_dct1;       ///< Register 0x94
  D18F2x2E0_dct0_STRUCT     D18F2x2E0_dct0;      ///< Register 0x2E0
  D18F2x2E0_dct1_STRUCT     D18F2x2E0_dct1;      ///< Register 0x2E0
  D18F2x200_dct0_mp0_STRUCT D18F2x200_dct0_mp0;  ///< Register 0x200
  D18F2x200_dct0_mp1_STRUCT D18F2x200_dct0_mp1;  ///< Register 0x200
  D18F2x200_dct1_mp0_STRUCT D18F2x200_dct1_mp0;  ///< Register 0x200
  D18F2x200_dct1_mp1_STRUCT D18F2x200_dct1_mp1;  ///< Register 0x200
  D18F2x204_dct0_mp0_STRUCT D18F2x204_dct0_mp0;  ///< Register 0x204
  D18F2x204_dct0_mp1_STRUCT D18F2x204_dct0_mp1;  ///< Register 0x204
  D18F2x204_dct1_mp0_STRUCT D18F2x204_dct1_mp0;  ///< Register 0x204
  D18F2x204_dct1_mp1_STRUCT D18F2x204_dct1_mp1;  ///< Register 0x204
  D18F2x22C_dct0_mp0_STRUCT D18F2x22C_dct0_mp0;  ///< Register 0x22C
  D18F2x22C_dct0_mp1_STRUCT D18F2x22C_dct0_mp1;  ///< Register 0x22C
  D18F2x22C_dct1_mp0_STRUCT D18F2x22C_dct1_mp0;  ///< Register 0x22C
  D18F2x22C_dct1_mp1_STRUCT D18F2x22C_dct1_mp1;  ///< Register 0x22C
  D18F2x21C_dct0_mp0_STRUCT D18F2x21C_dct0_mp0;  ///< Register 0x21C
  D18F2x21C_dct0_mp1_STRUCT D18F2x21C_dct0_mp1;  ///< Register 0x21C
  D18F2x21C_dct1_mp0_STRUCT D18F2x21C_dct1_mp0;  ///< Register 0x21C
  D18F2x21C_dct1_mp1_STRUCT D18F2x21C_dct1_mp1;  ///< Register 0x21C
  D18F2x20C_dct0_mp0_STRUCT D18F2x20C_dct0_mp0;  ///< Register 0x20C
  D18F2x20C_dct0_mp1_STRUCT D18F2x20C_dct0_mp1;  ///< Register 0x20C
  D18F2x20C_dct1_mp0_STRUCT D18F2x20C_dct1_mp0;  ///< Register 0x20C
  D18F2x20C_dct1_mp1_STRUCT D18F2x20C_dct1_mp1;  ///< Register 0x20C
} DCT_CHANNEL_INFO;

/// DCT_CHANNEL_INFO field entry
typedef struct {
  UINT8                 RegisterSpaceType;              ///< Register type
  UINT32                Address;                        ///< Register address
  UINT16                DctChannelInfoTableOffset;      ///< destination offset in DCT_CHANNEL_INFO table
} DCT_REGISTER_ENTRY;

#pragma pack (pop)

#endif
