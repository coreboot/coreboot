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
 * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with AMD.  This header does *NOT* give you permission to use the Materials
 * or any rights under AMD's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by AMD shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * AMD does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by AMD, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, AMD retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgement of AMD's proprietary rights in them.
 *
 * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
 * direct product thereof will be exported directly or indirectly, into any
 * country prohibited by the United States Export Administration Act and the
 * regulations thereunder, without the required authorization from the U.S.
 * government nor will be used for any purpose prohibited by the same.
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


