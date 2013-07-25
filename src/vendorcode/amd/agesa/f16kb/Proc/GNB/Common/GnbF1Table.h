/* $NoKeywords:$ */
/**
 * @file
 *
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 84159 $   @e \$Date: 2012-12-12 20:20:16 -0600 (Wed, 12 Dec 2012) $
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


#ifndef _GNBF1TABLE_H_
#define _GNBF1TABLE_H_

#pragma pack (push, 1)

#define PP_F1_MAX_NUM_DPM_STATE 5
#define PP_F1_MAX_NUM_SW_STATE  6

typedef struct  {
  UINT8                       PPlayTableRev;              ///< PP table revision
  UINT8                       PP_FUSE_ARRAY_V2_fld1[5];
  UINT8                       PP_FUSE_ARRAY_V2_fld2[5];
  UINT8                       PP_FUSE_ARRAY_V2_fld3;
  UINT8                       PP_FUSE_ARRAY_V2_fld4[6];
  UINT8                       PP_FUSE_ARRAY_V2_fld5[5];
  UINT8                       excel841_fld6[5];              ///< DispClk DID
  UINT8                       PcieGen2Vid;                ///< Pcie Gen 2 VID
  UINT8                       MainPllId;                  ///< Main PLL Id from fuses
  UINT8                       WrCkDid;                    ///< WRCK SMU clock Divisor
  UINT8                       GpuBoostCap;                ///< GPU boost cap
  UINT32                      PP_FUSE_ARRAY_V2_fld11;
  UINT32                      PP_FUSE_ARRAY_V2_fld12;
  BOOLEAN                     PP_FUSE_ARRAY_V2_fld13;
  UINT8                       VceFlags[5];                ///< VCE Flags
  UINT8                       VceMclk;                    ///< MCLK for VCE
  UINT8                       PP_FUSE_ARRAY_V2_fld16[4];
  UINT8                       EclkDid[5];                 ///< Eclk DID
  UINT8                       HtcEn;                      ///< HTC Enabled
  UINT8                       HtcTmpLmt;                  ///< HTC Temperature Limit
  UINT8                       PP_FUSE_ARRAY_V2_fld20;
  UINT8                       PP_FUSE_ARRAY_V2_fld21;
  UINT8                       DisDllShutdownSR[4];        ///< DRAM Configuration Low [per DCT]
  UINT8                       MemClkFreq[4];              ///< DRAM Configuration High [per DCT]
  UINT8                       MemPhyPllPdMode[4];         ///< DRAM Controller Miscellaneous 2 [per DCT]
  UINT8                       M1MemClkFreq[4];            ///< Memory PState Control and Status [per DCT]
  UINT8                       PP_FUSE_ARRAY_V2_fld26[4];
  UINT8                       PP_FUSE_ARRAY_V2_fld27[4];
  UINT8                       PP_FUSE_ARRAY_V2_fld28[4];
  UINT8                       PP_FUSE_ARRAY_V2_fld29[4];
  UINT8                       PP_FUSE_ARRAY_V2_fld30[4];
  UINT8                       PP_FUSE_ARRAY_V2_fld31[4];
  UINT8                       PP_FUSE_ARRAY_V2_fld32[5];
  UINT8                       PP_FUSE_ARRAY_V2_fld33[5];
  UINT8                       PP_FUSE_ARRAY_V2_fld34[5];
  UINT8                       PP_FUSE_ARRAY_V2_fld35[5];
  UINT8                       PP_FUSE_ARRAY_V2_fld36;
  UINT8                       PP_FUSE_ARRAY_V2_fld37;
  UINT8                       PP_FUSE_ARRAY_V2_fld38;
} PP_F1_ARRAY_V2;

#pragma pack (pop)

#endif

