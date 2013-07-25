/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Option Specific Routines for common F16
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 */
/*****************************************************************************
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
 ******************************************************************************
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "IdsLib.h"
#include "cpuFamilyTranslation.h"
#include "cpuF16PowerMgmt.h"
#include "F16KbPowerMgmt.h"
#include "IdsF16KbAllService.h"
#include "IdsF16KbNvDef.h"
#include "Gnb.h"
#include "GnbGfx.h"
#include "GnbRegistersKB.h"
#include "GnbPcie.h"
#include "GnbHandleLib.h"
#include "GnbRegisterAccKB.h"
#include "IdsRegAcc.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_IDS_FAMILY_0X16_KB_IDSF16KBALLSERVICE_FILECODE

/**
 *  IDS F16 Backend Function for HTC Controls
 *
 *  This function is used to override HTC control Parameter.
 *
 *  @param[in,out]   DataPtr      The Pointer of HTC register.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
STATIC IDS_STATUS
IdsSubHTCControlF16Kb (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  HTC_REGISTER *PHtcReg;
  IDS_STATUS NvValue;

  PHtcReg = (HTC_REGISTER *) DataPtr;
  IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_HTC_EN, IdsNvPtr, StdHeader) {
    switch (NvValue) {
    case  IdsNvThermalHTCEnDisabled:
      PHtcReg->HtcEn = 0;
      break;
    case  IdsNvThermalHTCEnEnabled:
      PHtcReg->HtcEn = 1;
      break;
    case  IdsNvThermalHTCEnAuto:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }

  IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_HTC_OVERRIDE, IdsNvPtr, StdHeader) {
    switch (NvValue) {
    case  IdsNvThermalHTCOverrideDisabled:
      break;
    case  IdsNvThermalHTCOverrideEnabled:
      IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_HTC_PSTATE_LIMIT, IdsNvPtr, StdHeader) {
        ASSERT ((NvValue >= IdsNvThermalHtcPstateLimitMin) && (NvValue <= IdsNvThermalHtcPstateLimitMax));
        PHtcReg->HtcPstateLimit = NvValue;
      }

      IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_HTC_TEMP_HYS, IdsNvPtr, StdHeader) {
        ASSERT ((NvValue >= IdsNvThermalHTCTempHysMin) && (NvValue <= IdsNvThermalHTCTempHysMax));
        PHtcReg->HtcHystLmt = NvValue;
      }

      IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_HTC_ACT_TEMP, IdsNvPtr, StdHeader) {
        ASSERT ((NvValue >= IdsNvThermalHTCActTempMin) && (NvValue <= IdsNvThermalHTCActTempMax));
        PHtcReg->HtcTmpLmt = NvValue;
      }
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Memory Mapping
 *
 *  This function is used to override the following setting.
 *    EnableBankIntlv, ChannelIntlvMode, EnableNodeIntlv, MemHole,
 *    EnablePowerDown, PowerDownMode, EnableBurstLen32, BankSwizzle,
 *    UserTimingMode, MemClockValue, EnableParity, DqsTrainCtl, AllMemClks,
 *    and EnableClkHZAltVidC3.
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
STATIC IDS_STATUS
IdsSubMemoryMappingF16Kb (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  AMD_POST_PARAMS *PostParamsPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  IDS_STATUS NvValue;
  MEM_DATA_STRUCT * memdataptr;

  PostParamsPtr = (AMD_POST_PARAMS *)DataPtr;
  memdataptr = PostParamsPtr->MemConfig.MemData;
  RefPtr = memdataptr->ParameterListPtr;
  IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_BANK_INTERLEAVE, IdsNvPtr, StdHeader) {
    switch (NvValue) {
    case  IdsNvMemMappingBankInterleaveDisabled:
      RefPtr->EnableBankIntlv = FALSE;
      break;
    case  IdsNvMemMappingBankInterleaveAuto:
      RefPtr->EnableBankIntlv = TRUE;
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for override GNB platform config
 *
 *  @param[in,out]   DataPtr      The Pointer of BOOLEAN.
 *  @param[in,out]   StdHeader The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
STATIC IDS_STATUS
IdsSubGnbPlatformCfgF16Kb (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  GFX_PLATFORM_CONFIG   *PGfx;
  IDS_STATUS NvValue;

  PGfx = (GFX_PLATFORM_CONFIG*) DataPtr;
  //NB Azalia
  IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_GNBHDAUDIOEN, IdsNvPtr, StdHeader) {
    switch (NvValue) {
    //Auto
    case  IdsNvGnbGfxNbAzaliaAuto:
      break;
    //Disabled
    case  IdsNvGnbGfxNbAzaliaDisabled:
      PGfx->GnbHdAudio = 0;
      break;
    //Enabled
    case  IdsNvGnbGfxNbAzaliaEnabled:
      PGfx->GnbHdAudio = 1;
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_SUCCESS;
}
/**
 *  IDS Family specific Function for programming GMMX register
 *
 *  @param[in,out]   DataPtr      The Pointer of BOOLEAN.
 *  @param[in,out]   StdHeader The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *
 **/
STATIC IDS_STATUS
IdsRegSetGmmxF16Kb (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IdsRegAcc132_STRUCT *PIdsRegGmmx;
  UINT32 Value;
  GNB_HANDLE *GnbHandle;

  PIdsRegGmmx = (IdsRegAcc132_STRUCT *) DataPtr;
  GnbHandle = GnbGetHandle (StdHeader);

  GnbRegisterReadKB (
    GnbHandle,
    0x12,
    PIdsRegGmmx->Offset,
    &Value,
    0,
    StdHeader);

  IdsLibDataMaskSet32 (&Value, PIdsRegGmmx->AndMask, PIdsRegGmmx->OrMask);

  GnbRegisterWriteKB (
    GnbHandle,
    0x12,
    PIdsRegGmmx->Offset,
    &Value,
    0,
    StdHeader);

  return IDS_SUCCESS;
}

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtcControlBlockF16Kb =
{
  IDS_FEAT_HTC_CTRL,
  IDS_ALL_CORES,
  IDS_HTC_CTRL,
  AMD_FAMILY_16_KB,
  IdsSubHTCControlF16Kb
};


CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingPostBeforeBlockF16Kb =
{
  IDS_FEAT_MEMORY_MAPPING,
  IDS_ALL_CORES,
  IDS_INIT_POST_BEFORE,
  AMD_FAMILY_16_KB,
  IdsSubMemoryMappingF16Kb
};

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF16Kb =
{
  IDS_FEAT_GNB_PLATFORMCFG,
  IDS_ALL_CORES,
  IDS_GNB_PLATFORMCFG_OVERRIDE,
  AMD_FAMILY_16_KB,
  IdsSubGnbPlatformCfgF16Kb
};

// For register access
CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatRegGmmxF16Kb =
  MAKE_IDS_FAMILY_FEAT_ALL_CORES (
    dummy210,
    AMD_FAMILY_16_KB,
    IdsRegSetGmmxF16Kb
  );


