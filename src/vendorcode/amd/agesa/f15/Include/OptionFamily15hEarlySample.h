/* $NoKeywords:$ */
/**
 * @file
 *
 * Family 15h 'early sample' support
 *
 * This file defines the required structures for family 15h pre-production processors.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 53356 $   @e \$Date: 2011-05-18 14:14:18 -0600 (Wed, 18 May 2011) $
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
 *
 ***************************************************************************/

#ifndef _OPTION_FAMILY_15H_EARLY_SAMPLE_H_
#define _OPTION_FAMILY_15H_EARLY_SAMPLE_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/*
 * Install family 15h model 0x00 - 0x0F Early Sample support
 */

/**
 *  Early sample hook point during HTC initialization
 *
 *  @param[in,out] HtcRegister             Value of F3x64 to be written.
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_F15_OR_ES_HTC_INIT_HOOK (
  IN OUT   UINT32  *HtcRegister,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F15_OR_ES_HTC_INIT_HOOK *PF_F15_OR_ES_HTC_INIT_HOOK;

/// Hook points in the core functionality necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F15_OR_ES_HTC_INIT_HOOK     F15OrHtcInitHook;    ///< Allows for override of a certain processor register value during HTC init
} F15_OR_ES_CORE_SUPPORT;

/**
 *  Returns whether or not the processor should enable the CPB feature.
 *
 *  @param[in,out] IsEnabled               Whether or not to enable CPB
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_F15_OR_ES_IS_CPB_SUPPORTED (
  IN OUT   BOOLEAN *IsEnabled,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F15_OR_ES_IS_CPB_SUPPORTED *PF_F15_OR_ES_IS_CPB_SUPPORTED;



/// Hook points in the CPB feature necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F15_OR_ES_IS_CPB_SUPPORTED    F15OrIsCpbSupportedHook;   ///< CPB enablement override
} F15_OR_ES_CPB_SUPPORT;

/**
 *  Returns whether or not the processor should enable the C6 feature.
 *
 *  @param[in,out] IsEnabled               Whether or not to enable C6
 *  @param[in]     StdHeader               Handle of Header for calling lib functions and services.
 *
 */
typedef VOID F_F15_OR_ES_IS_C6_SUPPORTED (
  IN OUT   BOOLEAN *IsEnabled,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F15_OR_ES_IS_C6_SUPPORTED *PF_F15_OR_ES_IS_C6_SUPPORTED;



/// Hook points in the C6 feature necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F15_OR_ES_IS_C6_SUPPORTED    F15OrIsC6SupportedHook;   ///< C6 enablement override
} F15_OR_ES_C6_SUPPORT;


/**
 *  Workaround to avoid patch loading from causing NB cycles
 *
 *  @param[in,out] StdHeader     - Config handle for library and services.
 *  @param[in,out] SavedMsrValue - Saved a MSR value
 *
 */
typedef VOID F_F15_OR_ES_AVOID_NB_CYCLES_START (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT64            *SavedMsrValue
  );

/// Reference to a Method.
typedef F_F15_OR_ES_AVOID_NB_CYCLES_START *PF_F15_OR_ES_AVOID_NB_CYCLES_START;

/**
 *  Workaround to avoid patch loading from causing NB cycles
 *
 *  @param[in,out] StdHeader - Config handle for library and services.
 *  @param[in] SavedMsrValue - Saved a MSR value
 *
 *
 */
typedef VOID F_F15_OR_ES_AVOID_NB_CYCLES_END (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       UINT64            *SavedMsrValue
  );

/// Reference to a Method.
typedef F_F15_OR_ES_AVOID_NB_CYCLES_END *PF_F15_OR_ES_AVOID_NB_CYCLES_END;

/**
 *  Workaround for Ax processors after patch is loaded.
 *
 *  @param[in] StdHeader     - Config handle for library and services.
 *  @param[in] IsPatchLoaded - Is patch loaded
 *
 *
 */
typedef VOID F_F15_OR_ES_AFTER_PATCH_LOADED (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader,
  IN       BOOLEAN IsPatchLoaded
  );

/// Reference to a Method.
typedef F_F15_OR_ES_AFTER_PATCH_LOADED *PF_F15_OR_ES_AFTER_PATCH_LOADED;

/**
 *  Update the CPU microcode.
 *
 *  @param[in] StdHeader   - Config handle for library and services.
 *
 *  @retval    TRUE   - Patch Loaded Successfully.
 *  @retval    FALSE  - Patch Did Not Get Loaded.
 *
 */
typedef BOOLEAN F_F15_OR_ES_LOAD_MCU_PATCH (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F15_OR_ES_LOAD_MCU_PATCH *PF_F15_OR_ES_LOAD_MCU_PATCH;


/// Hook points in the Microcode Update feature necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F15_OR_ES_AVOID_NB_CYCLES_START  F15OrESAvoidNbCyclesStart; ///< Workaround to avoid patch loading from causing NB cycles
  PF_F15_OR_ES_AVOID_NB_CYCLES_END    F15OrESAvoidNbCyclesEnd;   ///< Workaround to avoid patch loading from causing NB cycles
  PF_F15_OR_ES_LOAD_MCU_PATCH         F15OrUpdateMcuPatchHook;   ///< Processor MCU Update override
  PF_F15_OR_ES_AFTER_PATCH_LOADED     F15OrESAfterPatchLoaded;   ///< Workaround for Ax processors after patch is loaded
} F15_OR_ES_MCU_PATCH;



typedef BOOLEAN F_F15_TN_ES_LOAD_MCU_PATCH (
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  );

/// Reference to a Method.
typedef F_F15_TN_ES_LOAD_MCU_PATCH *PF_F15_TN_ES_LOAD_MCU_PATCH;

/// Hook points in the Microcode Update feature necessary for
/// providing support for pre-production CPUs.
typedef struct {
  PF_F15_TN_ES_LOAD_MCU_PATCH    F15TnUpdateMcuPatchHook;   ///< Processor MCU Update override
} F15_TN_ES_MCU_PATCH;

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */


#endif  // _OPTION_FAMILY_15H_EARLY_SAMPLE_H_
