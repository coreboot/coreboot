/**
 * @file
 *
 * AMD AGESA CPU HT Assist Function declarations.
 *
 * Contains code that declares the AGESA CPU Probe filter related APIs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU/Feature
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */

#ifndef _CPU_HT_ASSIST_H_
#define _CPU_HT_ASSIST_H_

#include "Filecode.h"
/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */
// Forward declaration.
typedef struct _HT_ASSIST_FAMILY_SERVICES HT_ASSIST_FAMILY_SERVICES;

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define AP_LATE_TASK_DISABLE_CACHE (0x00000000 | PROC_CPU_FEATURE_CPUHTASSIST_FILECODE)
#define AP_LATE_TASK_ENABLE_CACHE  (0x00010000 | PROC_CPU_FEATURE_CPUHTASSIST_FILECODE)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */
#define L3_SCRUBBER_CONTEXT_ARRAY_SIZE 4

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to check if HT Assist is supported.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @retval       TRUE               HT Assist is supported.
 * @retval       FALSE              HT Assist is not supported.
 *
 */
typedef BOOLEAN F_HT_ASSIST_IS_SUPPORTED (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_IS_SUPPORTED *PF_HT_ASSIST_IS_SUPPORTED;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific hook before HT Assist is initialized.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_BEFORE_INIT (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_BEFORE_INIT *PF_HT_ASSIST_BEFORE_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to disable cache.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_DISABLE_CACHE (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_DISABLE_CACHE *PF_HT_ASSIST_DISABLE_CACHE;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to disable cache.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @return       Family specific error value.
 *
 */
typedef VOID F_HT_ASSIST_ENABLE_CACHE (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_ENABLE_CACHE *PF_HT_ASSIST_ENABLE_CACHE;


/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to disable cache.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to enable.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_INIT (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_INIT *PF_HT_ASSIST_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to disable cache.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_AFTER_INIT (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_AFTER_INIT *PF_HT_ASSIST_AFTER_INIT;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to save the L3 scrubber.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Location to store current L3 scrubber settings.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_GET_L3_SCRUB_CTRL (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_GET_L3_SCRUB_CTRL *PF_HT_ASSIST_GET_L3_SCRUB_CTRL;

/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to restore the L3 scrubber.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    ScrubSettings       Contains L3 scrubber settings to restore.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 */
typedef VOID F_HT_ASSIST_SET_L3_SCRUB_CTRL (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       UINT32 ScrubSettings[L3_SCRUBBER_CONTEXT_ARRAY_SIZE],
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_SET_L3_SCRUB_CTRL *PF_HT_ASSIST_SET_L3_SCRUB_CTRL;


/*---------------------------------------------------------------------------------------*/
/**
 *  Family specific call to restore the L3 scrubber.
 *
 * @param[in]    HtAssistServices    HT Assist family services.
 * @param[in]    Socket              Processor socket to check.
 * @param[in]    StdHeader           Config Handle for library, services.
 *
 * @return       TRUE                The system may be running with non-optimal settings.
 * @return       FALSE               The system may is running optimally.
 *
 */
typedef BOOLEAN F_HT_ASSIST_IS_NONOPTIMAL (
  IN       HT_ASSIST_FAMILY_SERVICES *HtAssistServices,
  IN       UINT32 Socket,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_HT_ASSIST_IS_NONOPTIMAL *PF_HT_ASSIST_IS_NONOPTIMAL;


/**
 * Provide the interface to the HT Assist Family Specific Services.
 *
 * Use the methods or data in this struct to adapt the feature code to a specific cpu family or model (or stepping!).
 * Each supported Family must provide an implementation for all methods in this interface, even if the
 * implementation is a CommonReturn().
 */
struct _HT_ASSIST_FAMILY_SERVICES {
  UINT16          Revision;                             ///< Interface version
  // Public Methods.
  PF_HT_ASSIST_IS_SUPPORTED IsHtAssistSupported;        ///< Method: Check if HT Assist is supported.
  PF_HT_ASSIST_INIT HtAssistInit;                       ///< Method: Enable HT Assist.
  PF_HT_ASSIST_GET_L3_SCRUB_CTRL GetL3ScrubCtrl;        ///< Method: Save/disable the L3 scrubber.
  PF_HT_ASSIST_SET_L3_SCRUB_CTRL SetL3ScrubCtrl;        ///< Method: Restore the L3 scrubber.
  PF_HT_ASSIST_BEFORE_INIT HookBeforeInit;              ///< Method: Hook before enabling HT Assist.
  PF_HT_ASSIST_AFTER_INIT HookAfterInit;                ///< Method: Hook after enabling HT Assist.
  PF_HT_ASSIST_DISABLE_CACHE HookDisableCache;          ///< Method: Core hook just before disabling cache.
  PF_HT_ASSIST_ENABLE_CACHE HookEnableCache;            ///< Method: Core hook just after enabling cache.
  PF_HT_ASSIST_IS_NONOPTIMAL IsNonOptimalConfig;        ///< Method: Check if HT Assist is running optimally.
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

#endif  // _CPU_HT_ASSIST_H_
