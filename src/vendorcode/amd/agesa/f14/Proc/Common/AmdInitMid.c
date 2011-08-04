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
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
 *
 */
/*
 *****************************************************************************
 *
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
 * ***************************************************************************
 *
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "Ids.h"
#include "cpuFeatures.h"
#include "CommonInits.h"
#include "CreateStruct.h"
#include "GnbInterface.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_COMMON_AMDINITMID_FILECODE
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
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdInitMid: Start\n\n");
  AGESA_TESTPOINT (TpIfAmdInitMidEntry, &MidParams->StdHeader);
  IDS_PERF_TIME_MEASURE (&MidParams->StdHeader);

  AgesaStatus = AGESA_SUCCESS;

  ASSERT (MidParams != NULL);
  IDS_OPTION_HOOK (IDS_INIT_MID_BEFORE, MidParams, &MidParams->StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: MidStart\n");
  CalledStatus = DispatchCpuFeatures (CPU_FEAT_INIT_MID_END, &MidParams->PlatformConfig, &MidParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "DispatchCpuFeatures: MidEnd\n");
  if (CalledStatus > AgesaStatus) {
    AgesaStatus = CalledStatus;
  }

  CalledStatus = GnbInitAtMid (MidParams);
  if (CalledStatus > AgesaStatus) {
    AgesaStatus = CalledStatus;
  }

  IDS_OPTION_HOOK (IDS_INIT_MID_AFTER, MidParams, &MidParams->StdHeader);

  IDS_PERF_TIME_MEASURE (&MidParams->StdHeader);
  AGESA_TESTPOINT (TpIfAmdInitMidExit, &MidParams->StdHeader);
  IDS_HDT_CONSOLE (MAIN_FLOW, "\nAmdInitMid: End\n\n");
  IDS_HDT_CONSOLE_FLUSH_BUFFER (&MidParams->StdHeader);
  return AgesaStatus;
}


