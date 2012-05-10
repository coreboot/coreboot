/* $NoKeywords:$ */
/**
 * @file
 *
 * Graphics controller BIF straps control services.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 45407 $   @e \$Date: 2011-01-17 15:28:58 +0800 (Mon, 17 Jan 2011) $
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


#ifndef _GNBFUSETABLE_H_
#define _GNBFUSETABLE_H_

#pragma pack (push, 1)

#define PP_FUSE_MAX_NUM_DPM_STATE 5
#define PP_FUSE_MAX_NUM_SW_STATE  6

/// Fuse definition structure
typedef struct  {
  UINT8                       PPlayTableRev;              ///< PP table revision
  UINT8                       SclkDpmValid[6];            ///< Valid DPM states
  UINT8                       SclkDpmDid[6];              ///< Sclk DPM DID
  UINT8                       SclkDpmVid[6];              ///< Sclk DPM VID
  UINT8                       SclkDpmCac[5];              ///< Sclk DPM Cac
  UINT8                       PolicyFlags[6];             ///< State policy flags
  UINT8                       PolicyLabel[6];             ///< State policy label
  UINT8                       VclkDid[4];                 ///< VCLK DID
  UINT8                       DclkDid[4];                 ///< DCLK DID
  UINT8                       SclkThermDid;               ///< Thermal SCLK
  UINT8                       VclkDclkSel[6];             ///< Vclk/Dclk selector
  UINT8                       LclkDpmValid[4];            ///< Valid Lclk DPM states
  UINT8                       LclkDpmDid[4];              ///< Lclk DPM DID
  UINT8                       LclkDpmVid[4];              ///< Lclk DPM VID
  UINT8                       DisplclkDid[4];             ///< Displclk DID
  UINT8                       PcieGen2Vid;                ///< Pcie Gen 2 VID
  UINT8                       MainPllId;                  ///< Main PLL Id from fuses
  UINT8                       WrCkDid;                    ///< WRCK SMU clock Divisor
  UINT8                       SclkVid[4];                 ///< Sclk VID
  UINT8                       GpuBoostCap;                ///< GPU boost cap
  UINT16                      SclkDpmTdpLimit[6];         ///< Sclk DPM TDP limit
  UINT16                      SclkDpmTdpLimitPG;          ///< TDP limit PG
  UINT32                      SclkDpmBoostMargin;         ///< Boost margin
  UINT32                      SclkDpmThrottleMargin;      ///< Throttle margin
} PP_FUSE_ARRAY;

#pragma pack (pop)

#endif

