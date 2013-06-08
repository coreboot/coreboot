/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD AGESA Input Structure Creation
 *
 * Contains AGESA input creation structures.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
 *
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

#ifndef _CREATE_STRUCT_H_
#define _CREATE_STRUCT_H_

/**
 * A constructor method.
 *
 * Sets inputs to valid, basic level, defaults for the specific service instance.
 * Constructors should avoid using the header, since these routines should not
 * do operations which may fail or require status back to the user.  The constructor
 * should always SUCCEED.
 *
 * @param[in]   StdHeader         Opaque handle to standard config header
 * @param[in]   ServiceInterface  Service Interface structure to initialize.
 *
 * @retval AGESA_SUCCESS      Constructors are not allowed to fail
*/
typedef AGESA_STATUS
F_AGESA_FUNCTION (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       VOID                 *ServiceInterface
  );

/// Reference to a Method.
typedef F_AGESA_FUNCTION *PF_AGESA_FUNCTION;

/**
 * A Destructor method.
 *
 * Sets inputs to valid, basic level, defaults for the specific service instance.
 * The constructor should always SUCCEED.
 *
 * @param[in]   StdHeader         Opaque handle to standard config header.
 * @param[in]   ServiceInterface  Service Interface structure to initialize.
 *
 * @retval AGESA_SUCCESS      Constructors are not allowed to fail
*/
typedef AGESA_STATUS
F_AGESA_DESTRUCTOR (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       VOID                 *ServiceInterface
  );

/// Reference to a Method.
typedef F_AGESA_DESTRUCTOR *PF_AGESA_DESTRUCTOR;

/**
 * Provide the information needed to invoke each service constructor.
 */
typedef struct {
  IN       AGESA_STRUCT_NAME   AgesaFunctionName;  ///< Identifies the service
  IN       UINT16              CreateStructSize;   ///< The service's input struct size.
                                                   ///  Do NOT include a config params header!
     OUT   PF_AGESA_FUNCTION   AgesaFunction;      ///< The constructor function
     OUT   PF_AGESA_DESTRUCTOR AgesaDestructor;    ///< The destructor function.
  IN       AGESA_BUFFER_HANDLE BufferHandle;       ///< The buffer handle id for the service.
} FUNCTION_PARAMS_INFO;

/**
 * All available services have their constructor info here.
 */
AGESA_STATUS
AmdInitResetConstructor (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_RESET_PARAMS     *AmdResetParams
  );

AGESA_STATUS
AmdInitRecoveryInitializer (
  IN       AMD_CONFIG_PARAMS   *StdHeader,
  IN OUT   AMD_RECOVERY_PARAMS *AmdRecoveryParamsPtr
  );

AGESA_STATUS
AmdInitEarlyInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_EARLY_PARAMS  *EarlyParams
  );

AGESA_STATUS
AmdInitPostInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_POST_PARAMS *PostParamsPtr
  );

AGESA_STATUS
AmdInitPostDestructor (
  IN       AMD_CONFIG_PARAMS    *StdHeader,
  IN       AMD_POST_PARAMS      *PostParamsPtr
  );

AGESA_STATUS
AmdInitEnvInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_ENV_PARAMS *EnvParamsPtr
  );

AGESA_STATUS
AmdInitMidInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_MID_PARAMS *MidParamsPtr
  );

AGESA_STATUS
AmdInitLateInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_LATE_PARAMS *LateParamsPtr
  );

AGESA_STATUS
AmdInitLateDestructor (
  IN   AMD_CONFIG_PARAMS *StdHeader,
  IN   AMD_LATE_PARAMS   *LateParamsPtr
  );

AGESA_STATUS
AmdInitResumeInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_RESUME_PARAMS *ResumeParams
  );

AGESA_STATUS
AmdInitResumeDestructor (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_RESUME_PARAMS *ResumeParams
  );

AGESA_STATUS
AmdS3SaveInitializer (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_S3SAVE_PARAMS *S3SaveParams
  );

AGESA_STATUS
AmdS3SaveDestructor (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_S3SAVE_PARAMS *S3SaveParams
  );

AGESA_STATUS
AmdS3LateRestoreInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AMD_S3LATE_PARAMS *S3LateParams
  );

AGESA_STATUS
AmdLateRunApTaskInitializer (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   AP_EXE_PARAMS     *AmdApExeParams
  );
#endif // _CREATE_STRUCT_H_

