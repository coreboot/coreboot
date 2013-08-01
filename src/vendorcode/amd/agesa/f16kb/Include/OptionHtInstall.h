/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: Ht
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 84154 $   @e \$Date: 2012-12-12 17:02:37 -0600 (Wed, 12 Dec 2012) $
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

#ifndef _OPTION_HT_INSTALL_H_
#define _OPTION_HT_INSTALL_H_

#include "Topology.h"
#include "htFeat.h"
#include "htInterface.h"
#include "htNb.h"
#include "htTopologies.h"
/*
 * Advanced Option only, hardware socket naming is the preferred method.
 */
#ifdef BLDCFG_SYSTEM_PHYSICAL_SOCKET_MAP
  #define CFG_SYSTEM_PHYSICAL_SOCKET_MAP         (BLDCFG_SYSTEM_PHYSICAL_SOCKET_MAP)
#else
  #define CFG_SYSTEM_PHYSICAL_SOCKET_MAP         (NULL)
#endif

/*
 * OPTION_IS_RECOVERY_HT is true if Basic API is being used.
 */
#ifndef OPTION_IS_RECOVERY_HT
  #define OPTION_IS_RECOVERY_HT TRUE
#endif

/*
 * Macros will generate the correct item reference based on options
 */
#if AGESA_ENTRY_INIT_EARLY == TRUE
  // Select the interface and features
  #if ((OPTION_FAMILY15H_TN == TRUE) || (OPTION_FAMILY16H_KB == TRUE))
    #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES NULL
    #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesNone
    #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceMapsOnly
  #endif
  // Select Northbridge components
  #if OPTION_FAMILY15H == TRUE
    #if OPTION_FAMILY15H_TN == TRUE
      #define INTERNAL_HT_OPTION_FAM15TN_NB     &HtFam15Mod1xNb,
    #else
      #define INTERNAL_HT_OPTION_FAM15TN_NB
    #endif
  #else
    #define INTERNAL_HT_OPTION_FAM15TN_NB
  #endif

  #if OPTION_FAMILY16H == TRUE
    #if OPTION_FAMILY16H_KB == TRUE
      #define INTERNAL_HT_OPTION_FAM16KB_NB     &HtFam16Nb,
    #else
      #define INTERNAL_HT_OPTION_FAM16KB_NB
    #endif
  #else
    #define INTERNAL_HT_OPTION_FAM16KB_NB
  #endif

  #define INTERNAL_ONLY_NB_LIST_ITEM INTERNAL_ONLY_HT_OPTION_SUPPORTED_NBS,
  #ifndef INTERNAL_ONLY_HT_OPTION_SUPPORTED_NBS
    #undef INTERNAL_ONLY_NB_LIST_ITEM
    #define INTERNAL_ONLY_NB_LIST_ITEM
  #endif

  /* Install the correct set of northbridge implementations. Each item provides its own comma, the last item
   * is ok to have a comma because the final item (NULL) is added below.
   */
  #define INTERNAL_HT_OPTION_SUPPORTED_NBS \
                                             INTERNAL_ONLY_NB_LIST_ITEM \
                                             INTERNAL_HT_OPTION_FAM15TN_NB \
                                             INTERNAL_HT_OPTION_FAM16KB_NB




#else
  // Not Init Early
  #define INTERNAL_HT_OPTION_FEATURES     NULL
  #define INTERNAL_HT_OPTION_INTERFACE    NULL
  #define INTERNAL_HT_OPTION_SUPPORTED_NBS NULL
  #define HT_OPTIONS_PLATFORM             NULL
  #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES NULL
#endif

#ifdef AGESA_ENTRY_INIT_EARLY
  #if AGESA_ENTRY_INIT_EARLY == TRUE

    extern HT_FEATURES HtFeaturesDefault;
    extern HT_FEATURES HtFeaturesCoherentOnly;
    extern HT_FEATURES HtFeaturesNone;
    extern HT_INTERFACE HtInterfaceDefault;
    extern HT_INTERFACE HtInterfaceCoherentOnly;
    extern HT_INTERFACE HtInterfaceMapsOnly;
    extern HT_INTERFACE HtInterfaceNone;
    extern NORTHBRIDGE HtFam15Mod4xNb;
    extern NORTHBRIDGE HtFam15Mod1xNb;
    extern NORTHBRIDGE HtFam16Nb;

    CONST VOID * CONST ROMDATA HtInstalledFamilyNorthbridgeList[] = {
      INTERNAL_HT_OPTION_SUPPORTED_NBS
      NULL
    };

    STATIC CONST AMD_HT_INTERFACE ROMDATA HtOptionsPlatform =
    {
      CFG_STARTING_BUSNUM, CFG_MAXIMUM_BUSNUM, CFG_ALLOCATED_BUSNUM,
      (MANUAL_BUID_SWAP_LIST *)CFG_BUID_SWAP_LIST,
      (DEVICE_CAP_OVERRIDE *)CFG_HTDEVICE_CAPABILITIES_OVERRIDE_LIST,
      (CPU_TO_CPU_PCB_LIMITS *)CFG_HTFABRIC_LIMITS_LIST,
      (IO_PCB_LIMITS *)CFG_HTCHAIN_LIMITS_LIST,
      (OVERRIDE_BUS_NUMBERS *)CFG_BUS_NUMBERS_LIST,
      (IGNORE_LINK *)CFG_IGNORE_LINK_LIST,
      (SKIP_REGANG *)CFG_LINK_SKIP_REGANG_LIST,
      (UINT8 **)CFG_ADDITIONAL_TOPOLOGIES_LIST,
      (SYSTEM_PHYSICAL_SOCKET_MAP *)CFG_SYSTEM_PHYSICAL_SOCKET_MAP
    };
    #ifndef HT_OPTIONS_PLATFORM
      #define HT_OPTIONS_PLATFORM &HtOptionsPlatform
    #endif

    /**
     * A list of all the supported topologies.
     *
     */
    #ifndef INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES
      CONST UINT8 *CONST ROMDATA AmdTopolist[] =
      {
        amdHtTopologySingleNode,
        amdHtTopologyDualNode,
        amdHtTopologyThreeLine,
        amdHtTopologyTriangle,
        amdHtTopologyFourLine,
        amdHtTopologyFourStar,
        amdHtTopologyFourDegenerate,
        amdHtTopologyFourSquare,
        amdHtTopologyFourKite,
        amdHtTopologyFourFully,
        amdHtTopologyFiveFully,
        amdHtTopologyFiveTwistedLadder,
        amdHtTopologySixFully,
        amdHtTopologySixDoubloonLower,
        amdHtTopologySixDoubloonUpper,
        amdHtTopologySixTwistedLadder,
        amdHtTopologySevenFully,
        amdHtTopologySevenTwistedLadder,
        amdHtTopologyEightFully,
        amdHtTopologyEightDoubloon,
        amdHtTopologyEightTwistedLadder,
        amdHtTopologyEightStraightLadder,
        amdHtTopologySixTwinTriangles,
        amdHtTopologyEightTwinFullyFourWays,
        NULL
      };
      #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES AmdTopolist
    #endif

    /**
     * Declare the instance of the Ht option configuration structure
     */
    CONST OPTION_HT_CONFIGURATION ROMDATA OptionHtConfiguration = {
      OPTION_IS_RECOVERY_HT,
      CFG_SET_HTCRC_SYNC_FLOOD,
      CFG_USE_UNIT_ID_CLUMPING,
      HT_OPTIONS_PLATFORM,
      INTERNAL_HT_OPTION_INTERFACE,
      INTERNAL_HT_OPTION_FEATURES,
      &HtInstalledFamilyNorthbridgeList,
      INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES
    };

  #endif
#endif

#ifndef OPTION_HT_INIIT_RESET_ENTRY

  #define OPTION_HT_INIIT_RESET_ENTRY AmdHtInitReset
  #define OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY AmdHtResetConstructor

  #if ((OPTION_FAMILY15H_TN == TRUE) || (OPTION_FAMILY16H == TRUE))
    #undef OPTION_HT_INIIT_RESET_ENTRY
    #undef OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY
    #define OPTION_HT_INIIT_RESET_ENTRY NULL
    #define OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY NULL
  #endif

#endif

#ifdef AGESA_ENTRY_INIT_RESET
  #if AGESA_ENTRY_INIT_RESET == TRUE

    CONST AMD_HT_RESET_INTERFACE ROMDATA HtOptionResetDefaults = {
      (MANUAL_BUID_SWAP_LIST *)CFG_BUID_SWAP_LIST,
      0                                            // Unused by options
    };

    CONST OPTION_HT_INIT_RESET ROMDATA HtOptionInitReset = {
      OPTION_HT_INIIT_RESET_ENTRY,
      OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY
    };
  #endif

#endif

#endif  // _OPTION_HT_INSTALL_H_
