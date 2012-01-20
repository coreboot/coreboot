/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA Basic Level Public APIs
 *
 * Contains basic Level Initialization routines.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Interface
 * @e \$Revision: 55552 $   @e \$Date: 2011-06-22 09:31:58 -0600 (Wed, 22 Jun 2011) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include "amdlib.h"
#include "Ids.h"
#include "OptionDmi.h"
#include "OptionSlit.h"
#include "cpuLateInit.h"
#include "cpuFeatures.h"
#include "CommonInits.h"
#include "GnbInterface.h"
#include "OptionPstate.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_COMMON_AMDINITLATE_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
extern OPTION_DMI_CONFIGURATION    OptionDmiConfiguration;  // global user config record
extern OPTION_SLIT_CONFIGURATION   OptionSlitConfiguration; // global user config record
extern OPTION_PSTATE_LATE_CONFIGURATION OptionPstateLateConfiguration;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdLatePlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern BUILD_OPT_CFG UserOptions;

/*------------------------------------------------------------------------------------*/
/**
 * Initialize AmdInitLate stage platform profile and user option input.
 *
 * @param[in,out]   PlatformConfig   Platform profile/build option config structure
 * @param[in,out]   StdHeader        AMD standard header config param
 *
 * @retval      AGESA_SUCCESS     Always Succeeds.
 *
 */
AGESA_STATUS
AmdLatePlatformConfigInit (
  IN OUT   PLATFORM_CONFIGURATION    *PlatformConfig,
  IN OUT   AMD_CONFIG_PARAMS         *StdHeader
  )
{
  CommonPlatformConfigInit (PlatformConfig, StdHeader);

  return AGESA_SUCCESS;
}

/*
 *---------------------------------------------------------------------------------------
 *
 *  AmdInitLateInitializer
 *
 *  Initializer routine that will be invoked by the wrapper
 *  to initialize the input structure for the AmdInitLate
 *
 *  @param[in, out]    IN OUT   AMD_LATE_PARAMS *LateParamsPtr
 *
 *  @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdInitLateInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_LATE_PARAMS   *LateParamsPtr
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (LateParamsPtr != NULL);

  LateParamsPtr->StdHeader = *StdHeader;

  AmdLatePlatformConfigInit (&LateParamsPtr->PlatformConfig, &LateParamsPtr->StdHeader);

  LateParamsPtr->AcpiSlit = NULL;

  LateParamsPtr->AcpiSrat = NULL;

  LateParamsPtr->AcpiWheaMce = NULL;
  LateParamsPtr->AcpiWheaCmc = NULL;

  LateParamsPtr->AcpiPState = NULL;

  LateParamsPtr->DmiTable = NULL;

  LateParamsPtr->AcpiAlib = NULL;

  LateParamsPtr->IvrsExclusionRangeList = UserOptions.CfgIvrsExclusionRangeList;

  return AGESA_SUCCESS;
}

/*
 *---------------------------------------------------------------------------------------
 *
 *  AmdInitLateDestructor
 *
 *  Destruct routine that provide a chance if something need to be done
 *  before the end of AmdInitLate.
 *
 *  @param[in]    StdHeader       The standard header.
 *  @param[in]    LateParamsPtr   AMD init late param.
 *
 *  @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdInitLateDestructor (
  IN   AMD_CONFIG_PARAMS *StdHeader,
  IN   AMD_LATE_PARAMS   *LateParamsPtr
  )
{

  ASSERT (LateParamsPtr != NULL);

  (*(OptionDmiConfiguration.DmiReleaseBuffer)) (StdHeader);
  (*(OptionSlitConfiguration.SlitReleaseBuffer)) (StdHeader);

  return AGESA_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_INIT_LATE function.
 *
 * This entry point is responsible for creating any desired ACPI tables, providing
 * information for DMI, and to prepare the processors for the operating system
 * bootstrap load process.
 *
 * @param[in,out] LateParams     Required input parameters for the AMD_INIT_LATE
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD late calls invoked.
 *
 */
AGESA_STATUS
AmdInitLate (
  IN OUT   AMD_LATE_PARAMS *LateParams
  )
{
  AGESA_STATUS  AgesaStatus;
  AGESA_STATUS  AmdInitLateStatus;
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitLate: Start\n\n");
  AGESA_TESTPOINT (TpIfAmdInitLateEntry, &LateParams->StdHeader);
  IDS_PERF_TIME_MEASURE (&LateParams->StdHeader);

  ASSERT (LateParams != NULL);
  AmdInitLateStatus = AGESA_SUCCESS;

  IDS_OPTION_HOOK (IDS_INIT_LATE_BEFORE, LateParams, &LateParams->StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "CreatSystemTable: Start\n");
  // _PSS, XPSS, _PCT, _PSD, _PPC, _CST, _CSD Tables
  if ((LateParams->PlatformConfig.UserOptionPState) || (IsFeatureEnabled (IoCstate, &LateParams->PlatformConfig, &LateParams->StdHeader))) {
    AgesaStatus = ((*(OptionPstateLateConfiguration.SsdtFeature)) (&LateParams->StdHeader, &LateParams->PlatformConfig, &LateParams->AcpiPState));
    if (AgesaStatus > AmdInitLateStatus) {
      AmdInitLateStatus = AgesaStatus;
    }
  }

  // SRAT Table Generation
  if (LateParams->PlatformConfig.UserOptionSrat) {
    AgesaStatus = CreateAcpiSrat (&LateParams->StdHeader, &LateParams->AcpiSrat);
    if (AgesaStatus > AmdInitLateStatus) {
      AmdInitLateStatus = AgesaStatus;
    }
  }

  // SLIT Table Generation
  if (LateParams->PlatformConfig.UserOptionSlit) {
    AgesaStatus = CreateAcpiSlit (&LateParams->StdHeader, &LateParams->PlatformConfig, &LateParams->AcpiSlit);
    if (AgesaStatus > AmdInitLateStatus) {
      AmdInitLateStatus = AgesaStatus;
    }
  }

  // WHEA Table Generation
  if (LateParams->PlatformConfig.UserOptionWhea) {
    AgesaStatus = CreateAcpiWhea (&LateParams->StdHeader, &LateParams->AcpiWheaMce, &LateParams->AcpiWheaCmc);
    if (AgesaStatus > AmdInitLateStatus) {
      AmdInitLateStatus = AgesaStatus;
    }
  }

  // DMI Table Generation
  if (LateParams->PlatformConfig.UserOptionDmi) {
    AgesaStatus = CreateDmiRecords (&LateParams->StdHeader, &LateParams->DmiTable);
    if (AgesaStatus > AmdInitLateStatus) {
      AmdInitLateStatus = AgesaStatus;
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "CreatSystemTable: End\n");

  // Cpu Features
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: LateStart\n");
  AgesaStatus = DispatchCpuFeatures (CPU_FEAT_INIT_LATE_END, &LateParams->PlatformConfig, &LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: LateEnd\n");
  if (AgesaStatus > AmdInitLateStatus) {
    AmdInitLateStatus = AgesaStatus;
  }

  // It is the last function run by the AGESA CPU module and prepares the processor
  // for the operating system bootstrap load process.
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCpuLate: Start\n");
  AgesaStatus = AmdCpuLate (&LateParams->StdHeader, &LateParams->PlatformConfig);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCpuLate: End\n");
  if (AgesaStatus > AmdInitLateStatus) {
    AmdInitLateStatus = AgesaStatus;
  }

  AgesaStatus = GnbInitAtLate (LateParams);
  if (AgesaStatus > AmdInitLateStatus) {
    AmdInitLateStatus = AgesaStatus;
  }

  IDS_OPTION_HOOK (IDS_INIT_LATE_AFTER, LateParams, &LateParams->StdHeader);
  IDS_PERF_TIME_MEASURE (&LateParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitLateExit, &LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitLate: End\n\n");
  AGESA_TESTPOINT (EndAgesaTps, &LateParams->StdHeader);
//End Debug Print Service
  IDS_HDT_CONSOLE_EXIT (&LateParams->StdHeader);
  return  AmdInitLateStatus;
}

