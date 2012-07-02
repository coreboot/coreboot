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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*****************************************************************************
 *
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

  IDS_PERF_TIMESTAMP (&LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitLate: Start\n\n");
  AGESA_TESTPOINT (TpIfAmdInitLateEntry, &LateParams->StdHeader);

  ASSERT (LateParams != NULL);
  AmdInitLateStatus = AGESA_SUCCESS;

  IDS_OPTION_HOOK (IDS_INIT_LATE_BEFORE, LateParams, &LateParams->StdHeader);

  IDS_PERF_TIMESTAMP (&LateParams->StdHeader);
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
  IDS_PERF_TIMESTAMP (&LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: LateStart\n");
  AgesaStatus = DispatchCpuFeatures (CPU_FEAT_INIT_LATE_END, &LateParams->PlatformConfig, &LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: LateEnd\n");
  if (AgesaStatus > AmdInitLateStatus) {
    AmdInitLateStatus = AgesaStatus;
  }

  // It is the last function run by the AGESA CPU module and prepares the processor
  // for the operating system bootstrap load process.
  IDS_PERF_TIMESTAMP (&LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCpuLate: Start\n");
  AgesaStatus = AmdCpuLate (&LateParams->StdHeader, &LateParams->PlatformConfig);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCpuLate: End\n");
  if (AgesaStatus > AmdInitLateStatus) {
    AmdInitLateStatus = AgesaStatus;
  }

  IDS_PERF_TIMESTAMP (&LateParams->StdHeader);
  AgesaStatus = GnbInitAtLate (LateParams);
  if (AgesaStatus > AmdInitLateStatus) {
    AmdInitLateStatus = AgesaStatus;
  }

  IDS_OPTION_HOOK (IDS_INIT_LATE_AFTER, LateParams, &LateParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitLateExit, &LateParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitLate: End\n\n");
  AGESA_TESTPOINT (EndAgesaTps, &LateParams->StdHeader);
//End Debug Print Service
  IDS_HDT_CONSOLE_EXIT (&LateParams->StdHeader);
  IDS_PERF_TIMESTAMP (&LateParams->StdHeader);
  IDS_PERF_ANALYSE (&LateParams->StdHeader);

  return  AmdInitLateStatus;
}

