/**
 * @file
 *
 * Install of build options for a Lynx platform solution
 *
 * This file generates the defaults tables for the "Lynx" platform solution
 * set of processors. The documented build options are imported from a user
 * controlled file for processing.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
 * @e \$Revision: 6049 $   @e \$Date: 2008-05-14 01:58:02 -0500 (Wed, 14 May 2008) $
 */
/*****************************************************************************
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
 *
 ***************************************************************************/

#include "cpuRegisters.h"
#include "cpuFamRegisters.h"
#include "cpuFamilyTranslation.h"
#include "AdvancedApi.h"
#include "heapManager.h"
#include "CreateStruct.h"
#include "cpuFeatures.h"
#include "Table.h"
#include "CommonReturns.h"
#include "cpuEarlyInit.h"
#include "cpuLateInit.h"

/*****************************************************************************
 *   Define the RELEASE VERSION string
 *
 * The Release Version string should identify the next planned release.
 * When a branch is made in preparation for a release, the release manager
 * should change/confirm that the branch version of this file contains the
 * string matching the desired version for the release. The trunk version of
 * the file should always contain a trailing 'X'. This will make sure that a
 * development build from trunk will not be confused for a released version.
 * The release manager will need to remove the trailing 'X' and update the
 * version string as appropriate for the release. The trunk copy of this file
 * should also be updated/incremented for the next expected version, + trailing 'X'
 ****************************************************************************/
                  // This is the delivery package title, "DanNiPI "
                  // This string MUST be exactly 8 characters long
#define AGESA_PACKAGE_STRING  {'L', 'y', 'n', 'x', 'P', 'I', ' ', ' '}

                  // This is the release version number of the AGESA component
                  // This string MUST be exactly 12 characters long
#define AGESA_VERSION_STRING  {'V', '0', '.', '0', '.', '1', '.', '0', 'X', ' ', ' ', ' '}

//
//Support multiple dll's per image for legacy BIOS only.
//Platform BIOS may need to extern AmdAgesaDispatcher, if ModuleDispatcher is not NULL.
//
//extern AGESA_STATUS CALLCONV AmdAgesaDispatcher( VOID *);

VOLATILE  AMD_MODULE_HEADER mCpuModuleID = {
  //ModuleHeaderSignature
  // Remove 'DOM$' as temp solution before update BinUtil.exe ,
  '0000',
  //ModuleIdentifier[8]
  AGESA_ID,
  //ModuleVersion[12]
  AGESA_VERSION_STRING,
  //ModuleDispatcher
  NULL,//(VOID *)(UINT64)((MODULE_ENTRY)AmdAgesaDispatcher),
  //NextBlock
  NULL
};

/*****************************************************************************
 *
 * Start processing the user options:  First, set default settings
 *
 ****************************************************************************/

/* Available options for image builds.
 *
 * As part of the image build for each image, define the options below to select the
 * AGESA entry points included in that image.  Turn these on in your option c file, not
 * here.
 */
// #define AGESA_ENTRY_INIT_RESET                    TRUE
// #define AGESA_ENTRY_INIT_RECOVERY                 TRUE
// #define AGESA_ENTRY_INIT_EARLY                    TRUE
// #define AGESA_ENTRY_INIT_POST                     TRUE
// #define AGESA_ENTRY_INIT_ENV                      TRUE
// #define AGESA_ENTRY_INIT_MID                      TRUE
// #define AGESA_ENTRY_INIT_LATE                     TRUE
// #define AGESA_ENTRY_INIT_S3SAVE                   TRUE
// #define AGESA_ENTRY_INIT_RESUME                   TRUE
// #define AGESA_ENTRY_INIT_LATE_RESTORE             TRUE
// #define AGESA_ENTRY_INIT_GENERAL_SERVICES         TRUE

/*  Defaults for private/internal build control settings  */
/* Available options for image builds.
 *
 * As part of the image build for each image, define the options below to select the
 * AGESA entry points included in that image.
 */


#ifndef AGESA_ENTRY_INIT_RESET
  #define AGESA_ENTRY_INIT_RESET                     FALSE
#endif

#ifndef AGESA_ENTRY_INIT_RECOVERY
  #define AGESA_ENTRY_INIT_RECOVERY                  FALSE
#endif

#ifndef AGESA_ENTRY_INIT_EARLY
  #define AGESA_ENTRY_INIT_EARLY                     FALSE
#endif

#ifndef AGESA_ENTRY_INIT_POST
  #define AGESA_ENTRY_INIT_POST                      FALSE
#endif

#ifndef AGESA_ENTRY_INIT_ENV
  #define AGESA_ENTRY_INIT_ENV                       FALSE
#endif

#ifndef AGESA_ENTRY_INIT_MID
  #define AGESA_ENTRY_INIT_MID                       FALSE
#endif

#ifndef AGESA_ENTRY_INIT_LATE
  #define AGESA_ENTRY_INIT_LATE                      FALSE
#endif

#ifndef AGESA_ENTRY_INIT_S3SAVE
  #define AGESA_ENTRY_INIT_S3SAVE                    FALSE
#endif

#ifndef AGESA_ENTRY_INIT_RESUME
  #define AGESA_ENTRY_INIT_RESUME                    FALSE
#endif

#ifndef AGESA_ENTRY_INIT_LATE_RESTORE
  #define AGESA_ENTRY_INIT_LATE_RESTORE              FALSE
#endif

#ifndef AGESA_ENTRY_INIT_GENERAL_SERVICES
  #define AGESA_ENTRY_INIT_GENERAL_SERVICES          FALSE
#endif

// Add logic for determining if the AP callout is required
// by checking enabled entry points / enabled features that
// may need them.  Leave the ifndef here in case the logic
// does not define this equate.
#ifndef AGESA_ENTRY_LATE_RUN_AP_TASK
  #define AGESA_ENTRY_LATE_RUN_AP_TASK               FALSE
#endif

#define OPTION_FAMILY12H                          TRUE
#define OPTION_FAMILY12H_LN                       TRUE
#define OPTION_MEMCTLR_LN                         TRUE

#ifdef BLDCFG_AMD_PLATFORM_TYPE
  #define CFG_AMD_PLATFORM_TYPE              BLDCFG_AMD_PLATFORM_TYPE
#else
  #define CFG_AMD_PLATFORM_TYPE              0
#endif

CONST UINT32 ROMDATA AmdPlatformTypeCgf = CFG_AMD_PLATFORM_TYPE;

#define OPTION_HW_WRITE_LEV_TRAINING            TRUE
#define OPTION_SW_WRITE_LEV_TRAINING            FALSE
#define OPTION_CONTINOUS_PATTERN_GENERATION     TRUE
#define OPTION_HW_DQS_REC_EN_TRAINING           TRUE
#define OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING   FALSE
#define OPTION_OPT_SW_DQS_REC_EN_TRAINING       FALSE
#define OPTION_NON_OPT_SW_RD_WR_POS_TRAINING    FALSE
#define OPTION_OPT_SW_RD_WR_POS_TRAINING        TRUE
#define OPTION_MAX_RD_LAT_TRAINING              TRUE
#define OPTION_HW_DRAM_INIT                     FALSE
#define OPTION_SW_DRAM_INIT                     TRUE
#define OPTION_S3_MEM_SUPPORT                   TRUE
#define OPTION_ADDR_TO_CS_TRANSLATOR            FALSE

/*  Defaults for public user options  */
#define OPTION_MULTISOCKET              FALSE
#define OPTION_UDIMMS                   TRUE
#define OPTION_RDIMMS                   FALSE
#define OPTION_SODIMMS                  TRUE
#define OPTION_ECC                      FALSE
#define OPTION_BANK_INTERLEAVE          TRUE
#define OPTION_DCT_INTERLEAVE           TRUE
#define OPTION_NODE_INTERLEAVE          FALSE
#define OPTION_PARALLEL_TRAINING        FALSE
#define OPTION_ONLINE_SPARE             FALSE
#define OPTION_MEM_RESTORE              TRUE
#define OPTION_DIMM_EXCLUDE             TRUE
#define OPTION_ACPI_PSTATES             TRUE
#define OPTION_SRAT                     FALSE
#define OPTION_SLIT                     FALSE
#define OPTION_WHEA                     TRUE
#define OPTION_DMI                      TRUE
#define OPTION_DDR2                     FALSE
#define OPTION_DDR3                     TRUE
#define OPTION_EARLY_SAMPLES            FALSE
#define OPTION_HT_ASSIST                FALSE
#define OPTION_CPU_CORELEVLING          FALSE
#define OPTION_MSG_BASED_C1E            FALSE
#define OPTION_C6_STATE                 TRUE
#define OPTION_CPU_CFOH                 FALSE
#define OPTION_S3SCRIPT                 TRUE
#define OPTION_GFX_RECOVERY             TRUE
#define CFG_ACPI_PSTATES_PPC            TRUE
#define CFG_ACPI_PSTATES_PCT            TRUE
#define CFG_ACPI_PSTATES_PSD            TRUE
#define CFG_ACPI_PSTATES_PSS            TRUE
#define CFG_ACPI_PSTATES_XPSS           TRUE
#define CFG_ACPI_PSTATE_PSD_INDPX       FALSE
#define CFG_VRM_HIGH_SPEED_ENABLE       FALSE
/*---------------------------------------------------------------------------
 *       Processing the options:  Second, process the user's selections
 *--------------------------------------------------------------------------*/
#ifdef BLDOPT_REMOVE_MULTISOCKET_SUPPORT
  #if  BLDOPT_REMOVE_MULTISOCKET_SUPPORT == TRUE
    #undef  OPTION_MULTISOCKET
    #define OPTION_MULTISOCKET          FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_ECC_SUPPORT
  #if  BLDOPT_REMOVE_ECC_SUPPORT == TRUE
    #undef  OPTION_ECC
    #define OPTION_ECC        FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_UDIMMS_SUPPORT
  #if  BLDOPT_REMOVE_UDIMMS_SUPPORT == TRUE
    #undef  OPTION_UDIMMS
    #define OPTION_UDIMMS               FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_RDIMMS_SUPPORT
  #if  BLDOPT_REMOVE_RDIMMS_SUPPORT == TRUE
    #undef  OPTION_RDIMMS
    #define OPTION_RDIMMS               FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_SODIMMS_SUPPORT
  #if  BLDOPT_REMOVE_SODIMMS_SUPPORT == TRUE
    #undef  OPTION_SODIMMS
    #define OPTION_SODIMMS               FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_BANK_INTERLEAVE
  #if  BLDOPT_REMOVE_BANK_INTERLEAVE == TRUE
    #undef  OPTION_BANK_INTERLEAVE
    #define OPTION_BANK_INTERLEAVE      FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_DCT_INTERLEAVE
  #if  BLDOPT_REMOVE_DCT_INTERLEAVE == TRUE
    #undef  OPTION_DCT_INTERLEAVE
    #define OPTION_DCT_INTERLEAVE       FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_NODE_INTERLEAVE
  #if  BLDOPT_REMOVE_NODE_INTERLEAVE == TRUE
    #undef  OPTION_NODE_INTERLEAVE
    #define OPTION_NODE_INTERLEAVE      FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_PARALLEL_TRAINING
  #if  BLDOPT_REMOVE_PARALLEL_TRAINING == TRUE
    #undef  OPTION_PARALLEL_TRAINING
    #define OPTION_PARALLEL_TRAINING    FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT
  #if  BLDOPT_REMOVE_ONLINE_SPARE_SUPPORT == TRUE
    #undef  OPTION_ONLINE_SPARE
    #define OPTION_ONLINE_SPARE         FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_MEM_RESTORE_SUPPORT
  #if  BLDOPT_REMOVE_MEM_RESTORE_SUPPORT == TRUE
    #undef  OPTION_MEM_RESTORE
    #define OPTION_MEM_RESTORE         FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_ACPI_PSTATES
  #if  BLDOPT_REMOVE_ACPI_PSTATES == TRUE
    #undef  OPTION_ACPI_PSTATES
    #define OPTION_ACPI_PSTATES         FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_SRAT
  #if  BLDOPT_REMOVE_SRAT == TRUE
    #undef  OPTION_SRAT
    #define OPTION_SRAT                 FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_SLIT
  #if  BLDOPT_REMOVE_SLIT == TRUE
    #undef  OPTION_SLIT
    #define OPTION_SLIT                 FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_WHEA
  #if  BLDOPT_REMOVE_WHEA == TRUE
    #undef  OPTION_WHEA
    #define OPTION_WHEA                 FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_DMI
  #if  BLDOPT_REMOVE_DMI == TRUE
    #undef  OPTION_DMI
    #define OPTION_DMI                  FALSE
  #endif
#endif
#ifdef BLDOPT_REMOVE_ADDR_TO_CS_TRANSLATOR
  #if  BLDOPT_REMOVE_ADDR_TO_CS_TRANSLATOR == TRUE
    #undef  OPTION_ADDR_TO_CS_TRANSLATOR
    #define OPTION_ADDR_TO_CS_TRANSLATOR        FALSE
  #endif
#endif

#ifdef BLDOPT_REMOVE_HT_ASSIST
  #if  BLDOPT_REMOVE_HT_ASSIST == TRUE
    #undef  OPTION_HT_ASSIST
    #define OPTION_HT_ASSIST      FALSE
  #endif
#endif

#ifdef BLDOPT_REMOVE_MSG_BASED_C1E
  #if  BLDOPT_REMOVE_MSG_BASED_C1E == TRUE
    #undef  OPTION_MSG_BASED_C1E
    #define OPTION_MSG_BASED_C1E      FALSE
  #endif
#endif

#ifdef BLDOPT_REMOVE_C6_STATE
  #if  BLDOPT_REMOVE_C6_STATE == TRUE
    #undef  OPTION_C6_STATE
    #define OPTION_C6_STATE      FALSE
  #endif
#endif

#ifdef BLDOPT_REMOVE_GFX_RECOVERY
  #if  BLDOPT_REMOVE_GFX_RECOVERY == TRUE
    #undef  OPTION_GFX_RECOVERY
    #define OPTION_GFX_RECOVERY         FALSE
  #endif
#endif

#ifdef BLDCFG_REMOVE_ACPI_PSTATES_PPC
  #if  BLDCFG_REMOVE_ACPI_PSTATES_PPC == TRUE
    #undef  CFG_ACPI_PSTATES_PPC
    #define CFG_ACPI_PSTATES_PPC        FALSE
  #endif
#endif

#ifdef BLDCFG_REMOVE_ACPI_PSTATES_PCT
  #if  BLDCFG_REMOVE_ACPI_PSTATES_PCT == TRUE
    #undef  CFG_ACPI_PSTATES_PCT
    #define CFG_ACPI_PSTATES_PCT        FALSE
  #endif
#endif

#ifdef BLDCFG_REMOVE_ACPI_PSTATES_PSD
  #if  BLDCFG_REMOVE_ACPI_PSTATES_PSD == TRUE
    #undef  CFG_ACPI_PSTATES_PSD
    #define CFG_ACPI_PSTATES_PSD        FALSE
  #endif
#endif

#ifdef BLDCFG_REMOVE_ACPI_PSTATES_PSS
  #if  BLDCFG_REMOVE_ACPI_PSTATES_PSS == TRUE
    #undef  CFG_ACPI_PSTATES_PSS
    #define CFG_ACPI_PSTATES_PSS        FALSE
  #endif
#endif

#ifdef BLDCFG_REMOVE_ACPI_PSTATES_XPSS
  #if  BLDCFG_REMOVE_ACPI_PSTATES_XPSS == TRUE
    #undef  CFG_ACPI_PSTATES_XPSS
    #define CFG_ACPI_PSTATES_XPSS       FALSE
  #endif
#endif

#ifdef BLDCFG_FORCE_INDEPENDENT_PSD_OBJECT
  #if  BLDCFG_FORCE_INDEPENDENT_PSD_OBJECT == TRUE
    #undef  CFG_ACPI_PSTATE_PSD_INDPX
    #define CFG_ACPI_PSTATE_PSD_INDPX       TRUE
  #endif
#endif

#ifdef BLDCFG_VRM_HIGH_SPEED_ENABLE
  #if  BLDCFG_VRM_HIGH_SPEED_ENABLE == TRUE
    #undef  CFG_VRM_HIGH_SPEED_ENABLE
    #define CFG_VRM_HIGH_SPEED_ENABLE       TRUE
  #endif
#endif

#ifdef BLDCFG_VRM_CURRENT_LIMIT
  #define CFG_VRM_CURRENT_LIMIT            BLDCFG_VRM_CURRENT_LIMIT
#else
  #define CFG_VRM_CURRENT_LIMIT            0
#endif

#ifdef BLDCFG_VRM_LOW_POWER_THRESHOLD
  #define CFG_VRM_LOW_POWER_THRESHOLD      BLDCFG_VRM_LOW_POWER_THRESHOLD
#else
  #define CFG_VRM_LOW_POWER_THRESHOLD      0
#endif

#ifdef BLDCFG_VRM_SLEW_RATE
  #define CFG_VRM_SLEW_RATE                BLDCFG_VRM_SLEW_RATE
#else
  #define CFG_VRM_SLEW_RATE                5000
#endif

#ifdef BLDCFG_VRM_INRUSH_CURRENT_LIMIT
  #define CFG_VRM_INRUSH_CURRENT_LIMIT     BLDCFG_VRM_INRUSH_CURRENT_LIMIT
#else
  #define CFG_VRM_INRUSH_CURRENT_LIMIT     0
#endif

#ifdef BLDCFG_PLAT_NUM_IO_APICS
  #define CFG_PLAT_NUM_IO_APICS            BLDCFG_PLAT_NUM_IO_APICS
#else
  #define CFG_PLAT_NUM_IO_APICS            0
#endif

#ifdef BLDCFG_MEM_INIT_PSTATE
  #define CFG_MEM_INIT_PSTATE              BLDCFG_MEM_INIT_PSTATE
#else
  #define CFG_MEM_INIT_PSTATE              0
#endif

#ifdef BLDCFG_PLATFORM_C1E_MODE
  #define CFG_C1E_MODE                        BLDCFG_PLATFORM_C1E_MODE
#else
  #define CFG_C1E_MODE                        C1eModeDisabled
#endif

#ifdef BLDCFG_PLATFORM_C1E_OPDATA
  #define CFG_C1E_OPDATA                      BLDCFG_PLATFORM_C1E_OPDATA
#else
  #define CFG_C1E_OPDATA                      0
#endif

#ifdef BLDCFG_PLATFORM_CSTATE_MODE
  #define CFG_CSTATE_MODE                     BLDCFG_PLATFORM_CSTATE_MODE
#else
  #define CFG_CSTATE_MODE                     CStateModeDisabled
#endif

#ifdef BLDCFG_PLATFORM_CSTATE_OPDATA
  #define CFG_CSTATE_OPDATA                   BLDCFG_PLATFORM_CSTATE_OPDATA
#else
  #define CFG_CSTATE_OPDATA                   0
#endif

#ifdef BLDCFG_CORE_LEVELING_MODE
  #define CFG_CORE_LEVELING_MODE           BLDCFG_CORE_LEVELING_MODE
#else
  #define CFG_CORE_LEVELING_MODE           0
#endif

#ifdef BLDCFG_AMD_PSTATE_CAP_VALUE
  #define CFG_AMD_PSTATE_CAP_VALUE           BLDCFG_AMD_PSTATE_CAP_VALUE
#else
  #define CFG_AMD_PSTATE_CAP_VALUE           0
#endif

#ifdef BLDCFG_HEAP_DRAM_ADDRESS
  #define CFG_HEAP_DRAM_ADDRESS                 BLDCFG_HEAP_DRAM_ADDRESS
#else
  #define CFG_HEAP_DRAM_ADDRESS                 AMD_HEAP_RAM_ADDRESS
#endif

#ifdef BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT
  #define CFG_MEMORY_BUS_FREQUENCY_LIMIT        BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT
#else
  #define CFG_MEMORY_BUS_FREQUENCY_LIMIT        DDR800_FREQUENCY
#endif

#ifdef BLDCFG_MEMORY_MODE_UNGANGED
  #define CFG_MEMORY_MODE_UNGANGED              BLDCFG_MEMORY_MODE_UNGANGED
#else
  #define CFG_MEMORY_MODE_UNGANGED              TRUE
#endif

#ifdef BLDCFG_MEMORY_QUAD_RANK_CAPABLE
  #define CFG_MEMORY_QUAD_RANK_CAPABLE          BLDCFG_MEMORY_QUAD_RANK_CAPABLE
#else
  #define CFG_MEMORY_QUAD_RANK_CAPABLE          TRUE
#endif

#ifdef BLDCFG_MEMORY_QUADRANK_TYPE
  #define CFG_MEMORY_QUADRANK_TYPE              BLDCFG_MEMORY_QUADRANK_TYPE
#else
  #define CFG_MEMORY_QUADRANK_TYPE              QUADRANK_REGISTERED
#endif

#ifdef BLDCFG_MEMORY_RDIMM_CAPABLE
  #define CFG_MEMORY_RDIMM_CAPABLE              BLDCFG_MEMORY_RDIMM_CAPABLE
#else
  #define CFG_MEMORY_RDIMM_CAPABLE              TRUE
#endif

#ifdef BLDCFG_MEMORY_UDIMM_CAPABLE
  #define CFG_MEMORY_UDIMM_CAPABLE              BLDCFG_MEMORY_UDIMM_CAPABLE
#else
  #define CFG_MEMORY_UDIMM_CAPABLE              TRUE
#endif

#ifdef BLDCFG_MEMORY_SODIMM_CAPABLE
  #define CFG_MEMORY_SODIMM_CAPABLE             BLDCFG_MEMORY_SODIMM_CAPABLE
#else
  #define CFG_MEMORY_SODIMM_CAPABLE             FALSE
#endif

#ifdef BLDCFG_LIMIT_MEMORY_TO_BELOW_1TB
  #define CFG_LIMIT_MEMORY_TO_BELOW_1TB         BLDCFG_LIMIT_MEMORY_TO_BELOW_1TB
#else
  #define CFG_LIMIT_MEMORY_TO_BELOW_1TB         TRUE
#endif

#ifdef BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING
  #define CFG_MEMORY_ENABLE_BANK_INTERLEAVING   BLDCFG_MEMORY_ENABLE_BANK_INTERLEAVING
#else
  #define CFG_MEMORY_ENABLE_BANK_INTERLEAVING   TRUE
#endif

#ifdef BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING
  #define CFG_MEMORY_ENABLE_NODE_INTERLEAVING   BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING
#else
  #define CFG_MEMORY_ENABLE_NODE_INTERLEAVING   FALSE
#endif

#ifdef BLDCFG_MEMORY_CHANNEL_INTERLEAVING
  #define CFG_MEMORY_CHANNEL_INTERLEAVING       BLDCFG_MEMORY_CHANNEL_INTERLEAVING
#else
  #define CFG_MEMORY_CHANNEL_INTERLEAVING       TRUE
#endif

#ifdef BLDCFG_MEMORY_POWER_DOWN
  #define CFG_MEMORY_POWER_DOWN                 BLDCFG_MEMORY_POWER_DOWN
#else
  #define CFG_MEMORY_POWER_DOWN                 FALSE
#endif

#ifdef BLDCFG_POWER_DOWN_MODE
  #define CFG_POWER_DOWN_MODE                   BLDCFG_POWER_DOWN_MODE
#else
  #define CFG_POWER_DOWN_MODE                   POWER_DOWN_BY_CHANNEL
#endif

#ifdef BLDCFG_ONLINE_SPARE
  #define CFG_ONLINE_SPARE                      BLDCFG_ONLINE_SPARE
#else
  #define CFG_ONLINE_SPARE                      FALSE
#endif

#ifdef BLDCFG_MEMORY_PARITY_ENABLE
  #define CFG_MEMORY_PARITY_ENABLE              BLDCFG_MEMORY_PARITY_ENABLE
#else
  #define CFG_MEMORY_PARITY_ENABLE              FALSE
#endif

#ifdef BLDCFG_BANK_SWIZZLE
  #define CFG_BANK_SWIZZLE                      BLDCFG_BANK_SWIZZLE
#else
  #define CFG_BANK_SWIZZLE                      TRUE
#endif

#ifdef BLDCFG_TIMING_MODE_SELECT
  #define CFG_TIMING_MODE_SELECT                BLDCFG_TIMING_MODE_SELECT
#else
  #define CFG_TIMING_MODE_SELECT                TIMING_MODE_AUTO
#endif

#ifdef BLDCFG_MEMORY_CLOCK_SELECT
  #define CFG_MEMORY_CLOCK_SELECT               BLDCFG_MEMORY_CLOCK_SELECT
#else
  #define CFG_MEMORY_CLOCK_SELECT               DDR800_FREQUENCY
#endif

#ifdef BLDCFG_DQS_TRAINING_CONTROL
  #define CFG_DQS_TRAINING_CONTROL              BLDCFG_DQS_TRAINING_CONTROL
#else
  #define CFG_DQS_TRAINING_CONTROL              TRUE
#endif

#ifdef BLDCFG_IGNORE_SPD_CHECKSUM
  #define CFG_IGNORE_SPD_CHECKSUM               BLDCFG_IGNORE_SPD_CHECKSUM
#else
  #define CFG_IGNORE_SPD_CHECKSUM               FALSE
#endif

#ifdef BLDCFG_USE_BURST_MODE
  #define CFG_USE_BURST_MODE                    BLDCFG_USE_BURST_MODE
#else
  #define CFG_USE_BURST_MODE                    FALSE
#endif

#ifdef BLDCFG_MEMORY_ALL_CLOCKS_ON
  #define CFG_MEMORY_ALL_CLOCKS_ON              BLDCFG_MEMORY_ALL_CLOCKS_ON
#else
  #define CFG_MEMORY_ALL_CLOCKS_ON              FALSE
#endif

#ifdef BLDCFG_ENABLE_ECC_FEATURE
  #define CFG_ENABLE_ECC_FEATURE                BLDCFG_ENABLE_ECC_FEATURE
#else
  #define CFG_ENABLE_ECC_FEATURE                TRUE
#endif

#ifdef BLDCFG_ECC_REDIRECTION
  #define CFG_ECC_REDIRECTION                   BLDCFG_ECC_REDIRECTION
#else
  #define CFG_ECC_REDIRECTION                   FALSE
#endif

#ifdef BLDCFG_SCRUB_DRAM_RATE
  #define CFG_SCRUB_DRAM_RATE         BLDCFG_SCRUB_DRAM_RATE
#else
  #define CFG_SCRUB_DRAM_RATE         0
#endif

#ifdef BLDCFG_SCRUB_L2_RATE
  #define CFG_SCRUB_L2_RATE           BLDCFG_SCRUB_L2_RATE
#else
  #define CFG_SCRUB_L2_RATE           0
#endif

#ifdef BLDCFG_SCRUB_L3_RATE
  #define CFG_SCRUB_L3_RATE           BLDCFG_SCRUB_L3_RATE
#else
  #define CFG_SCRUB_L3_RATE           0
#endif

#ifdef BLDCFG_SCRUB_IC_RATE
  #define CFG_SCRUB_IC_RATE           BLDCFG_SCRUB_IC_RATE
#else
  #define CFG_SCRUB_IC_RATE           0
#endif

#ifdef BLDCFG_SCRUB_DC_RATE
  #define CFG_SCRUB_DC_RATE           BLDCFG_SCRUB_DC_RATE
#else
  #define CFG_SCRUB_DC_RATE           0
#endif

#ifdef BLDCFG_ECC_SYNC_FLOOD
  #define CFG_ECC_SYNC_FLOOD          BLDCFG_ECC_SYNC_FLOOD
#else
  #define CFG_ECC_SYNC_FLOOD          0
#endif

#ifdef BLDCFG_ECC_SYMBOL_SIZE
  #define CFG_ECC_SYMBOL_SIZE         BLDCFG_ECC_SYMBOL_SIZE
#else
  #define CFG_ECC_SYMBOL_SIZE         0
#endif

#ifdef BLDCFG_1GB_ALIGN
  #define CFG_1GB_ALIGN               BLDCFG_1GB_ALIGN
#else
  #define CFG_1GB_ALIGN               FALSE
#endif

#ifdef BLDCFG_UMA_ALLOCATION_MODE
  #define CFG_UMA_MODE                BLDCFG_UMA_ALLOCATION_MODE
#else
  #define CFG_UMA_MODE                UMA_AUTO
#endif

#ifdef BLDCFG_UMA_ALLOCATION_SIZE
  #define CFG_UMA_SIZE                BLDCFG_UMA_ALLOCATION_SIZE
#else
  #define CFG_UMA_SIZE                0
#endif

#ifdef BLDCFG_UMA_ABOVE4G_SUPPORT
  #define CFG_UMA_ABOVE4G             BLDCFG_UMA_ABOVE4G_SUPPORT
#else
  #define CFG_UMA_ABOVE4G             FALSE
#endif

#ifdef BLDCFG_UMA_ALIGNMENT
  #define CFG_UMA_ALIGNMENT           BLDCFG_UMA_ALIGNMENT
#else
  #define CFG_UMA_ALIGNMENT           NO_UMA_ALIGNED
#endif

#ifdef BLDCFG_PROCESSOR_SCOPE_IN_SB
  #define CFG_PROCESSOR_SCOPE_IN_SB   BLDCFG_PROCESSOR_SCOPE_IN_SB
#else
  #define CFG_PROCESSOR_SCOPE_IN_SB   FALSE
#endif

#ifdef BLDCFG_S3_LATE_RESTORE
  #define CFG_S3_LATE_RESTORE         BLDCFG_S3_LATE_RESTORE
#else
  #define CFG_S3_LATE_RESTORE         TRUE
#endif

#ifdef BLDCFG_USE_32_BYTE_REFRESH
  #define CFG_USE_32_BYTE_REFRESH  (BLDCFG_USE_32_BYTE_REFRESH)
#else
  #define CFG_USE_32_BYTE_REFRESH  (FALSE)
#endif

#ifdef BLDCFG_USE_VARIABLE_MCT_ISOC_PRIORITY
  #define CFG_USE_VARIABLE_MCT_ISOC_PRIORITY  (BLDCFG_USE_VARIABLE_MCT_ISOC_PRIORITY)
#else
  #define CFG_USE_VARIABLE_MCT_ISOC_PRIORITY  (FALSE)
#endif

#ifdef BLDCFG_PROCESSOR_SCOPE_NAME0
  #define CFG_PROCESSOR_SCOPE_NAME0          BLDCFG_PROCESSOR_SCOPE_NAME0
#else
  #define CFG_PROCESSOR_SCOPE_NAME0          SCOPE_NAME_VALUE
#endif

#ifdef BLDCFG_PROCESSOR_SCOPE_NAME1
  #define CFG_PROCESSOR_SCOPE_NAME1          BLDCFG_PROCESSOR_SCOPE_NAME1
#else
  #define CFG_PROCESSOR_SCOPE_NAME1          SCOPE_NAME_VALUE1
#endif

#ifdef BLDCFG_CFG_GNB_HD_AUDIO
  #define CFG_GNB_HD_AUDIO                   BLDCFG_CFG_GNB_HD_AUDIO
#else
  #define CFG_GNB_HD_AUDIO                   FALSE
#endif

#ifdef BLDCFG_CFG_ABM_SUPPORT
  #define CFG_ABM_SUPPORT                    BLDCFG_CFG_ABM_SUPPORT
#else
  #define CFG_ABM_SUPPORT                    FALSE
#endif

#ifdef BLDCFG_CFG_DYNAMIC_REFRESH_RATE
  #define CFG_DINAMIC_REFRESH_RATE           BLDCFG_CFG_DYNAMIC_REFRESH_RATE
#else
  #define CFG_DYNAMIC_REFRESH_RATE           0
#endif

#ifdef BLDCFG_CFG_LCD_BACK_LIGHT_CONTROL
  #define CFG_LCD_BACK_LIGHT_CONTROL         BLDCFG_CFG_LCD_BACK_LIGHT_CONTROL
#else
  #define CFG_LCD_BACK_LIGHT_CONTROL         0
#endif

#ifdef BLDCFG_CFG_TEMP_PCIE_MMIO_BASE_ADDRESS
  #define CFG_TEMP_PCIE_MMIO_BASE_ADDRESS    BLDCFG_CFG_TEMP_PCIE_MMIO_BASE_ADDRESS
#else
  #define CFG_TEMP_PCIE_MMIO_BASE_ADDRESS    0xD0000000
#endif

#ifdef BLDOPT_REMOVE_EARLY_SAMPLES
  #if  BLDOPT_REMOVE_EARLY_SAMPLES == TRUE
    #undef  OPTION_EARLY_SAMPLES
    #define OPTION_EARLY_SAMPLES        FALSE
  #else
    #undef  OPTION_EARLY_SAMPLES
    #define OPTION_EARLY_SAMPLES        TRUE
  #endif
#endif

/*---------------------------------------------------------------------------
 *       Processing the options:  Third, perform the option cross checks
 *--------------------------------------------------------------------------*/
// Assure that at least one type of memory support is included
#if OPTION_UDIMMS == FALSE
  #if OPTION_RDIMMS == FALSE
    #if OPTION_SODIMMS == FALSE
      #error  BLDOPT: No DIMM support selected. Either BLDOPT_REMOVE_UDIMMS_SUPPORT or BLDOPT_REMOVE_RDIMMS_SUPPORT or BLDOPT_REMOVE_SODIMMS_SUPPORT must be FALSE.
    #endif
  #endif
#endif
// Ensure at least one dimm type is capable
#if CFG_MEMORY_RDIMM_CAPABLE == FALSE
  #if CFG_MEMORY_UDIMM_CAPABLE == FALSE
    #if CFG_MEMORY_SODIMM_CAPABLE == FALSE
      #error   BLDCFG: No dimm type is capable
    #endif
  #endif
#endif
//  Turn off multi-socket based features if only one node...
#if OPTION_MULTISOCKET == FALSE
  #undef  OPTION_PARALLEL_TRAINING
  #define OPTION_PARALLEL_TRAINING    FALSE
  #undef  OPTION_NODE_INTERLEAVE
  #define OPTION_NODE_INTERLEAVE      FALSE
#endif
// Ensure that at least one write leveling option is selected
#if OPTION_DDR3 == TRUE
  #if OPTION_HW_WRITE_LEV_TRAINING == FALSE
    #if OPTION_SW_WRITE_LEV_TRAINING == FALSE
      #error  No Write leveling option selected for DDR3
    #endif
  #endif
  #if OPTION_SW_DRAM_INIT == FALSE
    #error   Software dram init must be enabled for DDR3 dimms
  #endif
#endif
// Ensure at least one DQS receiver training option is selected
#if OPTION_HW_DQS_REC_EN_TRAINING == FALSE
  #if OPTION_NON_OPT_SW_DQS_REC_EN_TRAINING == FALSE
    #if OPTION_OPT_SW_DQS_REC_EN_TRAINING == FALSE
      #error    No DQS receiver training option has been slected
    #endif
  #endif
#endif
// Ensure at least one Rd Wr position training option has been selected
#if OPTION_NON_OPT_SW_RD_WR_POS_TRAINING == FALSE
  #if OPTION_OPT_SW_RD_WR_POS_TRAINING == FALSE
    #error    No Rd Wr position training option has been selected
  #endif
#endif
// Ensure at least one dram init option has been selected
#if OPTION_HW_DRAM_INIT == FALSE
  #if OPTION_SW_DRAM_INIT == FALSE
    #error    No Dram init option has been selected
  #endif
#endif
// Ensure the frequency limit is valid
#if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR1600_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 800)
  #if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR1333_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 667)
    #if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR1066_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 533)
      #if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR800_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 400)
        #if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR667_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 333)
          #if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR533_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 266)
            #if (CFG_MEMORY_BUS_FREQUENCY_LIMIT != DDR400_FREQUENCY) && (CFG_MEMORY_BUS_FREQUENCY_LIMIT != 200)
              #error    BLDCFG: Unsupported memory bus frequency
            #endif
          #endif
        #endif
      #endif
    #endif
  #endif
#endif
// Ensure timing mode is valid
#if CFG_TIMING_MODE_SELECT != TIMING_MODE_SPECIFIC
  #if CFG_TIMING_MODE_SELECT != TIMING_MODE_LIMITED
    #if CFG_TIMING_MODE_SELECT != TIMING_MODE_AUTO
      #error    BLDCFG: Invalid timing mode is set
    #endif
  #endif
#endif
// Ensure the scrub rate is valid
#if ((CFG_SCRUB_DRAM_RATE > 0x16) && (CFG_SCRUB_DRAM_RATE != 0xFF))
  #error    BLDCFG: Unsupported dram scrub rate set
#endif
#if CFG_SCRUB_L2_RATE > 0x16
  #error   BLDCFG: Unsupported L2 scrubber rate set
#endif
#if CFG_SCRUB_L3_RATE > 0x16
  #error   BLDCFG: unsupported L3 scrubber rate set
#endif
#if CFG_SCRUB_IC_RATE > 0x16
  #error    BLDCFG: Unsupported Instruction cache scrub rate set
#endif
#if CFG_SCRUB_DC_RATE > 0x16
  #error   BLDCFG: Unsupported Dcache scrub rate set
#endif
// Ensure Quad rank dimm type is valid
#if CFG_MEMORY_QUADRANK_TYPE != QUADRANK_UNBUFFERED
  #if CFG_MEMORY_QUADRANK_TYPE != QUADRANK_REGISTERED
    #error     BLDCFG: Invalid quad rank dimm type set
  #endif
#endif
// Ensure ECC symbol size is valid
#if CFG_ECC_SYMBOL_SIZE != ECCSYMBOLSIZE_USE_BKDG
  #if CFG_ECC_SYMBOL_SIZE != ECCSYMBOLSIZE_FORCE_X4
    #if CFG_ECC_SYMBOL_SIZE != ECCSYMBOLSIZE_FORCE_X8
      #error   BLDCFG: Invalid Ecc symbol size set
    #endif
  #endif
#endif
// Ensure power down mode is valid
#if CFG_POWER_DOWN_MODE != POWER_DOWN_BY_CHIP_SELECT
  #if CFG_POWER_DOWN_MODE != POWER_DOWN_BY_CHANNEL
    #error     BLDCFG: Invalid power down mode set
  #endif
#endif

/*****************************************************************************
 *
 * Process the option logic, setting local control variables
 *
 ****************************************************************************/
#if OPTION_ACPI_PSTATES == TRUE
  #define OPTFCN_ACPI_TABLES           CreateAcpiTables
  #define OPTFCN_GATHER_DATA           PStateGatherData
  #if OPTION_MULTISOCKET == TRUE
    #define OPTFCN_PSTATE_LEVELING     PStateLeveling
  #else
    #define OPTFCN_PSTATE_LEVELING     CommonReturnAgesaSuccess
  #endif
#else
  #define OPTFCN_ACPI_TABLES           CommonReturnAgesaSuccess
  #define OPTFCN_GATHER_DATA           CommonReturnAgesaSuccess
  #define OPTFCN_PSTATE_LEVELING       CommonReturnAgesaSuccess
#endif

#define GET_PSTATE_POWER            FALSE
#define GET_PSTATE_FREQ             FALSE
#define DISABLE_PSTATE              FALSE
#define TRANSITION_PSTATE           FALSE
#define PROC_IDD_MAX                FALSE
#define GET_TSC_RATE                FALSE
#define PSTATE_TRANSITION_LATENCY   FALSE
#define GET_PSTATE_REGISTER_INFO    FALSE
#define GET_PSTATE_MAX_STATE        FALSE
#define SET_PSTATE_LEVELING_REG     FALSE
#define GET_NB_FREQ                 FALSE
#define IS_NBCOF_INIT_NEEDED        FALSE
#define IS_NB_PSTATE_ENABLED        FALSE
#define AP_INITIAL_LAUNCH           FALSE
#define GET_AP_MAILBOX_FROM_HW      FALSE
#define SET_AP_CORE_NUMBER          FALSE
#define GET_AP_CORE_NUMBER          FALSE
#define TRANSFER_AP_CORE_NUMBER     FALSE
#define ID_POSITION_INITIAL_APICID  FALSE
#define SAVE_FEATURES               FALSE
#define WRITE_FEATURES              FALSE
#define SET_DOWN_CORE_REG           FALSE
#define SET_WARM_RESET_FLAG         FALSE
#define GET_WARM_RESET_FLAG         FALSE
#define USES_REGISTER_TABLES        FALSE
#define BASE_FAMILY_PCI             FALSE
#define MODEL_SPECIFIC_PCI          FALSE
#define BASE_FAMILY_MSR             FALSE
#define MODEL_SPECIFIC_MSR          FALSE
#define BRAND_STRING1               FALSE
#define BRAND_STRING2               FALSE
#define BASE_FAMILY_HT_PCI          FALSE
#define MODEL_SPECIFIC_HT_PCI       FALSE
#define GET_PATCHES                 FALSE
#define GET_PATCHES_EQUIVALENCE_TABLE   FALSE
#define GET_CACHE_INFO              FALSE
#define GET_SYSTEM_PM_TABLE         FALSE
#define GET_WHEA_INIT               FALSE
#define GET_PLATFORM_TYPE_SPECIFIC_INFO FALSE

#if AGESA_ENTRY_INIT_RESET == TRUE
  #undef ID_POSITION_INITIAL_APICID
  #define ID_POSITION_INITIAL_APICID  TRUE
  #undef GET_AP_MAILBOX_FROM_HW
  #define GET_AP_MAILBOX_FROM_HW      TRUE
  #undef SET_WARM_RESET_FLAG
  #define SET_WARM_RESET_FLAG         TRUE
  #undef GET_WARM_RESET_FLAG
  #define GET_WARM_RESET_FLAG         TRUE
  #undef GET_CACHE_INFO
  #define GET_CACHE_INFO              TRUE
  #undef GET_AP_CORE_NUMBER
  #define GET_AP_CORE_NUMBER          TRUE
  #undef TRANSFER_AP_CORE_NUMBER
  #define TRANSFER_AP_CORE_NUMBER     TRUE
#endif

#if AGESA_ENTRY_INIT_RECOVERY == TRUE
  #undef ID_POSITION_INITIAL_APICID
  #define ID_POSITION_INITIAL_APICID  TRUE
  #undef USES_REGISTER_TABLES
  #define USES_REGISTER_TABLES        TRUE
  #undef BASE_FAMILY_PCI
  #define BASE_FAMILY_PCI             TRUE
  #undef MODEL_SPECIFIC_PCI
  #define MODEL_SPECIFIC_PCI          TRUE
  #undef BASE_FAMILY_MSR
  #define BASE_FAMILY_MSR             TRUE
  #undef MODEL_SPECIFIC_MSR
  #define MODEL_SPECIFIC_MSR          TRUE
  #undef GET_CACHE_INFO
  #define GET_CACHE_INFO              TRUE
  #undef GET_PLATFORM_TYPE_SPECIFIC_INFO
  #define GET_PLATFORM_TYPE_SPECIFIC_INFO TRUE
  #undef IS_NB_PSTATE_ENABLED
  #define IS_NB_PSTATE_ENABLED        TRUE
#endif

#if AGESA_ENTRY_INIT_EARLY == TRUE
  #undef TRANSITION_PSTATE
  #define TRANSITION_PSTATE           TRUE
  #undef PROC_IDD_MAX
  #define PROC_IDD_MAX                TRUE
  #undef GET_TSC_RATE
  #define GET_TSC_RATE                TRUE
  #undef GET_NB_FREQ
  #define GET_NB_FREQ                 TRUE
  #undef IS_NBCOF_INIT_NEEDED
  #define IS_NBCOF_INIT_NEEDED        TRUE
  #undef AP_INITIAL_LAUNCH
  #define AP_INITIAL_LAUNCH           TRUE
  #undef GET_AP_MAILBOX_FROM_HW
  #define GET_AP_MAILBOX_FROM_HW      TRUE
  #undef SET_AP_CORE_NUMBER
  #define SET_AP_CORE_NUMBER          TRUE
  #undef GET_AP_CORE_NUMBER
  #define GET_AP_CORE_NUMBER          TRUE
  #undef TRANSFER_AP_CORE_NUMBER
  #define TRANSFER_AP_CORE_NUMBER     TRUE
  #undef ID_POSITION_INITIAL_APICID
  #define ID_POSITION_INITIAL_APICID  TRUE
  #undef SET_DOWN_CORE_REG
  #define SET_DOWN_CORE_REG           TRUE
  #undef SET_WARM_RESET_FLAG
  #define SET_WARM_RESET_FLAG         TRUE
  #undef GET_WARM_RESET_FLAG
  #define GET_WARM_RESET_FLAG         TRUE
  #undef USES_REGISTER_TABLES
  #define USES_REGISTER_TABLES        TRUE
  #undef BASE_FAMILY_PCI
  #define BASE_FAMILY_PCI             TRUE
  #undef MODEL_SPECIFIC_PCI
  #define MODEL_SPECIFIC_PCI          TRUE
  #undef BASE_FAMILY_MSR
  #define BASE_FAMILY_MSR             TRUE
  #undef MODEL_SPECIFIC_MSR
  #define MODEL_SPECIFIC_MSR          TRUE
  #undef BRAND_STRING1
  #define BRAND_STRING1               TRUE
  #undef BRAND_STRING2
  #define BRAND_STRING2               TRUE
  #undef BASE_FAMILY_HT_PCI
  #define BASE_FAMILY_HT_PCI          TRUE
  #undef MODEL_SPECIFIC_HT_PCI
  #define MODEL_SPECIFIC_HT_PCI       TRUE
  #undef GET_PATCHES
  #define GET_PATCHES                 TRUE
  #undef GET_PATCHES_EQUIVALENCE_TABLE
  #define GET_PATCHES_EQUIVALENCE_TABLE   TRUE
  #undef GET_SYSTEM_PM_TABLE
  #define GET_SYSTEM_PM_TABLE         TRUE
  #undef GET_CACHE_INFO
  #define GET_CACHE_INFO              TRUE
  #undef GET_PLATFORM_TYPE_SPECIFIC_INFO
  #define GET_PLATFORM_TYPE_SPECIFIC_INFO TRUE
  #undef IS_NB_PSTATE_ENABLED
  #define IS_NB_PSTATE_ENABLED        TRUE
#endif

#if AGESA_ENTRY_INIT_POST == TRUE
  #undef ID_POSITION_INITIAL_APICID
  #define ID_POSITION_INITIAL_APICID  TRUE
  #undef GET_PSTATE_POWER
  #define GET_PSTATE_POWER            TRUE
  #undef GET_PSTATE_FREQ
  #define GET_PSTATE_FREQ             TRUE
  #undef TRANSITION_PSTATE
  #define TRANSITION_PSTATE           TRUE
  #undef PROC_IDD_MAX
  #define PROC_IDD_MAX                TRUE
  #undef GET_AP_CORE_NUMBER
  #define GET_AP_CORE_NUMBER          TRUE
  #undef GET_PSTATE_REGISTER_INFO
  #define GET_PSTATE_REGISTER_INFO    TRUE
  #undef GET_PSTATE_MAX_STATE
  #define GET_PSTATE_MAX_STATE        TRUE
  #undef SET_PSTATE_LEVELING_REG
  #define SET_PSTATE_LEVELING_REG     TRUE
  #undef SET_WARM_RESET_FLAG
  #define SET_WARM_RESET_FLAG         TRUE
  #undef GET_WARM_RESET_FLAG
  #define GET_WARM_RESET_FLAG         TRUE
  #undef SAVE_FEATURES
  #define SAVE_FEATURES               TRUE
  #undef WRITE_FEATURES
  #define WRITE_FEATURES              TRUE
  #undef IS_NB_PSTATE_ENABLED
  #define IS_NB_PSTATE_ENABLED        TRUE
#endif

#if AGESA_ENTRY_INIT_ENV == TRUE
#endif

#if AGESA_ENTRY_INIT_MID == TRUE
#endif

#if AGESA_ENTRY_INIT_LATE == TRUE
  #undef GET_PSTATE_FREQ
  #define GET_PSTATE_FREQ             TRUE
  #undef TRANSITION_PSTATE
  #define TRANSITION_PSTATE           TRUE
  #undef PSTATE_TRANSITION_LATENCY
  #define PSTATE_TRANSITION_LATENCY   TRUE
  #undef GET_WHEA_INIT
  #define GET_WHEA_INIT               TRUE
  #undef GET_PLATFORM_TYPE_SPECIFIC_INFO
  #define GET_PLATFORM_TYPE_SPECIFIC_INFO TRUE
#endif

#if AGESA_ENTRY_INIT_S3SAVE == TRUE
#endif

#if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
#endif

#if AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE
  #undef ID_POSITION_INITIAL_APICID
  #define ID_POSITION_INITIAL_APICID  TRUE
#endif

// Initialize family mask for the PCI MMIO MSR to 0
#define FAMILY_MMIO_BASE_MASK (0ull)

/*****************************************************************************
 *
 * Include the structure definitions for the defaults table structures
 *
 ****************************************************************************/
#include  "OptionFamily12h.h"
#include  "OptionFamily12hLnInstall.h"
#include  "OptionFamily12hInstall.h"
#include  "Options.h"
#include  "OptionsHt.h"
#include  "OptionHtInstall.h"
#include  "OptionMemory.h"
#include  "PlatformMemoryConfiguration.h"
#include  "OptionMemoryInstall.h"
#include  "OptionMemoryRecovery.h"
#include  "OptionMemoryRecoveryInstall.h"
#include  "OptionDmi.h"
#include  "OptionDmiInstall.h"
#include  "OptionPstate.h"
#include  "OptionPstateInstall.h"
#include  "OptionWhea.h"
#include  "OptionWheaInstall.h"
#include  "OptionSrat.h"
#include  "OptionSratInstall.h"
#include  "OptionSlit.h"
#include  "OptionSlitInstall.h"
#include  "OptionMultiSocket.h"
#include  "OptionMultiSocketInstall.h"
#include  "OptionCpuFeaturesInstall.h"
#include  "OptionIdsInstall.h"
#include  "OptionS3ScriptInstall.h"
#include  "OptionGfxRecovery.h"
#include  "OptionGfxRecoveryInstall.h"
#if (AGESA_ENTRY_INIT_EARLY == TRUE)
  #include  "OptionLynxMicrocodeInstall.h"
#endif

// If size is 0, but base is not, break the build.
#if (CFG_PCI_MMIO_BASE != 0) && (CFG_PCI_MMIO_SIZE == 0)
  #error    BLDCFG: Invalid PCI MMIO size -- acceptable values are 1, 2, 4, 8, 16, 32, 64, 128, and 256
#endif

// If base is 0, but size is not, break the build.
#if (CFG_PCI_MMIO_BASE == 0) && (CFG_PCI_MMIO_SIZE != 0)
  #error    BLDCFG: Invalid PCI MMIO base -- must be 8MB or greater
#endif

#if (CFG_PCI_MMIO_BASE != 0) && (CFG_PCI_MMIO_SIZE != 0)
  // Both are non-zero, begin further processing.

  // Heap runs from 4MB to 8MB. Disallow any addresses below 8MB.
  #if (CFG_PCI_MMIO_BASE < 0x800000)
    #error    BLDCFG: Invalid PCI MMIO base -- must be 8MB or greater
  #endif

  // Break the build if the address is too high for the enabled families.
  #if ((CFG_PCI_MMIO_BASE & FAMILY_MMIO_BASE_MASK) != 0)
    #error       BLDCFG: Invalid PCI MMIO base address for the installed CPU families
  #endif

  // If the size parameter is not valid, break the build.
  #if (CFG_PCI_MMIO_SIZE != 1) && (CFG_PCI_MMIO_SIZE != 2) && (CFG_PCI_MMIO_SIZE != 4) && (CFG_PCI_MMIO_SIZE != 8) && (CFG_PCI_MMIO_SIZE != 16)
    #if (CFG_PCI_MMIO_SIZE != 32) && (CFG_PCI_MMIO_SIZE != 64) && (CFG_PCI_MMIO_SIZE != 128) && (CFG_PCI_MMIO_SIZE != 256)
      #error      BLDCFG: Invalid PCI MMIO size -- acceptable values are 1, 2, 4, 8, 16, 32, 64, 128, and 256
    #endif
  #endif

  #define PCI_MMIO_ALIGNMENT ((0x100000 * CFG_PCI_MMIO_SIZE) - 1)
  // If the base is not aligned according to size, break the build.
  #if ((CFG_PCI_MMIO_BASE & PCI_MMIO_ALIGNMENT) != 0)
    #error      BLDCFG: Invalid PCI MMIO base -- must be properly aligned according to MMIO size
  #endif
  #undef PCI_MMIO_ALIGNMENT
#endif


/*****************************************************************************
 *
 *  Generate the output structures (defaults tables)
 *
 ****************************************************************************/
#ifndef ADVCFG_PLATFORM_SOCKETS
  #error BLDOPT Set Family supported sockets.
#endif
#ifndef ADVCFG_PLATFORM_MODULES
  #error BLDOPT Set Family supported modules.
#endif


BUILD_OPT_CFG UserOptions = {
  {                                             // AGESA version string
    AGESA_CODE_SIGNATURE,                       //    code header Signature
    AGESA_PACKAGE_STRING,                       //    8 character ID
    AGESA_VERSION_STRING,                       //    12 character version string
    0                                           //    null string terminator
  },
  //Build Option Area
  OPTION_UDIMMS,                        //UDIMMS
  OPTION_RDIMMS,                        //RDIMMS
  OPTION_ECC,                           //ECC
  OPTION_BANK_INTERLEAVE,               //BANK_INTERLEAVE
  OPTION_DCT_INTERLEAVE,                //DCT_INTERLEAVE
  OPTION_NODE_INTERLEAVE,               //NODE_INTERLEAVE
  OPTION_PARALLEL_TRAINING,             //PARALLEL_TRAINING
  OPTION_ONLINE_SPARE,                  //ONLINE_SPARE
  OPTION_MEM_RESTORE,                   //MEM CONTEXT RESTORE
  OPTION_MULTISOCKET,                   //MULTISOCKET
  OPTION_ACPI_PSTATES,                  //ACPI_PSTATES
  OPTION_SRAT,                          //SRAT
  OPTION_SLIT,                          //SLIT
  OPTION_WHEA,                          //WHEA
  OPTION_DMI,                           //DMI
  OPTION_EARLY_SAMPLES,                 //EARLY_SAMPLES
  OPTION_ADDR_TO_CS_TRANSLATOR,         //ADDR_TO_CS_TRANSLATOR

  //Build Configuration Area
  CFG_PCI_MMIO_BASE,
  CFG_PCI_MMIO_SIZE,
  {
    CFG_VRM_CURRENT_LIMIT,              // VrmCurrentLimit
    CFG_VRM_LOW_POWER_THRESHOLD,        // VrmLowPowerThershold
    CFG_VRM_SLEW_RATE,                  // VrmSlewRate
    CFG_VRM_ADDITIONAL_DELAY,           // VrmAdditionalDelay
    CFG_VRM_HIGH_SPEED_ENABLE,          // VrmHiSpeedEnable
    CFG_VRM_INRUSH_CURRENT_LIMIT        // VrmInrushCurrentLimit
  },
  CFG_PLAT_NUM_IO_APICS,                //PlatformApicIoNumber
  CFG_MEM_INIT_PSTATE,                  //MemoryInitPstate
  CFG_C1E_MODE,                         //C1eMode
  CFG_C1E_OPDATA,                       //C1ePlatformData
  CFG_CSTATE_MODE,                      //CStateMode
  CFG_CSTATE_OPDATA,                    //CStatePlatformData
  CFG_CORE_LEVELING_MODE,               //CoreLevelingCofig
  {
    CFG_PLATFORM_CONTROL_FLOW_MODE,       // The platform's control flow mode.
    CFG_USE_HT_ASSIST,                    // CfgUseHtAssist
    CFG_USE_32_BYTE_REFRESH,              // Display Refresh uses 32 byte packets.
    CFG_USE_VARIABLE_MCT_ISOC_PRIORITY,   // The Memory controller will be set to Variable Isoc Priority.
    CFG_PLATFORM_POWER_POLICY_MODE        // The platform's power policy mode.
  },
  (CPU_HT_DEEMPHASIS_LEVEL *)CFG_PLATFORM_DEEMPHASIS_LIST,   // Deemphasis settings
  CFG_AMD_PLATFORM_TYPE,                //AmdPlatformType
  CFG_AMD_PSTATE_CAP_VALUE,             // Amd pstate ceiling enabling deck

  CFG_MEMORY_BUS_FREQUENCY_LIMIT,       // CfgMemoryBusFrequencyLimit
  CFG_MEMORY_MODE_UNGANGED,             // CfgMemoryModeUnganged
  CFG_MEMORY_QUAD_RANK_CAPABLE,         // CfgMemoryQuadRankCapable
  CFG_MEMORY_QUADRANK_TYPE,             // CfgMemoryQuadrankType
  CFG_MEMORY_RDIMM_CAPABLE,             // CfgMemoryRDimmCapable
  CFG_MEMORY_UDIMM_CAPABLE,             // CfgMemoryUDimmCapable
  CFG_MEMORY_SODIMM_CAPABLE,            // CfgMemorySodimmCapable
  CFG_LIMIT_MEMORY_TO_BELOW_1TB,        // CfgLimitMemoryToBelow1Tb
  CFG_MEMORY_ENABLE_BANK_INTERLEAVING,  // CfgMemoryEnableBankInterleaving
  CFG_MEMORY_ENABLE_NODE_INTERLEAVING,  // CfgMemoryEnableNodeInterleaving
  CFG_MEMORY_CHANNEL_INTERLEAVING,      // CfgMemoryChannelInterleaving
  CFG_MEMORY_POWER_DOWN,                // CfgMemoryPowerDown
  CFG_POWER_DOWN_MODE,                  // CfgPowerDownMode
  CFG_ONLINE_SPARE,                     // CfgOnlineSpare
  CFG_MEMORY_PARITY_ENABLE,             // CfgMemoryParityEnable
  CFG_BANK_SWIZZLE,                     // CfgBankSwizzle
  CFG_TIMING_MODE_SELECT,               // CfgTimingModeSelect
  CFG_MEMORY_CLOCK_SELECT,              // CfgMemoryClockSelect
  CFG_DQS_TRAINING_CONTROL,             // CfgDqsTrainingControl
  CFG_IGNORE_SPD_CHECKSUM,              // CfgIgnoreSpdChecksum
  CFG_USE_BURST_MODE,                   // CfgUseBurstMode
  CFG_MEMORY_ALL_CLOCKS_ON,             // CfgMemoryAllClocksOn
  CFG_ENABLE_ECC_FEATURE,               // CfgEnableEccFeature
  CFG_ECC_REDIRECTION,                  // CfgEccRedirection
  CFG_SCRUB_DRAM_RATE,                  // CfgScrubDramRate
  CFG_SCRUB_L2_RATE,                    // CfgScrubL2Rate
  CFG_SCRUB_L3_RATE,                    // CfgScrubL3Rate
  CFG_SCRUB_IC_RATE,                    // CfgScrubIcRate
  CFG_SCRUB_DC_RATE,                    // CfgScrubDcRate
  CFG_ECC_SYNC_FLOOD,                   // CfgEccSyncFlood
  CFG_ECC_SYMBOL_SIZE,                  // CfgEccSymbolSize
  CFG_HEAP_DRAM_ADDRESS,                // CfgHeapDramAddress
  CFG_1GB_ALIGN,                        // CfgNodeMem1GBAlign
  CFG_S3_LATE_RESTORE,                  // CfgS3LateRestore
  CFG_ACPI_PSTATE_PSD_INDPX,            // CfgAcpiPstateIndependent
  (AP_MTRR_SETTINGS *) CFG_AP_MTRR_SETTINGS_LIST, // CfgApMtrrSettingsList
  CFG_UMA_MODE,                         // CfgUmaMode
  CFG_UMA_SIZE,                         // CfgUmaSize
  CFG_UMA_ABOVE4G,                      // CfgUmaAbove4G
  CFG_UMA_ALIGNMENT,                    // CfgUmaAlignment
  CFG_PROCESSOR_SCOPE_IN_SB,            // CfgProcessorScopeInSb
  CFG_PROCESSOR_SCOPE_NAME0,            // CfgProcessorScopeName0
  CFG_PROCESSOR_SCOPE_NAME1,            // CfgProcessorScopeName1
  CFG_GNB_HD_AUDIO,                     // CfgGnbHdAudio
  CFG_ABM_SUPPORT,                      // CfgAbmSupport
  CFG_DYNAMIC_REFRESH_RATE,             // CfgDynamicRefreshRate
  CFG_LCD_BACK_LIGHT_CONTROL,           // CfgLcdBackLightControl
  CFG_TEMP_PCIE_MMIO_BASE_ADDRESS,      // CfgTempPcieMmioBaseAddress

  0,                                    //reserved...
};

/// @todo, hmmm options.c override? multisocket == false?  Look broken.
CONST OPTIONS_CONFIG_TOPOLOGY ROMDATA TopologyConfiguration =
{
  ADVCFG_PLATFORM_SOCKETS,
  ADVCFG_PLATFORM_MODULES
};

CONST CPU_LOGICAL_ID_FAMILY_XLAT ROMDATA CpuSupportedFamilyIdArray[] =
{
  OPT_F12_ID_TABLE
};

CONST CPU_FAMILY_ID_XLAT_TABLE ROMDATA CpuSupportedFamilyIdTable =
{
  (sizeof (CpuSupportedFamilyIdArray) / sizeof (CPU_LOGICAL_ID_FAMILY_XLAT)),
  CpuSupportedFamilyIdArray
};

CONST FUNCTION_PARAMS_INFO ROMDATA FuncParamsInfo[] =
{
  #if AGESA_ENTRY_INIT_RESET == TRUE
    { AMD_INIT_RESET,
      sizeof (AMD_RESET_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitResetConstructor,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_RESET_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_RECOVERY == TRUE
    { AMD_INIT_RECOVERY,
      sizeof (AMD_RECOVERY_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitRecoveryInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_POST_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_EARLY == TRUE
    { AMD_INIT_EARLY,
      sizeof (AMD_EARLY_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitEarlyInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_EARLY_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_ENV == TRUE
    { AMD_INIT_ENV,
      sizeof (AMD_ENV_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitEnvInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_ENV_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_LATE == TRUE
    { AMD_INIT_LATE,
      sizeof (AMD_LATE_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitLateInitializer,
      (PF_AGESA_DESTRUCTOR) AmdInitLateDestructor,
      AMD_INIT_LATE_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_MID == TRUE
    { AMD_INIT_MID,
      sizeof (AMD_MID_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitMidInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_INIT_MID_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_POST == TRUE
    { AMD_INIT_POST,
      sizeof (AMD_POST_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitPostInitializer,
      (PF_AGESA_DESTRUCTOR) AmdInitPostDestructor,
      AMD_INIT_POST_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_RESUME == TRUE
    { AMD_INIT_RESUME,
      sizeof (AMD_RESUME_PARAMS),
      (PF_AGESA_FUNCTION) AmdInitResumeInitializer,
      (PF_AGESA_DESTRUCTOR) AmdInitResumeDestructor,
      AMD_INIT_RESUME_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
    { AMD_S3LATE_RESTORE,
      sizeof (AMD_S3LATE_PARAMS),
      (PF_AGESA_FUNCTION) AmdS3LateRestoreInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_S3_LATE_RESTORE_HANDLE
    },
  #endif

  #if AGESA_ENTRY_INIT_S3SAVE == TRUE
    { AMD_S3_SAVE,
      sizeof (AMD_S3SAVE_PARAMS),
      (PF_AGESA_FUNCTION) AmdS3SaveInitializer,
      (PF_AGESA_DESTRUCTOR) AmdS3SaveDestructor,
      AMD_S3_SAVE_HANDLE
    },
  #endif

  #if AGESA_ENTRY_LATE_RUN_AP_TASK == TRUE
    { AMD_LATE_RUN_AP_TASK,
      sizeof (AP_EXE_PARAMS),
      (PF_AGESA_FUNCTION) AmdLateRunApTaskInitializer,
      (PF_AGESA_DESTRUCTOR) CommonReturnAgesaSuccess,
      AMD_LATE_RUN_AP_TASK_HANDLE
    },
  #endif
  { 0, NULL }
};

CONST UINTN InitializerCount = ((sizeof (FuncParamsInfo)) / (sizeof (FuncParamsInfo[0])));

CONST DISPATCH_TABLE ROMDATA DispatchTable[] =
{
  { AMD_CREATE_STRUCT, (IMAGE_ENTRY)AmdCreateStruct },
  { AMD_RELEASE_STRUCT, (IMAGE_ENTRY)AmdReleaseStruct },

  #if AGESA_ENTRY_INIT_RESET == TRUE
    { AMD_INIT_RESET, (IMAGE_ENTRY)AmdInitReset },
  #endif

  #if AGESA_ENTRY_INIT_RECOVERY == TRUE
    { AMD_INIT_RECOVERY, (IMAGE_ENTRY)AmdInitRecovery },
  #endif

  #if AGESA_ENTRY_INIT_EARLY == TRUE
    { AMD_INIT_EARLY, (IMAGE_ENTRY)AmdInitEarly },
  #endif

  #if AGESA_ENTRY_INIT_POST == TRUE
    { AMD_INIT_POST, (IMAGE_ENTRY)AmdInitPost },
  #endif

  #if AGESA_ENTRY_INIT_ENV == TRUE
    { AMD_INIT_ENV, (IMAGE_ENTRY)AmdInitEnv },
  #endif

  #if AGESA_ENTRY_INIT_MID == TRUE
    { AMD_INIT_MID, (IMAGE_ENTRY)AmdInitMid },
  #endif

  #if AGESA_ENTRY_INIT_LATE == TRUE
    { AMD_INIT_LATE, (IMAGE_ENTRY)AmdInitLate },
  #endif

  #if AGESA_ENTRY_INIT_S3SAVE == TRUE
    { AMD_S3_SAVE, (IMAGE_ENTRY)AmdS3Save },
  #endif

  #if AGESA_ENTRY_INIT_RESUME == TRUE
    { AMD_INIT_RESUME, (IMAGE_ENTRY)AmdInitResume },
  #endif

  #if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
    { AMD_S3LATE_RESTORE, (IMAGE_ENTRY)AmdS3LateRestore },
  #endif

  #if AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE
    { AMD_GET_APIC_ID, (IMAGE_ENTRY)AmdGetApicId },
    { AMD_GET_PCI_ADDRESS, (IMAGE_ENTRY)AmdGetPciAddress },
    { AMD_IDENTIFY_CORE, (IMAGE_ENTRY)AmdIdentifyCore },
    { AMD_READ_EVENT_LOG, (IMAGE_ENTRY)AmdReadEventLog },
    { AMD_IDENTIFY_DIMMS, (IMAGE_ENTRY)AmdIdentifyDimm },
  #endif

  #if AGESA_ENTRY_LATE_RUN_AP_TASK == TRUE
    { AMD_LATE_RUN_AP_TASK, (IMAGE_ENTRY)AmdLateRunApTask },
  #endif
  { 0, NULL }
};

CONST DISPATCH_TABLE ROMDATA ApDispatchTable[] =
{
  IDS_LATE_RUN_AP_TASK
  // Get DMI info

  // Probe filter enable

  { 0, NULL }
};

CONST CPU_BRAND_TABLE ROMDATA *F12BrandIdString1Tables[] =
{
  NULL
};

CONST CPU_BRAND_TABLE ROMDATA *F12BrandIdString2Tables[] =
{
  NULL
};

CONST UINT8 F12BrandIdString1TableCount = (sizeof (F12BrandIdString1Tables) / sizeof (F12BrandIdString1Tables[0]));
CONST UINT8 F12BrandIdString2TableCount = (sizeof (F12BrandIdString2Tables) / sizeof (F12BrandIdString2Tables[0]));
