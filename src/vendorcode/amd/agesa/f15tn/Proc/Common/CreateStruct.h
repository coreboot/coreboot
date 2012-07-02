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

