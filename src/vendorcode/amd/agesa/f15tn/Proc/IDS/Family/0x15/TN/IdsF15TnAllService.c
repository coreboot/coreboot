/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Integrated Debug Option Specific Routines for common F15
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
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
#include "cpuF15PowerMgmt.h"
#include "cpuF15TnPowerMgmt.h"
#include "IdsF15AllService.h"
#include "IdsF15TnAllService.h"
#include "IdsF15TnNvDef.h"
#include "Gnb.h"
#include "GnbGfx.h"
#include "GnbRegistersTN.h"
#include "GnbRegisterAccTN.h"
#include "IdsRegAcc.h"
#include "Filecode.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_IDS_FAMILY_0X15_TN_IDSF15TNALLSERVICE_FILECODE

/**
 *  IDS F15 Backend Function for HTC Controls
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
IdsSubHTCControlF15Tn (
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
IdsSubMemoryMappingF15Tn (
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

  IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_CHANNEL_INTERLEAVE, IdsNvPtr, StdHeader) {
    switch (NvValue) {
    case  IdsNvMemMappingChlInterleaveAddress_bit_6:
    case  IdsNvMemMappingChlInterleaveAddress_bit_12:
    case  IdsNvMemMappingChlInterleaveHash__exclusive_OR_of_address_bits_20_16__6_:
    case  IdsNvMemMappingChlInterleaveHash__excluseive_OR_of_address_bits_20_16__9_:
    case  IdsNvMemMappingChlInterleaveAddress_bit_8:
    case  IdsNvMemMappingChlInterleaveAddress_bit_9:
      RefPtr->EnableChannelIntlv = TRUE;
      break;
    case  IdsNvMemMappingChlInterleaveDisabled:
      RefPtr->EnableChannelIntlv = FALSE;
      break;
    case  IdsNvMemMappingChlInterleaveAuto:
      break;
    default:
      ASSERT (FALSE);
      break;
    }
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Channel Interleave F10
 *
 *  This function is used to override Channel Interleave.
 *
 *  @param[in,out]   DataPtr      The Pointer of Data to Override.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
STATIC IDS_STATUS
IdsIntSubChannelInterleaveF15Tn (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS NvValue;
  UINT8 *PDctSelIntLvAddr;

  PDctSelIntLvAddr = (UINT8 *)DataPtr;
  //DctSelIntLvAddr DCT Select Function
  //000b            Address bit 6.
  //001b            Address bit 12.
  //010b            Hash: exclusive OR of address bits[20:16, 6].
  //011b            Hash: exclusive OR of address bits[20:16, 9].
  //100b            Address bit 8.
  //101b            Address bit 9.
  IDS_NV_READ_SKIP (NvValue, AGESA_IDS_NV_CHANNEL_INTERLEAVE, IdsNvPtr, StdHeader) {
    switch (NvValue) {
    case  IdsNvMemMappingChlInterleaveAddress_bit_6:
      *PDctSelIntLvAddr = 0;
      break;
    case  IdsNvMemMappingChlInterleaveAddress_bit_12:
      *PDctSelIntLvAddr = 1;
      break;
    case  IdsNvMemMappingChlInterleaveHash__exclusive_OR_of_address_bits_20_16__6_:
      *PDctSelIntLvAddr = 2;
      break;
    case  IdsNvMemMappingChlInterleaveHash__excluseive_OR_of_address_bits_20_16__9_:
      *PDctSelIntLvAddr = 3;
      break;
    case  IdsNvMemMappingChlInterleaveAddress_bit_8:
      *PDctSelIntLvAddr = 4;
      break;
    case  IdsNvMemMappingChlInterleaveAddress_bit_9:
      *PDctSelIntLvAddr = 5;
      break;
    case  IdsNvMemMappingChlInterleaveDisabled:
    case  IdsNvMemMappingChlInterleaveAuto:
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
IdsSubGnbPlatformCfgF15Tn (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  GFX_PLATFORM_CONFIG   *PGfx;
  IDS_STATUS NvValue;

  PGfx = (GFX_PLATFORM_CONFIG*) DataPtr;
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
IdsRegSetGmmxF15Tn (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_REG_GMMX *PIdsRegGmmx;
  UINT32 Value;

  PIdsRegGmmx = (IDS_REG_GMMX *) DataPtr;
  GnbRegisterReadTN (
    TYPE_GMM,
    PIdsRegGmmx->Offset,
    &Value,
    0,
    StdHeader);

  IdsLibDataMaskSet32 (&Value, PIdsRegGmmx->AndMask, PIdsRegGmmx->OrMask);

  GnbRegisterWriteTN (
    TYPE_GMM,
    PIdsRegGmmx->Offset,
    &Value,
    0,
    StdHeader);

  return IDS_SUCCESS;
}

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatHtcControlBlockF15Tn =
{
  IDS_FEAT_HTC_CTRL,
  IDS_ALL_CORES,
  IDS_HTC_CTRL,
  AMD_FAMILY_15_TN,
  IdsSubHTCControlF15Tn
};


CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingPostBeforeBlockF15Tn =
{
  IDS_FEAT_MEMORY_MAPPING,
  IDS_ALL_CORES,
  IDS_INIT_POST_BEFORE,
  AMD_FAMILY_15_TN,
  IdsSubMemoryMappingF15Tn
};

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatMemoryMappingChIntlvBlockF15Tn =
{
  IDS_FEAT_MEMORY_MAPPING,
  IDS_ALL_CORES,
  IDS_CHANNEL_INTERLEAVE,
  AMD_FAMILY_15_TN,
  IdsIntSubChannelInterleaveF15Tn
};

CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatGnbPlatformCfgBlockF15Tn =
{
  IDS_FEAT_GNB_PLATFORMCFG,
  IDS_ALL_CORES,
  IDS_GNB_PLATFORMCFG_OVERRIDE,
  AMD_FAMILY_15_TN,
  IdsSubGnbPlatformCfgF15Tn
};

// For register access
CONST IDS_FAMILY_FEAT_STRUCT ROMDATA IdsFeatRegGmmxF15Tn =
  MAKE_IDS_FAMILY_FEAT_ALL_CORES (
    IDS_FAM_REG_GMMX,
    AMD_FAMILY_15_TN,
    IdsRegSetGmmxF15Tn
  );


