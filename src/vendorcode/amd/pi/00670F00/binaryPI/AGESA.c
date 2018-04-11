/* $NoKeywords:$ */
/**
 * @file
 *
 * Agesa structures and definitions
 *
 * Contains AMD AGESA core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 85818 $   @e \$Date: 2013-01-11 17:04:21 -0600 (Fri, 11 Jan 2013) $
 */
/*****************************************************************************
 *
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
 *
 ***************************************************************************/
// TODO This list needs to be pruned of anything that is not API
#include "AGESA.h"
#include <amdblocks/agesawrapper.h>
#include "cbfs.h"
#include <console/console.h>
#include <commonlib/loglevel.h>

CONST UINT32 ImageSignature = IMAGE_SIGNATURE;
CONST UINT32 ModuleSignature = MODULE_SIGNATURE;
CONST CHAR8 ModuleIdentifier[8] = AGESA_ID;

/**********************************************************************
 * Interface call:  AmdCreateStruct
 **********************************************************************/
AGESA_STATUS
AmdCreateStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	InterfaceParams->StdHeader.Func = AMD_CREATE_STRUCT;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(InterfaceParams);
}

/**********************************************************************
 * Interface call:  AmdReleaseStruct
 **********************************************************************/
AGESA_STATUS
AmdReleaseStruct (
  IN OUT   AMD_INTERFACE_PARAMS *InterfaceParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	InterfaceParams->StdHeader.Func = AMD_RELEASE_STRUCT;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(InterfaceParams);
}

/**********************************************************************
 * Interface call:  AmdInitReset
 **********************************************************************/
AGESA_STATUS
AmdInitReset (
  IN OUT   AMD_RESET_PARAMS     *ResetParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	ResetParams->StdHeader.Func = AMD_INIT_RESET;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(ResetParams);
}

/**********************************************************************
 * Interface call:  AmdInitEarly
 **********************************************************************/
AGESA_STATUS
AmdInitEarly (
  IN OUT   AMD_EARLY_PARAMS     *EarlyParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	EarlyParams->StdHeader.Func = AMD_INIT_EARLY;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(EarlyParams);
}

/**********************************************************************
 * Interface call:  AmdInitPost
 **********************************************************************/
AGESA_STATUS
AmdInitPost (
  IN OUT   AMD_POST_PARAMS      *PostParams         ///< Amd Cpu init param
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	PostParams->StdHeader.Func = AMD_INIT_POST;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(PostParams);
}

/**********************************************************************
 * Interface call:  AmdInitEnv
 **********************************************************************/
AGESA_STATUS
AmdInitEnv (
  IN OUT   AMD_ENV_PARAMS       *EnvParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	EnvParams->StdHeader.Func = AMD_INIT_ENV;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(EnvParams);
}

/**********************************************************************
 * Interface call:  AmdInitMid
 **********************************************************************/
AGESA_STATUS
AmdInitMid (
  IN OUT   AMD_MID_PARAMS       *MidParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	MidParams->StdHeader.Func = AMD_INIT_MID;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(MidParams);
}

/**********************************************************************
 * Interface call:  AmdInitLate
 **********************************************************************/
AGESA_STATUS
AmdInitLate (
  IN OUT   AMD_LATE_PARAMS      *LateParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	LateParams->StdHeader.Func = AMD_INIT_LATE;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(LateParams);
}

/**********************************************************************
 * Interface call:  AmdInitRecovery
 **********************************************************************/
AGESA_STATUS
AmdInitRecovery (
  IN OUT   AMD_RECOVERY_PARAMS    *RecoveryParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	RecoveryParams->StdHeader.Func = AMD_INIT_RECOVERY;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(RecoveryParams);
}

/**********************************************************************
 * Interface call:  AmdInitResume
 **********************************************************************/
AGESA_STATUS
AmdInitResume (
  IN       AMD_RESUME_PARAMS    *ResumeParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	ResumeParams->StdHeader.Func = AMD_INIT_RESUME;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(ResumeParams);
}

/**********************************************************************
 * Interface call:  AmdS3LateRestore
 **********************************************************************/
AGESA_STATUS
AmdS3LateRestore (
  IN OUT   AMD_S3LATE_PARAMS    *S3LateParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	S3LateParams->StdHeader.Func = AMD_S3LATE_RESTORE;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(S3LateParams);
}

/**********************************************************************
 * Interface call:  AmdS3FinalRestore
 **********************************************************************/
AGESA_STATUS
AmdS3FinalRestore (
  IN OUT   AMD_S3FINAL_PARAMS    *S3FinalParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	S3FinalParams->StdHeader.Func = AMD_S3FINAL_RESTORE;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(S3FinalParams);
}

/**********************************************************************
 * Interface call:  AmdInitRtb
 **********************************************************************/
AGESA_STATUS
AmdInitRtb (
  IN OUT   AMD_RTB_PARAMS *AmdInitRtbParams
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdInitRtbParams->StdHeader.Func = AMD_INIT_RTB;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdInitRtbParams);
}

/**********************************************************************
 * Interface call:  AmdLateRunApTask
 **********************************************************************/
AGESA_STATUS
AmdLateRunApTask (
  IN       AP_EXE_PARAMS  *AmdApExeParams
)
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdApExeParams->StdHeader.Func = AMD_LATE_RUN_AP_TASK;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdApExeParams);
}

/**********************************************************************
 * Interface service call:  AmdGetApicId
 **********************************************************************/
AGESA_STATUS
AmdGetApicId (
  IN OUT AMD_APIC_PARAMS *AmdParamApic
)
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdParamApic->StdHeader.Func = AMD_GET_APIC_ID;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdParamApic);
}

/**********************************************************************
 * Interface service call:  AmdGetPciAddress
 **********************************************************************/
AGESA_STATUS
AmdGetPciAddress (
  IN OUT   AMD_GET_PCI_PARAMS *AmdParamGetPci
)
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdParamGetPci->StdHeader.Func = AMD_GET_PCI_ADDRESS;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdParamGetPci);
}

/**********************************************************************
 * Interface service call:  AmdIdentifyCore
 **********************************************************************/
AGESA_STATUS
AmdIdentifyCore (
  IN OUT  AMD_IDENTIFY_PARAMS *AmdParamIdentify
)
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdParamIdentify->StdHeader.Func = AMD_IDENTIFY_CORE;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdParamIdentify);
}

/**********************************************************************
 * Interface service call:  AmdReadEventLog
 **********************************************************************/
AGESA_STATUS
AmdReadEventLog (
  IN       EVENT_PARAMS *Event
)
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	Event->StdHeader.Func = AMD_READ_EVENT_LOG;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(Event);
}

/**********************************************************************
 * Interface service call:  AmdIdentifyDimm
 **********************************************************************/
AGESA_STATUS
AmdIdentifyDimm (
  IN OUT   AMD_IDENTIFY_DIMM *AmdDimmIdentify
)
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdDimmIdentify->StdHeader.Func = AMD_IDENTIFY_DIMMS;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdDimmIdentify);
}

AGESA_STATUS
AmdIdsRunApTaskLate (
  IN       AP_EXE_PARAMS  *AmdApExeParams
  )
{
	AmdApExeParams->StdHeader.Func = -1;
	return AGESA_UNSUPPORTED;
}

/**********************************************************************
 * Interface service call:  AmdGet2DDataEye
 **********************************************************************/
AGESA_STATUS
AmdGet2DDataEye (
  IN OUT   AMD_GET_DATAEYE *AmdGetDataEye
  )
{
	MODULE_ENTRY Dispatcher = agesa_get_dispatcher();
	AmdGetDataEye->StdHeader.Func = AMD_GET_2D_DATA_EYE;
	if (!Dispatcher) return AGESA_UNSUPPORTED;
	return Dispatcher(AmdGetDataEye);
}
