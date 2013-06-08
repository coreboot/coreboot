/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU L3 Features Initialization functions.
 *
 * Contains code that declares the AGESA CPU L3 dependent feature related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
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

#ifndef _CPU_L3_FEATURES_H_
#define _CPU_L3_FEATIRES_H_

#include "Filecode.h"
/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (L3_FEATURE_FAMILY_SERVICES);

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define AP_LATE_TASK_DISABLE_CACHE (0x00000000ul | PROC_CPU_FEATURE_CPUL3FEATURES_FILECODE)
#define AP_LATE_TASK_ENABLE_CACHE  (0x00010000ul | PROC_CPU_FEATURE_CPUL3FEATURES_FILECODE)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
#define L3_SCRUBBER_CONTEXT_ARRAY_SIZE 4

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if L3 Features are supported.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               L3 dependent features are supported
 * @retval       FALSE              L3 dependent features are not supported
 *
 */
typedef BOOLEAN F_L3_FEATURE_IS_SUPPORTED (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_IS_SUPPORTED *PF_L3_FEATURE_IS_SUPPORTED;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific hook before L3 features are initialized.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_L3_FEATURE_BEFORE_INIT (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_BEFORE_INIT *PF_L3_FEATURE_BEFORE_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to disable cache.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    HtAssistEnabled     Indicates whether Ht Assist is enabled.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_L3_FEATURE_DISABLE_CACHE (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       BOOLEAN HtAssistEnabled,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_DISABLE_CACHE *PF_L3_FEATURE_DISABLE_CACHE;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to disable cache.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @return       Family specific error value.
 *
 */
typedef VOID F_L3_FEATURE_ENABLE_CACHE (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_ENABLE_CACHE *PF_L3_FEATURE_ENABLE_CACHE;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to Initialize L3 Features
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to enable.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_L3_FEATURE_INIT (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_INIT *PF_L3_FEATURE_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific hook after L3 Features are initialized.
 *
 * @param[in]    L3FeatureServices   L3 Features family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_L3_FEATURE_AFTER_INIT (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_AFTER_INIT *PF_L3_FEATURE_AFTER_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to save the L3 scrubber.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_L3_FEATURE_GET_L3_SCRUB_CTRL (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_GET_L3_SCRUB_CTRL *PF_L3_FEATURE_GET_L3_SCRUB_CTRL;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to restore the L3 scrubber.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Contains L3 scrubber settings to restore.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_L3_FEATURE_SET_L3_SCRUB_CTRL (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_L3_FEATURE_SET_L3_SCRUB_CTRL *PF_L3_FEATURE_SET_L3_SCRUB_CTRL;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if HT Assist is supported.
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is supported.
 * @retval       FALSE              HT Assist is not supported.
 *
 */
typedef BOOLEAN F_HT_ASSIST_IS_SUPPORTED (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_IS_SUPPORTED *PF_HT_ASSIST_IS_SUPPORTED;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to Initialize HT Assist
 *
 * @param[in]    L3FeatureServices   L3 Features family services.
 * @param[in]    Socket              Processor socket to enable.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_INIT (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_INIT *PF_HT_ASSIST_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to provide non_optimal HT Assist support
 *
 * @param[in]    L3FeatureServices   L3 Feature family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @return       TRUE                The system may be running with non-optimal settings.
 * @return       FALSE               The system may is running optimally.
 *
 */
typedef BOOLEAN F_HT_ASSIST_IS_NONOPTIMAL (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_IS_NONOPTIMAL *PF_HT_ASSIST_IS_NONOPTIMAL;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if ATM Mode is supported.
 *
 * @param[in]    L3FeatureServices   L3 Features family services.
 * @param[in]    PlatformConfig      Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               ATM Mode is supported.
 * @retval       FALSE              ATM Mode is not supported.
 *
 */
typedef BOOLEAN F_ATM_MODE_IS_SUPPORTED (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_ATM_MODE_IS_SUPPORTED *PF_ATM_MODE_IS_SUPPORTED;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to Initialize ATM mode
 *
 * @param[in]    L3FeatureServices   L3 Features family services.
 * @param[in]    Socket              Processor socket to enable.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_ATM_MODE_INIT (
  IN       L3_FEATURE_FAMILY_SERVICES *L3FeatureServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_ATM_MODE_INIT *PF_ATM_MODE_INIT;

/**
 * Provide the interface to the L3 dependent features Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _L3_FEATURE_FAMILY_SERVICES {
  UINT16          Revision;                             ///< Interface version
  // Public Methods.
  PF_L3_FEATURE_IS_SUPPORTED IsL3FeatureSupported;      ///< Method: Check if L3 dependent features are supported.
  PF_L3_FEATURE_GET_L3_SCRUB_CTRL GetL3ScrubCtrl;       ///< Method: Save/disable the L3 scrubber.
  PF_L3_FEATURE_SET_L3_SCRUB_CTRL SetL3ScrubCtrl;       ///< Method: Restore the L3 scrubber.
  PF_L3_FEATURE_BEFORE_INIT HookBeforeInit;             ///< Method: Hook before enabling L3 dependent features.
  PF_L3_FEATURE_AFTER_INIT HookAfterInit;               ///< Method: Hook after enabling L3 dependent features.
  PF_L3_FEATURE_DISABLE_CACHE HookDisableCache;         ///< Method: Core hook just before disabling cache.
  PF_L3_FEATURE_ENABLE_CACHE HookEnableCache;           ///< Method: Core hook just after enabling cache.
  PF_HT_ASSIST_IS_SUPPORTED IsHtAssistSupported;        ///< Method: Check if HT Assist is supported.
  PF_HT_ASSIST_INIT HtAssistInit;                       ///< Method: Enable HT Assist.
  PF_HT_ASSIST_IS_NONOPTIMAL IsNonOptimalConfig;        ///< Method: Check if HT Assist is running optimally.
  PF_ATM_MODE_IS_SUPPORTED IsAtmModeSupported;          ///< Method: Check if ATM Mode is supported.
  PF_ATM_MODE_INIT AtmModeInit;                         ///< Method: Enable ATM Mode.
};


/*----------------------------------------------------------------------------------------
 *                          F U N C T I O N S     P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */
AGESA_STATUS
DisableAllCaches (
  IN       AP_EXE_PARAMS *ApExeParams
  );

AGESA_STATUS
EnableAllCaches (
  IN       AP_EXE_PARAMS *ApExeParams
  );

#endif  // _CPU_L3_FEATURES_H_
