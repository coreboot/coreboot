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
 * @e \$Revision: 34897 $   @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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

/*  This option is designed to be included into the platform solution install
 *  file. The platform solution install file will define the options status.
 *  Check to validate the definition.
 */

#ifndef OPTION_MULTISOCKET
  #error  BLDOPT: Option not defined: "OPTION_MULTISOCKET"
#endif

/*
 * Based on user level options, set Ht internal options.
 * For now, Family 10h support will assume single module.  For multi module,
 * this will have to be changed to not set non-coherent only.
 */
#define OPTION_HT_NON_COHERENT_ONLY FALSE

#if ((OPTION_FAMILY12H == TRUE) || (OPTION_FAMILY14H == TRUE) || (OPTION_FAMILY16H == TRUE))
/* Fusion Families do not need a non-coherent only option. */
#else
  // Process Family 10h and 15h by socket, applying the MultiSocket option where it is allowable.
  #if OPTION_G34_SOCKET_SUPPORT == FALSE
  // Hydra has coherent support, other Family 10h should follow MultiSocket support.
    #if OPTION_MULTISOCKET == FALSE
      #undef OPTION_HT_NON_COHERENT_ONLY
      #define OPTION_HT_NON_COHERENT_ONLY TRUE
    #endif
  #endif
#endif

/*
 * Macros will generate the correct item reference based on options
 */
#if AGESA_ENTRY_INIT_EARLY == TRUE
  // Select the interface and features
  #if ((OPTION_FAMILY12H == TRUE) || (OPTION_FAMILY14H == TRUE) || (OPTION_FAMILY16H == TRUE))
    #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES NULL
    #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesNone
    #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceMapsOnly
  #else
    // Family 10h and 15h
    #if OPTION_HT_NON_COHERENT_ONLY == FALSE
      #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesDefault
      #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceDefault
    #else
      #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES NULL
      #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesNonCoherentOnly
      #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceNonCoherentOnly
    #endif
  #endif
  // Select Northbridge components
  #if OPTION_FAMILY10H == TRUE
    #if OPTION_HT_NON_COHERENT_ONLY == TRUE
      #define INTERNAL_HT_OPTION_FAM10_NB     &HtFam10NbNonCoherentOnly, &HtFam10RevDNbNonCoherentOnly,
    #else
      #define INTERNAL_HT_OPTION_FAM10_NB     &HtFam10NbDefault, &HtFam10RevDNbDefault,
    #endif
  #else
    #define INTERNAL_HT_OPTION_FAM10_NB
  #endif
  #if OPTION_FAMILY12H == TRUE
    #define INTERNAL_HT_OPTION_FAM12_NB     &HtFam12Nb,
  #else
    #define INTERNAL_HT_OPTION_FAM12_NB
  #endif
  #if OPTION_FAMILY14H == TRUE
    #define INTERNAL_HT_OPTION_FAM14_NB     &HtFam14Nb,
  #else
    #define INTERNAL_HT_OPTION_FAM14_NB
  #endif
  #if OPTION_FAMILY15H == TRUE
    #if OPTION_HT_NON_COHERENT_ONLY == TRUE
      #define INTERNAL_HT_OPTION_FAM15_NB     &HtFam15NbNonCoherentOnly,
    #else
      #define INTERNAL_HT_OPTION_FAM15_NB     &HtFam15NbDefault,
    #endif
  #else
    #define INTERNAL_HT_OPTION_FAM15_NB
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
                                             INTERNAL_HT_OPTION_FAM10_NB \
                                             INTERNAL_HT_OPTION_FAM15_NB \
                                             INTERNAL_HT_OPTION_FAM12_NB \
                                             INTERNAL_HT_OPTION_FAM14_NB

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
    extern HT_FEATURES HtFeaturesNonCoherentOnly;
    extern HT_FEATURES HtFeaturesNone;
    extern HT_INTERFACE HtInterfaceDefault;
    extern HT_INTERFACE HtInterfaceNonCoherentOnly;
    extern HT_INTERFACE HtInterfaceMapsOnly;
    extern HT_INTERFACE HtInterfaceNone;
    extern NORTHBRIDGE HtFam10NbDefault;
    extern NORTHBRIDGE HtFam10RevDNbDefault;
    extern NORTHBRIDGE HtFam10NbNonCoherentOnly;
    extern NORTHBRIDGE HtFam10RevDNbNonCoherentOnly;
    extern NORTHBRIDGE HtFam12Nb;
    extern NORTHBRIDGE HtFam14Nb;
    extern NORTHBRIDGE HtFam10NbNone;
    extern NORTHBRIDGE HtFam15NbDefault;
    extern NORTHBRIDGE HtFam15NbNonCoherentOnly;

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

  #if (OPTION_FAMILY12H == TRUE) || (OPTION_FAMILY14H == TRUE)
    #undef OPTION_HT_INIIT_RESET_ENTRY
    #undef OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY
    #define OPTION_HT_INIIT_RESET_ENTRY NULL
    #define OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY NULL
  #endif

  #if ((OPTION_FAMILY10H == TRUE) || (OPTION_FAMILY15H == TRUE))
    #undef OPTION_HT_INIIT_RESET_ENTRY
    #define OPTION_HT_INIIT_RESET_ENTRY AmdHtInitReset
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
