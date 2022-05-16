/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe late post initialization.
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
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  "AGESA.h"
#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbCommonLib.h"
#include  "GnbTable.h"
#include  "GnbRegistersTN.h"
#include  "GnbInitTN.h"
#include  "cpuFamRegisters.h"
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
CONST GNB_TABLE ROMDATA GnbEarlierInitTableBeforeSmuTN [] = {
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

CONST GNB_TABLE ROMDATA GnbEarlierInitTableAfterSmuTN [] = {
 // Config GFX to legacy mode initially
  GNB_ENTRY_RMW (
    D0F0x64_x1D_TYPE,
    D0F0x64_x1D_ADDRESS,
    D0F0x64_x1D_IntGfxAsPcieEn_MASK,
    0
    ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D0F0xBC_x1F87C_TYPE,
    D0F0xBC_x1F87C_ADDRESS,
    D0F0xBC_x1F87C_LL_PCIE_LoadStep_MASK | D0F0xBC_x1F87C_LL_VddNbLoadStepBase_MASK,
    0
  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D0F0xBC_x1F880_TYPE,
    D0F0xBC_x1F880_ADDRESS,
    D0F0xBC_x1F880_LL_VCE_LoadStep_MASK | D0F0xBC_x1F880_LL_UVD_LoadStep_MASK,
    0
  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D0F0xBC_x1F884_TYPE,
    D0F0xBC_x1F884_ADDRESS,
    D0F0xBC_x1F884_LL_DCE2_LoadStep_MASK | D0F0xBC_x1F884_LL_DCE_LoadStep_MASK,
    0
  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D0F0xBC_x1F888_TYPE,
    D0F0xBC_x1F888_ADDRESS,
    D0F0xBC_x1F888_LL_GPU_LoadStep_MASK,
    0
  ),
 // Configure load line VID
  GNB_ENTRY_WR (
    D0F0xBC_x1F3D8_TYPE,
    D0F0xBC_x1F3D8_ADDRESS,
    (0x00 << D0F0xBC_x1F3D8_LoadLineTrim3_OFFSET) |
    (0xFE << D0F0xBC_x1F3D8_LoadLineTrim2_OFFSET) |
    (0xFC << D0F0xBC_x1F3D8_LoadLineTrim1_OFFSET) |
    (0xF6 << D0F0xBC_x1F3D8_LoadLineTrim0_OFFSET)
    ),
  GNB_ENTRY_WR (
    D0F0xBC_x1F3DC_TYPE,
    D0F0xBC_x1F3DC_ADDRESS,
    (0x08 << D0F0xBC_x1F3DC_LoadLineTrim7_OFFSET) |
    (0x06 << D0F0xBC_x1F3DC_LoadLineTrim6_OFFSET) |
    (0x04 << D0F0xBC_x1F3DC_LoadLineTrim5_OFFSET) |
    (0x02 << D0F0xBC_x1F3DC_LoadLineTrim4_OFFSET)
    ),
  GNB_ENTRY_WR (
    D0F0xBC_x1F404_TYPE,
    D0F0xBC_x1F404_ADDRESS,
    (0x19 << D0F0xBC_x1F404_LoadLineOffset3_OFFSET) |
    (0x00 << D0F0xBC_x1F404_LoadLineOffset2_OFFSET) |
    (0xE7 << D0F0xBC_x1F404_LoadLineOffset1_OFFSET) |
    (0x00 << D0F0xBC_x1F404_LoadLineOffset0_OFFSET)
    ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F3F8_TYPE,
    D0F0xBC_x1F3F8_ADDRESS,
    D0F0xBC_x1F3F8_SviInitLoadLineVdd_OFFSET, D0F0xBC_x1F3F8_SviInitLoadLineVdd_WIDTH,
    D0F0xBC_xE01040A8_TYPE,
    D0F0xBC_xE01040A8_ADDRESS,
    D0F0xBC_xE01040A8_SviLoadLineVdd_OFFSET, D0F0xBC_xE01040A8_SviLoadLineVdd_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F3F8_TYPE,
    D0F0xBC_x1F3F8_ADDRESS,
    D0F0xBC_x1F3F8_SviInitLoadLineVddNB_OFFSET, D0F0xBC_x1F3F8_SviInitLoadLineVddNB_WIDTH,
    D0F0xBC_xE01040A8_TYPE,
    D0F0xBC_xE01040A8_ADDRESS,
    D0F0xBC_xE01040A8_SviLoadLineVddNb_OFFSET, D0F0xBC_xE01040A8_SviLoadLineVddNb_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F3F8_TYPE,
    D0F0xBC_x1F3F8_ADDRESS,
    D0F0xBC_x1F3F8_SviTrimValueVdd_OFFSET, D0F0xBC_x1F3F8_SviTrimValueVdd_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineTrimVdd_OFFSET, D0F0xBC_xE0104184_SviLoadLineTrimVdd_WIDTH
  ),
 GNB_ENTRY_COPY (
    D18F5x12C_TYPE,
    D18F5x12C_ADDRESS,
    D18F5x12C_CoreLoadLineTrim_OFFSET, D18F5x12C_CoreLoadLineTrim_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineTrimVdd_OFFSET, D0F0xBC_xE0104184_SviLoadLineTrimVdd_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F3F8_TYPE,
    D0F0xBC_x1F3F8_ADDRESS,
    D0F0xBC_x1F3F8_SviTrimValueVddNB_OFFSET, D0F0xBC_x1F3F8_SviTrimValueVddNB_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineTrimVddNb_OFFSET, D0F0xBC_xE0104184_SviLoadLineTrimVddNb_WIDTH
  ),
  GNB_ENTRY_COPY (
    D18F5x188_TYPE,
    D18F5x188_ADDRESS,
    D18F5x188_NbLoadLineTrim_OFFSET, D18F5x188_NbLoadLineTrim_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineTrimVddNb_OFFSET, D0F0xBC_xE0104184_SviLoadLineTrimVddNb_WIDTH
  ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F3FC_TYPE,
    D0F0xBC_x1F3FC_ADDRESS,
    D0F0xBC_x1F3FC_SviVidStepBase_MASK | D0F0xBC_x1F3FC_SviVidStep_MASK,
    (0x1838 << D0F0xBC_x1F3FC_SviVidStepBase_OFFSET) | (0x19 << D0F0xBC_x1F3FC_SviVidStep_OFFSET)
    ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F400_TYPE,
    D0F0xBC_x1F400_ADDRESS,
    D0F0xBC_x1F400_SviLoadLineOffsetVdd_OFFSET, D0F0xBC_x1F400_SviLoadLineOffsetVdd_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineOffsetVdd_OFFSET, D0F0xBC_xE0104184_SviLoadLineOffsetVdd_WIDTH
  ),
  GNB_ENTRY_COPY (
    D18F5x12C_TYPE,
    D18F5x12C_ADDRESS,
    D18F5x12C_CoreOffsetTrim_OFFSET, D18F5x12C_CoreOffsetTrim_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineOffsetVdd_OFFSET, D0F0xBC_xE0104184_SviLoadLineOffsetVdd_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F400_TYPE,
    D0F0xBC_x1F400_ADDRESS,
    D0F0xBC_x1F400_SviLoadLineOffsetVddNB_OFFSET, D0F0xBC_x1F400_SviLoadLineOffsetVddNB_WIDTH,
    D0F0xBC_xE0104184_TYPE,
    D0F0xBC_xE0104184_ADDRESS,
    D0F0xBC_xE0104184_SviLoadLineOffsetVddNb_OFFSET, D0F0xBC_xE0104184_SviLoadLineOffsetVddNb_WIDTH
  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D0F0xBC_x1F400_TYPE,
    D0F0xBC_x1F400_ADDRESS,
    D0F0xBC_x1F400_SviLoadLineOffsetVddNB_MASK | D0F0xBC_x1F400_SviLoadLineOffsetVdd_MASK,
    (2 << D0F0xBC_x1F400_SviLoadLineOffsetVddNB_OFFSET) | (2 << D0F0xBC_x1F400_SviLoadLineOffsetVdd_OFFSET)
  ),
//  GNB_ENTRY_COPY (
//    D18F5x188_TYPE,
//    D18F5x188_ADDRESS,
//    D18F5x188_NbOffsetTrim_OFFSET, D18F5x188_NbOffsetTrim_WIDTH,
//    D0F0xBC_xE0104184_TYPE,
//    D0F0xBC_xE0104184_ADDRESS,
//    D0F0xBC_xE0104184_SviLoadLineOffsetVddNb_OFFSET, D0F0xBC_xE0104184_SviLoadLineOffsetVddNb_WIDTH
//  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D18F5x188_TYPE,
    D18F5x188_ADDRESS,
    D18F5x188_NbLoadLineTrim_MASK,// | D18F5x188_NbOffsetTrim_MASK,
    (3 << D18F5x188_NbLoadLineTrim_OFFSET)// | (2 << D18F5x188_NbOffsetTrim_OFFSET)
  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D18F5x12C_TYPE,
    D18F5x12C_ADDRESS,
    D18F5x12C_CoreLoadLineTrim_MASK | D18F5x12C_CoreOffsetTrim_MASK,
    (3 << D18F5x12C_CoreLoadLineTrim_OFFSET) | (2 << D18F5x12C_CoreOffsetTrim_OFFSET)
  ),
  GNB_ENTRY_REV_RMW (
    0x0000000000000100ull ,
    D0F0xBC_x1F3F8_TYPE,
    D0F0xBC_x1F3F8_ADDRESS,
    D0F0xBC_x1F3F8_SviTrimValueVdd_MASK | D0F0xBC_x1F3F8_SviTrimValueVddNB_MASK,
    (3 << D0F0xBC_x1F3F8_SviTrimValueVdd_OFFSET) | (3 << D0F0xBC_x1F3F8_SviTrimValueVddNB_OFFSET)
  ),
  // Enable SVI2
  GNB_ENTRY_RMW (
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_SviMode_MASK,
    (1 << D0F0xBC_x1F428_SviMode_OFFSET)
    ),
  GNB_ENTRY_TERMINATE
};

CONST GNB_TABLE ROMDATA GnbEarlyInitTableTN [] = {
  GNB_ENTRY_WR (
    D0F0x04_TYPE,
    D0F0x04_ADDRESS,
    (0x1 << D0F0x04_MemAccessEn_OFFSET) | (0x1 << D0F0x04_BusMasterEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0x4C_TYPE,
    D0F0x4C_ADDRESS,
    D0F0x4C_CfgRdTime_MASK,
    0x2 << D0F0x4C_CfgRdTime_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0x84_TYPE,
    D0F0x84_ADDRESS,
    D0F0x84_Ev6Mode_MASK,
    0x1 << D0F0x84_Ev6Mode_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0x64_x46_TYPE,
    D0F0x64_x46_ADDRESS,
    0x6 ,
    1 << D0F0x64_x46_Msi64bitEn_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0x98_x0C_TYPE,
    D0F0x98_x0C_ADDRESS,
    D0F0x98_x0C_StrictSelWinnerEn_MASK,
    1 << D0F0x98_x0C_StrictSelWinnerEn_OFFSET
    ),
  // Configure PM timer
  GNB_ENTRY_RMW (
    D0F0xBC_x1F468_TYPE,
    D0F0xBC_x1F468_ADDRESS,
    D0F0xBC_x1F468_TimerPeriod_MASK,
    D0F0xBC_x1F468_TimerPeriod_Value << D0F0xBC_x1F468_TimerPeriod_OFFSET
    ),
  GNB_ENTRY_WR (
    SMU_MSG_TYPE,
    SMC_MSG_EN_PM_CNTL,
    0
    ),
  //Enable voltage controller
  GNB_ENTRY_RMW (
    D0F0xBC_x1F460_TYPE,
    D0F0xBC_x1F460_ADDRESS,
    D0F0xBC_x1F460_VoltageCntl_MASK,
    1 << D0F0xBC_x1F460_VoltageCntl_OFFSET
    ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F384_TYPE,
    D0F0xBC_x1F384_ADDRESS,
    D0F0xBC_x1F384_FirmwareVid_OFFSET,
    D0F0xBC_x1F384_FirmwareVid_WIDTH,
    D0F0xBC_xE0001008_TYPE ,
    D0F0xBC_xE0001008_ADDRESS,
    D0F0xBC_xE0001008_SClkVid0_OFFSET,
    D0F0xBC_xE0001008_SClkVid0_WIDTH
    ),
  GNB_ENTRY_WR (
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_VOLTAGE_CNTL,
    0
    ),
  GNB_ENTRY_POLL (
    GMMx7B0_TYPE,
    GMMx7B0_ADDRESS,
    GMMx7B0_SMU_VOLTAGE_EN_MASK,
    0x1 << GMMx7B0_SMU_VOLTAGE_EN_OFFSET
    ),
  // Enable thermal controller
  GNB_ENTRY_RMW (
    D0F0xBC_x1F460_TYPE,
    D0F0xBC_x1F460_ADDRESS,
    D0F0xBC_x1F460_ThermalCntl_MASK,
    30 << D0F0xBC_x1F460_ThermalCntl_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F388_TYPE,
    D0F0xBC_x1F388_ADDRESS,
    D0F0xBC_x1F388_CsrAddr_MASK | D0F0xBC_x1F388_TcenId_MASK,
    (0x9 << D0F0xBC_x1F388_CsrAddr_OFFSET) | (0xE << D0F0xBC_x1F388_TcenId_OFFSET)
    ),
  GNB_ENTRY_WR (
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_THERMAL_CNTL,
    0
    ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F400_TYPE,
    D0F0xBC_x1F400_ADDRESS,
    D0F0xBC_x1F400_PstateMax_OFFSET,
    D0F0xBC_x1F400_PstateMax_WIDTH,
    TYPE_D18F3 ,
    0xdc ,
    8 ,
    3
    ),
  // Configure VPC
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_BAPM,
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_EnableVpcAccumulators_MASK,
    (1 << D0F0xBC_x1F428_EnableVpcAccumulators_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_PstateAllCpusIdle_MASK | D0F0xBC_x1F428_NbPstateAllCpusIdle_MASK,
    (1 << D0F0xBC_x1F428_NbPstateAllCpusIdle_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F46C_TYPE,
    D0F0xBC_x1F46C_ADDRESS,
    D0F0xBC_x1F46C_VpcPeriod_MASK,
    (0x1B58 << D0F0xBC_x1F46C_VpcPeriod_OFFSET)
    ),

  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_BAPM,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_VPC_ACCUMULATOR,
    0
    ),
 // Enable TDC
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_BAPM,
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_EnableTdcLimit_MASK,
    (1 << D0F0xBC_x1F428_EnableTdcLimit_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F638_TYPE,
    D0F0xBC_x1F638_ADDRESS,
    D0F0xBC_x1F638_TdcPeriod_MASK,
    (0x1 << D0F0xBC_x1F638_TdcPeriod_OFFSET)
    ),

  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_BAPM,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_TDC_LIMIT,
    0
    ),

  // Enable LPMx
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_BAPM,
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_EnableLpmx_MASK,
    (1 << D0F0xBC_x1F428_EnableLpmx_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F46C_TYPE,
    D0F0xBC_x1F46C_ADDRESS,
    D0F0xBC_x1F46C_LpmxPeriod_MASK,
    (1 << D0F0xBC_x1F46C_LpmxPeriod_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_BAPM,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_LPMx,
    0
    ),
  // Enable BAPM
  GNB_ENTRY_RMW (
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_BapmCoeffOverride_MASK,
    (0x1 << D0F0xBC_x1F428_BapmCoeffOverride_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_BAPM,
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_EnableBapm_MASK,
    (1 << D0F0xBC_x1F428_EnableBapm_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F46C_TYPE,
    D0F0xBC_x1F46C_ADDRESS,
    D0F0xBC_x1F46C_BapmPeriod_MASK,
    (D0F0xBC_x1F46C_BapmPeriod_Value << D0F0xBC_x1F46C_BapmPeriod_OFFSET)
    ),
 // Config BAPM
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_BAPM,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_BAPM,
    0
    ),
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GnbEnvInitTableTN [] = {
//---------------------------------------------------------------------------
// ORB Init
//D0F0x98_x07[IocBwOptEn]
//D0F0x98_x07[DropZeroMaskWrEn]
//D0F0x98_x28[ForceCoherentIntr] = 1
//D0F0x98_x07[UnadjustThrottlingStpclk ] = 1
//D0F0x98_x07[MSIHTIntConversionEn] = 0
//D0F0x98_x07[IommuBwOptEn] = 1
//D0F0x98_x07[IommuIsocPassPWMode] = 1
//D0F0x98_x08[NpWrrLenC] = 1
//D0F0x98_x28[ForceCoherentIntr] = 1
//D0F0x98_x2C[NBOutbWakeMask] = 1
//D0F0x98_x2C[OrbRxIdlesMask] = 1

  GNB_ENTRY_RMW (
    D0F0x98_x07_TYPE,
    D0F0x98_x07_ADDRESS,
    D0F0x98_x07_UnadjustThrottlingStpclk_MASK | D0F0x98_x07_MSIHTIntConversionEn_MASK |
    D0F0x98_x07_IommuBwOptEn_MASK | D0F0x98_x07_IommuIsocPassPWMode_MASK |
    D0F0x98_x07_IocBwOptEn_MASK | D0F0x98_x07_DropZeroMaskWrEn_MASK,
    (0x1 << D0F0x98_x07_UnadjustThrottlingStpclk_OFFSET) | (0x0 << D0F0x98_x07_MSIHTIntConversionEn_OFFSET) |
    (0x1 << D0F0x98_x07_IommuBwOptEn_OFFSET) | (0x1 << D0F0x98_x07_IommuIsocPassPWMode_OFFSET) |
    (0x1 << D0F0x98_x07_IocBwOptEn_OFFSET) | (0x1 << D0F0x98_x07_DropZeroMaskWrEn_OFFSET)
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
    D0F0x98_x2C_NBOutbWakeMask_MASK | D0F0x98_x2C_OrbRxIdlesMask_MASK,
    (0x1 << D0F0x98_x2C_NBOutbWakeMask_OFFSET) | (0x1 << D0F0x98_x2C_OrbRxIdlesMask_OFFSET)
    ),
//---------------------------------------------------------------------------
//IOMMU L2 Initialization
  GNB_ENTRY_RMW (
    D0F2xF4_x10_TYPE,
    D0F2xF4_x10_ADDRESS,
    D0F2xF4_x10_DTCInvalidationSel_MASK,
    0x2 << D0F2xF4_x10_DTCInvalidationSel_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x11_TYPE,
    D0F2xF4_x11_ADDRESS,
    D0F2xF4_x11_DtcAddressMask_MASK | D0F2xF4_x11_DtcAltHashEn_MASK,
    (0x0 << D0F2xF4_x11_DtcAddressMask_OFFSET) | (0x1 << D0F2xF4_x11_DtcAltHashEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x14_TYPE,
    D0F2xF4_x14_ADDRESS,
    D0F2xF4_x14_ITCInvalidationSel_MASK,
    0x2 << D0F2xF4_x14_ITCInvalidationSel_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x15_TYPE,
    D0F2xF4_x15_ADDRESS,
    D0F2xF4_x15_ITCAddressMask_MASK | D0F2xF4_x15_ItcAltHashEn_MASK,
    (0x0 << D0F2xF4_x15_ITCAddressMask_OFFSET) | (1 << D0F2xF4_x15_ItcAltHashEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x18_TYPE,
    D0F2xF4_x18_ADDRESS,
    D0F2xF4_x18_PTCAInvalidationSel_MASK,
    0x2 << D0F2xF4_x18_PTCAInvalidationSel_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x19_TYPE,
    D0F2xF4_x19_ADDRESS,
    D0F2xF4_x19_PTCAAddressMask_MASK | D0F2xF4_x19_PtcAltHashEn_MASK,
    (0x0 << D0F2xF4_x19_PTCAAddressMask_OFFSET) | (1 << D0F2xF4_x19_PtcAltHashEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x30_TYPE,
    D0F2xF4_x30_ADDRESS,
    D0F2xF4_x30_ERRRuleLock1_MASK,
    0x1 << D0F2xF4_x30_ERRRuleLock1_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x34_TYPE,
    D0F2xF4_x34_ADDRESS,
    D0F2xF4_x34_L2aregHostPgsize_MASK | D0F2xF4_x34_L2aregGstPgsize_MASK,
    (0x2 << D0F2xF4_x34_L2aregHostPgsize_OFFSET) | (0x2 <<  D0F2xF4_x34_L2aregGstPgsize_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x47_TYPE,
    D0F2xF4_x47_ADDRESS,
    D0F2xF4_x47_TwAtomicFilterEn_MASK | D0F2xF4_x47_TwNwEn_MASK,
    (0x1 << D0F2xF4_x47_TwAtomicFilterEn_OFFSET) | (1 << D0F2xF4_x47_TwNwEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x4C_TYPE,
    D0F2xF4_x4C_ADDRESS,
    D0F2xF4_x4C_GstPartialPtcCntrl_MASK,
    0x3 << D0F2xF4_x4C_GstPartialPtcCntrl_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x50_TYPE,
    D0F2xF4_x50_ADDRESS,
    D0F2xF4_x50_PDCInvalidationSel_MASK,
    0x2 << D0F2xF4_x50_PDCInvalidationSel_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x51_TYPE,
    D0F2xF4_x51_ADDRESS,
    D0F2xF4_x51_PDCAddressMask_MASK | D0F2xF4_x51_PdcAltHashEn_MASK,
    (0x0 << D0F2xF4_x51_PDCAddressMask_OFFSET) | (1 << D0F2xF4_x51_PdcAltHashEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x56_TYPE,
    D0F2xF4_x56_ADDRESS,
    D0F2xF4_x56_CPFlushOnInv_MASK | D0F2xF4_x56_CPFlushOnWait_MASK,
    (0x0 << D0F2xF4_x56_CPFlushOnInv_OFFSET) | (1 << D0F2xF4_x56_CPFlushOnWait_OFFSET)
    ),

  GNB_ENTRY_RMW (
    D0F2xF4_x80_TYPE,
    D0F2xF4_x80_ADDRESS,
    D0F2xF4_x80_ERRRuleLock0_MASK,
    0x1 << D0F2xF4_x80_ERRRuleLock0_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x90_TYPE,
    D0F2xF4_x90_ADDRESS,
    D0F2xF4_x90_CKGateL2BMiscDisable_MASK | D0F2xF4_x90_CKGateL2BDynamicDisable_MASK | D0F2xF4_x90_CKGateL2BRegsDisable_MASK | D0F2xF4_x90_CKGateL2BCacheDisable_MASK,
    (0x1 << D0F2xF4_x90_CKGateL2BMiscDisable_OFFSET) | (0x1 << D0F2xF4_x90_CKGateL2BDynamicDisable_OFFSET) | (0x1 << D0F2xF4_x90_CKGateL2BRegsDisable_OFFSET) | (0x1 << D0F2xF4_x90_CKGateL2BCacheDisable_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x92_TYPE,
    D0F2xF4_x92_ADDRESS,
    D0F2xF4_x92_PprIntcoallesceEn_MASK | D0F2xF4_x92_PprIntreqdelay_MASK | D0F2xF4_x92_PprInttimedelay_MASK,
    (0x0 << D0F2xF4_x92_PprIntcoallesceEn_OFFSET) | (0x20 << D0F2xF4_x92_PprIntreqdelay_OFFSET) | (0x15 << D0F2xF4_x92_PprInttimedelay_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xF4_x94_TYPE,
    D0F2xF4_x94_ADDRESS,
    D0F2xF4_x94_L2bregHostPgsize_MASK | D0F2xF4_x94_L2bregGstPgsize_MASK,
    (0x2 << D0F2xF4_x94_L2bregHostPgsize_OFFSET) | (0x2ull << D0F2xF4_x94_L2bregGstPgsize_OFFSET)
    ),
//IOMMU L1 Initialization
  GNB_ENTRY_RMW (
    D0F2xFC_x0C_L1_TYPE,
    D0F2xFC_x0C_L1_ADDRESS (L1_SEL_GFX),
    D0F2xFC_x0C_L1_L1VirtOrderQueues_MASK,
    0x4 << D0F2xFC_x0C_L1_L1VirtOrderQueues_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x32_L1_TYPE,
    D0F2xFC_x32_L1_ADDRESS (L1_SEL_GFX),
    D0F2xFC_x32_L1_AtsMultipleL1toL2En_MASK | D0F2xFC_x32_L1_AtsMultipleRespEn_MASK | D0F2xFC_x32_L1_TimeoutPulseExtEn_MASK,
    (0x1 << D0F2xFC_x32_L1_AtsMultipleL1toL2En_OFFSET) | (0x1 << D0F2xFC_x32_L1_AtsMultipleRespEn_OFFSET) | (0x1 << D0F2xFC_x32_L1_TimeoutPulseExtEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x07_L1_TYPE,
    D0F2xFC_x07_L1_ADDRESS (L1_SEL_GFX),
    D0F2xFC_x07_L1_L1NwEn_MASK | D0F2xFC_x07_L1_AtsPhysPageOverlapDis_MASK |
    D0F2xFC_x07_L1_AtsSeqNumEn_MASK | D0F2xFC_x07_L1_SpecReqFilterEn_MASK,
    (0x1 << D0F2xFC_x07_L1_L1NwEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_AtsPhysPageOverlapDis_OFFSET) |
    (0x1 << D0F2xFC_x07_L1_AtsSeqNumEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_SpecReqFilterEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x0C_L1_TYPE,
    D0F2xFC_x0C_L1_ADDRESS (L1_SEL_GPPSB),
    D0F2xFC_x0C_L1_L1VirtOrderQueues_MASK,
    0x4 << D0F2xFC_x0C_L1_L1VirtOrderQueues_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x32_L1_TYPE,
    D0F2xFC_x32_L1_ADDRESS (L1_SEL_GPPSB),
    D0F2xFC_x32_L1_AtsMultipleL1toL2En_MASK | D0F2xFC_x32_L1_AtsMultipleRespEn_MASK | D0F2xFC_x32_L1_TimeoutPulseExtEn_MASK,
    (0x1 << D0F2xFC_x32_L1_AtsMultipleL1toL2En_OFFSET) | (0x1 << D0F2xFC_x32_L1_AtsMultipleRespEn_OFFSET) | (0x1 << D0F2xFC_x32_L1_TimeoutPulseExtEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x07_L1_TYPE,
    D0F2xFC_x07_L1_ADDRESS (L1_SEL_GPPSB),
    D0F2xFC_x07_L1_L1NwEn_MASK | D0F2xFC_x07_L1_AtsPhysPageOverlapDis_MASK |
    D0F2xFC_x07_L1_AtsSeqNumEn_MASK | D0F2xFC_x07_L1_SpecReqFilterEn_MASK,
    (0x1 << D0F2xFC_x07_L1_L1NwEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_AtsPhysPageOverlapDis_OFFSET) |
    (0x1 << D0F2xFC_x07_L1_AtsSeqNumEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_SpecReqFilterEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x0C_L1_TYPE,
    D0F2xFC_x0C_L1_ADDRESS (L1_SEL_GBIF),
    D0F2xFC_x0C_L1_L1VirtOrderQueues_MASK,
    0x4 << D0F2xFC_x0C_L1_L1VirtOrderQueues_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x32_L1_TYPE,
    D0F2xFC_x32_L1_ADDRESS (L1_SEL_GBIF),
    D0F2xFC_x32_L1_AtsMultipleL1toL2En_MASK | D0F2xFC_x32_L1_AtsMultipleRespEn_MASK | D0F2xFC_x32_L1_TimeoutPulseExtEn_MASK,
    (0x1 << D0F2xFC_x32_L1_AtsMultipleL1toL2En_OFFSET) | (0x1 << D0F2xFC_x32_L1_AtsMultipleRespEn_OFFSET) | (0x1 << D0F2xFC_x32_L1_TimeoutPulseExtEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x07_L1_TYPE,
    D0F2xFC_x07_L1_ADDRESS (L1_SEL_GBIF),
    D0F2xFC_x07_L1_L1NwEn_MASK | D0F2xFC_x07_L1_AtsPhysPageOverlapDis_MASK |
    D0F2xFC_x07_L1_AtsSeqNumEn_MASK | D0F2xFC_x07_L1_SpecReqFilterEn_MASK,
    (0x1 << D0F2xFC_x07_L1_L1NwEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_AtsPhysPageOverlapDis_OFFSET) |
    (0x1 << D0F2xFC_x07_L1_AtsSeqNumEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_SpecReqFilterEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x0C_L1_TYPE,
    D0F2xFC_x0C_L1_ADDRESS (L1_SEL_INTGEN),
    D0F2xFC_x0C_L1_L1VirtOrderQueues_MASK,
    0x4 << D0F2xFC_x0C_L1_L1VirtOrderQueues_OFFSET
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x32_L1_TYPE,
    D0F2xFC_x32_L1_ADDRESS (L1_SEL_INTGEN),
    D0F2xFC_x32_L1_AtsMultipleL1toL2En_MASK | D0F2xFC_x32_L1_AtsMultipleRespEn_MASK | D0F2xFC_x32_L1_TimeoutPulseExtEn_MASK,
    (0x1 << D0F2xFC_x32_L1_AtsMultipleL1toL2En_OFFSET) | (0x1 << D0F2xFC_x32_L1_AtsMultipleRespEn_OFFSET) | (0x1 << D0F2xFC_x32_L1_TimeoutPulseExtEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F2xFC_x07_L1_TYPE,
    D0F2xFC_x07_L1_ADDRESS (L1_SEL_INTGEN),
    D0F2xFC_x07_L1_L1NwEn_MASK | D0F2xFC_x07_L1_AtsPhysPageOverlapDis_MASK |
    D0F2xFC_x07_L1_AtsSeqNumEn_MASK | D0F2xFC_x07_L1_SpecReqFilterEn_MASK,
    (0x1 << D0F2xFC_x07_L1_L1NwEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_AtsPhysPageOverlapDis_OFFSET) |
    (0x1 << D0F2xFC_x07_L1_AtsSeqNumEn_OFFSET) | (0x1 << D0F2xFC_x07_L1_SpecReqFilterEn_OFFSET)
    ),
//---------------------------------------------------------------------------
// IOMMU Initialization
  GNB_ENTRY_RMW (
    D0F2x70_TYPE,
    D0F2x70_ADDRESS,
    D0F2x70_PcSupW_MASK,
    (0x0 << D0F2x70_PcSupW_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0x64_x0D_TYPE,
    D0F0x64_x0D_ADDRESS,
    D0F0x64_x0D_PciDev0Fn2RegEn_MASK,
    (0x1 << D0F0x64_x0D_PciDev0Fn2RegEn_OFFSET)
    ),
// IOMMU L2 clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    D0F2xF4_x33_TYPE,
    D0F2xF4_x33_ADDRESS,
    D0F2xF4_x33_CKGateL2ARegsDisable_MASK | D0F2xF4_x33_CKGateL2ADynamicDisable_MASK | D0F2xF4_x33_CKGateL2ACacheDisable_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    D0F2xF4_x90_TYPE,
    D0F2xF4_x90_ADDRESS,
    D0F2xF4_x90_CKGateL2BRegsDisable_MASK | D0F2xF4_x90_CKGateL2BDynamicDisable_MASK | D0F2xF4_x90_CKGateL2BMiscDisable_MASK | D0F2xF4_x90_CKGateL2BCacheDisable_MASK,
    0x0
    ),
// IOMMU L1 clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    D0F2xFC_x33_L1_TYPE,
    D0F2xFC_x33_L1_ADDRESS (L1_SEL_GFX),
    D0F2xFC_x33_L1_L1DmaClkgateEn_MASK | D0F2xFC_x33_L1_L1CacheClkgateEn_MASK |
    D0F2xFC_x33_L1_L1CpslvClkgateEn_MASK | D0F2xFC_x33_L1_L1DmaInputClkgateEn_MASK |
    D0F2xFC_x33_L1_L1PerfClkgateEn_MASK | D0F2xFC_x33_L1_L1MemoryClkgateEn_MASK |
    D0F2xFC_x33_L1_L1RegClkgateEn_MASK | D0F2xFC_x33_L1_L1L2ClkgateEn_MASK,
    (0x1 << D0F2xFC_x33_L1_L1DmaClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1CacheClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1CpslvClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1DmaInputClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1PerfClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1MemoryClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1RegClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1L2ClkgateEn_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    D0F2xFC_x33_L1_TYPE,
    D0F2xFC_x33_L1_ADDRESS (L1_SEL_GPPSB),
    D0F2xFC_x33_L1_L1DmaClkgateEn_MASK | D0F2xFC_x33_L1_L1CacheClkgateEn_MASK |
    D0F2xFC_x33_L1_L1CpslvClkgateEn_MASK | D0F2xFC_x33_L1_L1DmaInputClkgateEn_MASK |
    D0F2xFC_x33_L1_L1PerfClkgateEn_MASK | D0F2xFC_x33_L1_L1MemoryClkgateEn_MASK |
    D0F2xFC_x33_L1_L1RegClkgateEn_MASK | D0F2xFC_x33_L1_L1L2ClkgateEn_MASK,
    (0x1 << D0F2xFC_x33_L1_L1DmaClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1CacheClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1CpslvClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1DmaInputClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1PerfClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1MemoryClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1RegClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1L2ClkgateEn_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    D0F2xFC_x33_L1_TYPE,
    D0F2xFC_x33_L1_ADDRESS (L1_SEL_GBIF),
    D0F2xFC_x33_L1_L1DmaClkgateEn_MASK | D0F2xFC_x33_L1_L1CacheClkgateEn_MASK |
    D0F2xFC_x33_L1_L1CpslvClkgateEn_MASK | D0F2xFC_x33_L1_L1DmaInputClkgateEn_MASK |
    D0F2xFC_x33_L1_L1PerfClkgateEn_MASK | D0F2xFC_x33_L1_L1MemoryClkgateEn_MASK |
    D0F2xFC_x33_L1_L1RegClkgateEn_MASK | D0F2xFC_x33_L1_L1L2ClkgateEn_MASK,
    (0x1 << D0F2xFC_x33_L1_L1DmaClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1CacheClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1CpslvClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1DmaInputClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1PerfClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1MemoryClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1RegClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1L2ClkgateEn_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    D0F2xFC_x33_L1_TYPE,
    D0F2xFC_x33_L1_ADDRESS (L1_SEL_INTGEN),
    D0F2xFC_x33_L1_L1DmaClkgateEn_MASK | D0F2xFC_x33_L1_L1CacheClkgateEn_MASK |
    D0F2xFC_x33_L1_L1CpslvClkgateEn_MASK | D0F2xFC_x33_L1_L1DmaInputClkgateEn_MASK |
    D0F2xFC_x33_L1_L1PerfClkgateEn_MASK | D0F2xFC_x33_L1_L1MemoryClkgateEn_MASK |
    D0F2xFC_x33_L1_L1RegClkgateEn_MASK | D0F2xFC_x33_L1_L1L2ClkgateEn_MASK,
    (0x1 << D0F2xFC_x33_L1_L1DmaClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1CacheClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1CpslvClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1DmaInputClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1PerfClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1MemoryClkgateEn_OFFSET) |
    (0x1 << D0F2xFC_x33_L1_L1RegClkgateEn_OFFSET) | (0x1 << D0F2xFC_x33_L1_L1L2ClkgateEn_OFFSET)
    ),
//---------------------------------------------------------------------------
// Configure IOMMU Power Island
  GNB_ENTRY_WR (
    D0F0xBC_xE030001C_TYPE,
    D0F0xBC_xE030001C_ADDRESS,
    (10 << 0 ) | (50 << 8 ) |
    (5 << 16 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300018_TYPE,
    D0F0xBC_xE0300018_ADDRESS,
    (0xff << D0F0xBC_xE0300018_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300018_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE0300018_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE030001C_TYPE,
    D0F0xBC_xE030001C_ADDRESS,
    (50 << 0 ) | (50 << 12 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300018_TYPE,
    D0F0xBC_xE0300018_ADDRESS,
    (0xff << D0F0xBC_xE0300018_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300018_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE0300018_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE030001C_TYPE,
    D0F0xBC_xE030001C_ADDRESS,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300018_TYPE,
    D0F0xBC_xE0300018_ADDRESS,
    (0xff << D0F0xBC_xE0300018_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300018_WriteOp_OFFSET) |
    (1  << D0F0xBC_xE0300018_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_RMW (
    D0F0xBC_xE0300320_TYPE,
    D0F0xBC_xE0300320_ADDRESS,
    D0F0xBC_xE0300320_IommuPgfsmClockEn_MASK,
    0x0
    ),
// Hide IOMMU function if disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    D0F0x64_x0D_TYPE,
    D0F0x64_x0D_ADDRESS,
    D0F0x64_x0D_PciDev0Fn2RegEn_MASK,
    0x0
    ),
  //NB P-state Configuration for Runtime
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_NBDPM,
    D0F0xBC_x1F428_TYPE,
    D0F0xBC_x1F428_ADDRESS,
    D0F0xBC_x1F428_EnableNbDpm_MASK,
    (1 << D0F0xBC_x1F428_EnableNbDpm_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F638_TYPE,
    D0F0xBC_x1F638_ADDRESS,
    D0F0xBC_x1F638_NbdpmPeriod_MASK | D0F0xBC_x1F638_PginterlockPeriod_MASK,
    (1 << D0F0xBC_x1F638_NbdpmPeriod_OFFSET) | (1 << D0F0xBC_x1F638_PginterlockPeriod_OFFSET)
    ),

  GNB_ENTRY_COPY (
    D0F0xBC_x1F5F8_TYPE,
    D0F0xBC_x1F5F8_ADDRESS,
    D0F0xBC_x1F5F8_Dpm0PgNbPsLo_OFFSET, D0F0xBC_x1F5F8_Dpm0PgNbPsLo_WIDTH,
    D0F0xBC_xE010703C_TYPE,
    D0F0xBC_xE010703C_ADDRESS,
    D0F0xBC_xE010703C_NbPstateLo_OFFSET, D0F0xBC_xE010703C_NbPstateLo_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F5F8_TYPE,
    D0F0xBC_x1F5F8_ADDRESS,
    D0F0xBC_x1F5F8_Dpm0PgNbPsHi_OFFSET, D0F0xBC_x1F5F8_Dpm0PgNbPsHi_WIDTH,
    D0F0xBC_xE010703C_TYPE,
    D0F0xBC_xE010703C_ADDRESS,
    D0F0xBC_xE010703C_NbPstateHi_OFFSET, D0F0xBC_xE010703C_NbPstateHi_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F5F8_TYPE,
    D0F0xBC_x1F5F8_ADDRESS,
    D0F0xBC_x1F5F8_DpmXNbPsLo_OFFSET, D0F0xBC_x1F5F8_DpmXNbPsLo_WIDTH,
    D0F0xBC_xE010703C_TYPE,
    D0F0xBC_xE010703C_ADDRESS,
    D0F0xBC_xE010703C_NbPstateLo_OFFSET, D0F0xBC_xE010703C_NbPstateLo_WIDTH
  ),
  GNB_ENTRY_COPY (
    D0F0xBC_x1F5F8_TYPE,
    D0F0xBC_x1F5F8_ADDRESS,
    D0F0xBC_x1F5F8_DpmXNbPsHi_OFFSET, D0F0xBC_x1F5F8_DpmXNbPsHi_WIDTH,
    D0F0xBC_xE010703C_TYPE,
    D0F0xBC_xE010703C_ADDRESS,
    D0F0xBC_xE010703C_NbPstateHi_OFFSET, D0F0xBC_xE010703C_NbPstateHi_WIDTH
  ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F5F8_TYPE,
    D0F0xBC_x1F5F8_ADDRESS,
    D0F0xBC_x1F5F8_Hysteresis_MASK | D0F0xBC_x1F5F8_SkipDPM0_MASK |
    D0F0xBC_x1F5F8_SkipPG_MASK | D0F0xBC_x1F5F8_EnableNbPsi1_MASK | D0F0xBC_x1F5F8_EnableDpmPstatePoll_MASK,
    (10 << D0F0xBC_x1F5F8_Hysteresis_OFFSET) | (1 << D0F0xBC_x1F5F8_SkipDPM0_OFFSET) |
    (1 << D0F0xBC_x1F5F8_EnableNbPsi1_OFFSET) | (1 << D0F0xBC_x1F5F8_EnableDpmPstatePoll_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F6E4_TYPE,
    D0F0xBC_x1F6E4_ADDRESS,
    D0F0xBC_x1F6E4_DdrVoltFloor_MASK | D0F0xBC_x1F6E4_BapmDdrVoltFloor_MASK,
    (0xFF << D0F0xBC_x1F6E4_DdrVoltFloor_OFFSET) | (0xFF << D0F0xBC_x1F6E4_BapmDdrVoltFloor_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_NBDPM,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_NBDPM,
    0
    ),

//---------------------------------------------------------------------------
// Configure PCIe Power Island
  GNB_ENTRY_WR (
    D0F0xBC_xE0300010_TYPE,
    D0F0xBC_xE0300010_ADDRESS,
    (10 << 0 ) | (50 << 8 ) |
    (5 << 16 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030000C_TYPE,
    D0F0xBC_xE030000C_ADDRESS,
    (0xff << D0F0xBC_xE030000C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030000C_WriteOp_OFFSET) |
    (2  << D0F0xBC_xE030000C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300010_TYPE,
    D0F0xBC_xE0300010_ADDRESS,
    (50 << 0 ) | (50 << 12 )
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030000C_TYPE,
    D0F0xBC_xE030000C_ADDRESS,
    (0xff << D0F0xBC_xE030000C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030000C_WriteOp_OFFSET) |
    (3  << D0F0xBC_xE030000C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_WR (
    D0F0xBC_xE0300010_TYPE,
    D0F0xBC_xE0300010_ADDRESS,
    0x0
    ),
  GNB_ENTRY_WR (
    D0F0xBC_xE030000C_TYPE,
    D0F0xBC_xE030000C_ADDRESS,
    (0xff << D0F0xBC_xE030000C_FsmAddr_OFFSET) | (1 << D0F0xBC_xE030000C_WriteOp_OFFSET) | (1 << D0F0xBC_xE030000C_RegAddr_OFFSET)
    ),
  GNB_ENTRY_STALL (1),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_LOADLINE_ENABLE,
    TYPE_D0F0xBC ,
    0x1f428 ,
    0x40 ,
    (1 << 6 )
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_LOADLINE_ENABLE,
    SMU_MSG_TYPE,
    SMC_MSG_CONFIG_LOADLINE,
    0
    ),
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GnbMidInitTableTN [] = {
//---------------------------------------------------------------------------
// Enable LCLK Deep Sleep
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_LCLK_DEEP_SLEEP,
    TYPE_GMM,
    GMMx7A0_ADDRESS,
    GMMx7A0_DivId_MASK | GMMx7A0_RampDis_MASK | GMMx7A0_Hysteresis_MASK | GMMx7A0_SclkRunningMask_MASK | GMMx7A0_SmuBusyMask_MASK | GMMx7A0_PcieLclkIdle1Mask_MASK | GMMx7A0_PcieLclkIdle2Mask_MASK | GMMx7A0_L1imugfxIdleMask_MASK | GMMx7A0_L1imugppsbIdleMask_MASK | GMMx7A0_L1imubifIdleMask_MASK | GMMx7A0_L1imuintgenIdleMask_MASK | GMMx7A0_L2imuIdleMask_MASK | GMMx7A0_OrbIdleMask_MASK | GMMx7A0_OnInbWakeMask_MASK | GMMx7A0_OnInbWakeAckMask_MASK | GMMx7A0_OnOutbWakeMask_MASK | GMMx7A0_OnOutbWakeAckMask_MASK | GMMx7A0_DmaactiveMask_MASK,
    (0x5 << GMMx7A0_DivId_OFFSET) | (0x0 << GMMx7A0_RampDis_OFFSET) | (0xF << GMMx7A0_Hysteresis_OFFSET) | (0x1 << GMMx7A0_SclkRunningMask_OFFSET) | (0x1 << GMMx7A0_SmuBusyMask_OFFSET) | (0x1 << GMMx7A0_PcieLclkIdle1Mask_OFFSET) | (0x1 << GMMx7A0_PcieLclkIdle2Mask_OFFSET) | (0x1 << GMMx7A0_L1imugfxIdleMask_OFFSET) | (0x1 << GMMx7A0_L1imugppsbIdleMask_OFFSET) | (0x1 << GMMx7A0_L1imubifIdleMask_OFFSET) | (0x1 << GMMx7A0_L1imuintgenIdleMask_OFFSET) | (0x1 << GMMx7A0_L2imuIdleMask_OFFSET) | (0x1 << GMMx7A0_OrbIdleMask_OFFSET) | (0x1 << GMMx7A0_OnInbWakeMask_OFFSET) | (0x1 << GMMx7A0_OnInbWakeAckMask_OFFSET) | (0x1 << GMMx7A0_OnOutbWakeMask_OFFSET) | (0x1 << GMMx7A0_OnOutbWakeAckMask_OFFSET) | (0x1 << GMMx7A0_DmaactiveMask_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IGFX_DISABLED,
    TYPE_GMM,
    GMMx7A0_ADDRESS,
    GMMx7A0_SclkRunningMask_MASK,
    0x0
    ),
// Reset : 0, Enable : 1
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_LCLK_DEEP_SLEEP,
    TYPE_GMM,
    GMMx7A0_ADDRESS,
    GMMx7A0_EnableDs_MASK,
    (0x1 << GMMx7A0_EnableDs_OFFSET)
    ),
//---------------------------------------------------------------------------
// LCLK DPM init
  GNB_ENTRY_RMW (
    D0F0xBC_xE0000120_TYPE,
    D0F0xBC_xE0000120_ADDRESS,
    D0F0xBC_xE0000120_BusyCntSel_MASK | D0F0xBC_xE0000120_ActivityCntRst_MASK |
    D0F0xBC_xE0000120_PeriodCntRst_MASK | D0F0xBC_xE0000120_EnOrbUsCnt_MASK |
    D0F0xBC_xE0000120_EnOrbDsCnt_MASK,
    (0x3 << D0F0xBC_xE0000120_BusyCntSel_OFFSET) | (0 << D0F0xBC_xE0000120_ActivityCntRst_OFFSET) |
    (0x0 << D0F0xBC_xE0000120_PeriodCntRst_OFFSET) | (0x1 << D0F0xBC_xE0000120_EnOrbUsCnt_OFFSET) |
    (0x1 << D0F0xBC_xE0000120_EnOrbDsCnt_OFFSET)
    ),
  //Programming Lclk Thermal Throttling Threshold in GnbLclkDpmInitTN()
  GNB_ENTRY_RMW (
    D0F0xBC_x1F308_TYPE,
    D0F0xBC_x1F308_ADDRESS,
    D0F0xBC_x1F308_LclkThermalThrottlingEn_MASK,
    (0x1 << D0F0xBC_x1F308_LclkThermalThrottlingEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    D0F0xBC_x1F460_TYPE,
    D0F0xBC_x1F460_ADDRESS,
    D0F0xBC_x1F460_LclkDpm_MASK,
    (0x1 << D0F0xBC_x1F460_LclkDpm_OFFSET)
    ),
//---------------------------------------------------------------------------
// ORB clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_ORB_CLK_GATING,
    D0F0x98_x49_TYPE,
    D0F0x98_x49_ADDRESS,
    D0F0x98_x49_SoftOverrideClk6_MASK | D0F0x98_x49_SoftOverrideClk5_MASK | D0F0x98_x49_SoftOverrideClk4_MASK | D0F0x98_x49_SoftOverrideClk3_MASK | D0F0x98_x49_SoftOverrideClk2_MASK | D0F0x98_x49_SoftOverrideClk1_MASK | D0F0x98_x49_SoftOverrideClk0_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_ORB_CLK_GATING,
    D0F0x98_x4A_TYPE,
    D0F0x98_x4A_ADDRESS,
    D0F0x98_x4A_SoftOverrideClk6_MASK | D0F0x98_x4A_SoftOverrideClk5_MASK | D0F0x98_x4A_SoftOverrideClk4_MASK | D0F0x98_x4A_SoftOverrideClk3_MASK | D0F0x98_x4A_SoftOverrideClk2_MASK | D0F0x98_x4A_SoftOverrideClk1_MASK | D0F0x98_x4A_SoftOverrideClk0_MASK,
    (1 << D0F0x98_x4A_SoftOverrideClk0_OFFSET)
    ),

//---------------------------------------------------------------------------
// IOC clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOC_LCLK_CLOCK_GATING,
    D0F0x64_x22_TYPE,
    D0F0x64_x22_ADDRESS,
    D0F0x64_x22_SoftOverrideClk4_MASK | D0F0x64_x22_SoftOverrideClk3_MASK | D0F0x64_x22_SoftOverrideClk2_MASK | D0F0x64_x22_SoftOverrideClk1_MASK | D0F0x64_x22_SoftOverrideClk0_MASK,
    0x0
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOC_LCLK_CLOCK_GATING,
    D0F0x64_x23_TYPE,
    D0F0x64_x23_ADDRESS,
    D0F0x64_x23_SoftOverrideClk4_MASK | D0F0x64_x23_SoftOverrideClk3_MASK | D0F0x64_x23_SoftOverrideClk2_MASK | D0F0x64_x23_SoftOverrideClk1_MASK | D0F0x64_x23_SoftOverrideClk0_MASK,
    0x0
    ),

//---------------------------------------------------------------------------
// Shutdown IOMMU if disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    D0F0xBC_xE0300320_TYPE,
    D0F0xBC_xE0300320_ADDRESS,
    D0F0xBC_xE0300320_IommuPgfsmClockEn_MASK,
    1 << D0F0xBC_xE0300320_IommuPgfsmClockEn_OFFSET
    ),
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_IOMMU_DISABLED,
    D0F0xBC_xE0300018_TYPE,
    D0F0xBC_xE0300018_ADDRESS,
    (0xff << D0F0xBC_xE0300018_FsmAddr_OFFSET) | (1 << D0F0xBC_xE0300018_PowerDown_OFFSET) |
    (1 << D0F0xBC_xE0300018_P1Select_OFFSET) | (1 << D0F0xBC_xE0300018_P2Select_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IOMMU_DISABLED,
    D0F0xBC_xE0300208_TYPE,
    0xe0300208 ,
    D0F0xBC_xE0300208_P1IsoN_MASK,
    0 << D0F0xBC_xE0300208_P1IsoN_OFFSET
    ),
  GNB_ENTRY_PROPERTY_POLL (
    TABLE_PROPERTY_IOMMU_DISABLED,
    TYPE_D0F0xBC ,
    0xe0300208 ,
    0x2000 ,
    1 << 13
    ),
  GNB_ENTRY_STALL (10),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    D0F0xBC_xE0300320_TYPE,
    D0F0xBC_xE0300320_ADDRESS,
    D0F0xBC_xE0300320_IommuPgfsmClockEn_MASK,
    0x0
    ),
//---------------------------------------------------------------------------
  GNB_ENTRY_TERMINATE
};
