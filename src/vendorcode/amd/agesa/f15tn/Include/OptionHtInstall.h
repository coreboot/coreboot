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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 */
/*****************************************************************************
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

#if (OPTION_FAMILY15H_TN == TRUE)
/* Families with only PCIe do not need a non-coherent only option. */
#else
  // Process Family 10h and 15h Models 00h-0Fh by socket, applying the MultiSocket option where it is allowable.
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
  #if ((OPTION_FAMILY15H_TN == TRUE))
    #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES NULL
    #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesNone
    #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceMapsOnly
  #else
    #if (FALSE)
    #else
      // Family 10h and 15h Models 00h-0Fh
      #if OPTION_HT_NON_COHERENT_ONLY == FALSE
        #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesDefault
        #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceDefault
      #else
        #define INTERNAL_HT_OPTION_BUILTIN_TOPOLOGIES NULL
        #define INTERNAL_HT_OPTION_FEATURES     &HtFeaturesNonCoherentOnly
        #define INTERNAL_HT_OPTION_INTERFACE    &HtInterfaceNonCoherentOnly
      #endif
    #endif
  #endif
  // Select Northbridge components
  #if OPTION_FAMILY10H == TRUE
    #if OPTION_HT_NON_COHERENT_ONLY == TRUE
      #define INTERNAL_HT_OPTION_FAM10_NB     &HtFam10NbNonCoherentOnly, &HtFam10RevDNbNonCoherentOnly, &HtFam10RevENbNonCoherentOnly,
    #else
      #define INTERNAL_HT_OPTION_FAM10_NB     &HtFam10NbDefault, &HtFam10RevDNbDefault, &HtFam10RevENbDefault,
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
    #if OPTION_FAMILY15H_OR == TRUE
      #if OPTION_HT_NON_COHERENT_ONLY == TRUE
        #define INTERNAL_HT_OPTION_FAM15_NB     &HtFam15NbNonCoherentOnly,
      #else
        #define INTERNAL_HT_OPTION_FAM15_NB     &HtFam15NbDefault,
      #endif
    #else
      #define INTERNAL_HT_OPTION_FAM15_NB
    #endif
    #if OPTION_FAMILY15H_TN == TRUE
      #define INTERNAL_HT_OPTION_FAM15TN_NB     &HtFam15Mod1xNb,
    #else
      #define INTERNAL_HT_OPTION_FAM15TN_NB
    #endif
//    #if OPTION_FAMILY15H_KM == TRUE
//      #define INTERNAL_HT_OPTION_FAM15KM_NB     &HtFam15Mod2xNbDefault,
//    #else
//      #define INTERNAL_HT_OPTION_FAM15KM_NB
//    #endif
//    #if OPTION_FAMILY15H_KV == TRUE
//      #define INTERNAL_HT_OPTION_FAM15KV_NB     &HtFam15Mod1xNb,
//    #else
//      #define INTERNAL_HT_OPTION_FAM15KV_NB
//    #endif
  #else
    #define INTERNAL_HT_OPTION_FAM15_NB
    #define INTERNAL_HT_OPTION_FAM15TN_NB
    //#define INTERNAL_HT_OPTION_FAM15KM_NB
    //#define INTERNAL_HT_OPTION_FAM15KV_NB
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
                                             INTERNAL_HT_OPTION_FAM14_NB \
                                             INTERNAL_HT_OPTION_FAM15TN_NB

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
    extern HT_FEATURES HtFeaturesCoherentOnly;
    extern HT_FEATURES HtFeaturesNone;
    extern HT_INTERFACE HtInterfaceDefault;
    extern HT_INTERFACE HtInterfaceNonCoherentOnly;
    extern HT_INTERFACE HtInterfaceCoherentOnly;
    extern HT_INTERFACE HtInterfaceMapsOnly;
    extern HT_INTERFACE HtInterfaceNone;
    extern NORTHBRIDGE HtFam10NbDefault;
    extern NORTHBRIDGE HtFam10RevDNbDefault;
    extern NORTHBRIDGE HtFam10NbNonCoherentOnly;
    extern NORTHBRIDGE HtFam10RevDNbNonCoherentOnly;
    extern NORTHBRIDGE HtFam10RevENbDefault;
    extern NORTHBRIDGE HtFam10RevENbNonCoherentOnly;
    extern NORTHBRIDGE HtFam12Nb;
    extern NORTHBRIDGE HtFam14Nb;
    extern NORTHBRIDGE HtFam10NbNone;
    extern NORTHBRIDGE HtFam15NbDefault;
    extern NORTHBRIDGE HtFam15NbNonCoherentOnly;
    extern NORTHBRIDGE HtFam15Mod1xNb;

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

  #if ((OPTION_FAMILY15H_TN == TRUE)  )
    #undef OPTION_HT_INIIT_RESET_ENTRY
    #undef OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY
    #define OPTION_HT_INIIT_RESET_ENTRY NULL
    #define OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY NULL
  #endif

  #if ((OPTION_FAMILY10H == TRUE) || (OPTION_FAMILY15H_OR == TRUE))
    #undef OPTION_HT_INIIT_RESET_ENTRY
    #undef OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY
    #define OPTION_HT_INIIT_RESET_ENTRY AmdHtInitReset
    #define OPTION_HT_INIIT_RESET_CONSTRUCTOR_ENTRY AmdHtResetConstructor
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
