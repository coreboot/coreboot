/**
 * @file
 *
 * GNB init tables
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 86714 $   @e \$Date: 2013-01-24 17:51:46 -0600 (Thu, 24 Jan 2013) $
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbRegistersKB.h"
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

CONST GNB_TABLE ROMDATA GnbEarlierInitTableBeforeSmuKB [] = {
  GNB_ENTRY_RMW (
    D0F0x98_x07_TYPE,
    D0F0x98_x07_ADDRESS,
    D0F0x98_x07_SMUCsrIsocEn_MASK,
    (1 << D0F0x98_x07_SMUCsrIsocEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x1E_TYPE,
    D0F0x98_x1E_ADDRESS,
    D0F0x98_x1E_HiPriEn_MASK,
    (1 << D0F0x98_x1E_HiPriEn_OFFSET)
    ),
  GNB_ENTRY_TERMINATE
};

CONST GNB_TABLE ROMDATA GnbEarlyInitTableKB [] = {
  // Set SVI2
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_SVI2,
    0x4,
    0x3F9D8,
    0x20000000,
    (1 << 29)
    ),
  GNB_ENTRY_WR (
    D0F0x04_TYPE,
    D0F0x04_ADDRESS,
    (0x1 << D0F0x04_MemAccessEn_WIDTH)
    ),
  GNB_ENTRY_RMW (
    D0F0x64_x16_TYPE,
    D0F0x64_x16_ADDRESS,
    D0F0x64_x16_AerUrMsgEn_MASK,
    0x0 << D0F0x64_x16_AerUrMsgEn_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x07_TYPE,
    D0F0x98_x07_ADDRESS,
    D0F0x98_x07_IocBwOptEn_MASK | D0F0x98_x07_DropZeroMaskWrEn_MASK,
    (1 << D0F0x98_x07_IocBwOptEn_OFFSET) | (1 << D0F0x98_x07_DropZeroMaskWrEn_OFFSET)
    ),

  GNB_ENTRY_RMW (
    D0F0x98_x0C_TYPE,
    D0F0x98_x0C_ADDRESS,
    D0F0x98_x0C_GcmWrrLenA_MASK | D0F0x98_x0C_GcmWrrLenB_MASK,
    (0x8 << D0F0x98_x0C_GcmWrrLenA_OFFSET) | (0x8 << D0F0x98_x0C_GcmWrrLenB_OFFSET)
    ),

  // Enable voltage controller
  GNB_ENTRY_WR (
    SMU_MSG_TYPE,
    SMC_MSG_VOLTAGE_CNTL_ENABLE,
    0
    ),
//---------------------------------------------------------------------------
  GNB_ENTRY_COPY (
    0x9,
    0x49,
    0,
    32,
    D0F0x64_x1F_TYPE,
    D0F0x64_x1F_ADDRESS,
    0,
    32
    ),
  // Enable VPC
  //  CSR_GNB_1.SviTrimValueVdd = Vdd Trim
  GNB_ENTRY_COPY (
    0x4,
    0x3F9F4,
    16, 8,
    D18F5x12C_TYPE,
    D18F5x12C_ADDRESS,
    D18F5x12C_CoreLoadLineTrim_OFFSET, D18F5x12C_CoreLoadLineTrim_WIDTH
  ),
  //  CSR_GNB_1.SviTrimValueVddNB = VddNB Trim
  GNB_ENTRY_COPY (
      0x4,
      0x3F9F4,
      24, 8,
      D18F5x188_TYPE,
      D18F5x188_ADDRESS,
      D18F5x188_NbLoadLineTrim_OFFSET, D18F5x188_NbLoadLineTrim_WIDTH
  ),
  //  CSR_GNB_3.SviLoadLineOffsetVdd = Vdd Offset

  GNB_ENTRY_COPY (
    0x4,
    0x3F9F8,
    0, 8,
    D18F5x12C_TYPE,
    D18F5x12C_ADDRESS,
    D18F5x12C_CoreOffsetTrim_OFFSET, D18F5x12C_CoreOffsetTrim_WIDTH
  ),
  //  CSR_GNB_3.SviLoadLineOffsetVddNB = VddNB Offset
  GNB_ENTRY_COPY (
    0x4,
    0x3F9F8,
    8, 8,
    D18F5x188_TYPE,
    D18F5x188_ADDRESS,
    D18F5x188_NbOffsetTrim_OFFSET, D18F5x188_NbOffsetTrim_WIDTH
  ),

  // Set SRBM time out
  GNB_ENTRY_RMW (
    0x12,
    0xE40,
    0x1FFF,
    (0x1FFF << 0)
    ),
  GNB_ENTRY_TERMINATE
};

CONST GNB_TABLE ROMDATA GnbEnvInitTableKB [] = {
  //---------------------------------------------------------------------------

  // SMU Enable Thermal Controller
  GNB_ENTRY_WR (
    SMU_MSG_TYPE,
    56,
    0
    ),
  // Enable VPC
  GNB_ENTRY_PROPERTY_RMW (
    0x00040000ul,
    0x4,
    0x3F9D8,
    0x1,
    (1 << 0)
    ),
  // Enable LHTC
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_LHTC,
    SMU_MSG_TYPE,
    70,
    0
    ),
  GNB_ENTRY_PROPERTY_WR (
    0x00040000ul,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_VPC_ACCUMULATOR,
    0
    ),

  // PM_CONFIG.f.enable_tdc_limit = 1;
  GNB_ENTRY_PROPERTY_RMW (
    0x00040000ul,
    0x4,
    0x3F9D8,
    0x4,
    (1 << 2)
    ),
  GNB_ENTRY_PROPERTY_WR (
    0x00040000ul,
    SMU_MSG_TYPE,
    46,
    0
    ),

  // Enable Package Power Limit
  GNB_ENTRY_PROPERTY_WR (
    0x00080000ul,
    SMU_MSG_TYPE,
    SMC_MSG_ENABLE_PKGPWRLIMIT,
    0
    ),

  // Enable BAPM
  // PM_CONFIG.f.enable_bapm = 1;
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_BAPM,
    0x4,
    0x3F9D8,
    0x2,
    (1 << 1)
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_BAPM,
    SMU_MSG_TYPE,
    48,
    0
    ),
//---------------------------------------------------------------------------
// ORB Init
  GNB_ENTRY_RMW (
    D0F0x98_x07_TYPE,
    D0F0x98_x07_ADDRESS,
    D0F0x98_x07_IocBwOptEn_MASK | D0F0x98_x07_DropZeroMaskWrEn_MASK,
    (0x1 << D0F0x98_x07_IocBwOptEn_OFFSET) | (0x1 << D0F0x98_x07_DropZeroMaskWrEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x07_TYPE,
    D0F0x98_x07_ADDRESS,
    D0F0x98_x07_IommuBwOptEn_MASK | D0F0x98_x07_IommuIsocPassPWMode_MASK | D0F0x98_x07_DmaReqRespPassPWMode_MASK,
    (0x1 << D0F0x98_x07_IommuBwOptEn_OFFSET) | (0x1 << D0F0x98_x07_IommuIsocPassPWMode_OFFSET) | (0 << D0F0x98_x07_DmaReqRespPassPWMode_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x08_TYPE,
    D0F0x98_x08_ADDRESS,
    D0F0x98_x08_NpWrrLenC_MASK,
    0x1 << D0F0x98_x08_NpWrrLenC_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x28_TYPE,
    D0F0x98_x28_ADDRESS,
    D0F0x98_x28_ForceCoherentIntr_MASK,
    0x1 << D0F0x98_x28_ForceCoherentIntr_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x2C_TYPE,
    D0F0x98_x2C_ADDRESS,
    D0F0x98_x2C_SBDmaActiveMask_MASK | D0F0x98_x2C_CgttLclkOverride_MASK,
    (1 << D0F0x98_x2C_SBDmaActiveMask_OFFSET) | (1 << D0F0x98_x2C_CgttLclkOverride_OFFSET)
    ),
  // Enable gBIF UID Clumping [BIT 23:21 = 011b]
  GNB_ENTRY_RMW (
    D0F0x98_x3A_TYPE,
    D0F0x98_x3A_ADDRESS,
    0x00E00000,
    0x00600000
    ),
  GNB_ENTRY_RMW (
    D18F0x110_TYPE,
    D18F0x110_ADDRESS,
    0x00E00000,
    0x00600000
    ),
  //NB P-state Configuration for Runtime
  GNB_ENTRY_RMW (
    0x4,
    0x3F9E8,
    0xFF | 0xFF00 |
    0xFF0000 | 0xFF000000,
    (3 << 0) | (0 << 8) |
    (2 << 16) | (1 << 24)
    ),
  // Programming a conservative watermark for NBP states
  GNB_ENTRY_RMW (
    0x12,
    0x6cd8,
    0x10 |
    0x100 |
    0xffff0000,
    (1 << 4) |
    (1 << 8) |
    (0x7FFF << 16)
    ),

  GNB_ENTRY_RMW (
    0x12,
    0x6cc8,
    0x3 |
    0x30000,
    (0x3 << 0) |
    (0x3 << 16)
    ),

  GNB_ENTRY_RMW (
    0x12,
    0x6cd4,
    0xffff0000,
    (0x7FFF << 16)
    ),

  GNB_ENTRY_RMW (
    0x12,
    0x6cd4,
    0x1,
    (1 << 0)
    ),

  GNB_ENTRY_RMW (
    0x12,
    0x6cd8,
    0x1,
    (1 << 0)
    ),

  GNB_ENTRY_TERMINATE
};

CONST GNB_TABLE ROMDATA GnbMidInitTableKB [] = {
//---------------------------------------------------------------------------
// ORB clock gating
  GNB_ENTRY_PROPERTY_RMW (
    0x00000008ul,
    D0F0x98_x49_TYPE,
    D0F0x98_x49_ADDRESS,
    D0F0x98_x49_SoftOverrideClk6_MASK | D0F0x98_x49_SoftOverrideClk5_MASK | D0F0x98_x49_SoftOverrideClk4_MASK | D0F0x98_x49_SoftOverrideClk3_MASK | D0F0x98_x49_SoftOverrideClk2_MASK | D0F0x98_x49_SoftOverrideClk1_MASK | D0F0x98_x49_SoftOverrideClk0_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    0x00000008ul,
    D0F0x98_x4A_TYPE,
    D0F0x98_x4A_ADDRESS,
    D0F0x98_x4A_SoftOverrideClk6_MASK | D0F0x98_x4A_SoftOverrideClk5_MASK | D0F0x98_x4A_SoftOverrideClk4_MASK | D0F0x98_x4A_SoftOverrideClk3_MASK | D0F0x98_x4A_SoftOverrideClk2_MASK | D0F0x98_x4A_SoftOverrideClk1_MASK | D0F0x98_x4A_SoftOverrideClk0_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    0x00000008ul,
    D0F0x98_x2C_TYPE,
    D0F0x98_x2C_ADDRESS,
    D0F0x98_x2C_CgttLclkOverride_MASK,
    0x0
    ),
//---------------------------------------------------------------------------
// IOC clock gating
  GNB_ENTRY_PROPERTY_RMW (
    0x00000010ul,
    D0F0x64_x22_TYPE,
    D0F0x64_x22_ADDRESS,
    0x4000000 | 0x8000000 | 0x10000000 | 0x20000000 | 0x40000000,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    0x00000010ul,
    D0F0x64_x23_TYPE,
    D0F0x64_x23_ADDRESS,
    0x4000000 | 0x8000000 | 0x10000000 | 0x20000000 | 0x40000000,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    0x00000010ul,
    D0F0x64_x46_TYPE,
    D0F0x64_x46_ADDRESS,
    0x10000,
    0x0
    ),
  //---------------------------------------------------------------------------
    GNB_ENTRY_RMW (
      0x4,
      0xc0200110,
      0x18 | 0x1 |
      0x2 | 0x200 |
      0x400,
      (0x3 << 3)   |
      (0 << 0) |
      (0x0 << 1) |
      (0x1 << 9)   |
      (0x1 << 10)
      ),
//---------------------------------------------------------------------------
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_LCLK_DEEP_SLEEP,
    0x4,
    0xc0200310,
    0x20 |
    0x40 |
    0x80 |
    0x100 |
    0x200 |
    0x400 |
    0x800 |
    0x1000 |
    0x2000 |
    0x4000 |
    0x8000 |
    0x10000 |
    0x20000 |
    0x40000 |
    0x80000 |
    0x100000 |
    0x1 |
    0x2 |
    0x4,
    ( 1 << 5) |
    ( 0 << 6) |
    ( 0 << 7) |
    ( 0 << 8) |
    ( 0 << 9) |
    ( 1 << 10) |
    ( 1 << 11) |
    ( 1 << 12) |
    ( 1 << 13) |
    ( 1 << 14) |
    ( 1 << 15) |
    ( 1 << 16) |
    ( 1 << 17) |
    ( 1 << 18) |
    ( 1 << 19) |
    ( 1 << 20) |
    ( 1 << 0) |
    ( 1 << 1) |
    ( 1 << 2)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_LCLK_DEEP_SLEEP,
    0x4,
    0xc020008c,
    0x7 |
    0x8 |
    0xfff0,
    ( 4 << 0) |
    ( 0 << 3) |
    ( 0xF << 4)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    0x4,
    0xc0200310,
    0x200000,
    0x0
    ),
// Reset : 0, Enable : 1
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_LCLK_DEEP_SLEEP,
    0x4,
    0xc020008c,
    0x80000000,
    (0x1 << 31)
    ),
//---------------------------------------------------------------------------
  GNB_ENTRY_TERMINATE
};
