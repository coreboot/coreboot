/**
 * @file
 *
 * AMD Integrated Debug Option Specific Routines for common F10
 *
 * Contains AMD AGESA debug macros and library functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision  $   @e \$Date  $
 */
/*****************************************************************************
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
 * 
 ******************************************************************************
 */
/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "Ids.h"
#include "IdsLib.h"
#include "cpuFamilyTranslation.h"
#include "IdsF10AllService.h"
#include "Filecode.h"

#define FILECODE PROC_IDS_FAMILY_0X10_IDSF10ALLSERVICE_FILECODE


/**
 *  IDS F10 Backend Function for ECC Controls
 *
 *  This function is used to override ECC control Parameter.
 *
 *  @param[in,out]   DataPtr      The Pointer of AMD_POST_PARAMS.
 *  @param[in,out]   StdHeader    The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]     IdsNvPtr     The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubEccControlF10 (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  ECC_OVERRIDE_STRUCT * PeccOverStruct;
  IDS_STATUS ids_value;

  PeccOverStruct = (ECC_OVERRIDE_STRUCT *)DataPtr;

  IDS_NV_READ_SKIP (ids_value, AGESA_IDS_NV_SCRUB_REDIRECTION, IdsNvPtr) {
    PeccOverStruct->CfgEccRedirection = (BOOLEAN) ids_value;
  }

  IDS_NV_READ_SKIP (ids_value, AGESA_IDS_NV_SYNC_ON_ECC_ERROR, IdsNvPtr) {
    PeccOverStruct->CfgEccSyncFlood = (BOOLEAN) ids_value;
  }

  IDS_NV_READ_SKIP (ids_value, AGESA_IDS_NV_DRAM_SCRUB, IdsNvPtr) {
    if (ids_value == IDS_F10_SCRUB_DIS) {
      PeccOverStruct->CfgScrubDramRate = 0;
    } else if ((ids_value <= IDS_F10_SCRUB_84MS) && (ids_value >= IDS_F10_SCRUB_40NS)) {
      PeccOverStruct->CfgScrubDramRate = (UINT16) (ids_value - 1);
    }
  }

  IDS_NV_READ_SKIP (ids_value, AGESA_IDS_NV_L2_SCRUB, IdsNvPtr) {
    if (ids_value == IDS_F10_SCRUB_DIS) {
      PeccOverStruct->CfgScrubL2Rate = 0;
    } else if ((ids_value <= IDS_F10_SCRUB_84MS) && (ids_value >= IDS_F10_SCRUB_40NS)) {
      PeccOverStruct->CfgScrubL2Rate = (UINT16) (ids_value - 1);
    }
  }

  IDS_NV_READ_SKIP (ids_value, AGESA_IDS_NV_L3_SCRUB, IdsNvPtr) {
    if (ids_value == IDS_F10_SCRUB_DIS) {
      PeccOverStruct->CfgScrubL3Rate = 0;
    } else if ((ids_value <= IDS_F10_SCRUB_84MS) && (ids_value >= IDS_F10_SCRUB_40NS)) {
      PeccOverStruct->CfgScrubL3Rate = (UINT16) (ids_value - 1);
    }
  }

  IDS_NV_READ_SKIP (ids_value, AGESA_IDS_NV_DCACHE_SCRUB, IdsNvPtr) {
    if (ids_value == IDS_F10_SCRUB_DIS) {
      PeccOverStruct->CfgScrubDcRate = 0;
    } else if ((ids_value <= IDS_F10_SCRUB_84MS) && (ids_value >= IDS_F10_SCRUB_40NS)) {
      PeccOverStruct->CfgScrubDcRate = (UINT16) (ids_value - 1);
    }
  }
  return IDS_SUCCESS;
}



CONST IDS_FEAT_STRUCT ROMDATA IdsFeatEccCtrlBlockF10 =
{
  IDS_FEAT_ECC_CTRL,
  IDS_BSP_ONLY,
  IDS_ECC,
  AMD_FAMILY_10,
  IdsSubEccControlF10
};
