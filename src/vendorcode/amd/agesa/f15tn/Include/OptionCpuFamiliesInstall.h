/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of all appropriate CPU family specific support.
 *
 * This file generates the defaults tables for all family specific
 * combinations.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Core
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

/*  Default all CPU Specific Service members to off.  They
    will be enabled as needed by cross referencing families
    with entry points in the family / model install files.  */
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
#define GET_NB_IDD_MAX              FALSE
#define IS_NBCOF_INIT_NEEDED        FALSE
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
#define BASE_FAMILY_WORKAROUNDS     FALSE
#define GET_PATCHES                 FALSE
#define GET_PATCHES_EQUIVALENCE_TABLE FALSE
#define GET_CACHE_INFO              FALSE
#define GET_SYSTEM_PM_TABLE         FALSE
#define GET_WHEA_INIT               FALSE
#define GET_CFOH_REG                FALSE
#define GET_PLATFORM_TYPE_SPECIFIC_INFO FALSE
#define IS_NB_PSTATE_ENABLED        FALSE

/*
 * Pull in family specific services based on entry point
 */
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
  #undef GET_PATCHES
  #define GET_PATCHES                 TRUE
  #undef GET_PATCHES_EQUIVALENCE_TABLE
  #define GET_PATCHES_EQUIVALENCE_TABLE TRUE
#endif

#if AGESA_ENTRY_INIT_EARLY == TRUE
  #undef TRANSITION_PSTATE
  #define TRANSITION_PSTATE           TRUE
  #undef DISABLE_PSTATE
  #define DISABLE_PSTATE              TRUE
  #undef PROC_IDD_MAX
  #define PROC_IDD_MAX                TRUE
  #undef GET_TSC_RATE
  #define GET_TSC_RATE                TRUE
  #undef GET_NB_FREQ
  #define GET_NB_FREQ                 TRUE
  #undef GET_NB_IDD_MAX
  #define GET_NB_IDD_MAX              TRUE
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
  #undef BASE_FAMILY_WORKAROUNDS
  #define BASE_FAMILY_WORKAROUNDS     TRUE
  #undef GET_PATCHES
  #define GET_PATCHES                 TRUE
  #undef GET_PATCHES_EQUIVALENCE_TABLE
  #define GET_PATCHES_EQUIVALENCE_TABLE TRUE
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
  #undef GET_CFOH_REG
  #define GET_CFOH_REG                TRUE
  #undef IS_NB_PSTATE_ENABLED
  #define IS_NB_PSTATE_ENABLED        TRUE
#endif

#if AGESA_ENTRY_INIT_ENV == TRUE
#endif

#if AGESA_ENTRY_INIT_MID == TRUE
#endif

#if AGESA_ENTRY_INIT_LATE == TRUE
  #undef GET_AP_CORE_NUMBER
  #define GET_AP_CORE_NUMBER          TRUE
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
  #undef GET_TSC_RATE
  #define GET_TSC_RATE                TRUE
  #undef BRAND_STRING1
  #define BRAND_STRING1               TRUE
  #undef BRAND_STRING2
  #define BRAND_STRING2               TRUE
#endif

#if AGESA_ENTRY_INIT_S3SAVE == TRUE
#endif

#if AGESA_ENTRY_INIT_RESUME == TRUE
  #undef GET_CFOH_REG
  #define GET_CFOH_REG                TRUE
#endif

#if AGESA_ENTRY_INIT_LATE_RESTORE == TRUE
#endif

#if AGESA_ENTRY_INIT_GENERAL_SERVICES == TRUE
  #undef ID_POSITION_INITIAL_APICID
  #define ID_POSITION_INITIAL_APICID  TRUE
#endif

/*
 * Initialize PCI MMIO mask to 0
 */
#define FAMILY_MMIO_BASE_MASK (0ull)


/*
 * Initialize all families to disabled
 */
#define OPT_F10_TABLE
#define OPT_F12_TABLE
#define OPT_F14_TABLE
#define OPT_F15_TABLE

#define OPT_F10_ID_TABLE
#define OPT_F12_ID_TABLE
#define OPT_F14_ID_TABLE
#define OPT_F15_ID_TABLE


/*
 * Install family specific support
 */
#if (OPTION_FAMILY10H == TRUE)
  #include  "OptionFamily10hInstall.h"
#endif

#if (OPTION_FAMILY12H == TRUE)
  #include  "OptionFamily12hInstall.h"
#endif

#if (OPTION_FAMILY14H == TRUE)
  #include  "OptionFamily14hInstall.h"
#endif

#if (OPTION_FAMILY15H_OR == TRUE) || (OPTION_FAMILY15H_TN == TRUE)
  #include  "OptionFamily15hInstall.h"
#endif

/*
 * Process PCI MMIO mask
 */

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

  #define PCI_MMIO_ALIGNMENT ((0x100000ul * CFG_PCI_MMIO_SIZE) - 1)
  // If the base is not aligned according to size, break the build.
  #if ((CFG_PCI_MMIO_BASE & PCI_MMIO_ALIGNMENT) != 0)
    #error      BLDCFG: Invalid PCI MMIO base -- must be properly aligned according to MMIO size
  #endif
  #undef PCI_MMIO_ALIGNMENT
#endif

/*
 * Process sockets / modules
 */
#ifndef ADVCFG_PLATFORM_SOCKETS
  #error BLDOPT Set Family supported sockets.
#endif
#ifndef ADVCFG_PLATFORM_MODULES
  #error BLDOPT Set Family supported modules.
#endif

CONST OPTIONS_CONFIG_TOPOLOGY ROMDATA TopologyConfiguration =
{
  ADVCFG_PLATFORM_SOCKETS,
  ADVCFG_PLATFORM_MODULES
};

/*
 * Instantiate global data needed for processor identification
 */
CONST CPU_SPECIFIC_SERVICES_XLAT ROMDATA CpuSupportedFamiliesArray[] =
{
  OPT_F10_TABLE
  OPT_F12_TABLE
  OPT_F14_TABLE
  OPT_F15_TABLE
  {0, NULL}
};

CONST CPU_FAMILY_SUPPORT_TABLE ROMDATA CpuSupportedFamiliesTable =
{
  (sizeof (CpuSupportedFamiliesArray) / sizeof (CPU_SPECIFIC_SERVICES_XLAT)),
  &CpuSupportedFamiliesArray[0]
};


CONST CPU_LOGICAL_ID_FAMILY_XLAT ROMDATA CpuSupportedFamilyIdArray[] =
{
  OPT_F10_ID_TABLE
  OPT_F12_ID_TABLE
  OPT_F14_ID_TABLE
  OPT_F15_ID_TABLE

};

CONST CPU_FAMILY_ID_XLAT_TABLE ROMDATA CpuSupportedFamilyIdTable =
{
  (sizeof (CpuSupportedFamilyIdArray) / sizeof (CPU_LOGICAL_ID_FAMILY_XLAT)),
  CpuSupportedFamilyIdArray
};
