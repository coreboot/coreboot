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
