/**
 * @file
 *
 * AMD Integrated Debug Option Specific Routines for HY
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
#include "IdsF10HYService.h"
#include "Filecode.h"

#define FILECODE PROC_IDS_FAMILY_0X10_HY_IDSF10HYSERVICE_FILECODE

/**
 *  IDS Backend Function for Probe Filter
 *
 *  This function is used to Enable or Disable Probe Filter.
 *
 *  @param[in,out]   DataPtr  The Pointer of PLATFORM_CONFIGURATION.
 *  @param[in,out]   StdHeader       The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]       IdsNvPtr        The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubProbeFilter (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  IDS_STATUS idsvalue;
  PLATFORM_CONFIGURATION *PlatformConfig;

  PlatformConfig = (PLATFORM_CONFIGURATION *) DataPtr;
//For SetRegistersFromTables in AmdCpuEarly will refer to probefilter status
//So we will let it know the status such as others
  IDS_NV_READ_SKIP (idsvalue, AGESA_IDS_NV_PROBEFILTER, IdsNvPtr) {
    switch (idsvalue) {
    case (IDS_STATUS) 0x1:
      PlatformConfig->PlatformProfile.UseHtAssist = FALSE;
      break;
    case (IDS_STATUS) 0x2:
      PlatformConfig->PlatformProfile.UseHtAssist = TRUE;
      break;
    default:
      break;
    }
  }
  return IDS_SUCCESS;
}

/**
 *  IDS Backend Function for Probe Filter ealry before
 *
 *  This function is used to Enable or Disable Probe Filter.
 *
 *  @param[in,out]   DataPtr  The Pointer of PLATFORM_CONFIGURATION.
 *  @param[in,out]   StdHeader       The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]       IdsNvPtr        The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubProbeFilterEarlyBefore (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  AMD_EARLY_PARAMS * EarlyParams;

  EarlyParams = (AMD_EARLY_PARAMS *)DataPtr;
  return IdsSubProbeFilter (&EarlyParams->PlatformConfig, StdHeader, IdsNvPtr);
}


/**
 *  IDS Backend Function for Probe Filter Late before
 *
 *  This function is used to Enable or Disable Probe Filter.
 *
 *  @param[in,out]   DataPtr  The Pointer of PLATFORM_CONFIGURATION.
 *  @param[in,out]   StdHeader       The Pointer of AMD_CONFIG_PARAMS.
 *  @param[in]       IdsNvPtr        The Pointer of NV Table.
 *
 *  @retval IDS_SUCCESS         Backend function is called successfully.
 *  @retval IDS_UNSUPPORTED     No Backend function is found.
 *
 **/
IDS_STATUS
IdsSubProbeFilterLateBefore (
  IN OUT   VOID *DataPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       IDS_NV_ITEM *IdsNvPtr
  )
{
  AMD_LATE_PARAMS * LateParams;

  LateParams = (AMD_LATE_PARAMS *)DataPtr;
  return IdsSubProbeFilter (&LateParams->PlatformConfig, StdHeader, IdsNvPtr);
}

CONST IDS_FEAT_STRUCT ROMDATA IdsFeatProbeFilterBlockPlatformCfg =
{
  IDS_FEAT_PROBE_FILTER,
  IDS_ALL_CORES,
  IDS_PLATFORMCFG_OVERRIDE,
  AMD_FAMILY_10_HY,
  IdsSubProbeFilter
};

CONST IDS_FEAT_STRUCT ROMDATA IdsFeatProbeFilterBlockEarlyBefore =
{
  IDS_FEAT_PROBE_FILTER,
  IDS_ALL_CORES,
  IDS_INIT_EARLY_BEFORE,
  AMD_FAMILY_10_HY,
  IdsSubProbeFilterEarlyBefore
};

CONST IDS_FEAT_STRUCT ROMDATA IdsFeatProbeFilterBlockLateBefore =
{
  IDS_FEAT_PROBE_FILTER,
  IDS_ALL_CORES,
  IDS_INIT_LATE_BEFORE,
  AMD_FAMILY_10_HY,
  IdsSubProbeFilterLateBefore
};