/* $NoKeywords:$ */
/**
 * @file
 *
 * Generic CPU feature dispatcher and related services.
 *
 * Provides a feature processing engine to handle feature in a
 * more generic way.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Common
 * @e \$Revision: 49029 $   @e \$Date: 2011-03-16 09:55:06 +0800 (Wed, 16 Mar 2011) $
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
 ******************************************************************************
 */

#ifndef _CPU_FEATURES_H_
#define _CPU_FEATURES_H_

/**
 * @page cpufeatimpl CPU Generic Feature Implementation Guide
 *
 * The CPU generic feature dispatcher provides services which can be used to implement a
 * wide range of features in a manner that isolates calling code from knowledge about which
 * families or features are supported in the current build.
 *
 * @par Determine if a New Feature is a Suitable Candidate
 *
 * A feature must meet the following requirements:
 * <ul>
 * <li> Any core in the system must be able to determine if the feature should be enabled or not.
 *
 *   <ul>
 *   <li> MSRs cannot be read in multisocket systems in the 'IsEnabled' function.
 *
 *   <li> Cores cannot be launched in the 'IsEnabled' function.
 *   </ul>
 * </ul>
 *
 * @par Determine the Time Point at which the Feature Should be Enabled
 *
 * Factors to consider in making this determination:
 *
 * <ul>
 * <li> Determine if there are any dependencies on other settings that require strict ordering.
 *
 * <li> Consider the state of the APs that you will need.
 *
 * <li> Remember that features enabled during AmdInitEarly will automatically be restored on S3 resume.
 * </ul>
 *
 * @par Implementing a new feature
 *
 * Perform the following steps to implement a new feature:
 *
 * <ul>
 * <li> Create a unique equate for your time point, @b if you cannot use an existing time point.
 *
 * <li> Create a new value in the DISPATCHABLE_CPU_FEATURES enum for your feature.
 *
 * <li> Add a new 'C' file to the Features folder for your feature.
 *
 *   <ul>
 *   <li> The 'C' file must implement 2 functions -- 'IsEnabled' and 'Initialize'
 *
 *   <li> The 'C' file must instantiate a CPU_FEATURE_DESCRIPTOR structure.
 *   </ul>
 *
 * <li> Add a new 'H' file to the Features folder for your feature.
 *
 *   <ul>
 *   <li> The 'H' file declares whatever family specific functions required by the feature.
 *
 *   <li> The 'H' file declares a structure containing all family specific functions.  For a reference
 *        example, your feature API should have a set of conventions similar to cpu specific services,
 *        @ref cpuimplfss.
 *   </ul>
 *
 * <li> Create 'C' files in all applicable family folders.
 *
 *   <ul>
 *   <li> Implement the required family specific functions.
 *
 *   <li> Instantiate a family specific services structure.
 *   </ul>
 *
 * <li> Create \<feature name\>Install.h in the include folder.
 *
 *   <ul>
 *   <li> Add logic to determine when your feature should be included in the build.
 *
 *   <li> If the feature should be included, define OPTION_\<feature name\> to the address of your
 *        CPU_FEATURE_DESCRIPTOR instantiation.  If not, define OPTION_\<feature name\> to be blank.
 *
 *   <li> Create a family translation table pointing to all applicable instantiations of
 *        family specific function structures.
 *   </ul>
 *
 * <li> Modify OptionCpuFeaturesInstall.h in the include folder.
 *
 *   <ul>
 *   <li> Include \<feature name\>Install.h.
 *
 *   <li> Add OPTION_\<feature name\> to the SupportedCpuFeatureList array.
 *   </ul>
 *
 * <li> If a new time point was created, add a call to DispatchCpuFeatures at the desired location,
 *      passing your new time point equate.
 * </ul>
 *
 */


/*----------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *----------------------------------------------------------------------------------------
 */
#define CPU_FEAT_BEFORE_PM_INIT           (0x0000000000000001ull)
#define CPU_FEAT_AFTER_PM_INIT            (0x0000000000000002ull)
#define CPU_FEAT_AFTER_POST_MTRR_SYNC     (0x0000000000000004ull)
#define CPU_FEAT_INIT_MID_END             (0x0000000000000008ull)
#define CPU_FEAT_INIT_LATE_END            (0x0000000000000010ull)
#define CPU_FEAT_S3_LATE_RESTORE_END      (0x0000000000000020ull)
#define CPU_FEAT_AFTER_RESUME_MTRR_SYNC   (0x0000000000000040ull)
#define CPU_FEAT_AFTER_COHERENT_DISCOVERY (0x0000000000000080ull)
#define CPU_FEAT_BEFORE_RELINQUISH_AP     (0x0000000000000100ull)
/**
 * Enumerated list of supported features.
 */
typedef enum {
  HardwareC1e,               ///< Hardware C1e
  L3Features,                ///< L3 dependent features
  MsgBasedC1e,               ///< Message-based C1e
  SoftwareC1e,               ///< Software C1e
  CoreLeveling,              ///< Core Leveling
  C6Cstate,                  ///< C6 C-state
  IoCstate,                  ///< IO C-state
  CacheFlushOnHalt,          ///< Cache Flush On Halt
  PreserveAroundMailbox,     ///< Save-Restore the registers used for AP mailbox, to preserve their normal function.
  CoreBoost,                 ///< Core Performance Boost (CPB)
  LowPwrPstate,              ///< 500 MHz Low Power P-state
  MaxCpuFeature              ///< Not a valid value, used for verifying input
} DISPATCHABLE_CPU_FEATURES;

/*---------------------------------------------------------------------------------------*/
/**
 *  Feature specific call to check if it is supported by the system.
 *
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @retval       TRUE               Feature is supported.
 * @retval       FALSE              Feature is not supported.
 *
 */
typedef BOOLEAN F_CPU_FEATURE_IS_ENABLED (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_FEATURE_IS_ENABLED *PF_CPU_FEATURE_IS_ENABLED;

/*---------------------------------------------------------------------------------------*/
/**
 *  The feature's main entry point for enablement.
 *
 * @param[in]    EntryPoint         Timepoint designator.
 * @param[in]    PlatformConfig     Contains the runtime modifiable feature input data.
 * @param[in]    StdHeader          Config Handle for library, services.
 *
 * @return       Family specific error value.
 *
 */
typedef AGESA_STATUS F_CPU_FEATURE_INITIALIZE (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

/// Reference to a Method.
typedef F_CPU_FEATURE_INITIALIZE *PF_CPU_FEATURE_INITIALIZE;


/**
 * Generic feature descriptor
 */
typedef struct {
  DISPATCHABLE_CPU_FEATURES Feature;     ///< Enumerated feature ID
  UINT64                    EntryPoint;  ///< Timepoint designator
  PF_CPU_FEATURE_IS_ENABLED IsEnabled;   ///< Pointer to the function that checks if the feature is supported
  PF_CPU_FEATURE_INITIALIZE InitializeFeature; ///< Pointer to the function that enables the feature
} CPU_FEATURE_DESCRIPTOR;

/**
 * Table descriptor for the installed features.
 */
typedef struct {
  UINT8       NumberOfFeats;      ///< Number of valid entries in the table.
  CPU_FEATURE_DESCRIPTOR *FeatureList;       ///< Pointer to the first element in the array.
} CPU_FEATURE_TABLE;

/*----------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *----------------------------------------------------------------------------------------
 */

BOOLEAN
IsFeatureEnabled (
  IN       DISPATCHABLE_CPU_FEATURES Feature,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
DispatchCpuFeatures (
  IN       UINT64                 EntryPoint,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

BOOLEAN
IsNonCoherentHt1 (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif // _CPU_FEATURES_H_
