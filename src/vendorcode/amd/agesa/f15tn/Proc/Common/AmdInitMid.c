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
#include "Ids.h"
#include "cpuFeatures.h"
#include "CommonInits.h"
#include "GnbInterface.h"
#include "AmdFch.h"
#include "Filecode.h"
#include "heapManager.h"
#include "CreateStruct.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITMID_FILECODE

extern BLDOPT_FCH_FUNCTION  BldoptFchFunction;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*
 *---------------------------------------------------------------------------------------
 *
 *  Initializer routine that will be invoked by the wrapper
 *  to initialize the input structure for the AmdInitMid
 *
 *  @param[in,out]  MidParamsPtr   Newly created interface parameters for AmdInitMid
 *
 *  @retval         AGESA_SUCCESS  Always succeeds
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdInitMidInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_MID_PARAMS    *MidParamsPtr
  )
{
  ASSERT (StdHeader != NULL);
  ASSERT (MidParamsPtr != NULL);

  MidParamsPtr->StdHeader = *StdHeader;
  CommonPlatformConfigInit (&MidParamsPtr->PlatformConfig, &MidParamsPtr->StdHeader);
  BldoptFchFunction.InitMidConstructor (MidParamsPtr);

  return AGESA_SUCCESS;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for the AMD_INIT_MID function.
 *
 * This entry point is responsible for performing any necessary functions needed
 * after PCI bus enumeration and just before control is passed to the video option ROM.
 *
 * @param[in,out] MidParams      Required input parameters for the AMD_INIT_MID
 *                                  entry point.
 *
 * @return        Aggregated status across all internal AMD mid calls invoked.
 *
 */
AGESA_STATUS
AmdInitMid (
  IN OUT   AMD_MID_PARAMS *MidParams
  )
{
  AGESA_STATUS  AgesaStatus;
  AGESA_STATUS  CalledStatus;

  IDS_PERF_TIMESTAMP (&MidParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitMid: Start\n\n");
  AGESA_TESTPOINT (TpIfAmdInitMidEntry, &MidParams->StdHeader);

  AgesaStatus = AGESA_SUCCESS;

  ASSERT (MidParams != NULL);
  IDS_OPTION_HOOK (IDS_INIT_MID_BEFORE, MidParams, &MidParams->StdHeader);

  IDS_PERF_TIMESTAMP (&MidParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: MidStart\n");
  CalledStatus = DispatchCpuFeatures (CPU_FEAT_INIT_MID_END, &MidParams->PlatformConfig, &MidParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: MidEnd\n");
  if (CalledStatus > AgesaStatus) {
    AgesaStatus = CalledStatus;
  }

  IDS_PERF_TIMESTAMP (&MidParams->StdHeader);
  CalledStatus = BldoptFchFunction.InitMid (MidParams);
  AgesaStatus = (CalledStatus > AgesaStatus) ? CalledStatus : AgesaStatus;

  IDS_PERF_TIMESTAMP (&MidParams->StdHeader);
  CalledStatus = GnbInitAtMid (MidParams);
  if (CalledStatus > AgesaStatus) {
    AgesaStatus = CalledStatus;
  }

  IDS_OPTION_HOOK (IDS_INIT_MID_AFTER, MidParams, &MidParams->StdHeader);

  AGESA_TESTPOINT (TpIfAmdInitMidExit, &MidParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitMid: End\n\n");
  IDS_HDT_CONSOLE_FLUSH_BUFFER (&MidParams->StdHeader);
  IDS_PERF_TIMESTAMP (&MidParams->StdHeader);

  return AgesaStatus;
}


